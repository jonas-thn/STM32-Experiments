#ifndef RC522_H
#define RC522_H

#include "stm32f4xx_hal.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define MI_OK       0
#define MI_NOTAGERR 1
#define MI_ERR      2

#define CommandReg      0x01
#define ComIEnReg       0x02
#define ComIrqReg       0x04
#define ErrorReg        0x06
#define Status2Reg      0x08
#define FIFODataReg     0x09
#define FIFOLevelReg    0x0A
#define ControlReg      0x0C
#define BitFramingReg   0x0D
#define ModeReg         0x11
#define TxModeReg       0x12
#define RxModeReg       0x13
#define TxControlReg    0x14
#define TxAutoReg       0x15
#define TModeReg        0x2A
#define TPrescalerReg   0x2B
#define TReloadRegH     0x2C
#define TReloadRegL     0x2D

#define PCD_IDLE        0x00
#define PCD_TRANSCEIVE  0x0C
#define PCD_RESETPHASE  0x0F

#define PICC_REQIDL     0x26
#define PICC_ANTICOLL   0x93

void MFRC522_Init(void);
uint8_t MFRC522_Check(uint8_t* id);

#endif
