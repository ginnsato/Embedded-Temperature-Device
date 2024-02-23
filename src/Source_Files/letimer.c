/**
 * @file letimer.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  03/14/2023
 *
 * @brief
 *  Setup the LETIMER
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "letimer.h"

//***********************************************************************************
// private variables
//***********************************************************************************

static uint32_t scheduled_comp0_cb;
static uint32_t scheduled_comp1_cb;
static uint32_t scheduled_uf_cb;

//***********************************************************************************
// functions
//***********************************************************************************


/***************************************************************************//**
 * @brief
 *  Setup the LETIMER for our application
 *
 * @details
 *  Initialize and configure the LETIMER for our application specific purposes
 *  using predefine variables passed in as paramters to the function. We also enable
 *  different interrupts within the LETIMER
 *
 * @param [in] LETIMER_TypeDef *letimer
 *  This is a pointer to the LETIMER Peripheral Register to configure
 *
 * @param [in] APP_LETIMER_PWM_TypeDef *app_letimer_struct
 *  The application specific struct used to configure the LETIMER Register
 *  as well as initialization of the LETIMER
 *
 ******************************************************************************/
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct){
  LETIMER_Init_TypeDef letimer_pwm_values;

  CMU_ClockEnable(cmuClock_LETIMER0, true);

  letimer_start(letimer, false);    // IS THIS THE RIGHT SPOT FOR THIS

  while(letimer->SYNCBUSY);

  letimer->IEN |= LETIMER_TEST_BITMASK;                                         // write to some register
  EFM_ASSERT((letimer->IEN & LETIMER_TEST_BITMASK) == LETIMER_TEST_BITMASK);    // assert that write worked correctly
  letimer->IEN &= ~LETIMER_TEST_BITMASK;                                        // reset that register

  while(letimer->SYNCBUSY);

  letimer_pwm_values.bufTop = false;
  letimer_pwm_values.comp0Top = true;
  letimer_pwm_values.debugRun = app_letimer_struct->debugRun;
  letimer_pwm_values.enable = app_letimer_struct->enable;
  letimer_pwm_values.out0Pol = DISABLE;
  letimer_pwm_values.out1Pol = ENABLE;
  letimer_pwm_values.repMode = letimerRepeatFree;
  letimer_pwm_values.ufoa0 = letimerUFOAPwm;
  letimer_pwm_values.ufoa1 = letimerUFOAPwm;


  LETIMER_Init(letimer, &letimer_pwm_values);       // Initialize LETIMER

  // Set Period and Active Period
  letimer->COMP0 = app_letimer_struct->period * LETIMER_HZ;
  letimer->COMP1 = app_letimer_struct->active_period * LETIMER_HZ;

  // Set Repeat Value (should be anything other than 1 since we are in repeat free mode)
  letimer->REP0 = REPEAT_COUNT;
  letimer->REP1 = REPEAT_COUNT;

  // Set the route bit fields within ROUTELOC0 register
  letimer->ROUTELOC0 = app_letimer_struct->out_pin_route0;
  letimer->ROUTELOC0 |= (app_letimer_struct->out_pin_route1) << ROUTE_1_BIT_SHIFT;

  // Enable the route outputs
  letimer->ROUTEPEN = app_letimer_struct->out_pin_0_en;
  letimer->ROUTEPEN |= app_letimer_struct->out_pin_1_en << ROUTE_1_EN_BIT_SHIFT;

  // CLEAR ALL IF (best practice)
  letimer->IFC  = LETIMER_CLEAR_IF;

  // ENABLE INTERRUPTS HERE
  letimer->IEN |= app_letimer_struct->comp0_irq_enable;
  letimer->IEN |= app_letimer_struct->comp1_irq_enable << COMP_1_IRQ_BIT_SHIFT;
  letimer->IEN |= app_letimer_struct->uf_irq_enable << UF_IRQ_BIT_SHIFT;

  // LETIMER0 Interrupt is enabled within NVIC in the app_peripheral_open() function

  // set private static variable for scheduling call backs
  scheduled_comp0_cb = app_letimer_struct->comp0_cb;
  scheduled_comp1_cb = app_letimer_struct->comp1_cb;
  scheduled_uf_cb = app_letimer_struct->uf_cb;

  if(letimer->STATUS & LETIMER_STATUS_RUNNING){
      sleep_block_mode(LETIMER_EM);
  }

  // WHERE TO CALL letimer_start(letimer, false)
  // WHERE TO PUT EFM_ASSERT(letimer->STATUS & LETIMER_STATUS_RUNNING)
}


/***************************************************************************//**
 * @brief
 *  Enable the timer for the given LETIMER
 *
 * @details
 *  Enables or disables the timer for the letimer passed in as argument
 *
 * @param [in] letimer
 *  Pointer to the LETIMER peripheral register
 *
 * @param [in] enable
 *  boolean used to enable the LETIMER
 *
 ******************************************************************************/
void letimer_start(LETIMER_TypeDef *letimer, bool enable){
  // if not running and enabled
  if(!(letimer->STATUS & LETIMER_STATUS_RUNNING) && enable){
      sleep_block_mode(LETIMER_EM);
      LETIMER_Enable(letimer, enable);
      while(letimer->SYNCBUSY);
  }

  // if running and not enabled
  if((letimer->STATUS & LETIMER_STATUS_RUNNING) && !enable){
      sleep_unblock_mode(LETIMER_EM);
      LETIMER_Enable(letimer, enable);
      while(letimer->SYNCBUSY);
  }
}

/***************************************************************************//**
 * @brief
 *  IRQ Handler for the LETIMER0
 *
 * @details
 *  Generic handler for interrupts that occur within the LETIMER0. For our
 *  current implementation we only handle COMP0, COMP1, and UF
 *
 ******************************************************************************/
void LETIMER0_IRQHandler(void){
  // read interrupt source
  uint32_t flag = (LETIMER0->IF) & (LETIMER0->IEN);

  if((flag & _LETIMER_IF_COMP0_MASK) == LETIMER_IF_COMP0){
      LETIMER0->IFC = LETIMER_IFC_COMP0;
      EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP0));
      add_scheduled_event(scheduled_comp0_cb);
  }

  if((flag & _LETIMER_IF_COMP1_MASK) == LETIMER_IF_COMP1){
      LETIMER0->IFC = LETIMER_IFC_COMP1;
      EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP1));
      add_scheduled_event(scheduled_comp1_cb);
  }

  if((flag & _LETIMER_IF_UF_MASK) == LETIMER_IF_UF){
      LETIMER0->IFC = LETIMER_IFC_UF;
      EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_UF));
      add_scheduled_event(scheduled_uf_cb);
  }
}







