/**
 ******************************************************************************
 *  @file       Lib_H_w15q64_flash_memory.h
 *  @author     Исаев Михаил
 *  @version    v1.0
 *  @date       1.09.2017
 *  @brief      Низкоуровневая библиотека для работы с микросхемой flash памяти 
 *              w15q65
 *  @warning    Префикс W15Q64FV необходим для гарантии уникальности названия именованных 
 *              констант во всех проектах, в которых данный модуль может быть использован.
 *              Для гарантии уникальности функций, следует добавлять данное обозначение 
 *              как префикс.
 *              Пример: Именования константа - "W15Q64_WRITE_ENABLE"
 *                      Название функции - "void W15Q64_WriteEn(W15Q64spi_t *spi);"
 *******************************************************************************
 */

//
#ifndef LIB_H_W15Q64_FLASH_MEMORY_H
#define	LIB_H_W15Q64_FLASH_MEMORY_H

//******************************************************************************
// Секция include (подключаем заголовочные файлы используемых модулей)
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
//******************************************************************************


//******************************************************************************
// Секция определения констант
// Standart SPI Instructions
#define W15Q64_WRITE_ENABLE                               0x06
#define W15Q64_VOLATILE_SR_WRITE_EN                       0x50
#define W15Q64_WRITE_DIS                                  0x04
#define W15Q64_READ_STATUS_REGISTER_1                     0x05
#define W15Q64_READ_STATUS_REGISTER_2                     0x35
#define W15Q64_WRITE_STATUS_REGISTER                      0x01
#define W15Q64_PAGE_PROGRAM                               0x02
#define W15Q64_SECTOR_ERASE_4KB                           0x20
#define W15Q64_BLOCK_ERASE_32KB                           0x52
#define W15Q64_BLOCK_ERASE_64KB                           0xD8
#define W15Q64_CHIP_ERASE                                 0xC7
#define W15Q64_ERASE_PROGRAM_SUSPEND                      0x75
#define W15Q64_ERASE_PROGRAM_RESUME                       0x7A
#define W15Q64_POWER_DOWN                                 0xB9
#define W15Q64_READ_DATA                                  0x03
#define W15Q64_FAST_READ                                  0x0B
#define W15Q64_RELEASE_POWER_DOWN                         0xAB
#define W15Q64_MANUFACTURER_DEVICE_ID                     0x90
#define W15Q64_JEDEC_ID                                   0x9F
#define W15Q64_READ_UNIQUE_ID                             0x4B
#define W15Q64_READ_SFDP_REGISTER                         0x5A
#define W15Q64_ERASE_SECURITY_REGISTER                    0x44
#define W15Q64_READ_SECURITY_REGISTER                     0x48
#define W15Q64_ENABLE_QPI                                 0x38
#define W15Q64_ENABLE_RESET                               0x66
#define W15Q64_RESET                                      0x99
#define W15Q64_SET_READ_PARAMETERS                        0xC0
#define W15Q64_BURST_READ_WITH_WRAP                       0x0C

// Dual SPI Instructions
#define W15Q64_FAST_READ_DUAL_OUTPUT                      0x3B
#define W15Q64_FAST_READ_DUAL_IO                          0xBB
#define W15Q64_MANUFACTURER_DEVICE_ID_BY_DUAL_IO          0x92

//Quad SPI Instructions
#define W15Q64_QUAD_PAGE_PROGRAM                          0x32
#define W15Q64_FAST_READ_QUAD_OUTPUT                      0x6B
#define W15Q64_FAST_READ_QUAD_IO                          0xEB
#define W15Q64_WORD_READ_QUAD_IO                          0xE7
#define W15Q64_OCTAL_WORD_READ_QUAD_IO                    0xE3
#define W15Q64_SET_BURST_WITH_WRAP                        0x77
#define W15Q64_MANUFACTURE_DEVICE_ID_BY_QUAD_IO           0x94



#define W15Q64_SECURITY_REGISTER_ADDRES_1                0x001000
#define W15Q64_SECURITY_REGISTER_ADDRES_2                0x002000
#define W15Q64_SECURITY_REGISTER_ADDRES_3                0x003000
//******************************************************************************


//******************************************************************************
// Секция определения типов

enum W15Q64_statReg1 {
    W15Q64_BUSY = 0,
    W15Q64_WEL = 1,
    W15Q64_BP0 = 2,
    W15Q64_BP1 = 3,
    W15Q64_BP2 = 4,
    W15Q64_TB = 5,
    W15Q64_SEC = 6,
    W15Q64_SRP0 = 7
};

enum W15Q64_statReg2 {
    W15Q64_SRP1,
    W15Q64_QE,
    W15Q64_R,
    W15Q64_LB1,
    W15Q64_LB2,
    W15Q64_LB3,
    W15Q64_CMP,
    W15Q64_SUS
};
//******************************************************************************


//******************************************************************************
// Секция определения глобальных переменных

typedef struct {
    // Указатели на функции для работы с шиной SPI
    void (* transmit) (uint8_t *pTxData, uint16_t cnt);
    void (* receive) (uint8_t *pRxData, uint16_t cnt);
//    void (* transmitReceive) (uint8_t *pTxData,
//            uint8_t *pRxData,
//            uint16_t cnt);
    void (* sc_ON) (void);
    void (* cs_OFF) (void);
} W15Q64spi_t; //       Стуктура содержит указатели на функции, обеспечивающие 
//                      работу на шине SPI. Должны быть проинициализированы в
//                      вызывающей функции

typedef struct {
    _Bool reg1[8]; //            Status Register 1 array
    _Bool reg2[8]; //            Status Register 2 array
} W15Q64statRegs_t; //  Структура содержит два массива для хранения значений 
//                      Status Register 1 and Status Register 2
//******************************************************************************


//******************************************************************************
// Секция прототипов глобальных функций
extern void W15Q64_WriteStatRegs(W15Q64spi_t *spi,
        W15Q64statRegs_t *statRegs);
extern void W15Q64_ReadStatRegs(W15Q64spi_t *spi,
        W15Q64statRegs_t *statRegs);
extern void W15Q64_PageProg(W15Q64spi_t *spi,
        uint32_t addr,
        uint8_t *pTxData,
        uint16_t cnt);
extern void W15Q64_ReadSecReg(W15Q64spi_t *spi,
        uint32_t addr,
        uint8_t *pRxData,
        uint16_t cnt);
extern uint8_t W15Q64_ReadData(W15Q64spi_t *spi,
        uint32_t addr);
extern void W15Q64_FastReadData(W15Q64spi_t *spi,
        uint32_t addr,
        uint8_t *pRxData,
        uint16_t cnt);
extern void W15Q64_Erase(W15Q64spi_t *spi,
        uint32_t addr,
        uint8_t txInstruct);
extern void W15Q64_SectorErase4KB(W15Q64spi_t *spi,
        uint32_t addr);
extern void W15Q64_BlockErase32KB(W15Q64spi_t *spi,
        uint32_t addr);
extern void W15Q64_BlockErase64KB(W15Q64spi_t *spi,
        uint32_t addr);
extern void W15Q64_ChipErase(W15Q64spi_t *spi);
extern void W15Q64_EraseProgram_Suspend(W15Q64spi_t *spi);
extern void W15Q64_EraseProgram_Resume(W15Q64spi_t *spi);
extern void W15Q64_PowerDown(W15Q64spi_t *spi);
extern void W15Q64_ReleasePowerDown(W15Q64spi_t *spi);
extern uint8_t W15Q64_DeviceID(W15Q64spi_t *spi);
//******************************************************************************


//******************************************************************************
// Секция определения макросов
//******************************************************************************

#endif

////////////////////////////////////////////////////////////////////////////////
// END OF FILE
////////////////////////////////////////////////////////////////////////////////
