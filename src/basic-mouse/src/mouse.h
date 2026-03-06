#ifndef _MOUSE_H
#define _MOUSE_H

#define MOUSE_INT 0x33
enum MouseService {
    MOUSE_GET_STATUS = 0x00,
    MOUSE_SHOW_POINTER = 0x01,
    MOUSE_HIDE_POINTER = 0x02,
    MOUSE_GET_DATA = 0x03,
    MOUSE_SET_POS = 0x04,
    MOUSE_SET_H_LIMITS = 0x07,
    MOUSE_SET_V_LIMITS = 0x08,
    MOUSE_GET_COUNTERS = 0x0B
};

enum MouseButton {
    MOUSE_NO_BTN = 0x00,
    MOUSE_LEFT_BTN = 0x01,
    MOUSE_RIGHT_BTN = 0x02,
    MOUSE_MID_BTN = 0x03,
};

typedef struct MouseState {
    unsigned char detected;
    int num_buttons;
    int x, y, buttons;
} MouseState;

void mouse_get_status(MouseState* state);
void mouse_get_data(MouseState* state);
void mouse_set_horizontal_limits(int min, int max);
void mouse_set_vertical_limits(int min, int limit);
void mouse_hide_pointer();
void mouse_show_pointer();

#endif  // _MOUSE_H