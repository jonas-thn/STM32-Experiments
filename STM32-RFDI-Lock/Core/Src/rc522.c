#include "rc522.h"

void MFRC522_WriteRegister(uint8_t addr, uint8_t val) {
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    uint8_t tx_addr = (addr << 1) & 0x7E;
    HAL_SPI_Transmit(&hspi1, &tx_addr, 1, 10);
    HAL_SPI_Transmit(&hspi1, &val, 1, 10);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

uint8_t MFRC522_ReadRegister(uint8_t addr) {
    uint8_t rx_data;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    uint8_t tx_addr = ((addr << 1) & 0x7E) | 0x80;
    HAL_SPI_Transmit(&hspi1, &tx_addr, 1, 10);
    uint8_t dummy = 0x00;
    HAL_SPI_TransmitReceive(&hspi1, &dummy, &rx_data, 1, 10);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    return rx_data;
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadRegister(reg);
    MFRC522_WriteRegister(reg, tmp | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = MFRC522_ReadRegister(reg);
    MFRC522_WriteRegister(reg, tmp & (~mask));
}

void MFRC522_AntennaOn(void) {
    uint8_t temp = MFRC522_ReadRegister(TxControlReg);
    if (!(temp & 0x03)) {
        MFRC522_SetBitMask(TxControlReg, 0x03);
    }
}

uint8_t MFRC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen) {
    uint8_t status = MI_ERR;
    uint8_t irqEn = 0x00, waitIRq = 0x00;
    uint8_t lastBits, n;
    uint16_t i;

    if (command == PCD_TRANSCEIVE) {
        irqEn = 0x77; waitIRq = 0x30;
    }

    MFRC522_WriteRegister(ComIEnReg, irqEn | 0x80);
    MFRC522_ClearBitMask(ComIrqReg, 0x80);
    MFRC522_SetBitMask(FIFOLevelReg, 0x80);

    for (i = 0; i < sendLen; i++) { MFRC522_WriteRegister(FIFODataReg, sendData[i]); }

    MFRC522_WriteRegister(CommandReg, command);
    if (command == PCD_TRANSCEIVE) { MFRC522_SetBitMask(BitFramingReg, 0x80); }

    i = 2000;
    do {
        n = MFRC522_ReadRegister(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    MFRC522_ClearBitMask(BitFramingReg, 0x80);

    if (i != 0 && !(MFRC522_ReadRegister(ErrorReg) & 0x1B)) {
        status = MI_OK;
        if (n & irqEn & 0x01) { status = MI_NOTAGERR; }
        if (command == PCD_TRANSCEIVE) {
            n = MFRC522_ReadRegister(FIFOLevelReg);
            lastBits = MFRC522_ReadRegister(ControlReg) & 0x07;
            if (lastBits) { *backLen = (n - 1) * 8 + lastBits; } else { *backLen = n * 8; }
            if (n == 0) { n = 1; }
            if (n > 16) { n = 16; }
            for (i = 0; i < n; i++) { backData[i] = MFRC522_ReadRegister(FIFODataReg); }
        }
    }
    return status;
}

void MFRC522_Init(void) {
    HAL_GPIO_WritePin(RFID_RST_GPIO_Port, RFID_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(RFID_RST_GPIO_Port, RFID_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50);

    MFRC522_WriteRegister(CommandReg, PCD_RESETPHASE);
    HAL_Delay(50);

    MFRC522_WriteRegister(TModeReg, 0x8D);
    MFRC522_WriteRegister(TPrescalerReg, 0x3E);
    MFRC522_WriteRegister(TReloadRegL, 30);
    MFRC522_WriteRegister(TReloadRegH, 0);

    MFRC522_WriteRegister(TxAutoReg, 0x40);
    MFRC522_WriteRegister(ModeReg, 0x3D);

    MFRC522_AntennaOn();
}

uint8_t MFRC522_Check(uint8_t* id) {
    uint8_t status;
    uint8_t buffer[16];
    uint16_t backLen;

    MFRC522_WriteRegister(BitFramingReg, 0x07);
    buffer[0] = PICC_REQIDL;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 1, buffer, &backLen);

    if (status != MI_OK) {
        return MI_ERR;
    }

    MFRC522_WriteRegister(BitFramingReg, 0x00);
    buffer[0] = PICC_ANTICOLL;
    buffer[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 2, buffer, &backLen);

    if (status == MI_OK) {
        for (uint8_t i = 0; i < 4; i++) {
            id[i] = buffer[i];
        }
        return MI_OK;
    }

    return MI_ERR;
}
