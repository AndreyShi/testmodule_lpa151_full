//--------------------------------------------------
#include <stdio.h>

#include "stm32f7xx_hal.h"

#include "app_export.h"
#include "gpio_if.h"
#include "menu.h"
#include "relay_if.h"
#include "render.h"
#include "resources.h"
#include "usb_device.h"

#include "menu_private.h"
#include "KModuleState151.h"

#include "KBusAddress.h"
#include "KAbstractBusManager.h"
#include "KAbstractEngine.h"

//--------------------------------------------------
typedef struct {
    bool has_digit;
    bool has_letter;

    uint8_t digit;
    uint8_t letter;

    uint8_t dig_x;
    uint8_t dig_y;

    uint8_t ltr_x;
    uint8_t ltr_y;
} failed_stage_t;

//--------------------------------------------------
static bool update;
static menu_item_t *menu;

/* replace the zero with appropriate data pointer */
image_t channels_img =     {  46,  7,  true, 0 };
image_t bg_img =           { 128, 64, false, 0 };

//--------------------------------------------------
static void menu_render_lpa_power(void);
static void menu_render_connection(void);
static void menu_render_usb(void);
//--------------------------------------------------
void menu_init()/*{{{*/
{
update = true;
menu = menu_root;
}/*}}}*/
//--------------------------------------------------
void menu_task()/*{{{*/
{
if( !update )
    { return; }
update = false;

if(menu->render != 0)
    { menu->render(); }
}/*}}}*/
//--------------------------------------------------
void menu_update(void)
{ update = true; }
//--------------------------------------------------
// menu engine functions
//--------------------------------------------------
// private functions
//--------------------------------------------------
static void menu_enter_root(void)
{ }
//--------------------------------------------------
static void menu_render_root(void)/*{{{*/
{
if(menu->cur == 0)
    { bg_img.data = bg_data[BG_ONE]; }
else
    { bg_img.data = bg_data[BG_TWO]; }

render_cls();
render_image(0, 0, false, &bg_img);

menu_render_lpa_power();
menu_render_usb();
menu_render_connection();
}/*}}}*/
//--------------------------------------------------
static void menu_click_start(void)/*{{{*/
{
engine_set_channels(menu->cur + 1);
engine_start_stop();
}/*}}}*/
//--------------------------------------------------
static void menu_render_lpa_power(void)/*{{{*/
{
if(get_lpa_power() == STATE_ON)
    { render_image(59, 2, false, &power_img); }
else
    { render_image(59, 5, false, &disc_img); }
}/*}}}*/
//--------------------------------------------------
static void menu_render_connection(void)/*{{{*/
{
if(bus_address() != ADDR_NOT_SET)
    { render_text(115, 5, false, "%02d", bus_address()); }
else
    { render_image(115, 5, false, &disc_img); }
}/*}}}*/
//--------------------------------------------------
static void menu_render_usb(void)/*{{{*/
{
if(get_lpa_connection() == STATE_ON)
    { render_image(80, 0, false, &usb_img); }
}/*}}}*/
//--------------------------------------------------
