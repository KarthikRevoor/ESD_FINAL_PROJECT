#include "integration.h"
#include "lcd_functions.h"
#include "lcd.h"
#include "stdbool.h"
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
int pokemon_health_percentage = 100; // Default to 100%
int previous_damage = 0;
bool welcome_message_displayed = false;
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
    // Add a flag to track whether the welcome message has been displayed


    case IDLE:
        if (!welcome_message_displayed) {
            // Clear the screen and add a welcome message
            ILI9341_FillScreen(0x0000); // Clear the screen
            ILI9341_FillRect(0, 0, 320, 60, 0x001F); // Blue title background

            // Center-align the "Welcome to Pokemon Adventure!" text
            // Center-align the "Welcome to Pokemon Adventure!" text
            DrawString(60, 15, "Welcome to", 0xFFFF, 0x001F, 2); // Line 1
            DrawString(20, 35, "Pokemon Adventure!", 0xFFFF, 0x001F, 2); // Line 2

            // Center-align the "Wave your hand to start!" text
            DrawString(40, 100, "Wave your hand", 0xFFFF, 0x0000, 2); // Line 1
            DrawString(70, 120, "to start!", 0xFFFF, 0x0000, 2); // Line 2
            uart_send_string("Waiting for IR sensor trigger...\n\r");

            HAL_Delay(2000);

            process_SD_card("init3.bmp");

            // Set the flag to true after displaying the message
            welcome_message_displayed = true;
        }

        // Wait for IR sensor trigger
        ir_triggered = 0;
        if (IR_is_triggered()) {
            // Transition to SENSOR_READ when IR is triggered
            uart_send_string("Pokemon detected! Moving to SENSOR_READ.\n\r");

            // Clear the screen for the next state
            ILI9341_FillScreen(0x0000);
            currentState = SENSOR_READ;

            // Reset the flag for the next time IDLE is entered
            welcome_message_displayed = false;
        }
        break;

        case SENSOR_READ: {
            ILI9341_FillScreen(0x0000); // Clear the screen before displaying new data
            ILI9341_FillRect(0, 0, 320, 40, 0x001F); // Blue background for heading
            DrawString(10, 10, "Sensor Readings", 0xFFFF, 0x001F, 2); // Heading in white text

            DHT_DataTypedef DHT_Data;
            DHT_GetData(&DHT_Data); // Read DHT11 data

            // Only use humidity from DHT11
            dht11_humidity = (int)DHT_Data.Humidity;

            // Read temperature from DS18B20
            //temp_sensor_check();
            ds18b20_temperature = temperature;

            // Debug output for humidity and temperature
            char buffer[100];
            sprintf(buffer, "Humidity: %d%%", dht11_humidity);
            uart_send_string(buffer);
            DrawString(10, 50, buffer, 0xFFFF, 0x0000, 1); // Display humidity on the screen

            sprintf(buffer, "Temperature: %.2f°C", ds18b20_temperature);
            uart_send_string(buffer);
            DrawString(10, 70, buffer, 0xFFFF, 0x0000, 1); // Display temperature on the screen

            if (ds18b20_temperature > 25.0) {
                // Split the reasoning message into two lines
                DrawString(10, 100, "Reason:", 0xF800, 0x0000, 1); // Display "Reason:" in red
                DrawString(10, 120, "High temperature -> Fire type.", 0xF800, 0x0000, 1); // Second line for explanation
                uart_send_string("Reason: High temperature -> Fire type.\n\r");
            } else if (dht11_humidity > 60) {
                // Split the reasoning message into two lines
                DrawString(10, 100, "Reason:", 0x001F, 0x0000, 1); // Display "Reason:" in blue
                DrawString(10, 120, "High humidity -> Water type.", 0x001F, 0x0000, 1); // Second line for explanation
                uart_send_string("Reason: High humidity -> Water type.\n\r");
            } else {
                // Split the reasoning message into two lines
                DrawString(10, 100, "Reason:", 0x07E0, 0x0000, 1); // Display "Reason:" in green
                DrawString(10, 120, "Neutral conditions -> Normal type.", 0x07E0, 0x0000, 1); // Second line for explanation
                uart_send_string("Reason: Neutral conditions -> Normal type.\n\r");
            }

            HAL_Delay(2000); // Allow time for the user to view the readings
            currentState = CHECK_POKEMON_ENCOUNTER; // Move to next state
            break;
        }


        case CHECK_POKEMON_ENCOUNTER: {
            FlashScreen(3, 50); // Flash to indicate encounter transition
            ILI9341_FillScreen(0x0000);
            ILI9341_FillRect(0, 0, 320, 40, 0x001F); // Blue background for heading
            DrawString(10, 10, "Pokemon Encounter!", 0xFFFF, 0x001F, 2); // White text on blue background

            // Reset health and damage for the new Pokémon
            pokemon_health_percentage = 100;
            previous_damage = 0;

            // Determine Pokémon to spawn
            const char *pokemon_name = NULL;
            if (ds18b20_temperature > 25.0) {
                pokemon_name = spawn_pokemon_from_array(fire_pokemon);
            } else if (dht11_humidity > 60) {
                pokemon_name = spawn_pokemon_from_array(water_pokemon);
            } else {
                pokemon_name = spawn_pokemon_from_array(normal_pokemon);
            }
            strcpy(current_pokemon, pokemon_name);

            // Display the Pokémon name
            char buffer[100];
            DrawString(10, 60, "A wild", 0xFFFF, 0x0000, 2); // White text
            sprintf(buffer, "%s", pokemon_name);
            DrawString(10, 90, buffer, 0xFFFF, 0x0000, 2); // Pokémon name
            DrawString(10 + (strlen(buffer) * 12) + 10, 90, "appears!", 0xFFFF, 0x0000, 2); // Adjusted text position

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
            const int max_shakes = 70;     // Maximum allowed shakes for 100% health
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

            // Update `previous_damage`
            static int previous_damage = 0; // Persistent across calls
            previous_damage += total_damage;

            // Calculate health percentage
            int max_health = max_shakes * base_damage; // Maximum possible damage
            pokemon_health_percentage = 100 - ((previous_damage * 100) / max_health);
            if (pokemon_health_percentage < 0) {
                pokemon_health_percentage = 0; // Ensure it doesn't go below 0%
            }

            // Determine health bar color based on health percentage
            uint16_t health_color;
            if (pokemon_health_percentage <= 0) {
                health_color = 0xF800; // Red for fainted
            } else if (pokemon_health_percentage < 40) {
                health_color = 0xF800; // Red for health < 40%
            } else if (pokemon_health_percentage < 75) {
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
            uint16_t health_bar_width = (pokemon_health_percentage * 220) / 100; // Scale health bar width
            ILI9341_FillRect(10, 170, health_bar_width, 20, health_color); // Fill health bar

            // Display health percentage
            sprintf(buffer, "Health: %d%%", pokemon_health_percentage);
            DrawString(10, 200, buffer, 0xFFFF, 0x0000, 2); // White text for health percentage

            // Handle fainted Pokemon
            if (pokemon_health_percentage <= 0) {
                DrawString(10, 240, "Pokemon fainted!", 0xF800, 0x0000, 2); // Red text
                uart_send_string("Pokemon fainted! Cannot battle or capture.\n\r");
                HAL_Delay(3000); // Pause to display message
                currentState = POST_BATTLE; // Transition to POST_BATTLE state
            } else {
                // Return to action selection
                uart_send_string("Returning to action selection.\n\r");
                DrawString(10, 240, "Select Action:", 0x07E0, 0x0000, 1); // Green text
                DrawString(10, 260, "PC13 - Battle", 0xF800, 0x0000, 2); // Red text for Battle
                DrawString(10, 280, "PC14 - Capture", 0x07E0, 0x0000, 2); // Green text for Capture
                Reset_Button_State(); // Reset button state for new selection
                currentState = SELECT_ACTION; // Return to action selection
            }
            break;
        }




        case CAPTURE: {
            uart_send_string("Capture initiated! Apply pressure to capture the Pokemon.\n\r");

            char buffer_line1[50];
            char buffer_line2[50];
            char buffer[50];
            uint32_t low_threshold, high_threshold;

            if (pokemon_health_percentage > 75) {
                low_threshold = 500;
                high_threshold = 700;
                sprintf(buffer_line1, "The Pokemon is healthy.");
                sprintf(buffer_line2, "Capturing is tough!");
            } else if (pokemon_health_percentage > 40) {
                low_threshold = 300;
                high_threshold = 600;
                sprintf(buffer_line1, "The Pokemon is weakening.");
                sprintf(buffer_line2, "Capturing is easier.");
            } else {
                low_threshold = 200;
                high_threshold = 500;
                sprintf(buffer_line1, "The Pokemon is very weak.");
                sprintf(buffer_line2, "Capturing is likely!");
            }

            // Display the divided lines on the LCD
            DrawString(10, 70, "Hint:", 0x07E0, 0x0000, 2); // Green text for "Hint:"
            DrawString(10, 100, buffer_line1, 0xFFFF, 0x0000, 1); // First line of the hint
            DrawString(10, 120, buffer_line2, 0xFFFF, 0x0000, 1); // Second line of the hint

            // Countdown before applying pressure
            for (int countdown = 3; countdown > 0; countdown--) {
                // Clear only the countdown number area
                ILI9341_FillRect(10, 150, 300, 50, 0x0000); // Clear countdown area

                // Display the countdown number
                sprintf(buffer, "Press in: %d", countdown);
                DrawString(10, 150, buffer, 0xFFFF, 0x0000, 2); // White text for countdown

                HAL_Delay(1000); // 1-second delay
            }

            // Prompt user to apply pressure
            uart_send_string("Apply pressure now for 2 seconds...\n\r");
            DrawString(10, 150, "Apply pressure now!", 0xFFFF, 0x0000, 2); // White text

            // Measure pressure over 2 seconds with progress bar
            uint32_t start_time = HAL_GetTick();
            uint32_t total_pressure = 0;
            uint32_t sample_count = 0;

            uint16_t progress_x = 10;
            uint16_t progress_y = 200;
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

            // Display results
            ILI9341_FillScreen(0x0000);
            DrawString(10, 10, "Capture Results:", 0xFFFF, 0x001F, 2);

            if (avg_pressure > low_threshold && avg_pressure < high_threshold) {
                // Success message
                sprintf(buffer, "Success!");
                DrawString(10, 50, buffer, 0x07E0, 0x0000, 2); // Green text for success
                sprintf(buffer, "Pokemon captured!");
                DrawString(10, 80, buffer, 0x07E0, 0x0000, 2); // Green text for second line
                sprintf(buffer, "Pressure: %lu", avg_pressure);
                DrawString(10, 120, buffer, 0xFFFF, 0x0000, 2); // White text for pressure value

                HAL_Delay(2000); // Pause for user to view results
                currentState = POST_BATTLE; // Transition to POST_BATTLE
            } else {
                // Failure message
                sprintf(buffer, "Failed!");
                DrawString(10, 50, buffer, 0xF800, 0x0000, 2); // Red text for failure
                sprintf(buffer, "Pokemon broke out!");
                DrawString(10, 80, buffer, 0xF800, 0x0000, 2); // Red text for second line
                sprintf(buffer, "Pressure: %lu", avg_pressure);
                DrawString(10, 120, buffer, 0xFFFF, 0x0000, 2); // White text for pressure value

                HAL_Delay(2000); // Pause for user to view results

                // Return to action selection
                ILI9341_FillScreen(0x0000); // Clear screen
                DrawString(10, 30, "Select Action:", 0x07E0, 0x0000, 2); // Green text
                DrawString(10, 90, "Battle:", 0xF800, 0x0000, 3);   // Red text for Battle
                DrawString(10, 120, "Capture:", 0x07E0, 0x0000, 3);  // Green text for Capture
                Reset_Button_State(); // Reset button state
                currentState = SELECT_ACTION; // Return to action selection
            }
            break;
        }

        case POST_BATTLE: {
            // Clear the screen
        	ILI9341_FillScreen(0x0000);

        	    if (pokemon_health_percentage <= 0) {
        	        DrawString(70, 100, "Pokemon", 0xF800, 0x0000, 3); // Display "Fainted" message in red
        	        DrawString(70, 150, "Fainted!", 0xF800, 0x0000, 3); // Display "Fainted" message in red
        	        HAL_Delay(3000);
        	        currentState = IDLE;
        	    } else {

            // Calculate centered x-coordinates for the text
            int text_x;

            // Display Pokemon Name
            text_x = 120 - (strlen(current_pokemon) * 6); // Center Pokémon name horizontally (120 is half of 240 width)
            DrawString(text_x, 100, current_pokemon, 0xFFFF, 0x0000, 3); // White text for Pokémon name

            // Display "Captured!"
            const char *captured_message = "Captured!";
            text_x = 120 - (strlen(captured_message) * 6); // Center "Captured!" horizontally
            DrawString(text_x, 150, captured_message, 0x07E0, 0x0000, 3); // Green text for "Captured!"

            // Confetti Animation with larger pixels
            for (int i = 0; i < 30; i++) { // Number of confetti iterations
                for (int j = 0; j < 10; j++) { // Number of confetti per iteration
                    int x = rand() % 235; // Random x-coordinate (ensure rectangle fits within screen width)
                    int y = rand() % 315; // Random y-coordinate (ensure rectangle fits within screen height)
                    uint16_t color = rand() % 0xFFFF; // Random color

                    // Draw a small rectangle as confetti
                    ILI9341_FillRect(x, y, 5, 5, color); // Rectangle of size 5x5
                }
                HAL_Delay(100); // Small delay to simulate animation
            }
            // Wait for 3 seconds to allow user to view the message
            HAL_Delay(3000);

            // Transition back to IDLE state
            ILI9341_FillScreen(0x0000); // Clear the screen
            uart_send_string("Returning to the forest\n\r");
            currentState = IDLE; // Set the state to IDLE
            break;
        }



           }
	}
}


