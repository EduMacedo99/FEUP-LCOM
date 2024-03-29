#pragma once
#include <stdbool.h>
#include <stdint.h>

//returns the horizontal resolution in pixels
unsigned get_h_res();

//returns the vertical resolution in pixels
unsigned get_v_res();

//returns the number of VRAM bits per pixel
unsigned get_bitspp();

//returns the number of VRAM bytes per pixel
unsigned get_bytespp();

//returns the struct with the information about minix's memory range
struct minix_mem_range get_minix_memory_range();

//returns VRAM's physical addresss
unsigned int get_vram_base();

//returns VRAM's size
unsigned int get_vram_size();

//return the video memory pointer
void* get_video_mem();

//returns the color mode
unsigned get_color_mode();


//sets the desired graphical mode
int set_graph_mode(uint16_t mode);

//function that maps the vram physical memory into the process's virtual memory, given the information in the static global variables
int map_memory();

//gets the vram physical address, and updates the static variables accordingly
int get_vram_info(uint16_t mode);

//function that returns the least significant bits of a 32-bit value
uint32_t get_less_bits(uint32_t value, uint32_t num_bits);

//function that just colors in a pixel, situated at the given coordinates
int color_pixel(uint16_t x, uint16_t y, uint32_t color);

//function that gets the right color for a rectangle, given its coordinates, the first color, the step and the no of rectangles (used in video_test_pattern)
uint32_t change_color(uint16_t row, uint16_t col, uint8_t no_rectangles, uint32_t first, uint8_t step);

//returns a certain color component, given its field position and mask size
uint8_t return_color_comp(unsigned field_position, unsigned mask_size, uint32_t color);

//draws a xpm on the screen, on the given coordinates
int draw_pixmap(const char* sprite, uint16_t x, uint16_t y, int width, int height);

//erases an xpm that is on the screen, given its coordinates
int erase_pixmap(uint16_t x, uint16_t y, int width, int height);

//function that gets the information about the current graphical mode
int vbe_get_info_about_mode(uint16_t mode, vbe_mode_info_t *vmi_p);

//Clears the screen
int clear_screen();


#pragma pack(1)

typedef struct {
    char VBESignature[4];   
    BCD VBEVersion[2];          
    uint32_t OEMStringPtr;               
    uint8_t Capabilities[4];                
    uint32_t VideoModeList;      
    uint16_t TotalMemory;            

    uint16_t OemSoftwareRev;                 
    uint32_t OemVendorNamePtr;
    uint32_t OemProductNamePtr;
    uint32_t OemProductRevPtr;  
    uint8_t Reserved[222];            
    uint8_t OemData[256];

}  vbe_info_block_t;

#pragma options align=reset

//function that calls VBE function 0x4F00, to get controller information
int vbe_get_ctrl_info(vg_vbe_contr_info_t *ctrl_info, vbe_info_block_t* info_block);
