/*
"Enunciado"
 
por na control word os bits
5- ativar stream mode
3- scroll wheel on ou off (4 ou 3 bytes)
0- se os bytes vêm tds na mesma interrupçao ou nao
 
byte 0:
(aqui n tenho bem a certeza pq n me lembro se ainda havia as flags de overflow para o z)
  7 e 6 se houver overflow é o y_ov e o x_ov
  5 4 e 3 é msb de y x e z
  2 1 e 0 sao os mb rb e lb
byte 1: delta z
byte 2: delta x
byte 3: delta y
 
dar reset da control word no fim
 
ex 1-
    4 bytes
    tudo na mesma interrupçao
 
ex 2-
    3 bytes
    interrupçoes separadas
    (z por a 0) 
*/
 
 
#include "mouse.h"
//resto dos includes
 
int pegunta1(int cnt){
 
  int ipc_status;
  int r;
  message msg;
 
  uint8_t bit_no_m;
 
 
  if (mouse_subscribe_int(&bit_no_m)) {//subscribes the mouse interrupts
     return 1;
  }
  if (set_cw_1()){
    return 1;
  }
 
  uint32_t irq_set = BIT(bit_no_m);
 
  while (cnt>0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            mouse_ih_1();
            pack_builder_1();
            mouse_print_pack(get_pack());
            cnt--;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { //received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
  if (reset_cw()) {
    return 1;
  }
  if (mouse_unsubscribe_int()) {
    return 1;
  }
  return 0; 
}
 
int pegunta2(int cnt){
 
  int ipc_status;
  int r;
  message msg;
 
  uint8_t bit_no_m;
 
 
  if (mouse_subscribe_int(&bit_no_m)) {
     return 1;
  }
  if (set_cw_2()){
    return 1;
  }
 
  uint32_t irq_set = BIT(bit_no_m);
 
  while (cnt>0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            if(mouse_ih_2())
                break;
           
            pack_builder_2();
            mouse_print_pack(get_pack());
            cnt--;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { //received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
  if (reset_cw()) {
    return 1;
  }
  if (mouse_unsubscribe_int()) {
    return 1;
  }
  return 0;
}