#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#include "main.h"

//------------------------------------------------------------------------------------------------------
//                                        FUNCTIONS
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
inline int8_t shiftInData(uint8_t *data_buff, uint32_t len_of_data)
{
    if(data_buff == nullptr)
    {
        return -1;  
    }
    else if(len_of_data == 0)
    {
        return -2;  
    }

    //записать данные с входов во внутренний регистр 
    digitalWrite(latch_pin_in, LOW);
    digitalWrite(latch_pin_in, HIGH);

    //последовательное получение данных из 74HC165
    for(uint8_t i = 0; i < len_of_data; i++)
    {
    data_buff[i] = 0;
	    for(int8_t j = 7; j >=0 ; j--)
	    {
	        digitalWrite(clock_pin_in, LOW);
	        data_buff[i] |= (digitalRead(data_pin_in)) ? (0 << j) : (1 << j);
	        digitalWrite(clock_pin_in, HIGH);
	    }//for_inner
    }//for

    #ifdef DBG
    //  Serial.println("[DBG] shift in ");
    //    for(int i = 0; i < len_of_data; i++)
    //    {
    //    Serial.print("data_buff[");
    //    Serial.print(i);
    //    Serial.print("] = ");
    //    Serial.println(data_buff[i]);
    //    }//for
    //    Serial.println("<<< ");
    #endif

    return 0;
}//shiftInData()
//------------------------------------------------------------------------------------------------------
int8_t shiftOutData(uint8_t *data_buff, uint32_t len_of_data)
{
    if(data_buff == nullptr)
    {
        return -1;  
    }
    else if(len_of_data == 0)
    {
        return -2;  
    }

    digitalWrite(latch_pin_out, LOW);
    for(int8_t i = len_of_data - 1; i >= 0; i--)
    {
        shiftOut(data_pin_out, clock_pin_out, MSBFIRST, data_buff[i]);
    }
    digitalWrite(latch_pin_out, HIGH);

    #ifdef DBG
    //  Serial.println("[DBG] function shift out");
    //    Serial.print("data_buff[0] = ");
    //    Serial.println(data_buff[0]);
    //    Serial.print("data_buff[1] = ");
    //    Serial.println(data_buff[1]);
    #endif

    return 0;
}//shiftOutData()
//------------------------------------------------------------------------------------------------------
inline void win(void)
{
    doorOutControl(OFF);
    room_2_light(ON);
}//win()
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
void playSound(uint8_t track)
{
    myDFPlayer.pause();
    delay(5);
    myDFPlayer.play(track);

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
    myDFPlayer.pause();
    delay(5);
}//stopSound()
//------------------------------------------------------------------------------------------------------
void room_1_light(uint8_t state)
{
    digitalWrite(oRoom_1_light_pin, state);
}//room_1_light()
//------------------------------------------------------------------------------------------------------
void room_2_light(uint8_t state)
{
    digitalWrite(oRoom_2_light_pin, state);
}//room_2_light()
//------------------------------------------------------------------------------------------------------
void UV_light(uint8_t state)
{
    digitalWrite(oUV_light_pin, state);
}//UV_light()
//------------------------------------------------------------------------------------------------------
void fog_control(uint8_t state)
{
  switch(state)
  {
    case ON:
    case OFF:
        //включаем через контроллер алтаря
        break;

    case ENABLE:
        digitalWrite(oFog_machine_en_pin, HIGH);
        break;

    case DISABLE:
        digitalWrite(oFog_machine_en_pin, LOW);
        break;

    default:
        break;
  }//switch
  
}//fog_control()
//------------------------------------------------------------------------------------------------------
void torch_light(uint8_t state)
{
    digitalWrite(oTorh_led_pin, state); 
}//torch_light()
//------------------------------------------------------------------------------------------------------
uint8_t isDogDoorOpen(void)
{
    return !digitalRead(iDog_door_pin);
}//isDogDoorOpen()
//------------------------------------------------------------------------------------------------------
uint8_t isFireplaceDoorOpen(void)
{
    return !digitalRead(iFireplace_pin);
}//isFireplaceDoorOpen()
//------------------------------------------------------------------------------------------------------
void glassPaintLight(uint8_t state)
{
    switch (state)
    {
    case ON:
        if(glass_paint_led_on == false)
        {
            for(int i = 0; i < 255; i++)
            {
                analogWrite(oGlass_paint_pin, i);  
                delay(SOOTH_LED_DELAY);
            }//for
            analogWrite(oGlass_paint_pin, 0);
            digitalWrite(oGlass_paint_pin, HIGH);
            glass_paint_led_on = true;
        }//if
        break;

    case OFF:
        if(glass_paint_led_on == true)
        {
            for(int i = 255; i >= 0; i--)
            {
                analogWrite(oGlass_paint_pin, i);  
                delay(SOOTH_LED_DELAY);
            }//for
            glass_paint_led_on = false;
        }//if
        break;

    case ENABLE:
        digitalWrite(oGlass_paint_pin, HIGH);
        break;

    case DISABLE:
        digitalWrite(oGlass_paint_pin, LOW);
        break;
    
    default:
        break;
    }//switch

}//glassPaintLight()
//------------------------------------------------------------------------------------------------------
void mirrorLight(uint8_t state)
{
    switch(state)
    {
    case ON:
        if(mirror_led_on == false)
        {
            for(int i = 0; i < 255; i++)
            {
                analogWrite(oMirror_led_pin, i);  
                delay(SOOTH_LED_DELAY);
            }//for
            analogWrite(oMirror_led_pin, 0);
            digitalWrite(oMirror_led_pin, HIGH);
            mirror_led_on = true;
        }
        break;

    case OFF:
        if(mirror_led_on == true)
        {
            for(int i = 255; i >= 0; i--)
            {
                analogWrite(oMirror_led_pin, i);  
                delay(SOOTH_LED_DELAY);
            }//for
            mirror_led_on = false;
        }
        break;

    case DISABLE:
        digitalWrite(oMirror_led_pin, LOW);
        break;

    case ENABLE:
        digitalWrite(oMirror_led_pin, HIGH);
        break;

    default:
        break;
    }//switch

}//mirrorLight()
//------------------------------------------------------------------------------------------------------
void doorInControl(uint8_t state)
{
    if(state == ON)
    {
        shift_output_buff[0] |= (1 << DOR_IN_LOCK_OFFSET);   
    }
    else if(state == OFF)
    {
        shift_output_buff[0] &= ~(1 << DOR_IN_LOCK_OFFSET);
    }

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//doorInControl()
//------------------------------------------------------------------------------------------------------
void doorOutControl(uint8_t state)
{
    if(state == ON)
    {
        shift_output_buff[0] |= (1 << DOR_OUT_LOCK_OFFSET);   
    }
    else if(state == OFF)
    {
        shift_output_buff[0] &= ~(1 << DOR_OUT_LOCK_OFFSET);
    }

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//doorOutControl()
//------------------------------------------------------------------------------------------------------
void doorFairplaceControl(uint8_t state)
{
    if(state == ON)
    {
        shift_output_buff[0] |= (1 << DOR_FIREPLACE_LOCK_OFFSET);   
    }
    else if(state == OFF)
    {
        shift_output_buff[0] &= ~(1 << DOR_FIREPLACE_LOCK_OFFSET);
    }

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//doorFairplaceControl()
//------------------------------------------------------------------------------------------------------
void lightDogControl(uint8_t state)
{
    if(state == ON)
    {
        shift_output_buff[0] |= (1 << DOG_LIGHT_OFFSET);   
    }
    else if(state == OFF)
    {
        shift_output_buff[0] &= ~(1 << DOG_LIGHT_OFFSET);
    }

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lightDogControl()
//------------------------------------------------------------------------------------------------------
void lightPaintControl(uint8_t number_of_paint, uint8_t state)
{
  switch(number_of_paint)
  {
    case PAINT_1:
      shift_output_buff[0] = (state == ON) ? shift_output_buff[0] | (1 << PAINT_1_OFFSET): shift_output_buff[0] & ~(1 << PAINT_1_OFFSET);
      break;
      
    case PAINT_2:
      shift_output_buff[0] = (state == ON) ? shift_output_buff[0] | (1 << PAINT_2_OFFSET): shift_output_buff[0] & ~(1 << PAINT_2_OFFSET);
      break;
      
    case PAINT_3:
      shift_output_buff[0] = (state == ON) ? shift_output_buff[0] | (1 << PAINT_3_OFFSET): shift_output_buff[0] & ~(1 << PAINT_3_OFFSET);
      break;
      
    case PAINT_4:
      shift_output_buff[0] = (state == ON) ? shift_output_buff[0] | (1 << PAINT_4_OFFSET): shift_output_buff[0] & ~(1 << PAINT_4_OFFSET);
      break;

    default:
      break;
  }//switch

  shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lightPaintControl()
//------------------------------------------------------------------------------------------------------
void lightDementorControl(uint8_t number_of_dementor, uint8_t state)
{
  switch(number_of_dementor)
  {
    case DEMENTOR_1:
      shift_output_buff[1] = (state == ON) ? shift_output_buff[1] | (1 << DEMENTOR_1_LED_OFFSET): shift_output_buff[1] & ~(1 << DEMENTOR_1_LED_OFFSET);
      break;
      
    case DEMENTOR_2:
      shift_output_buff[1] = (state == ON) ? shift_output_buff[1] | (1 << DEMENTOR_2_LED_OFFSET): shift_output_buff[1] & ~(1 << DEMENTOR_2_LED_OFFSET);
      break;
      
    case DEMENTOR_3:
      shift_output_buff[1] = (state == ON) ? shift_output_buff[1] | (1 << DEMENTOR_3_LED_OFFSET): shift_output_buff[1] & ~(1 << DEMENTOR_3_LED_OFFSET);
      break;
      
    case DEMENTOR_4:
      shift_output_buff[1] = (state == ON) ? shift_output_buff[1] | (1 << DEMENTOR_4_LED_OFFSET): shift_output_buff[1] & ~(1 << DEMENTOR_4_LED_OFFSET);
      break;

    default:
      break;
  }//switch

  shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lightDementorControl()
//------------------------------------------------------------------------------------------------------
uint8_t getTorchState(void)
{
    uint8_t current_state;

    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    current_state = shift_input_buff[2];
    current_state &= TORCH_STATE_MASK;
    
    if(current_state == TORCH_STATE_MASK)
    {
        return SOLVED;
    }
    else
    {
        return NOT_SOLVED;
    }//if/else

}//torchState()
//------------------------------------------------------------------------------------------------------
uint8_t getKnockState(void)
{
    uint8_t current_state;

    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    current_state = shift_input_buff[2];
    current_state &= KNOCK_STATE_MASK;
    
	if(current_state == 0x00)
    {
        return RELEASED;
    }
    else
    {
        if(current_state & KNOCK_1_MASK)
            knock_1_timer = millis();

        if(current_state & KNOCK_2_MASK)  
            knock_2_timer = millis();

        if(current_state & KNOCK_3_MASK)    
            knock_3_timer = millis();

        if(current_state & KNOCK_4_MASK)   
            knock_4_timer = millis();
        
        return IN_PROGRESS;
    }

    return 99;  //somthing go wrong
}//knockState()
//------------------------------------------------------------------------------------------------------
void getPaintState(uint8_t * state_buff)
{
    uint8_t state_1, state_2, state_3, state_4;

    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    state_1 = shift_input_buff[0];
    state_1 &= PAINT_1_MASK;
    state_2 = shift_input_buff[0];
    state_2 &= PAINT_2_MASK;
    state_3 = shift_input_buff[1];
    state_3 &= PAINT_3_MASK;
    state_4 = shift_input_buff[1];
    state_4 &= PAINT_4_MASK;

    state_buff[PAINT_1] = (state_1 == PAINT_1_MASK) ? SOLVED : NOT_SOLVED;
    state_buff[PAINT_2] = (state_2 == PAINT_2_MASK) ? SOLVED : NOT_SOLVED;
    state_buff[PAINT_3] = (state_3 == PAINT_3_MASK) ? SOLVED : NOT_SOLVED;
    state_buff[PAINT_4] = (state_4 == PAINT_4_MASK) ? SOLVED : NOT_SOLVED;
  
  }//getPaintState()
//------------------------------------------------------------------------------------------------------
uint8_t getDementorsState(void)
{
    uint8_t state;
    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    state =  shift_input_buff[3] & 0x0F;

    switch(state)
    {
        case DEMENTOR_1_MASK:
            return DEMENTOR_1;
            break;

        case DEMENTOR_2_MASK:
            return DEMENTOR_2;
            break;

        case DEMENTOR_3_MASK:
            return DEMENTOR_3;
            break;

        case DEMENTOR_4_MASK:
            return DEMENTOR_4;
            break;

        default:
            return NOT_ACTIVATED;
            break;            
    }//switch

}//getDementorsState()
//------------------------------------------------------------------------------------------------------
void lightAnimation_2_room(void)
{
    room_2_light(OFF);
    delay(500);
    room_2_light(ON);
    delay(500);
    room_2_light(OFF);
    delay(300);
    room_2_light(ON);
    delay(300);
    room_2_light(OFF);
    delay(300);
    room_2_light(ON);
    delay(300);
    room_2_light(OFF);
}//lightAnimation_2_room()
//------------------------------------------------------------------------------------------------------
inline uint8_t dogGame(void)
{
    if(isDogDoorOpen() && !dog_start_flag)
    {
        lightDogControl(ON);
        playSound(DOG_SOUND);
        dog_voice_timer = millis();
        dog_start_flag = true;
    }
    if(isDogDoorOpen())
    {
        dog_timer = millis();
        dog_dor_prew_state = OPEN;
    }
    if(millis() - dog_voice_timer > dog_voice_interval)
    {
        dog_start_flag = false;
    }
    if(!isDogDoorOpen() && dog_dor_prew_state == OPEN)
    {
        dog_dor_prew_state = CLOSED;
        dog_timer = millis();
    }
    if(!isDogDoorOpen())
    {
        dog_dor_prew_state = CLOSED;
        dog_start_flag = false;
    }
    if(millis() - dog_timer > dog_interval && dog_dor_prew_state == CLOSED)
    {
        stopSound();
        lightDogControl(OFF);
        return SOLVED;
    }   

    return NOT_SOLVED; 
}//dogGame()
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

#endif //FUNCTIONS_INCLUDED