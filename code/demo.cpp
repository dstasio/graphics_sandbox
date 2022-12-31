// demo.cpp

#include "gs/gs.h"



int main() {
    while(gs_window_2d())
    {
        //draw_polygon(points);

        //gs_draw_point(1, 5);


#define dash(length, x, y, r, g, b) \
        for (int it = 0; it < (length); it += 1) { \
            ((unsigned char *)gs_state->backbuffer)[(y) * gs_state->backbuffer_width * 4 + ((x) + it) * 4 + 0] = (b); \
            ((unsigned char *)gs_state->backbuffer)[(y) * gs_state->backbuffer_width * 4 + ((x) + it) * 4 + 1] = (g); \
            ((unsigned char *)gs_state->backbuffer)[(y) * gs_state->backbuffer_width * 4 + ((x) + it) * 4 + 2] = (r); \
        }

        int x = 100;
        int y =  50;
        dash(50, x, y, 0xFF, 0x00, 0x00);
        dash(50, x, y + 2, 0xFF, 0x00, 0x00);
        dash(50, x, y + 4, 0xFF, 0x00, 0x00);
        dash(50, x, y + 6, 0xFF, 0x00, 0x00);
        dash(50, x, y + 8, 0xFF, 0x00, 0x00);

        x += 50;
        dash(50, x, y, 0x00, 0xFF, 0x00);
        dash(50, x, y + 2, 0x00, 0xFF, 0x00);
        dash(50, x, y + 4, 0x00, 0xFF, 0x00);
        dash(50, x, y + 6, 0x00, 0xFF, 0x00);
        dash(50, x, y + 8, 0x00, 0xFF, 0x00);

        x += 50;
        dash(50, x, y, 0x00, 0x00, 0xFF);
        dash(50, x, y + 2, 0x00, 0x00, 0xFF);
        dash(50, x, y + 4, 0x00, 0x00, 0xFF);
        dash(50, x, y + 6, 0x00, 0x00, 0xFF);
        dash(50, x, y + 8, 0x00, 0x00, 0xFF);

        gs_swap();
    }
}
