#include "projectile.h"

#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Variables partagées (Textures & Animation)
static SDL_Texture* sharedProjectileFrames[NUM_PROJECTILE_FRAMES] = {NULL};
static int projectileTextureWidth = 0;
static int projectileTextureHeight = 0;
static int currentProjectileFrame = 0;
static Uint32 lastProjectileFrameTime = 0;

// Charge les textures (appelé une seule fois)
int loadSharedProjectileFrames(SDL_Renderer* renderer) {
    if (sharedProjectileFrames[0] != NULL) return 1; // Déjà chargé

    for (int i = 0; i < NUM_PROJECTILE_FRAMES; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "assets/projectile/laser_%02d.png", i);

        SDL_Surface* surface = IMG_Load(filename);
        if (surface == NULL) {
            printf("Erreur chargement %s: %s\n", filename, IMG_GetError());
            // Nettoyage en cas d'échec
            for (int j = 0; j < i; j++) {
                SDL_DestroyTexture(sharedProjectileFrames[j]);
                sharedProjectileFrames[j] = NULL;
            }
            return 0;
        }
        sharedProjectileFrames[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    // Récupère la taille pour les collisions
    SDL_QueryTexture(sharedProjectileFrames[0], NULL, NULL, &projectileTextureWidth, &projectileTextureHeight);
    return 1;
}

// Nettoyage fin de programme
void destroySharedProjectileFrames() {
    for (int i = 0; i < NUM_PROJECTILE_FRAMES; ++i) {
        if (sharedProjectileFrames[i] != NULL) {
            SDL_DestroyTexture(sharedProjectileFrames[i]);
            sharedProjectileFrames[i] = NULL;
        }
    }
}

// Gestion de l'animation globale
void projectile_update_animation() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastProjectileFrameTime + PROJECTILE_FRAME_DELAY) {
        currentProjectileFrame = (currentProjectileFrame + 1) % NUM_PROJECTILE_FRAMES;
        lastProjectileFrameTime = currentTime;
    }
}

Projectile* projectile_create(SDL_Renderer* renderer, float x, float y) {
    // Chargement automatique si nécessaire
    if (sharedProjectileFrames[0] == NULL) {
        if (!loadSharedProjectileFrames(renderer)) return NULL;
    }

    Projectile* p = (Projectile*)malloc(sizeof(Projectile));
    if (p == NULL) return NULL;

    p->x = x;
    p->y = y - (projectileTextureHeight / 2.0f); // Centré verticalement
    p->width = projectileTextureWidth;
    p->height = projectileTextureHeight;
    p->active = 1;

    return p;
}

int projectile_update(Projectile* p, int windowWidth, int windowHeight) {
    if (p == NULL || !p->active) return 0;

    p->x -= PROJECTILE_SPEED; // Déplacement vers la gauche

    // Suppression si sort de l'écran
    if (p->x + p->width < 0) {
        p->active = 0;
        return 0;
    }
    return 1;
}

void projectile_draw(SDL_Renderer* renderer, const Projectile* p) {
    if (p == NULL || !p->active) return;

    SDL_Rect destRect = { (int)p->x, (int)p->y, p->width, p->height };

    // Rotation -90° pour orienter vers la gauche
    SDL_RenderCopyEx(renderer,
                     sharedProjectileFrames[currentProjectileFrame],
                     NULL, &destRect, -90.0, NULL, SDL_FLIP_NONE);
}

void projectile_destroy(Projectile* p) {
    if (p != NULL) free(p);
}