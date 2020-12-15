#ifndef _START_INITS_H
#define _START_INITS_H

#include "main.h"

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//hardware
inline void sleep_state_init(void);
inline void start_variables_init(void);
inline void props_is_ready(void);


//------------------------------------------------------------------------------------------------------
//                                       BEHAVIOR
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
void sleep_state_init(void)
{
    stopSound();

    //PIXELS
    pixels.clear(); // Set all pixel colors to 'off'
    pixels.show();

    sleep_state_done_flag = true;
    return;
}//sleep_state_init()
//------------------------------------------------------------------------------------------------------
void start_variables_init(void)
{
    //начальные состояния
    stopSound();

    //PIXELS
    pixels.clear(); // Set all pixel colors to 'off'
    pixels.show();

    //state machine
    current_state = IDLE_STATE;

    hint_stage = TORCH_HINT;

    //flags
    sleep_state_done_flag = false;
    game_start_flag = false;
    hint_repeat = false;
    owl_voice_is_active = false;
    hint_stage = TORCH_HINT;
    prew_hint_stage = TORCH_HINT;

    new_data_recieved = false;

    hint_lang = EN;

    return;
}//start_variables_init()
//------------------------------------------------------------------------------------------------------
void props_is_ready(void)
{
    hint_repeat = false;
    hint_stage = TORCH_HINT;
    prew_hint_stage = NULL_HINT;

    new_data_recieved = false;
    owl_voice_is_active = false;

    hint_lang = EN;
    
    delay(1000);
    return;
}//props_is_ready()
//------------------------------------------------------------------------------------------------------

#endif //_START_INITS_H