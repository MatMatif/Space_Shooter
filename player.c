#include "player.h"
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Crée un joueur et charge sa texture
Player* player_create(SDL_Renderer* renderer, float initialX, float initialY, float scaleFactor) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (player == NULL) {
        printf("Erreur d'allocation memoire pour le joueur\n");
        return NULL;
    }

    printf("DEBUG: Tentative de chargement du fichier 'assets/player/player_ship.png'\n");
    SDL_Surface* surface = IMG_Load("assets/player/player_ship.png");
    if (surface == NULL) {
        printf("Erreur de chargement de l'image du joueur : %s\n", IMG_GetError());
        free(player);
        return NULL;
    }

    printf("DEBUG: L'image du joueur a été chargée avec succès.\n");
    player->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (player->texture == NULL) {
        printf("Erreur de creation de la texture du joueur : %s\n", SDL_GetError());
        free(player);
        return NULL;
    }

    int originalWidth, originalHeight;
    SDL_QueryTexture(player->texture, NULL, NULL, &originalWidth, &originalHeight);
    player->width = (int)(originalWidth * scaleFactor);
    player->height = (int)(originalHeight * scaleFactor);

    player->x = initialX - player->width;
    player->y = initialY - (player->height / 2);
    player->lastShotTime = 0;

    printf("Joueur cree avec succes (taille redimensionnee: %dx%d).\n", player->width, player->height);
    return player;
}

// Détruit la texture et libère la mémoire
void player_destroy(Player* player) {
    if (player != NULL) {
        SDL_DestroyTexture(player->texture);
        free(player);
    }
}

// Met à jour le joueur (déplacements + tirs)
void player_update(Player* player, int windowWidth, int windowHeight,
                   SDL_Renderer* renderer,
                   Projectile** projectiles, int* numProjectiles, int maxProjectiles) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    float dx = 0, dy = 0;

    if (state[SDL_SCANCODE_UP]) dy -= PLAYER_SPEED;
    if (state[SDL_SCANCODE_DOWN]) dy += PLAYER_SPEED;
    if (state[SDL_SCANCODE_LEFT]) dx -= PLAYER_SPEED;
    if (state[SDL_SCANCODE_RIGHT]) dx += PLAYER_SPEED;

    // Ajuste la vitesse diagonale
    if (dx != 0 && dy != 0) {
        float diag = 1.0f / sqrtf(2.0f);
        dx *= diag;
        dy *= diag;
    }

    player->x += dx;
    player->y += dy;

    // Limites verticales
    if (player->y < 0) player->y = 0;
    if (player->y + player->height > windowHeight) player->y = windowHeight - player->height;

    // Limites horizontales (côté droit uniquement)
    int minX = windowWidth / 2;
    int maxX = windowWidth - player->width;
    if (player->x < minX) player->x = minX;
    if (player->x > maxX) player->x = maxX;

    // Gestion du tir
    if (state[SDL_SCANCODE_SPACE]) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > player->lastShotTime + FIRE_RATE_MS) {
            if (*numProjectiles < maxProjectiles) {
                float projectileStartX = player->x;
                float projectileStartY = player->y + (player->height / 2);

                Projectile* newProjectile = projectile_create(renderer, projectileStartX, projectileStartY);
                if (newProjectile != NULL) {
                    projectiles[*numProjectiles] = newProjectile;
                    (*numProjectiles)++;
                    player->lastShotTime = currentTime;
                    printf("Tir! Projectile cree. Total projectiles: %d\n", *numProjectiles);
                } else {
                    printf("Erreur: Impossible de creer un nouveau projectile.\n");
                }
            }
        }
    }
}

// Dessine le joueur
void player_draw(SDL_Renderer* renderer, Player* player) {
    SDL_Rect destRect = { (int)player->x, (int)player->y, player->width, player->height };
    SDL_Point center = { player->width / 2, player->height / 2 };
    SDL_RenderCopyEx(renderer, player->texture, NULL, &destRect, -90.0, &center, SDL_FLIP_NONE);
}
