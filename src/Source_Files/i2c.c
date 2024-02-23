/**
 * @file i2c.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/11/2023
 *
 * @brief
 *  Configuring a general i2c driver
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#include "i2c.h"

//***********************************************************************************
// Private variables
//***********************************************************************************

static I2C_StateMachine_TypeDef sm;

//***********************************************************************************
// Functions
//***********************************************************************************

/***************************************************************************//**
* @brief
*  Interrupt handling for ACK
*
* @details
*  Routine for the ACK interrupt. Performs the appropriate functions based on
*  the current state and interrupt for the i2c state machine
*
*  @param [in] i2c_sm
*  Uses the struct to handle interrupt based on input state and parameters
*
******************************************************************************/
static void i2c_ack(I2C_StateMachine_TypeDef *i2c_sm){
  switch(i2c_sm->current_state){
    case I2C_INIT:
      i2c_sm->current_state = I2C_RX_SEND_CMD;            // can move on to next state

      i2c_sm->i2c->TXDATA = i2c_sm->device_cmd;           // send measure temp CMD (0xF3)
      break;

    case I2C_RX_SEND_CMD:
      i2c_sm->current_state = I2C_RX_SEND_ADDR;           // can move on to next state

      i2c_sm->i2c_cmd = READ;                             // next time we will read

      i2c_sm->i2c->CMD = I2C_CMD_START;                                                           // send repeated start CMD
      i2c_sm->i2c->TXDATA = (i2c_sm->i2c_device_address << I2C_ADDRESS_SHIFT) | i2c_sm->i2c_cmd;  // read from address
      break;

    case I2C_RX_SEND_ADDR:
      i2c_sm->current_state = I2C_RX_RECEIVE_DATA;      // can move on to next state

      // nothing else, RX data interrupt should occur next
      break;

    case I2C_RX_RECEIVE_DATA:
      EFM_ASSERT(false);
      break;

    case I2C_CLOSE:
      EFM_ASSERT(false);
      break;

    default:
      EFM_ASSERT(false);
      break;
  }
}


/***************************************************************************//**
* @brief
*  Interrupt handling for NACK
*
* @details
*  Routine for the NACK interrupt. Performs the appropriate functions based on
*  the current state and interrupt for the i2c state machine
*
*  @param [in] i2c_sm
*  Uses the struct to handle interrupt based on input state and parameters
*
******************************************************************************/
static void i2c_nack(I2C_StateMachine_TypeDef *i2c_sm){
  switch(i2c_sm->current_state){
    case I2C_INIT:
      i2c_sm->i2c->CMD = I2C_CMD_START;                                                           // send start
      i2c_sm->i2c->TXDATA = (i2c_sm->i2c_device_address << I2C_ADDRESS_SHIFT) | i2c_sm->i2c_cmd;  // write to address
      break;

    case I2C_RX_SEND_CMD:
      i2c_sm->i2c->TXDATA = i2c_sm->device_cmd;                                                   // send measure temp CMD (0xF3)
      break;

    case I2C_RX_SEND_ADDR:
      i2c_sm->i2c->CMD = I2C_CMD_START;                                                           // send repeated start CMD
      i2c_sm->i2c->TXDATA = (i2c_sm->i2c_device_address << I2C_ADDRESS_SHIFT) | i2c_sm->i2c_cmd;  // read from address
      break;

    case I2C_RX_RECEIVE_DATA:
      EFM_ASSERT(false);
      break;

    case I2C_CLOSE:
      EFM_ASSERT(false);
      break;

    default:
      EFM_ASSERT(false);
      break;
  }
}


/***************************************************************************//**
* @brief
*  Interrupt handling for RXDATAV
*
* @details
*  Routine for the RXDATAV interrupt. Performs the appropriate functions based on
*  the current state and interrupt for the i2c state machine
*
*  @param [in] i2c_sm
*  Uses the struct to handle interrupt based on input state and parameters
*
******************************************************************************/
static void i2c_rxdatav(I2C_StateMachine_TypeDef *i2c_sm){
  // should be in this state, otherwise some other issue
  EFM_ASSERT(i2c_sm->current_state == I2C_RX_RECEIVE_DATA);

  i2c_sm->i2c_curr_bytes++;                           // we have received one byte

  uint8_t raw_data = i2c_sm->i2c->RXDATA;

  i2c_sm->i2c->IFC = I2C_IF_RXDATAV;                   // wait till we read data before clearing interrupt flag
  EFM_ASSERT(!(i2c_sm->i2c->IF & I2C_IF_RXDATAV));     // ensure flag was cleared properly

  if(i2c_sm->i2c_curr_bytes == i2c_sm->i2c_num_bytes){
      // read from RXDATA and put into raw_sensor_data in order of MSB to LSB
      *(i2c_sm->i2c_data) = (*(i2c_sm->i2c_data) << (BUFFER_OFFSET*(i2c_sm->i2c_curr_bytes-1))) | raw_data;
      i2c_sm->i2c->CMD = I2C_CMD_NACK;                // send NACK
      i2c_sm->i2c->CMD = I2C_CMD_STOP;                // send STOP
  }
  else{
      *(i2c_sm->i2c_data) = (*(i2c_sm->i2c_data) << (BUFFER_OFFSET*(i2c_sm->i2c_curr_bytes-1))) | raw_data;
      i2c_sm->i2c->CMD = I2C_CMD_ACK;                 // send ACK
  }

}


/***************************************************************************//**
* @brief
*  Interrupt handling for MSTOP
*
* @details
*  Routine for the MSTOP interrupt. Performs the appropriate functions based on
*  the current state and interrupt for the i2c state machine
*
*  @param [in] i2c_sm
*  Uses the struct to handle interrupt based on input state and parameters
*
******************************************************************************/
static void i2c_mstop(I2C_StateMachine_TypeDef *i2c_sm){

  // should only receive MSTOP when in this state
  EFM_ASSERT(i2c_sm->current_state == I2C_RX_RECEIVE_DATA);

  i2c_sm->i2c_busy = DISABLE;                  // done using i2c, can set busy bit to false
  sleep_unblock_mode(I2C_EM_BLOCK);            // done using i2c, can unblock energy mode
  add_scheduled_event(i2c_sm->i2c_cb);         // schedule event to read data

}


/***************************************************************************//**
* @brief
*  Configure i2c using input parameters
*
* @details
*  Enable the clock for i2c0, configure the initialization of the i2c and call
*  a bus reset to ensure i2c is ready for use
*
*  @param [in] i2c_peripheral
*   struct used to specify the i2c_peripheral we are using
*
*  @param [in] i2c_open
*   struct used to configure the I2C_Init_TypeDef for initialization of i2c
*
******************************************************************************/
void i2c_open(I2C_TypeDef *i2c_peripheral, I2C_Open_TypeDef *i2c_open){
  if(i2c_peripheral == I2C0){
      CMU_ClockEnable(cmuClock_I2C0, true);           // enable i2c0 clock
  }
  else if(i2c_peripheral == I2C1){
      CMU_ClockEnable(cmuClock_I2C1, true);           // enable i2c1 clock
  }

  // test clock tree is enabled properly
  i2c_peripheral->CTRL |= I2C_TEST_BIT;
  EFM_ASSERT((i2c_peripheral->CTRL & I2C_TEST_BIT) == I2C_TEST_BIT);
  i2c_peripheral->CTRL &= ~I2C_TEST_BIT;

  I2C_Init_TypeDef i2c_init;
  I2C_Init_TypeDef *init = &i2c_init;

  init->clhr = i2c_open->clhr;
  init->freq = i2c_open->freq;
  init->refFreq = i2c_open->refFreq;
  init->master = i2c_open->master;
  init->enable = i2c_open->enable;

  I2C_Init(i2c_peripheral, init);

  i2c_peripheral->ROUTEPEN = i2c_open->out_pin_SDA_en | (i2c_open->out_pin_SCL_en << SCL_EN_BIT_SHIFT);
  i2c_peripheral->ROUTELOC0 = i2c_open->out_pin_route_SDA | i2c_open->out_pin_route_SCL;    // already shifted

  i2c_peripheral->IEN = (I2C_IEN_ACK | I2C_IEN_NACK | I2C_IEN_MSTOP | I2C_IEN_RXDATAV); // enable these interrupts

  i2c_bus_reset(i2c_peripheral);


  sm.i2c_busy = DISABLE;    // set SM busy bit to not true

}


/***************************************************************************//**
* @brief
*  Start the i2c
*
* @details
*  Configure the private state machine and begin i2c functionality
*
* @param [in] i2c_sm
*  Uses a pointer to our state machine for i2c to start it
*
******************************************************************************/
void i2c_start(I2C_StateMachine_TypeDef *i2c_sm){

  while(sm.i2c_busy);

  // use parameters from function call state machine to private state machine
  sm.current_state = i2c_sm->current_state;
  sm.i2c = i2c_sm->i2c;
  //sm.i2c_busy = i2c_sm->i2c_busy;
  sm.i2c_cb = i2c_sm->i2c_cb;
  sm.i2c_cmd = i2c_sm->i2c_cmd;
  sm.i2c_curr_bytes = i2c_sm->i2c_curr_bytes;
  sm.i2c_data = i2c_sm->i2c_data;
  sm.i2c_device_address = i2c_sm->i2c_device_address;
  sm.i2c_num_bytes = i2c_sm->i2c_num_bytes;
  sm.device_cmd = i2c_sm->device_cmd;


  EFM_ASSERT((I2C0->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);

  sm.i2c_busy = ENABLE;

  sleep_block_mode(I2C_EM_BLOCK);                             // block sleep mode

  i2c_sm->i2c->IFC = _I2C_IF_MASK;                            // clear IF

  i2c_sm->i2c->CMD = I2C_CMD_START;                           // send start command

  i2c_sm->i2c->TXDATA = (i2c_sm->i2c_device_address << I2C_ADDRESS_SHIFT) | i2c_sm->i2c_cmd;   // send device address
}


/***************************************************************************//**
* @brief
*  Perform a bus reset on i2c
*
* @details
*  Take advantage of the CMD register to perform a total bus reset
*
* @param [in] i2c_peripheral
*  struct used to specify the i2c_peripheral we are resetting
*
******************************************************************************/
void i2c_bus_reset(I2C_TypeDef *i2c_peripheral){
  i2c_peripheral->CMD = I2C_CMD_ABORT;                      // abort CMD
  uint32_t IEN_state = i2c_peripheral->IEN;                 // save interrupt's enabled
  i2c_peripheral->IEN = _I2C_IEN_RESETVALUE;                // disable interrupts

  i2c_peripheral->IFC = i2c_peripheral->IF;                 // clear all IF

  EFM_ASSERT(i2c_peripheral->IEN == _I2C_IEN_RESETVALUE);   // ensure interrupts are cleared

  i2c_peripheral->CMD = I2C_CMD_CLEARTX;                    // clear TX

  i2c_peripheral->CMD = I2C_CMD_START | I2C_CMD_STOP;       // Send start and stop

  while(!(i2c_peripheral->IF & I2C_IF_MSTOP));              // ensure it worked correctly

  i2c_peripheral->IFC = i2c_peripheral->IF;                 // clear all IF

  i2c_peripheral->CMD = I2C_CMD_ABORT;                      // abort CMD

  i2c_peripheral->IEN = IEN_state;                          // enable interrupts that we previously disable

}


/***************************************************************************//**
* @brief
*  Interrupt handler for I2C0
*
* @details
*  Checks what flag called the interrupt and call the associated helper function
*
******************************************************************************/
void I2C0_IRQHandler(){
  uint32_t flag = (I2C0->IF) & (I2C0->IEN);

  if((flag & I2C_IF_ACK) == I2C_IF_ACK){
      I2C0->IFC = I2C_IFC_ACK;
      EFM_ASSERT(!(I2C0->IF & I2C_IF_ACK));
      i2c_ack(&sm);
  }

  if((flag & I2C_IF_NACK) == I2C_IF_NACK){
      I2C0->IFC = I2C_IFC_NACK;
      EFM_ASSERT(!(I2C0->IF & I2C_IF_NACK));
      i2c_nack(&sm);
  }

  if((flag & I2C_IF_RXDATAV) == I2C_IF_RXDATAV){
      i2c_rxdatav(&sm);
  }

  if((flag & I2C_IF_MSTOP) == I2C_IF_MSTOP){
      I2C0->IFC = I2C_IF_MSTOP;
      EFM_ASSERT(!(I2C0->IF & I2C_IF_MSTOP));
      i2c_mstop(&sm);
  }
}



/***************************************************************************//**
* @brief
*  Interrupt handler for I2C1
*
* @details
*  Checks what flag called the interrupt and call the associated helper function
*
******************************************************************************/
void I2C1_IRQHandler(){
  uint32_t flag = (I2C1->IF) & (I2C1->IEN);

  if((flag & I2C_IF_ACK) == I2C_IF_ACK){
      I2C1->IFC = I2C_IFC_ACK;
      EFM_ASSERT(!(I2C1->IF & I2C_IF_ACK));
      i2c_ack(&sm);
  }

  if((flag & I2C_IF_NACK) == I2C_IF_NACK){
      I2C1->IFC = I2C_IFC_NACK;
      EFM_ASSERT(!(I2C1->IF & I2C_IF_NACK));
      i2c_nack(&sm);
  }

  if((flag & I2C_IF_RXDATAV) == I2C_IF_RXDATAV){
      i2c_rxdatav(&sm);
  }

  if((flag & I2C_IF_MSTOP) == I2C_IF_MSTOP){
      I2C1->IFC = I2C_IF_MSTOP;
      EFM_ASSERT(!(I2C1->IF & I2C_IF_MSTOP));
      i2c_mstop(&sm);
  }
}


