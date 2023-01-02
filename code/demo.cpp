// demo.cpp

#include "gs/gs.h"



int main() {
    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(0x2d, 0x2d, 0x2d);

        float x = 200;
        float y = 100;
        float size = 5.f;

        gs_draw_grid();

        if (gs_state->current_input.tab == GS_PRESSED) {
            gs_draw_point(x, y, 0xFF, 0xFF, 0xFF, size);
            x += size * 2.f;
            gs_draw_point(x, y, 0xFF, 0x00, 0x00, size);
            x += size * 2.f;
            gs_draw_point(x, y, 0x00, 0xFF, 0x00, size);
            x += size * 2.f;
            gs_draw_point(x, y, 0x00, 0x00, 0xFF, size);

            gs_draw_point(   0, y, 0x3F, 0x6C, 0x22, size);
            gs_draw_point(1024, y, 0x3F, 0x9C, 0x5C, size);

            gs_draw_point(   0,   0, 0xAA, 0x55, 0xDF, size);
            gs_draw_point(1024, 720, 0xAA, 0x55, 0xDF, size);
        }

        gs_swap();
    }
}
