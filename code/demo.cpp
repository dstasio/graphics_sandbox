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

    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(GS_GREY(0x2D));

        gs_draw_grid(100, GS_GREY(0x3D));

        // rays
        float ray_len = 500.f;
        for (int it = 0; it < _gs_array_len(rays); it += 1)
        {
            gs_draw_line(rays[it].origin, rays[it].origin + rays[it].dir*ray_len, GS_RGB(0xA7, 0x35, 0x59));
        }

        gs_swap_buffers();
    }
}
