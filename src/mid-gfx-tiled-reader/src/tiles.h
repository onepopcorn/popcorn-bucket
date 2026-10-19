#ifndef TILES_H
#define TILES_H

#include <stdint.h>

#define TILES_SIZE 8
#define MAX_HORIZ_TILES 410
#define MAX_VERT_TILES 25
#define SCREEN_VIRTUAL 320

struct coords {
    int x;
    int y;
};

/**
Tiles image
*/

// dirty tiles array
extern char dirty_tiles[25][MAX_HORIZ_TILES];
extern int tiles_values[MAX_VERT_TILES][MAX_HORIZ_TILES];
extern int curr_tiles_width;
extern int map_width;
extern int map_pixel_width;
extern int almanac_tile_x;
/**
Preloads all tiles as raw pixels to blit them to screen
*/
void load_tiles();

/**
Loads the tiles into an screen buffer
WARNING! assumes that screen_buffer is reserved to the right size
*/
int load_background(char * contents, long size, uint8_t* screen_buffer);

/**
Returns x and y positions from a tile number
*/
struct coords get_tile_coords(int tile_number);

/** gets tile number at position x,y
helps for doing collisions (e.g. head banging, or player walking over tiles)
@param x position in pixels
@param y position in pixels
@return tile number
*/
int get_tile_at_position(int x, int y);

#endif