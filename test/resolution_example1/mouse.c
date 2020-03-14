#include <stdint.h>
#include "mouse.h"

static p_packet pp;
static uint8_t mouse_hook_id = 0;

int util_sys_inb(int port, uint8_t *value){
    uint32_t temp;
    if (sys_inb(port, &temp) != 0)
        return 1;
    *value = (uint8_t)temp;
    return 0;
}

int mouse_subscribe_int(uint8_t *bit_no){
    mouse_hook_id = *bit_no;
    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id))
        return 1;
    return 0;
}

int set_cw_1(){
    uint8_t cw;
    if (util_sys_inb(STATUS_REGISTER, &cw))
        return 1;
    cw |= STREAM_MODE;
    cw |= NR_INT;
    cw |= NR_PACK;
    if (sys_outb(STATUS_REGISTER, cw))
        return 1;
    return 0;
}

int mouse_ih_1(){
    uint8_t cont;
    for(unsigned int i = 0; i < 4; i++){
        if (util_sys_inb(OUT_BUF_REG, &cont))
            return 1;
        pp.bytes[i] = cont;
    }
    return 0;
}

void build_packet_1(){
    pp.rb = pp.bytes[0] & RB;
    pp.mb = pp.bytes[0] & MB;
    pp.lb = pp.bytes[0] & LB;
    pp.x_ov = pp.bytes[0] & X_OV;
    pp.y_ov = pp.bytes[0] & Y_OV;
    pp.delta_z = pp.bytes[3];
    if (pp.delta_z % MSB_Z)
        pp.delta_z |= 0xFF00;
    pp.delta_y = pp.bytes[2];
    if (pp.delta_y & MSB_Y)
        pp.delta_y |= 0xFF00;
    pp.delta_x = pp.bytes[1];
    if (pp.delta_x & MSB_X)
        pp.delta_x |= 0xFF00;
}

int set_cw_2(){
    uint8_t cw;
    if (util_sys_inb(STATUS_REGISTER, cw))
        return 1;
    cw |= STREAM_MODE;
    cw &= ~NR_INT;
    cw &= ~NR_PACK;
    if (sys_outb(STATUS_REGISTER, cw))
        return 1;
    return 0;
}

int mouse_ih_2(){
    static int count = 0;
    uint8_t cont;

    if (util_sys_inb(OUT_BUF_REG, cont))
        return 1;
    pp.bytes[count] = cont;
    count++;
    if (count == 3){
        count = 0;
        return 0;
    }
    else
        return 1;
}

void build_packet_2(){
    pp.rb = pp.bytes[0] & RB;
    pp.mb = pp.bytes[0] & MB;
    pp.lb = pp.bytes[0] & LB;
    pp.x_ov = pp.bytes[0] & X_OV;
    pp.y_ov = pp.bytes[0] & Y_OV;
    pp.delta_z = 0;
    pp.delta_y = pp.bytes[2];
    if (pp.delta_y & MSB_Y)
        pp.delta_y |= 0xFF00;
    pp.delta_x = pp.bytes[1];
    if (pp.delta_x & MSB_X)
        pp.delta_x |= 0xFF00;
}

p_packet get_packet(){
    return pp;
}

int reset_cw(){
    uint8_t cw;
    if (util_sys_inb(STATUS_REGISTER, &cw))
        return 1;
    cw &= ~STREAM_MODE;
    cw &= ~NR_INT;
    cw &= ~NR_PACK;
    if (sys_outb(STATUS_REGISTER), cw))
        return 1;
    return 0;
}

int mouse_unsubscribe_int(){
    if(sys_irqrmpolicy(&mouse_hook_id))
        return 1;
    return 0;
}




