/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "main.h"

int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  CMU_HFRCOBandSet(MCU_HFXO_FREQ);						// Sets main CPU oscillator frequency
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFRCO and disable HFXO */
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

  /* Call application program to open / initialize all required peripheral */
  app_peripheral_setup();

  while(1){

      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_CRITICAL();
      if(!get_scheduled_events()){
          enter_sleep();
      }
      CORE_EXIT_CRITICAL();

      if(get_scheduled_events() & GPIO_ODD_IRQ_CB){
          remove_scheduled_event(GPIO_ODD_IRQ_CB);
          scheduled_gpio_odd_irq_cb();
      }
      if(get_scheduled_events() & GPIO_EVEN_IRQ_CB){
          remove_scheduled_event(GPIO_EVEN_IRQ_CB);
          scheduled_gpio_even_irq_cb();
      }
      if(get_scheduled_events() & APP_BTN0_CB){
          remove_scheduled_event(APP_BTN0_CB);
          scheduled_record_button_press_cb(APP_BTN0_CB);
      }
      if(get_scheduled_events() & APP_BTN1_CB){
          remove_scheduled_event(APP_BTN1_CB);
          scheduled_record_button_press_cb(APP_BTN1_CB);
      }
      if(get_scheduled_events() & APP_CHK_INPUT_CB){
          remove_scheduled_event(APP_CHK_INPUT_CB);
          app_state_machine();
      }
      if(get_scheduled_events() & LETIMER_COMP0_IRQ_CB){
          remove_scheduled_event(LETIMER_COMP0_IRQ_CB);
          scheduled_letimer_comp0_cb();
      }
      if(get_scheduled_events() & LETIMER_COMP1_IRQ_CB){
          remove_scheduled_event(LETIMER_COMP1_IRQ_CB);
          scheduled_letimer_comp1_cb();
      }
      if(get_scheduled_events() & LETIMER_UF_IRQ_CB){
          remove_scheduled_event(LETIMER_UF_IRQ_CB);
          scheduled_letimer_uf_cb();
      }
      if(get_scheduled_events() & SI7021_TEMP_READ_CB){
          remove_scheduled_event(SI7021_TEMP_READ_CB);
          scheduled_read_i2c_cb();
      }
  }
}

