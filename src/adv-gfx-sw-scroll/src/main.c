#include <stdint.h>
#include <stdlib.h>
#include <sys/nearptr.h>
#include <string.h>
#include <conio.h>

#include "video.h"
#include "assets/tileset.h"
#include "assets/map.h"

#define SCREEN_W 320
#define SCREEN_H 200

#define TILE_SIZE 16

// #define FAST_MODULO(Q, D) (Q & (D - 1))  // (Q)uotient. (D)ividend needs to be power of 2
// #define FAST_DIVIDE(Q, D) Q >> __builtin_ctz(D)

uint8_t* vram;
uint8_t* back_buffer;

typedef struct {
    int x, y;                // Logical coordinates
    int scroll_x, scroll_y;  // Acumulated scroll
    int width;               // Camera
    int height;
    int vel_x, vel_y;  // camera velocity

} Camera;

// Create a viewport with 2 extra columns for horizontal scroll
Camera camera = {0, 0, 0, 0, SCREEN_W + TILE_SIZE, SCREEN_H, 1, 0};

/**
 * Copy given tile number from map to back_buffer
 * at given local coordinates (i.e. back_buffer coordinates)
 *
 */
void drawTile(int x, int y, int tile) {
    const int offset = tile * TILE_SIZE * TILE_SIZE;  // tile_idx = tile_num * tile_width * tile_height
    const uint8_t* src = &tileset[offset];
    uint8_t* dst = &back_buffer[(y * camera.width) + x];

    // Copy row chunks
    for (int i = 0; i < TILE_SIZE; i++) {
        const uint8_t* src_row = src + i * TILE_SIZE;
        uint8_t* dst_row = dst + i * camera.width;

        memcpy(dst_row, src_row, TILE_SIZE);
    }
}

/**
 * Copy given map column data from map to
 * back_buffer at given x local coordinate
 * (i.e. back_buffer coordinate)
 *
 */
void drawMapCol(int col, int x) {
    // Once more, we can only copy line by line
    const int total_rows = camera.height / TILE_SIZE;
    for (int i = 0; i < total_rows; i++) {
        int tile = map[i][col];
        drawTile(x, i * TILE_SIZE, tile);
    }
}

/**
 * Copy only the part from the map that fills
 * inside the camera back_buffer including
 * offscreen column
 *
 */
void drawMap() {
    const int total_rows = camera.height / TILE_SIZE;
    const int total_cols = camera.width / TILE_SIZE;

    for (int row = 0; row < total_rows; row++) {
        for (int col = 0; col < total_cols; col++) {
            int tile = map[row][col];
            drawTile(col * TILE_SIZE, row * TILE_SIZE, tile);
        }
    }
}

/**
 * Update given camera based on camera velocity.
 *
 * Also, draw next portion of map on offscreen
 * back_buffer row.
 *
 */
void updateCamera(Camera* camera) {
    int cam_x = camera->x;
    int cam_vel_x = camera->vel_x;
    int cam_width = camera->width;
    int cam_scroll_x = camera->scroll_x;

    // Check camera movement boundaries
    // Rightmost boundary || Leftmost boundary
    if ((cam_x + cam_vel_x + cam_width) / TILE_SIZE >= MAP_W || cam_x + cam_vel_x <= 0) {
        camera->vel_x *= -1;
        return;
    }

    // We can do a "fast modulo" operation becaue TILE_SIZE is power of 2
    if ((cam_x & (TILE_SIZE - 1)) == 0) {
        if (cam_vel_x > 0) {  // scrolling right
            // Get map column based on camera logical coordinates
            int map_col = (cam_x / TILE_SIZE) + (SCREEN_W / TILE_SIZE);
            // Get x coordinate of column outisde the visible area
            int x = (cam_scroll_x + SCREEN_W) % cam_width;
            drawMapCol(map_col, x);
        } else if (cam_vel_x < 0) {  // scrolling left
            // Get map column based on camera logical coordinates
            int map_col = (cam_x / TILE_SIZE) - 1;
            // Get x coordinate of column outisde the visible area
            int x = (cam_scroll_x - TILE_SIZE + cam_width) % cam_width;
            drawMapCol(map_col, x);
        }
    }

    cam_scroll_x = (cam_scroll_x + cam_vel_x) % cam_width;
    if (cam_scroll_x < 0) cam_scroll_x += cam_width;

    // update camera offsets
    camera->x += cam_vel_x;
    camera->scroll_x = cam_scroll_x;
}

/**
 * Render given camera data to VRAM
 *
 */
void renderCamera(const Camera* camera) {
    const int x_start = camera->scroll_x;
    const int cam_w = camera->width;
    const int len = cam_w - x_start;

    for (int y = 0; y < SCREEN_H; y++) {
        uint8_t* src_row = back_buffer + (y * cam_w);
        uint8_t* src = src_row + x_start;
        uint8_t* dst = vram + y * SCREEN_W;

        // Check how many pixels we can copy at once
        if (len >= SCREEN_W) {
            // The whole row can be copied in a single instruction
            memcpy(dst, src, SCREEN_W);
        } else {
            // The row needs to be copied in 2 parts,
            // from scroll_x end of row and from the rest
            // of the row that wrapped around
            memcpy(dst, src, len);
            memcpy(dst + len, src_row, SCREEN_W - len);
        }
    }
}

int main(int argc, char* argv[]) {
    // Get a pointer to VGA VRAM
    vram = (uint8_t*)(VIDEO_MEM_START + __djgpp_conventional_base);

    // Create a backbuffer same size of viewport
    back_buffer = malloc(camera.width * camera.height);

    setVideoMode(VIDEO_MODE_13h);
    setVideoPalette();

    /**
     * Enable nearptr to access memory below 1MB.
     *
     * DJGPP comes with a protection to not access
     * lower memory (below 1MB) but VGA memory is
     * below 1MB hence we need to tell DJGPP to
     * disable accessing data segment memory limits.
     *
     * Now we can access from 0x00000000 to 0xFFFFFFFF
     */
    if (__djgpp_nearptr_enable() == 0) {
        return 1;
    }

    waitVSync();
    drawMap();

    while (!kbhit()) {
        updateCamera(&camera);

        waitVSync();
        renderCamera(&camera);
    }

    // Go back to text mode
    setVideoMode(VIDEO_TEXT_MODE);

    // Re-enable DJGPP memory access protection
    __djgpp_nearptr_disable();

    free(back_buffer);
    return 0;
}
