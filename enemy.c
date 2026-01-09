#include "enemy.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// --- RESSOURCES PARTAGÉES (Optimisation) ---
// On charge les images une seule fois pour tous les ennemis
static SDL_Texture* sharedEnemyTexture = NULL;
static SDL_Texture* explosionTextures[NUM_EXPLOSION_FRAMES]; // Tableau pour l'anim d'explosion

static int enemyTextureWidth = 0;
static int enemyTextureHeight = 0;

#define ENEMY_SCALE_FACTOR 2.5f
#define EXPLOSION_FRAME_DURATION 50 // Vitesse de l'explosion (ms)

// Charge le vaisseau ennemi ET les frames d'explosion
int loadSharedEnemyTexture(SDL_Renderer* renderer) {
    if (sharedEnemyTexture != NULL) return 1; // Déjà chargé

    // 1. Texture du Vaisseau
    SDL_Surface* surface = IMG_Load("assets/enemy/enemy_ship.png");
    if (!surface) {
        printf("Erreur chargement enemy: %s\n", IMG_GetError());
        return 0;
    }
    sharedEnemyTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_QueryTexture(sharedEnemyTexture, NULL, NULL, &enemyTextureWidth, &enemyTextureHeight);

    // 2. Textures de l'Explosion (explo_0.png, explo_1.png...)
    char filename[64];
    for (int i = 0; i < NUM_EXPLOSION_FRAMES; i++) {
        snprintf(filename, sizeof(filename), "assets/explosion/explo_%d.png", i);
        SDL_Surface* expSurf = IMG_Load(filename);
        if (!expSurf) {
            printf("Erreur chargement explosion %d: %s\n", i, IMG_GetError());
            return 0;
        }
        explosionTextures[i] = SDL_CreateTextureFromSurface(renderer, expSurf);
        SDL_FreeSurface(expSurf);
    }

    return 1;
}

// Nettoyage de toutes les textures à la fin du jeu
void destroySharedEnemyTexture() {
    if (sharedEnemyTexture != NULL) {
        SDL_DestroyTexture(sharedEnemyTexture);
        sharedEnemyTexture = NULL;
    }
    for (int i = 0; i < NUM_EXPLOSION_FRAMES; i++) {
        if (explosionTextures[i] != NULL) {
            SDL_DestroyTexture(explosionTextures[i]);
            explosionTextures[i] = NULL;
        }
    }
}

Enemy* enemy_create(int windowHeight) {
    Enemy* e = (Enemy*)malloc(sizeof(Enemy));
    if (e == NULL) return NULL;

    e->width = (int)(enemyTextureWidth * ENEMY_SCALE_FACTOR);
    e->height = (int)(enemyTextureHeight * ENEMY_SCALE_FACTOR);

    // Apparition à GAUCHE (hors écran, en négatif)
    e->x = -e->width;
    e->y = rand() % (windowHeight - e->height);
    e->active = 1;

    // État initial (vivant, pas d'explosion)
    e->isExploding = 0;
    e->explosionFrame = 0;
    e->lastFrameTime = 0;

    return e;
}

// Déclenche l'animation de mort
void enemy_start_explosion(Enemy* e) {
    if (!e->isExploding) {
        e->isExploding = 1;
        e->explosionFrame = 0;
        e->lastFrameTime = SDL_GetTicks();
    }
}

int enemy_update(Enemy* e, int windowWidth) {
    if (e == NULL || !e->active) return 0;

    // CAS 1 : Ennemi en train d'exploser (ne bouge plus, joue l'anim)
    if (e->isExploding) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > e->lastFrameTime + EXPLOSION_FRAME_DURATION) {
            e->explosionFrame++;
            e->lastFrameTime = currentTime;

            // Si l'animation est finie, l'ennemi disparaît pour de bon
            if (e->explosionFrame >= NUM_EXPLOSION_FRAMES) {
                e->active = 0;
                return 0;
            }
        }
        return 1; // Toujours actif (visuellement) tant que l'anim joue
    }

    // CAS 2 : Ennemi vivant -> Déplacement vers la DROITE
    e->x += ENEMY_SPEED;

    // Si sort de l'écran à DROITE -> Suppression
    if (e->x > windowWidth) {
        e->active = 0;
        return 0;
    }
    return 1;
}

void enemy_draw(SDL_Renderer* renderer, const Enemy* e) {
    if (e == NULL || !e->active) return;

    SDL_Rect destRect = { (int)e->x, (int)e->y, e->width, e->height };

    if (e->isExploding) {
        // Affiche la frame actuelle de l'explosion
        SDL_RenderCopy(renderer, explosionTextures[e->explosionFrame], NULL, &destRect);
    } else {
        // Affiche le vaisseau
        SDL_RenderCopy(renderer, sharedEnemyTexture, NULL, &destRect);
    }
}

void enemy_destroy(Enemy* e) {
    if (e != NULL) free(e);
}