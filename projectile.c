#include "projectile.h"

#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Variables statiques partagées entre tous les projectiles
static SDL_Texture* sharedProjectileFrames[NUM_PROJECTILE_FRAMES];
static int projectileTextureWidth = 0;
static int projectileTextureHeight = 0;
static int currentProjectileFrame = 0;
static Uint32 lastProjectileFrameTime = 0;

// Chargement des textures partagées du projectile
int loadSharedProjectileFrames(SDL_Renderer* renderer) {
    if (sharedProjectileFrames[0] == NULL) {
        for (int i = 0; i < NUM_PROJECTILE_FRAMES; ++i) {
            char filename[256];
            sprintf(filename, "assets/projectile/laser_%02d.png", i);

            SDL_Surface* surface = IMG_Load(filename);
            if (surface == NULL) {
                printf("Erreur de chargement '%s': %s\n", filename, IMG_GetError());
                return 0;
            }
            sharedProjectileFrames[i] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            if (sharedProjectileFrames[i] == NULL) {
                printf("Erreur de creation texture '%s'\n", filename);
                return 0;
            }
        }
        SDL_QueryTexture(sharedProjectileFrames[0], NULL, NULL, &projectileTextureWidth, &projectileTextureHeight);
    }
    return 1;
}

// Libère les textures partagées
void destroySharedProjectileFrames() {
    for (int i = 0; i < NUM_PROJECTILE_FRAMES; ++i) {
        if (sharedProjectileFrames[i] != NULL) {
            SDL_DestroyTexture(sharedProjectileFrames[i]);
            sharedProjectileFrames[i] = NULL;
        }
    }
}

// Met à jour l’animation (changement de frame)
void projectile_update_animation() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastProjectileFrameTime + PROJECTILE_FRAME_DELAY) {
        currentProjectileFrame++;
        if (currentProjectileFrame >= NUM_PROJECTILE_FRAMES) {
            currentProjectileFrame = 0;
        }
        lastProjectileFrameTime = currentTime;
    }
}

// Crée un projectile
Projectile* projectile_create(SDL_Renderer* renderer, float x, float y) {
    if (sharedProjectileFrames[0] == NULL) {
        if (!loadSharedProjectileFrames(renderer)) return NULL;
    }

    Projectile* p = (Projectile*)malloc(sizeof(Projectile));
    if (p == NULL) {
        printf("Erreur d'allocation mémoire projectile\n");
        return NULL;
    }

    p->x = x;
    p->y = y - (projectileTextureHeight / 2.0f);
    p->width = projectileTextureWidth;
    p->height = projectileTextureHeight;
    p->active = 1;

    return p;
}

// Met à jour la position du projectile
int projectile_update(Projectile* p, int windowWidth, int windowHeight) {
    if (p == NULL || !p->active) return 0;

    p->x -= PROJECTILE_SPEED;

    if (p->x + p->width < 0) {
        p->active = 0;
        return 0;
    }
    return 1;
}

// Dessine le projectile à l’écran
void projectile_draw(SDL_Renderer* renderer, const Projectile* p) {
    if (p == NULL || !p->active) return;

    SDL_Rect destRect = { (int)p->x, (int)p->y, p->width, p->height };
    SDL_RenderCopyEx(renderer,
                     sharedProjectileFrames[currentProjectileFrame],
                     NULL,
                     &destRect,
                     -90.0,
                     NULL,
                     SDL_FLIP_NONE);
}

// Détruit un projectile
void projectile_destroy(Projectile* p) {
    if (p != NULL) free(p);
}
