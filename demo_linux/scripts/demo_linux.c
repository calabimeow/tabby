#include "tabby.h"
#include <stdio.h>

#define PADDLE_W 10.0f
#define PADDLE_H 80.0f
#define PADDLE_SPEED 300.0f
#define BALL_SIZE 15.0f
#define BALL_SPEED 250.0f

static vec2 p1, p2, ball_pos, ball_vel;
static int points1, points2;
static font fnt;
static sound bounce_snd, loss_snd;

void reset_ball()
{
    ball_pos = (vec2){get_screen_center().x - BALL_SIZE / 2.0f, get_screen_center().y - BALL_SIZE / 2.0f};
    ball_vel = (vec2){((get_random_value(0, 1) * 2) - 1) * BALL_SPEED * ((get_random_value(0, 1) * 2) - 1),
        ((get_random_value(0, 1) * 2) - 1) * BALL_SPEED * ((get_random_value(0, 1) * 2) - 1)};
}

void tabby_init(void)
{
    set_window_title("pong");
    set_target_fps(60);
    fnt = load_font("assets/aansa.ttf");
    bounce_snd = load_sound("assets/bounce.wav");
    loss_snd = load_sound("assets/loss.wav");
    set_sound_volume(loss_snd, 0.5f);

    reset_ball();
    ball_vel = vec2_zero();

    p1.x = PADDLE_W * 3.0f;
    p2.x = get_screen_width() - PADDLE_W * 4.0f;
    p1.y = get_screen_center().y - PADDLE_H / 2.0f;
    p2.y = get_screen_center().y - PADDLE_H / 2.0f;
}

void tabby_update(void)
{
    ball_pos = vec2_add(ball_pos, vec2_scale(ball_vel, get_delta_time()));

    if (check_collision_rects((rect){ball_pos.x, ball_pos.y, BALL_SIZE, BALL_SIZE}, (rect){p1.x, p1.y, PADDLE_W, PADDLE_H}) ||
            check_collision_rects((rect){ball_pos.x, ball_pos.y, BALL_SIZE, BALL_SIZE}, (rect){p2.x, p2.y, PADDLE_W, PADDLE_H}))
    {
        ball_vel.x = -ball_vel.x;
        play_sound(bounce_snd);
    }

    if (ball_pos.y <= 0.0f)
    {
        ball_vel.y = -ball_vel.y;
        play_sound(bounce_snd);
    }

    if (ball_pos.y >= get_screen_height() - BALL_SIZE / 2.0f)
    {
        ball_vel.y = -ball_vel.y;
        play_sound(bounce_snd);
    }

    static float reset_timer = 2.0f;
    static bool scored = true;

    if (!scored)
    {
        if (ball_pos.x + BALL_SIZE <= 0.0f)
        {
            scored = true;
            points2++;
            reset_timer = 2.0f;
            reset_ball();
            ball_vel = vec2_zero();
            play_sound(loss_snd);
        }
        else if (ball_pos.x >= get_screen_width())
        {
            scored = true;
            points1++;
            reset_timer = 2.0f;
            reset_ball();
            ball_vel = vec2_zero();
            play_sound(loss_snd);
        }
    }

    else
    {
        reset_timer -= get_delta_time();
        if (reset_timer <= 0.0f)
        {
            reset_ball();
            scored = false;
        }
    }

    if (is_key_down(KEY_W))
        p1.y -= PADDLE_SPEED * get_delta_time();
    if (is_key_down(KEY_S))
        p1.y += PADDLE_SPEED * get_delta_time();

    if (is_key_down(KEY_UP))
        p2.y -= PADDLE_SPEED * get_delta_time();
    if (is_key_down(KEY_DOWN))
        p2.y += PADDLE_SPEED * get_delta_time();

    p1.y = clamp(p1.y, 0.0f, get_screen_height() - PADDLE_H);
    p2.y = clamp(p2.y, 0.0f, get_screen_height() - PADDLE_H);

    if (is_key_pressed(KEY_R))
    {   
        reset_timer = 2.0f;
        scored = true;
        points1 = 0;
        points2 = 0;
        reset_ball();
        ball_vel = vec2_zero();
        p1.y = get_screen_center().y - PADDLE_H / 2.0f;
        p2.y = get_screen_center().y - PADDLE_H / 2.0f;
    }
}

void tabby_draw(void)
{
    clear_background((color){30, 30, 30, 255});
    draw_rect(p1.x, p1.y, PADDLE_W, PADDLE_H, WHITE);
    draw_rect(p2.x, p2.y, PADDLE_W, PADDLE_H, WHITE);
    draw_rect(ball_pos.x, ball_pos.y, BALL_SIZE, BALL_SIZE, WHITE);
    draw_rect(get_screen_center().x - 1.0f, 0.0f, 2.0f, get_screen_height(), WHITE);

    static char s1[32];
    snprintf(s1, sizeof(s1), "%d", points1);
    static char s2[32];
    snprintf(s2, sizeof(s1), "%d", points2);
    draw_text_centered(fnt, s1, get_screen_center().x - 60.0f, 80.0f, 70.0f, WHITE);
    draw_text_centered(fnt, s2, get_screen_center().x + 60.0f, 80.0f, 70.0f, WHITE);
}
