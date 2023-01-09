// demo.cpp

#include "gs/gs.h"

#include <math.h>

#define PI 3.1415926535897932384626433832795028841971f

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

v2 vector_from_angle(float angle)
{
    v2 result;
    result.x = cosf(angle);
    result.y = sinf(angle);
    return result;
};

struct Refractive_Segment
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

    float refractive_index;
};

void draw_refractive(Refractive_Segment *object)
{
    float inside_dist = 5.f;

    uint32_t outside_color = GS_RGB(0x6c, 0xef, 0x4f);
    uint32_t  inside_color = GS_RGB(0xef, 0x32, 0x71);
    gs_draw_line(object->start, object->end, outside_color);

    v2 right = normalize(get_orthogonal(object->end - object->start));
    gs_draw_line(object->start - right * inside_dist, object->end - right * inside_dist, inside_color);
}

// @note: returns negative number if there is no intersection.
float intersect(Refractive_Segment *object, Ray ray)
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

    v2 local_intersection = local_ray.origin + local_ray.dir * right_steps;
    if (local_intersection.y < 0)
        return -1.f;
    if (local_intersection.y > dot(object_up, object->end - object->start))
        return -1.f;
#if 0
    intersection = object_right * intersection.x + object_up * intersection.y;
    intersection += object->start;

    gs_draw_line(ray.origin, intersection, GS_GREEN);
#endif

    // @todo: return bool does_intersect
    return right_steps;
}

bool refract(Refractive_Segment *object, Ray incoming_ray, Ray *refracted_ray)
{
    float dist = intersect(object, incoming_ray);
    if (dist < 0.f)
        return false; // no intersection

    if (!refracted_ray)
        return true; // there is an intersection, but we don't bother computing it since there is not return pointer.

#if 0
    gs_draw_line(incoming_ray.origin, incoming_ray.origin + incoming_ray.dir * dist, GS_YELLOW);
#endif

    v2 intersection_point = incoming_ray.origin + incoming_ray.dir * dist;
    v2    object_normal       = normalize(get_orthogonal(object->end - object->start));
    float object_normal_angle = atan2f(object_normal.y, object_normal.x);

    float  incoming_angle  = atan2f(-incoming_ray.dir.y, -incoming_ray.dir.x);
    incoming_angle -= object_normal_angle;
    float refractive_index_air   = 1.000293f;
    float refracted_angle  = asinf((refractive_index_air / object->refractive_index) * sinf(incoming_angle));

    refracted_ray->origin = intersection_point;
    refracted_ray->dir    = vector_from_angle(PI + object_normal_angle + refracted_angle);
#if 0
    gs_draw_line(intersection_point, intersection_point + object_normal * 50, GS_BLUE);
    gs_draw_line(intersection_point, intersection_point - object_normal * 50, GS_BLUE);
    gs_draw_line(refracted_ray.origin, refracted_ray.origin + refracted_ray.dir * 500, GS_GREEN);
#endif

    return true;
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

    Refractive_Segment refractive = {{500.f, 10.f}, {300.f, 900.f}, 1.52f};

    while(gs_window_2d())
    {
        //draw_polygon(points);
        gs_clear(GS_GREY(0x2D));

        gs_draw_grid(100, GS_GREY(0x3D));


        if (gs_state->current_input.arrow_left == GS_PRESSED)
            refractive.end.x -= 2.f;
        if (gs_state->current_input.arrow_right == GS_PRESSED)
            refractive.end.x += 2.f;

        if (gs_state->current_input.arrow_up == GS_PRESSED)
            refractive.start.y += 2.f;
        if (gs_state->current_input.arrow_down == GS_PRESSED)
            refractive.start.y -= 2.f;


        // mouse stuff
#if 0
        {
            v2 mouse = gs_state->current_input.mouse_pos;
               mouse = gs_screen_to_world(mouse);

            Ray lightray;
            lightray.origin = {900.f, 500.f};
            lightray.dir    = normalize(mouse - lightray.origin);

            Ray refracted;
            if (refract(&refractive, lightray, &refracted))
            {
                gs_draw_line(refracted.origin, refracted.origin + refracted.dir * 500.f, GS_CYAN);
                gs_draw_line(lightray.origin, refracted.origin, GS_CYAN);
            }
            else 
                gs_draw_line(lightray.origin, lightray.origin + lightray.dir * 1000.f, GS_CYAN);
        }
#endif

        // rays
        for (int it = 0; it < _gs_array_len(rays); it += 1)
        {
            Ray refracted;
            if (refract(&refractive, rays[it], &refracted))
            {
                gs_draw_line(refracted.origin, refracted.origin + refracted.dir * 500.f, GS_RGB(0xA7, 0x35, 0x59));
                gs_draw_line(rays[it].origin, refracted.origin, GS_RGB(0xA7, 0x35, 0x59));
                continue;
            }

            gs_draw_line(rays[it].origin, rays[it].origin + rays[it].dir*2000.f, GS_RGB(0xA7, 0x35, 0x59));
        }

        draw_refractive(&refractive);

#if 1 // Line-Circumference intersection
        {
            float radius = 200.f;
            v2    center = {};
            // draw circumference
            float angle_step = 2 * PI / 128.f;
            
            v2 prev_point = {};
            v2 first_point = {};
            int index = 0;
            for (float angle = 0; angle < (2 * PI - 0.001f); angle += angle_step)
            {
                v2 radius_dir = { cosf(angle), sinf(angle) };
                radius_dir *= radius;
                radius_dir += center;

#if 0
                if (radius_dir.x < (lens_thickness * 0.5f))
                    continue;
#endif

                if (index++ > 0)
                    gs_draw_line(prev_point, radius_dir, GS_YELLOW);
                else
                    first_point = radius_dir;
                prev_point = radius_dir;
            }
            gs_draw_line(first_point, prev_point, GS_YELLOW);


            // line
            //Ray r = {{400, 300}, normalize({-1.f, -0.3f})};
            static Ray r = {{400, 100}, normalize({-1.f, 0.f})};
            if (gs_state->current_input.m == GS_PRESSED)
                r.dir = normalize(r.dir + gs_make_v2(0.f, 0.001f));
            if (gs_state->current_input.n == GS_PRESSED)
                r.dir = normalize(r.dir + gs_make_v2(0.f, -0.001f));
            if (gs_state->current_input.v == GS_JUST_RELEASED) {
                r.origin *= -1.f;
                r.dir *= -1.f;
            }

            gs_draw_line(r.origin, r.origin + r.dir * 700.f, GS_BLUE);

            v2 ray_normal = get_orthogonal(r.dir);
            float center_dist_from_line = dot(ray_normal, r.origin - center);
            if (fabsf(center_dist_from_line) <= radius) {
#if 0
                if (center_dist_from_line < 0.f) {
                    ray_normal            *= -1.f;
                    center_dist_from_line *= -1.f;
                }
#endif
                float intersection_angle = asinf(center_dist_from_line / radius);
                v2 intersection = { cosf(intersection_angle), center_dist_from_line / radius };
                intersection *= radius;

                gs_draw_line(center, center + ray_normal * 50.f, GS_GREEN);
                gs_draw_line(center, center + r.dir      * 50.f, GS_GREEN);
                intersection.x *= -1.f;
                intersection    = transform_space(intersection, ray_normal, r.dir);

                gs_draw_line(center, intersection, GS_BLUE);
            }
        }
#endif


#if 0
        {
            static v2 circle_center = {};
            if (gs_state->current_input.l == GS_PRESSED)
                circle_center.x += 0.5f;
            if (gs_state->current_input.h == GS_PRESSED)
                circle_center.x -= 0.5f;
            float lens_height    = 500.f;
            float lens_thickness = 200.f;

            gs_draw_point(circle_center.x, circle_center.y, GS_RED, 10.f);
#if 0
            gs_draw_point( lens_thickness * 0.5f,  lens_height * 0.5f, GS_CYAN);
            gs_draw_point(-lens_thickness * 0.5f,  lens_height * 0.5f, GS_CYAN);
            gs_draw_point( lens_thickness * 0.5f, -lens_height * 0.5f, GS_CYAN);
            gs_draw_point(-lens_thickness * 0.5f, -lens_height * 0.5f, GS_CYAN);
#endif

            float radius = length(gs_make_v2(lens_thickness * 0.5f, lens_height * 0.5f) - circle_center);


#define RESOLUTION 10
            v2 lens_face_points[RESOLUTION * 2 + 2] = {};
            int point_count = 0;

            v2 starting_radius = {lens_thickness * 0.5f, -lens_height * 0.5f};
            starting_radius -= circle_center;
            float max_angle = atan2f(starting_radius.y, starting_radius.x);
            float angle_step = -max_angle / (float)RESOLUTION;

            lens_face_points[point_count++] = starting_radius + circle_center;
            for (float angle = max_angle + angle_step; angle < -0.001f; angle += angle_step)
            {
                v2 radius_dir = { cosf(angle), sinf(angle) };
                radius_dir *= radius;
                radius_dir += circle_center;

#if 0
                if (radius_dir.x < (lens_thickness * 0.5f))
                    continue;
#endif

                lens_face_points[point_count++] = radius_dir;
            }

            lens_face_points[point_count++] = circle_center + gs_make_v2(radius, 0);

            //_gs_assert(point_count == RESOLUTION);
            for (int it = (point_count - 2); it >= 0; it -= 1) {
                v2 point = lens_face_points[it];
                point.y *= -1.f;
                lens_face_points[point_count++] = point;
            }
#if 1
            for (int it = 0; it < point_count; it += 1) {
                gs_draw_point(lens_face_points[it].x, lens_face_points[it].y, GS_YELLOW);

                if (it == 0) continue;
                gs_draw_line(lens_face_points[it - 1], lens_face_points[it], GS_BLUE);
            }
#endif

            v2 other_face[RESOLUTION * 2 + 2] = {};
            for (int it = 0; it < point_count; it += 1) {
                other_face[it] = lens_face_points[it];
                other_face[it].x *= -1.f;
                gs_draw_point(other_face[it].x, other_face[it].y, GS_GREEN);
            }
        }
#endif
        

        gs_swap_buffers();
    }
}
