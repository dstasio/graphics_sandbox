// demo.cpp

#include "gs/gs.h"



int main() {
    gs_v2 points[10]  = {};
    int point_count = -1;
    bool is_creating_point = false;

    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(GS_GREY(0x2D));

        float x = 200;
        float y = 100;
        float size = 5.f;

        gs_draw_grid();

        if (gs_state->current_input.mouse_left == GS_PRESSED) {
            if (!is_creating_point) {
                point_count += 1;
                is_creating_point = true;

                if (point_count >= _gs_array_len(points))
                    point_count = 0;
            }

            points[point_count]  = gs_state->current_input.mouse_pos;
            points[point_count] *= 1.f / gs_state->view_scale;
            points[point_count] -= gs_state->origin;
            gs_draw_point(1022, 718, GS_RGB(0x0, 0xE4, 0x55), size);
        }
        else if (gs_state->current_input.mouse_left == GS_JUST_RELEASED) {
            is_creating_point = false;
        }

#if 0
        if (gs_state->current_input.mouse_left == GS_PRESSED) {
            gs_draw_point(x, y, GS_GREY(0xFF), size);
            x += size * 2.f;
            gs_draw_point(x, y, GS_RGB(0xFF, 0x00, 0x00), size);
            x += size * 2.f;
            gs_draw_point(x, y, GS_RGB(0x00, 0xFF, 0x00), size);
            x += size * 2.f;
            gs_draw_point(x, y, GS_RGB(0x00, 0x00, 0xFF), size);

            gs_draw_point(   0, y, GS_RGB(0x3F, 0x6C, 0x22), size);
            gs_draw_point(1024, y, GS_RGB(0x3F, 0x9C, 0x5C), size);

            gs_draw_point(   0,   0, GS_RGB(0xAA, 0x55, 0xDF), size);
            gs_draw_point(1024, 720, GS_RGB(0xAA, 0x55, 0xDF), size);
        }
#endif

        for (int it = 0; it <= point_count; it += 1) {
            gs_draw_point(points[it].x, points[it].y, GS_RGB(0xA7, 0x35, 0x59), 10.f);
        }

        gs_swap();
    }
}
