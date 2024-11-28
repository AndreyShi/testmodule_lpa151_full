//--------------------------------------------------
// Все необходимые прошивки для барьера тут
//--------------------------------------------------
#ifndef TESTMODULE_151_FIRMWARE_H
#define TESTMODULE_151_FIRMWARE_H

//--------------------------------------------------
#include <stdint.h>

#include "app_export.h"
#include "common.h"
#include "qspi_flash.h"

//--------------------------------------------------
#define EEPROM_FACTORY_START 0

#define EEPROM_USER_START EEPROM_FACTORY_SIZE

#define FLASH_FW_START     0
#define FLASH_FW_PAGE_SIZE 128

#define FLASH_BL_START (32 - 4)*1024

#define FLASH_CALIBRATIONS_START FLASH_BL_START - FLASH_CALIBRATIONS_TOTAL_SIZE
#define FLASH_CALIBRATIONS_TOTAL_SIZE  11*256
//--------------------------------------------------
typedef enum _firmware_fuse {
    FUSE_LOW = 0,
    FUSE_HIGH,
    FUSE_EXT
} firmware_fuse_t;
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void firmware_init(void);

uint8_t firmware_get(data_id_t id, uint8_t *data, uint32_t sz, uint16_t offset);
const enum cmd_id_t *firmware_get_script(script_id_t id);

uint8_t firmware_put(data_id_t id, const uint8_t *data, uint32_t sz, uint16_t offset);
void firmware_put_script(script_id_t id, enum cmd_id_t *s, uint32_t sz);

uint8_t firmware_save(void);

/* non-blocking version,
 * just call once and again
 * if it returns 0 - go on
 * if it returns 255 - success
 * otherwise - error
 */
uint8_t firmware_save_nb(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
