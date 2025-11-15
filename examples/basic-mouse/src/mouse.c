/**
 * Some of the functions here are not really used in the example
 * but are here just for educational purposes.
 *
 * More info about mouse system calls:
 * https://www.stanislavs.org/helppc/int_33.html
 *
 */

#include <dpmi.h>

#include "mouse.h"

/**
 * Reset & Get mouse status
 *
 * This function resets mouse driver to its default
 * and returns if mouse drivers are present and the
 * number of buttons available
 *
 * Default values are:
 *
 * - mouse is positioned to screen center
 * - mouse cursor is reset and hidden
 * - no interrupts are enabled (mask = 0)
 * - double speed threshold set to 64 mickeys per second
 * - horizontal mickey to pixel ratio (8 to 8)
 * - vertical mickey to pixel ratio (16 to 8)
 * - max width and height are set to maximum for video mode
 *
 */
void mouse_get_status(MouseState* state) {
    __dpmi_regs regs;
    regs.x.ax = MOUSE_GET_STATUS;
    __dpmi_int(MOUSE_INT, &regs);
    // Returns AX = FFFFh , if mouse support is available, otherwise, returns Ax = 0.
    state->detected = regs.x.ax != 0;
    state->num_buttons = regs.x.bx;
}

/**
 * Get mouse coordinates and buttons pressed state
 *
 */
void mouse_get_data(MouseState* state) {
    __dpmi_regs regs;
    regs.x.ax = MOUSE_GET_DATA;
    __dpmi_int(MOUSE_INT, &regs);
    state->buttons = regs.x.bx;
    state->x = regs.x.cx;
    state->y = regs.x.dx;
}

/**
 * Set horizontal delimiter rectangle
 *
 * Note: Each mouse driver may implement this
 *       differently leading to different results.
 *       Some may apply a unit rescale and others don't
 *
 */
void mouse_set_horizontal_limits(int min, int max) {
    __dpmi_regs regs;
    regs.x.ax = MOUSE_SET_H_LIMITS;
    regs.x.cx = min;
    regs.x.dx = max;
    __dpmi_int(MOUSE_INT, &regs);
}

/**
 * Set vertical delimiter rectangle
 *
 * Note: Each mouse driver may implement this
 *       differently leading to different results.
 *       Some may apply a unit rescale and others don't
 *
 */
void mouse_set_vertical_limits(int min, int max) {
    __dpmi_regs regs;
    regs.x.ax = MOUSE_SET_V_LIMITS;
    regs.x.cx = min;
    regs.x.dx = max;
    __dpmi_int(MOUSE_INT, &regs);
}

/**
 * Hide system pointer.
 *
 */
void mouse_hide_pointer() {
    __dpmi_regs regs;
    regs.x.ax = MOUSE_HIDE_POINTER;
    __dpmi_int(MOUSE_INT, &regs);
}

/**
 * Show system pointer.
 *
 */
void mouse_show_pointer() {
    __dpmi_regs regs;
    regs.x.ax = MOUSE_SHOW_POINTER;
    __dpmi_int(MOUSE_INT, &regs);
}