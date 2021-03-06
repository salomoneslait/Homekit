// borrar memoria py -3.10 -m esptool --chip esp8266 --port COM8 erase_flash
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

void setup() {
	Serial.begin(115200);
	wifi_connect(); // in wifi_info.h
	//homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
	my_homekit_setup();
}

void loop() {
	my_homekit_loop();
	delay(10);
}

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_switch_on;

static uint32_t next_heap_millis = 0;

#define S1 2
#define R1 0
//#define PIN_SWITCH 2

int switch_ON_Flag_previous_I = 0;

//Called when the switch value is changed by iOS Home APP
void cha_switch_on_setter(const homekit_value_t value) {
	bool on = value.bool_value;
	cha_switch_on.value.bool_value = on;	//sync the value
	LOG_D("Switch: %s", on ? "ON" : "OFF");
	digitalWrite(R1, on ? LOW : HIGH);
}

void my_homekit_setup() {
	pinMode(R1, OUTPUT);
	digitalWrite(R1, HIGH);
 
	cha_switch_on.setter = cha_switch_on_setter;
	arduino_homekit_setup(&config);

}

void my_homekit_loop() {
	arduino_homekit_loop();
  //switch_feedback_function();
	const uint32_t t = millis();
	if (t > next_heap_millis) {
		// show heap info every 5 seconds
		next_heap_millis = t + 5 * 1000;
		LOG_D("Free heap: %d, HomeKit clients: %d",
				ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

	}
}
void switch_feedback_function(){
  if(digitalRead(S1) == LOW){
    if(switch_ON_Flag_previous_I == 0){
      digitalWrite(R1,LOW);
      bool switch_is_on = 1;
      cha_switch_on.value.bool_value = switch_is_on;
      homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
      switch_ON_Flag_previous_I = 1;
    }
  }
  if(digitalRead(S1) == HIGH){
    if(switch_ON_Flag_previous_I == 1){
      digitalWrite(R1,HIGH);
      bool switch_is_on = 0;
      cha_switch_on.value.bool_value = switch_is_on;
      homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
      switch_ON_Flag_previous_I = 0;
    }
  }
  
}
