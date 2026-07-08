ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))
    
    *(.text)           
    *(.rodata)         
    _etext = .;        
  } >FLASH

  .data :
  {
    _sdata = .;        
    *(.data)           
    _edata = .;        
  } >RAM AT>FLASH      

  .bss :
  {
    __bss_start__ = .; 
    *(.bss)            
    __bss_end__ = .;   
    end = .;           
  } >RAM
}