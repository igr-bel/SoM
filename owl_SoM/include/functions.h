#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
#define DBG

//HINTS CODES
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

//HINTS SOUNDS ../mp3/nnnn.mp3
#define OWL_SOUND                       41

#define TORCH_SOUND_1                   1                       
#define TORCH_SOUND_2                   2 
#define KNOCK_SOUND_1                   3
#define KNOCK_SOUND_2                   4
#define CHEST_SOUND_1                   5
#define CHEST_SOUND_2                   6
#define PAINTS_SOUND_1                  7
#define PAINTS_SOUND_2                  8
#define DOG_SOUND_1                     9
#define DOG_SOUND_2                     10
#define FIRE_SOUND_1                    11
#define FIRE_SOUND_2                    12
#define AIR_SOUND_1                     13
#define AIR_SOUND_2                     14
#define WATER_SOUND_1                   15
#define WATER_SOUND_2                   16
#define EARTH_SOUND_1                   17
#define EARTH_SOUND_2                   18
#define DEMENTORS_SOUND_1               19
#define DEMENTORS_SOUND_2               20

#define TORCH_SOUND_1_FR                21                       
#define TORCH_SOUND_2_FR                22 
#define KNOCK_SOUND_1_FR                23
#define KNOCK_SOUND_2_FR                24
#define CHEST_SOUND_1_FR                25
#define CHEST_SOUND_2_FR                26
#define PAINTS_SOUND_1_FR               27
#define PAINTS_SOUND_2_FR               28
#define DOG_SOUND_1_FR                  29
#define DOG_SOUND_2_FR                  30
#define FIRE_SOUND_1_FR                 31
#define FIRE_SOUND_2_FR                 32
#define AIR_SOUND_1_FR                  33
#define AIR_SOUND_2_FR                  34
#define WATER_SOUND_1_FR                35
#define WATER_SOUND_2_FR                36
#define EARTH_SOUND_1_FR                37
#define EARTH_SOUND_2_FR                38
#define DEMENTORS_SOUND_1_FR            39
#define DEMENTORS_SOUND_2_FR            40

//GPIO
#define sensor_1_pin                    A1
#define sensor_2_pin                    2
#define NEOPIXELS_PIN                   A2
#define NUM_OF_PIXELS                   2
#define PIXEL_FORMAT                    NEO_GRB + NEO_KHZ800   

//DF player
#define player_pin_tx1               8   //TX1
//#define player_pin_rx1                9   //RX1
#ifndef player_pin_rx1
  #define busy_pin_1                    9
#endif

#define VOLUME                          30

//EYE
#define MAX_EYE_BRIGHTNESS              200
#define MIN_EYE_BRIGHTNESS              50
#define EYE_ANIM_DELAY                  1


//------------------------------------------------------------------------------------------------------
//Class for DF player
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};
//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------
Adafruit_NeoPixel pixels(NUM_OF_PIXELS, NEOPIXELS_PIN, PIXEL_FORMAT);


#ifdef player_pin_rx1
  SoftwareSerial mp3_Serial(player_pin_rx1, player_pin_tx1); // RX, TX
#else
  SoftwareSerial mp3_Serial(99, player_pin_tx1); // RX, TX
#endif
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(mp3_Serial);

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
enum
{
    EN = 1,
    FR
};

uint8_t hint_lang = EN;

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//HARDWARE
inline void hardware_init(void);

//HINTS
inline bool hintIsActivated(void);
inline void checkAndPlayHint(uint16_t hint_stage, bool hint_repeat);

//MP3
inline void playSound(uint8_t track);
inline void stopSound(void);
inline void mp3_init(void);

//EYE
void eyeAnimation(void);

//RESET
void(* resetFunc) (void) = 0;


//------------------------------------------------------------------------------------------------------
//                                       BEHAVIOR
//------------------------------------------------------------------------------------------------------
void playSound(uint8_t track)
{
    mp3.stop();
    delay(5);
    mp3.playMp3FolderTrack(track);

    #ifdef DBG
        // Serial.print("[DBG] function playSound started");
        // Serial.println("");
        
        Serial.print("[DBG] play track - ");
        Serial.println(track); 
    #endif
  
}//playSound()
//------------------------------------------------------------------------------------------------------
void stopSound(void)
{
    mp3.stop();
    delay(5);
}//stopSound()
//------------------------------------------------------------------------------------------------------
void mp3_init(void)
{
    #ifdef busy_pin_1
        pinMode(busy_pin_1, INPUT);
        #define mp3isBusy !digitalRead(busy_pin_1)
    #endif

    mp3_Serial.begin (9600);
    mp3.begin();
    mp3.setVolume(VOLUME);

}//mp3_init()
//------------------------------------------------------------------------------------------------------
bool hintIsActivated(void)
{
    bool result = false;
    result = digitalRead(sensor_1_pin) && digitalRead(sensor_2_pin);
    return !result;
}//hintIsActivated()
//------------------------------------------------------------------------------------------------------
void checkAndPlayHint(uint16_t hint_stage, bool hint_repeat)
{
    switch (hint_stage)
    {
        case TORCH_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(TORCH_SOUND_1) : playSound(TORCH_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(TORCH_SOUND_2) : playSound(TORCH_SOUND_2_FR);
            break;
                    
        case KNOCKS_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(KNOCK_SOUND_1) : playSound(KNOCK_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(KNOCK_SOUND_2) : playSound(KNOCK_SOUND_2_FR);
            break;

        case CHEST_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(CHEST_SOUND_1) : playSound(CHEST_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(CHEST_SOUND_2) : playSound(CHEST_SOUND_2_FR);
            break;

        case PAINTS_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(PAINTS_SOUND_1) : playSound(PAINTS_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(PAINTS_SOUND_2) : playSound(PAINTS_SOUND_2_FR);
            break;

        case DOG_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(DOG_SOUND_1) : playSound(DOG_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(DOG_SOUND_2) : playSound(DOG_SOUND_2_FR);
            break;

        case FIRE_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(FIRE_SOUND_1) : playSound(FIRE_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(FIRE_SOUND_2) : playSound(FIRE_SOUND_2_FR);
            break;

        case AIR_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(AIR_SOUND_1) : playSound(AIR_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(AIR_SOUND_2) : playSound(AIR_SOUND_2_FR);
            break;

        case WATER_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(WATER_SOUND_1) : playSound(WATER_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(WATER_SOUND_2) : playSound(WATER_SOUND_2_FR);
            break;

        case EARTH_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(EARTH_SOUND_1) : playSound(EARTH_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(EARTH_SOUND_2) : playSound(EARTH_SOUND_2_FR);
            break;

        case DEMENTORS_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ? playSound(DEMENTORS_SOUND_1) : playSound(DEMENTORS_SOUND_1_FR);
            else
                (hint_lang == EN) ? playSound(DEMENTORS_SOUND_2) : playSound(DEMENTORS_SOUND_2_FR);
            break;

        case NULL_HINT:
            break;

        default:
            break;
    }//switch(check hint)

}//checkAndPlayHint()
//------------------------------------------------------------------------------------------------------
void hardware_init(void)
{
    #ifdef DBG
        Serial.begin(9600);
        Serial.println("___DBG___");
        Serial.print("[DBG] start init ... ");
    #endif

    //GPIO
    pinMode(sensor_1_pin, INPUT);
    pinMode(sensor_2_pin, INPUT);

    pixels.begin();

}//hardware_init()
//------------------------------------------------------------------------------------------------------
void eyeAnimation(void)
{
    static uint8_t current_brightness;
    uint8_t new_brightness;

    new_brightness = random(MIN_EYE_BRIGHTNESS, MAX_EYE_BRIGHTNESS);

    for(uint8_t i = current_brightness; i != new_brightness; )
    {
        for(uint8_t j = 0; j < NUM_OF_PIXELS; j++)
        {
            pixels.setPixelColor(j, pixels.Color(0, i, 0));
        }
        pixels.show();
        delay(EYE_ANIM_DELAY);
        i = (i < new_brightness) ? i + 1 : i - 1;
    }

    current_brightness = new_brightness;

}//eyeAnimation()
//------------------------------------------------------------------------------------------------------

#endif //_FUNCTIONS_H