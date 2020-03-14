// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>

#include <stdio.h>
#include "i8254.h"
#include <i8042.h>
#include "mouse.h"
#include "keyboard.h"
#include "util.h"
// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
 bool status_error_mouse=false;
 uint8_t ack_byte=0;
 

int (mouse_test_packet)(uint32_t cnt) {

  bool status_error_mouse=false;
  uint32_t npackets=0;
  
  
  int next_byte=0;
  struct packet mouse_packet;
  
  uint8_t bit_no;

  if(mouse_subscribe_int(&bit_no))return 1;
  uint32_t irq_set = BIT(bit_no);
  
  
  if(sys_irqdisable(&hook_mouse))return 1;

  do{
    if( issue_cmd_mouse(ENB_DATA_REPORT, &ack_byte))return 1;// enables data reporting      
    if(ack_byte == ERROR) return 1; //acknowledgment byte was ERROR;
         
      
  }while(ack_byte != ACK);

  if(sys_irqenable(&hook_mouse))return 1;


  int ipc_status=0;
  message msg;
  int r=0;
  
  while(npackets<cnt){ 
		 /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		} 
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
          mouse_ih();
          
          if(status_error_mouse)continue;
          if( next_byte==0){
            mouse_packet.bytes[0]=byte_mouse;
            next_byte=1;
          }
          else if (next_byte==1){
            mouse_packet.bytes[next_byte]=byte_mouse;
            next_byte=2;
          }
          else if(next_byte==2){
            bool sign_y=mouse_packet.bytes[0]& Y_SIGN;
            bool sign_x=mouse_packet.bytes[0]& X_SIGN;
            mouse_packet.bytes[next_byte]=byte_mouse;
            next_byte=0;
            npackets+=1;
            mouse_packet.rb=mouse_packet.bytes[0]& PRESSED_RB;
            mouse_packet.mb=mouse_packet.bytes[0]& PRESSED_MB;
            mouse_packet.lb=mouse_packet.bytes[0]& PRESSED_LB;
            mouse_packet.delta_x=convert(mouse_packet.bytes[1],sign_x);
            mouse_packet.delta_y=convert(mouse_packet.bytes[2],sign_y);
            mouse_packet.x_ov=mouse_packet.bytes[0]& X_OVF;
            mouse_packet.y_ov=mouse_packet.bytes[0]& Y_OVF;
            mouse_print_packet(&mouse_packet);
          }
          
        }
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a stprintf("%d   , %d", x , y);andard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
  
 if(sys_irqdisable(&hook_mouse))return 1;

  do{
      if( issue_cmd_mouse(DIS_DATA_REPORT, &ack_byte) != 0) //disables data reporting
          return 1;

        if(ack_byte == ERROR) //acknowledgment byte was ERROR; terminates program
          return 1;
      
  }while(ack_byte != ACK);

  if(sys_irqenable(&hook_mouse))return 1;
  
  if(mouse_unsubscribe_int())return 1;
  


  return 0;
  
}







int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  struct packet mouse_packet;
  uint8_t npackets=0;
  byte_mouse=0;
  uint8_t ack_byte;
  while(npackets<cnt){
    do{
      if(issue_cmd_mouse(READ_DATA,& ack_byte))return 1; 

      if(ack_byte == ERROR) return 1;//acknowledgment byte was ERROR;
      
     }while(ack_byte != ACK);

    for(int byte=0; byte<3; byte++){
      mouse_ih();
      
      if(status_error_mouse)return 1;
      mouse_packet.bytes[byte]=byte_mouse;
      if(byte==2){
            bool sign_y=mouse_packet.bytes[0]& Y_SIGN;
            bool sign_x=mouse_packet.bytes[0]& X_SIGN;
            mouse_packet.bytes[byte]=byte_mouse;
            
            mouse_packet.rb=mouse_packet.bytes[0]& PRESSED_RB;
            mouse_packet.mb=mouse_packet.bytes[0]& PRESSED_MB;
            mouse_packet.lb=mouse_packet.bytes[0]& PRESSED_LB;
            mouse_packet.delta_x=convert(mouse_packet.bytes[1],sign_x);
            mouse_packet.delta_y=convert(mouse_packet.bytes[2],sign_y);
            mouse_packet.x_ov=mouse_packet.bytes[0]& X_OVF;
            mouse_packet.y_ov=mouse_packet.bytes[0]& Y_OVF;
            mouse_print_packet(&mouse_packet);
          
      }  
    
     }
  

  npackets++;
  tickdelay(micros_to_ticks(period*1000));


  }

  if(issue_cmd_mouse(SET_STRM_MODE, &ack_byte))return 1;

  if(issue_cmd_mouse(DIS_DATA_REPORT, &ack_byte))return 1; //disables data reporting


  //resets the command byte
  if(kbc_issue_command(WRITE_CMD_BYTE))return 1;
  if(kbc_issue_arg(minix_get_dflt_kbc_cmd_byte()))return 1;
  
  return 0;

          
}









int (mouse_test_async)(uint8_t idle_time) {
  count=0;
  int time_passed=0;
  uint8_t  bit_mouse, bit_timer;
  
  //subcribes mouse interrupts
  if(mouse_subscribe_int(&bit_mouse))return 1;
  uint32_t irq_set_mouse= BIT(bit_mouse);


  //subcribes timer interrupts
  if(timer_subscribe_int(&bit_timer))return 1;
  uint32_t irq_set_timer =BIT(bit_timer);

  
  sys_irqdisable(&hook_mouse);
   do{
    if( issue_cmd_mouse(ENB_DATA_REPORT, &ack_byte))return 1;// enables data reporting      
    if(ack_byte == ERROR) return 1; //acknowledgment byte was ERROR;
         
      
  }while(ack_byte != ACK);

  if(sys_irqenable(&hook_mouse))return 1;


  int ipc_status=0;
  message msg;
  int r=0;

  struct packet mouse_packet;
  int next_byte=0;

  while(time_passed<idle_time){ //subcribes keyboard interrupts
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & irq_set_mouse ) { /* subscribed interrupt */
					
          mouse_ih();
          
          if(status_error_mouse)continue;
          if(/*(scancode & MOUSE_FIRST_BYTE)&&*/ next_byte==0){
            mouse_packet.bytes[0]=byte_mouse;
            next_byte=1;
          }
          else if (next_byte==1){
            mouse_packet.bytes[next_byte]=byte_mouse;
            next_byte=2;
          }
          else if(next_byte==2){
            bool sign_y=mouse_packet.bytes[0]& Y_SIGN;
            bool sign_x=mouse_packet.bytes[0]& X_SIGN;
            mouse_packet.bytes[next_byte]=byte_mouse;
            next_byte=0;
            mouse_packet.rb=mouse_packet.bytes[0]& PRESSED_RB;
            mouse_packet.mb=mouse_packet.bytes[0]& PRESSED_MB;
            mouse_packet.lb=mouse_packet.bytes[0]& PRESSED_LB;
            mouse_packet.delta_x=convert(mouse_packet.bytes[1],sign_x);
            mouse_packet.delta_y=convert(mouse_packet.bytes[2],sign_y);
            mouse_packet.x_ov=mouse_packet.bytes[0]& X_OVF;
            mouse_packet.y_ov=mouse_packet.bytes[0]& Y_OVF;
            mouse_print_packet(&mouse_packet);
            time_passed=0;
            counter=0; 
          }
          //restore the time after input
          
       
          
          
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


  if(timer_unsubscribe_int())return 1;  //unsubscribe timer interrupts

  if(sys_irqdisable(&hook_mouse))return 1;

  do{
      if( issue_cmd_mouse(DIS_DATA_REPORT, &ack_byte) != 0) //disables data reporting
          return 1;

        if(ack_byte == ERROR) //acknowledgment byte was ERROR; terminates program
          return 1;
      
  }while(ack_byte != ACK);
  if(sys_irqenable(&hook_mouse))return 1;
 

  if(mouse_unsubscribe_int())return 1;
  return 0;
}










struct mouse_state machine_state; //global variable to be used in mouse_detect_state

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  bool status_error_mouse=false;
  uint32_t npackets=0;
  machine_state.st=MIDDLE;
  
  int next_byte=0;
  struct packet mouse_packet;
  
  uint8_t bit_no;

  if(mouse_subscribe_int(&bit_no))return 1;
  uint32_t irq_set = BIT(bit_no);
  
  
  if(sys_irqdisable(&hook_mouse))return 1;

  do{
    if( issue_cmd_mouse(ENB_DATA_REPORT, &ack_byte))return 1;// enables data reporting      
    if(ack_byte == ERROR) return 1; //acknowledgment byte was ERROR;
         
      
  }while(ack_byte != ACK);

  if(sys_irqenable(&hook_mouse))return 1;

  
  
  int ipc_status=0;
  message msg;
  int r=0;
  
  while(machine_state.st!=END){ 
		 /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		} 
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
          mouse_ih();
          
          if(status_error_mouse)continue;
          if(/*(scancode & MOUSE_FIRST_BYTE)&&*/ next_byte==0){
            mouse_packet.bytes[0]=byte_mouse;
            next_byte=1;
        
          }
          else if (next_byte==1){
            mouse_packet.bytes[next_byte]=byte_mouse;
            next_byte=2;
          }
          else if(next_byte==2){
            bool sign_y=mouse_packet.bytes[0]& Y_SIGN;
            bool sign_x=mouse_packet.bytes[0]& X_SIGN;
            mouse_packet.bytes[next_byte]=byte_mouse;
            next_byte=0;
            npackets+=1;
            mouse_packet.rb=mouse_packet.bytes[0]& PRESSED_RB;
            mouse_packet.mb=mouse_packet.bytes[0]& PRESSED_MB;
            mouse_packet.lb=mouse_packet.bytes[0]& PRESSED_LB;
            mouse_packet.delta_x=convert(mouse_packet.bytes[1],sign_x);
            mouse_packet.delta_y=convert(mouse_packet.bytes[2],sign_y);
            mouse_packet.x_ov=mouse_packet.bytes[0]& X_OVF;
            mouse_packet.y_ov=mouse_packet.bytes[0]& Y_OVF;
            mouse_print_packet(&mouse_packet); 
            struct mouse_ev* ev;
            ev =mouse_detect_ev(&mouse_packet);
            //printf("evento: %d\n",ev->type) ;
            mouse_detect_state(ev,x_len,tolerance);
            //printf("state:%d\n x: %d \n y: %d \n" ,machine_state.st, machine_state.delta_x,machine_state.delta_y) ;
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
  
 if(sys_irqdisable(&hook_mouse))return 1;

  do{
      if( issue_cmd_mouse(DIS_DATA_REPORT, &ack_byte) != 0) //disables data reporting
          return 1;

        if(ack_byte == ERROR) //acknowledgment byte was ERROR; terminates program
          return 1;
      
  }while(ack_byte != ACK);

  if(sys_irqenable(&hook_mouse))return 1;
  
  if(mouse_unsubscribe_int())return 1;
  


  return 0;
}
