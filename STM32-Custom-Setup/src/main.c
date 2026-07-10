#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t *)(0x40023800 + 0x30)) // ahb1 enable reg
#define RCC_APB1ENR  (*(volatile uint32_t *)(0x40023800 + 0x40)) //apb1 enable reg

#define GPIOC_MODER  (*(volatile uint32_t *)(0x40020800 + 0x00)) // port c mode reg
#define GPIOC_PUPDR  (*(volatile uint32_t *)(0x40020800 + 0x0C)) //port c pullup pulldown reg
#define GPIOC_AFRL   (*(volatile uint32_t *)(0x40020800 + 0x20)) //port c alternate function reg

#define TIM3_CR1     (*(volatile uint32_t *)(0x40000400 + 0x00)) //timer3 control reg
#define TIM3_SMCR    (*(volatile uint32_t *)(0x40000400 + 0x08)) //timer3 slave mode control reg
#define TIM3_CCMR1   (*(volatile uint32_t *)(0x40000400 + 0x18)) //timer3 capture compare mode reg
#define TIM3_CNT     (*(volatile uint32_t *)(0x40000400 + 0x24)) //timer3 count reg
#define TIM3_ARR     (*(volatile uint32_t *)(0x40000400 + 0x2C)) //timer3 auto reload register

int main(void)
{
    RCC_AHB1ENR |= (1U << 2); // GPIOC clock enable
    RCC_APB1ENR |= (1U << 1); // TIM3 clock enable

    GPIOC_MODER &= ~((3U << 12) | (3U << 14));  //reset mode 
    GPIOC_MODER |=  ((2U << 12) | (2U << 14)); //set alternate function (10)

    GPIOC_PUPDR &= ~((3U << 12) | (3U << 14)); //reset pullup/down 
    GPIOC_PUPDR |=  ((1U << 12) | (1U << 14)); //set pullups (01)

    GPIOC_AFRL  &= ~((0xFU << 24) | (0xFU << 28)); //clear alrernate function mapping
    GPIOC_AFRL  |=  ((2U << 24) | (2U << 28)); //set af 

    TIM3_ARR = 0xFFFF; //set max auto reload

    TIM3_CCMR1 = (1U << 0) | (1U << 8) | (0xFU << 4) | (0xFU << 12); //map pins to timer & add max filtwe
    TIM3_SMCR = (3U << 0); //encoder mode 3 (count edges)
    TIM3_CR1 = (1U << 0); //counter enable

    volatile int32_t encoder_position = 0;

    while(1) {
        encoder_position = ((int32_t)(int16_t)TIM3_CNT) / 4; //4 esges counted (2 per pin)
    }
}