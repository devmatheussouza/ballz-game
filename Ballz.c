#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Bolas.h"
#include "Blocos.h"

#define RES_WIDTH 800
#define RES_HEIGHT 960
#define FPS 60.0
#define KEY_SEEN 1
#define KEY_RELEASED 2
#define RAIO 8
#define INCREMENT_HEIGHT 70

// Estados
enum STATES
{
    MENU = 1,
    PLAYING,
    GAMEOVER,
    SCORES,
    PAUSE
};

double tempoInicial = 0;

void iniciarTimer()
{
    tempoInicial = al_get_time();
}

double obterTempoTimer()
{
    return al_get_time() - tempoInicial;
}

// Prototipos das funcoes
void must_init(bool test, const char *description);

// Inicio MAIN
int main(int argc, char const *argv[])
{
    srand(time(NULL));

    bool redraw = true;
    bool fimDoGame = false;
    bool tocandoMusicaMenu = true;
    bool clickDesativarMusicaMenu = false;
    bool atirouBola = false;
    bool clickPlay = false;
    int estadoAtual = MENU;
    int scoreAtual = 0;
    double mouseX;
    double mouseY;
    int numeroDalinha = 0;

    int frame = 0;
    bool limitado = true;

    // Allegro variables
    must_init(al_init(), "Allegro");
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    ALLEGRO_DISPLAY *display = al_create_display(RES_WIDTH, RES_HEIGHT);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_FONT *font28 = NULL;
    ALLEGRO_FONT *font32 = NULL;
    ALLEGRO_FONT *font40 = NULL;
    ALLEGRO_FONT *font60 = NULL;
    ALLEGRO_FONT *font80 = NULL;
    ALLEGRO_FONT *font140 = NULL;
    ALLEGRO_BITMAP *nota_musical = NULL;

    // Inicialization
    must_init(display, "Display");
    must_init(event_queue, "Event queue");
    must_init(timer, "Timer");
    must_init(al_init_primitives_addon(), "Primitives Addon");
    must_init(al_init_font_addon(), "Font Addon");
    must_init(al_init_ttf_addon(), "TTF Addon");
    must_init(al_init_image_addon(), "Image");
    must_init(al_install_keyboard(), "Keyboard");
    must_init(al_install_mouse(), "Mouse");
    al_set_window_title(display, "BALLZ");
    nota_musical = al_load_bitmap("./images/nota-musical.png");
    al_convert_mask_to_alpha(nota_musical, al_map_rgb(255, 0, 255));
    font28 = al_load_font("./fonts/creHappiness.ttf", 28, 0);
    font32 = al_load_font("./fonts/creHappiness.ttf", 32, 0);
    font40 = al_load_font("./fonts/creHappiness.ttf", 40, 0);
    font60 = al_load_font("./fonts/creHappiness.ttf", 60, 0);
    font80 = al_load_font("./fonts/creHappiness.ttf", 80, 0);
    font140 = al_load_font("./fonts/creHappiness.ttf", 140, 0);
    must_init(font28, "Font 20");
    must_init(font32, "Font 32");
    must_init(font40, "Font 40");
    must_init(font60, "Font 60");
    must_init(font80, "Font 80");
    must_init(font140, "Font 140");
    must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    must_init(al_reserve_samples(16), "reserve samples");

    ALLEGRO_SAMPLE *menuSong = al_load_sample("./audio/menuSong.wav");
    ALLEGRO_SAMPLE_ID menuSongId;
    must_init(menuSong, "pianoLoop");

    ListaBolas *listaBolas = criaListaBolas();
    for (int i = 0; i < 5; i++)
        insereFinalDaListaBolas(listaBolas);

    ListaBlocos *listaBlocos = criaListaBlocos();
    preencheLinhaBlocos(listaBlocos, &numeroDalinha);

    // Register event source;
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    // Start timer;
    al_start_timer(timer);
    al_play_sample(menuSong, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &menuSongId);

    while (!fimDoGame)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        switch (ev.type)
        {
        case ALLEGRO_EVENT_KEY_DOWN:
            key[ev.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;

            break;

        case ALLEGRO_EVENT_KEY_UP:
            key[ev.keyboard.keycode] &= KEY_RELEASED;
            break;

        case ALLEGRO_EVENT_MOUSE_AXES:
            mouseX = ev.mouse.x;
            mouseY = ev.mouse.y;
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if ((ev.mouse.button & 1) && (estadoAtual == PLAYING) && (atirouBola == false) && (clickPlay == true))
            {
                atirouBola = true;
                clickPlay = false;
                atiraBolas(listaBolas, mouseX, mouseY);
            }

            if (((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 450) && (mouseY <= 520) && (estadoAtual == MENU)) ||
                ((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 460) && (mouseY <= 530) && (estadoAtual == GAMEOVER)) ||
                ((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 470) && (mouseY <= 540) && (estadoAtual == PAUSE)))
            {

                if (tocandoMusicaMenu == true)
                {
                    tocandoMusicaMenu = false;
                    al_stop_sample(&menuSongId);
                }

                if (listaBolas != NULL)
                {
                    liberaListaBolas(listaBolas);
                    ListaBolas *listaBolas = criaListaBolas();
                    for (int i = 0; i < 5; i++)
                        insereFinalDaListaBolas(listaBolas);

                    liberaListaBlocos(listaBlocos);
                    ListaBlocos *listaBlocos = criaListaBlocos();
                    numeroDalinha = 0;
                    preencheLinhaBlocos(listaBlocos, &numeroDalinha);

                    atirouBola = false;
                    estadoAtual = PLAYING;
                    clickPlay = true;
                    scoreAtual = 0;
                }
            }

            if ((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 560) && (mouseY <= 630) && (estadoAtual == MENU))
            {
                estadoAtual = SCORES;
            }

            if ((mouseX >= 340) && (mouseX <= 460) && (mouseY >= 700) && (mouseY <= 820) && (estadoAtual == MENU) && (tocandoMusicaMenu == false))
            {
                clickDesativarMusicaMenu = false;
            }

            if ((mouseX >= 340) && (mouseX <= 460) && (mouseY >= 700) && (mouseY <= 820) && (estadoAtual == MENU) && (tocandoMusicaMenu == true))
            {
                clickDesativarMusicaMenu = true;
            }

            if ((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 570) && (mouseY <= 640) && (estadoAtual == GAMEOVER))
            {
                estadoAtual = MENU;
            }

            if ((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 360) && (mouseY <= 430) && (estadoAtual == PAUSE))
            {
                estadoAtual = PLAYING;
            }

            if ((mouseX >= 255) && (mouseX <= 545) && (mouseY >= 580) && (mouseY <= 650) && (estadoAtual == PAUSE))
            {
                estadoAtual = MENU;
            }

            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            fimDoGame = true;
            break;

        case ALLEGRO_EVENT_TIMER:
            if (!fimDoGame)
            {
                switch (estadoAtual)
                {
                case MENU:
                    if ((clickDesativarMusicaMenu == false) && (tocandoMusicaMenu == false))
                    {
                        tocandoMusicaMenu = true;
                        al_play_sample(menuSong, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &menuSongId);
                    }

                    if ((clickDesativarMusicaMenu == true) && (tocandoMusicaMenu == true))
                    {
                        tocandoMusicaMenu = false;
                        al_stop_sample(&menuSongId);
                    }

                    if (key[ALLEGRO_KEY_Q])
                        fimDoGame = true;

                    if (key[ALLEGRO_KEY_ENTER])
                    {
                        estadoAtual = PLAYING;
                        if (listaBolas != NULL)
                        {
                            liberaListaBolas(listaBolas);
                            ListaBolas *listaBolas = criaListaBolas();
                            for (int i = 0; i < 5; i++)
                                insereFinalDaListaBolas(listaBolas);

                            liberaListaBlocos(listaBlocos);
                            ListaBlocos *listaBlocos = criaListaBlocos();
                            preencheLinhaBlocos(listaBlocos, &numeroDalinha);
                            numeroDalinha = 0;

                            atirouBola = false;
                            estadoAtual = PLAYING;
                            clickPlay = true;
                            scoreAtual = 0;
                        }
                    }
                    break;

                case PLAYING:
                    colisaoBolas(listaBolas, &atirouBola);
                    updateBolas(listaBolas);

                    if (key[ALLEGRO_KEY_ESCAPE])
                        estadoAtual = PAUSE;
                    break;

                case PAUSE:
                    if (key[ALLEGRO_KEY_ENTER])
                    {
                        estadoAtual = PLAYING;
                        clickPlay = true;
                    }
                    break;

                case GAMEOVER:
                    if (key[ALLEGRO_KEY_ENTER])
                        fimDoGame = true;
                    break;

                case SCORES:
                    if (key[ALLEGRO_KEY_ESCAPE])
                        estadoAtual = MENU;
                    break;
                }
            }

            for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;

            redraw = true;
            break;
        }

        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            switch (estadoAtual)
            {
            case MENU:
                al_draw_textf(font32, al_map_rgb(255, 255, 255), 20, 20, 0, "Pos x: %.2f Pos y: %.2f", mouseX, mouseY);
                al_draw_text(font140, al_map_rgba(235, 34, 95, 255), 272, 150, 0, "B");
                al_draw_text(font140, al_map_rgba(249, 181, 49, 255), 343, 149, 0, "a");
                al_draw_text(font140, al_map_rgba(22, 114, 190, 255), 414, 151, 0, "l");
                al_draw_text(font140, al_map_rgba(0, 164, 149, 255), 432, 151, 0, "l");
                al_draw_text(font140, al_map_rgba(128, 199, 67, 255), 453, 150, 0, "z");
                al_draw_filled_rounded_rectangle(250, 450, 550, 520, 35, 35, al_map_rgba(234, 35, 95, 255));
                al_draw_filled_rounded_rectangle(250, 560, 550, 630, 35, 35, al_map_rgba(22, 117, 187, 255));
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 468, ALLEGRO_ALIGN_CENTRE, "PLAY");
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 578, ALLEGRO_ALIGN_CENTRE, "SCORES");
                al_draw_circle(RES_WIDTH / 2, RES_HEIGHT - 200, 60, al_map_rgb(240, 0, 240), 2);
                al_draw_bitmap(nota_musical, (RES_WIDTH / 2) - 44, RES_HEIGHT - 240, 0);
                if (clickDesativarMusicaMenu == true)
                    al_draw_line(360, 718, 445, 802, al_map_rgb(255, 0, 0), 3);
                break;

            case PLAYING:
                al_draw_textf(font32, al_map_rgb(255, 255, 255), 10, 15, 0, "Score: %d", scoreAtual);
                drawBlocos(listaBlocos, font28);
                if (atirouBola == false)
                {
                    clickPlay = true;
                    al_draw_filled_circle(listaBolas->ponteiroInicio->bola.posX, listaBolas->ponteiroInicio->bola.posY - 2 * RAIO, RAIO, al_map_rgb(255, 0, 50));
                    al_draw_line(listaBolas->ponteiroInicio->bola.posX, listaBolas->ponteiroInicio->bola.posY - 3 * RAIO, mouseX, mouseY, al_map_rgb(255, 255, 255), 1);
                }
                else
                {
                    drawBolas(listaBolas);
                }
                break;

            case PAUSE:
                al_draw_text(font80, al_map_rgb(255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2 - 220, ALLEGRO_ALIGN_CENTRE, "PAUSE");
                al_draw_filled_rounded_rectangle(250, 360, 550, 430, 35, 35, al_map_rgba(234, 35, 95, 255));
                al_draw_filled_rounded_rectangle(250, 470, 550, 540, 35, 35, al_map_rgba(249, 151, 49, 255));
                al_draw_filled_rounded_rectangle(250, 580, 550, 650, 35, 35, al_map_rgba(0, 164, 149, 255));
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 378, ALLEGRO_ALIGN_CENTRE, "RESUME");
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 488, ALLEGRO_ALIGN_CENTRE, "REPLAY");
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 598, ALLEGRO_ALIGN_CENTRE, "MAIN MENU");
                break;

            case GAMEOVER:
                al_draw_textf(font80, al_map_rgb(249, 181, 49), RES_WIDTH / 2, RES_HEIGHT / 2 - 220, ALLEGRO_ALIGN_CENTRE, "%d", scoreAtual);
                al_draw_text(font60, al_map_rgb(255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2 - 150, ALLEGRO_ALIGN_CENTRE, "BEST 80");
                al_draw_filled_rounded_rectangle(250, 460, 550, 530, 35, 35, al_map_rgba(234, 35, 95, 255));
                al_draw_filled_rounded_rectangle(250, 570, 550, 640, 35, 35, al_map_rgba(22, 117, 187, 255));
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 478, ALLEGRO_ALIGN_CENTRE, "REPLAY");
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 588, ALLEGRO_ALIGN_CENTRE, "MAIN MENU");
                break;

            case SCORES:
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "TELA DE SCORES");
                al_draw_text(font40, al_map_rgba(255, 255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2 + 30, ALLEGRO_ALIGN_CENTRE, "PRESS ESC TO BACK TO MENU");
                break;
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            frame++;

            if (limitado && (obterTempoTimer() < 1.0 / FPS))
            {
                al_rest((1.0 / FPS) - obterTempoTimer());
            }
        }
    }

    if (listaBolas != NULL)
        liberaListaBolas(listaBolas);

    if (listaBlocos != NULL)
        liberaListaBlocos(listaBlocos);

    al_destroy_bitmap(nota_musical);
    al_destroy_sample(menuSong);
    al_destroy_font(font28);
    al_destroy_font(font32);
    al_destroy_font(font40);
    al_destroy_font(font60);
    al_destroy_font(font80);
    al_destroy_font(font140);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_display(display);

    return 0;
}
// Fim MAIN

void must_init(bool test, const char *description)
{
    if (test)
        return;
    printf("Couldn't initialize %s\n", description);
    exit(1);
}