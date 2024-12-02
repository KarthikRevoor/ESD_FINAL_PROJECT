#include "integration.h"

typedef enum {
    IDLE,
    SENSOR_READ,
    CHECK_POKEMON_ENCOUNTER,
    BATTLE,
    POST_BATTLE
} GameState;

// Variables for Game State Machine
GameState currentState = IDLE;

int dht11_temperature;
int dht11_humidity;
float ds18b20_temperature;

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


    // Initialize game state
    currentState = IDLE;

    // Debugging output
    uart_send_string("Game initialized. System ready!\n");
    //MPU_init();

    // Additional setup for game variables (if needed)
    // Example: Initialize Pokémon health, score, or other gameplay elements
}

void spawn_pokemon_from_array(const char *pokemon_array[]) {
    uint32_t index = rand() % POKEMON_COUNT;
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
                uart_send_string("Object detected! Moving to SENSOR_READ.\n\r");
                currentState = SENSOR_READ;
            }
            break;

        case SENSOR_READ: {
            DHT_DataTypedef DHT_Data;
            DHT_GetData(&DHT_Data); // Read DHT11 data

            // Only use humidity from DHT11
            dht11_humidity = (int)DHT_Data.Humidity;

            // Read temperature from DS18B20
            //temp_sensor_check(); // Reads DS18B20 temperature
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
            currentState = BATTLE;
            break;


        case BATTLE: {
            uart_send_string("Battle initiated! Shake the board to attack.\n");

            // Detect shakes for 3 seconds
            const uint32_t duration = 3000; // 3 seconds
            const int threshold = 2000;    // Acceleration threshold
            uint32_t shake_count = detect_shakes(duration, threshold);

            // Calculate damage
            int base_damage = 10;
            int total_damage = shake_count * base_damage;

            // Output result
            char buffer[100];
            sprintf(buffer, "Shake count: %lu, Total damage: %d\n", shake_count, total_damage);
            uart_send_string(buffer);

            uart_send_string("Battle concluded. Returning to IDLE.\n");
            currentState = POST_BATTLE;
            break;
        }

        case POST_BATTLE:
            uart_send_string("Battle concluded. Returning to IDLE.\n\r");
            currentState = IDLE;
            break;
    }
}
