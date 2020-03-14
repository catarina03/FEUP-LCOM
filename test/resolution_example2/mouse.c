#include "lcf.h"
#include "mouse.h"
#include <stdint.h>

int mouseHookID = MouseIRQ;
uint32_t initStatus = 0;

int mouse_subscribe_int(uint8_t *bit_no)
{
    mouseHookID = (int)*bit_no;

    if (sys_irqsetpolicy(MouseIRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseHookID) != 0)
    {
        return 1;
    }
    *bit_no = (uint8_t)BIT(*bit_no);
    return 0;
}

int mouse_unsubscribe_int()
{
    if (sys_irqrmpolicy(&mouseHookID) != 0)
    {
        return 1;
    }
    return 0;
}

int mouse_ih()
{
    struct pp_packet pp;
    uint32_t mouseData;

    for (uint8_t i = 0; i < 4; i++)
    {
        sys_inb(mouseOutReg, &mouseData);
        pp.bytes[i]= (uint8_t)mouseData;
    }

    pp.x_ov = ((pp.bytes[0] & MOUSE_X_OVF) >> 2);
    pp.y_ov = ((pp.bytes[0] & MOUSE_Y_OVF) >> 3);
    pp.rb = ((pp.bytes[0] & MOUSE_RB) >> 4);
    pp.mb = ((pp.bytes[0] & MOUSE_MB) >> 5);
    pp.lb = ((pp.bytes[0] & MOUSE_LB) >> 6);
    pp.delta_z = pp.bytes[1];
    pp.delta_x = pp.bytes[2];
    pp.delta_y = pp.bytes[3];

    pp_display_packet(&pp);

    return 0;
}

int mouse_init()
{
    uint32_t status;

    sys_inb(mouseStatReg,&status);
    initStatus = status;
    status |= mouseInitState;
    sys_outb(mouseStatReg, status);

    return 0;
}
int mouse_reset()
{
    uint32_t status;

    status = initStatus & mouseDfltState;
    sys_outb(mouseStatReg, status);

    return 0;
}