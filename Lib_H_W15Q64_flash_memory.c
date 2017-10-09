/**
 *******************************************************************************
 *  @file       Lib_H_w15q64_flash_memory.c
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

//******************************************************************************
// Секция include: здесь подключается заголовочный файл к модулю
#include "Lib_H_w15q64_flash_memory.h"
//******************************************************************************


//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные переменные
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Локальные переменные
//------------------------------------------------------------------------------
//******************************************************************************


//******************************************************************************
// Секция прототипов локальных функций
void W15Q64_WriteEn(W15Q64spi_t *spi);
void W15Q64_WriteEnStatReg(W15Q64spi_t *spi);
void W15Q64_WriteDis(W15Q64spi_t *spi);
uint8_t W15Q64_ReadStatReg(W15Q64spi_t *spi,
                           uint8_t instruct);
void W15Q64_AddrTo3Arr(uint32_t addr,
                       uint8_t *pAddr);
uint8_t W15Q64_BitsInByte(_Bool *pStatReg);
//******************************************************************************


//******************************************************************************
// Секция описания функций (сначала глобальных, потом локальных)

/**
 *  @brief  Функция читает значение StatusRegister1 и StatusRegister2  и 
 *          значение битов данного регистра заносит в соотвествующий массив типа "bool"         
 *  @param  *spiFunc:   Указатель на структуру, содержащую указатели на функции 
 *                      для работы с шиной SPI
 *  @param  *statReg:   Указатель на структуру в которую выполняется побитная 
 *                      запись значений Status Register 1 и 2
 *  @retval None
 */
void W15Q64_ReadStatRegs(W15Q64spi_t *spi,
                         W15Q64statRegs_t *status)
{
    uint8_t reg1Temp = W15Q64_ReadStatReg(spi, (uint8_t) W15Q64_READ_STATUS_REGISTER_1),
            reg2Temp = W15Q64_ReadStatReg(spi, (uint8_t) W15Q64_READ_STATUS_REGISTER_2);
    uint8_t byteMask [8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    uint8_t i = 0;
    for (i = 0; i <= 7; i++)
    {
        status->reg1[i] = ((reg1Temp & byteMask[i]) == byteMask[i]) ? true : false;
        status->reg2[i] = ((reg2Temp & byteMask[i]) == byteMask[i]) ? true : false;
    }
}

/**
 *  @brief  Функция выполняет чтение одного байта данных из памяти микросхемы flash memory
 *  @param  *spi:   Указатель на структуру в которой содержатся указатели на 
 *                  функции для работы с шиной SPI
 *  @param  addr:   "Адрес памяти в микросхеме 24-Bit Address"
 *  @retval Байт данных
 */
uint8_t W15Q64_ReadData(W15Q64spi_t *spi,
                        uint32_t addr)
{
    uint8_t addrArr[3],
            instruct = W15Q64_READ_DATA,
            dataByte = 0;
    ;
    // Преобразовываем адрес устройства в массив из 3-х байт для отправки на шину SPI
    W15Q64_AddrTo3Arr(addr, addrArr);

    // Работа с микросхемой через интерфейс SPI (см. 7.2.11 Read Data (03h))
    spi->sc_ON();
    spi->transmit(&instruct, 1); //         Instruсtion
    spi->transmit(addrArr, 3); //           24-Bit Address
    spi->receive(&dataByte, 1); //          Data Out 1
    spi->cs_OFF();

    return dataByte;
}

/**
 *  @brief  Функция выполняет чтение массива байт из памяти микросхемы flash memory
 *  @param  *spi:   Указатель на структуру в которой содержатся указатели на 
 *                  функции для работы с шиной SPI
 *  @param  addr:   "Адрес памяти в микросхеме 24-Bit Address"
 *  @param  *pRxData:   Указатель на первый элемент массива, в который будут 
 *                      записаны данные из микросхемы flash memory
 *  @param  cnt:    Количество данных, которое необходимо записать в массив
 *  @retval None
 */
void W15Q64_FastReadData(W15Q64spi_t *spi,
                         uint32_t addr,
                         uint8_t *pRxData,
                         uint16_t cnt)
{
    uint8_t addrArr[3],
            instruct = W15Q64_FAST_READ,
            dummyByte = 0;
    ;
    // Преобразовываем адрес устройства в массив из 3-х байт для оправки на шину SPI
    W15Q64_AddrTo3Arr(addr, addrArr);

    // Работа с микросхемой через интерфейс SPI (см. 7.2.12 Fast Read (0Bh))
    spi->sc_ON();
    spi->transmit(&instruct, 1); //         Instruction
    spi->transmit(addrArr, 3); //           24-Bit Address
    spi->transmit(&dummyByte, 1); //        Dummy Clocks
    spi->receive(pRxData, cnt); //          Data Out Array
    spi->cs_OFF();
}

/**
 *  @brief  Функция записывает данные в StatusRegister1 и StatusRegister2
 *  @param  *spiFunc:   Указатель на структуру, содержащую указатели на функции 
 *                      для работы с шиной SPI
 *  @param  *statReg:   Указатель на стуктуру в которой содержатся значения битов 
 *                      Status Register которые необходимо записать в микросхему
 *  @retval None
 */
void W15Q64_WriteStatRegs(W15Q64spi_t *spi,
                          W15Q64statRegs_t *status)
{
    uint8_t txData = W15Q64_WRITE_STATUS_REGISTER,
            statRegsArr[2] = {W15Q64_BitsInByte(status->reg1),
        W15Q64_BitsInByte(status->reg2)};

    // Работа с микросхемой через интерфейс SPI (см. 7.2.10 Write Status Register (01h))
    spi->sc_ON();
    spi->transmit(&txData, 1); //       Instruction
    spi->transmit(statRegsArr, 2); //   Status Register 1, then Status Register 2
    spi->cs_OFF();
}

/**
 *  @brief  Функция выполняет запись массива данных по указанному адресу во flash память
 *  @param  *spi:   Указатель на структуру в которой содержатся указатели на 
 *                  функции для работы с шиной SPI
 *  @param  addr:   Адрес памяти в микросхеме "24-Bit Address"
 *  @param  *pTxdata:   Указатель на первый элемент массива в котором содержатся 
 *                      данные для записи во flash память
 *  @param  cnt:    Количество данных в массиве, которое необходимо отправить 
 *                  на шину SPI
 *  @retval None
 * 
 *  @warning    Запись данных может начинаться с шагом в 256 байт, т.е. младшие 
 *              8 бит должны быть нулями.
 *              Записать за один раз можно только 256 байт, 257 байт будет 
 *              перезаписывать первый байт.
 */
void W15Q64_PageProg(W15Q64spi_t *spi,
                     uint32_t addr,
                     uint8_t *pTxData,
                     uint16_t cnt)
{
    uint8_t txInstruct = W15Q64_PAGE_PROGRAM,
            addrArr[3];

    // Программирование страницы ограниченно 256 байтами
    // cnt - это количество передаваемых данных!!! Поэтому верхний предел "cnt = 256" а не "cnt = 255"
    if (cnt > 256)
    {
        cnt = 256;
    }

    // Преобразовываем адрес устройства в массив, состоящий из 3-х байт для оправки на шину SPI
    W15Q64_AddrTo3Arr(addr, addrArr);

    W15Q64_WriteEn(spi);

    // Работа с микросхемой через интерфейс SPI (см. 7.2.20 Page Program (02h))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->transmit(addrArr, 3); //           24-Bit Address
    spi->transmit(pTxData, cnt); //         Data Bytes
    spi->cs_OFF();
}

/**
 *  @brief  Функция выполняет чтение последовательности Security Registers
 *  @param  *spi:   Указатель на структуру в которой содержатся указатели на 
 *                  функции для работы с шиной SPI
 *  @param  addr:   "Адрес памяти в микросхеме 24-Bit Address"
 *  @param  *pRxData:   Указатель на первый элемент массива в который идет 
 *                      запись значений Security Registers
 *  @param  cnt:    Количество данных, которое необходимо записать в массив
 *  @retval None
 */
void W15Q64_ReadSecReg(W15Q64spi_t *spi,
                       uint32_t addr,
                       uint8_t *pRxData,
                       uint16_t cnt)
{
    uint8_t dummyByte = 0,
            instruct = W15Q64_READ_SECURITY_REGISTER,
            addrArr[3];

    // Преобразовываем адрес устройства в массив, состоящий из 3-х байт для оправки на шину SPI
    W15Q64_AddrTo3Arr(addr, addrArr);

    // Работа с микросхемой через интерфейс SPI (см. 7.2.38 Read Security Registers (48h))
    spi->sc_ON();
    spi->transmit(&instruct, 1); //         Instruction
    spi->transmit(addrArr, 3); //           24-Bit Address
    spi->transmit(&dummyByte, 1); //        Dummy Byte
    spi->transmit(pRxData, cnt); //         Data Out
    spi->cs_OFF();
}


//==============================================================================
// Функции для очисти памяти микросхемы Flash Memory

/**
 *  @brief  Функция выполняет стирание выбранной области памяти микросхемы flash memory
 *  @param  *spi:   Указатель на структуру в которой содержатся указатели на 
 *                  функции для работы с шиной SPI
 *  @param  addr:   "Адрес памяти в микросхеме 24-Bit Address"
 *  @param  txInstruct: Инструкция, указывающая какая именно команда на стирание 
 *                      будет отправлена на шину данных SPI
 *  @retval None
 */
void W15Q64_Erase(W15Q64spi_t *spi, uint32_t addr, uint8_t txInstruct)
{
    W15Q64_WriteEn(spi);
    uint8_t txData = txInstruct,
            addrAdrr[3];
    W15Q64_AddrTo3Arr(addr, addrAdrr);

    // Работа с микросхемой через интерфейс SPI (общая последовательность при стирании данных)
    spi->sc_ON();
    spi->transmit(&txData, 1); //           Instruction
    spi->transmit(addrAdrr, 3); //          24-Bit Address
    spi->cs_OFF();
}

void W15Q64_SectorErase4KB(W15Q64spi_t *spi,
                           uint32_t addr)
{
    W15Q64_Erase(spi, addr, (uint8_t) W15Q64_SECTOR_ERASE_4KB);
}

void W15Q64_BlockErase32KB(W15Q64spi_t *spi,
                           uint32_t addr)
{
    W15Q64_Erase(spi, addr, (uint8_t) W15Q64_BLOCK_ERASE_32KB);
}

void W15Q64_BlockErase64KB(W15Q64spi_t *spi,
                           uint32_t addr)
{
    W15Q64_Erase(spi, addr, (uint8_t) W15Q64_BLOCK_ERASE_64KB);
}

void W15Q64_ChipErase(W15Q64spi_t *spi)
{
    uint8_t txInstruct = W15Q64_CHIP_ERASE;

    // Работа с микросхемой через интерфейс SPI (см. Chip Erase (C7h))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->cs_OFF();
}

void W15Q64_EraseProgram_Suspend(W15Q64spi_t *spi)
{
    uint8_t txInstruct = W15Q64_ERASE_PROGRAM_SUSPEND;

    // Работа с микросхемой через интерфейс SPI (см. 7.2.26 Erase_Program Suspend (75h))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->cs_OFF();
}

void W15Q64_EraseProgram_Resume(W15Q64spi_t *spi)
{
    uint8_t txInstruct = W15Q64_ERASE_PROGRAM_RESUME;

    // Работа с микросхемой через интерфейс SPI (см. 7.2.27 Erase/Program Resume (7Ah))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->cs_OFF();
}

void W15Q64_PowerDown(W15Q64spi_t *spi)
{
    uint8_t txInstruct = W15Q64_POWER_DOWN;

    // Работа с микросхемой через интерфейс SPI (см. 7.2.28 Power-down (B9h))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->cs_OFF();
}

void W15Q64_ReleasePowerDown(W15Q64spi_t *spi)
{
    uint8_t txInstruct = W15Q64_RELEASE_POWER_DOWN;

    // Работа с микросхемой через интерфейс SPI (см. 7.2.29 Release Power-down (ABh))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->cs_OFF();
}

uint8_t W15Q64_DeviceID(W15Q64spi_t *spi)
{
    uint8_t txInstruct = W15Q64_RELEASE_POWER_DOWN,
            dummyBytes[3] = {0x00, 0x00, 0x00},
    deviceID = 0;

    // Работа с микросхемой через интерфейс SPI (см. 7.2.29 Release Power-down (ABh))
    spi->sc_ON();
    spi->transmit(&txInstruct, 1); //       Instruction
    spi->transmit(dummyBytes, 3); //        3 Dummy Bytes
    spi->receive(&deviceID, 1); //          Device ID
    spi->cs_OFF();
    return deviceID;
}

//==============================================================================
// Локальные функции

/**
 *  @brief  Функция отправляет инструкцию в микросхему flash памяти для отключения 
 *          блокировки на запись
 *  @param  *spiFunc:   Указатель на структуру, содержащую указатели на функции 
 *                      для работы с шиной SPI
 *  @retval None
 */
void W15Q64_WriteEn(W15Q64spi_t *spi)
{
    uint8_t txData = W15Q64_WRITE_ENABLE;

    // Работа с микросхемой по шине SPI (см. 7.2.6 Write Enable (06h))
    spi->sc_ON();
    spi->transmit(&txData, 1); //           Instruction
    spi->cs_OFF();
}

/**
 *  @brief  Функция отправляет инструкцию в микросхему flash памяти для отключения 
 *          блокировки на запись 
 *  @param  *spiFunc:   Указатель на структуру, содержащую указатели на функции 
 *                      для работы с шиной SPI
 *  @retval None
 */
void W15Q64_WriteEnStatReg(W15Q64spi_t *spi)
{
    uint8_t txData = W15Q64_VOLATILE_SR_WRITE_EN;

    // Работа с микросхемой по шине SPI (см. 7.2.7 Write Enable for Volatile Status Register (50h))
    spi->sc_ON();
    spi->transmit(&txData, 1);
    spi->cs_OFF();
}

/**
 *  @brief  Функция отправляет инструкцию в микросхему flash памяти для включения 
 *          блокировки на запись
 *  @param  *spiFunc:   Указатель на структуру, содержащую указатели на функции 
 *                      для работы с шиной SPI
 *  @retval None
 */
void W15Q64_WriteDis(W15Q64spi_t *spi)
{
    uint8_t txData = W15Q64_WRITE_DIS;

    // Работа с микросхемой по шине SPI (см. 7.2.8 Write Disable (04h))
    spi->sc_ON();
    spi->transmit(&txData, 1);
    spi->cs_OFF();
}

/**
 *  @brief  Функция делает преобразование 3-х байтного адреса в массив из 3-х байт
 *  @param  addr:   Адрес в памяти микросхемы flash памяти
 *  @param  *pAddr: Указатель на массив в который будут записаны 3 байта адреса   
 *  @retval None
 */
void W15Q64_AddrTo3Arr(uint32_t addr,
                       uint8_t *pAddr)
{
    *pAddr++ = (uint8_t) ((addr >> 16) & 0xFF);
    *pAddr++ = (uint8_t) ((addr >> 8) & 0xFF);
    *pAddr = (uint8_t) (addr & 0xFF);
}

/**
 *  @brief  Функция выполняет запись битов из массива размерностью 8 в байт данных
 *  @param  *statRegs:  Массив, в каждой ячейке которого может быть одно из двух 
 *                      состояний - "0" или "1"
 *  @retval Байт данных, преобразованный из массива битов
 */
uint8_t W15Q64_BitsInByte(_Bool *pStatReg)
{
    uint8_t byteMask_if_true [8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    uint8_t byteMask_if_false [8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
    uint8_t statReg = 0;
    uint8_t i = 0;

    for (i = 0; i <= W15Q64_SRP0; i++)
    {
        if (pStatReg[i])
        {
            statReg |= byteMask_if_true[i];
        }
        else
        {
            statReg &= byteMask_if_false[i];
        }
    }
    return statReg;
}

/**
 *  @brief  Функция отправляет данные на шину SPI и принимает 1 байт данных 
 *          с шины SPI
 *  @param  *spiFunc:   Указатель на структуру, содержащую указатели на функции 
 *                      для работы с шиной SPI
 *  @param  instruct:   Инструкция для отправки на шину данных SPI
 */
uint8_t W15Q64_ReadStatReg(W15Q64spi_t *spi,
                           uint8_t instruct)
{
    uint8_t rxData = 0;

    // Работа с шиной данных SPI (см. 7.2.9 Read Status Register 1 and 2)
    spi->sc_ON();
    spi->transmit(&instruct, 1); //     Instruction
    spi->receive(&rxData, 1); //        Status Register 1 or 2
    spi->cs_OFF();
    return rxData;
}
//******************************************************************************


////////////////////////////////////////////////////////////////////////////////
// END OF FILE
////////////////////////////////////////////////////////////////////////////////
