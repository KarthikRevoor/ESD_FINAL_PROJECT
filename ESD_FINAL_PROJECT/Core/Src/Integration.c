#include "integration.h"

typedef enum {
    IDLE,
    SENSOR_READ,
    CHECK_POKEMON_ENCOUNTER,
    SELECT_ACTION,
    BATTLE,
    CAPTURE,
    POST_BATTLE
} GameState;

// Variables for Game State Machine
GameState currentState = IDLE;

int dht11_temperature;
int dht11_humidity;
float ds18b20_temperature;
char current_pokemon[50];
#define POKEMON_COUNT 10
const char *fire_pokemon[POKEMON_COUNT] = {"Charmander", "Vulpix", "Growlithe", "Magmar", "Flareon", "Cyndaquil", "Torchic", "Chimchar", "Tepig", "Fennekin"};
const char *ice_pokemon[POKEMON_COUNT] = {"Articuno", "Jynx", "Lapras", "Delibird", "Swinub", "Sneasel", "Snorunt", "Glalie", "Spheal", "Froslass"};
const char *normal_pokemon[POKEMON_COUNT] = {"Pidgey", "Rattata", "Meowth", "Eevee", "Sentret", "Togepi", "Zigzagoon", "Bidoof", "Patrat", "Lillipup"};
const char *water_pokemon[POKEMON_COUNT] = {
    "Squirtle", "Totodile", "Mudkip", "Piplup", "Froakie",
    "Vaporeon", "Lapras", "Poliwag", "Wooper", "Marill"
};

void init_game(void) {
	IR_init();
    // Initialize sensors

	//MPU_init();
    Push_Button_Init(); // Initialize push button
    Pressure_sensor_Init();
    ADC_Config();
    MPU_init();
    // Initialize game state
    currentState = IDLE;

    // Debugging output
    uart_send_string("Game initialized. System ready!\n\r");

    // Additional setup for game variables (if needed)
    // Example: Initialize Pokémon health, score, or other gameplay elements
}

void spawn_pokemon_from_array(const char *pokemon_array[]) {
    uint32_t index = rand() % POKEMON_COUNT;
    snprintf(current_pokemon, sizeof(current_pokemon), "%s", pokemon_array[index]); // Store the Pokémon name
    char buffer[100];
    sprintf(buffer, "A wild %s appears!\n", pokemon_array[index]);
    uart_send_string(buffer);
}

void game_state_machine() {
    switch (currentState) {
        case IDLE:
            // Wait for IR sensor trigger
        	ir_triggered=0;
            if (IR_is_triggered()) {
                uart_send_string("Pokemon detected! Moving to SENSOR_READ.\n\r");
                currentState = SENSOR_READ;
            }
            break;

        case SENSOR_READ: {
            DHT_DataTypedef DHT_Data;
            DHT_GetData(&DHT_Data); // Read DHT11 data

            // Only use humidity from DHT11
            dht11_humidity = (int)DHT_Data.Humidity;

            // Read temperature from DS18B20
            //temp_sensor_check();
            ds18b20_temperature = temperature;

            // Debug output for humidity and temperature
            char buffer[100];
            sprintf(buffer, "DHT11 Humidity: %d%%\n\r", dht11_humidity);
            uart_send_string(buffer);

            sprintf(buffer, "DS18B20 Temperature: %.2f°C\n\r", ds18b20_temperature);
            uart_send_string(buffer);

            currentState = CHECK_POKEMON_ENCOUNTER;
            break;
        }

        case CHECK_POKEMON_ENCOUNTER:
            // Determine Pokémon to spawn based on sensor data
            if (ds18b20_temperature > 25.0) {
                // Spawn a Fire-type Pokémon for high temperature
                spawn_pokemon_from_array(fire_pokemon);
            } else if (dht11_humidity > 60) {
                // Spawn a Water-type Pokémon for high humidity
                spawn_pokemon_from_array(water_pokemon);
            } else {
                // Spawn a Normal-type Pokémon by default
                spawn_pokemon_from_array(normal_pokemon);
            }
            uart_send_string("\rSelect an action: Press button for Battle or Capture.\n\r");
            Reset_Button_State(); // Reset button state for selection
            currentState = SELECT_ACTION;
            break;

        case SELECT_ACTION:

                    // Check button states
                    if (Get_Battle_Button_State()) { // PC13 for Battle
                        uart_send_string("Battle selected!\n\r");
                        currentState = BATTLE;
                    } else if (Get_Capture_Button_State()) { // PC14 for Capture
                        uart_send_string("Capture selected!\n\r");
                        currentState = CAPTURE;
                    }
                    break;

        case BATTLE: {
            uart_send_string("Battle initiated! Shake the board to attack.\n\r");
            // Countdown before shaking starts
            for (int countdown = 3; countdown > 0; countdown--) {
                char buffer[50];
                sprintf(buffer, "Get ready! Shake starts in: %d\n\r", countdown);
                uart_send_string(buffer);
                HAL_Delay(1000); // 1-second delay for countdown
            }

            uart_send_string("Shake now!\n\r");
            // Detect shakes for 3 seconds
            const uint32_t duration = 3000; // 3 seconds
            const int threshold = 2000;    // Acceleration threshold
            uint32_t shake_count = detect_shakes(duration, threshold);

            // Calculate damage
            int base_damage = 10;
            int total_damage = shake_count * base_damage;

            // Debug output for shake count and damage
            char buffer[100];
            if (shake_count > 0) {
                sprintf(buffer, "Shake count: %lu, Total damage: %d\n\r", shake_count, total_damage);
            } else {
                sprintf(buffer, "No shakes detected. Total damage: %d\n\r", total_damage);
            }
            uart_send_string(buffer);

            uart_send_string("Returning to action selection.\n\r");
            uart_send_string("\rSelect an action: Press button for Battle or Capture.\n\r");
            Reset_Button_State(); // Reset button state for new selection
            currentState = SELECT_ACTION; // Return to action selection
            break;
        }
        case CAPTURE: {
            uart_send_string("Apply pressure to capture the Pokémon...\n\r");

            // Countdown for pressing the button
            for (int countdown = 3; countdown > 0; countdown--) {
                char buffer[50];
                sprintf(buffer, "Press in: %d\n\r", countdown);
                uart_send_string(buffer);
                HAL_Delay(1000); // 1-second delay for countdown
            }

            uart_send_string("Apply pressure now for 2 seconds...\n\r");

            // Read ADC value and calculate average over 2 seconds
            uint32_t start_time = HAL_GetTick();
            uint32_t total_value = 0;
            uint32_t samples = 0;

            while ((HAL_GetTick() - start_time) < 2000) { // 2-second window
                ADC_Read(); // Update global adcValue
                total_value += adcValue;
                samples++;
                HAL_Delay(100); // Sample every 100ms
            }

            uint32_t avg_value = total_value / samples;

            // Check if average pressure is within threshold
            char buffer[100];
            if (avg_value > 200 && avg_value < 800) {
                sprintf(buffer, "Success! Pokémon captured with average pressure: %lu\n\r", avg_value);
                uart_send_string(buffer);
                sprintf(buffer, "%s is captured!!!\n\r", current_pokemon); // Use current_pokemon
                uart_send_string(buffer);
                currentState = POST_BATTLE;
            } else {
                sprintf(buffer, "Failed! Pokémon broke out. Average pressure: %lu\n\r", avg_value);
                uart_send_string(buffer);
                uart_send_string("Returning to action selection.\n\r");
                uart_send_string("\rSelect an action: Press button for Battle or Capture.\n\r");
                Reset_Button_State(); // Reset button state
                currentState = SELECT_ACTION; // Return to SELECT_ACTION after capture attempt
            }


            break;
        }

               case POST_BATTLE:
                   uart_send_string("Battle concluded. Returning to IDLE.\n\r");
                   currentState = IDLE;
                   break;
           }
}

