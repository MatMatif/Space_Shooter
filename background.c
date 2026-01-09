#include "background.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Initialise le background, charge les images de l'animation
Background* background_create(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
    Background* bg = (Background*)malloc(sizeof(Background));
    if (bg == NULL) {
        printf("Erreur d'allocation memoire pour Background.\n");
        return NULL;
    }

    // Init des variables
    bg->frames = NULL;
    bg->numFrames = 0;
    bg->currentFrame = 0;
    bg->lastFrameTime = SDL_GetTicks();
    bg->frameWidth = 0;
    bg->frameHeight = 0;
    bg->scrollOffsetX = 0.0f;

    // Allocation du tableau de textures (pour le GIF animé)
    bg->frames = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * NUM_GIF_FRAMES);
    if (bg->frames == NULL) {
        printf("Erreur d'allocation memoire pour les frames.\n");
        free(bg);
        return NULL;
    }

    // Boucle de chargement des images (background_frame_00.png, 01, etc.)
    for (int i = 0; i < NUM_GIF_FRAMES; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "assets/background/background_frame_%02d.png", i); // snprintf est plus sûr

        SDL_Surface* frameSurface = IMG_Load(filename);
        if (frameSurface == NULL) {
            printf("Erreur chargement '%s': %s\n", filename, IMG_GetError());
            // Nettoyage d'urgence si une image plante
            for (int j = 0; j < bg->numFrames; ++j) SDL_DestroyTexture(bg->frames[j]);
            free(bg->frames);
            free(bg);
            return NULL;
        }

        bg->frames[i] = SDL_CreateTextureFromSurface(renderer, frameSurface);
        SDL_FreeSurface(frameSurface);

        if (bg->frames[i] == NULL) {
            // Gestion erreur conversion texture
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

    // On récupère la taille de la première image pour gérer le tuilage
    SDL_QueryTexture(bg->frames[0], NULL, NULL, &bg->frameWidth, &bg->frameHeight);
    printf("DEBUG: Fond d'ecran charge (%d frames, %dx%d).\n", bg->numFrames, bg->frameWidth, bg->frameHeight);

    return bg;
}

// Met à jour l'animation (GIF) et la position du scrolling
void background_update(Background* bg) {
    if (bg == NULL || bg->frames == NULL || bg->numFrames == 0) return;

    // 1. Animation du fond (changement d'image)
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > bg->lastFrameTime + FRAME_DELAY) {
        bg->currentFrame = (bg->currentFrame + 1) % bg->numFrames;
        bg->lastFrameTime = currentTime;
    }

    // 2. Calcul du Scrolling
    // On décrémente l'offset. Comme on soustrait l'offset au dessin :
    // offset devient négatif -> Position visuelle augmente -> Mouvement vers la DROITE
    // (Simule que le vaisseau avance vers la GAUCHE)
    bg->scrollOffsetX -= BACKGROUND_SCROLL_SPEED;

    // Reset pour boucle infinie sans saccade
    if (bg->scrollOffsetX <= -bg->frameWidth) {
        bg->scrollOffsetX += bg->frameWidth;
    }
}

// Dessine le fond en répétant l'image (Tiling) pour remplir l'écran
void background_draw(SDL_Renderer* renderer, const Background* bg, int windowWidth, int windowHeight) {
    if (bg == NULL || bg->frames == NULL || bg->numFrames == 0) return;

    // Combien de tuiles faut-il pour couvrir l'écran ?
    int numTilesX = (windowWidth + bg->frameWidth - 1) / bg->frameWidth;
    int numTilesY = (windowHeight + bg->frameHeight - 1) / bg->frameHeight;

    for (int y = 0; y < numTilesY; ++y) {
        // On commence à x = -1 pour gérer le bord gauche proprement lors du défilement
        for (int x = -1; x < numTilesX; ++x) {

            // Calcul de la position de la tuile
            int startX = (x * bg->frameWidth) - (int)bg->scrollOffsetX;

            SDL_Rect destRect = {
                startX,
                y * bg->frameHeight,
                bg->frameWidth,
                bg->frameHeight
            };

            // Dessin de la frame actuelle de l'animation
            SDL_RenderCopy(renderer, bg->frames[bg->currentFrame], NULL, &destRect);
        }
    }
}

// Libération propre de la mémoire
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