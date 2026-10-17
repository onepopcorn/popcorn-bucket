#include "tiles.h"
#include <stdint.h>
#include <string.h>
#include "video.h"
#include "files.h"
#include <stdio.h>
#include <stdlib.h>

// raw 8-bit tileset pixels, TILESET_STRIDE wide
uint8_t* tiles;
#define TILESET_STRIDE 256 // 32 tiles * 8px, see get_tile_coords()
// width="80" height="25"

char dirty_tiles[MAX_VERT_TILES][MAX_HORIZ_TILES] = { 0 };
int tiles_values[MAX_VERT_TILES][MAX_HORIZ_TILES] = { 0 };
// defines the current width in tiles of the loaded background (e.g., 80 for 640px)
int curr_tiles_width = 0;
int map_width = 0;
int map_pixel_width = 0;
int almanac_tile_x = -1;

// loads tiles.bmp (8bpp, 256px wide, bottom-up) into `tiles` and sets the VGA palette
void load_tiles() {
    long size;
    uint8_t* bmp = (uint8_t*)read_file("tiles.bmp", &size);
    if (bmp == NULL) {
        exit(1);
    }
    int height = *(int32_t*)(bmp + 22);
    uint32_t data_offset = *(uint32_t*)(bmp + 10);
    tiles = malloc(TILESET_STRIDE * height);
    if (tiles == NULL) {
        exit(1);
    }
    for (int row = 0; row < height; row++) {
        memcpy(tiles + row * TILESET_STRIDE,
            bmp + data_offset + (height - 1 - row) * TILESET_STRIDE, TILESET_STRIDE);
    }
    setPaletteBGRA(bmp + 54, 256);
    free(bmp);
}

struct coords get_tile_coords(int tile_number) {
    struct coords result;
    result.x = ((tile_number - 1) & 31) << 3;
    result.y = ((tile_number - 1) >> 5) << 3;

    return result;
}

int get_tile_at_position(int x, int y) {
    int tile_x = x >> 3;
    int tile_y = y >> 3;

    // Validar límites
    if (tile_x < 0 || tile_x >= curr_tiles_width || tile_y < 0 || tile_y >= MAX_VERT_TILES) {
        return -1; // Fuera de límites
    }

    return tiles_values[tile_y][tile_x];
}

int load_background(char * contents, long size, uint8_t* background) {
    char* in_file;
    if (contents == NULL || size <= 0 || !background) {
        return 1;
    }

    // assumes contents is not null terminated (e.g. a datafile entry)
    in_file = malloc(size + 1);
    if (in_file == NULL) {
        return 1;
    }

    memcpy(in_file, contents, size);
    in_file[size] = '\0';

    unsigned char current;

    // skip xml data
    int start_csv = 0;
    int i = 0;

    curr_tiles_width = 0;

    // Find the first '>' and extract width before skipping header
    do {
        current = (unsigned char)in_file[i++];
        if (current == '>') {
            // Read whole header line
            int header_start = i - 1;
            while (header_start > 0 && in_file[header_start] != '<')
                header_start--;
            char header_line[256] = { 0 };
            int hlen = 0;
            while (in_file[header_start + hlen] != '>' && hlen < 255) {
                header_line[hlen] = in_file[header_start + hlen];
                hlen++;
            }
            header_line[hlen] = '\0';
            // search for width="
            const char* wptr = strstr(header_line, "width=\"");
            if (wptr) {
                // skip width=": we just want the number
                wptr += 7;
                char numbuf[16] = { 0 };
                int ni = 0;
                while (wptr[ni] && wptr[ni] != '"' && ni < 15) {
                    numbuf[ni] = wptr[ni];
                    ni++;
                }
                numbuf[ni] = '\0';
                curr_tiles_width = atoi(numbuf);
            }
            start_csv += 1;
        }
    } while (start_csv < 5 && current != '\0');
    if (curr_tiles_width <= 0) {
        exit(1);
    }

    map_width = (curr_tiles_width << 3) - 320;
    map_pixel_width = curr_tiles_width << 3;
    if (map_pixel_width <= 0) {
        exit(1);
    }

    // temporal data for csv
    char current_tile[16] = { 0 };
    int charpos = 0;
    char* output;
    int iterations = 0;
    // current screen position
    struct coords screen_coords;
    screen_coords.x = screen_coords.y = 0;
    int tiles_x = 0, tiles_y = 0;

    do {
        current = in_file[i++];

        if (current == ',' || current == '<') {
            if (current == '<') {
                // signal finish reading
                start_csv = -1;
            }
            current_tile[charpos] = '\0';
            charpos = 0;
            int tile_number = strtol(current_tile, &output, 10);
            struct coords coordinates = get_tile_coords(tile_number);

            // copies from tiles to background
            blit(tiles, TILESET_STRIDE, coordinates.x, coordinates.y,
                background, map_pixel_width, screen_coords.x, screen_coords.y,
                TILES_SIZE, TILES_SIZE);
            // store tile value
            tiles_values[tiles_y][tiles_x] = tile_number;
            if (tile_number - 1 == 1040) { // ALMANAC_TILE_ID
                almanac_tile_x = screen_coords.x;
            }

            screen_coords.x += 8;
            tiles_x++;
            if (screen_coords.x >= map_pixel_width - 1) {
                iterations = 0;
                screen_coords.y += 8;
                screen_coords.x = 0;
                // store the current tiles values
                tiles_x = 0;
                tiles_y++;
            }

        } else if (current != '\0') { // a number
            current_tile[charpos++] = current;
        }

        iterations += 1;
        // if (iterations == 200) start_csv = -1; // Uncomment if needed
    } while (current != '\0' && start_csv != -1);

    free(in_file);
    return 0;
}

void mark_dirty_tiles(int x, int y, int width, int height) {
    int sx, sy, ex, ey;

    sx = x / TILES_SIZE;
    sy = y / TILES_SIZE;
    ex = (x + width - 1) / TILES_SIZE;
    ey = (y + height - 1) / TILES_SIZE;

    if (sx < 0)
        sx = 0;
    if (sy < 0)
        sy = 0;
    if (ex >= MAX_HORIZ_TILES)
        ex = MAX_HORIZ_TILES - 1;
    if (ey >= MAX_VERT_TILES)
        ey = MAX_VERT_TILES - 1;

    for (int ty = sy; ty <= ey; ++ty) {
        for (int tx = sx; tx <= ex; ++tx) {
            dirty_tiles[ty][tx] = 1;
        }
    }
}

// Clear all dirty tiles
void clear_dirty_tiles() {
    memset(dirty_tiles, 0, sizeof(dirty_tiles));
}
