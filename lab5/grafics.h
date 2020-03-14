#include <lcom/lcf.h>

int vbe_get_mode_inf(uint16_t mode, vbe_mode_info_t *vmi_p);

int set_vbe_mode(uint16_t mode);

void*(vg_init)( uint16_t mode) ;	

int draw_pixel(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_rectangle)(uint16_t	x, uint16_t  y, uint16_t width, uint16_t height, uint32_t color);

int (vg_draw_hline)(uint16_t	x, uint16_t  y, uint16_t len, uint32_t color);

int get_mode_info(uint16_t mode);

int draw_xpm(int x ,int y, xpm_map_t img);

int clear_screen();

extern unsigned int x_res;
extern unsigned y_res;

extern unsigned color_mode;
extern unsigned int bppixel;
extern unsigned int bytes_per_pixel;
static unsigned int vram_base;  
static unsigned int vram_size; 
vbe_mode_info_t mode_info;
struct reg86 r;  
static void* v_mem;
