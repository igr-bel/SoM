/*
* 
*
* 
*/

#include "main.h"
#include "functions.h"
#include "start_inits.h"
#include "mqtt_parser.h"

//======================================================================================================
//------------------------------------------------------------------------------------------------------
//                                        SETUP
//------------------------------------------------------------------------------------------------------
void setup()
{
	#ifdef DBG
		Serial.begin(9600);
		Serial.println("___DBG___");
		Serial.print("[DBG] start init ... ");
	#endif

	//------------------------------------------------------ GPIO
	pinMode(oRoom_1_light_pin, OUTPUT);
	pinMode(oRoom_2_light_pin, OUTPUT);
	pinMode(oUV_light_pin, OUTPUT);
	pinMode(oFog_machine_en_pin, OUTPUT);
	pinMode(oTorh_led_pin, OUTPUT);
	pinMode(iDog_door_pin, INPUT);
	pinMode(iFireplace_pin, INPUT);
	pinMode(oMirror_led_pin, OUTPUT);
	pinMode(oGlass_paint_pin, OUTPUT);

	// shift registers init
	pinMode(data_pin_in, INPUT_PULLUP);
	pinMode(clock_pin_in, OUTPUT);
	pinMode(latch_pin_in, OUTPUT);

	pinMode(data_pin_out, OUTPUT);
	pinMode(clock_pin_out, OUTPUT);
	pinMode(latch_pin_out, OUTPUT);

	#ifdef output_enable
		pinMode(output_enable, OUTPUT);
	#endif
	outputEnable();

	//------------------------------------------------------ RAND INITS
	randomSeed(analogRead(0));
  
	//------------------------------------------------------ MP3 INITS
	Serial2.begin(9600);
	myDFPlayer.begin(Serial2);
	// myDFPlayer.setTimeOut(500);
	myDFPlayer.volume(VOLUME);

	//------------------------------------------------------ GAME INITS
	start_variables_init();

	//------------------------------------------------------ ETHERNET
	// setup mqtt client
	Ethernet.init(53);
	Ethernet.begin(mac, ip, myDns);
	delay(1500);

	//------------------------------------------------------ check for Ethernet hardware present
	if (Ethernet.hardwareStatus() == EthernetNoHardware) 
	{
		Serial.println(" ");
		Serial.println("Ethernet shield was not found.");
		while (true) 
		{
			delay(1); // empty loop
		}
	}
	while (Ethernet.linkStatus() == LinkOFF) 
	{
		Serial.println("Ethernet cable is not connected.");
		delay(500);
	}
	// give the Ethernet shield a second to initialize:
	delay(1000);
	Serial.println("connecting...");
  
	//------------------------------------------------------ setup mqtt client
	mqttClient.setClient(ethClient);
	mqttClient.setServer(server, 1883);

	while(!mqttClient.connect(CLIENT_ID))
	{
		Serial.println("Looks like the server connection failed...");
		Serial.println(mqttClient.state());
		delay(1000);
	}
	Serial.println("[DBG] Connection has been established, well done");

	mqttClient.setCallback(subscribeReceive);

	mqttClient.subscribe(sub_topic.c_str());
	mqttClient.subscribe(brdcst_topic.c_str());

	//------------------------------------------------------ reset web interface
	publishData(SOLVE_CLEAR);

	#ifdef DBG
		Serial.println("[DBG] done!");
		Serial.println("[DBG] start loop() ... ");
	#endif
}//setup()
//------------------------------------------------------------------------------------------------------
//                                        LOOP
//------------------------------------------------------------------------------------------------------
void loop()
{
	mqttClient.loop();

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
			//reset web interface block
			publishData(SOLVE_CLEAR);
			break;
	    
		case IDLE_STATE:
			if(game_start_flag == true)
			{
				props_is_ready();
				current_state = GAME_1_STATE;
				doorInControl(ON);
			}
			break;

		case GAME_1_STATE:
			//first part of game - tirches and knocks
			if(getTorchState() == SOLVED)
			{
				current_state = TORCH_SOLVED_STATE;
			}//check torches

			if((millis() - torch_timer) > torch_interval)
			{
				mirrorLight(OFF);
			}
	    
			//check knocks
			if(!knock_win_flag)
			{
				knock_state = getKnockState(); 
				if (knock_state == IN_PROGRESS)
				{
					if(!knock_started_flag)
					{
						knock_started_flag = true;
						knock_general_timer = millis();
						knock_1_timer = knock_2_timer = knock_3_timer = knock_4_timer = millis();
					}
				}
			}//check cnock if it's not yet win

	    	if(((millis() - knock_general_timer) > knock_general_interval) && knock_started_flag && !knock_win_flag)
			{
				//check timers of all cnocks
				if( ((millis() - knock_1_timer) < knock_separate_interval)
					&& ((millis() - knock_2_timer) < knock_separate_interval)
					&& ((millis() - knock_3_timer) < knock_separate_interval)
					&& ((millis() - knock_4_timer) < knock_separate_interval) )
				{
					current_state = KNOCK_SOLVED_STATE;
				}
				else
				{
					//send "error" / turn-on sound
					publishData(KNOCK_ERROR);

					knock_started_flag = false; 
				}
			}//if general timer is up

			if((millis() - glass_paint_timer) > glass_paint_interval && glass_paint_led_on)
			{
				glassPaintLight(OFF);
				glass_paint_hide_timer = millis();
				glass_paint_led_on = false;
			}

			//if knocks win -> show glass_paint periodic
			if(knock_win_flag)
			{
				if(millis() - glass_paint_hide_timer > glass_paint_hide_interval && !glass_paint_led_on)
				{
					glassPaintLight(ON);
					glass_paint_timer = millis();
					glass_paint_led_on = true;
				}
			}//if knocks win just show glass_paint

			if(knock_win_flag && torch_win_flag)
			{
				current_state = GAME_1_PAINTS_STATE;  
			}

			break;

		case GAME_1_PAINTS_STATE:
			//animation of glass_paint
			if((millis() - glass_paint_timer) > glass_paint_interval && glass_paint_led_on)
			{
				glassPaintLight(OFF);
				glass_paint_hide_timer = millis();
				glass_paint_led_on = false;
			}
			if(millis() - glass_paint_hide_timer > glass_paint_hide_interval && !glass_paint_led_on)
			{
				glassPaintLight(ON);
				glass_paint_timer = millis();
				glass_paint_led_on = true;
			}

			//animation of mirror
			if(getTorchState() == SOLVED)
			{
				mirrorLight(ON);
				torch_timer = millis();
			}
			if((millis() - torch_timer) > torch_interval)
			{
				mirrorLight(OFF);
			}
	    
			//check the paints
			getPaintState(paint_states);
			if(paint_states[PAINT_1] == SOLVED && !paint_1_win_flag)
			{
				publishData(PAINT_1_WIN); 
				lightPaintControl(PAINT_1, OFF);
				paint_1_win_flag = true; 
			}
	    
			if(paint_states[PAINT_2] == SOLVED && !paint_2_win_flag)
			{
				publishData(PAINT_2_WIN); 
				lightPaintControl(PAINT_2, OFF);
				paint_2_win_flag = true; 
			}

			if(paint_states[PAINT_3] == SOLVED && !paint_3_win_flag)
			{
				publishData(PAINT_3_WIN); 
				lightPaintControl(PAINT_3, OFF);
				paint_3_win_flag = true; 
			}

			if(paint_states[PAINT_4] == SOLVED && !paint_4_win_flag)
			{
				publishData(PAINT_4_WIN); 
				lightPaintControl(PAINT_4, OFF);
				paint_4_win_flag = true;
			}

			//check the first room win
			if(  paint_1_win_flag && paint_2_win_flag
				&& paint_3_win_flag && paint_4_win_flag 
				&& !paints_win_flag)
			{
				doorFairplaceControl(OFF); 
				current_state = PAINTS_SOLVED_STATE;
			}

			if(paints_win_flag && chest_win_flag)
			{
				if(isFireplaceDoorOpen())
				{
					fog_control(ENABLE);
					current_state = GAME_2_DOG_STATE;
				}
			}

			break;

		case GAME_2_DOG_STATE:
			if(dogGame() == SOLVED)
			{
				current_state = DOG_SOLVED_STATE;
			}
			break;

		case ALTAR_GAME_STATE:
			dogGame();
			if( fire_win_flag && air_win_flag 
				&& water_win_flag && earth_win_flag )
			{
				current_state = LIGHT_ANIMATION_START_STATE;
			}
			break;

		case LIGHT_ANIMATION_START_STATE:
		if(start_fin_animation_flag)
			{
				//light animation from node-red control
			}
			break;

		case LIGHT_ANIMATION_STOP_STATE:
			room_2_light(OFF);
			UV_light(ON);
			current_state = DEMENTORS_GAME_STATE;
			break;

		case DEMENTORS_GAME_STATE:
			do
			{
				current_dementor = random(4);
			} while(prew_dementor == current_dementor);
			prew_dementor = current_dementor;
			dementor_prew_win_counter = dementor_win_counter;
			lightDementorControl(current_dementor, ON);
			dementor_start_time = millis();

			do
			{
				active_dementor_sensor = getDementorsState();

				if (active_dementor_sensor == current_dementor)
				{
					dementor_err_counter = 0;
					dementor_counter_game_time = DEMENTOR_MIN_GAME_TIME;
					dementor_win_counter++;
					publishData(DEMENTORS_CNT_0 | dementor_win_counter);
					if(dementor_win_counter == DEMENTOR_WIN_COUNT)
					{
						current_state = DEMENTORS_SOLVED_STATE;
						break; 
					}//if_win
					break;
				}//if right sensor activated
				else if(active_dementor_sensor != NOT_ACTIVATED)
				{
					dementor_err_counter++;
					dementor_prew_win_counter = 99; //for not increase error twice
					if(dementor_err_counter >= DEMENTOR_ERR_COUNT)
					{
					dementor_counter_game_time = (dementor_counter_game_time < DEMENTOR_MAX_GAME_TIME) ? dementor_counter_game_time + DEMENTOR_TIME_INTERVAL: DEMENTOR_MAX_GAME_TIME;
					}
					break;
				}//else - error
			} while (millis() - dementor_start_time < dementor_counter_game_time);
	   
			lightDementorControl(current_dementor, OFF);
			delay(DEMENTOR_GAME_DELAY);
			while(current_dementor == getDementorsState())
			{
				delay(10);
			}

			//if no one sensor is activated
			if(dementor_win_counter == dementor_prew_win_counter)
			{
				dementor_err_counter++;
				if(dementor_err_counter >= DEMENTOR_ERR_COUNT)
				{
					dementor_counter_game_time = (dementor_counter_game_time < DEMENTOR_MAX_GAME_TIME) ? dementor_counter_game_time + DEMENTOR_TIME_INTERVAL: DEMENTOR_MAX_GAME_TIME;
				}
			}

			break;

		// when props is solved
		case TORCH_SOLVED_STATE:
			if(millis() - torch_timer > torch_interval)
			{
				//send win_packet / turn on sound
				publishData(TORCH_WIN);
				//turn off mirror light
				mirrorLight(ON);
				//start timer
				torch_timer = millis();
				torch_win_flag = true;
			}
			current_state = GAME_1_STATE;
			break;

		case KNOCK_SOLVED_STATE:
			//send win_packet
			publishData(KNOCK_WIN);
			//turn off glass_paint
			glassPaintLight(ON);
			//start timer
			glass_paint_timer = millis();
			knock_win_flag = true;

			current_state = GAME_1_STATE;
			break;

		case PAINTS_SOLVED_STATE:
			publishData(PAINTS_WIN);
			mirrorLight(DISABLE);
			glassPaintLight(DISABLE);
			paints_win_flag = true;

			current_state = GAME_1_PAINTS_STATE;
			break;

		case DOG_SOLVED_STATE:
			publishData(DOG_WIN);
			dog_win_flag = true;
			lightDogControl(OFF);
			delay(500);
			stopSound();
			room_2_light(ON);
			glassPaintLight(DISABLE);
			mirrorLight(DISABLE);
			current_state = ALTAR_GAME_STATE;
			break;
	    
		case DEMENTORS_SOLVED_STATE:
			dementors_win_flag = true;
			publishData(DEMENTORS_WIN);
			current_state = WIN_STATE;
			break;

		case WIN_STATE:
			if(win_flag)
			{
				current_state = WIN_IDLE_STATE;
				break;
			}
			publishData(WIN_PAYLOAD);
			win();
			win_flag = true;
			current_state = WIN_IDLE_STATE;
			break;

		case WIN_IDLE_STATE:

			break;


		case FINAL_STATE:
			current_state = WIN_IDLE_STATE;
			break;

		default:
			break;
	}//switch STATE MACHINE

	mqttClient.loop();

	//------------------------------------------------------------------------------------
	//HINTS
	switch(hint_stage)
	{
		case TORCH_HINT:
		hint_stage = (torch_win_flag) ? KNOCKS_HINT : TORCH_HINT;
		break;

		case KNOCKS_HINT:
		hint_stage = (knock_win_flag) ? CHEST_HINT : KNOCKS_HINT;
		break;

		case CHEST_HINT:
		hint_stage = (chest_win_flag) ? PAINTS_HINT : CHEST_HINT;
		break;

		case PAINTS_HINT:
		hint_stage = (paints_win_flag) ? DOG_HINT : PAINTS_HINT;
		break;

		case DOG_HINT:
		hint_stage = (dog_win_flag) ? FIRE_HINT : DOG_HINT;
		break;

		case FIRE_HINT:
		hint_stage = (fire_win_flag) ? AIR_HINT : FIRE_HINT;
		break;
		      
		case AIR_HINT:
		hint_stage = (air_win_flag) ? WATER_HINT : AIR_HINT;
		break;

		case WATER_HINT:
		hint_stage = (water_win_flag) ? EARTH_HINT : WATER_HINT;
		break;
		      
		case EARTH_HINT:
		hint_stage = (earth_win_flag) ? DEMENTORS_HINT : EARTH_HINT;
		break;

		case DEMENTORS_HINT:
		hint_stage = (dementors_win_flag) ? NULL_HINT : DEMENTORS_HINT;
		break;
	}//switch

	// if(millis() - hint_send_timer > hint_send_interval)
	if(prew_hint_stage != hint_stage)
	{
		publishData(hint_stage);
		prew_hint_stage = hint_stage;
		// hint_send_timer = millis();
	}

	//------------------------------------------------------------------------------------

	//------------------------------------------- <<< ALGORITHM
  
  
	//------------------------------------------- COMMUNICATION >>>
	mqttClient.loop();

	//parse new data
	if(new_data_recieved)
	{
		new_data_recieved = false;

		//string to command
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
		Serial.println("[ERR] Reconnect ... ");
		while(!mqttClient.connect(CLIENT_ID))
		{
			Ethernet.init(53);
			Ethernet.begin(mac, ip, myDns);
			delay(1500);

			// check for Ethernet hardware present
			if(Ethernet.hardwareStatus() == EthernetNoHardware) 
			{
				Serial.println(" ");
				Serial.println("[INF] Ethernet shield was not found.");

				while(true) 
				{
					delay(1); // empty loop
				}//while(true)
			}//if
			while(Ethernet.linkStatus() == LinkOFF) 
			{
				Serial.println("[INF] Ethernet cable is not connected.");
				delay(500);
			}//while
			// give the Ethernet shield a second to initialize:
			delay(1000);
		}//while

		mqttClient.setClient(ethClient);
		mqttClient.setServer(server, 1883);

		Serial.println(" ");
		Serial.println("[INF] connecting...");
		while(!mqttClient.connect(CLIENT_ID))
		{
			Serial.println("[INF] Looks like the server connection failed...");
			Serial.println(mqttClient.state());
			delay(1000);
		}
		Serial.println("[INF] Connection has been established, well done!");

		mqttClient.setCallback(subscribeReceive);

		mqttClient.subscribe(sub_topic.c_str());
		mqttClient.subscribe(brdcst_topic.c_str());

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

//======================================================================================================