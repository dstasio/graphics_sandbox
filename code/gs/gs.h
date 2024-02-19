// Version 1

//
// Config variables
//
#define GS_INTERNAL 1

#define GS_DRAG_WITH_MIDDLE_CLICK 1
#define GS_DRAG_WITH_RIGHT_CLICK  0

#define GS_WINDOW_WIDTH  1500
#define GS_WINDOW_HEIGHT  950

//
// Platform flags
//
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #define GS_WIN   1
    #pragma comment(lib, "gdi32.lib")
    #pragma comment(lib, "user32.lib")
#elif defined(__APPLE__) || defined(__MACH__)
    #define GS_MACOS 1
#elif defined(unix) || defined(__unix) || defined(__unix__)
    #define GS_UNIX  1
#endif

#ifndef GS_WIN
    #define GS_WIN   0
#endif

#ifndef GS_MACOS
    #define GS_MACOS 0
#endif

#ifndef GS_UNIX
    #define GS_UNIX  0
#endif

#include <stdint.h>
#include <math.h>

#if GS_WIN
#include <windows.h>

// ===========================================================================
// Math
//
struct gs_v2
{
    float x;
    float y;
};
gs_v2  operator -  (gs_v2  a)          { return {-a.x, -a.y}; }
gs_v2  operator *  (gs_v2  a, float b) { return { a.x * b, a.y * b}; }
gs_v2  operator /  (gs_v2  a, float b) { return { a.x / b, a.y / b}; }
gs_v2  operator +  (gs_v2  a, gs_v2 b) { return { a.x + b.x, a.y + b.y }; }
gs_v2  operator -  (gs_v2  a, gs_v2 b) { return { a.x - b.x, a.y - b.y }; }
gs_v2 &operator *= (gs_v2 &a, float b) { a.x *= b  ; a.y *= b  ; return a; }
gs_v2 &operator *= (gs_v2 &a, gs_v2 b) { a.x *= b.x; a.y *= b.y; return a; }
gs_v2 &operator /= (gs_v2 &a, float b) { a.x /= b  ; a.y /= b  ; return a; }
gs_v2 &operator /= (gs_v2 &a, gs_v2 b) { a.x /= b.x; a.y /= b.y; return a; }
gs_v2 &operator += (gs_v2 &a, gs_v2 b) { a.x += b.x; a.y += b.y; return a; }
gs_v2 &operator -= (gs_v2 &a, gs_v2 b) { a.x -= b.x; a.y -= b.y; return a; }

struct gs_v3
{
    float x;
    float y;
    float z;
};
gs_v3  operator -  (gs_v3  a)          { return {-a.x, -a.y, -a.z}; }
gs_v3  operator +  (gs_v3  a, gs_v3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
gs_v3  operator -  (gs_v3  a, gs_v3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
gs_v3 &operator /= (gs_v3 &a, float b) { a.x /= b  ; a.y /= b  ; a.z /= b  ; return a; }
gs_v3 &operator /= (gs_v3 &a, gs_v3 b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }
gs_v3 &operator += (gs_v3 &a, gs_v3 b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
gs_v3 &operator -= (gs_v3 &a, gs_v3 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }

gs_v3 gs_cross(gs_v3 a, gs_v3 b) { return {a.y*b.z - a.z*b.y,
                                           a.z*b.x - a.x*b.z,
                                           a.x*b.y - a.y*b.x}; }

gs_v2 gs_make_v2(float x, float y)          { return {  x,   y   }; }
gs_v3 gs_make_v3(float x, float y, float z) { return {  x,   y, z}; }
gs_v3 gs_make_v3(         gs_v2 v, float z) { return {v.x, v.y, z}; }

// ==========================================================================
//

struct _GS_Platform
{
    BITMAPINFO backbuffer_info;
};
#endif

struct GS_Input
{
    gs_v2 mouse_pos;

#define GS_IDLE          0b00000000 // key was not interacted with recently
#define GS_PRESSED       0b00000001 // key is currently held down
#define GS_JUST_RELEASED 0b00000010 // key was being held until last frame and has just been relesed
    union {
        struct {
            uint8_t a;
            uint8_t b;
            uint8_t c;
            uint8_t d;
            uint8_t e;
            uint8_t f;
            uint8_t g;
            uint8_t h;
            uint8_t i;
            uint8_t j;
            uint8_t k;
            uint8_t l;
            uint8_t m;
            uint8_t n;
            uint8_t o;
            uint8_t p;
            uint8_t q;
            uint8_t r;
            uint8_t s;
            uint8_t t;
            uint8_t u;
            uint8_t v;
            uint8_t w;
            uint8_t x;
            uint8_t y;
            uint8_t z;

            uint8_t ctrl;
            uint8_t shift;
            uint8_t space;
            uint8_t tab;
            uint8_t enter;
            uint8_t arrow_right;
            uint8_t arrow_left;
            uint8_t arrow_up;
            uint8_t arrow_down;

            uint8_t mouse_left;
            uint8_t mouse_right;
            uint8_t mouse_middle;
        };

        uint8_t keys[38];
    };
};

struct GS_State
{
    void *window;
    bool  running;

    void *backbuffer;
    int   backbuffer_width;
    int   backbuffer_height;

    _GS_Platform _platform;

    bool  is_dragging_origin;
    gs_v2 origin;
    float view_scale;

    GS_Input current_input;
    GS_Input    last_input;
};

static GS_State * gs_state;
static GS_State  _gs_default_state;

// this works for windows, might need to be changed on other platforms
#define GS_RGB(r, g, b) (((r) << 16) | \
                         ((g) <<  8) | \
                         ((b)      ))
#define GS_GREY(grey_value) GS_RGB(grey_value, grey_value, grey_value)


#define GS_RED    GS_RGB(0xDF, 0x21, 0x00)
#define GS_GREEN  GS_RGB(0x00, 0xDF, 0x78)
#define GS_BLUE   GS_RGB(0x00, 0x87, 0xDF)

#define GS_YELLOW  GS_RGB(0xFF, 0xD3, 0x00)
#define GS_MAGENTA GS_RGB(0xF4, 0x66, 0xF4)
#define GS_CYAN    GS_RGB(0x00, 0xEF, 0xDF)

bool gs_window_2d();
void gs_draw_pixel(int32_t x, int32_t y, uint32_t color);
void gs_draw_grid(int grid_size = 100, uint32_t color = GS_GREY(0x6C), uint32_t x_axis_color = GS_RGB(0xC4, 0x02, 0x33), uint32_t y_axis_color = GS_RGB(0, 0x9F, 0x6B));
void gs_draw_point(float x, float y, uint32_t color = GS_GREY(0xFF), float point_size = 1);

void gs_draw_line          (gs_v2 start_point, gs_v2 end_point, uint32_t color);
void gs_draw_line_on_screen(int32_t x0, int32_t y0, uint32_t c0, int32_t x1, int32_t y1, uint32_t c1);
void gs_draw_arrow         (gs_v2 start_point, gs_v2 end_point, uint32_t color);
void gs_draw_quad          (float   minx, float   miny, float   maxx, float   maxy, uint32_t color);
void gs_draw_quad_on_screen(int32_t minx, int32_t miny, int32_t maxx, int32_t maxy, uint32_t color);
void gs_draw_quad_on_screen(gs_v2 p0, gs_v2 p1, gs_v2 p2, gs_v2 p3, uint32_t color);
void gs_draw_triangle_on_screen(gs_v2 p0, gs_v2 p1, gs_v2 p2, uint32_t color);

void gs_swap_buffers();
void gs_clear(uint32_t color = GS_GREY(0));

gs_v2 gs_screen_to_world(gs_v2 screen_point);
gs_v2 gs_world_to_screen(gs_v2 screen_point);

// =========================================================================
// Utils

inline
int32_t _gs_abs(int32_t a)
{
    if (a < 0) return -a;
    return a;
}

inline
float _gs_sign(float a)
{
    if (a < 0) return -1;
    return 1;
}


// =========================================================================
// Settings

#define GS_PRESS_ESC_TO_CLOSE 1

// =========================================================================

#define _gs_array_len(arr)   (sizeof(arr) / sizeof(arr[0]))
#define _gs_swap(a, b, type) { type _temp = (a); (a) = (b); (b) = _temp; }

#if GS_INTERNAL
#include <stdio.h>
    #define output_string(s, ...)        {char Buffer[1000];sprintf_s(Buffer, s, __VA_ARGS__);OutputDebugStringA(Buffer);}
    #define throw_error_and_exit(e, ...) {output_string(" ------------------------------[ERROR] "   ## e, __VA_ARGS__); getchar(); global_error = true;}
    #define throw_error(e, ...)           output_string(" ------------------------------[ERROR] "   ## e, __VA_ARGS__)
    #define inform(i, ...)                output_string(" ------------------------------[INFO] "    ## i, __VA_ARGS__)
    #define warn(w, ...)                  output_string(" ------------------------------[WARNING] " ## w, __VA_ARGS__)
    #define _gs_assert(expr) if(!(expr)) {*(int *)0 = 0;}
#else
    #define output_string(s, ...)
    #define throw_error_and_exit(e, ...)
    #define throw_error(e, ...)
    #define inform(i, ...)
    #define warn(w, ...)
    #define _gs_assert(expr)
#endif

#define GS_MIN_VIEW_SCALE 0.1f

#if GS_WIN
LRESULT CALLBACK
_gs_win32_window_proc(HWND window, UINT message, WPARAM w, LPARAM l)
{
    LRESULT result = 0;
    switch(message)
    {
        case WM_CLOSE:
        {
            DestroyWindow(window);
        } break;
            
        case WM_DESTROY:
        {
            if (gs_state)
                gs_state->running = false;
            PostQuitMessage(0);
        } break;

        case WM_SIZE:
        {
#if 0
            global_width  = LOWORD(l);
            global_height = HIWORD(l);
#endif
        } break;

        // @todo: implement this
        case WM_SETCURSOR:
        {
            HCURSOR cursor = LoadCursorA(0, IDC_CROSS);
            SetCursor(cursor);
        } break;

        default:
        {
          result = DefWindowProcA(window, message, w, l);
        } break;
    }

    return result;
}

bool gs_window_2d()
{
    if (!gs_state)
    {
        gs_state = &_gs_default_state;
        gs_state->origin = {500, 330};
    }

    if (!gs_state->window)
    { // begin window initialization
        HINSTANCE instance = GetModuleHandle(0);

        WNDCLASSA wnd_class       = {};
        wnd_class.style           = CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
        wnd_class.lpfnWndProc     = _gs_win32_window_proc;
        wnd_class.hInstance       = instance;
        wnd_class.lpszClassName   = "_gs_win32_window_class";
        auto Result               = RegisterClassA(&wnd_class);

        RECT wnd_dims = {0, 0, GS_WINDOW_WIDTH, GS_WINDOW_HEIGHT};
        AdjustWindowRect(&wnd_dims, WS_OVERLAPPEDWINDOW, FALSE);
        wnd_dims.right  -= wnd_dims.left;
        wnd_dims.bottom -= wnd_dims.top;

        gs_state->window = CreateWindowA("_gs_win32_window_class", "Graphics Sandbox",
                                         WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                         CW_USEDEFAULT, CW_USEDEFAULT,
                                         wnd_dims.right,
                                         wnd_dims.bottom,
                                         0, 0, instance, 0);

        gs_state->running = true;

        _gs_assert(gs_state->window);

        // creating backbuffer
        gs_state->backbuffer_width  = GS_WINDOW_WIDTH;
        gs_state->backbuffer_height = GS_WINDOW_HEIGHT;

        gs_state->_platform.backbuffer_info = {};
        gs_state->_platform.backbuffer_info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        gs_state->_platform.backbuffer_info.bmiHeader.biWidth       = GS_WINDOW_WIDTH;
        gs_state->_platform.backbuffer_info.bmiHeader.biHeight      = GS_WINDOW_HEIGHT;
        gs_state->_platform.backbuffer_info.bmiHeader.biPlanes      = 1;
        gs_state->_platform.backbuffer_info.bmiHeader.biBitCount    = 32;
        gs_state->_platform.backbuffer_info.bmiHeader.biCompression = BI_RGB;

        gs_state->backbuffer = VirtualAlloc(0,
                     gs_state->_platform.backbuffer_info.bmiHeader.biWidth * gs_state->_platform.backbuffer_info.bmiHeader.biHeight * 4,
                     MEM_COMMIT|MEM_RESERVE,
                     PAGE_READWRITE);
        _gs_assert(gs_state->backbuffer);

        gs_state->view_scale = 1.f;
    } // end window initialization


    MSG message = {};

    gs_state->last_input    = gs_state->current_input;

    for (int it = 0; it < _gs_array_len(gs_state->current_input.keys); it += 1) {
        if (gs_state->current_input.keys[it] == GS_JUST_RELEASED)
            gs_state->current_input.keys[it] = GS_IDLE;
    }
    //gs_state->current_input = {};
    while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_MBUTTONDOWN: {
                gs_state->current_input.mouse_middle = GS_PRESSED;
#if GS_DRAG_WITH_MIDDLE_CLICK
                gs_state->is_dragging_origin = true;
#endif
            } break;
            case WM_RBUTTONDOWN:
            {
                gs_state->current_input.mouse_right = GS_PRESSED;
#if GS_DRAG_WITH_RIGHT_CLICK
                gs_state->is_dragging_origin = true;
#endif
            } break;

            case WM_LBUTTONDOWN: {
                gs_state->current_input.mouse_left = GS_PRESSED;
            } break;
            case WM_LBUTTONUP: {
                gs_state->current_input.mouse_left = GS_JUST_RELEASED;
            } break;

            case WM_MBUTTONUP: {
                gs_state->current_input.mouse_middle = GS_JUST_RELEASED;
#if GS_DRAG_WITH_MIDDLE_CLICK
                gs_state->is_dragging_origin = false;
#endif
            } break;
            case WM_RBUTTONUP: {
                gs_state->current_input.mouse_right = GS_JUST_RELEASED;
#if GS_DRAG_WITH_RIGHT_CLICK
                gs_state->is_dragging_origin = false;
#endif
            } break;
            case WM_MOUSEWHEEL: {
                gs_v2 old_mouse_world = gs_screen_to_world(gs_state->current_input.mouse_pos);

                int wheel_delta = GET_WHEEL_DELTA_WPARAM(message.wParam);
                if (!wheel_delta) break;
                gs_state->view_scale += wheel_delta > 0 ? 0.1f : -0.1f;
                if (gs_state->view_scale < GS_MIN_VIEW_SCALE)
                    gs_state->view_scale = GS_MIN_VIEW_SCALE;

                gs_v2 new_mouse_world = gs_screen_to_world(gs_state->current_input.mouse_pos);

                gs_state->origin += new_mouse_world - old_mouse_world;
            } break;

            case WM_MOUSEMOVE: {
                POINTS mouse_point = MAKEPOINTS(message.lParam);
                gs_state->current_input.mouse_pos.x = mouse_point.x;
                gs_state->current_input.mouse_pos.y = GS_WINDOW_HEIGHT - (float)mouse_point.y;
            } break;

#define _GS_Keydown(key, scancode) else if(message.wParam == (scancode))  gs_state->current_input.key = GS_PRESSED
#define _GS_Keyup(key, scancode)   else if(message.wParam == (scancode))  gs_state->current_input.key = GS_JUST_RELEASED
            case WM_KEYDOWN: {
#if GS_PRESS_ESC_TO_CLOSE
            {if (message.wParam == VK_ESCAPE) gs_state->running = false;}
#endif // GS_PRESS_ESC_TO_CLOSE

                if (0) {}
                _GS_Keydown(a, 'A');
                _GS_Keydown(b, 'B');
                _GS_Keydown(c, 'C');
                _GS_Keydown(d, 'D');
                _GS_Keydown(e, 'E');
                _GS_Keydown(f, 'F');
                _GS_Keydown(g, 'G');
                _GS_Keydown(h, 'H');
                _GS_Keydown(i, 'I');
                _GS_Keydown(j, 'J');
                _GS_Keydown(k, 'K');
                _GS_Keydown(l, 'L');
                _GS_Keydown(m, 'M');
                _GS_Keydown(n, 'N');
                _GS_Keydown(o, 'O');
                _GS_Keydown(p, 'P');
                _GS_Keydown(q, 'Q');
                _GS_Keydown(r, 'R');
                _GS_Keydown(s, 'S');
                _GS_Keydown(t, 'T');
                _GS_Keydown(u, 'U');
                _GS_Keydown(v, 'V');
                _GS_Keydown(w, 'W');
                _GS_Keydown(x, 'X');
                _GS_Keydown(y, 'Y');
                _GS_Keydown(z, 'Z');

                _GS_Keydown(       ctrl, VK_CONTROL);
                _GS_Keydown(      shift, VK_SHIFT);
                _GS_Keydown(      space, VK_SPACE);
                _GS_Keydown(        tab, VK_TAB);
                _GS_Keydown(      enter, VK_RETURN);
                _GS_Keydown(arrow_right, VK_RIGHT);
                _GS_Keydown(arrow_left , VK_LEFT);
                _GS_Keydown(arrow_up   , VK_UP);
                _GS_Keydown(arrow_down , VK_DOWN);
            } break;
            case WM_KEYUP:
            {
                if (0) {}
                _GS_Keyup(a, 'A');
                _GS_Keyup(b, 'B');
                _GS_Keyup(c, 'C');
                _GS_Keyup(d, 'D');
                _GS_Keyup(e, 'E');
                _GS_Keyup(f, 'F');
                _GS_Keyup(g, 'G');
                _GS_Keyup(h, 'H');
                _GS_Keyup(i, 'I');
                _GS_Keyup(j, 'J');
                _GS_Keyup(k, 'K');
                _GS_Keyup(l, 'L');
                _GS_Keyup(m, 'M');
                _GS_Keyup(n, 'N');
                _GS_Keyup(o, 'O');
                _GS_Keyup(p, 'P');
                _GS_Keyup(q, 'Q');
                _GS_Keyup(r, 'R');
                _GS_Keyup(s, 'S');
                _GS_Keyup(t, 'T');
                _GS_Keyup(u, 'U');
                _GS_Keyup(v, 'V');
                _GS_Keyup(w, 'W');
                _GS_Keyup(x, 'X');
                _GS_Keyup(y, 'Y');
                _GS_Keyup(z, 'Z');

                _GS_Keyup(       ctrl, VK_CONTROL);
                _GS_Keyup(      shift, VK_SHIFT);
                _GS_Keyup(      space, VK_SPACE);
                _GS_Keyup(        tab, VK_TAB);
                _GS_Keyup(      enter, VK_RETURN);
                _GS_Keyup(arrow_right, VK_RIGHT);
                _GS_Keyup(arrow_left , VK_LEFT);
                _GS_Keyup(arrow_up   , VK_UP);
                _GS_Keyup(arrow_down , VK_DOWN);
            } break;

            default:
            {
                TranslateMessage(&message);
                 DispatchMessage(&message);
            } break;
        }
    }

    if (gs_state->is_dragging_origin) {
        gs_state->origin.x += (1.f / gs_state->view_scale) * (gs_state->current_input.mouse_pos.x - gs_state->last_input.mouse_pos.x);
        gs_state->origin.y += (1.f / gs_state->view_scale) * (gs_state->current_input.mouse_pos.y - gs_state->last_input.mouse_pos.y);
    }

    return gs_state->running;
}

void gs_swap_buffers()
{
    HDC device_context = GetDC((HWND)gs_state->window);

    StretchDIBits(device_context,
                  0, 0, gs_state->backbuffer_width, gs_state->backbuffer_height,
                  0, 0, gs_state->backbuffer_width, gs_state->backbuffer_height,
                  gs_state->backbuffer, &gs_state->_platform.backbuffer_info,
                  DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC((HWND)gs_state->window, device_context);
}

void gs_draw_grid(int grid_size, uint32_t color, uint32_t x_axis_color, uint32_t y_axis_color)
{
    grid_size = (int)((float)grid_size * gs_state->view_scale);

    gs_v2 scaled_origin = gs_state->origin * gs_state->view_scale;

    gs_v2 grid_start = scaled_origin;
#if 0
    if (grid_start.x < 0)
        grid_start.x = 0;
#endif

    float xx = grid_start.x;
    while ((int)xx < gs_state->backbuffer_width)
    {
        if (xx < 0) {
            xx += grid_size;
            continue;
        }
        for (int yy = 0; yy < gs_state->backbuffer_height; yy += 1) {
            ((uint32_t *)gs_state->backbuffer)[yy * gs_state->backbuffer_width + (int)xx] = color;
        }

        xx += grid_size;
    }

    float yy = grid_start.y;
    while ((int)yy < gs_state->backbuffer_height)
    {
        if (yy < 0) {
            yy += grid_size;
            continue;
        }
        for (int xx = 0; xx < gs_state->backbuffer_width; xx += 1) {
            ((uint32_t *)gs_state->backbuffer)[(int)yy * gs_state->backbuffer_width + xx] = color;
        }

        yy += grid_size;
    }


    // drawing axes
    {
        // x axis
        if (scaled_origin.y >= 0 && scaled_origin.y < (float)gs_state->backbuffer_height) {
            for (int xx = 0; xx < gs_state->backbuffer_width; xx += 1) {
                ((uint32_t *)gs_state->backbuffer)[(int)scaled_origin.y * gs_state->backbuffer_width + xx] = x_axis_color;
            }
        }

        // y axis
        if (scaled_origin.x >= 0 && scaled_origin.x < (float)gs_state->backbuffer_width) {
            for (int yy = 0; yy < gs_state->backbuffer_height; yy += 1) {
                ((uint32_t *)gs_state->backbuffer)[yy * gs_state->backbuffer_width + (int)scaled_origin.x] = y_axis_color;
            }
        }
    }
}

void gs_draw_point(float x, float y, uint32_t color, float point_size)
{
    x += gs_state->origin.x;
    y += gs_state->origin.y;

    x *= gs_state->view_scale;
    y *= gs_state->view_scale;

    // @todo: coordinate mapping here
    for (int xx = (int)x - (int)point_size; xx <= ((int)x + (int)point_size); xx += 1) {
    for (int yy = (int)y - (int)point_size; yy <= ((int)y + (int)point_size); yy += 1) {
        if (xx < 0 || xx >= gs_state->backbuffer_width)
            continue;
        if (yy < 0 || yy >= gs_state->backbuffer_height)
            continue;
        ((uint32_t *)gs_state->backbuffer)[yy * gs_state->backbuffer_width + xx] = color;
    }}
}

void gs_draw_pixel(int32_t x, int32_t y, uint32_t color)
{
    // @todo: coordinate mapping here
    if (x < 0 || x >= gs_state->backbuffer_width)
        return;
    if (y < 0 || y >= gs_state->backbuffer_height)
        return;
    ((uint32_t *)gs_state->backbuffer)[y * gs_state->backbuffer_width + x] = color;
}

void gs_draw_quad(float minx, float miny, float maxx, float maxy, uint32_t color)
{
    _gs_assert(minx <= maxx);
    _gs_assert(miny <= maxy);

    _gs_assert(!isnan(minx));
    _gs_assert(!isnan(miny));
    _gs_assert(!isnan(maxx));
    _gs_assert(!isnan(maxy));
    gs_v2 screen_min = gs_world_to_screen({minx, miny});
    gs_v2 screen_max = gs_world_to_screen({maxx, maxy});

    gs_draw_quad_on_screen((int32_t)screen_min.x, (int32_t)screen_min.y,
                           (int32_t)screen_max.x, (int32_t)screen_max.y, color);
}

void gs_draw_quad_on_screen(int32_t minx, int32_t miny, int32_t maxx, int32_t maxy, uint32_t color)
{
    _gs_assert(minx <= maxx);
    _gs_assert(miny <= maxy);

    for (int32_t xx = minx; xx < maxx; xx += 1) {
    for (int32_t yy = miny; yy < maxy; yy += 1) {
        if (xx < 0 || xx >= gs_state->backbuffer_width)
            continue;
        if (yy < 0 || yy >= gs_state->backbuffer_height)
            continue;

        ((uint32_t *)gs_state->backbuffer)[yy * gs_state->backbuffer_width + xx] = color;
    }}
}

void gs_draw_quad_on_screen(gs_v2 p0, gs_v2 p1, gs_v2 p2, gs_v2 p3, uint32_t color) {
    gs_draw_triangle_on_screen(p0, p1, p2, color);
    gs_draw_triangle_on_screen(p0, p2, p3, color);
}

void gs_clear(uint32_t color)
{
    uint32_t *it = (uint32_t *)gs_state->backbuffer;
    for (int _ = 0; _ < (gs_state->backbuffer_width * gs_state->backbuffer_height); _ += 1) {
        *it = color;
        it += 1;
    }
}

gs_v2 gs_screen_to_world(gs_v2 screen_point)
{
    gs_v2 world_point = screen_point;
    world_point *= 1.f / gs_state->view_scale;
    world_point -= gs_state->origin;
    return world_point;
}

gs_v2 gs_world_to_screen(gs_v2 screen_point)
{
    screen_point.x += gs_state->origin.x;
    screen_point.y += gs_state->origin.y;

    screen_point.x *= gs_state->view_scale;
    screen_point.y *= gs_state->view_scale;

    return screen_point;
}

void gs_draw_line(gs_v2 start_point, gs_v2 end_point, uint32_t color)
{
    _gs_assert(!isnan(start_point.x));
    _gs_assert(!isnan(start_point.y));
    _gs_assert(!isnan(end_point.x));
    _gs_assert(!isnan(end_point.y));
    start_point = gs_world_to_screen(start_point);
      end_point = gs_world_to_screen(  end_point);

    gs_draw_line_on_screen((int32_t)start_point.x, (int32_t)start_point.y, color,
                           (int32_t)  end_point.x, (int32_t)  end_point.y, color);
}

void gs_draw_line_on_screen(int32_t x0, int32_t y0, uint32_t c0,
                            int32_t x1, int32_t y1, uint32_t c1)
{
    int32_t x, y;
    int32_t *pixel_x = &x;
    int32_t *pixel_y = &y;

    int32_t dx = x1 - x0,
            dy = y1 - y0;
    if (_gs_abs(dx) < _gs_abs(dy))
    {
        _gs_swap(x0, y0, int32_t);
        _gs_swap(x1, y1, int32_t);
        _gs_swap(dx, dy, int32_t);
        pixel_x = &y;
        pixel_y = &x;
    }

    if (x1 < x0) {
        _gs_swap(x0, x1,  int32_t);
        _gs_swap(y0, y1,  int32_t);
        _gs_swap(c0, c1, uint32_t);
        dx = -dx;
        dy = -dy;
    }

    int32_t e = 0,
            i = 1;
    y = y0;

    if (y1 < y0)
        i = -1;

    //float color_t = 0.f;
    //float color_tStep = 1.f / (float)(dx);
    for (x = x0; x <= x1; x += 1)  {
        //setPixel(*pixelX, *pixelY, lerp(c0, c1, color_t));
        //color_t += color_tStep;
        gs_draw_pixel(*pixel_x, *pixel_y, c0);

        e += dy;
        if ( ((e << 1)*i) >= dx) {
            y += 1*i;
            e -= dx*i;
        }
    }
}

void gs_draw_arrow(gs_v2 start_point, gs_v2 end_point, uint32_t color)
{
    gs_draw_line(start_point, end_point, color);
    gs_draw_point(end_point.x, end_point.y, color, 1);
}


void gs_draw_triangle_on_screen(gs_v2 p0, gs_v2 p1, gs_v2 p2, uint32_t color) {
    // slow, accurate version
    gs_v3 min = {p0.x, p0.y, 0};
    gs_v3 max = {p0.x, p0.y, 0};

    if (p1.x < min.x)  min.x = p1.x;
    if (p2.x < min.x)  min.x = p2.x;
    if (p1.y < min.y)  min.y = p1.y;
    if (p2.y < min.y)  min.y = p2.y;
    if (p1.x > max.x)  max.x = p1.x;
    if (p2.x > max.x)  max.x = p2.x;
    if (p1.y > max.y)  max.y = p1.y;
    if (p2.y > max.y)  max.y = p2.y;

    //gs_draw_quad_on_screen((int32_t)min.x, (int32_t)min.y,
                           //(int32_t)max.x, (int32_t)max.y,
                           //GS_GREY(200));

    int32_t min_x = (int32_t)min.x;
    int32_t min_y = (int32_t)min.y;
    int32_t max_x = (int32_t)max.x;
    int32_t max_y = (int32_t)max.y;

    gs_v3 v01 = gs_make_v3(p1 - p0, 0);
    gs_v3 v02 = gs_make_v3(p2 - p0, 0);
    gs_v3 v12 = gs_make_v3(p2 - p1, 0);
    for (int y = min_y; y <= max_y; y += 1) {
    for (int x = min_x; x <= max_x; x += 1) {
        gs_v3 v = {(float)x, (float)y, 0};

        gs_v3 p0_to_v = v - gs_make_v3(p0, 0);
        gs_v3 p1_to_v = v - gs_make_v3(p1, 0);
        float s01 = _gs_sign(gs_cross(v01    , p0_to_v).z);
        float s02 = _gs_sign(gs_cross(p0_to_v,     v02).z);
        float s12 = _gs_sign(gs_cross(v12    , p1_to_v).z);

        if (s01 != s02)  continue;
        if (s01 != s12)  continue;

        gs_draw_pixel(x, y, color);
    }}
}

#if 0
void _gs_draw_triangle_on_screen(gs_v2 p0, gs_v2 p1, gs_v2 p2, uint32_t color) {
    int32_t x, y;
    int32_t *pixel_x = &x;
    int32_t *pixel_y = &y;

    // @todo: sub-pixel precision
    // @todo: maybe round instead of truncating?
    int32_t dx = (int32_t)(p1.x - p0.x),
            dy = (int32_t)(p1.y - p0.y);
    if (_gs_abs(dx) < _gs_abs(dy))
    {
	// @todo: do colors also need to be swapped?
	_gs_swap(p0.x, p0.y, float);
	_gs_swap(p1.x, p1.y, float);
	_gs_swap(dx, dy, uint32_t);
	pixel_x = &y;
	pixel_y = &x;
    }

    if (p1.x < p0.x) {
	_gs_swap(p0, p1, gs_v2);
	dx = -dx;
	dy = -dy;
    }

    int32_t e = 0,
	    i = 1;
    y = (int32_t)p0.y;

    if (p1.y < p0.y)  i = -1;

    //r32 total_dist = distance(p0, p1);
    for (x = (int32_t)p0.x; x <= (int32_t)p1.x; x += 1)  {
        // @todo: do this to lerp colors.
	//gs_v2 curr_p = {(float)x, (float)y};
	//float      t = distance(p0, curr_p) / total_dist;
	//gs_v2     uv =    lerp(p0.uv,uv, t);

	gs_draw_line_on_screen(     *pixel_x,      *pixel_y, color,
                               (int32_t)p2.x, (int32_t)p2.y, color);

	e += dy;
	if ( ((e << 1)*i) >= dx) {
	    y +=  1*i;
	    e -= dx*i;
	}
    }
}
#endif

#endif // GS_WIN
