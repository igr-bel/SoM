#ifndef START_INITS_H
#define START_INITS_H

#include "main.h"

//------------------------------------------------------------------------------------------------------
void sleep_state_init(void)
{
    fog_control(DISABLE);
    room_1_light(OFF);
    room_2_light(OFF);
    UV_light(OFF);
    torch_light(OFF);
    glassPaintLight(DISABLE);
    mirrorLight(DISABLE);
    doorInControl(OFF);
    doorOutControl(OFF);
    doorFairplaceControl(OFF);
    lightDogControl(OFF);
    lightPaintControl(PAINT_1, OFF);
    lightPaintControl(PAINT_2, OFF);
    lightPaintControl(PAINT_3, OFF);
    lightPaintControl(PAINT_4, OFF);
    lightDementorControl(DEMENTOR_1, OFF);
    lightDementorControl(DEMENTOR_2, OFF);
    lightDementorControl(DEMENTOR_3, OFF);
    lightDementorControl(DEMENTOR_4, OFF);

    stopSound();

    sleep_state_done_flag = true;
    return;
}//sleep_state_init()
//------------------------------------------------------------------------------------------------------
void start_variables_init(void)
{
    //начальные состояния магнитов, освещения
    doorOutControl(ON);
    doorFairplaceControl(ON);    
    doorInControl(ON);
    fog_control(DISABLE);
    room_1_light(ON);
    room_2_light(ON);
    torch_light(OFF);
    UV_light(OFF);
    lightPaintControl(PAINT_1, OFF);
    lightPaintControl(PAINT_2, OFF);
    lightPaintControl(PAINT_3, OFF);
    lightPaintControl(PAINT_4, OFF);
    lightDementorControl(DEMENTOR_1, OFF);
    lightDementorControl(DEMENTOR_2, OFF);
    lightDementorControl(DEMENTOR_3, OFF);
    lightDementorControl(DEMENTOR_4, OFF); 

    mirror_led_on = false;
    mirrorLight(DISABLE);
    glass_paint_led_on = false;
    glassPaintLight(DISABLE);

    stopSound();

    //state machine
    current_state = IDLE_STATE;

    hint_stage = TORCH_HINT;
    prew_hint_stage = NULL_HINT;

    //  solved_part = NOTHING;

    //game variables
    for(uint8_t i = 0; i < 4; i++)
    {
        paint_states[i] = 0;
    }
        

    //flags
    sleep_state_done_flag = false;
    game_start_flag = false;
    win_flag = false;
    paint_1_win_flag = false;
    paint_2_win_flag = false;
    paint_3_win_flag = false;
    paint_4_win_flag = false;
    paints_win_flag = false;
    knock_win_flag = false;
    chest_win_flag = false;
    torch_win_flag = false;
    dog_start_flag = false;
    dog_win_flag = false;
    fire_win_flag = false;
    air_win_flag = false;
    water_win_flag = false;
    earth_win_flag = false;
    altar_win_flag = false;
    dementors_win_flag = false;
    new_data_recieved = false;
    mirror_led_on = false;
    glass_paint_led_on = false;
    knock_started_flag = false;
    start_fin_animation_flag = false;

    dog_dor_prew_state = OPEN;

    //shift out enable
    outputEnable();

    //timers
    torch_timer = millis() + torch_interval;
    knock_general_timer = millis() + knock_general_interval;
    hint_send_timer = millis();

    dementor_counter_game_time = DEMENTOR_MIN_GAME_TIME;
    dementor_win_counter = 0;
    dementor_prew_win_counter = 0;
    dementor_err_counter = 0;

    return;
}//start_variables_init()
//------------------------------------------------------------------------------------------------------
void props_is_ready(void)
{
    start_variables_init();

    room_1_light(ON);
    room_2_light(OFF);
    torch_light(ON);
    lightPaintControl(PAINT_1, ON);
    lightPaintControl(PAINT_2, ON);
    lightPaintControl(PAINT_3, ON);
    lightPaintControl(PAINT_4, ON);

    hint_send_timer = millis();

    return;
}//props_is_ready()
//------------------------------------------------------------------------------------------------------

#endif //START_INITS_H