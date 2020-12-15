#ifndef _MQTT_PARSER_H
#define _MQTT_PARSER_H


//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include "main.h"
#include <Ethernet.h>
#include <PubSubClient.h>

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
//ETHERNET      
#define CLIENT_ID                       "client_3"

//out cmd       

//in cmd        
#define SLEEP_CMD                       0x309
#define RESTART_CMD                     0x302
#define RESET_CMD                       0x301
#define START_CMD                       0x306

#define EN_SOUNDS                       0x1011
#define FR_SOUNDS                       0x1012

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
uint8_t mac[6] = {0x12,0xFD,0xBD,0xA4,0x86,0x1C};
const char* server = "192.168.0.50";
IPAddress ip(192, 168, 0, 183);
IPAddress myDns(192, 168, 0, 1);

const String sub_topic = "/client_3_sub";
const String pub_topic = "/client_3_pub";
const String brdcst_topic = "/broadcast";

//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------
EthernetClient ethClient;
PubSubClient mqttClient;

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//ethernet
void subscribeReceive(char* topic, byte* payload, unsigned int length);
void publishData(int data);
void mqttParser(uint16_t command);
inline void setup_ethernet(void);
inline void setup_mqtt(void);

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

        case TORCH_HINT:
            hint_stage = TORCH_HINT;
            break;

        case KNOCKS_HINT:
            hint_stage = KNOCKS_HINT;
            break;

        case CHEST_HINT:
            hint_stage = CHEST_HINT;
            break;

        case PAINTS_HINT:
            hint_stage = PAINTS_HINT;
            break;

        case DOG_HINT:
            hint_stage = DOG_HINT;
            break;

        case FIRE_HINT:
            hint_stage = FIRE_HINT;
            break;

        case AIR_HINT:
            hint_stage = AIR_HINT;
            break;

        case WATER_HINT:
            hint_stage = WATER_HINT;
            break;

        case EARTH_HINT:
            hint_stage = EARTH_HINT;
            break;

        case DEMENTORS_HINT:
            hint_stage = DEMENTORS_HINT;
            break;

        case NULL_HINT:
            hint_stage = NULL_HINT;
            break;

        case EN_SOUNDS:
            hint_lang = EN;
            break;

        case FR_SOUNDS:
            hint_lang = FR;
            break;

        default:
            break;
    }//switch

}//mqttParser()
//------------------------------------------------------------------------------------------------------
void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  //выставляем флаг готовности новых данных
  new_data_recieved = true;

  //сохраняем данные для дальнейшей обработки
  for(uint8_t i=0; i<length; i++)
  {
    msg_payload[i] = payload[i];
  }//for
  msg_length = length;

}//subscribeReceive()
//------------------------------------------------------------------------------------------------------
void publishData(int data)
{
    char msgBuffer[5];  //максимальное количество символов при преобразовании в строку 0xFFFF = 65535

    if(mqttClient.connect(CLIENT_ID)) 
    {
        mqttClient.publish(pub_topic.c_str(), dtostrf(data, 5, 0, msgBuffer));
    }

    return;
}//publishData()
//------------------------------------------------------------------------------------------------------
void setup_ethernet(void)
{
    //
    Ethernet.init(10);
    Ethernet.begin(mac, ip, myDns);
    delay(1500);

    // check for Ethernet hardware present
    if(Ethernet.hardwareStatus() == EthernetNoHardware) 
    {
        Serial.println(" ");
        Serial.println("Ethernet shield was not found.");
        
        while(true) 
        {
            delay(1); // empty loop
        }//while(true)
    }//if
    while(Ethernet.linkStatus() == LinkOFF) 
    {
        Serial.println("Ethernet cable is not connected.");
        delay(500);
    }//while
    // give the Ethernet shield a second to initialize:
    delay(1000);
    Serial.println("connecting...");

}//setup_ethernet()
//------------------------------------------------------------------------------------------------------
void setup_mqtt(void)
{
    mqttClient.setClient(ethClient);
    mqttClient.setServer(server, 1883);

    while(!mqttClient.connect(CLIENT_ID))
    {
        Serial.println("Looks like the server connection failed...");
        Serial.println(mqttClient.state());
        delay(1000);
    }
    Serial.println("Connection has been established, well done");

    mqttClient.setCallback(subscribeReceive);

    mqttClient.subscribe(sub_topic.c_str());
    mqttClient.subscribe(brdcst_topic.c_str());
}//setup_mqtt()
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
#endif //_MQTT_PARSER_H