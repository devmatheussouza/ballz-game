#include <allegro5/allegro5.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "Bolas.h"
#include "libScore.h"

#define RES_WIDTH 800
#define RES_HEIGHT 960
#define FPS 60
#define KEY_SEEN 1
#define KEY_RELEASED 2
#define RAIO 8
#define INCREMENT_HEIGHT 70
#define HEIGHT_LANCAMENTO 800

// Estados
enum STATES { MENU = 1, PLAYING, GAMEOVER, SCORES, PAUSE };

// Prototipos das funcoes
void must_init(bool test, const char *description);

// Inicio MAIN
int main() {
    srand(time(NULL));

    bool redraw = true, fimDoGame = false;
    bool tocandoMusicaMenu = true, clickDesativarMusicaMenu = false;
    bool atirouBola = false, clickPlay = false, descerBloco = false, atualizarScore = true, arqExiste;
    bool atualizacaoScoreGameOver = true;
    double mouseX, mouseY;
    int estadoAtual = MENU, scoreAtual = 0;
    int qntBolasAdicionadas = 0, qntBolasMortas = 0;
    int qntScores = 0, limite = 0;
    Score *scores;
    arqExiste = verificaExistenciaArquivo();

    // Allegro variables
    must_init(al_init(), "Allegro");
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    ALLEGRO_DISPLAY *display = al_create_display(RES_WIDTH, RES_HEIGHT);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_FONT *font28 = NULL, *font32 = NULL, *font40 = NULL;
    ALLEGRO_FONT *font60 = NULL, *font80 = NULL, *font140 = NULL;
    ALLEGRO_BITMAP *audioLigado = NULL;
    ALLEGRO_BITMAP *audioDesligado = NULL;
    ALLEGRO_BITMAP *score = NULL;

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
    audioLigado = al_load_bitmap("./images/audioLigado.jpg");
    audioDesligado = al_load_bitmap("./images/audioDesligado.jpg");
    score = al_load_bitmap("./images/score.jpg");
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
    must_init(al_reserve_samples(1), "reserve samples");

    ALLEGRO_SAMPLE *menuSong = al_load_sample("./audio/menuSong.wav");
    ALLEGRO_SAMPLE_INSTANCE *instMenuSong = al_create_sample_instance(menuSong);
    al_attach_sample_instance_to_mixer(instMenuSong, al_get_default_mixer());
    must_init(menuSong, "MenuSong");

    ListaBolas *listaBolas = criaListaBolas();
    insereFinalDaListaBolas(listaBolas);

    ListaBlocos *listaBlocos = criaListaBlocos();
    preencheLinhaBlocos(listaBlocos, &scoreAtual);

    ListaBolas *listaBolasMira = criaListaBolas();
    for (int i = 0; i < 10; i++) insereFinalDaListaBolas(listaBolasMira);

    if (arqExiste) {
        scores = leituraScores(&qntScores);
        if (qntScores < 10)
            limite = qntScores;
        else
            limite = 10;
    }

    // Register event source;
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    // Start timer;
    al_start_timer(timer);
    al_set_sample_instance_playmode(instMenuSong, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(instMenuSong, 0.8);
    al_play_sample_instance(instMenuSong);

    while (!fimDoGame) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        switch (ev.type) {
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
                if ((ev.mouse.button & 1) && (estadoAtual == PLAYING) && (atirouBola == false) && (clickPlay == true)) {
                    atirouBola = true;
                    clickPlay = false;
                    atiraBolas(listaBolas, mouseX, mouseY);
                    descerBloco = true;
                }

                if (((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 450) && (mouseY <= 520) &&
                     (estadoAtual == MENU)) ||
                    ((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 460) && (mouseY <= 530) &&
                     (estadoAtual == GAMEOVER)) ||
                    ((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 470) && (mouseY <= 540) &&
                     (estadoAtual == PAUSE))) {
                    if (tocandoMusicaMenu == true) {
                        tocandoMusicaMenu = false;
                        al_stop_sample_instance(instMenuSong);
                    }

                    qntBolasAdicionadas = 0;
                    qntBolasMortas = 0;
                    scoreAtual = 0;
                    if (listaBolas != NULL) {
                        liberaListaBolas(listaBolas);
                        listaBolas = criaListaBolas();
                        insereFinalDaListaBolas(listaBolas);
                    }

                    if (listaBlocos != NULL) {
                        liberaListaBlocos(listaBlocos);
                        listaBlocos = criaListaBlocos();
                        preencheLinhaBlocos(listaBlocos, &scoreAtual);
                    }
                    descerBloco = false;
                    atirouBola = false;
                    clickPlay = true;
                    estadoAtual = PLAYING;
                }

                if ((ev.mouse.button & 1) && (mouseX >= 470) && (mouseX <= 570) && (mouseY >= 640) && (mouseY <= 750) && (estadoAtual == MENU))
                    estadoAtual = SCORES;

                if ((ev.mouse.button & 1) && (mouseX >= 230) && (mouseX <= 330) && (mouseY >= 640) && (mouseY <= 750) && (estadoAtual == MENU) &&
                    (tocandoMusicaMenu == false))
                    clickDesativarMusicaMenu = false;

                if ((ev.mouse.button & 1) && (mouseX >= 230) && (mouseX <= 330) && (mouseY >= 640) && (mouseY <= 750) && (estadoAtual == MENU) &&
                    (tocandoMusicaMenu == true))
                    clickDesativarMusicaMenu = true;

                if ((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 570) && (mouseY <= 640) &&
                    (estadoAtual == GAMEOVER))
                    estadoAtual = MENU;

                if ((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 360) && (mouseY <= 430) && (estadoAtual == PAUSE))
                    estadoAtual = PLAYING;

                if ((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 580) && (mouseY <= 650) && (estadoAtual == PAUSE))
                    estadoAtual = MENU;

                if ((ev.mouse.button & 1) && (mouseX >= 255) && (mouseX <= 545) && (mouseY >= 780) && (mouseY <= 840) && (estadoAtual == SCORES))
                    estadoAtual = MENU;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                fimDoGame = true;
                break;

            case ALLEGRO_EVENT_TIMER:
                if (!fimDoGame) {
                    switch (estadoAtual) {
                        case MENU:
                            if ((clickDesativarMusicaMenu == false) && (tocandoMusicaMenu == false)) {
                                tocandoMusicaMenu = true;
                                al_play_sample_instance(instMenuSong);
                            }

                            if ((clickDesativarMusicaMenu == true) && (tocandoMusicaMenu == true)) {
                                tocandoMusicaMenu = false;
                                al_stop_sample_instance(instMenuSong);
                            }

                            if (key[ALLEGRO_KEY_Q]) fimDoGame = true;

                            if (key[ALLEGRO_KEY_ENTER]) {
                                qntBolasAdicionadas = 0;
                                qntBolasMortas = 0;
                                scoreAtual = 0;
                                if (listaBolas != NULL) {
                                    liberaListaBolas(listaBolas);
                                    listaBolas = criaListaBolas();
                                    insereFinalDaListaBolas(listaBolas);
                                }

                                if (listaBlocos != NULL) {
                                    liberaListaBlocos(listaBlocos);
                                    listaBlocos = criaListaBlocos();
                                    preencheLinhaBlocos(listaBlocos, &scoreAtual);
                                }
                                descerBloco = false;
                                atirouBola = false;
                                clickPlay = true;
                                estadoAtual = PLAYING;
                            }
                            break;

                        case PLAYING:
                            updateBlocos(listaBlocos, &descerBloco, &atirouBola, &scoreAtual, &estadoAtual);
                            updateBolas(listaBolas);
                            colisaoBolas(listaBolas, &atirouBola, listaBlocos, &qntBolasAdicionadas, &qntBolasMortas);

                            if (atualizacaoScoreGameOver == false) atualizacaoScoreGameOver = true;
                            if (key[ALLEGRO_KEY_ESCAPE]) estadoAtual = PAUSE;
                            break;

                        case PAUSE:
                            if (key[ALLEGRO_KEY_ENTER]) {
                                estadoAtual = PLAYING;
                                clickPlay = true;
                            }
                            break;

                        case GAMEOVER:
                            arqExiste = verificaExistenciaArquivo();
                            atualizarScore = true;
                            if (atualizacaoScoreGameOver == true) {
                                if (arqExiste) {
                                    scores = leituraScores(&qntScores);
                                    if (qntScores < 10)
                                        limite = qntScores;
                                    else
                                        limite = 10;
                                }
                                atualizacaoScoreGameOver = false;
                            }
                            if (key[ALLEGRO_KEY_ENTER]) fimDoGame = true;
                            break;

                        case SCORES:
                            arqExiste = verificaExistenciaArquivo();
                            if (atualizarScore == true) {
                                if (arqExiste) {
                                    scores = leituraScores(&qntScores);
                                    if (qntScores < 10)
                                        limite = qntScores;
                                    else
                                        limite = 10;
                                }
                                atualizarScore = false;
                            }
                            if (key[ALLEGRO_KEY_ESCAPE]) estadoAtual = MENU;
                            break;
                    }
                }

                for (int i = 0; i < ALLEGRO_KEY_MAX; i++) key[i] &= KEY_SEEN;

                redraw = true;
                break;
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            switch (estadoAtual) {
                case MENU:
                    // al_draw_textf(font32, al_map_rgb(255, 255, 255), 20, 20, 0, "Pos x: %.2f Pos y: %.2f", mouseX, mouseY);
                    al_draw_text(font140, al_map_rgba(235, 34, 95, 255), 272, 150, 0, "B");
                    al_draw_text(font140, al_map_rgba(249, 181, 49, 255), 343, 149, 0, "a");
                    al_draw_text(font140, al_map_rgba(22, 114, 190, 255), 414, 151, 0, "l");
                    al_draw_text(font140, al_map_rgba(0, 164, 149, 255), 432, 151, 0, "l");
                    al_draw_text(font140, al_map_rgba(128, 199, 67, 255), 453, 150, 0, "z");
                    al_draw_filled_rounded_rectangle(250, 450, 550, 520, 35, 35, al_map_rgba(234, 35, 95, 255));
                    al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 468, ALLEGRO_ALIGN_CENTRE, "PLAY");
                    al_draw_bitmap(score, (RES_WIDTH / 2) - 10, RES_HEIGHT - 400, 0);
                    if (clickDesativarMusicaMenu == true) {
                        al_draw_bitmap(audioDesligado, (RES_WIDTH / 2) - 250, RES_HEIGHT - 400, 0);
                    } else {
                        al_draw_bitmap(audioLigado, (RES_WIDTH / 2) - 250, RES_HEIGHT - 400, 0);
                    }
                    break;

                case PLAYING:
                    al_draw_filled_rectangle(0, 160, RES_WIDTH, 800, al_map_rgb(0, 0, 0));
                    al_draw_text(font28, al_map_rgb(255, 255, 255), 10, 15, 0, "BEST");
                    if (!arqExiste)
                        al_draw_text(font40, al_map_rgb(255, 255, 255), 20, 45, 0, "0");
                    else
                        al_draw_textf(font40, al_map_rgb(255, 255, 255), 18, 45, 0, "%d", scores[0].score);
                    al_draw_textf(font80, al_map_rgb(255, 255, 255), RES_WIDTH / 2, 15, ALLEGRO_ALIGN_CENTRE, "%d", scoreAtual);
                    drawBlocos(listaBlocos, font28);
                    if (atirouBola == true) {
                        drawBolas(listaBolas);
                    } else {
                        clickPlay = true;
                        drawMiraBolas(listaBolasMira, mouseX, mouseY, listaBolas->ponteiroInicio->bola.posX);
                        al_draw_filled_circle(listaBolas->ponteiroInicio->bola.posX, HEIGHT_LANCAMENTO - RAIO, 2 * RAIO, al_map_rgb(0, 0, 0));
                        al_draw_filled_circle(listaBolas->ponteiroInicio->bola.posX, HEIGHT_LANCAMENTO - RAIO, RAIO, al_map_rgb(255, 0, 50));
                        al_draw_filled_rectangle(listaBolas->ponteiroInicio->bola.posX - 2 * RAIO, HEIGHT_LANCAMENTO,
                                                 listaBolas->ponteiroInicio->bola.posX + 2 * RAIO, HEIGHT_LANCAMENTO + RAIO,
                                                 al_map_rgb(31, 31, 31));
                        if (abs(listaBolas->ponteiroInicio->bola.posX - RES_WIDTH) < 50) {
                            al_draw_textf(font28, al_map_rgb(255, 255, 255), listaBolas->ponteiroInicio->bola.posX - 40,
                                          HEIGHT_LANCAMENTO - 3 * RAIO, 0, "x%d", tamanhoDaListaBolas(listaBolas));
                        } else {
                            al_draw_textf(font28, al_map_rgb(255, 255, 255), listaBolas->ponteiroInicio->bola.posX + 20,
                                          HEIGHT_LANCAMENTO - 3 * RAIO, 0, "x%d", tamanhoDaListaBolas(listaBolas));
                        }
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
                    al_draw_textf(font140, al_map_rgb(255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2 - 320, ALLEGRO_ALIGN_CENTRE, "%d",
                                  scoreAtual);
                    if (arqExiste)
                        al_draw_textf(font80, al_map_rgb(255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2 - 120, ALLEGRO_ALIGN_CENTRE, "BEST %d",
                                      scores[0].score);
                    else
                        al_draw_text(font80, al_map_rgb(255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2 - 120, ALLEGRO_ALIGN_CENTRE, "BEST 0");

                    al_draw_filled_rounded_rectangle(250, 460, 550, 530, 35, 35, al_map_rgba(234, 35, 95, 255));
                    al_draw_filled_rounded_rectangle(250, 570, 550, 640, 35, 35, al_map_rgba(0, 164, 149, 255));
                    al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 478, ALLEGRO_ALIGN_CENTRE, "REPLAY");
                    al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 588, ALLEGRO_ALIGN_CENTRE, "MAIN MENU");
                    break;

                case SCORES:
                    al_draw_text(font80, al_map_rgba(249, 151, 49, 255), RES_WIDTH / 2, 20, ALLEGRO_ALIGN_CENTRE, "TOP SCORES");
                    al_draw_text(font60, al_map_rgba(255, 255, 255, 255), 150, 150, ALLEGRO_ALIGN_CENTRE, "POS.");
                    al_draw_text(font60, al_map_rgba(255, 255, 255, 255), 400, 150, ALLEGRO_ALIGN_CENTRE, "SCORE");
                    al_draw_text(font60, al_map_rgba(255, 255, 255, 255), 650, 150, ALLEGRO_ALIGN_CENTRE, "DATE");
                    if (!arqExiste) {
                        al_draw_text(font60, al_map_rgba(255, 255, 255, 255), RES_WIDTH / 2, RES_HEIGHT / 2, ALLEGRO_ALIGN_CENTRE,
                                     "NO SCORES YET");
                    } else {
                        for (int i = 0; i < limite; i++) {
                            al_draw_textf(font40, al_map_rgba(255, 255, 255, 255), 150, 220 + 50 * i, ALLEGRO_ALIGN_CENTRE, "%d°", i + 1);
                            al_draw_textf(font40, al_map_rgba(255, 255, 255, 255), 400, 220 + 50 * i, ALLEGRO_ALIGN_CENTRE, "%d",
                                          scores[i].score);
                            al_draw_textf(font40, al_map_rgba(255, 255, 255, 255), 650, 220 + 50 * i, ALLEGRO_ALIGN_CENTRE, "%02d/%02d/%d",
                                          scores[i].dia, scores[i].mes, scores[i].ano);
                        }
                    }
                    al_draw_filled_rounded_rectangle(250, 780, 550, 850, 35, 35, al_map_rgba(0, 164, 149, 255));
                    al_draw_text(font40, al_map_rgba(255, 255, 255, 255), 400, 798, ALLEGRO_ALIGN_CENTRE, "MAIN MENU");
                    break;
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(31, 31, 31));
        }
    }

    if (listaBolas != NULL) liberaListaBolas(listaBolas);

    if (listaBlocos != NULL) liberaListaBlocos(listaBlocos);

    liberaListaBolas(listaBolasMira);
    free(scores);

    al_destroy_bitmap(audioLigado);
    al_destroy_bitmap(audioDesligado);
    al_destroy_bitmap(score);
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

void must_init(bool test, const char *description) {
    if (test) return;
    printf("Couldn't initialize %s\n", description);
    exit(1);
}