//--------------------------------------------------
// Модуль работы с АЦП
//--------------------------------------------------
#include "app_config.h"
#include "emu_u.h"
#include "gpio_if.h"
#include "spi.h"

#include "KModuleState151.h"

#include "ad7795.h"

#include "debug.h"

//--------------------------------------------------
// {{{
const uint8_t refs[REF_N]={
    REF_R1,
    REF_R2,
    REF_INT
};

const uint8_t modes[CM_N]={
    TEMP2,
    TEMP3,
    TEMP4,
    TEMPC,
    VOLTS,
    TEMPDS,
};

const uint8_t io[CM_N]={
    (1<<IXCEN0)|(1<<IXCEN1)|0           |0           ,
    (1<<IXCEN0)|(1<<IXCEN1)|0           |0           ,
    (1<<IXCEN0)|(1<<IXCEN1)|0           |(1<<IXCDIR1), /* !it is a bad practice! */
    (1<<IXCEN0)|(1<<IXCEN1)|(1<<IXCDIR0)|(1<<IXCDIR1), /* !it is a bad practice! */
    0
};

const uint16_t gains[GAIN_N]={
    GAINx1,
    GAINx2,
    GAINx4,
    GAINx8,
    GAINx16,
    GAINx32,
    GAINx64,
    GAINx128
};

const uint8_t polls[POLL_N]={
    FS_4H, FS_6H,
    FS_8H,
    FS_10H, FS_12H,
    FS_16H, FS_17H,
    FS_20H, FS_33H,
    FS_40H, FS_50H,
    FS_60H,
    FS_120H, FS_240H,
    FS_470H
};// }}}
//--------------------------------------------------
void adc_init(void)/*{{{*/
{
// прерывания/*{{{*/
DDRD  &= ~(1<<DDD0)&~(1<<DDD1);
PORTD |=  (1<<PD0) |(1<<PD1);
EICRA = (1<<ISC11)|(1<<ISC01);
ADC1_STOP;
ADC2_STOP;
/*}}}*/
// инициализация
for(uint8_t i=0; i<CHANNELS; i++)
    {
    adc_device[i].flags.data_ready  = 0;
    adc_device[i].flags.value_ready = 0;
    adc_device[i].flags.acc_ready   = 0;
    adc_device[i].flags.adjust_ready = 0;
    adc_device[i].flags.termo_ready  = 0;
    adc_device[i].flags.etermo_ready = 0;
    adc_device[i].flags.persist = 0;
    adc_device[i].flags.error   = 1;
    adc_device[i].flags.valid   = 0;
    adc_device[i].flags.accumulation = 0;
    adc_device[i].flags.stopped = 0;

    switch(adc_conn_index[i])/*{{{*/
	{
	case 0:
	case 1:
	case 2:
	    adc_device[i].conf=modes[adc_conn_index[i]] |
		refs[adc_ref_index[i]] |
		gains[adc_gain_index[i]] |
		(1<<UnB);
	    break;

	default:
	    // turn down adc polling and *fall-through*
	    adc_device[i].flags.stopped = 1;
	case 3:
	case 4:
	    adc_device[i].conf=modes[adc_conn_index[i]] |
		refs[2] |
		gains[adc_gain_index[i]] |
		(1<<BOOST);
		//(1<<VBIAS0);
	    break;

	case 5:
	    adc_device[i].conf=TEMPR |
		REF_INT | (1<<UnB) | (1<<BUF);
	    break;
	};
    /*}}}*/
    adc_device[i].freq=polls[adc_freq_index[i]];
    adc_device[i].io=io[adc_conn_index[i]];

    adc_device[i].state=ADC_STATE_LOST;
    adc_device[i].wdt=ticks;
    adc_device[i].settle=ticks;

    adc_device[i].counter_adjust = 0;
    adc_device[i].counter_termo  = 0;
    adc_device[i].counter_etermo = 0;
    }
}/*}}}*/
//--------------------------------------------------
void adc_task(void)/*{{{*/
{
uint8_t buff[SPI_BUFF_SIZE];
uint16_t tmp=0;
 int32_t tmp1=0;
for(uint8_t i=0; i<CHANNELS; i++)
    {
    if(check_time_elapsed(adc_device[i].wdt, ADC_WDT_TIME))
	{ adc_device[i].state=ADC_STATE_LOST; }
 
    switch(adc_device[i].state)
	{
	case ADC_STATE_WARMUP:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    tmp=(1<<MD1)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    //tmp=(1<<MD1)|(1<<AMP_CM)| adc_device[i].freq;
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_WARMUP_HOLDOFF;
		}
	    cli();
	    adc_device[i].settle=ticks;
	    sei();
	    break;
	    /*}}}*/
	case ADC_STATE_WARMUP_HOLDOFF:/*{{{*/
	    if(!check_time_elapsed(adc_device[i].settle, ADC_WARMUP_TIME))
		break;
	    adc_device[i].state=ADC_STATE_CHAN_OK;
	    break;
	    /*}}}*/
	case ADC_STATE_CHAN_OK:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;
	    tmp=(1<<MD0)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    //tmp=(1<<MD0)|(1<<AMP_CM)| adc_device[i].freq;
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_WAIT_ENB1;
		}
	    break;/*}}}*/
	case ADC_STATE_WAIT_ENB1:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;
	    if(i == 0)
		{ ADC1_WAIT; }
	    else if(i == 1)
		{ ADC2_WAIT; }
	    adc_device[i].state=ADC_STATE_WAIT_DATA1;
	    break;/*}}}*/
	case ADC_STATE_WAIT_DATA1:/*{{{*/
	    if(adc_device[i].flags.data_ready == 0)
		break;

	    if(adc_get_data(i) == SPI_OK)
		adc_device[i].state=ADC_STATE_GOT_DATA1;
	    break;/*}}}*/
	case ADC_STATE_GOT_DATA1:/*{{{*/
	    if(spi_read(i, buff) != SPI_OK)
		break;

	    tmp = buff[1];
	    tmp = (tmp << 8) | buff[2];
	    if(adc_conn_index[i] != 1)
		{ adc_device[i].value = tmp; }
	    else
		{
		if(tmp == 0xFFFF || tmp == 0x0000)
		    {
		    adc_device[i].flags.valid = 0;

		    if(adc_device[i].flags.accumulation == 1)
			{
			adc_device[i].flags.accumulation = 0;
			adc_device[i].flags.acc_ready = 1;
			}
		    }
		else if(adc_device[i].flags.valid == 0)
		    { adc_device[i].flags.valid = 1; }
		else if(adc_device[i].flags.valid == 1)
		    { adc_device[i].flags.valid = 2; }

		tmp1 = tmp;
		tmp1 += adc_device[i].old_value;
		tmp1 /= 2;
		adc_device[i].old_value = tmp;
		adc_device[i].value = tmp1;
		}

	    if(adc_conn_index[i] == 1 && (adc_device[i].flags.valid == 0 ||
					       adc_device[i].flags.valid == 1))
		{ adc_device[i].flags.value_ready = 0; }
	    else
		{
		adc_device[i].flags.value_ready = 1;
		if(adc_device[i].flags.accumulation == 1)
		    {
		    adc_device[i].acc[ adc_device[i].cursor_acc ] = adc_device[i].value;
		    adc_device[i].cursor_acc++;
		    if(adc_device[i].cursor_acc >= adc_device[i].counter_acc)
			{
			adc_device[i].flags.accumulation = 0;
			adc_device[i].flags.acc_ready = 1;
			}
		    }
		}

	    adc_device[i].flags.error = 0;
	    adc_device[i].state = ADC_STATE_MILESTONE;
	    break;/*}}}*/

	case ADC_STATE_CJ_OK:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    tmp=(1<<MD0)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    //tmp=(1<<MD0)|(1<<CHOP_DIS)| adc_device[i].freq; // #2
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_WAIT_ENB2;
		}
	    break;/*}}}*/
	case ADC_STATE_WAIT_ENB2:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;
	    if(i == 0)
		{ ADC1_WAIT; }
	    else if(i == 1)
		{ ADC2_WAIT; }
	    adc_device[i].state=ADC_STATE_WAIT_DATA2;
	    break;/*}}}*/
	case ADC_STATE_WAIT_DATA2:/*{{{*/
	    if(adc_device[i].flags.data_ready == 0)
		break;

	    if(adc_get_data(i) == SPI_OK)
		adc_device[i].state=ADC_STATE_GOT_DATA2;
	    break;/*}}}*/
	case ADC_STATE_GOT_DATA2:/*{{{*/
	    if(spi_read(i, buff) != SPI_OK)
		break;

	    tmp=buff[1];
	    tmp=(tmp << 8) | buff[2];
	    adc_device[i].adjust=tmp;
	    adc_device[i].flags.adjust_ready=1;
	    adc_device[i].flags.error=0;
	    adc_device[i].state=ADC_STATE_MILESTONE;
	    break;/*}}}*/

	case ADC_STATE_TERMO_OK:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    tmp=(1<<MD0)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_WAIT_ENB3;
		}
	    break;/*}}}*/
	case ADC_STATE_WAIT_ENB3:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;
	    if(i == 0)
		{ ADC1_WAIT; }
	    else if(i == 1)
		{ ADC2_WAIT; }
	    adc_device[i].state=ADC_STATE_WAIT_DATA3;
	    break;/*}}}*/
	case ADC_STATE_WAIT_DATA3:/*{{{*/
	    if(adc_device[i].flags.data_ready == 0)
		break;

	    if(adc_get_data(i) == SPI_OK)
		adc_device[i].state=ADC_STATE_GOT_DATA3;
	    break;/*}}}*/
	case ADC_STATE_GOT_DATA3:/*{{{*/
	    if(spi_read(i, buff) != SPI_OK)
		break;

	    tmp=buff[1];
	    tmp=(tmp << 8) | buff[2];
	    adc_device[i].termo=tmp;
	    adc_device[i].flags.termo_ready=1;
	    adc_device[i].flags.error=0;
	    adc_device[i].state=ADC_STATE_MILESTONE;
	    break;/*}}}*/

	case ADC_STATE_ETERMO_OK:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    tmp=(1<<MD0)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_WAIT_ENB4;
		}
	    break;/*}}}*/
	case ADC_STATE_WAIT_ENB4:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;
	    if(i == 0)
		{ ADC1_WAIT; }
	    else if(i == 1)
		{ ADC2_WAIT; }
	    adc_device[i].state=ADC_STATE_WAIT_DATA4;
	    break;/*}}}*/
	case ADC_STATE_WAIT_DATA4:/*{{{*/
	    if(adc_device[i].flags.data_ready == 0)
		break;

	    if(adc_get_data(i) == SPI_OK)
		adc_device[i].state=ADC_STATE_GOT_DATA4;
	    break;/*}}}*/
	case ADC_STATE_GOT_DATA4:/*{{{*/
	    if(spi_read(i, buff) != SPI_OK)
		break;

	    tmp=buff[1];
	    tmp=(tmp << 8) | buff[2];
	    adc_device[i].etermo=tmp;
	    adc_device[i].flags.etermo_ready=1;
	    adc_device[i].flags.error=0;
	    adc_device[i].state=ADC_STATE_MILESTONE;
	    break;/*}}}*/

	case ADC_STATE_MILESTONE:/*{{{*/
	    cli();
	    adc_device[i].wdt=ticks;
	    sei();

	    if(adc_device[i].flags.stopped == 1)
		{
		adc_device[i].state = ADC_STATE_LOST;
		break;
		}

	    if(adc_conn_index[i] == 1)
		{ adc_device[i].state = ADC_STATE_SET_IO; }
	    else
		{ adc_device[i].state = ADC_STATE_GO_MAIN; }

	    // смотрим куда идти дальше
	    /*
	    adc_device[i].counter_adjust++;
	    adc_device[i].counter_termo++;
	    adc_device[i].counter_etermo++;
	    */

	    /*
	    if(adc_device[i].counter_adjust >= ADC_ADJ_PASS)
		{ adc_device[i].state = ADC_STATE_GO_CJ; }
	    else if(adc_device[i].counter_termo >= ADC_TERMO_PASS)
		{ adc_device[i].state = ADC_STATE_GO_TERMO; }
	    else if(adc_device[i].counter_etermo >= ADC_ETERMO_PASS)
		{ adc_device[i].state = ADC_STATE_GO_ETERMO; }
	    else if(adc_conn_index[i] == 1)
		{ adc_device[i].state = ADC_STATE_SET_IO; }
	    else if(adc_device[i].counter_etermo == 1)
		{ adc_device[i].state = ADC_STATE_UNSET_IO; }
	    else
		{ adc_device[i].state = ADC_STATE_GO_MAIN; }
		*/
	    break;/*}}}*/
	case ADC_STATE_SET_IO:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		cli();
		adc_device[i].wdt = ticks;
		sei();

		// only toggle currents once per measurement
		if((adc_device[i].io & (1<<IXCDIR0)) == (1<<IXCDIR0))
		    adc_device[i].io = adc_device[i].io & ~(1<<IXCDIR0);
		else
		    adc_device[i].io = adc_device[i].io |  (1<<IXCDIR0);
		}

	    if(adc_set_io(i, adc_device[i].io) == SPI_OK)
		{
		adc_device[i].flags.persist = 0;
		adc_device[i].state = ADC_STATE_WAIT_IO;
		}
	    else
		adc_device[i].flags.persist = 1;
	    break;/*}}}*/
	case ADC_STATE_WAIT_IO:/*{{{*/
	    if(spi_read(i, buff) != SPI_OK)
		break;

	    adc_device[i].state = ADC_STATE_GO_MAIN;
	    break;/*}}}*/
	case ADC_STATE_UNSET_IO:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		cli();
		adc_device[i].wdt = ticks;
		sei();
		}

	    // etermo enables current sources, disable them here
	    if(adc_set_io(i, adc_device[i].io) == SPI_OK)
		{
		adc_device[i].flags.persist = 0;
		adc_device[i].state = ADC_STATE_WAIT_IO;
		}
	    else
		{ adc_device[i].flags.persist = 1; }
	    break;/*}}}*/

	case ADC_STATE_GO_MAIN:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		cli();
		adc_device[i].wdt = ticks;
		sei();
		}

	    if(adc_set_conf(i, adc_device[i].conf) == SPI_OK)
		{
		adc_device[i].flags.persist = 0;
		adc_device[i].state = ADC_STATE_WARMUP;
		}
	    else
		adc_device[i].flags.persist = 1;
	    break;/*}}}*/
	case ADC_STATE_GO_CJ:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		cli();
		adc_device[i].wdt=ticks;
		sei();
		}

	    tmp=TEMPR | REF_INT | (1<<UnB) | (1<<BUF);
	    //tmp=TEMPR | REF_INT | (1<<UnB); // #1
	    if(adc_set_conf(i, tmp) == SPI_OK)
		{
		adc_device[i].counter_adjust=0;
		adc_device[i].flags.persist=0;
		adc_device[i].state=ADC_STATE_CJ_OK;
		}
	    else
		adc_device[i].flags.persist=1;
	    break;/*}}}*/
	case ADC_STATE_GO_TERMO:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		cli();
		adc_device[i].wdt=ticks;
		sei();
		}

	    tmp=TERMO | REF_INT | (1<<BUF);
	    if(adc_set_conf(i, tmp) == SPI_OK)
		{
		adc_device[i].counter_termo=0;
		adc_device[i].flags.persist=0;
		adc_device[i].state=ADC_STATE_TERMO_OK;
		}
	    else
		adc_device[i].flags.persist=1;
	    break;/*}}}*/
	case ADC_STATE_GO_ETERMO:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		cli();
		adc_device[i].wdt = ticks;
		sei();
		}

	    tmp = (1<<IXCEN0)|(1<<IXCEN1);
	    if(adc_set_io(i, (uint8_t)tmp) == SPI_OK)
		{
		adc_device[i].counter_etermo = 0;
		adc_device[i].flags.persist = 0;
		adc_device[i].state = ADC_STATE_ETERMO_IO;
		}
	    else
		{ adc_device[i].flags.persist=1; }
	    break;
/*}}}*/
	case ADC_STATE_ETERMO_IO:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		{ break; }

	    tmp=ETERMO | REF_R1 | (1<<UnB);
	    if(adc_set_conf(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.persist=0;
		adc_device[i].state=ADC_STATE_ETERMO_OK;
		}
	    else
		adc_device[i].flags.persist=1;
	    break;/*}}}*/

	case ADC_STATE_LOST:/*{{{*/
	    cli();
	    adc_device[i].wdt=ticks;
	    adc_device[i].flags.error=1;
	    adc_device[i].flags.value_ready=0;
	    adc_device[i].flags.adjust_ready=0;
	    sei();
	    spi_flush(i);
	    if(adc_cmd_reset(i) == SPI_OK)
		adc_device[i].state=ADC_STATE_LOST_SENT;
	    break;/*}}}*/
	case ADC_STATE_LOST_SENT:/*{{{*/
	    if(spi_flush(i) == SPI_BUSY)
		break;
	    cli();
	    adc_device[i].settle=ticks;
	    sei();
	    adc_device[i].state=ADC_STATE_HOLDOFF;
	    break;/*}}}*/
	case ADC_STATE_HOLDOFF:/*{{{*/
	    if(!check_time_elapsed(adc_device[i].settle, ADC_SETTLE_TIME))
		break;
	    if(adc_device[i].flags.stopped == 1)
		{ break; }

	    if(adc_get_id(i) == SPI_OK)
		{
		adc_device[i].flags.persist=0;
		adc_device[i].state=ADC_STATE_CHECK_ID;
		}
	    break;/*}}}*/
	case ADC_STATE_CHECK_ID:/*{{{*/
	    if(adc_device[i].flags.persist == 0)
		{
		if(spi_read(i, buff) != SPI_OK)
		    break;
		if(buff[1] == 0xFF ||
		   buff[1] == 0x00)
		    {
		    adc_device[i].state=ADC_STATE_LOST;
		    break;
		    }
		}
	    if(adc_set_conf(i, adc_device[i].conf) == SPI_OK)
		{
		adc_device[i].flags.persist=0;
		adc_device[i].state=ADC_STATE_CONF_OK;
		}
	    else
		adc_device[i].flags.persist=1;
	    break;/*}}}*/
	case ADC_STATE_CONF_OK:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;
	    if(adc_set_io(i, adc_device[i].io) == SPI_OK)
		adc_device[i].state=ADC_STATE_IO_OK;
	    break;/*}}}*/
	case ADC_STATE_IO_OK:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    tmp=(1<<MD2)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_CHAN_OK;
		}
	    break;/*}}}*/

	case ADC_STATE_ZERO_CAL:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    if(i == 0)
		{ ADC1_WAIT; }
	    else if(i == 1)
		{ ADC2_WAIT; }

	    adc_device[i].state=ADC_STATE_ZERO_CAL_OK;
	    break;/*}}}*/
	case ADC_STATE_ZERO_CAL_OK:/*{{{*/
	    if(adc_device[i].flags.data_ready == 0)
		break;

	    tmp=(1<<MD2)|(1<<MD0)|(1<<AMP_CM)|(1<<CHOP_DIS)| adc_device[i].freq;
	    if(adc_set_mode(i, tmp) == SPI_OK)
		{
		adc_device[i].flags.data_ready=0;
		adc_device[i].state=ADC_STATE_FS_CAL;
		}
	    break;/*}}}*/
	case ADC_STATE_FS_CAL:/*{{{*/
	    if(spi_flush(i) != SPI_OK)
		break;

	    if(i == 0)
		{ ADC1_WAIT; }
	    else if(i == 1)
		{ ADC2_WAIT; }

	    adc_device[i].state=ADC_STATE_FS_CAL_OK;
	    break;/*}}}*/
	case ADC_STATE_FS_CAL_OK:/*{{{*/
	    if(adc_device[i].flags.data_ready == 0)
		break;

	    // dummy command to let it go on
	    if(adc_get_id(i) == SPI_OK)
		{
		adc_device[i].flags.persist=0;
		adc_device[i].state=ADC_STATE_CHAN_OK;
		}
	    break;/*}}}*/

	default:/*{{{*/
	case ADC_STATE_IDLE:
	    break;/*}}}*/
	};
    }
}/*}}}*/
//--------------------------------------------------
bool adc_get_value(uint8_t device, uint16_t *data)/*{{{*/
{
if(adc_device[device].flags.value_ready == 0)
    { return false; }

adc_device[device].flags.value_ready=0;
*data=adc_device[device].value;
return true;
}/*}}}*/
//--------------------------------------------------
bool adc_get_acc(uint8_t device, uint16_t **data, uint8_t *size)/*{{{*/
{
if(adc_device[device].flags.acc_ready == 0)
    { return false; }

*data = (uint16_t *)adc_device[device].acc;
*size = adc_device[device].cursor_acc;
return true;
}/*}}}*/
//--------------------------------------------------
bool adc_get_adjust(uint8_t device, uint16_t *data)/*{{{*/
{
if(adc_device[device].flags.adjust_ready == 0)
    return false;

adc_device[device].flags.adjust_ready=0;
*data=adc_device[device].adjust;
return true;
}
/*}}}*/
//--------------------------------------------------
bool adc_get_termo(uint8_t device, uint16_t *data)/*{{{*/
{
if(adc_device[device].flags.termo_ready == 0)
    return false;

adc_device[device].flags.termo_ready=0;
*data=adc_device[device].termo;
return true;
}
/*}}}*/
//--------------------------------------------------
bool adc_get_etermo(uint8_t device, uint16_t *data)/*{{{*/
{
if(adc_device[device].flags.etermo_ready == 0)
    { return false; }

adc_device[device].flags.etermo_ready=0;
*data=adc_device[device].etermo;
return true;
}
/*}}}*/
//--------------------------------------------------
bool adc_start_accumulation(uint8_t device, uint8_t limit)/*{{{*/
{
bool retval;
uint8_t dev;

retval = true;
dev = device;
if(dev > 1)
    {
    retval = adc_start_accumulation(0, limit);
    dev = 1;
    }

if( !retval || adc_device[dev].flags.accumulation )
    { return false; }

adc_device[dev].cursor_acc = 0;
adc_device[dev].counter_acc = limit;
adc_device[dev].flags.acc_ready = 0;
adc_device[dev].flags.accumulation = 1;
return true;
}/*}}}*/
//--------------------------------------------------
struct _flags adc_get_status(uint8_t device)
{ return adc_device[device].flags; } 
//--------------------------------------------------
float get_physical(uint8_t index, uint16_t value)/*{{{*/
{
float tmp=value*1.0;
return tmp*adc_k[index]+adc_b[index];
}/*}}}*/
//--------------------------------------------------
// вспомогательные функции/*{{{*/
uint8_t adc_set_mode(uint8_t device, uint16_t val)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_WRITE | ADC_MODE_REG;
buff[1]=(val>>8)&0xFF;
buff[2]=(val   )&0xFF;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_set_conf(uint8_t device, uint16_t val)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_WRITE | ADC_CONF_REG;
buff[1]=(val>>8)&0xFF;
buff[2]=(val   )&0xFF;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_set_io(uint8_t device, uint8_t val)/*{{{*/
{
const uint8_t buff_lim=2;
uint8_t buff[buff_lim];

buff[0]=ADC_WRITE | ADC_IO_REG;
buff[1]=val;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_set_offset(uint8_t device, uint16_t val)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_WRITE | ADC_OFFSET_REG;
buff[1]=(val>>8)&0xFF;
buff[2]=(val   )&0xFF;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_set_fullscale(uint8_t device, uint16_t val)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_WRITE | ADC_FSR_REG;
buff[1]=(val>>8)&0xFF;
buff[2]=(val   )&0xFF;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_str(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=2;
uint8_t buff[buff_lim];

buff[0]=ADC_READ;
buff[1]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_mode(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_MODE_REG;
buff[1]=0x00;
buff[2]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_conf(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_CONF_REG;
buff[1]=0x00;
buff[2]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_data(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_DATA_REG;
buff[1]=0x00;
buff[2]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_id(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=2;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_ID_REG;
buff[1]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_io(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=2;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_IO_REG;
buff[1]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_offset(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_OFFSET_REG;
buff[1]=0x00;
buff[2]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_get_fullscale(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=3;
uint8_t buff[buff_lim];

buff[0]=ADC_READ | ADC_FSR_REG;
buff[1]=0x00;
buff[2]=0x00;
return spi_send(device, buff, buff_lim);
}/*}}}*/
//--------------------------------------------------
uint8_t adc_cmd_reset(uint8_t device)/*{{{*/
{
const uint8_t buff_lim=4;
uint8_t buff[buff_lim];
buff[0]=0xFF;
buff[1]=0xFF;
buff[2]=0xFF;
buff[3]=0xFF;
return spi_send(device, buff, buff_lim);
}
/*}}}*//*}}}*/
//--------------------------------------------------
ISR(INT0_vect)/*{{{*/
{
ADC1_STOP;
adc_device[0].flags.data_ready=1;
}/*}}}*/
//--------------------------------------------------
ISR(INT1_vect)/*{{{*/
{
ADC2_STOP;
adc_device[1].flags.data_ready=1;
}/*}}}*/
//--------------------------------------------------
