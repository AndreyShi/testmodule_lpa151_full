//--------------------------------------------------
// Приватные определения для интерфейса к isp
//--------------------------------------------------
#ifndef ISP_IF_PRIVATE_H
#define ISP_IF_PRIVATE_H

//--------------------------------------------------
#include <stdio.h>

//--------------------------------------------------
#define ISP_TRANSFER_SIZE 4

//--------------------------------------------------
typedef enum _isp_enter_state {
    ISP_ENTER_START,
    ISP_ENTER_WAIT_RESET,
    ISP_ENTER_DETECT,
    ISP_ENTER_RESET2,
    ISP_ENTER_WAIT_RESET2,
    ISP_ENTER_RESET2_WAIT,
    ISP_ENTER_WAIT_XMIT,
    ISP_ENTER_XMIT_DONE,
    ISP_ENTER_WAIT_REDETECT,
    ISP_ENTER_RESET3,
    ISP_ENTER_WAIT_RESET3,
    ISP_ENTER_GO_LOW,
    ISP_ENTER_RESET3_WAIT,
    ISP_ENTER_WAIT_XMIT_LOW,
    ISP_ENTER_XMIT_DONE_LOW
} isp_enter_state_t;

typedef enum _isp_chiperase_state {
    ISP_CHIPERASE_START,
    ISP_CHIPERASE_WAIT_XMIT,
    ISP_CHIPERASE_XMIT_DONE,
    ISP_CHIPERASE_WAIT_ERASE
} isp_chiperase_state_t;

typedef enum _isp_lea_state {
    ISP_LEA_START,
    ISP_LEA_WAIT_XMIT,
    ISP_LEA_XMIT_DONE
} isp_lea_state_t;

typedef enum _isp_read_flash_state {
    ISP_READ_FLASH_START,
    ISP_READ_FLASH_WAIT_XMIT,
    ISP_READ_FLASH_XMIT_DONE
} isp_read_flash_state_t;

typedef enum _isp_load_flash_state {
    ISP_LOAD_FLASH_START,
    ISP_LOAD_FLASH_WAIT_XMIT,
    ISP_LOAD_FLASH_XMIT_DONE
} isp_load_flash_state_t;

typedef enum _isp_burn_flash_state {
    ISP_BURN_FLASH_START,
    ISP_BURN_FLASH_WAIT_XMIT,
    ISP_BURN_FLASH_XMIT_DONE,
    ISP_BURN_FLASH_WAIT_BURN
} isp_burn_flash_state_t;

typedef enum _isp_read_eeprom_state {
    ISP_READ_EE_START,
    ISP_READ_EE_WAIT_XMIT,
    ISP_READ_EE_XMIT_DONE
} isp_read_eeprom_state_t;

typedef enum _isp_write_eeprom_state {
    ISP_WRITE_EE_START,
    ISP_WRITE_EE_WAIT_XMIT,
    ISP_WRITE_EE_XMIT_DONE,
    ISP_WRITE_EE_WAIT_WRITE
} isp_write_eeprom_state_t;

typedef enum _isp_read_lock_state {
    ISP_READ_LOCK_START,
    ISP_READ_LOCK_WAIT_XMIT,
    ISP_READ_LOCK_XMIT_DONE
} isp_read_lock_state_t;

typedef enum _isp_write_lock_state {
    ISP_WRITE_LOCK_START,
    ISP_WRITE_LOCK_WAIT_XMIT,
    ISP_WRITE_LOCK_XMIT_DONE
} isp_write_lock_state_t;

typedef enum _isp_read_signature_state {
    ISP_READ_SIG_START,
    ISP_READ_SIG_WAIT_XMIT_1,
    ISP_READ_SIG_XMIT_1_DONE,
    ISP_READ_SIG_START_2,
    ISP_READ_SIG_WAIT_XMIT_2,
    ISP_READ_SIG_XMIT_2_DONE,
    ISP_READ_SIG_START_3,
    ISP_READ_SIG_WAIT_XMIT_3,
    ISP_READ_SIG_XMIT_3_DONE
} isp_read_signature_state_t;

typedef enum _isp_write_low_fuse_state {
    ISP_WRITE_LOW_FUSE_START,
    ISP_WRITE_LOW_FUSE_WAIT_XMIT,
    ISP_WRITE_LOW_FUSE_XMIT_DONE
} isp_write_low_fuse_state_t;

typedef enum _isp_write_high_fuse_state {
    ISP_WRITE_HIGH_FUSE_START,
    ISP_WRITE_HIGH_FUSE_WAIT_XMIT,
    ISP_WRITE_HIGH_FUSE_XMIT_DONE
} isp_write_high_fuse_state_t;

typedef enum _isp_write_ext_fuse_state {
    ISP_WRITE_EXT_FUSE_START,
    ISP_WRITE_EXT_FUSE_WAIT_XMIT,
    ISP_WRITE_EXT_FUSE_XMIT_DONE
} isp_write_ext_fuse_state_t;

typedef enum _isp_read_low_fuse_state {
    ISP_READ_LOW_FUSE_START,
    ISP_READ_LOW_FUSE_WAIT_XMIT,
    ISP_READ_LOW_FUSE_XMIT_DONE
} isp_read_low_fuse_state_t;

typedef enum _isp_read_high_fuse_state {
    ISP_READ_HIGH_FUSE_START,
    ISP_READ_HIGH_FUSE_WAIT_XMIT,
    ISP_READ_HIGH_FUSE_XMIT_DONE
} isp_read_high_fuse_state_t;

typedef enum _isp_read_ext_fuse_state {
    ISP_READ_EXT_FUSE_START,
    ISP_READ_EXT_FUSE_WAIT_XMIT,
    ISP_READ_EXT_FUSE_XMIT_DONE
} isp_read_ext_fuse_state_t;
//--------------------------------------------------
extern const uint32_t isp_atmega16u4_signature;
extern const uint32_t isp_atmega32u4_signature;

//--------------------------------------------------
extern uint8_t isp_tx_buff[ISP_TRANSFER_SIZE];
extern uint8_t isp_rx_buff[ISP_TRANSFER_SIZE];

// isp access lock.
// 0 - lock open. One may use isp
// not 0 - lock closed. Do not use isp
//--------------------------------------------------
extern uint8_t isp_lock;

//--------------------------------------------------
extern uint8_t isp_check_lock(void);
extern uint8_t isp_error(uint8_t err);

//--------------------------------------------------
#endif
