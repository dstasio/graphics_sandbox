// demo.cpp

#include "gs/gs.h"

#include <math.h>

#define PI 3.1415926535897932384626433832795028841971f
#define REFR_INDEX_AIR   1.000293f
#define REFR_INDEX_GLASS 1.52f

typedef gs_v2 v2;
union v3
{
    struct { float x, y, z; };
    float    row[3];
    v2       xy;
};

v3 to_v3(v2 v) {
    return {v.x, v.y, 0};
}

bool equals(float a, float b, float eps = 0.0001f)
{
    // @todo: correct float comparison
    bool result = fabsf(a - b) < (double)eps;
    return result;
}

v2 &operator /= (v2 &a, float  b) { a.x /= b  ; a.y /= b  ; return a; }

inline v2
get_orthogonal(v2 a)
{
    // crossing with v3(0, 0, 1)
    v2 result = { a.y, -a.x, };
    return result;
}

inline float
dot(v2 a, v2 b)
{
    float result = a.x*b.x + a.y*b.y;
    return result;
}

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

v2 transform_space(v2 vector, v2 new_up, v2 new_right)
{
    _gs_assert(equals(length(new_up), 1.f));
    _gs_assert(equals(length(new_right), 1.f));

    v2 result;
    result.x = dot(vector, new_right);
    result.y = dot(vector, new_up);
    return result;
};

struct Ray
{
    v2 origin;
    v2 dir;
};

v2 vector_from_angle(float angle)
{
    v2 result;
    result.x = cosf(angle);
    result.y = sinf(angle);
    return result;
};

inline v3
cross(v3 a, v3 b)
{
    v3 result = {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
    return result;
}

int main() {
    Ray rays[10] = {};

#define draw_ray(r, color) gs_draw_line(r.origin, r.origin + r.dir * 100.f, color);

    Ray mouseray;
    mouseray.origin = {200.f, 100.f};
    mouseray.dir    = {-1.f, 0};

    Ray normal;
    normal.origin = {200.f, 100.f};
    normal.dir    = {  0.f,   1.f};

    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(GS_GREY(0x2D));

        gs_draw_grid(100, GS_GREY(0x3D));

        v2 mouse = gs_state->current_input.mouse_pos;
        mouse = gs_screen_to_world(mouse);

        if (gs_state->current_input.mouse_left == GS_PRESSED)
            mouseray.dir = normalize(mouse - mouseray.origin);

        auto c = cross(to_v3(normal.dir), to_v3(mouseray.dir));
        auto cc = cross(c, to_v3(normal.dir));

        Ray cc_ray = {normal.origin, cc.xy};

        draw_ray(mouseray, GS_CYAN);
        draw_ray(  normal, GS_YELLOW);
        draw_ray(  cc_ray, GS_GREEN);

        cc = cross(c, to_v3(-normal.dir));
        cc_ray.dir = cc.xy;

        draw_ray(  cc_ray, GS_BLUE);

        gs_swap_buffers();
    }
}
