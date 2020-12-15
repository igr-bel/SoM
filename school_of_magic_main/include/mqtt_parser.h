#ifndef MQTT_PARSER_H
#define MQTT_PARSER_H

#include "main.h"

//out cmd       
#define WIN_PAYLOAD                     0x2FF
#define SOLVE_CLEAR                     0x200
#define TORCH_WIN                       0x205
#define KNOCK_ERROR                     0x2F2
#define KNOCK_WIN                       0x255
#define PAINT_1_WIN                     0x211
#define PAINT_2_WIN                     0x212
#define PAINT_3_WIN                     0x213
#define PAINT_4_WIN                     0x214
#define PAINTS_WIN                      0x21F
#define DOG_WIN                         0x260
#define DEMENTORS_WIN                   0x270
#define DEMENTORS_CNT_0                 0x280
//in cmd        
#define SLEEP_CMD                       0x309
#define RESTART_CMD                     0x302
#define RESET_CMD                       0x301
#define START_CMD                       0x306
#define SOLVE_TORCH                     0x3F1
#define SOLVE_KNOCK                     0x351
#define SOLVE_PAINT_1                   0x311
#define SOLVE_PAINT_2                   0x312
#define SOLVE_PAINT_3                   0x313
#define SOLVE_PAINT_4                   0x314
#define SOLVE_DOG                       0x361
#define SOLVE_DEMENTORS                 0x371
#define SOLVE_CHEST                     0x381    
#define SOLVE_ALTAR                     0x391
#define SOLVE_FIRE                      0x392
#define SOLVE_AIR                       0x393
#define SOLVE_WATER                     0x394
#define SOLVE_EARTH                     0x395
#define FINAL_ANIMATION                 0X913
#define SWITCH_ON_UV                    0x501
#define SWITCH_OFF_UV                   0x601
#define SWITCH_ON_LIHGT_1               0x502
#define SWITCH_OFF_LIGHT_1              0x602
#define SWITCH_ON_LIHGT_2               0x503
#define SWITCH_OFF_LIGHT_2              0x603
#define LOCKED_OUT_DOOR                 0x504
#define UNLOCKED_OUT_DOOR               0x604
#define START_LIGHT_ANIMATION           0x506
#define STOP_LIGHT_ANIMATION            0x606

//------------------------------------------------------------------------------------------------------
void mqttParser(uint16_t command)
{
    switch(command)
    {
        case SLEEP_CMD:
            current_state = SLEEP_STATE;
            break;

        case RESTART_CMD:     //restart prop
            current_state = RESTART_STATE;
            break;

        case RESET_CMD:       //reset prop
            current_state = RESET_STATE;
            break;

        case START_CMD:       //start prop
            current_state = IDLE_STATE;
            game_start_flag = true;
            break;

        case SOLVE_TORCH:
            current_state = TORCH_SOLVED_STATE;
            break;

        case SOLVE_KNOCK:
            current_state = KNOCK_SOLVED_STATE;
            break;

        case SOLVE_PAINT_1:
            publishData(PAINT_1_WIN); 
            lightPaintControl(PAINT_1, OFF);
            paint_1_win_flag = true; 
            current_state = GAME_1_PAINTS_STATE;
            break;

        case SOLVE_PAINT_2:
            publishData(PAINT_2_WIN); 
            lightPaintControl(PAINT_2, OFF);
            paint_2_win_flag = true; 
            current_state = GAME_1_PAINTS_STATE;
            break;

        case SOLVE_PAINT_3:
            publishData(PAINT_3_WIN); 
            lightPaintControl(PAINT_3, OFF);
            paint_3_win_flag = true; 
            current_state = GAME_1_PAINTS_STATE;
            break;

        case SOLVE_PAINT_4:
            publishData(PAINT_4_WIN); 
            lightPaintControl(PAINT_4, OFF);
            paint_4_win_flag = true; 
            current_state = GAME_1_PAINTS_STATE;
            break;

        case SOLVE_DOG:
            current_state = DOG_SOLVED_STATE;
            break;

        case SOLVE_DEMENTORS:
            current_state = DEMENTORS_SOLVED_STATE;
            break;

        case SOLVE_CHEST:
            chest_win_flag = true;
            break;

        case SOLVE_FIRE:
            fire_win_flag = true;
            break;

        case SOLVE_AIR:
            air_win_flag = true;
            break;  

        case SOLVE_WATER:
            water_win_flag = true;
            break; 

        case SOLVE_EARTH:
            earth_win_flag = true;
            break;

        case SWITCH_ON_UV:
            UV_light(ON);
            break;

        case SWITCH_OFF_UV:
            UV_light(OFF);
            break;

        case SWITCH_ON_LIHGT_1:
            room_1_light(ON);
            break;

        case SWITCH_OFF_LIGHT_1:
            room_1_light(OFF);
            break;

        case SWITCH_ON_LIHGT_2:
            room_2_light(ON);
            break;

        case SWITCH_OFF_LIGHT_2:
            room_2_light(OFF);
            break;

        case LOCKED_OUT_DOOR:
            doorOutControl(ON);
            break;        

        case UNLOCKED_OUT_DOOR:
            doorOutControl(OFF);
            break;

        case START_LIGHT_ANIMATION:
            current_state = LIGHT_ANIMATION_START_STATE;
            start_fin_animation_flag = true;
            break;   

        case STOP_LIGHT_ANIMATION:
            current_state = LIGHT_ANIMATION_STOP_STATE;
            break; 

        default:
            break;
    }//switch

}//mqttParser()

#endif //MQTT_PARSER_H