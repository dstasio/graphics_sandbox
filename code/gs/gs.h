//#include "datatypes.h"

// @note: On windows, requires linking against 'Gdi32.lib'
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #define GS_WIN   1
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

#if GS_WIN
#include <windows.h>

struct _GS_Platform
{
    BITMAPINFO backbuffer_info;
};
#endif

struct GS_Input
{
    float mouse_pos_x;
    float mouse_pos_y;
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
    float origin_x;
    float origin_y;
    float view_scale;

    GS_Input current_input;
    GS_Input    last_input;
};

static GS_State * gs_state;
static GS_State  _gs_default_state;

bool gs_window_2d();
void gs_draw_point(float x, float y, uint8_t r = 0xFF, uint8_t g = 0xFF, uint8_t b = 0xFF, float point_size = 1);
void gs_swap();
void gs_clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);


// =========================================================================
// Settings

#define GS_PRESS_ESC_TO_CLOSE 1

// =========================================================================

#if GS_INTERNAL
    #define output_string(s, ...)        {char Buffer[100];sprintf_s(Buffer, s, __VA_ARGS__);OutputDebugStringA(Buffer);}
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

        RECT wnd_dims = {0, 0, 1024, 720};
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
        gs_state->backbuffer_width  = 1024;
        gs_state->backbuffer_height =  720;

        gs_state->_platform.backbuffer_info = {};
        gs_state->_platform.backbuffer_info.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        gs_state->_platform.backbuffer_info.bmiHeader.biWidth       = 1024;
        gs_state->_platform.backbuffer_info.bmiHeader.biHeight      = -720; // negative height -> origin is upper-left
        gs_state->_platform.backbuffer_info.bmiHeader.biPlanes      = 1;
        gs_state->_platform.backbuffer_info.bmiHeader.biBitCount    = 32;
        gs_state->_platform.backbuffer_info.bmiHeader.biCompression = BI_RGB;

        gs_state->backbuffer = VirtualAlloc(0,
                     gs_state->_platform.backbuffer_info.bmiHeader.biWidth * -gs_state->_platform.backbuffer_info.bmiHeader.biHeight * 4,
                     MEM_COMMIT|MEM_RESERVE,
                     PAGE_READWRITE);
        _gs_assert(gs_state->backbuffer);

        gs_state->view_scale = 1.f;
    } // end window initialization


    MSG message = {};

    gs_state->last_input    = gs_state->current_input;
    //gs_state->current_input = {};
    while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_KEYDOWN: {
#if GS_PRESS_ESC_TO_CLOSE
            {if (message.wParam == VK_ESCAPE) gs_state->running = false;}
#endif // GS_PRESS_ESC_TO_CLOSE
            } break;

            case WM_MBUTTONDOWN: {
                gs_state->is_dragging_origin = true;
            } break;
            case WM_MBUTTONUP: {
                gs_state->is_dragging_origin = false;
            } break;
            case WM_MOUSEWHEEL: {
                int wheel_delta = GET_WHEEL_DELTA_WPARAM(message.wParam);
                if (!wheel_delta) break;
                gs_state->view_scale += wheel_delta > 0 ? 0.1f : -0.1f;
            } break;

            case WM_MOUSEMOVE: {
                POINTS mouse_point = MAKEPOINTS(message.lParam);
                gs_state->current_input.mouse_pos_x = mouse_point.x;
                gs_state->current_input.mouse_pos_y = mouse_point.y;
            } break;

            default:
            {
                TranslateMessage(&message);
                 DispatchMessage(&message);
            } break;
        }
    }

    if (gs_state->is_dragging_origin) {
        gs_state->origin_x += (1.f / gs_state->view_scale) * (gs_state->current_input.mouse_pos_x - gs_state->last_input.mouse_pos_x);
        gs_state->origin_y += (1.f / gs_state->view_scale) * (gs_state->current_input.mouse_pos_y - gs_state->last_input.mouse_pos_y);
    }

    return gs_state->running;
}

void gs_swap()
{
    HDC device_context = GetDC((HWND)gs_state->window);

    StretchDIBits(device_context,
                  0, 0, gs_state->backbuffer_width, gs_state->backbuffer_height,
                  0, 0, gs_state->backbuffer_width, gs_state->backbuffer_height,
                  gs_state->backbuffer, &gs_state->_platform.backbuffer_info,
                  DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC((HWND)gs_state->window, device_context);
}

void gs_draw_point(float x, float y, uint8_t r, uint8_t g, uint8_t b, float point_size)
{
    uint32_t color = ((r << 16) |
                      (g <<  8) |
                      (b      ));

    x += gs_state->origin_x;
    y += gs_state->origin_y;

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

void gs_clear(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = ((r << 16) |
                      (g <<  8) |
                      (b      ));

    uint32_t *it = (uint32_t *)gs_state->backbuffer;
    for (int _ = 0; _ < (gs_state->backbuffer_width * gs_state->backbuffer_height); _ += 1) {
        *it = color;
        it += 1;
    }
}

#endif // GS_WIN
