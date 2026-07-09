#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t *)(0x40023800 + 0x30)) // ahb1 enable reg
#define GPIOC_MODER (*(volatile uint32_t *)(0x40020800 + 0x00)) // port c mode reg
#define GPIOC_IDR (*(volatile uint32_t *)(0x40020800 + 0x10))   // input data reg

int main(void)
{
    RCC_AHB1ENR |= (1 << 1) | (1 << 2);
    GPIOC_MODER &= ~((3 << 0) | (3 << 2) | (3 << 4));

    uint8_t last_state_A = 1;
    volatile int32_t counter = 0;

    while (1)
    {
        uint8_t current_A = (GPIOC_IDR & (1 << 0)) ? 1 : 0;
        uint8_t current_B = (GPIOC_IDR & (1 << 1)) ? 1 : 0;
        uint8_t button = (GPIOC_IDR & (1 << 2)) ? 1 : 0;

        // pin a falling edge
        if ((last_state_A == 1) && (current_A == 0))
        {
            // b 1 = right, b 0 = left
            if (current_B == 1)
            {
                counter++;
            }
            else
            {
                counter--;
            }

            // debounce delay
            for (volatile int i = 0; i < 50000; i++)
            {
            }
        }

        last_state_A = current_A;

        if (button == 0)
        {
            counter = 0;
        }
    }
}