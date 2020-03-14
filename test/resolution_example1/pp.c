// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/pp.h>

#include <stdint.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/pp/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/pp/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}




int(pp_test_packet)(uint32_t cnt) {
  /* To be completed */
  printf("%s(%u): under construction\n", __func__, cnt);

  int ipc_status, r, irq_set = 0;
  message msg;

  bool done = false;

  // XXX : you may need to add some code here

  uint8_t bit_no;

  if(mouse_subscribe_int(&bit_no))
    return 1;
  if(set_cw_1())
    return 1;

  irq_set = BIT(bit_no);


  // interrupt loop
  while (cnt > 0) {
    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
                                                   // handle interrupt
            mouse_ih_1();
            build_packet_1();
            mouse_print_packet(get_packet());
            cnt--;
          }
          break;
        default:
          break;
      }
    }
  }

  if (reset_cw())
    return 1;
  if (mouse_unsubscribe_int())
    return 1;

  return 0;
}

int(pp_test_optimized)(uint32_t cnt) {
  /* To be completed */
  printf("%s(%u): under construction\n", __func__, cnt);

  int ipc_status, r, irq_set = 0;
  message msg;

  //bool done = false;

  // XXX : you may need to add some code here

  uint8_t bit_no;

  if(mouse_subscribe_int(&bit_no))
    return 1;
  if(set_cw_2())
    return 1;

  irq_set = BIT(bit_no);


  // interrupt loop
  while (cnt > 0) {
    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
                                                   // handle interrupt
            if(mouse_ih_2())
              break; //Sai do loop e vai para a proxima interrupçao
                     //Se for 0 quer dizer que leu todos os packs e pode dar build ao pack
            build_packet_2();
            mouse_print_packet(get_packet());
            cnt--;
          }
          break;
        default:
          break;
      }
    }
  }

  if (reset_cw())
    return 1;
  if (mouse_unsubscribe_int())
    return 1;

  return 0;
}
