#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include <i8042.h>
#include "keyboard.h"
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
 
 uint8_t scancode;
 bool status_error;

int(kbd_test_scan)() {
  count=0;
  uint8_t  bit_no;
  
  //enable keyboard interrupt notifications
  if(kbd_subscribe_int(&bit_no))return 1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status=0;
  message msg;
  int r=0;

  //initialization of variables to be used as kbd_print_scancode arguments
  bool make=false;
  uint8_t size=0;
  uint8_t scancode_bytes[2];



  while(scancode!=ESC_BRK_CODE){ 
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
					
          kbc_ih();
          if(status_error)continue;
          scancode_bytes[size]=scancode;
          size++;
          
          //check if the scancode has only one byte
          if(scancode!=SECOND_BYTE_CODE){
            make=!(scancode & BREAK_CODE);
            kbd_print_scancode(make,size,scancode_bytes);
            size=0;
            scancode_bytes[0]=0;
            scancode_bytes[1]=0; 
          }
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
  #ifdef LAB3
  if(kbd_print_no_sysinb(count))return 1;
  #endif

  //unsubscribe interruptions
  if(kbd_unsubscribe_int())return 1;


  return 0;
}

int(kbd_test_poll)() {
  //initialization of the global variables
  count=0;
  scancode=0;

  //initialization of local variables to be used as kbd_print_scancode arguments
  bool make=false;
  uint8_t size=0;
  uint8_t scancode_bytes[2];
  
  //loop that checks if there is information to read from the keyboard 
  while(scancode!=ESC_BRK_CODE){
   
    tickdelay(micros_to_ticks(DELAY));
    kbc_ih(); 
    if (status_error)continue;
    if (scancode==ERROR)continue; //ignores already processed scancodes
    scancode_bytes[size]=scancode;
    size++;

    if(scancode!=SECOND_BYTE_CODE){ //checks if the scancode has 2 bytes
      make=!(scancode & BREAK_CODE); //checks if the scancode is a breakcode
      if(kbd_print_scancode(make,size,scancode_bytes))return 1;
      size=0;
      scancode_bytes[0]=0;
      scancode_bytes[1]=0;
    }
    
  }
  //reenables the keyboard interrupts
  if(kbd_enable())return 1 ;
  #ifdef LAB3
  if(kbd_print_no_sysinb(count))return 1;
  #endif
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) { 
  count=0;
  int time_passed=0;
  uint8_t  bit_kb, bit_timer;
  
  //subcribes keyboard interrupts
  if(kbd_subscribe_int(&bit_kb))return 1;
  uint32_t irq_set_kb= BIT(bit_kb);

  //subcribes timer interrupts
  if(timer_subscribe_int(&bit_timer))return 1;
  uint32_t irq_set_timer =BIT(bit_timer);

  int ipc_status=0;
  message msg;
  int r=0;

  bool make=false;
  uint8_t size=0;
  uint8_t scancode_bytes[2];

  while(scancode!=ESC_BRK_CODE && time_passed<n){ //subcribes keyboard interrupts
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & irq_set_kb ) { /* subscribed interrupt */
					
          kbc_ih();
          if(status_error)continue;
          scancode_bytes[size]=scancode;
          size++;

          //restore the time after input
          time_passed=0;
          counter=0; 

          if(scancode!=SECOND_BYTE_CODE){//checks if the scancode has 2 bytes
            make=!(scancode & BREAK_CODE);//checks if the scancode is a breakcode
            if(kbd_print_scancode(make,size,scancode_bytes))return 1;
            size=0;
            scancode_bytes[0]=0;
            scancode_bytes[1]=0;
            
          }
          
          
				}
				
        if (msg.m_notify.interrupts & irq_set_timer ) { /* subscribed interrupt */
					
          timer_int_handler();
					if((counter%sys_hz()) == 0) time_passed++; 
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

  if(kbd_unsubscribe_int())return 1;  //unsubscribe keyboard interrupts
  if(timer_unsubscribe_int())return 1;  //unsubscribe timer interrupts
  return 0;
}


