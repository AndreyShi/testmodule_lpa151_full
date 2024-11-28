//--------------------------------------------------
// N25Q128A quad spi flash memory
//--------------------------------------------------
#ifndef QSPI_FLASH_H
#define QSPI_FLASH_H

//--------------------------------------------------
#include <stdint.h>
#include "quadspi.h"
#include "n25q128a.h"

//--------------------------------------------------
#define N25Q128A_PAGE_WRITE_MAX_TIME 50

//--------------------------------------------------
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)

//--------------------------------------------------
typedef enum _qspi_flash_state {
    QSPI_FLASH_IDLE = 0,

    QSPI_FLASH_READ_STARTED,

    QSPI_FLASH_ERASE_WRITE_ENABLE_SENT,
    QSPI_FLASH_ERASE_WRITE_ENABLED,
    QSPI_FLASH_ERASE_SENT,
    QSPI_FLASH_ERASED,

    QSPI_FLASH_PAGE_WRITE_ENABLE_SENT,
    QSPI_FLASH_PAGE_WRITE_ENABLED,
    QSPI_FLASH_PAGE_SEND_CMD,
    QSPI_FLASH_PAGE_DATA_SENT,
    QSPI_FLASH_PAGE_WRITTEN,

    QSPI_FLASH_LAST_PAGE_WRITE_ENABLE_SENT,
    QSPI_FLASH_LAST_PAGE_WRITE_ENABLED,
    QSPI_FLASH_LAST_PAGE_SEND_CMD,
    QSPI_FLASH_LAST_PAGE_DATA_SENT,
    QSPI_FLASH_LAST_PAGE_WRITTEN,
} qspi_flash_state_t;

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void QSPI_Flash_Init(void);
void QSPI_Flash_Task(void);

/* blocking interface */
uint8_t QSPI_Read(uint32_t addr, uint8_t *buff, uint32_t sz);
uint8_t QSPI_EraseSector(uint32_t addr);
uint8_t QSPI_Write(uint32_t addr, uint8_t *buff, uint32_t sz);

/* non-blocking, IT based, interface */
qspi_flash_state_t QSPI_Flash_State(void);
uint8_t QSPI_Flash_Status(void);

uint8_t QSPI_Read_IT(uint32_t addr, uint8_t *buff, uint32_t sz);

uint8_t QSPI_EraseSector_IT(uint32_t addr);
uint8_t QSPI_Write_IT(uint32_t addr, uint8_t *buff, uint32_t sz);

void qspi_timeout_isr(void);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
