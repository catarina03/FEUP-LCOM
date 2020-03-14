// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "grafics.h"
#include <stdint.h>
#include <stdio.h>
#include "keyboard.h"

#include "i8042.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  /* To be completed */
 
  set_vbe_mode(mode);
  get_mode_info(mode);
  sleep(delay);
  
  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,uint16_t width, uint16_t height, uint32_t color) {
  get_mode_info(mode);
  set_vbe_mode(mode);
  uint8_t  bit_no;
  
  //enable keyboard interrupt notifications
  if(kbd_subscribe_int(&bit_no))return 1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status=0;
  message msg;
  int r=0;
  

  vg_draw_rectangle(x,y,width,height,color);

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
          
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}

    
   

	}
  if(kbd_unsubscribe_int())return 1;
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  get_mode_info(mode);
  vg_init(mode);
  set_vbe_mode(mode);
 //enable keyboard interrupt notifications
  uint8_t  bit_no;
  if(kbd_subscribe_int(&bit_no))return 1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status=0;
  message msg;
  int r=0;

  uint16_t recWidth = x_res / no_rectangles;
  uint16_t recHeight = y_res / no_rectangles;

  uint32_t color = first;

  for (unsigned row = 0; row < no_rectangles; row++) {

    for (unsigned col = 0; col < no_rectangles; col++) {

      //gets the color for the next rectangle


      if(mode_info.MemoryModel==0x04) //inexed mode
      color = (first + (row * no_rectangles + col) * step) % (1 << bppixel);
      
      else if(mode_info.MemoryModel==0x06){//direct mode
        
      uint32_t red_mask = ((1 << mode_info.RedMaskSize)-1);
      uint32_t red=(((first>>mode_info.RedFieldPosition) & red_mask)+ col * step) % (1 << mode_info.RedMaskSize);
      uint32_t green_mask = ((1 << mode_info.GreenMaskSize)-1);
	    uint32_t green = (((first>>mode_info.GreenFieldPosition ) & green_mask)+ row * step) % (1 << mode_info.GreenMaskSize);
      uint32_t blue_mask = ((1 << mode_info.BlueMaskSize)-1);
	    uint32_t blue = (((first<<mode_info.BlueFieldPosition) & blue_mask)+ (col + row) * step) % (1 << mode_info.BlueMaskSize);	

      color=((red << mode_info.RedFieldPosition )| (green << mode_info.GreenFieldPosition)|blue);
      }
      if (vg_draw_rectangle(recWidth * col, recHeight * row, recWidth, recHeight, color)) return 1;
    }
  }

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
          
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}

   
	}
  if(kbd_unsubscribe_int())return 1;
  vg_exit();
  return 0;

}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  get_mode_info(0x105);
  vg_init(0x105);
  set_vbe_mode(0x105);
  
 
 //enable keyboard interrupt notifications
  uint8_t  bit_no;
  if(kbd_subscribe_int(&bit_no))return 1;
  uint32_t irq_set = BIT(bit_no);
  int ipc_status=0;
  message msg;
  int r=0;


  draw_xpm(x,y,xpm);
  


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
          
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}

  

	}
  if(kbd_unsubscribe_int())return 1;
  vg_exit();
  return 0;

}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
int16_t speed, uint8_t fr_rate) {

  get_mode_info(0x105);
  vg_init(0x105);
  set_vbe_mode(0x105);
  
 
 //enable keyboard and timer interrupt notifications
  uint8_t  kbd_bit_no, timer_bit_no;
  if(kbd_subscribe_int(&kbd_bit_no))return 1;
  if(timer_subscribe_int(&timer_bit_no))return 1;
  uint32_t irq_set_kbd = BIT(kbd_bit_no);
  uint32_t irq_set_timer=BIT(timer_bit_no);
  int ipc_status=0;
  message msg;
  int r=0;

  //first picture
  if (draw_xpm(xi, yi, xpm))return 1;


  uint8_t time_per_frame =sys_hz()/fr_rate;
  int fr_passed=0;

  int32_t distance_to_end;
  if(yi==yf)distance_to_end =abs(xf-xi);
  else distance_to_end =abs(yf-yi);

  while(scancode!=ESC_BRK_CODE){ 
		/* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & irq_set_kbd) { /* subscribed interrupt */
					
          kbc_ih();
          
          
				}

        if(msg.m_notify.interrupts & irq_set_timer){
          
          timer_int_handler();
          if(speed > 0){
          if ((counter % time_per_frame == 0) ) {
                 //the end position was reached
                if((xi == xf) && (yi == yf))continue;

              
                //vertical movement
                if(xi == xf){

                  if(yf > yi)
                    yi += speed;
                  else yi -= speed;

                  distance_to_end -= speed;

                  if(distance_to_end < 0)
                    yi = yf;
                  
                  
                }
                //horizontal movement
                else{
                    if(xf > xi)
                      xi += speed;
                    else xi -= speed;
                    distance_to_end-= speed;
                    if(distance_to_end < 0)
                      xi = xf;
                      
                }
                //paints screen black to erase previous pixmap
                vg_draw_rectangle(0,0,x_res,y_res,0);
                //draws new pixmap
                if (draw_xpm(xi, yi, xpm))return 1;
               
            }
            }
            else{

              if (counter % time_per_frame == 0)
                fr_passed++;

              if ((abs(speed)==fr_passed ) && (distance_to_end > 0)){
                fr_passed = 0; 
                if((yi == yf)&&(xi == xf) )
                  continue;

                //vertical movement
                if(yi != yf){
                  if(yf > yi)
                    yi += 1;
                  else yi -= 1;
                }
                //horizontal movement
                else{

                  if(xf > xi)
                    xi += 1;
                  else xi -= 1;    

                }
                distance_to_end -= 1;
                //paints screen black to erase previous pixmap
                vg_draw_rectangle(0,0,x_res,y_res,0);
                //draws new pixmap
                if (draw_xpm(xi, yi, xpm))return 1;
              }
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

  if(kbd_unsubscribe_int())return 1;
  if(timer_unsubscribe_int())return 1;
  if(vg_exit())return 1;
  return 0;

}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
