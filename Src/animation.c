//--------------------------------------------------
// Файлик организующий анимацию картинок на дисплее
//--------------------------------------------------
#include "stm32f7xx_hal.h"

#include "animation.h"
#include "render.h"
#include "resources.h"
#include "ssd1306.h"

//--------------------------------------------------
enum {
    ANIM_IDLE,
    ANIM_GOING,
} static volatile state;
//--------------------------------------------------
static const uint32_t mspf = 20; // 64 frames per second
static const uint8_t  anim_start =  12;
static const uint8_t  anim_end   = 117;
//--------------------------------------------------
static uint8_t  mask;
static uint32_t clock;
//--------------------------------------------------
void display_logo(void)/*{{{*/
{
clock = HAL_GetTick();

mask  = anim_start;
state = ANIM_GOING;
}/*}}}*/
//--------------------------------------------------
void finish_logo(void)/*{{{*/
{
while(state != ANIM_IDLE) { };
HAL_Delay(500);
}/*}}}*/
//--------------------------------------------------
void animation_systick_isr(void)/*{{{*/
{
if(state != ANIM_GOING)
    { return; }

if( !ssd1306_render_now() )
    { return; }

if(mask == anim_end)
    {
    state = ANIM_IDLE;
    return;
    }

if(HAL_GetTick() - clock < mspf)
    { return; }
clock = HAL_GetTick();

render_image(anim_start, 0, false, &logo_img);
for(uint8_t i = mask; i<anim_end; i++)
    {
    ssd_frame_buff[128*0 + i] = 0x00;
    ssd_frame_buff[128*1 + i] = 0x00;
    ssd_frame_buff[128*2 + i] = 0x00;
    ssd_frame_buff[128*3 + i] = 0x00;
    ssd_frame_buff[128*4 + i] = 0x00;
    ssd_frame_buff[128*5 + i] = 0x00;
    ssd_frame_buff[128*6 + i] = 0x00;
    ssd_frame_buff[128*7 + i] = 0x00;
    }

mask++;
}/*}}}*/
//--------------------------------------------------
