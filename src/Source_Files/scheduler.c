/**
 * @file scheduler.c
 *
 * @author
 *  Ginn Sato
 *
 * @date
 *  03/07/2023
 *
 * @brief
 *  Schedule
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#include "scheduler.h"

//***********************************************************************************
// private variables
//***********************************************************************************

static uint32_t event_scheduled;

//***********************************************************************************
// function prototypes
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Initialize the scheduler
 *
 * @details
 *  Set the even_scheduled private static variable to zero
 *
 ******************************************************************************/
void scheduler_open(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled = NO_EVENTS;
  CORE_EXIT_CRITICAL();
}


/***************************************************************************//**
 * @brief
 *  Get the scheduled events
 *
 * @details
 *  returns a 32 bit integer that represents the scheduled events
 *
 ******************************************************************************/
uint32_t get_scheduled_events(void){
  return event_scheduled;
}


/***************************************************************************//**
 * @brief
 *  Add a scheduled event
 *
 * @details
 *  Use an atomic operation to access and update our even scheduler, to add a
 *  new event
 *
 ******************************************************************************/
void add_scheduled_event(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled |= event;
  CORE_EXIT_CRITICAL();
}


/***************************************************************************//**
 * @brief
 *  Remove a scheduled event
 *
 * @details
 *  Used to remove a specific event in the event scheduler. Often done after
 *  reading the events.
 *
 ******************************************************************************/
void remove_scheduled_event(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled &= ~event;
  CORE_EXIT_CRITICAL();
}
