#include "enemy.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h> // Pour malloc, free, et rand()

// Texture partagée pour tous les ennemis (pour la performance)
static SDL_Texture* sharedEnemyTexture = NULL;
static int enemyTextureWidth = 0;
static int enemyTextureHeight = 0;

#define ENEMY_SCALE_FACTOR 2.5f

// Charge la texture partagée
int loadSharedEnemyTexture(SDL_Renderer* renderer) {
    if (sharedEnemyTexture == NULL) {
        printf("DEBUG: Tentative de chargement du fichier 'assets/enemy/enemy_ship.png'\n");
        SDL_Surface* surface = IMG_Load("assets/enemy/enemy_ship.png");
        if (surface == NULL) {
            printf("Erreur de chargement de l'image de l'ennemi : %s\n", IMG_GetError());
            return 0;
        }
        sharedEnemyTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (sharedEnemyTexture == NULL) {
            printf("Erreur de creation de la texture de l'ennemi\n");
            return 0;
        }
        SDL_QueryTexture(sharedEnemyTexture, NULL, NULL, &enemyTextureWidth, &enemyTextureHeight);
        printf("DEBUG: Texture ennemie chargee, taille: %dx%d\n", enemyTextureWidth, enemyTextureHeight);
    }
    return 1;
}

// Détruit la texture partagée
void destroySharedEnemyTexture() {
    if (sharedEnemyTexture != NULL) {
        SDL_DestroyTexture(sharedEnemyTexture);
        sharedEnemyTexture = NULL;
    }
}

// Crée un nouvel ennemi
Enemy* enemy_create(int windowHeight) {
    Enemy* e = (Enemy*)malloc(sizeof(Enemy));
    if (e == NULL) {
        printf("Erreur d'allocation memoire pour l'ennemi\n");
        return NULL;
    }

    e->width = (int)(enemyTextureWidth * ENEMY_SCALE_FACTOR);
    e->height = (int)(enemyTextureHeight * ENEMY_SCALE_FACTOR);

    e->x = -e->width;
    e->y = rand() % (windowHeight - e->height);
    e->active = 1;

    return e;
}

// Met à jour la position de l'ennemi
int enemy_update(Enemy* e, int windowWidth) {
    if (e == NULL || !e->active) return 0;

    e->x += ENEMY_SPEED;
    if (e->x > windowWidth) {
        e->active = 0;
        return 0;
    }
    return 1;
}

// Dessine l'ennemi
void enemy_draw(SDL_Renderer* renderer, const Enemy* e) {
    if (e == NULL || !e->active) return;

    SDL_Rect destRect = { (int)e->x, (int)e->y, e->width, e->height };
    SDL_RenderCopy(renderer, sharedEnemyTexture, NULL, &destRect);
}

// Détruit un ennemi
void enemy_destroy(Enemy* e) {
    if (e != NULL) {
        free(e);
    }
}