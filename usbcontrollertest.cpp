#include <stdio.h>
#include "pico/stdlib.h"
#include <gamepad.h>
#include "tusb.h"

typedef unsigned long DWORD;
void InfoNES_PadState(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem)
{
    static constexpr int LEFT = 1 << 6;
    static constexpr int RIGHT = 1 << 7;
    static constexpr int UP = 1 << 4;
    static constexpr int DOWN = 1 << 5;
    static constexpr int SELECT = 1 << 2;
    static constexpr int START = 1 << 3;
    static constexpr int A = 1 << 0;
    static constexpr int B = 1 << 1;

    static DWORD prevButtons[2]{};
    static int rapidFireMask[2]{};
    static int rapidFireCounter = 0;

    ++rapidFireCounter;
    bool reset = false;

    for (int i = 0; i < 2; ++i)
    {
        auto &dst = i == 0 ? *pdwPad1 : *pdwPad2;
        auto &gp = io::getCurrentGamePadState(i);

        int v = (gp.buttons & io::GamePadState::Button::LEFT ? LEFT : 0) |
                (gp.buttons & io::GamePadState::Button::RIGHT ? RIGHT : 0) |
                (gp.buttons & io::GamePadState::Button::UP ? UP : 0) |
                (gp.buttons & io::GamePadState::Button::DOWN ? DOWN : 0) |
                (gp.buttons & io::GamePadState::Button::A ? A : 0) |
                (gp.buttons & io::GamePadState::Button::B ? B : 0) |
                (gp.buttons & io::GamePadState::Button::SELECT ? SELECT : 0) |
                (gp.buttons & io::GamePadState::Button::START ? START : 0) |
                0;

        int rv = v;
        if (rapidFireCounter & 2)
        {
            // 15 fire/sec
            rv &= ~rapidFireMask[i];
        }

        dst = rv;

        auto p1 = v;
        auto pushed = v & ~prevButtons[i];
      
            if (pushed & LEFT)
            {
               printf("LEFT\n");
            }
            if (pushed & RIGHT)
            {
                printf("RIGHT\n");
            }
            if (pushed & START)
            {
                printf("START\n");
            }
            if (pushed & SELECT)
            {
                printf("SELECT\n");
            }
            if (pushed & A)
            {
               printf("A\n");
            }
            if (pushed & B)
            {
                printf("B\n");
            }
            if (pushed & UP)
            {
               printf("UP\n");
            }
            else if (pushed & DOWN)
            {
               printf("DOWN\n"); 
            }
      

        prevButtons[i] = v;
    }
}

int main()
{
    stdio_init_all();
    tusb_init();
    while (true) {
        //printf("Hello, world!\n");
        sleep_ms(1000 / 60);
        tuh_task();
        DWORD pdwPad1, pdwPad2, pdwSystem;
        InfoNES_PadState(&pdwPad1, &pdwPad2, &pdwSystem);
    }
}
