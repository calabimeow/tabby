#ifndef TABBY_H
#define TABBY_H

// version 1.0

#include <stdbool.h>

typedef struct vec2 {float x, y;} vec2;
typedef struct vec3 {float x, y, z;} vec3;
typedef struct rect {float x, y, w, h;} rect;
typedef struct color {float r, g, b, a;} color;

typedef struct texture
{
    int w, h;
    void *data;
}texture;

typedef struct sound
{
    void *data;
}sound;

typedef struct font
{
    void *data;
}font;

typedef struct cam2d
{
    vec2 target;
    vec2 offset;
    float zoom;
}cam2d;

typedef enum draw_mode
{
    FILL,
    LINE
}draw_mode;

typedef enum keyboard_key
{
    KEY_A = 4,
    KEY_B = 5,
    KEY_C = 6,
    KEY_D = 7,
    KEY_E = 8,
    KEY_F = 9,
    KEY_G = 10,
    KEY_H = 11,
    KEY_I = 12,
    KEY_J = 13,
    KEY_K = 14,
    KEY_L = 15,
    KEY_M = 16,
    KEY_N = 17,
    KEY_O = 18,
    KEY_P = 19,
    KEY_Q = 20,
    KEY_R = 21,
    KEY_S = 22,
    KEY_T = 23,
    KEY_U = 24,
    KEY_V = 25,
    KEY_W = 26,
    KEY_X = 27,
    KEY_Y = 28,
    KEY_Z = 29,
    KEY_1 = 30,
    KEY_2 = 31,
    KEY_3 = 32,
    KEY_4 = 33,
    KEY_5 = 34,
    KEY_6 = 35,
    KEY_7 = 36,
    KEY_8 = 37,
    KEY_9 = 38,
    KEY_0 = 39,
    KEY_ENTER = 40,
    KEY_ESCAPE = 41,
    KEY_BACKSPACE = 42,
    KEY_TAB = 43,
    KEY_SPACE = 44,
    KEY_MINUS = 45,
    KEY_EQUAL = 46,
    KEY_LEFT_BRACKET = 47,
    KEY_RIGHT_BRACKET = 48,
    KEY_BACKSLASH = 49,
    KEY_SEMICOLON = 51,
    KEY_APOSTROPHE = 52,
    KEY_GRAVE = 53,
    KEY_COMMA = 54,
    KEY_PERIOD = 55,
    KEY_SLASH = 56,
    KEY_CAPS_LOCK = 57,
    KEY_F1 = 58,
    KEY_F2 = 59,
    KEY_F3 = 60,
    KEY_F4 = 61,
    KEY_F5 = 62,
    KEY_F6 = 63,
    KEY_F7 = 64,
    KEY_F8 = 65,
    KEY_F9 = 66,
    KEY_F10 = 67,
    KEY_F11 = 68,
    KEY_F12 = 69,
    KEY_PRINT_SCREEN = 70,
    KEY_SCROLL_LOCK = 71,
    KEY_PAUSE = 72,
    KEY_INSERT = 73,
    KEY_HOME = 74,
    KEY_PAGE_UP = 75,
    KEY_DELETE = 76,
    KEY_END = 77,
    KEY_PAGE_DOWN = 78,
    KEY_RIGHT = 79,
    KEY_LEFT = 80,
    KEY_DOWN = 81,
    KEY_UP = 82,
    KEY_NUM_LOCK = 83,
    KEY_MENU = 118,
    KEY_LEFT_CONTROL = 224,
    KEY_LEFT_SHIFT = 225,
    KEY_LEFT_ALT = 226,
    KEY_LEFT_SUPER = 227,
    KEY_RIGHT_CONTROL = 228,
    KEY_RIGHT_SHIFT = 229,
    KEY_RIGHT_ALT = 230,
    KEY_RIGHT_SUPER = 231
}keyboard_key;

typedef enum mouse_button
{
    MOUSE_BUTTON_LEFT = 1,
    MOUSE_BUTTON_RIGHT = 3,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_BACK = 4,
    MOUSE_BUTTON_FORWARD = 5
}mouse_button;

#ifndef PI
    #define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
    #define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
    #define RAD2DEG (180.0f / PI)
#endif

#define BLACK           ((color){0, 0, 0, 255})
#define DARK_GRAY       ((color){128, 128, 128, 255})
#define DARK_RED        ((color){128, 0, 0, 255})
#define DARK_YELLOW     ((color){128, 128, 0, 255})
#define DARK_GREEN      ((color){0, 128, 0, 255})
#define TEAL            ((color){0, 128, 128, 255})
#define DARK_BLUE       ((color){0, 0, 128, 255})
#define DARK_PURPLE     ((color){128, 0, 128, 255})
#define OLIVE           ((color){128, 128, 64, 255})
#define DARKER_TEAL     ((color){0, 64, 64, 255})
#define AZURE           ((color){0, 128, 255, 255})
#define NAVY            ((color){0, 64, 128, 255})
#define PURPLE          ((color){128, 0, 255, 255})
#define BROWN           ((color){128, 64, 0, 255})
#define WHITE           ((color){255, 255, 255, 255})
#define GRAY            ((color){192, 192, 192, 255})
#define RED             ((color){255, 0, 0, 255})
#define YELLOW          ((color){255, 255, 0, 255})
#define GREEN           ((color){0, 255, 0, 255})
#define CYAN            ((color){0, 255, 255, 255})
#define BLUE            ((color){0, 0, 255, 255})
#define MAGENTA         ((color){255, 0, 255, 255})
#define LIGHT_YELLOW    ((color){255, 255, 128, 255})
#define LIGHT_GREEN     ((color){0, 255, 128, 255})
#define LIGHT_BLUE      ((color){128, 255, 255, 255})
#define SKY_BLUE        ((color){128, 128, 255, 255})
#define PINK            ((color){255, 0, 128, 255})
#define ORANGE          ((color){255, 128, 64, 255})

// window
void set_window_title(const char *title);
void set_screen_size(int w, int h);
vec2 get_screen_size(void);
int get_screen_width(void);
int get_screen_height(void);
void toggle_fullscreen(void);
void maximize_window(void);
void minimize_window(void);
void close_window(void);
vec2 get_screen_center(void);
bool is_fullscreen(void);

// mouse
int get_mouse_x(void);
int get_mouse_y(void);
vec2 get_mouse_pos(void);
vec2 get_mouse_delta(void);
vec2 get_mouse_wheel(void);

// input
bool is_key_down(keyboard_key key);
bool is_key_pressed(keyboard_key key);
bool is_key_released(keyboard_key key);
bool is_mouse_button_down(mouse_button button);
bool is_mouse_button_pressed(mouse_button button);
bool is_mouse_button_released(mouse_button button);

// frames
void set_target_fps(int fps);
int get_fps(void);
void draw_fps(font f, float x, float y, float size);
float get_delta_time(void);

//camera
cam2d cam2d_new(void);
void start2d(cam2d cam);
void end2d(void);
vec2 get_cam_pos(cam2d cam);
vec2 get_screen_to_world(vec2 pos, cam2d cam);
vec2 get_world_to_screen(vec2 world, cam2d cam);

// drawing
void clear_background(color col);
void draw_pixel(float x, float y, color col);
void draw_rect(float x, float y, float w, float h, color col);
void draw_rect_mode(float x, float y, float w, float h, draw_mode mode, color col);
void draw_circle(float x, float y, float r, int segments, color col);
void draw_triangle(vec2 v1, vec2 v2, vec2 v3, color col);
void draw_triangle_mode(vec2 v1, vec2 v2, vec2 v3, draw_mode mode, color col);
void draw_line(vec2 v1, vec2 v2, color col);
void draw_texture(texture text, int x, int y, color col);
void draw_texture_ex(texture text, vec2 pos, vec2 size, color col);
void draw_texture_src(texture tex, rect src, rect dst, color col);
void draw_text(font f, const char *text, float x, float y, float size, color col);
void draw_text_centered(font f, const char *text, float x, float y, float size, color col);

texture create_render_texture(int w, int h);
void start_texture_mode(texture tex);
void end_texture_mode(void);

float get_text_width(font f, const char *text, float size);
vec2 get_text_size(font f, const char *text, float size);

// sound
sound load_sound(const char *path);
void play_sound(sound snd);
void stop_sound(sound snd);
void set_sound_volume(sound snd, float volume);
int get_sound_volume(sound snd);
void set_sound_pan(sound snd, float pan);
bool is_sound_playing(sound snd);
void set_master_volume(float volume);
int get_master_volume(void);

// textures
texture load_texture(const char *path);
vec2 texture_size(texture tex);

font load_font(const char *path);

color color_255_to_1(color col);
color color_1_to_255(color col);

// collisions
bool check_collision_point_rect(vec2 p, rect r);
bool check_collision_point_circle(vec2 p, float cx, float cy, float cr);
bool check_collision_circle_rect(float cx, float cy, float cr, rect r);
bool check_collision_rects(rect r1, rect r2);
bool check_collision_circles(vec2 pos1, float r1, vec2 pos2, float r2);

// math
int get_random_value(int min, int max);
float lerp(float start, float end, float amount);
float clamp(float value, float min, float max);

vec2 vec2_zero(void);
vec2 vec2_lerp(vec2 start, vec2 end, float amount);
vec2 vec2_clamp(vec2 v, vec2 min, vec2 max);
vec2 vec2_add(vec2 v1, vec2 v2);
vec2 vec2_sub(vec2 v1, vec2 v2);
vec2 vec2_mul(vec2 v1, vec2 v2);
vec2 vec2_div(vec2 v1, vec2 v2);
vec2 vec2_scale(vec2 v, float value);
vec2 vec2_normalize(vec2 v);
vec2 vec2_negate(vec2 v);
vec2 vec2_dir(vec2 v1, vec2 v2);
float vec2_dot(vec2 v1, vec2 v2);
float vec2_cross(vec2 v1, vec2 v2);
float vec2_length(vec2 v);
float vec2_dist(vec2 v1, vec2 v2);

vec3 vec3_zero(void);
vec3 vec3_lerp(vec3 start, vec3 end, float amount);
vec3 vec3_clamp(vec3 v, vec3 min, vec3 max);
vec3 vec3_add(vec3 v1, vec3 v2);
vec3 vec3_sub(vec3 v1, vec3 v2);
vec3 vec3_mul(vec3 v1, vec3 v2);
vec3 vec3_div(vec3 v1, vec3 v2);
vec3 vec3_scale(vec3 v, float value);
vec3 vec3_normalize(vec3 v);
vec3 vec3_negate(vec3 v);
vec3 vec3_dir(vec3 v1, vec3 v2);
float vec3_dot(vec3 v1, vec3 v2);
vec3 vec3_cross(vec3 v1, vec3 v2);
float vec3_length(vec3 v);
float vec3_dist(vec3 v1, vec3 v2);

#endif // TABBY_H
