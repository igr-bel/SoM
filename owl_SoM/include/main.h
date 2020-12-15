#ifndef _MAIN_H
#define _MAIN_H


//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "functions.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
#define DBG

//STATE MACHINE
#define IDLE_STATE                      0
#define SLEEP_STATE                     1
#define RESET_STATE                     2
#define RESTART_STATE                   3
#define WAIT_HINT_RELEASE               4
#define WIN_STATE                       5
#define WIN_IDLE_STATE                  6
#define GAME_STATE                      7

//OWL RANDOM VOICE
#define MAX_VOICE_DELAY                 240000  //4 min
#define MIN_VOICE_DELAY                 120000  //2 min

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
//-------------------------------------------- TIMERS
unsigned long eye_timer;
uint16_t eye_animation_period;
unsigned long owl_voice_timer;
uint16_t owl_voice_period;

//-------------------------------------------- INPUT ETHERNET DATA
byte msg_payload[10];
uint8_t msg_length;

//-------------------------------------------- DATA PARSER
uint16_t command;
String cmd_string;
char *pos = 0;
int base = 16;

//-------------------------------------------- STATE MACHINE
uint8_t current_state;

//-------------------------------------------- HINTS
uint16_t hint_stage = NULL_HINT; 
uint16_t prew_hint_stage = NULL_HINT; 
bool hint_repeat = false;

#ifdef DBG
  unsigned long DBG_out_interval = 100;
  unsigned long DBG_time_delay = 0;
#endif
//------------------------------------------------------------------------------------------------------
//                                        FLAGS
//------------------------------------------------------------------------------------------------------
bool game_start_flag = false;
bool new_data_recieved = false;
bool sleep_state_done_flag = false;
bool owl_voice_is_active = false;

//------------------------------------------------------------------------------------------------------
#endif //_MAIN_H