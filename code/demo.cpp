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

        gs_draw_grid();

        if (gs_state->current_input.mouse_left == GS_PRESSED) {
            if (!is_creating_point) {
                point_count += 1;
                is_creating_point = true;

                if (point_count >= _gs_array_len(points))
                    point_count = 0;
            }

            points[point_count]  = gs_screen_point_to_world(gs_state->current_input.mouse_pos);
        }
        else if (gs_state->current_input.mouse_left == GS_JUST_RELEASED) {
            is_creating_point = false;
        }

        for (int it = 0; it <= point_count; it += 1) {
            gs_draw_point(points[it].x, points[it].y, GS_RGB(0xA7, 0x35, 0x59), 10.f);
        }

        gs_swap();
    }
}
