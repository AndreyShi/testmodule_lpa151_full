//--------------------------------------------------
// Платформо-зависимые функции для работы с isp
//--------------------------------------------------
#ifndef ISP_IF_H
#define ISP_IF_H

//--------------------------------------------------
#include <stdint.h>

//--------------------------------------------------
extern const uint32_t isp_atmega16u4_signature;
extern const uint32_t isp_atmega32u4_signature;
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

/* all functions return 0 on success, otherwise on error */
/* common service functions */
uint8_t isp_go_slow(void); // to 200k, for blank device operations
uint8_t isp_go_fast(void); // to 1.6M, mind the atmel fuses
uint8_t isp_is_fast(void); // 0 - nope, slow, 1 - eah, fast

uint8_t isp_exit(void);

/* one place to parse and display errors */
void isp_parse_error(const char *cmd, uint8_t err);

/* interrupt service routine */
void isp_spi_isr(void);

/* blocking set of functions,
 * just call and get result
 */
uint8_t isp_enter(void);

uint8_t isp_chip_erase(void);

uint8_t isp_load_ext_addr(uint8_t ext_addr);
uint8_t isp_read_flash(uint16_t addr, uint8_t h, uint8_t *out);
uint8_t isp_load_flash(uint8_t val, uint8_t addr, uint8_t h);
uint8_t isp_burn_flash(uint16_t addr);

/* burn eeprom byte-by-byte */
uint8_t isp_read_eeprom(uint16_t addr, uint8_t *out);
uint8_t isp_write_eeprom(uint8_t val, uint16_t addr);

/* burn eeprom 4 bytes at a time, so 4x faster */
uint8_t isp_load_eeprom(uint8_t val, uint8_t addr);
uint8_t isp_burn_eeprom(uint16_t addr);

/* lock byte gets effective after isp_exit */
uint8_t isp_read_lock(uint8_t *out);
uint8_t isp_write_lock(uint8_t lock);

uint8_t isp_read_signature(uint32_t *out);

/* write one fuse byte at a time, do:
 * 1. isp_enter
 * 2. write {low,high,ext} fuse
 * 3. isp_exit
 * 4. isp_enter
 * 5. go on
 * otherwise fuse byte is not accepted
 */
uint8_t isp_write_low_fuse(uint8_t  lf);
uint8_t isp_write_high_fuse(uint8_t hf);
uint8_t isp_write_ext_fuse(uint8_t  ef);

uint8_t isp_read_low_fuse(uint8_t *out);
uint8_t isp_read_high_fuse(uint8_t *out);
uint8_t isp_read_ext_fuse(uint8_t *out);

/* did not check */
uint8_t isp_read_calib_byte(uint8_t *out);

/* non-blocking set of functions,
 * just call once and again
 * if it returns 0 - go on
 * if it returns 255 - success
 * otherwise - error
 */
uint8_t isp_enter_nb(void);

uint8_t isp_chip_erase_nb(void);

uint8_t isp_load_ext_addr_nb(uint8_t ext_addr);
uint8_t isp_read_flash_nb(uint16_t addr, uint8_t h, uint8_t *out);
uint8_t isp_load_flash_nb(uint8_t val, uint8_t addr, uint8_t h);
uint8_t isp_burn_flash_nb(uint16_t addr);

/* burn eeprom byte-by-byte */
uint8_t isp_read_eeprom_nb(uint16_t addr, uint8_t *out);
uint8_t isp_write_eeprom_nb(uint8_t val, uint16_t addr);

/* lock byte gets effective after isp_exit */
uint8_t isp_read_lock_nb(uint8_t *out);
uint8_t isp_write_lock_nb(uint8_t lock);

uint8_t isp_read_signature_nb(uint32_t *out);

/* write one fuse byte at a time, do:
 * 1. isp_enter
 * 2. write {low,high,ext} fuse
 * 3. isp_exit
 * 4. isp_enter
 * 5. go on from step 1 if needed
 * otherwise fuse byte is not accepted
 */
uint8_t isp_write_low_fuse_nb(uint8_t  lf);
uint8_t isp_write_high_fuse_nb(uint8_t hf);
uint8_t isp_write_ext_fuse_nb(uint8_t  ef);

uint8_t isp_read_low_fuse_nb(uint8_t *out);
uint8_t isp_read_high_fuse_nb(uint8_t *out);
uint8_t isp_read_ext_fuse_nb(uint8_t *out);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
