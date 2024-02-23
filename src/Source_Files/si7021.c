/**
 * @file si7021.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/16/2023
 *
 * @brief
 *  Setup si7021
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "si7021.h"

//***********************************************************************************
// Private Variables
//***********************************************************************************

static uint32_t raw_sensor_data;

//***********************************************************************************
// Functions
//***********************************************************************************

/***************************************************************************//**
* @brief
*  Configure i2c for si7021
*
* @details
*  Update the I2C_Open_TypeDef for our sensor specific needs, and wait for the
*  sensor to power up before calling i2c_open.
*
******************************************************************************/
void si7021_open(void){
  timer_delay(Si7021_POWER_UP_DELAY);                 // delay for power up

  I2C_Open_TypeDef I2C_OPEN;
  I2C_Open_TypeDef *i2c = &I2C_OPEN;

  i2c->enable = ENABLE;
  i2c->clhr = i2cClockHLRAsymetric;
  i2c->freq = I2C_FREQ_FAST_MAX;
  i2c->out_pin_route_SCL = I2C_ROUTELOC0_SCLLOC_LOC15;
  i2c->out_pin_route_SDA = I2C_ROUTELOC0_SDALOC_LOC15;
  i2c->out_pin_SCL_en = I2C_ROUTEPEN_SCLPEN;
  i2c->out_pin_SDA_en = I2C_ROUTEPEN_SDAPEN;
  i2c->master = ENABLE;
  i2c->refFreq = DISABLE;

  i2c_open(I2C0, i2c);
}


/***************************************************************************//**
* @brief
*  Configure state machine for reading from si7021
*
* @details
*  Create an instance of the SM struct so that it is compatible to read from
*  the Si7021 sensor via the i2c. We then start the i2c operation that will
*  read from the sensor.
*
* @param [in] call_back
*   The call back to be added to the events scheduler after successful
*   completion of the si7021 read
*
******************************************************************************/
void si7021_read(uint32_t call_back){
  raw_sensor_data = 0;                          // reset raw sensor data value

  I2C_StateMachine_TypeDef sm;
  sm.current_state = I2C_INIT;                  // initial state of SM
  sm.i2c = I2C0;                                // I2C Peripheral Number
  //sm.i2c_busy = DISABLE;                      // Not busy yet
  sm.i2c_cb = call_back;                        // Call Back bit
  sm.i2c_cmd = WRITE;                           // read or write bit
  sm.i2c_curr_bytes = Si7021_CUR_BYTES;         // number of bytes read so far (0)
  sm.i2c_data = &raw_sensor_data;               // pointer to raw data
  sm.i2c_device_address = SI7021_I2C_ADDRESS;   // address of peripheral sensor
  sm.i2c_num_bytes = Si7021_NUM_BYTES;          // number of bytes to read
  sm.device_cmd = Si7021_READ_TEMP_CMD;         // read temperature cmd

  i2c_start(&sm);
}


/***************************************************************************//**
* @brief
*  Function to access the raw sensor data
*
* @details
*  This function transfers the data held in our private static variable
*  raw_sensor_data so that it is accessible in the application layer
*
******************************************************************************/
uint32_t si7021_get_raw_data(void){
  return raw_sensor_data;
}



/***************************************************************************//**
* @brief
*  Convert the raw data into a temperature
*
* @details
*  Using the pre-defined calculation found in the Si7021 data sheet, this
*  function performs the appropriate operation to convert the raw sensor data
*  held inside 2 bytes into a temperature in units of degrees Celsius.
*
******************************************************************************/
float si7021_calc_temp(uint32_t raw_data){
  return (175.72*raw_data / 65536.0) - 46.85;
}
