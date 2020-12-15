#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

#include "DFRobotDFPlayerMini.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
#define DBG

//GPIO
#define oTorh_led_pin                   12
#define oMirror_led_pin                 46
#define oGlass_paint_pin                13
#define iDog_door_pin                   48           
#define iFireplace_pin                  47

//high voltage      
#define oRoom_1_light_pin               28
#define oRoom_2_light_pin               29
#define oUV_light_pin                   30
#define oFog_machine_en_pin             31     

//IN SHIFT_REGISTER     
#define latch_pin_in                    5
#define data_pin_in                     7
#define clock_pin_in                    9
#define NUM_INPUT_REGS                  4

//OUT SHIFT_REGISTER        
#define latch_pin_out                   24
#define data_pin_out                    23
#define clock_pin_out                   22
#define output_enable                   25
#define NUM_OUTPUT_REGS                 2
#ifdef output_enable
    #define outputEnable() digitalWrite(output_enable, 0)
    #define outputDisable() digitalWrite(output_enable, 1)
#endif

//DF player
#define VOLUME                          5
        
//ETHERNET      
#define CLIENT_ID                       "client_main"

//STATE MACHINE
#define IDLE_STATE                      0
#define GAME_1_STATE                    1
#define TORCH_SOLVED_STATE              2
#define KNOCK_SOLVED_STATE              3
#define PAINTS_SOLVED_STATE             4
#define DOG_SOLVED_STATE                5
#define DEMENTORS_GAME_STATE            6
#define DEMENTORS_SOLVED_STATE          7
#define WIN_STATE                       9
#define WIN_IDLE_STATE                  10
#define SLEEP_STATE                     11
#define FINAL_STATE                     13
#define RESET_STATE                     14
#define RESTART_STATE                   15
#define GAME_2_DOG_STATE                16
#define GAME_1_PAINTS_STATE             17
#define ALTAR_GAME_STATE                18
#define LIGHT_ANIMATION_START_STATE     19
#define LIGHT_ANIMATION_STOP_STATE      20

//LEDS / ANIMATION / TIMERS / DELAYS
#define SOOTH_LED_DELAY                 5   //in milliseconds
#define DEMENTOR_MIN_GAME_TIME          3000
#define DEMENTOR_MAX_GAME_TIME          10000
#define DEMENTOR_TIME_INTERVAL          1000
#define DEMENTOR_WIN_COUNT              10
#define DEMENTOR_ERR_COUNT              2
#define DEMENTOR_GAME_DELAY             300

//OFFSETS
#define DOR_IN_LOCK_OFFSET              0
#define DOR_OUT_LOCK_OFFSET             2
#define DOR_FIREPLACE_LOCK_OFFSET       1
#define DOG_LIGHT_OFFSET                3
#define PAINT_1_OFFSET                  4
#define PAINT_2_OFFSET                  5
#define PAINT_3_OFFSET                  6
#define PAINT_4_OFFSET                  7

#define DEMENTOR_1_LED_OFFSET           0
#define DEMENTOR_2_LED_OFFSET           1
#define DEMENTOR_3_LED_OFFSET           2
#define DEMENTOR_4_LED_OFFSET           3

//SHIFT MASKS
#define TORCH_STATE_MASK                0x0F
#define KNOCK_STATE_MASK                0xF0
#define KNOCK_1_MASK                    0x10
#define KNOCK_2_MASK                    0x20
#define KNOCK_3_MASK                    0x40
#define KNOCK_4_MASK                    0x80
#define PAINT_1_MASK                    0x0F
#define PAINT_2_MASK                    0xF0
#define PAINT_3_MASK                    0x0F
#define PAINT_4_MASK                    0xF0
#define DEMENTOR_1_MASK                 0x01
#define DEMENTOR_2_MASK                 0x02
#define DEMENTOR_3_MASK                 0x04
#define DEMENTOR_4_MASK                 0x08

//SOUNDS ../mp3/nnnn.mp3
#define DOG_SOUND                       1
#define TORCH_SOUND                     2
#define KNOCK_ERR_SOUND                 3

//HINTS FOR OWL
#define TORCH_HINT                      0x901
#define KNOCKS_HINT                     0x902
#define CHEST_HINT                      0x903
#define PAINTS_HINT                     0x904
#define DOG_HINT                        0x905
#define FIRE_HINT                       0x906
#define AIR_HINT                        0x907
#define WATER_HINT                      0x908
#define EARTH_HINT                      0x909
#define DEMENTORS_HINT                  0x910
#define NULL_HINT                       0x911

//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------
EthernetClient ethClient;
PubSubClient mqttClient;
DFRobotDFPlayerMini myDFPlayer;

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
//-------------------------------------------- ENUMS / STRUCTS
enum
{
    PAINT_1,
    PAINT_2,
    PAINT_3,
    PAINT_4
};

enum
{
    DEMENTOR_1,
    DEMENTOR_2,
    DEMENTOR_3,
    DEMENTOR_4,
    NOT_ACTIVATED
};

enum
{
    NOT_SOLVED,
    SOLVED,
    ERROR,
    COUNT_ERROR,
    IN_PROGRESS,
    RELEASED
};

enum
{
    OFF,
    ON,
    ENABLE,
    DISABLE
};

enum
{
    CLOSED,
    OPEN
};

//-------------------------------------------- INPUT SHIFT REGISTER
uint8_t shift_input_buff[NUM_INPUT_REGS];     //первый датчик == нулевой бит
uint8_t shift_output_buff[NUM_OUTPUT_REGS];   //первый выход == нулевой бит

//-------------------------------------------- GAME COUNTERS & DATA
uint16_t hint_stage = 0;        //data for owl
uint16_t prew_hint_stage = 99;
uint8_t solved_part = 0;        //number of prop is solved by operator

uint8_t paint_states[4];        //состояние картин (решена / не решена)

uint8_t prew_dementor;
uint8_t current_dementor;
uint8_t active_dementor_sensor;
uint16_t dementor_counter_game_time;
uint8_t dementor_win_counter;
uint8_t dementor_prew_win_counter;
uint8_t dementor_err_counter;
unsigned long dementor_start_time;

unsigned long torch_timer;
uint16_t torch_interval = 25000;

unsigned long glass_paint_timer;
uint16_t glass_paint_interval = 25000;
uint16_t glass_paint_hide_interval = 30000;
unsigned long glass_paint_hide_timer;

uint8_t knock_state;
unsigned long knock_general_timer;
unsigned long knock_1_timer;
unsigned long knock_2_timer;
unsigned long knock_3_timer;
unsigned long knock_4_timer;
uint16_t knock_general_interval = 3000;
uint16_t knock_separate_interval = 1000;

unsigned long dog_timer;
uint16_t dog_interval = 3000;
unsigned long dog_voice_timer;
uint16_t dog_voice_interval = 5000;
uint8_t dog_dor_prew_state = OPEN;

unsigned long hint_send_timer;
uint16_t hint_send_interval = 1000;

//-------------------------------------------- ETHERNET
uint8_t mac[6] = {0xA0,0x12,0x5D,0xC8,0x88,0x0B};
const char* server = "192.168.0.50";
IPAddress ip(192, 168, 0, 182);
IPAddress myDns(192, 168, 0, 1);

const String sub_topic = "/main_sub";
const String pub_topic = "/main_pub";
const String brdcst_topic = "/broadcast";

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


#ifdef DBG
  unsigned long DBG_out_interval = 100;
  unsigned long DBG_time_delay = 0;
#endif
//------------------------------------------------------------------------------------------------------
//                                        FLAGS
//------------------------------------------------------------------------------------------------------
bool sleep_state_done_flag = false;
bool game_start_flag = false;
bool win_flag = false;
bool paint_1_win_flag = false;
bool paint_2_win_flag = false;
bool paint_3_win_flag = false;
bool paint_4_win_flag = false;
bool paints_win_flag = false;
bool knock_win_flag = false;
bool chest_win_flag = false;
bool torch_win_flag = false;
bool dog_start_flag = false;
bool dog_win_flag = false;
bool fire_win_flag = false;
bool air_win_flag = false;
bool water_win_flag = false;
bool earth_win_flag = false;
bool altar_win_flag = false;
bool dementors_win_flag = false;

bool start_fin_animation_flag = false;

bool new_data_recieved = false;

bool mirror_led_on = false;
bool glass_paint_led_on = false;
bool knock_started_flag = false;

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//hardware
void sleep_state_init(void);
void start_variables_init(void);
void props_is_ready(void);

void fog_control(uint8_t state);                                            //разрешение работы, вкл/выкл дыммашины
void room_1_light(uint8_t state);                                           //управление светом в первой комнате
void room_2_light(uint8_t state);                                           //управление светом во второй комнате
void UV_light(uint8_t state);                                               //управление УФ светом
void torch_light(uint8_t state);                                            //управление светом факелов
uint8_t isDogDoorOpen(void);                                                //состояние двери собаки
uint8_t isFireplaceDoorOpen(void);                                          //состояние двери в камине
void glassPaintLight(uint8_t state);                                        //подсветка витража
void mirrorLight(uint8_t state);                                            //подсветка зеркала

void doorInControl(uint8_t state);                                          //замок входа
void doorOutControl(uint8_t state);                                         //замок выхода
void doorFairplaceControl(uint8_t state);                                   //замок камина
void lightDogControl(uint8_t state);                                        //подсветка собаки
void lightPaintControl(uint8_t number_of_paint, uint8_t state);             //подсветка картины
void lightDementorControl(uint8_t number_of_dementor, uint8_t state);       //подсветка дементора
void lightAnimation_2_room(void);

uint8_t getTorchState(void);                                                //статус загадки "факелы"
uint8_t getKnockState(void);                                                //статус загадки "стучалки"
void getPaintState(uint8_t * state_buff);                                   //статус загадки "картины"
uint8_t getDementorsState(void);

//MP3
void playSound(uint8_t track);
void stopSound(void);

//shift registers
int8_t shiftInData(uint8_t *data_buff, uint32_t len_of_data);
int8_t shiftOutData(uint8_t *data_buff, uint32_t len_of_data);

//game
inline uint8_t dogGame(void);
void win(void);

//ethernet
void subscribeReceive(char* topic, byte* payload, unsigned int length);
void publishData(int data);

//reset
void(* resetFunc) (void) = 0;


//------------------------------------------------------------------------------------------------------
#endif //MAIN_H_INCLUDED