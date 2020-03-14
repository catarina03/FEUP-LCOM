
#include <lcom/lcf.h>
#include "i8042.h"
extern uint8_t scancode;
extern int count;
extern int hook_kbd;
extern bool status_error;
extern int counter;

int (kbd_subscribe_int)(uint8_t *bit_no);

int (kbd_unsubscribe_int)();

void (kbc_ih)();

int (kbd_enable)();

int (kbd_disable)();
