/**
 * @file sleep_routines.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  04/02/2023
 *
 * @brief
 *  Define the sleep routines
 *
 */

/**************************************************************************
* @file sleep.c
***************************************************************************
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
***************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
**************************************************************************/
//***********************************************************************************
// Include files
//***********************************************************************************

#include "sleep_routines.h"

//***********************************************************************************
// private variables
//***********************************************************************************

static uint32_t lowest_energy_mode[MAX_ENERGY_MODES];

//***********************************************************************************
// function
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *  Initialization of sleep_routines
 *
 * @details
 *  Initialize the sleep_routines static / private array, lowest_energy_mode[], to
 *  all zeroes
 *
 ******************************************************************************/
void sleep_open(void){
  for(int i=0; i< MAX_ENERGY_MODES; i++){
      lowest_energy_mode[i] = 0;
   }
}


/***************************************************************************//**
 * @brief
 *  Blocks the sleep mode
 *
 * @details
 *  Utilized by a peripheral to prevent the Mighty Gecko from
 *  going into that sleep mode while the peripheral is active. It will increment the associated array
 *  element in lowest_energy_mode[] by one.
 *
 * @param [in] EM
 *  Block the given energy mode
 *
 ******************************************************************************/
void sleep_block_mode(uint32_t EM){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  lowest_energy_mode[EM] ++;

  CORE_EXIT_CRITICAL();

  // should not go over the max energy modes
  EFM_ASSERT(lowest_energy_mode[EM] < MAX_ENERGY_MODES);
}


/***************************************************************************//**
 * @brief
 *  Released the processor from going into sleep mode
 *
 * @details
 *  Utilized to release the processor from going into a sleep
 *  mode with a peripheral that is no longer active. It will decrement the associated array element in
 *  lowest_energy_mode[] by one.
 *
 * @param [in] EM
 *  Unblock the given energy mode
 *
 ******************************************************************************/
void sleep_unblock_mode(uint32_t EM){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  lowest_energy_mode[EM] --;

  CORE_EXIT_CRITICAL();

  // application is calling more unblock sleep modes than block sleep modes
  EFM_ASSERT(lowest_energy_mode[EM] >= EM0);
}


/***************************************************************************//**
 * @brief
 *  Enter appropriate sleep energy mode
 *
 * @details
 *  Function that will enter the appropriate sleep Energy Mode based on the
 *  first non-zero array element in lowest_energy_mode[].
 *
 ******************************************************************************/
void enter_sleep(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  if(lowest_energy_mode[EM0] > 0 || lowest_energy_mode[EM1] > 0){}
  else if(lowest_energy_mode[EM2] > 0){
      EMU_EnterEM1();
  }
  else if(lowest_energy_mode[EM3] > 0){
      EMU_EnterEM2(true);
  }
  else{
      EMU_EnterEM3(true);
  }

  CORE_EXIT_CRITICAL();
  return;
}


/***************************************************************************//**
 * @brief
 *  Returns which energy mode system cannot enter
 *
 * @details
 *  Function that returns which energy mode that the
 *  current system cannot enter, the first non-zero array element in lowest_energy_mode[].
 *
 *  @param [out] EM
 *    the number of the energy mode that the current system cannot enter
 *
 ******************************************************************************/
uint32_t current_block_energy_mode(void){
  for(int i = 0; i < MAX_ENERGY_MODES; i++){
      if(lowest_energy_mode[i] != EM0){
          return i;
      }
  }
  return SOME_ERROR;      // change this
}
