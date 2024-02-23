/**
 * @file cmu.c
 *
 * @author
 *  Ginn Sato and Arielle Blum
 *
 * @date
 *  02/23/2023
 *
 * @brief
 *  Enable all oscillators and route the clock tree
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *  Enable the oscillators and route clock tree
 *
 * @details
 *  Use predefined functions to achieve this task
 *
 *
 ******************************************************************************/
void cmu_open(void){

		//CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
		//CMU_ClockEnable(cmuClock_HFPER, true);
    //CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);  // this is enabled by default

		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	  // By default, LFRCO is enabled
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO

		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// routing clock to LFA

		CMU_ClockEnable(cmuClock_CORELE, true);

}

