#ifndef _I8042_H_
#define _I8042_H_

#include <lcom/lcf.h>

#define KBD_IRQ 1

//KBC registers
#define STATUS_REGISTER 0x64
#define OUT_BUF 0X60
#define IN_BUF 0X64
#define KBC_CMD_ARGS 0x60


//KBC commands
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60

//KBC command byte bits
#define ENABLE_KBD_OBF BIT(0)

//important key scancodes
#define ERROR 0xff
#define ESC_BRK_CODE 0x81
#define SECOND_BYTE_CODE 0xe0
#define BREAK_CODE BIT(7)

//status register bits
#define PARITY_ERROR BIT(7)
#define TIMEOUT_ERROR BIT(6)
#define MOUSE_ACTIVE BIT(5)
#define IBF BIT(1)
#define OBF BIT(0)


#define DELAY 20000

#endif
