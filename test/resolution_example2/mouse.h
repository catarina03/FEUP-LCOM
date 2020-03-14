#define MouseIRQ 12;
#define mouseStatReg 0x64;
#define mouseOutReg 0x60;
#define mouseInitState 0x89;
#define mouseDfltState 0x3E;

int mouse_init();
int mouse_ih();
int mouse_reset();
int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();
