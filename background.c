#include "background.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Crée et charge le fond d’écran animé
Background* background_create(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
    Background* bg = (Background*)malloc(sizeof(Background));
    if (bg == NULL) {
        printf("Erreur d'allocation memoire pour Background.\n");
        return NULL;
    }

    bg->frames = NULL;
    bg->numFrames = 0;
    bg->currentFrame = 0;
    bg->lastFrameTime = SDL_GetTicks();
    bg->frameWidth = 0;
    bg->frameHeight = 0;

    bg->frames = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * NUM_GIF_FRAMES);
    if (bg->frames == NULL) {
        printf("Erreur d'allocation memoire pour les frames.\n");
        free(bg);
        return NULL;
    }

    // Charge chaque frame
    for (int i = 0; i < NUM_GIF_FRAMES; ++i) {
        char filename[256];
        sprintf(filename, "assets/background/background_frame_%02d.png", i);

        SDL_Surface* frameSurface = IMG_Load(filename);
        if (frameSurface == NULL) {
            printf("Erreur chargement '%s': %s\n", filename, IMG_GetError());
            for (int j = 0; j < bg->numFrames; ++j) SDL_DestroyTexture(bg->frames[j]);
            free(bg->frames);
            free(bg);
            return NULL;
        }

        bg->frames[i] = SDL_CreateTextureFromSurface(renderer, frameSurface);
        SDL_FreeSurface(frameSurface);
        if (bg->frames[i] == NULL) {
            printf("Erreur texture '%s': %s\n", filename, SDL_GetError());
            for (int j = 0; j < bg->numFrames; ++j) SDL_DestroyTexture(bg->frames[j]);
            free(bg->frames);
            free(bg);
            return NULL;
        }

        bg->numFrames++;
    }

    if (bg->numFrames == 0) {
        printf("Aucune frame de background chargee.\n");
        free(bg->frames);
        free(bg);
        return NULL;
    }

    SDL_QueryTexture(bg->frames[0], NULL, NULL, &bg->frameWidth, &bg->frameHeight);
    printf("DEBUG: Fond d'ecran charge (%d frames, %dx%d).\n", bg->numFrames, bg->frameWidth, bg->frameHeight);

    return bg;
}

// Met à jour l’animation
void background_update(Background* bg) {
    if (bg == NULL || bg->frames == NULL || bg->numFrames == 0) return;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > bg->lastFrameTime + FRAME_DELAY) {
        bg->currentFrame = (bg->currentFrame + 1) % bg->numFrames;
        bg->lastFrameTime = currentTime;
    }
}

// Dessine le fond en "tiling"
void background_draw(SDL_Renderer* renderer, const Background* bg, int windowWidth, int windowHeight) {
    if (bg == NULL || bg->frames == NULL || bg->numFrames == 0) return;

    int numTilesX = (windowWidth + bg->frameWidth - 1) / bg->frameWidth;
    int numTilesY = (windowHeight + bg->frameHeight - 1) / bg->frameHeight;

    for (int y = 0; y < numTilesY; ++y) {
        for (int x = 0; x < numTilesX; ++x) {
            SDL_Rect destRect = {
                x * bg->frameWidth,
                y * bg->frameHeight,
                bg->frameWidth,
                bg->frameHeight
            };
            SDL_RenderCopy(renderer, bg->frames[bg->currentFrame], NULL, &destRect);
        }
    }
}

// Détruit le fond et libère la mémoire
void background_destroy(Background* bg) {
    if (bg != NULL) {
        if (bg->frames != NULL) {
            for (int i = 0; i < bg->numFrames; ++i) {
                if (bg->frames[i] != NULL) SDL_DestroyTexture(bg->frames[i]);
            }
            free(bg->frames);
        }
        free(bg);
    }
}
