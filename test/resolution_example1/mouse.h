#define MOUSE_IRQ 12;

#define STATUS_REGISTER 0x64
#define OUT_BUF 0X60

#define mouse_init_state 0x89
#define mouse_default_state 0x3E

#define LB BIT(0)
#define RB BIT(1)
#define MB BIT(2)
#define MSB_Z BIT(3)
#define MSB_X BIT(4)
#define MSB_Y BIT(5)
#define X_OV BIT(6)
#define Y_OV BIT(7)
#define CMD_REG 0x64
#define OUT_BUF_REG 0x60
 
#define STREAM_MODE BIT(5)
#define NR_PACK BIT(3)
#define NR_INT BIT(0)

//LCF
#  define IRQ_REENABLE  0x001	/* reenable IRQ line after interrupt */
#  define IRQ_EXCLUSIVE 0x002	/* exclusive IRQ: disables active handlers */
 
typedef struct {
    uint8_t bytes[4];
     bool rb;            
     bool mb;            
     bool lb;            
     int16_t delta_z;    
     int16_t delta_x;
     int16_t delta_y;    
     bool x_ov;          
     bool y_ov;          
}p_packet;

int util_sys_inb(int port, uint8_t *value);
int mouse_subscribe_int(uint8_t *bit_no);
int set_cw_1();
int mouse_ih_1();
void build_packet_1();
int set_cw_2();
int mouse_ih_2();
void build_packet_2();
p_packet get_packet();
int reset_cw();
int mouse_unsubscribe_int();

