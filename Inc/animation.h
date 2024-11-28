//--------------------------------------------------
// Файлик организующий анимацию картинок на дисплее
//--------------------------------------------------
#ifndef ANIMATION_H
#define ANIMATION_H

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void display_logo(void);
extern void finish_logo(void);

extern void animation_systick_isr(void);

#ifdef __cplusplus
};
#endif
//--------------------------------------------------
#endif
