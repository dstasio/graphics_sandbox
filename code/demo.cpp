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
    float inside_dist = 5.f;

    uint32_t outside_color = GS_RGB(0x6c, 0xef, 0x4f);
    uint32_t  inside_color = GS_RGB(0xef, 0x32, 0x71);
    gs_draw_line(object->start, object->end, outside_color);

    v2 right = normalize(get_orthogonal(object->end - object->start));
    gs_draw_line(object->start - right * inside_dist, object->end - right * inside_dist, inside_color);
}

float intersect(Refractive_Object *object, Ray ray)
{
    float _len = 50.f;

    v2 object_up    = normalize(object->end - object->start);
    v2 object_right = normalize(get_orthogonal(object_up));

#if 0
    gs_draw_line(ray.origin, ray.origin + ray.dir * _len, GS_YELLOW);
#endif

    Ray local_ray = ray;
    local_ray.origin -= object->start;
    //ray.dir    -= object->start;
    local_ray.origin = transform_space(local_ray.origin, object_up, object_right);
    local_ray.dir    = transform_space(local_ray.   dir, object_up, object_right);

#if 0
    gs_draw_line(object->start, object->end, GS_BLUE);
    gs_draw_line(object->start, object->start + object_up    * _len, GS_RED);
    gs_draw_line(object->start, object->start + object_right * _len, GS_RED);
#endif


    float target_right = 0.f;
    float right_steps = (-local_ray.origin.x) / local_ray.dir.x;

#if 0
    v2 intersection = local_ray.origin + local_ray.dir * right_steps;
    intersection = object_right * intersection.x + object_up * intersection.y;
    intersection += object->start;

    gs_draw_line(ray.origin, intersection, GS_GREEN);
#endif

    // @todo: return bool does_intersect
    return right_steps;
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

    Refractive_Object refractive = {{500.f, 10.f}, {300.f, 900.f}};

    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(GS_GREY(0x2D));

        gs_draw_grid(100, GS_GREY(0x3D));

        // mouse stuff
#if 1
        {
            v2 mouse = gs_state->current_input.mouse_pos;
               mouse = gs_screen_to_world(mouse);

            Ray r;
            r.origin = {900.f, 500.f};
            r.dir    = normalize(mouse - r.origin);
            float dist = intersect(&refractive, r);
            gs_draw_line(r.origin, r.origin + r.dir * dist, GS_YELLOW);
        }
#endif

        // rays
        for (int it = 0; it < _gs_array_len(rays); it += 1)
        {
            float ray_len = intersect(&refractive, rays[it]);
            gs_draw_line(rays[it].origin, rays[it].origin + rays[it].dir*ray_len, GS_RGB(0xA7, 0x35, 0x59));
        }

        draw_refractive(&refractive);

        gs_swap_buffers();
    }
}
