#include "main.h"
#include "mqtt_parser.h"
#include "start_inits.h"
#include "functions.h"

//======================================================================================================
//------------------------------------------------------------------------------------------------------
//                                        SETUP
//------------------------------------------------------------------------------------------------------
void setup(void)
{
  //------------------------------------------------------ GAME INITS
  hardware_init();
  //------------------------------------------------------ GAME INITS
  start_variables_init();
  //------------------------------------------------------ ETHERNET
  setup_ethernet();
  //------------------------------------------------------ setup mqtt client
  setup_mqtt();  
  //------------------------------------------------------ MP3 INITS
  mp3_init();

}//setup()
//------------------------------------------------------------------------------------------------------
//                                        LOOP
//------------------------------------------------------------------------------------------------------
void loop(void)
{
  //------------------------------------------- ALGORITHM >>>
  //------------------------------------------------------------------------------------
  //STATE MACHINE
  switch(current_state)
  {
  case SLEEP_STATE:
    if(!sleep_state_done_flag)
    {
      sleep_state_init();
    }
    break;

  case RESET_STATE:
    resetFunc();
    break;

  case RESTART_STATE:
    start_variables_init();
    current_state = IDLE_STATE;
    break;
    
  case IDLE_STATE:
    if(game_start_flag == true)
    {
      props_is_ready();
      current_state = GAME_STATE;
    }
    break;

  case GAME_STATE: 
    if( hintIsActivated() && !((mp3isBusy) ? ((owl_voice_is_active) ? false : true) : false) )
    {
      if(hint_stage != prew_hint_stage)
      {
        prew_hint_stage = hint_stage;
        hint_repeat = false;
      }
      else
      {
        hint_repeat = true;
      }
      stopSound();
      owl_voice_is_active = false;
      checkAndPlayHint(hint_stage, hint_repeat);
      eyeAnimation();
      current_state = WAIT_HINT_RELEASE;
    }
    delay(50);  //delay for activate busy pin

    //------------------------------------------- OWL LIFE >>>
    if(millis() - owl_voice_timer > owl_voice_period)
    {
      eyeAnimation();
      if(!mp3isBusy)
        playSound(OWL_SOUND);
      delay(50); //delay for activate busy pin

      owl_voice_period = random(MIN_VOICE_DELAY, MAX_VOICE_DELAY);
      owl_voice_timer = millis();
      owl_voice_is_active = true;
    }
    else
    {
      pixels.clear();
      pixels.show();
      owl_voice_is_active = false;
    }
    
    if(mp3isBusy)
    {
      eyeAnimation();
    }
    else
    {
      pixels.clear();
      pixels.show();
      owl_voice_is_active = false;
    }
    //------------------------------------------- <<< OWL LIFE

    break;

  case WAIT_HINT_RELEASE:
    if(!hintIsActivated())
      current_state = GAME_STATE;
    
    if(mp3isBusy)
    {
      eyeAnimation();
    }
    else
    {
      pixels.clear();
      pixels.show();
      owl_voice_is_active = false;
    }

    break;

  default:
    break;
  }//switch STATE MACHINE
  //------------------------------------------- <<< ALGORITHM

  //------------------------------------------- COMMUNICATION >>>
  mqttClient.loop();

  //parse new data
  if(new_data_recieved)
  {
    new_data_recieved = false;

    //string to command code
    command = 0;
    uint8_t shft = (msg_length == 5) ? 8 : 12;
    for(uint8_t indx = 2; indx < msg_length; indx++)
    {
      command |= (msg_payload[indx] > 64) ? (msg_payload[indx] - 55) << shft : (msg_payload[indx] - 48) << shft;
      shft -= 4;
    }//for
    mqttParser(command);

    mqttClient.loop();
    
  }//if new data recieved  
  //------------------------------------------- <<< COMMUNICATION

  //------------------------------------------- RECONNECT >>>
  if(mqttClient.state() != MQTT_CONNECTED)
  {
    while(!mqttClient.connect(CLIENT_ID))
    {
      setup_ethernet();
      delay(1000);
    }
    setup_mqtt();  
  }//if connection is lost
  //------------------------------------------- <<< RECONNECT

  //------------------------------------------------------------------------------------

  //---DBG info
  #ifdef DBG
    if(millis() - DBG_time_delay >= DBG_out_interval)
    {
      DBG_time_delay = millis();
      
      // Serial.print("[DBG]  = ");
      // Serial.println("");
      
      // Serial.print("[DBG]  = ");
      // Serial.println("");

      // Serial.print("[DBG]  = ");
      // Serial.println("");   
    }
  #endif
  
}//loop()
