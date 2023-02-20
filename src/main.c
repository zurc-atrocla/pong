#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

void
SDLErrorDie();

int
main(int argc, char *argv[])
{
    /* frames per second */
    const int FPS = 60;
    /* convert the FPS value to milliseconds, so you can use it in SDL_Delay() */
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    SDL_Window *window;

    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        SDLErrorDie();

    window = SDL_CreateWindow(
            "Pong",
            0,
            0,
            0,
            0,
            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (window == NULL)
        SDLErrorDie();

    /* variable definitions */
    bool gameRunning = true;
    SDL_Event event;
    SDL_Renderer *renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    int RENDERER_WIDTH, RENDERER_HEIGHT;
    SDL_GetRendererOutputSize(renderer, &RENDERER_WIDTH, &RENDERER_HEIGHT);

    SDL_Rect paddle1 = { .x = 20, .y = RENDERER_HEIGHT / 2 - 50, .w = 20, .h = 100 };
    SDL_Rect paddle2 = { .x = RENDERER_WIDTH - 40, .y = RENDERER_HEIGHT / 2 - 50, .w = 20, .h = 100 };

    int paddle1DY = 0;
    int paddle2DY = 0;

    int paddle1Score = 0;
    int paddle2Score = 0;

    SDL_Rect ball = { .x = RENDERER_WIDTH / 2 - 10, .y = RENDERER_HEIGHT / 2 - 10, .w = 20, .h = 20 };
    int ballDX = -6;
    /* can be changed as the ball gets hit */
    int ballDY = rand() % 20;

    TTF_Init();
    SDL_Rect paddle1ScoreRect = { .x = RENDERER_WIDTH / 2 - 120, .y = 0, .w = 100, .h = 100 };
    SDL_Rect paddle2ScoreRect = { .x = RENDERER_WIDTH / 2 + 30, .y = 0, .w = 100, .h = 100 };
    TTF_Font *Arcade = TTF_OpenFont("../assets/arcade_font.ttf", 24);
    SDL_Color white = { .r = 255, .g = 255, .b = 255, .a = 255 };

    /* main loop */
    while (gameRunning) {
        /* get the start time of the frame in ticks (milliseconds since prog. start) */
        frameStart = SDL_GetTicks();

        /* event loop */
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            /* is initiated from a window close from the user */
            case SDL_QUIT:
                gameRunning = false;
                break;

            /* handle keyboard events */
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {

                case SDLK_f:
                    paddle1DY = -10;
                    break;

                case SDLK_s:
                    paddle1DY = 10;
                    break;

                case SDLK_j:
                    paddle2DY = -10;
                    break;

                case SDLK_l:
                    paddle2DY = 10;
                    break;

                default:
                    break;

                }
                break;

            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_f || event.key.keysym.sym == SDLK_s)
                    paddle1DY = 0;
                else if (event.key.keysym.sym == SDLK_j || event.key.keysym.sym == SDLK_l)
                    paddle2DY = 0;
                break;

            default:
                break;

            }
        }
        
        /* update objects */
        paddle1.y += paddle1DY;
        paddle2.y += paddle2DY;

        /* do this last to prevent the paddle1 from bobbing */
        if (paddle1.y <= 10)
            paddle1.y = 10;
        else if (paddle1.y + paddle1.h >= RENDERER_HEIGHT - 10)
            paddle1.y = RENDERER_HEIGHT - 10 - paddle1.h;

        if (paddle2.y <= 10)
            paddle2.y = 10;
        else if (paddle2.y + paddle2.h >= RENDERER_HEIGHT - 10)
            paddle2.y = RENDERER_HEIGHT - 10 - paddle2.h;

        /* update the ball */
        /* check if ball is out of bounds on the y-axis and bounce it */
        if (ball.y < 0) {
            ball.y = 0;
            ballDY = -ballDY;
        } else if (ball.y + ball.h > RENDERER_HEIGHT) {
            ball.y = RENDERER_HEIGHT - ball.h;
            ballDY = -ballDY;
        }

        /* check if ball is out of bounds on x-axis (and is hitting the paddle) and bounce it */
        if (ball.x < 40 && ball.y >= paddle1.y && ball.y <= paddle1.y + paddle1.h) {
            ball.x = 40;
            ballDX = -ballDX;
            ballDY = rand() % 20;
        } else if (ball.x + ball.w > RENDERER_WIDTH - 40 && ball.y >= paddle2.y && ball.y <= paddle2.y + paddle2.h) {
            ball.x = RENDERER_WIDTH - 40 - ball.w; /* reset the x position of the ball */
            ballDX = -ballDX;
            ballDY = rand() % 20;
        }

        /* add to the players' score if the ball passed by even after the prev. statement */
        if (ball.x < 0) {
            paddle2Score += 1;
            ball.x = RENDERER_WIDTH / 2 - 10;
            ballDX = -ballDX;
            ballDY = rand() % 20;
        } else if (ball.x + ball.w > RENDERER_WIDTH) {
            paddle1Score += 1;
            ball.x = RENDERER_WIDTH / 2 - 10;
            ballDX = -ballDX;
            ballDY = rand() % 20;
        }

        /* update the position of the ball */
        ball.x += ballDX;
        ball.y += ballDY;

        /* render all objects on the canvas */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &paddle1);
        SDL_RenderFillRect(renderer, &paddle2);
        SDL_RenderFillRect(renderer, &ball);
        SDL_RenderDrawLine(renderer, RENDERER_WIDTH / 2, 0, RENDERER_WIDTH / 2, RENDERER_HEIGHT);

        /* render the score text */
        char paddle1ScoreText[3];
        sprintf(paddle1ScoreText, "%d", paddle1Score);
        SDL_Surface *surfacePaddle1ScoreLabel = TTF_RenderText_Solid(Arcade, paddle1ScoreText, white);
        SDL_Texture *texturePaddle1ScoreLabel = SDL_CreateTextureFromSurface(renderer, surfacePaddle1ScoreLabel);
        SDL_RenderCopy(renderer, texturePaddle1ScoreLabel, NULL, &paddle1ScoreRect);
        SDL_FreeSurface(surfacePaddle1ScoreLabel);
        SDL_DestroyTexture(texturePaddle1ScoreLabel);

        char paddle2ScoreText[3];
        sprintf(paddle2ScoreText, "%d", paddle2Score);
        SDL_Surface *surfacePaddle2ScoreLabel = TTF_RenderText_Solid(Arcade, paddle2ScoreText, white);
        SDL_Texture *texturePaddle2ScoreLabel = SDL_CreateTextureFromSurface(renderer, surfacePaddle2ScoreLabel);
        SDL_RenderCopy(renderer, texturePaddle2ScoreLabel, NULL, &paddle2ScoreRect);
        SDL_FreeSurface(surfacePaddle2ScoreLabel);
        SDL_DestroyTexture(texturePaddle2ScoreLabel);

        /* update the screen */
        SDL_RenderPresent(renderer);

        /* check for winners */
        if (paddle1Score == 10) {
            printf("Player 1 (left) Won the Game!\n");
            gameRunning = false;
        } else if (paddle2Score == 10) {
            printf("Player 2 (right) Won the Game!\n");
            gameRunning = false;
        }

        /* get the end time in ticks and find the time elapsed */
        frameTime = SDL_GetTicks() - frameStart;

        /* wait for an adjustable time (clamped to frameDelay or below) to
         * maintain 60 FPS */
        if (frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
    }

    TTF_CloseFont(Arcade);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void
SDLErrorDie()
{
    printf("%s\n", SDL_GetError());
    exit(EXIT_FAILURE);
}
