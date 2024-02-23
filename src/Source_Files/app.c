/**
 * @file app.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  02/23/2023
 *
 * @brief
 *  Setup the game that reads inputs. Define functions to read button inputs.
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#include "app.h"

//***********************************************************************************
// global variables
//***********************************************************************************

typedef enum {
  ONE,
  TWO,
  THREE
} tCountState;

typedef struct {
  //Current state of the state machine
  tCountState state;
  //Keeps track of the # of times a btn has been pressed in the current state
  uint32_t num_press;
  //Encoded event handler to be serviced
  uint32_t cb;
  //For the current state, records whether btn0 has been pressed
  bool btn0;
  //For the current state, records whether btn1 has been pressed
  bool btn1;
}tAPP_STATE_MACHINE;


// initialize the ASM as a global variable
tAPP_STATE_MACHINE ASM;
tAPP_STATE_MACHINE *p = &ASM;

//***********************************************************************************
// function
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  Configure the GPIO for our application
 *
 * @details
 *  Define a GPIO struct needed for our application, and call a function that sets
 *  the corresponding values to setup interrupts and button pin modes.
 *  In this game, we enable button 0 and button 1 as well as their interrupts.
 *
 ******************************************************************************/
void app_gpio_open(void){
  GAME_GPIO_TypeDef gpio;
  gpio.btn0_port = BUT0_PORT;
  gpio.btn0_pin  = BUT0_PIN;
  gpio.btn0_en   = ENABLE;
  gpio.btn0_irq_enable   = ENABLE;
  gpio.btn1_port = BUT1_PORT;
  gpio.btn1_pin  = BUT1_PIN;
  gpio.btn1_en   = ENABLE;
  gpio.btn1_irq_enable   = ENABLE;
  gpio_open(&gpio);
}


/***************************************************************************//**
 * @brief
 *  Called every time a new state is entered
 *
 * @details
 *  Set the values of our state machine struct so they correspond to the
 *  beginning of a new state. This means resetting the button 0 and button 1
 *  values and number of presses
 *
 ******************************************************************************/
void app_new_state(void){
  p->num_press = NONE;
  p->btn0 = NONE;
  p->btn1 = NONE;
}


/***************************************************************************//**
 * @brief
 *  Set the LEDs that correspond to the state
 *
 * @details
 *  Using switch statements that read the current state, we update the LEDs
 *  so that the state number is represented in binary by the LEDs
 *
 ******************************************************************************/
void app_set_LEDS(void){
  switch(p->state){
    case ONE:
      GPIO_PinOutSet(LED0_PORT, LED0_PIN);              // LED0 ON
      GPIO_PinOutClear(LED1_PORT, LED1_PIN);            // LED1 OFF
      break;
    case TWO:
      GPIO_PinOutClear(LED0_PORT, LED0_PIN);            // LED0 OFF
      GPIO_PinOutSet(LED1_PORT, LED1_PIN);              // LED1 ON
      break;
    case THREE:
      GPIO_PinOutSet(LED0_PORT, LED0_PIN);              // LED0 ON
      GPIO_PinOutSet(LED1_PORT, LED1_PIN);              // LED1 ON
      break;
    default:
      break;
  }
}


/***************************************************************************//**
 * @brief
 *  Initialize the state machine
 *
 * @details
 *  Set the state machine's state and LEDs that correspond to the inital state
 *  in this case this is state one
 *
 ******************************************************************************/
void app_init_state_machine(void){
  p->state = ONE;
  app_set_LEDS();
  app_new_state();
}


/***************************************************************************//**
 * @brief
 *  Handle the functionality and logic of the state machine
 *
 * @details
 *  Use the current state of the state machine, the presses of the buttons,
 *  and the number of presses to change states
 *
 ******************************************************************************/
void app_state_machine(void){
  switch(p->state){
    case(ONE):
        if(p->btn0){
            p->state = TWO;
            app_set_LEDS();
            app_new_state();
        }
        break;

    case(TWO):
        if(p->btn0){
            p->state = ONE;
            app_set_LEDS();
            app_new_state();
        }
        else if(p->btn1){
            p->state = THREE;
            app_set_LEDS();
            app_new_state();
        }
        break;

    case(THREE):
        if(p->num_press == TWICE)
            app_init_state_machine();
        break;

    default:
        break;

  }
}


/***************************************************************************//**
 * @brief
 *  Configure LETIMER
 *
 * @details
 *  Configure settings for the application specific LETIMER. Then call letimer_pwn_open()
 *  to apply these settings.
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
  APP_LETIMER_PWM_TypeDef a;
  a.enable = DISABLE;
  a.debugRun = ENABLE;
  a.out_pin_route0 = out0_route;
  a.out_pin_route1 = out1_route;
  a.out_pin_0_en = !LETIMER_ROUTEPEN_OUT0PEN;
  a.out_pin_1_en = !LETIMER_ROUTEPEN_OUT1PEN;      // this variable should be a uint32_t
  a.period = period;
  a.active_period = act_period;
  a.comp0_irq_enable = ENABLE;
  a.comp0_cb = LETIMER_COMP0_IRQ_CB;
  a.comp1_irq_enable = ENABLE;
  a.comp1_cb = LETIMER_COMP1_IRQ_CB;
  a.uf_irq_enable = ENABLE;
  a.uf_cb = LETIMER_UF_IRQ_CB;
  letimer_pwm_open(LETIMER0, &a);
}


/***************************************************************************//**
 * @brief
 *  Setup the peripheral interrupts and scheduler
 *
 * @details
 *  Using the HAL, enable the EVEN and ODD interrupts from the GPIO within
 *  the NVIC. Call scheduler_open() function
 *
 ******************************************************************************/
void app_peripheral_open(void){
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(I2C0_IRQn);
  scheduler_open();
}


/***************************************************************************//**
 * @brief
 *  Setup the peripherals
 *
 * @details
 *  Setup multiple different peripherals that we are going to use in our
 *  application including the CMU, letimer, and GPIO. We also initialize the
 *  application state machine, start the timer, instantiate our sleep modes,
 *  and open our si7021 sensor for compatibility with the i2c.
 *
 ******************************************************************************/
void app_peripheral_setup(void){
	cmu_open();
	app_peripheral_open();
	sleep_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, OUT0_ROUTE, OUT1_ROUTE);
	app_gpio_open();
	app_init_state_machine();
	letimer_start(LETIMER0, ENABLE);
  si7021_open();
}


/***************************************************************************//**
 * @brief
 *  Using scheduler to handle a GPIO even IRQ
 *
 * @details
 *  Schedule a button 0 call back
 *
 ******************************************************************************/
void scheduled_gpio_even_irq_cb(void){
  add_scheduled_event(APP_BTN0_CB);               // add event for btn0 press
}


/***************************************************************************//**
 * @brief
 *  Using scheduler to handle a GPIO odd IRQ
 *
 * @details
 *  Schedule a button 1 call back
 *
 ******************************************************************************/
void scheduled_gpio_odd_irq_cb(void){
  add_scheduled_event(APP_BTN1_CB);               // add event for btn1 press
}


/***************************************************************************//**
 * @brief
 *  Using scheduler to determine what button was
 *
 * @details
 *  Update state machine struct and schedule a state machine check
 *
 ******************************************************************************/
void scheduled_record_button_press_cb(uint32_t button){
  if(button == APP_BTN0_CB){
      //p->btn0 = PRESS;
      uint32_t current_block_em = current_block_energy_mode();
      sleep_unblock_mode(current_block_em);
      if(current_block_em > EM0){
          sleep_block_mode(current_block_em - EM1);
      }
      else{
          sleep_block_mode(EM4);
      }
  }
  if(button == APP_BTN1_CB){
      //p->btn1 = PRESS;
      uint32_t current_block_em = current_block_energy_mode();
      sleep_unblock_mode(current_block_em);
      if(current_block_em < EM4){
          sleep_block_mode(current_block_em + EM1);
      }
      else{
          sleep_block_mode(EM0);
      }
  }
  //p->num_press++;
  //add_scheduled_event(APP_CHK_INPUT_CB);
}


/***************************************************************************//**
 * @brief
 *  Call back function for comp0 interrupt
 *
 * @details
 *  Handles a scheduled event call back for the comp0 interrupt within letimer
 *
 ******************************************************************************/
void scheduled_letimer_comp0_cb(void){

}


/***************************************************************************//**
 * @brief
 *  Call back function for comp1 interrupt
 *
 * @details
 *  Handles a scheduled event call back for the comp1 interrupt within letimer
 *
 ******************************************************************************/
void scheduled_letimer_comp1_cb(void){

}


/***************************************************************************//**
 * @brief
 *  Call back function for uf interrupt
 *
 * @details
 *  Handles a scheduled event call back for the uf interrupt within the letimer
 *
 ******************************************************************************/
void scheduled_letimer_uf_cb(void){
  si7021_read(SI7021_TEMP_READ_CB);
}


/***************************************************************************//**
 * @brief
 *  Call back function for i2c read
 *
 * @details
 *  Converts the raw data into a standard temperature value using the Si7021
 *  datasheet for the conversion. Turns on LED1 if the value is greater than
 *  or equal to the ambient temp, otherwise it turns it off.
 *
 ******************************************************************************/
void scheduled_read_i2c_cb(void){
  uint32_t raw_data = si7021_get_raw_data();

  float temp = si7021_calc_temp(raw_data);

  if(temp >= AMBIENT_TEMP){
      GPIO_PinOutSet(LED1_PORT, LED1_PIN);              // LED1 ON
      GPIO_PinOutClear(LED0_PORT, LED0_PIN);            // LED0 OFF
  }
  else{
      GPIO_PinOutClear(LED1_PORT, LED1_PIN);            // LED1 OFF
      GPIO_PinOutClear(LED0_PORT, LED0_PIN);            // LED0 OFF
  }
}






