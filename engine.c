#include "tabby.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef void (*init_fn)(void);
typedef void (*update_fn)(void);
typedef void (*draw_fn)(void);

static SDL_Window *window;
static SDL_Renderer *renderer;
static bool fullscreen;
static SDL_SharedObject *game;
static init_fn game_init;
static update_fn game_update;
static draw_fn game_draw;

static Uint32 mouse_buttons;
static Uint32 mouse_buttons_prev;
static bool keys[512];
static bool keys_prev[512];

static MIX_Mixer *mixer;

static int frame_count;
static int fps;
static int target_fps;
static float acc_time;
static float delta_time;

static TTF_TextEngine *text_engine;

static vec2 mouse_pos_prev;
static vec2 mouse_delta;
static vec2 mouse_wheel;
static vec2 mouse_pos;

static cam2d current_cam;

int tabby_run(const char *project_name)
{
    static bool init = false;

    if (!init)
    {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        TTF_Init();
        MIX_Init();
        mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

        window = SDL_CreateWindow(project_name, 800, 600, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, "opengl");
        text_engine = TTF_CreateRendererTextEngine(renderer);
        current_cam = cam2d_new();
        
        char str[512];
        #ifdef _WIN32
            snprintf(str, sizeof(str), "%s.dll", project_name);
        #else
            snprintf(str, sizeof(str), "lib%s.so", project_name);
        #endif

        game = SDL_LoadObject(str);
        game_init = (init_fn)SDL_LoadFunction(game, "tabby_init");
        game_update = (update_fn)SDL_LoadFunction(game, "tabby_update");
        game_draw = (draw_fn)SDL_LoadFunction(game, "tabby_draw");

        if (!game_update)
        {
            fprintf(stderr, "Missing tabby_update function\n");
            return 1;
        }

        if (!game_draw)
        {
            fprintf(stderr, "Missing tabby_draw function\n");
            return 1;
        }

        if (!game_init)
        {
            fprintf(stderr, "Missing tabby_init function\n");
            return 1;
        }

        game_init();

        init = true;
    }

    bool running = true;
    SDL_Event event;
    Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 perf_counter  = SDL_GetPerformanceCounter();

    while (running) 
    {
        Uint64 frame_start = SDL_GetPerformanceCounter();
        delta_time = (float)(frame_start - perf_counter) / (float)freq;
        perf_counter = frame_start;

        mouse_buttons_prev = mouse_buttons;
        memcpy(keys_prev, keys, sizeof(keys));

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN)
                if (!event.key.repeat)
                    keys[event.key.scancode] = true;

            if (event.type == SDL_EVENT_KEY_UP)
                keys[event.key.scancode] = false;

            if (event.type == SDL_EVENT_MOUSE_WHEEL)
            {
                float x = event.wheel.x;
                float y = event.wheel.y;

                if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                {
                    x = -x;
                    y = -y;
                }

                mouse_wheel.x = x;
                mouse_wheel.y = y;
            }
        }

        mouse_buttons = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        mouse_delta.x = mouse_pos.x - mouse_pos_prev.x;
        mouse_delta.y = mouse_pos.y - mouse_pos_prev.y;
        mouse_pos_prev.x = mouse_pos.x;
        mouse_pos_prev.y = mouse_pos.y;

        game_update();
        game_draw();
        SDL_RenderPresent(renderer);

        mouse_wheel = vec2_zero();

        acc_time += delta_time;
        frame_count++;

        if (acc_time >= 1.0f)
        {
            fps = frame_count;
            frame_count = 0;
            acc_time -= 1.0f;
        }

        if (target_fps > 0)
        {
            float target_time = 1.0f / (float)target_fps;
            Uint64 now = SDL_GetPerformanceCounter();
            float elapsed = (float)(now - frame_start) / (float)freq;
            float remaining = target_time - elapsed;

            if (remaining > 0.0f)
                SDL_DelayPrecise((Uint64)(remaining * (float)freq * 1e9f / (float)freq));
        }
    }

    SDL_UnloadObject(game);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// window
void set_window_title(const char *title)
{
    SDL_SetWindowTitle(window, title);
}

void set_screen_size(int w, int h)
{
    SDL_SetWindowSize(window, w, h);
}

vec2 get_screen_size(void)
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return (vec2){w, h};
}

int get_screen_width(void)
{
    return get_screen_size().x;
}

int get_screen_height(void)
{
    return get_screen_size().y;
}

void toggle_fullscreen(void)
{
    fullscreen = !fullscreen;
    SDL_SetWindowFullscreen(window, fullscreen);
}

void maximize_window(void)
{
    SDL_MaximizeWindow(window);
}

void minimize_window(void)
{
    SDL_MinimizeWindow(window);
}

void close_window(void)
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

vec2 get_screen_center(void)
{
    vec2 center;
    center.x = get_screen_size().x / 2;
    center.y = get_screen_size().y / 2;
    return center;
}

bool is_fullscreen(void)
{
    return fullscreen;
}

// mouse
int get_mouse_x(void)
{
    return mouse_pos.x;
}

int get_mouse_y(void)
{
    return mouse_pos.y;
}

vec2 get_mouse_pos(void)
{
    return mouse_pos;
}

vec2 get_mouse_delta(void)
{
    return mouse_delta;
}

vec2 get_mouse_wheel(void)
{
    return mouse_wheel;
}

// input
bool is_key_down(keyboard_key key)
{
    return keys[key];
}

bool is_key_pressed(keyboard_key key)
{
    return keys[key] && !keys_prev[key];
}

bool is_key_released(keyboard_key key)
{
    return !keys[key] && keys_prev[key];
}

bool is_mouse_button_down(mouse_button button)
{
    return (mouse_buttons & SDL_BUTTON_MASK(button)) != 0;
}

bool is_mouse_button_pressed(mouse_button button)
{
    Uint32 mask = SDL_BUTTON_MASK(button);
    return (mouse_buttons & mask) && !(mouse_buttons_prev & mask);
}

bool is_mouse_button_released(mouse_button button)
{
    Uint32 mask = SDL_BUTTON_MASK(button);
    return !(mouse_buttons & mask) && (mouse_buttons_prev & mask);
}

// frames
void set_target_fps(int fps)
{
    target_fps = fps;
}

void draw_fps(font f, float x, float y, float size)
{
    char str[32];
    snprintf(str, sizeof(str), "%d", get_fps());
    draw_text(f, str, x, y, size, GREEN);
}

int get_fps(void)
{
    return fps;
}

float get_delta_time(void)
{
    return delta_time;
}

//camera
cam2d cam2d_new(void)
{
    return (cam2d){{get_screen_size().x / 2.0f, get_screen_size().y / 2.0f}, {0, 0}, 1.0f};
}

void start2d(cam2d cam)
{
    current_cam = cam;
}

void end2d(void)
{
    current_cam = (cam2d){get_screen_center(), {0, 0}, 1.0f};
}

vec2 get_cam_pos(cam2d cam)
{
    return (vec2){cam.target.x - cam.offset.x, cam.target.y - cam.offset.y};
}

vec2 get_screen_to_world(vec2 pos, cam2d cam)
{
    vec2 cam_pos = get_cam_pos(cam);
    vec2 world = {(pos.x - get_screen_size().x / 2.0f) / cam.zoom + cam_pos.x, 
         (pos.y - get_screen_size().y / 2.0f) / cam.zoom + cam_pos.y};
    return world;
}

vec2 get_world_to_screen(vec2 world, cam2d cam)
{
    vec2 cam_pos = get_cam_pos(cam);
    vec2 screen = {(world.x - cam_pos.x) * cam.zoom + get_screen_size().x / 2.0f, 
         (world.y - cam_pos.y) * cam.zoom + get_screen_size().y / 2.0f};
    return screen;
}

// drawing
void clear_background(color col)
{
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderClear(renderer);
}

void draw_pixel(float x, float y, color col)
{
    draw_rect(x, y, 1, 1, col);
}

void draw_rect(float x, float y, float w, float h, color col)
{
    draw_rect_mode(x, y, w, h, FILL, col);
}

void draw_rect_mode(float x, float y, float w, float h, draw_mode mode, color col)
{
    vec2 p = get_world_to_screen((vec2){x, y}, current_cam);

    SDL_FRect rect = {p.x, p.y, w * current_cam.zoom, h * current_cam.zoom};
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);

    if (mode == FILL)
        SDL_RenderFillRect(renderer, &rect);
    if (mode == LINE)
        SDL_RenderRect(renderer, &rect);
}

void draw_circle(float x, float y, float r, int segments, color col)
{
    SDL_Vertex *verts = malloc(sizeof(SDL_Vertex) * (segments + 2));
    int *inds = malloc(sizeof(int) * segments * 3);

    vec2 p = get_world_to_screen((vec2){x, y}, current_cam);
    float sr = r * current_cam.zoom;
    col = color_255_to_1(col);

    verts[0] = (SDL_Vertex){{p.x, p.y}, (SDL_FColor){col.r, col.g, col.b, col.a}, {0.0f, 0.0f}};

    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * PI;
        float dx = p.x + sr * cosf(angle);
        float dy = p.y + sr * sinf(angle);

        verts[i + 1] = (SDL_Vertex){{dx, dy}, (SDL_FColor){col.r, col.g, col.b, col.a},
            {0.0f, 0.0f}};
    }

    for (int i = 0; i < segments; i++)
    {
        inds[i * 3 + 0] = 0;
        inds[i * 3 + 1] = i + 1;
        inds[i * 3 + 2] = i + 2;
    }

    SDL_RenderGeometry(renderer, NULL, verts, segments + 2, inds, segments * 3);

    free(verts);
    free(inds);
}

void draw_triangle(vec2 v1, vec2 v2, vec2 v3, color col)
{
    draw_triangle_mode(v1, v2, v3, FILL, col);
}

void draw_triangle_mode(vec2 v1, vec2 v2, vec2 v3, draw_mode mode, color col)
{
    vec2 p1 = get_world_to_screen(v1, current_cam);
    vec2 p2 = get_world_to_screen(v2, current_cam);
    vec2 p3 = get_world_to_screen(v3, current_cam);

    if (mode == FILL)
    {
        SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
        SDL_Vertex verts[3];
        color c = color_255_to_1(col);

        verts[0].position.x = p1.x;
        verts[0].position.y = p1.y;
        verts[1].position.x = p2.x;
        verts[1].position.y = p2.y;
        verts[2].position.x = p3.x;
        verts[2].position.y = p3.y;

        verts[0].color.r = c.r;
        verts[0].color.g = c.g;
        verts[0].color.b = c.b;
        verts[0].color.a = c.a;
        verts[1].color.r = c.r;
        verts[1].color.g = c.g;
        verts[1].color.b = c.b;
        verts[1].color.a = c.a;
        verts[2].color.r = c.r;
        verts[2].color.g = c.g;
        verts[2].color.b = c.b;
        verts[2].color.a = c.a;

        SDL_RenderGeometry(renderer, NULL, verts, 3, NULL, 0);
    }

    else
    {
        draw_line(p1, p2, col);
        draw_line(p2, p3, col);
        draw_line(p3, p1, col);
    }
}

void draw_line(vec2 v1, vec2 v2, color col)
{
    vec2 p1 = get_world_to_screen(v1, current_cam);
    vec2 p2 = get_world_to_screen(v2, current_cam);

    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
}

void draw_texture(texture tex, int x, int y, color col)
{
    draw_texture_ex(tex, (vec2){x, y}, (vec2){tex.w, tex.h}, col);
}

void draw_texture_ex(texture tex, vec2 pos, vec2 size, color col)
{
    SDL_SetTextureColorMod((SDL_Texture *)tex.data, col.r, col.g, col.b);
    SDL_SetTextureAlphaMod((SDL_Texture *)tex.data, col.a);

    vec2 p = get_world_to_screen(pos, current_cam);

    SDL_FRect dst = {p.x, p.y, size.x * current_cam.zoom, size.y * current_cam.zoom};
    SDL_RenderTexture(renderer, (SDL_Texture *)tex.data, NULL, &dst);
}

void draw_texture_src(texture tex, rect src, rect dst, color col)
{
    SDL_SetTextureColorMod((SDL_Texture *)tex.data, col.r, col.g, col.b);
    SDL_SetTextureAlphaMod((SDL_Texture *)tex.data, col.a);

    vec2 d = get_world_to_screen((vec2){dst.x, dst.y}, current_cam);

    SDL_FRect sr = {src.x, src.y, src.w, src.h};
    SDL_FRect dr = {d.x, d.y, dst.w * current_cam.zoom, dst.h * current_cam.zoom};

    SDL_RenderTexture(renderer, (SDL_Texture *)tex.data, &sr, &dr);
}

void draw_text(font f, const char *text, float x, float y, float size, color col)
{
    if (!f.data)
        return;

    TTF_SetFontSize((TTF_Font *)f.data, size * current_cam.zoom);
    TTF_Text *t = TTF_CreateText(text_engine, (TTF_Font *)f.data, text, 0);

    vec2 p = get_world_to_screen((vec2){x, y}, current_cam);

    TTF_SetTextColor(t, col.r, col.g, col.b, col.a);
    TTF_DrawRendererText(t, p.x, p.y);

    TTF_DestroyText(t);
}

void draw_text_centered(font f, const char *text, float x, float y, float size, color col)
{
    vec2 s = get_text_size(f, text, size);
    draw_text(f, text, x - s.x / 2.0f, y - s.y / 2.0f, size, col);
}

texture create_render_texture(int w, int h)
{
    texture t;
    t.data = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureScaleMode(t.data, SDL_SCALEMODE_NEAREST);

    t.w = w;
    t.h = h;
    return t;
}

void start_texture_mode(texture tex)
{
    if (!tex.data)
        return;

    SDL_SetRenderTarget(renderer, (SDL_Texture *)tex.data);
}

void end_texture_mode(void)
{
    SDL_SetRenderTarget(renderer, NULL);
}

float get_text_width(font f, const char *text, float size)
{
    return get_text_size(f, text, size).x;
}

vec2 get_text_size(font f, const char *text, float size)
{
    int w, h;

    TTF_SetFontSize((TTF_Font *)f.data, size * current_cam.zoom);
    TTF_GetStringSize(f.data, text, 0, &w, &h);
    return (vec2){w, h};
}

// sound
sound load_sound(const char *path)
{
    sound snd;
    snd.data = NULL;

    MIX_Audio *audio = MIX_LoadAudio(mixer, path, true);
    
    if (!audio)
    {
        fprintf(stderr, SDL_GetError());
        return snd;
    }

    MIX_Track *track = MIX_CreateTrack(mixer);

    if (!track)
    {
        MIX_DestroyAudio(audio);
        return snd;
    }

    snd.data = track;
    MIX_SetTrackAudio(track, audio);
    
    return snd;
}

void play_sound(sound snd)
{
    if (!snd.data)
        return;

    MIX_PlayTrack((MIX_Track *)snd.data, 0);
}

void stop_sound(sound snd)
{
    if (!snd.data)
        return;

    MIX_StopTrack((MIX_Track *)snd.data, 0);
}

void set_sound_volume(sound snd, float volume)
{
    if (!snd.data)
        return;

    MIX_Track *track = (MIX_Track *)snd.data;
    MIX_SetTrackGain(track, volume);
}

int get_sound_volume(sound snd)
{
    return MIX_GetTrackGain((MIX_Track *)snd.data);
}

void set_master_volume(float volume)
{
    MIX_SetMixerGain(mixer, volume);
}

int get_master_volume(void)
{
    return MIX_GetMixerGain(mixer);
}

void set_sound_pan(sound snd, float pan)
{
    if (!snd.data)
        return;

    MIX_Track *track = (MIX_Track *)snd.data;
    
    pan = clamp(pan, -1.0f, 1.0f);
    
    MIX_StereoGains gains;
    gains.left = (1.0f - pan) * 0.5f;
    gains.right = (1.0f + pan) * 0.5f;
    
    MIX_SetTrackStereo(track, &gains);
}

bool is_sound_playing(sound snd)
{
    if (!snd.data)
        return false;

    MIX_Track *track = (MIX_Track *)snd.data;
    return MIX_TrackPlaying(track);
}

texture load_texture(const char *path)
{
    texture t;
    SDL_Texture *tex = IMG_LoadTexture(renderer, path);

    if (!tex)
    {
        fprintf(stderr, SDL_GetError());
        return t;
    }

    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    t.w = w;
    t.h = h;
    t.data = tex;

    return t;
}

vec2 texture_size(texture tex)
{
    return (vec2){tex.w, tex.h};
}

font load_font(const char *path)
{
    TTF_Font *fnt = TTF_OpenFont(path, 24);
    font f;
    f.data = NULL;
    
    if (!fnt)
    {
        fprintf(stderr, SDL_GetError());
        return f;
    }

    f.data = fnt;
    return f;
}

color color_255_to_1(color col)
{
    color c;
    c.r = col.r / 255.0f;
    c.g = col.g / 255.0f;
    c.b = col.b / 255.0f;
    c.a = col.a / 255.0f;
    return c;
}

color color_1_to_255(color col)
{
    color c;
    c.r = col.r * 255.0f;
    c.g = col.g * 255.0f;
    c.b = col.b * 255.0f;
    c.a = col.a * 255.0f;
    return c;
}


// collisions
bool check_collision_point_rect(vec2 p, rect r)
{
    return
        p.x <= r.x + r.w &&
        p.y <= r.y + r.h &&
        p.x >= r.x &&
        p.y >= r.y;
}

bool check_collision_point_circle(vec2 p, float cx, float cy, float cr)
{
    return vec2_dist(p, (vec2){cx, cy}) < cr;
}

bool check_collision_circle_rect(float cx, float cy, float cr, rect r)
{
    vec2 closest = {clamp(cx, r.x, r.x + r.w), clamp(cy, r.y, r.y + r.h)};
    float dist = vec2_dist((vec2){cx, cy}, closest);
    return dist <= cr;
}

bool check_collision_rects(rect r1, rect r2)
{
    return
        (r1.x <= r2.x + r2.w &&
        r1.y <= r2.y + r2.h) &&
        (r2.x <= r1.x + r1.w &&
        r2.y <= r1.y + r1.h);
}

bool check_collision_circles(vec2 pos1, float r1, vec2 pos2, float r2)
{
    return vec2_dist(pos1, pos2) <= r1 + r2;
}

// math
int get_random_value(int min, int max)
{
    return min + SDL_rand(max - min + 1);
}
                                        
float lerp(float start, float end, float amount)
{
    return (1.0f - amount) * start + amount * end;
}

float clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

vec2 vec2_zero(void)
{
    return (vec2){0, 0};
}

vec2 vec2_clamp(vec2 v, vec2 min, vec2 max)
{
    return (vec2){clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y)};
}

vec2 vec2_lerp(vec2 start, vec2 end, float amount)
{
    return (vec2){lerp(start.x, end.x, amount), lerp(start.y, end.y, amount)};
}

vec2 vec2_add(vec2 v1, vec2 v2)
{
    return (vec2){v1.x + v2.x, v1.y + v2.y};
}

vec2 vec2_sub(vec2 v1, vec2 v2)
{
    return (vec2){v1.x - v2.x, v1.y - v2.y};
}

vec2 vec2_mul(vec2 v1, vec2 v2)
{
    return (vec2){v1.x * v2.x, v1.y * v2.y};
}

vec2 vec2_div(vec2 v1, vec2 v2)
{
    return (vec2)
    {
        v2.x == 0 ? 0 : v1.x / v2.x,
        v2.y == 0 ? 0 : v1.y / v2.y
    };
}

vec2 vec2_scale(vec2 v, float value)
{
    return(vec2){v.x * value, v.y * value};
}

vec2 vec2_normalize(vec2 v)
{
    float length = vec2_length(v);
    if (length == 0.0f) 
        return (vec2){0, 0};
    return (vec2){v.x / length, v.y / length};
}

vec2 vec2_negate(vec2 v)
{
    return (vec2){-v.x, -v.y};
}

vec2 vec2_dir(vec2 v1, vec2 v2)
{
    return vec2_sub(v2, v1);
}

float vec2_dot(vec2 v1, vec2 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

float vec2_cross(vec2 v1, vec2 v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

float vec2_length(vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

float vec2_dist(vec2 v1, vec2 v2)
{
    return sqrtf((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y));
}

vec3 vec3_zero(void)
{
    return (vec3){0, 0, 0};
}

vec3 vec3_clamp(vec3 v, vec3 min, vec3 max)
{
    return (vec3){clamp(v.x, min.x, max.x), clamp(v.y, min.y, max.y), clamp(v.z, min.z, max.z)};
}

vec3 vec3_lerp(vec3 start, vec3 end, float amount)
{
    return (vec3){lerp(start.x, end.x, amount), lerp(start.y, end.y, amount),
        lerp(start.z, end.z, amount)};
}

vec3 vec3_add(vec3 v1, vec3 v2)
{
    return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3 vec3_sub(vec3 v1, vec3 v2)
{
    return (vec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec3 vec3_mul(vec3 v1, vec3 v2)
{
    return (vec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

vec3 vec3_div(vec3 v1, vec3 v2)
{
    return (vec3)
    {
        v2.x == 0 ? 0 : v1.x / v2.x,
        v2.y == 0 ? 0 : v1.y / v2.y,
        v2.z == 0 ? 0 : v1.z / v2.z
    };
}

vec3 vec3_scale(vec3 v, float value)
{
    return(vec3){v.x * value, v.y * value, v.z * value};
}

vec3 vec3_normalize(vec3 v)
{
    float length = vec3_length(v);
    if (length == 0.0f) 
        return (vec3){0, 0, 0};
    return (vec3){v.x / length, v.y / length, v.z / length};
}

vec3 vec3_negate(vec3 v)
{
    return (vec3){-v.x, -v.y, -v.z};
}

vec3 vec3_cross(vec3 v1, vec3 v2)
{
    return (vec3)
    {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

vec3 vec3_dir(vec3 v1, vec3 v2)
{
    return vec3_sub(v2, v1);
}

float vec3_dot(vec3 v1, vec3 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float vec3_length(vec3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec3_dist(vec3 v1, vec3 v2)
{
    return sqrtf((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y)
            + (v2.z - v1.z) * (v2.z - v1.z));
}

