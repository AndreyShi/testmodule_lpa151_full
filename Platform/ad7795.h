//--------------------------------------------------
// Модуль работы с АЦП
//--------------------------------------------------
#ifndef AD7795_H
#define AD7795_H

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

//--------------------------------------------------
#define ADC_1	0
#define ADC_2	1

//--------------------------------------------------
// ADC flags, so anyone can use them
struct _flags {
    uint16_t data_ready:1; // АЦП закончил преобразование

    uint16_t value_ready:1; // можно забирать данные наружу
    uint16_t acc_ready:1;   // сбор значений АЦП завершен

    uint16_t adjust_ready:1; // можно забирать температуру ХС
    uint16_t termo_ready:1;  // можно забирать температуру встроенного датчика
    uint16_t etermo_ready:1; // можно забирать температуру выносного датчика

    uint16_t persist:1; // команда обломалась, надо повторить
    uint16_t error:1;   // тайм-аут ожидания АЦП
    uint16_t valid:2;   // сколько раз подряд пришли данные отличные от 0 и F

    uint16_t accumulation:1; // набор заданного количества отсчетов в аккумулятор
    uint16_t stopped:1;      // признак остановки АЦП
};

//--------------------------------------------------
#ifndef ADC_EX
#define ADC_EX extern
#endif

ADC_EX void adc_init(void);
ADC_EX void adc_task(void);
ADC_EX bool adc_get_value(uint8_t, uint16_t *data);
ADC_EX bool adc_get_acc(uint8_t, uint16_t **data, uint8_t *size);
ADC_EX bool adc_get_adjust(uint8_t, uint16_t *data);
ADC_EX bool adc_get_termo(uint8_t, uint16_t *data);
ADC_EX bool adc_get_etermo(uint8_t, uint16_t *data);
ADC_EX bool adc_start_accumulation(uint8_t, uint8_t);
ADC_EX struct _flags adc_get_status(uint8_t);

ADC_EX int32_t get_resistance(uint8_t, int32_t);
ADC_EX int32_t get_voltage(uint8_t, int32_t);

#endif
//--------------------------------------------------
// waiting for adc conversion ready
//#define ADC1_WAIT EIFR |= (1<<INT0); EIMSK |=  (1<<INT0)/*{{{*/
//#define ADC1_STOP EIMSK &= ~(1<<INT0); EIFR |= (1<<INT0)

//#define ADC2_WAIT EIFR |= (1<<INT1); EIMSK |=  (1<<INT1)
//#define ADC2_STOP EIMSK &= ~(1<<INT1); EIFR |= (1<<INT1)
/*}}}*/
// low-level data/*{{{*/
// adc internal registers
#define ADC_MODE_REG	(1<<ADDR_SHIFT)/*{{{*/
#define ADC_CONF_REG	(2<<ADDR_SHIFT)
#define ADC_DATA_REG	(3<<ADDR_SHIFT)
#define ADC_ID_REG	(4<<ADDR_SHIFT)
#define ADC_IO_REG	(5<<ADDR_SHIFT)
#define ADC_OFFSET_REG	(6<<ADDR_SHIFT)
#define ADC_FSR_REG	(7<<ADDR_SHIFT)/*}}}*/
// comm register
#define ADDR_SHIFT 3/*{{{*/
#define ADC_READ   0x40
#define ADC_WRITE  0x00/*}}}*/
// status bits
#define ST_CH0		0/*{{{*/
#define ST_CH1		1
#define ST_CH2		2
#define ST_ONE		7
#define ST_ZERO		4
#define ST_REF		5
#define ST_ERR		6
#define ST_RDY		3/*}}}*/
// mode register bits
#define FS0	0/*{{{*/
#define FS1	1
#define FS2	2
#define FS3	3
#define CHOP_DIS	4
#define CLK0	6
#define CLK1	7
#define AMP_CM	9
#define PSW	12
#define MD0	13
#define MD1	14
#define MD2	15/*}}}*/
// config register
#define CH0	0/*{{{*/
#define CH1	1
#define CH2	2
#define CH3	3
#define BUF	4
#define REF_DET 5
#define REFSEL0	6
#define REFSEL1	7
#define G0	8
#define G1	9
#define G2	10
#define BOOST	11
#define UnB	12
#define BO	13
#define VBIAS0	14
#define VBIAS1	15/*}}}*/
// io register
#define IXCEN0	0/*{{{*/
#define IXCEN1	1
#define IXCDIR0	2
#define IXCDIR1	3
#define IO1DAT	4
#define IO2DAT	5
#define IOEN	6/*}}}*/
/*}}}*/
// удобные представления/*{{{*/
// предопределенные скорости опроса входа
#define POLL_N 15/*{{{*/
#define FS_470H	(1<<FS0)
#define FS_240H	(1<<FS1)
#define FS_120H	(1<<FS1)|(1<<FS0)
#define FS_60H	(1<<FS2)
#define FS_50H	(1<<FS2)|(1<<FS0)
#define FS_40H	(1<<FS2)|(1<<FS1)
#define FS_33H	(1<<FS2)|(1<<FS1)|(1<<FS0)
#define FS_20H	(1<<FS3)
#define FS_17H	(1<<FS3)|(1<<FS0)
#define FS_16H	(1<<FS3)|(1<<FS1)
#define FS_12H	(1<<FS3)|(1<<FS1)|(1<<FS0)
#define FS_10H	(1<<FS3)|(1<<FS2)
#define FS_8H	(1<<FS3)|(1<<FS2)|(1<<FS0)
#define FS_6H	(1<<FS3)|(1<<FS2)|(1<<FS1)
#define FS_4H	(1<<FS3)|(1<<FS2)|(1<<FS1)|(1<<FS0)/*}}}*/
// предопределенные усиления
#define GAIN_N	 8/*{{{*/
#define GAINx1	 0
#define GAINx2	 (1<<G0)
#define GAINx4	 (1<<G1)
#define GAINx8	 (1<<G1)|(1<<G0)
#define GAINx16  (1<<G2)
#define GAINx32	 (1<<G2)|(1<<G0)
#define GAINx64	 (1<<G2)|(1<<G1)
#define GAINx128 (1<<G2)|(1<<G1)|(1<<G0)/*}}}*/
// типы подключения
#define CM_N	6/*{{{*/
#define TEMP2	(1<<CH1)
#define TEMP3	(1<<CH0)
#define TEMP4	0
#define TEMPC	0
#define VOLTS	0
#define TEMPDS	0
#define TEMPR	 (1<<CH0)|         (1<<CH2)
#define TERMO             (1<<CH1)|(1<<CH2)
#define ETERMO            (1<<CH1)
#define CM_CHAN ((1<<CH0)|(1<<CH1)|(1<<CH2)|(1<<CH3))/*}}}*/
// опоры
#define REF_N	3/*{{{*/
#define REF_R1	(1<<REFSEL0)	// ref2 input
#define REF_R2	0		// ref1 input = ref2/2
#define REF_INT	(1<<REFSEL1)	// 1.17V internal/*}}}*/
/*}}}*/
// в единицах отсчетов АЦП по целевому каналу
#define ADC_ADJ_PASS	25

// в единицах отсчетов АЦП по целевому каналу
#define ADC_TERMO_PASS	25

// в единицах отсчетов АЦП по целевому каналу
#define ADC_ETERMO_PASS 25

// таймер ожидания ответа от АЦП в единицах 0.1мс
#define ADC_WDT_TIME 10000
#define ADC_SETTLE_TIME 5
#define ADC_WARMUP_TIME 10
#define MAX_ACC_SIZE 100

//--------------------------------------------------
enum adc_state {/*{{{*/
    ADC_STATE_IDLE, // состояние-залипуха (из него нет выхода)

    // состояния относящиеся к рабочему каналу
    ADC_STATE_WARMUP,
    ADC_STATE_WARMUP_HOLDOFF,
    ADC_STATE_CHAN_OK,
    ADC_STATE_WAIT_ENB1,
    ADC_STATE_WAIT_DATA1,
    ADC_STATE_GOT_DATA1,

    // состояния относящиеся к датчику температуры холодного спая
    ADC_STATE_CJ_OK, // сконфигурирован для чтения ТХС
    ADC_STATE_WAIT_ENB2, // далее - аналогично
    ADC_STATE_WAIT_DATA2,
    ADC_STATE_GOT_DATA2,

    // состояния относящиеся к встроенному термодатчику
    ADC_STATE_TERMO_OK,
    ADC_STATE_WAIT_ENB3,
    ADC_STATE_WAIT_DATA3,
    ADC_STATE_GOT_DATA3,

    // состояния относящиеся к выносному термодатчику
    ADC_STATE_ETERMO_IO,
    ADC_STATE_ETERMO_OK,
    ADC_STATE_WAIT_ENB4,
    ADC_STATE_WAIT_DATA4,
    ADC_STATE_GOT_DATA4,

    // состояния относящиеся к любому каналу АЦП
    ADC_STATE_MILESTONE, // выбор дальнейшего пути
    ADC_STATE_SET_IO,    // для 3х проводки - переключение токов
    ADC_STATE_WAIT_IO,
    ADC_STATE_UNSET_IO,  // для напряжения нужно отключать токи после etermo

    // переход к измерениям
    ADC_STATE_GO_MAIN,
    ADC_STATE_GO_CJ,
    ADC_STATE_GO_TERMO,
    ADC_STATE_GO_ETERMO,

    // ошибки и вспомогательные режимы
    ADC_STATE_LOST, // АЦП не отвечает
    ADC_STATE_LOST_SENT, // команда сброса ушла
    ADC_STATE_HOLDOFF, // забиваем на АЦП на 500us
    ADC_STATE_CHECK_ID, // завелся?
    ADC_STATE_CONF_OK, // после записи конф. регистра
    ADC_STATE_IO_OK, // после записи ВВ регистра
    ADC_STATE_ZERO_CAL, // запуск калибровки zero-scale
    ADC_STATE_ZERO_CAL_OK, // завершилась калибровка zero-scale
    ADC_STATE_FS_CAL, // запуск калибровки full-scale
    ADC_STATE_FS_CAL_OK // завершилась калибровка zero-scale
};/*}}}*/
//--------------------------------------------------
// one ADC descriptor for everything/*{{{*/
volatile struct {
    enum adc_state state;
    uint16_t conf;
    uint8_t freq;
    uint8_t io;

    uint16_t wdt, settle;
    uint16_t value, old_value; // для 3х проводки

    uint16_t adjust;
    uint8_t counter_adjust;

    uint16_t termo;
    uint8_t counter_termo;

    uint16_t etermo;
    uint8_t counter_etermo;

    uint16_t acc[MAX_ACC_SIZE]; // accumulator
    uint8_t cursor_acc;
    uint8_t counter_acc;

    struct _flags flags;
} adc_device[CHANNELS];/*}}}*/
//--------------------------------------------------
// вспомогательные функции/*{{{*/
// запись в регистры АЦП
uint8_t adc_set_mode(uint8_t device, uint16_t val);
uint8_t adc_set_conf(uint8_t device, uint16_t val);
uint8_t adc_set_io(uint8_t device, uint8_t val);
uint8_t adc_set_offset(uint8_t device, uint16_t val);
uint8_t adc_set_fullscale(uint8_t device, uint16_t val);

// чтение регистров АЦП
uint8_t adc_get_str(uint8_t device);
uint8_t adc_get_mode(uint8_t device);
uint8_t adc_get_conf(uint8_t device);
uint8_t adc_get_data(uint8_t device);
uint8_t adc_get_id(uint8_t device);
uint8_t adc_get_io(uint8_t device);
uint8_t adc_get_offset(uint8_t device);
uint8_t adc_get_fullscale(uint8_t device);

// сброс интерфейса к АЦП
uint8_t adc_cmd_reset(uint8_t);