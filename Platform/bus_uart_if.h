//--------------------------------------------------
// Интерфейс к UART связи с шиной,
// используется для общения с внешним миром
//--------------------------------------------------
#ifndef BUS_UART_H
#define BUS_UART_H

//--------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

//--------------------------------------------------
typedef struct {/*{{{*/
    uint8_t incoming:1;

    uint8_t outgoing:1;
} bus_flags_t;
/*}}}*/
typedef enum {/*{{{*/
    BUS_OK,
    BUS_TOO_LONG,
    BUS_BUSY
} bus_error_t;
/*}}}*/
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void bus_init(void);

bus_flags_t bus_state(void);

void bus_frame_get(char **buff, uint32_t *sz);

void bus_out_buff_get(char **buff);
bus_error_t bus_send(const uint32_t sz);

void bus_gpio_isr(void);
void bus_uart_isr(void);
void bus_tim_isr(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
