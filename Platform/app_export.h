//--------------------------------------------------
// Экспортируемая конфигурация
//--------------------------------------------------
#ifndef TESTMODULE_151_EXPORT_H
#define TESTMODULE_151_EXPORT_H

// Псевдонимы для светодиодов
//--------------------------------------------------
#define TM_151_LED_RED    LED_1
#define TM_151_LED_BLUE   LED_2
#define TM_151_LED_GREEN  LED_3
#define TM_151_LED_YELLOW LED_4
#define TM_151_LED_MAX    LED_5

// Псевдонимы для реле
//--------------------------------------------------
// Закоротка
#define TM_151_RELAY_SHORT RELAY_1 // 0x21

// Подключение резисторов
#define TM_151_RELAY_R1 RELAY_2
#define TM_151_RELAY_R2 RELAY_3
#define TM_151_RELAY_R3 RELAY_4
#define TM_151_RELAY_R4 RELAY_5
#define TM_151_RELAY_R5 RELAY_6
#define TM_151_RELAY_R6 RELAY_7
#define TM_151_RELAY_R7 RELAY_8
#define TM_151_RELAY_R8 RELAY_9

// Подключение источника напряжения
#define TM_151_RELAY_EMUU_POSITIVE RELAY_10
#define TM_151_RELAY_EMUU_NEGATIVE RELAY_11

// Подключение питания ЛПА-151
#define TM_151_RELAY_LPA_POWER RELAY_12
#define TM_151_RELAY_LPA_USB RELAY_13

// Измерение токоизмерительного резистора
#define TM_151_RELAY_SENSE_AG420_1 RELAY_14
#define TM_151_RELAY_SENSE_AG420_2 RELAY_15

// Далее реле с делением на каналы ЛПА-151
// Подключение портов ЛПА-151
#define TM_151_RELAY_LPA_IN_AG_PLUS  RELAY_19
#define TM_151_RELAY_LPA_IN_AG_MINUS RELAY_23

// Подключение каналов Agilent 34420
#define TM_151_RELAY_AG420_1 RELAY_24
#define TM_151_RELAY_AG420_2 RELAY_25

// Подключение терморезистора (любого из двух к любому из двух Agilent 34420)
#define TM_151_RELAY_AG420_1_TR RELAY_26
#define TM_151_RELAY_AG420_2_TR RELAY_27

// Псевдонимы для совместимости с программой управления старой версии
#define TM_151_RELAY_THERMO_1 RELAY_26
#define TM_151_RELAY_THERMO_2 RELAY_27

// Подключение Agilent 34401 (любого из двух к любому из двух каналов)
#define TM_151_RELAY_AG401_1 RELAY_28
#define TM_151_RELAY_AG401_2 RELAY_29

// Подключение прогревочных резисторов
#define TM_151_RELAY_WARMUP RELAY_30

// Подключение токоизмерительного резистора к выходу ЛПА-151
#define TM_151_RELAY_SENSE RELAY_31

// На единицу больше, чем номер максимального рэле в модуле
#define TM_151_RELAY_MAX RELAY_32

// Псевдонимы для ЦАП
//--------------------------------------------------
#define TM_151_DAC DAC_OUTPUT_1

// Версии
//--------------------------------------------------
#define TM_151_VERSION_MODULE VERSION_1
#define TM_151_VERSION_LPA_FW VERSION_2
#define TM_151_VERSION_LPA_BL VERSION_3

// Все данные, что хранятся в модуле
//--------------------------------------------------
#define TM_151_DATA_LOCK           DATA_ID_1
#define TM_151_DATA_FUSE_WORK      DATA_ID_2
#define TM_151_DATA_FUSE_CAL       DATA_ID_3
#define TM_151_DATA_EEPROM_FACTORY DATA_ID_4
#define TM_151_DATA_EEPROM_USER    DATA_ID_5
#define TM_151_DATA_FLASH_WORK_FW  DATA_ID_6
#define TM_151_DATA_FLASH_CAL_FW   DATA_ID_7
#define TM_151_DATA_FLASH_BL       DATA_ID_8
#define TM_151_DATA_CALIBRATIONS   DATA_ID_9

// Всё, что касается скриптов
//--------------------------------------------------
#define TM_151_SCRIPTS_COUNT 4
#define TM_151_SCRIPT_SIZE 2048

// Характерные размеры областей данных
//--------------------------------------------------
#define FLASH_LOCK_SIZE 1
#define FLASH_FUSE_SIZE 3

#define EEPROM_FACTORY_SIZE 300
#define EEPROM_USER_SIZE 724

#define FLASH_FW_SIZE 25*1024 + 256
#define FLASH_BL_SIZE 4*1024

#define FLASH_CALIBRATIONS_PADDING 144
#define FLASH_CALIBRATIONS_ACTIVE_SIZE 11*256 - FLASH_CALIBRATIONS_PADDING

//--------------------------------------------------
#endif
