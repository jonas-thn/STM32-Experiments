#include <stdint.h>

#define SRAM_START 0x20000000U
#define SRAM_SIZE (128 * 1024U)
#define SRAM_END (SRAM_START + SRAM_SIZE)

extern uint32_t _etext;         
extern uint32_t _sdata;         
extern uint32_t _edata;         
extern uint32_t __bss_start__;  
extern uint32_t __bss_end__;    

extern int main(void);
void Reset_Handler(void);

uint32_t vector_table[] __attribute__((section(".isr_vector"))) = {
    SRAM_END,
    (uint32_t)&Reset_Handler};

void Reset_Handler(void)
{
    uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint8_t *pDst = (uint8_t*)&_sdata; 
    uint8_t *pSrc = (uint8_t*)&_etext; 
    
    for(uint32_t i = 0; i < size; i++) {
        *pDst++ = *pSrc++;
    }
    
    size = (uint32_t)&__bss_end__ - (uint32_t)&__bss_start__;
    pDst = (uint8_t*)&__bss_start__;
    
    for(uint32_t i = 0; i < size; i++) {
        *pDst++ = 0;
    }

    main();

    while (1)
    {
    }
}