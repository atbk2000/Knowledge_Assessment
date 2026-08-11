#include <stdio.h>
#include <windows.h>

#define PROTECTION_TIME_MS 10000

typedef enum{
    OFF,
    ON,
    PROTECTED
}ButtonState;

typedef struct{
    ButtonState state;
    unsigned long press_start_time;
    unsigned long press_duration;
    unsigned long protected_start_time;
}Button;

void button_update(Button* button, int is_pressed, unsigned long current_time){
    switch(button->state){
        case OFF:
            if(is_pressed){
                button->press_start_time = current_time;
                button->state = ON;
                printf("State: ON\n");
            }
            break;
        case ON:
            if(!is_pressed){
                button->press_duration = (current_time - button->press_start_time);
                button->protected_start_time = current_time;
                button->state = PROTECTED;
                printf("Button pressed for %lu ms\n", button->press_duration);
                printf("State: PROTECTED\n");
            }
            break;
        case PROTECTED:
            if(current_time - button->press_start_time >= PROTECTION_TIME_MS){
                button->state = OFF;
                printf("State: OFF\n");
            }
            break;
    }
}

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