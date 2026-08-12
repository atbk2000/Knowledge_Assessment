#include <stdio.h>
#include <windows.h>

#define PROTECTION_TIME_MS 10000 // Time window (ms) where button is inert after release

// ============================================================================
// BUTTON STATE MACHINE
// ============================================================================

typedef enum{
    OFF,
    ON,
    PROTECTED
}ButtonState;

/**
 * @brief Represents a debounced/protected button with timing metadata.
 *
 * - `state` holds the logical state of the button.
 * - `press_start_time` records when the button transitioned to ON.
 * - `press_duration` stores how long the last press lasted (ms).
 * - `protected_start_time` stores when protection started after release.
 */
typedef struct{
    ButtonState state;
    unsigned long press_start_time;
    unsigned long press_duration;
    unsigned long protected_start_time;
}Button;

/**
 * @brief Update the button state machine
 *
 * This function should be called periodically (polling loop) and will
 * transition the `button` through OFF -> ON -> PROTECTED based on
 * the `is_pressed` input and the provided `current_time` (milliseconds).
 *
 * @param button Pointer to the Button instance to update.
 * @param is_pressed Non-zero when the physical button is currently pressed.
 * @param current_time Milliseconds elapsed since a fixed epoch (caller-provided).
 */
void button_update(Button* button, int is_pressed, unsigned long current_time){
    switch(button->state){
        case OFF:
            if(is_pressed){
                // Record press timestamp and enter ON state
                button->press_start_time = current_time;
                button->state = ON;
                printf("State: ON\n");
            }
            break;
        case ON:
            if(!is_pressed){
                // Button released: compute duration and enter PROTECTED
                button->press_duration = (current_time - button->press_start_time);
                button->protected_start_time = current_time;
                button->state = PROTECTED;
                printf("Button pressed for %lu ms\n", button->press_duration);
                printf("State: PROTECTED\n");
            }
            break;
        case PROTECTED:
            // Remain protected until protection timeout expires
            if(current_time - button->press_start_time >= PROTECTION_TIME_MS){
                button->state = OFF;
                printf("State: OFF\n");
            }
            break;
    }
}

// ============================================================================
// MAIN
// ============================================================================
int main(){

    Button button = {
        .state = OFF,
        .press_start_time = 0,
        .press_duration = 0,
        .protected_start_time = 0
    };

    // real ms since some fixed point
    DWORD start_time = GetTickCount(); 

    while(1){

        // spacebar = button
        int is_pressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        // real elapsed ms
        unsigned long current_time = GetTickCount() - start_time;

        button_update(&button, is_pressed, current_time);

        // simulate a periodic polling tick                           
        Sleep(50);

    }

    return 0;
}