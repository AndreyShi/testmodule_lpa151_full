//--------------------------------------------------
// Работа с загрузчиком
//--------------------------------------------------
#ifndef BOOT_H
#define BOOT_H

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void boot_init(void);
void boot_enter(void);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif