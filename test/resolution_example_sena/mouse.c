#include "mouse.h"
//resto dos includes
 
static int mouse_hookId = 0;
static p_packet pack;
 
 
int (util_sys_inb)(int port, uint8_t *value) {//reads the value from port into a uint8_t variable 
  uint32_t temp;
  if (sys_inb(port, &temp)){
    printf("Error in the sys_inb\n");
    return 1;
  }
  *value = (uint8_t)temp;
  return 0;
}
 
int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = mouse_hookId;
  //*bit_no = BIT(hookid);
  if (sys_irqsetpolicy(IRQ_MOUSE, (IRQ_REENABLE | IRQ_EXCLUSIVE), &mouse_hookId)) {
    printf("Subscribe notification failed!\n");
    return 1;
  }
  return 0;
}
 
void(pack_builder_1)() {
  pack->lb = (pack->bytes[0] & LB);       //same as BIT(0)
  pack->rb = (pack->bytes[0] & RB);       //same as BIT(1)
  pack->mb = (pack->bytes[0] & MB);       //same as BIT(2)
  pack->x_ov = (pack->bytes[0] & X_OV);   //same as BIT(6)
  pack->y_ov = (pack->bytes[0] & Y_OV);   //same as BIT(7)
  pack->delta_z = pack->bytes[1];
  if (pack->bytes[0] & MSB_Z)
    pack->delta_z |= 0xFF00;
  pack->delta_x = pack->bytes[2];
  if (pack->bytes[0] & MSB_X)
    pack->delta_x |= 0xFF00;
  pack->delta_y = pack->bytes[3];
  if (pack->bytes[0] & MSB_Y)
    pack->delta_y |= 0xFF00;
}
 
void(pack_builder_2)() {
  pack->lb = (pack->bytes[0] & LB);       //same as BIT(0)
  pack->rb = (pack->bytes[0] & RB);       //same as BIT(1)
  pack->mb = (pack->bytes[0] & MB);       //same as BIT(2)
  pack->x_ov = (pack->bytes[0] & X_OV);   //same as BIT(7)
  pack->y_ov = (pack->bytes[0] & Y_OV);   //same as BIT(7)
  pack->delta_z = 0;
  pack->delta_x = pack->bytes[1];
  if (pack->bytes[0] & MSB_X)
    pack->delta_x |= 0xFF00;
  pack->delta_y = pack->bytes[2];
  if (pack->bytes[0] & MSB_Y)
    pack->delta_y |= 0xFF00;
}
 
void mouse_ih_1(){
    uint8_t cont;
 
    for (unsigned int=0; i<4;i++){
        if(util_sys_inb(OUT_BUF_REG,cont)) //inutil mas caguei
          return;
        pack->bytes[i]=cont;
    } 
}
 
 
int mouse_ih_2(){
    static int count=0;
    uint8_t cont;
 
    if(util_sys_inb(OUT_BUF_REG,cont)) //inutil mas caguei
       return 1;
    pack->bytes[count]=cont;
    count++;
    if(count==3){
        count=0;
        return 0;
    }
    else
      return 1;
}
 
 
int set_cw_1(){
    uint8_t cw;
    util_sys_inb(CMD_REG,cw);
 
    cw|=STREAM_MODE;
    cw|=NR_INT;
    cw|=NR_PACK;
 
    if(sys_outb(CMD_REG,cw)){
        return 1;
    } //(nao tenho a certeza se era p o cmd_reg ou outbuf_reg)
    return 0;
 
}
 
int set_cw_2(){
    uint8_t cw;
    util_sys_inb(CMD_REG,cw);
 
    cw|=STREAM_MODE;
    cw&=~NR_INT;
    cw&=~NR_PACK;
 
    if(sys_outb(CMD_REG,cw)){
        return 1;
    } //(nao tenho a certeza se era p o cmd_reg ou outbuf_reg)
    return 0;
 
}
 
int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hookId)) {
    printf("Unsubscribes a previous subscription failed!\n");
    return 1;
  }
  return 0;
}
 
 
 
int reset_cw(){
uint8_t cw;
    util_sys_inb(CMD_REG,cw);
 
    cw&=~STREAM_MODE;
    cw&=~NR_INT;
    cw&=~NR_PACK;
 
    if(sys_outb(CMD_REG,cw)){
        return 1;
    } //(nao tenho a certeza se era p o cmd_reg ou outbuf_reg)
    return 0;
 
}
 
p_packet get_packet(){
    return pack;
}