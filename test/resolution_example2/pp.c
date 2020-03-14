// IMPORTANT: you must include the following line in all your C files
#include "lcf.h"
#include "pp.h"
#include "mouse.h"

#include <stdint.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[])
{
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

int(pp_test_packet)(uint32_t cnt)
{
    int ipc_status, rec;
    message msg;
    int mouse_bit = 12;
    mouse_subscribe_int(&mouse_bit);
    mouse_init();

    while (cnt > 0)
    {
        if ((rec = driver_receive(ANY, &msg, &ipcStatus)) != 0)
        {
            printf("driver_receive failed with: %d", rec);
            continue;
        }
        if (is_ipc_notify(ipcStatus))
        {
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE:
                if (msg.m_notify.interrupts & mouse_bit)
                {
                    mouse_ih();
                    cnt--;
                }
                break;
            default:
                break;
            }
        }
    }
    mouse_unsubscribe_int();
    mouse_reset();
    

    return 0;
}