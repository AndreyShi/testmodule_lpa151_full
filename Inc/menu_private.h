//--------------------------------------------------
// Приватная часть механизма реализации меню
//--------------------------------------------------
#ifndef MENU_PRIVATE_H
#define MENU_PRIVATE_H

//--------------------------------------------------
// элемент меню содержит всё необходимое для его
// отрисовки и переходов
//--------------------------------------------------
typedef struct _menu_item {
    const uint8_t id; // item id, for convenience
    const uint16_t max;
    const bool is_click_base_array;

    uint16_t cur;

    struct _menu_item *click_base; // must point to an array menu_items[max], or zero if no-op
    struct _menu_item *long_base;  // same

    void (*enter)(void);  // action prior to rendering. Update cursor to current values of state variables
    void (*render)(void); // render function
    void (*click)(void);  // click action
    void (*leave)(void);  // what to do before leaving. Update state variables
} menu_item_t;

//--------------------------------------------------
static const uint16_t step = 22; // 0xC00/140, ticks per .1mA

//--------------------------------------------------
static void menu_enter_root(void);
static void menu_render_root(void);

static void menu_click_start(void);
static void menu_click_stop(void);
static void menu_click_barr(void);

//--------------------------------------------------
static struct _menu_item menu_root[];

static struct _menu_item menu_root[] = {
	// channel selection
	// left-right changes channel count
	{ 0, 1, false, 0,
	0, &(menu_root[1]), // not leaving this state automatically, only by engine state change from IDLE
	menu_enter_root,
	menu_render_root, menu_click_start,
	0 } 
};
//--------------------------------------------------
#endif
