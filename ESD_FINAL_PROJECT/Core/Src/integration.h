#include "DS18b20.h"
#include "uart_init.h"
#include <stdio.h>
#include <stdlib.h>
#include "IRSensor.h"
#include "DHT11.h"
#include "accelerometer.h"
#include "push_button.h"
#include "Pressure_Sensor.h"

void init_game();
void game_state_machine();
