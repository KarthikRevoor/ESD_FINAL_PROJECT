#include "integration.h"
#include "lcd_functions.h"
#include "lcd.h"
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
    ILI9341_Init(); // Initialize the LCD
    SD_Init();
    ILI9341_FillScreen(0x0000); // Clear the screen with black color
    currentState = IDLE;

    // Debugging output
    uart_send_string("Game initialized. System ready!\n\r");

    // Additional setup for game variables (if needed)
    // Example: Initialize Pokémon health, score, or other gameplay elements
}

const char *spawn_pokemon_from_array(const char *pokemon_array[]) {
    uint32_t index = rand() % POKEMON_COUNT; // Generate a random index
    char buffer[100];
    sprintf(buffer, "A wild %s appears!", pokemon_array[index]);
    uart_send_string(buffer);
    uart_send_string("\n\r");

    // Display on LCD

    return pokemon_array[index]; // Return the name of the Pokémon
}

void game_state_machine() {
    switch (currentState) {
        case IDLE:
            //DrawString(10, 10, "Welcome to Pokemon Adventure!", 0xFFFF, 0x0000,2);
            //DrawString(10, 30, "Wave your hand to start!", 0xFFFF, 0x0000,2);
            // Wait for IR sensor trigger
        	ir_triggered=0;
            if (IR_is_triggered()) {
                uart_send_string("Pokemon detected! Moving to SENSOR_READ.\n\r");
                currentState = SENSOR_READ;
                ILI9341_FillScreen(0x0000);
            }
            break;

        case SENSOR_READ: {
        	ILI9341_FillRect(10, 30, 100, 50, 0x07E0);
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
            DrawString(10, 30, buffer, 0xFFFF, 0x0000,1);
            //uart_send_string(buffer);

            sprintf(buffer, "DS18B20 Temperature: %.2f°C\n\r", ds18b20_temperature);
            DrawString(10, 50, buffer, 0xFFFF, 0x0000,1);
            //uart_send_string(buffer);

            currentState = CHECK_POKEMON_ENCOUNTER;
            break;
        }

        case CHECK_POKEMON_ENCOUNTER: {
            FlashScreen(3, 50); // Flash to indicate encounter transition
            ILI9341_FillScreen(0x0000);
            ILI9341_FillRect(0, 0, 320, 40, 0x001F); // Blue background for heading
            DrawString(10, 10, "Pokemon Encounter!", 0xFFFF, 0x001F, 2); // White text on blue background

            // Determine Pokémon to spawn
            const char *pokemon_name = NULL;
            if (ds18b20_temperature > 25.0) {
                pokemon_name = spawn_pokemon_from_array(fire_pokemon);
            } else if (dht11_humidity > 60) {
                pokemon_name = spawn_pokemon_from_array(water_pokemon);
            } else {
                pokemon_name = spawn_pokemon_from_array(normal_pokemon);
            }

            // Display the Pokémon name
            char buffer[100];

            // Display "A wild" on the first line
            DrawString(10, 60, "A wild", 0xFFFF, 0x0000, 2); // White text

            // Display the Pokémon name on the second line
            sprintf(buffer, "%s", pokemon_name);
            DrawString(10, 90, buffer, 0xFFFF, 0x0000, 2); // White text for the Pokémon name

            // Display "appears" on the same line as the Pokémon name
            DrawString(10 + (strlen(buffer) * 12) + 10, 90, "appears!", 0xFFFF, 0x0000, 2); // Adjust position based on name length

            // Send the Pokémon name via UART for debugging
            uart_send_string("A wild ");
            uart_send_string(buffer);
            uart_send_string(" appears!\n\r");

            // Prompt user to select an action
            DrawString(10, 120, "Select an action:", 0x07E0, 0x0000, 2); // Green text
            DrawString(10, 160, "PC13 - Battle", 0xF800, 0x0000, 2); // Red text for Battle
            DrawString(10, 200, "PC14 - Capture", 0x07E0, 0x0000, 2); // Green text for Capture

            uart_send_string("\rSelect an action: Press button for Battle or Capture.\n\r");
            Reset_Button_State(); // Reset button state for selection
            currentState = SELECT_ACTION;
            break;
        }
        case SELECT_ACTION: {
            // Wait for button press
            if (Get_Battle_Button_State()) { // PC13 for Battle
                uart_send_string("Battle selected!\n\r");

                ILI9341_FillScreen(0x0000); // Clear the screen
                ILI9341_FillRect(0, 0, 320, 40, 0xF800); // Red background for Battle heading
                DrawString(10, 10, "Battle Mode!", 0xFFFF, 0xF800, 3); // White text on red background

                currentState = BATTLE; // Transition to Battle state
            } else if (Get_Capture_Button_State()) { // PC14 for Capture
                uart_send_string("Capture selected!\n\r");

                ILI9341_FillScreen(0x0000); // Clear the screen
                ILI9341_FillRect(0, 0, 320, 40, 0x001F); // Blue background for Capture heading
                DrawString(10, 10, "Capture Mode!", 0xFFFF, 0x001F, 3); // White text on blue background

                currentState = CAPTURE; // Transition to Capture state
            }
            break;
        }


        case BATTLE: {

            uart_send_string("Battle initiated! Shake the board to attack.\n\r");
            DrawString(10, 60, "Shake to attack!", 0x07E0, 0x0000, 2); // Green text, size 2

            // Countdown before shaking
            for (int countdown = 3; countdown > 0; countdown--) {
                char buffer[50];
                sprintf(buffer, "Prepare to shake in: %d\n\r", countdown);
                uart_send_string(buffer);

                char lcd_buffer[20];
                sprintf(lcd_buffer, "Shake in: %d", countdown);
                DrawString(10, 100, lcd_buffer, 0xFFFF, 0x0000, 2); // White text
                HAL_Delay(1000); // 1-second delay
            }

            uart_send_string("Shake the board now!\n\r");
            DrawString(10, 150, "Attack Progress:", 0xFFFF, 0x0000, 2); // Display attack progress on LCD

            const uint32_t duration = 3000; // Duration for shaking (3 seconds)
            const int threshold = 2000;    // Threshold for shake detection
            const int max_shakes = 70;    // Maximum allowed shakes for 100% health
            uint32_t start_time = HAL_GetTick();
            uint32_t shake_count = 0;

            // Progress bar and shake detection loop
            while ((HAL_GetTick() - start_time) < duration) {
                uint32_t elapsed = HAL_GetTick() - start_time;
                int percentage = (elapsed * 100) / duration;

                // Update progress bar dynamically
                progress_bar(10, 200, 220, 20, 0x07E0, 0x0000, percentage); // Green progress bar

                // Detect shakes
                int32_t magnitude = MPU_read();
                if (magnitude > threshold && shake_count < max_shakes) {
                    shake_count++;
                }

                HAL_Delay(50); // Small delay for smoother updates
            }

            // Calculate damage
            int base_damage = 10;
            int total_damage = shake_count * base_damage;

            // Calculate health percentage
            int max_health = max_shakes * base_damage; // Maximum possible damage
            int health_percentage = 100 - ((total_damage * 100) / max_health); // Remaining health

            // Determine health bar color based on health percentage
            uint16_t health_color;
            if (health_percentage < 40) {
                health_color = 0xF800; // Red for health < 40%
            } else if (health_percentage < 75) {
                health_color = 0xFFE0; // Yellow for health between 40% and 75%
            } else {
                health_color = 0x07E0; // Green for health >= 75%
            }

            // Clear the screen and move to results
            ILI9341_FillScreen(0x0000); // Clear screen for results display

            // Display results on the new screen
            uart_send_string("Battle complete! Displaying results...\n\r");
            DrawString(10, 20, "Battle Results:", 0xFFE0, 0x0000, 2); // Yellow text

            // Display shake count
            char buffer[50];
            sprintf(buffer, "Shakes: %lu", shake_count);
            DrawString(10, 60, buffer, 0xFFFF, 0x0000, 2); // White text for shake count

            // Display damage on the next line
            sprintf(buffer, "Damage: %d", total_damage);
            DrawString(10, 100, buffer, 0xFFFF, 0x0000, 2); // White text for damage

            // Display health bar
            DrawString(10, 140, "Health Bar:", 0xFFFF, 0x0000, 2); // White text for health bar
            ILI9341_DrawRect(10, 170, 220, 20, 0xFFFF); // Outline of health bar
            uint16_t health_bar_width = (health_percentage * 220) / 100; // Scale health bar width
            ILI9341_FillRect(10, 170, health_bar_width, 20, health_color); // Fill health bar

            // Display health percentage
            sprintf(buffer, "Health: %d%%", health_percentage);
            DrawString(10, 200, buffer, 0xFFFF, 0x0000, 2); // White text for health percentage

            // Return to action selection
            uart_send_string("Returning to action selection.\n\r");
            DrawString(10, 240, "Select Action:", 0x07E0, 0x0000, 1); // Green text
            DrawString(10, 260, "PC13 - Battle", 0xF800, 0x0000, 2); // Red text for Battle
            DrawString(10, 280, "PC14 - Capture", 0x07E0, 0x0000, 2); // Green text for Capture
            Reset_Button_State(); // Reset button state for new selection
            currentState = SELECT_ACTION; // Return to action selection
            break;
        }


        case CAPTURE: {

            uart_send_string("Capture initiated! Apply pressure to capture the Pokemon.\n\r");

            // Countdown before applying pressure
            char buffer[50];
            DrawString(10, 50, "Press in: ", 0xFFFF, 0x0000, 2); // Static part of the countdown message
            for (int countdown = 3; countdown > 0; countdown--) {
                // Clear only the number area
                ILI9341_FillRect(110, 50, 40, 20, 0x0000); // Clear just the area for the countdown number

                // Draw the countdown number
                sprintf(buffer, " %d", countdown);
                DrawString(110, 50, buffer, 0xFFFF, 0x0000, 2); // White text for the number

                // Debug output
                sprintf(buffer, "Press in: %d", countdown);
                uart_send_string(buffer);
                uart_send_string("\n\r");

                HAL_Delay(1000); // 1-second delay
            }

            // Prompt user to apply pressure
            uart_send_string("Apply pressure now for 2 seconds...\n\r");
            ILI9341_FillRect(10, 100, 300, 40, 0x0000); // Clear prompt area
            DrawString(10, 100, "Apply pressure now!", 0xFFFF, 0x0000, 2); // White text

            // Measure pressure over 2 seconds with progress bar
            uint32_t start_time = HAL_GetTick();
            uint32_t total_pressure = 0;
            uint32_t sample_count = 0;

            uint16_t progress_x = 10;
            uint16_t progress_y = 140;
            uint16_t progress_width = 300;
            uint16_t progress_height = 20;
            uint16_t progress_color = 0x07E0; // Green

            while ((HAL_GetTick() - start_time) < 2000) { // 2 seconds
                ADC_Read(); // Update global adcValue
                total_pressure += adcValue;
                sample_count++;

                // Update progress bar
                uint32_t elapsed_time = HAL_GetTick() - start_time;
                uint16_t progress_fill = (elapsed_time * progress_width) / 2000; // Proportional fill
                ILI9341_DrawRect(progress_x, progress_y, progress_width, progress_height, 0xFFFF); // Outline
                ILI9341_FillRect(progress_x, progress_y, progress_fill, progress_height, progress_color); // Fill

                HAL_Delay(100); // Sampling delay
            }

            // Calculate average pressure
            uint32_t avg_pressure = total_pressure / sample_count;

            // Clear screen and display results
            ILI9341_FillScreen(0x0000);
            ILI9341_FillRect(0, 0, 320, 40, 0x001F); // Blue background for the heading
            DrawString(10, 10, "Capture Results:", 0xFFFF, 0x001F, 2); // White text on blue background

            if (avg_pressure > 200 && avg_pressure < 800) {
              	sprintf(buffer, "Success!");
                	DrawString(10, 50, buffer, 0x07E0, 0x0000, 2); // Red text for failure

                	sprintf(buffer, "Pressure: %lu", avg_pressure);
                	DrawString(10, 120, buffer, 0xFFFF, 0x0000, 2); // White text for average pressure
                	sprintf(buffer, "Pokemon Captured!");
                	DrawString(10, 80, buffer, 0x07E0, 0x0000, 2); // Red text for the second line

                HAL_Delay(2000); // Pause for user to view results

                currentState = POST_BATTLE; // Transition to POST_BATTLE state after successful capture
            } else {
            	sprintf(buffer, "Failed!");
            	DrawString(10, 50, buffer, 0xF800, 0x0000, 2); // Red text for failure

            	sprintf(buffer, "Pressure: %lu", avg_pressure);
            	DrawString(10, 120, buffer, 0xFFFF, 0x0000, 2); // White text for average pressure
            	sprintf(buffer, "Pokemon broke out!");
            	DrawString(10, 80, buffer, 0xF800, 0x0000, 2); // Red text for the second line


                HAL_Delay(2000); // Pause for user to view results

                // Transition back to action selection
                ILI9341_FillScreen(0x0000); // Clear screen
                DrawString(10, 30, "Select Action:", 0x07E0, 0x0000, 2); // Green text
                DrawString(10, 90, "Battle:", 0xF800, 0x0000, 3);   // Red text for Battle
                DrawString(10, 120, "Capture:", 0x07E0, 0x0000, 3);  // Green text for Capture
                Reset_Button_State(); // Reset button state
                currentState = SELECT_ACTION; // Return to action selection
            }
            break;
        }



               case POST_BATTLE:
                   uart_send_string("Pokemon Caught. Returning to the forest.\n\r");
                   currentState = IDLE;
                   break;
           }
	}


