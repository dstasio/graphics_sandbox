// demo.cpp

#include "gs/gs.h"

#include <math.h>

bool equals(float a, float b, float eps = 0.0001f)
{
    // @todo: correct float comparison
    bool result = fabsf(a - b) < (double)eps;
    return result;
}

typedef gs_v2 v2;
struct Ray
{
    v2 origin;
    v2 dir;
};
v2 &operator /= (v2 &a, float  b) { a.x /= b  ; a.y /= b  ; return a; }

float length(v2 vector)
{
    float result = vector.x*vector.x + vector.y*vector.y;
          result = sqrtf(result);
    return result;
};

v2 normalize(v2 vector)
{
    v2 result  = vector;
    float l = length(vector);
    if (equals(l, 0)) {
        // error
        return {};
    }

    if (equals(l, 1.f)) {
        return result;
    }

    result /= l;
    return result;
};

inline v2
get_orthogonal(v2 a)
{
    // crossing with v3(0, 0, 1)

    v2 result = {
         a.y*1.f,
        -a.x*1.f,
    };
    return result;
}

struct Refractive_Object
{
    //
    //               inside
    //  Start ------------------------ End
    //               outside
    //
    //
    //
    //               End
    //                |
    //                |
    //        inside  |  outside
    //                |
    //                |
    //                |
    //              Start
    //
    v2 start;
    v2   end;
};

void draw_refractive(Refractive_Object *object)
{
    uint32_t outside = GS_RGB(0x6c, 0xef, 0x4f);
    uint32_t  inside = GS_RGB(0xef, 0x32, 0x71);
    gs_draw_line(object->start, object->end, outside);
}

int main() {
    Ray rays[10] = {};

    {
        float x = 1300.f;
        float y =   10.f;
        v2  dir = normalize({-700.f, 0});
        for (int it = 0; it < _gs_array_len(rays); it += 1) {
            rays[it].origin = {x, y};
            rays[it].dir    = dir;

            y += 50.f;
        }
    }

    Refractive_Object refractive = {{500.f, 10.f}, {500.f, 900.f}};

    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(GS_GREY(0x2D));

        gs_draw_grid(100, GS_GREY(0x3D));

        // mouse stuff
        {
            v2 mouse = gs_state->current_input.mouse_pos;
               mouse = gs_screen_to_world(mouse);

            v2 o = {500, 400};
            gs_draw_line(o, mouse, GS_YELLOW);

            v2 orthogonal = get_orthogonal(mouse - o);
            gs_draw_line(o, o + orthogonal, GS_BLUE);
        }

        // rays
        float ray_len = 500.f;
        for (int it = 0; it < _gs_array_len(rays); it += 1)
        {
            gs_draw_line(rays[it].origin, rays[it].origin + rays[it].dir*ray_len, GS_RGB(0xA7, 0x35, 0x59));
        }

        draw_refractive(&refractive);

        gs_swap_buffers();
    }
}
