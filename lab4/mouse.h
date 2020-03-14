
#include <lcom/lcf.h>
#include "i8042.h"


typedef enum { INITIAL, LEFT, MIDDLE,RIGHT, END} state;

struct mouse_state
{
  state st;
  int16_t delta_x;
  int16_t delta_y;
};

extern int hook_mouse;
extern uint8_t byte_mouse;
extern bool status_error_mouse;
extern uint8_t ack_byte;
extern struct mouse_state machine_state;
int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
int kbc_issue_arg(uint8_t cmd);
int kbc_issue_command(uint8_t cmd);
int returnvalue_mouse(uint8_t *ret);
int issue_cmd_mouse(uint8_t mouse_cmd, uint8_t *ack_byte);
int16_t convert(uint8_t delta, bool sign);
struct mouse_ev* (mouse_detect_ev)(struct packet *pp);
void mouse_detect_state(struct mouse_ev* ev, uint8_t x_len, uint8_t tolerance);
int absValue(int value);
