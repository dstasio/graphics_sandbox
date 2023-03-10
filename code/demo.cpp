// demo.cpp

#include "gs/gs.h"

#include <math.h>

#define PI 3.1415926535897932384626433832795028841971f
#define REFR_INDEX_AIR   1.000293f
#define REFR_INDEX_GLASS 1.52f

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

struct Refractive_Arc
{
    v2    center_of_curvature;
    float radius;
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

bool refract_by_object(Refractive_Segment *object, Ray incoming_ray, Ray *refracted_ray)
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

Ray refract(v2 intersection, v2 normal_towards_incoming, Ray incoming_ray,
            float leaving_refractive_index, float entering_refractive_index)
             
{
    float normal_angle = atan2f(normal_towards_incoming.y, normal_towards_incoming.x);

    float  incoming_angle  = atan2f(-incoming_ray.dir.y, -incoming_ray.dir.x);
    incoming_angle -= normal_angle;
    float refracted_angle  = asinf((leaving_refractive_index / entering_refractive_index) * sinf(incoming_angle));

    Ray refracted = {};
    refracted.origin = intersection;
    refracted.dir    = vector_from_angle(PI + normal_angle + refracted_angle);
    return refracted;
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

    Refractive_Segment refractive = {{500.f, 10.f}, {300.f, 900.f}, REFR_INDEX_GLASS};

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
            if (refract_by_object(&refractive, lightray, &refracted))
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
            if (refract_by_object(&refractive, rays[it], &refracted))
            {
                gs_draw_line(refracted.origin, refracted.origin + refracted.dir * 500.f, GS_RGB(0xA7, 0x35, 0x59));
                gs_draw_line(rays[it].origin, refracted.origin, GS_RGB(0xA7, 0x35, 0x59));
                continue;
            }

            gs_draw_line(rays[it].origin, rays[it].origin + rays[it].dir*2000.f, GS_RGB(0xA7, 0x35, 0x59));
        }

        draw_refractive(&refractive);

#if 0 // Line-Circumference intersection
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

                if (index++ > 0)
                    gs_draw_line(prev_point, radius_dir, GS_YELLOW);
                else
                    first_point = radius_dir;
                prev_point = radius_dir;
            }
            gs_draw_line(first_point, prev_point, GS_YELLOW);


            // line
            static Ray r = {{400, 100}, normalize({-1.f, 0.f})};
            v2 ray_normal = get_orthogonal(r.dir);
            float center_dist_from_line = dot(ray_normal, r.origin - center);
            if (fabsf(center_dist_from_line) <= radius) {
                float intersection_angle = asinf(center_dist_from_line / radius);
                v2 intersection = { cosf(intersection_angle), center_dist_from_line / radius };
                intersection *= radius;

                intersection.x *= -1.f;
                intersection    = transform_space(intersection, ray_normal, r.dir);

                v2 intersection_normal = normalize(intersection - center );
                Ray refracted = refract(intersection, intersection_normal, r, REFR_INDEX_AIR, REFR_INDEX_GLASS);

                gs_draw_line(r.origin, intersection, GS_BLUE);
                gs_draw_line(refracted.origin, refracted.origin + refracted.dir * 200.f, GS_GREEN);
            }
        }
#endif


#if 1
        {
            float lens_height    = 500.f;
            float lens_thickness = 200.f;
#if 0
            gs_draw_point( lens_thickness * 0.5f,  lens_height * 0.5f, GS_CYAN);
            gs_draw_point(-lens_thickness * 0.5f,  lens_height * 0.5f, GS_CYAN);
            gs_draw_point( lens_thickness * 0.5f, -lens_height * 0.5f, GS_CYAN);
            gs_draw_point(-lens_thickness * 0.5f, -lens_height * 0.5f, GS_CYAN);
#endif


#define RESOLUTION 10
#if 1 // temp
            static
#endif // temp
            struct {
                v2 draw_points[500]; // @todo: dynamic/runtime-static array
                int draw_points_count;

                v2 center_of_curvature;
            } lens_face = {};
#if 1 // temp
            lens_face.draw_points_count = 0;
#endif // temp

            if (gs_state->current_input.l == GS_PRESSED)
                lens_face.center_of_curvature.x += 0.5f;
            if (gs_state->current_input.h == GS_PRESSED)
                lens_face.center_of_curvature.x -= 0.5f;
            gs_draw_point(lens_face.center_of_curvature.x, lens_face.center_of_curvature.y, GS_RED, 10.f);
            float radius = length(gs_make_v2(lens_thickness * 0.5f, lens_height * 0.5f) - lens_face.center_of_curvature);


            v2 starting_radius = {lens_thickness * 0.5f, -lens_height * 0.5f};
            starting_radius -= lens_face.center_of_curvature;
            float max_angle = atan2f(starting_radius.y, starting_radius.x);
            float angle_step = -max_angle / (float)RESOLUTION;

            lens_face.draw_points[lens_face.draw_points_count++] = starting_radius + lens_face.center_of_curvature;
            for (float angle = max_angle + angle_step; angle < -0.001f; angle += angle_step)
            {
                v2 radius_dir = { cosf(angle), sinf(angle) };
                radius_dir *= radius;
                radius_dir += lens_face.center_of_curvature;

                lens_face.draw_points[lens_face.draw_points_count++] = radius_dir;
            }

            lens_face.draw_points[lens_face.draw_points_count++] = lens_face.center_of_curvature + gs_make_v2(radius, 0);

            _gs_assert(lens_face.draw_points_count == (RESOLUTION + 1));
            for (int it = (lens_face.draw_points_count - 2); it >= 0; it -= 1) {
                v2 point = lens_face.draw_points[it];
                point.y *= -1.f;
                lens_face.draw_points[lens_face.draw_points_count++] = point;
            }
#if 1
            for (int it = 0; it < lens_face.draw_points_count; it += 1) {
                gs_draw_point(lens_face.draw_points[it].x, lens_face.draw_points[it].y, GS_YELLOW);

                if (it == 0) continue;
                gs_draw_line(lens_face.draw_points[it - 1], lens_face.draw_points[it], GS_BLUE);
            }
#endif

            // Line intersection
            static Ray r = {{400, 100}, normalize({-1.f, 0.f})};
            if (gs_state->current_input.n == GS_PRESSED)
                r.dir = normalize(r.dir + gs_make_v2(0.f, -0.005f));
            if (gs_state->current_input.m == GS_PRESSED)
                r.dir = normalize(r.dir + gs_make_v2(0.f,  0.005f));

            v2 ray_normal = get_orthogonal(r.dir);
            float center_dist_from_line = dot(ray_normal, r.origin - lens_face.center_of_curvature);
            if (fabsf(center_dist_from_line) <= radius) {
                float intersection_angle = asinf(center_dist_from_line / radius);

                gs_draw_point(cosf(max_angle) * radius, sinf(max_angle) * radius, GS_GREEN, 10.f);
                if ((intersection_angle >= max_angle) && (intersection_angle <= -max_angle)) {
                    if (gs_state->current_input.enter == GS_JUST_RELEASED)
                        inform("angle: %.2f\n", intersection_angle);
                    v2 intersection = { cosf(intersection_angle), center_dist_from_line / radius };
                    intersection *= radius;

                    intersection.x *= -1.f;
                    intersection    = transform_space(intersection, ray_normal, r.dir);
                    intersection   += lens_face.center_of_curvature;

                    v2 intersection_normal = normalize(intersection - lens_face.center_of_curvature);
                    Ray refracted = refract(intersection, intersection_normal, r, REFR_INDEX_AIR, REFR_INDEX_GLASS);

                    gs_draw_line(r.origin, intersection, GS_BLUE);
                    gs_draw_line(refracted.origin, refracted.origin + refracted.dir * 200.f, GS_GREEN);
                }
            }
        }
#endif
        

        gs_swap_buffers();
    }
}
