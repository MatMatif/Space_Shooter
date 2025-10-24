#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "projectile.h"

// Structure du joueur
typedef struct {
    float x, y;          // Position
    int width, height;   // Dimensions
    SDL_Texture* texture; // Texture du vaisseau
    Uint32 lastShotTime; // Temps du dernier tir
} Player;

#define PLAYER_SPEED 7.0f   // Vitesse du joueur
#define FIRE_RATE_MS 200    // Délai entre deux tirs (ms)

// Création du joueur
Player* player_create(SDL_Renderer* renderer, float initialX, float initialY, float scaleFactor);

// Destruction du joueur
void player_destroy(Player* player);

// Mise à jour (mouvements + tirs)
void player_update(Player* player, int windowWidth, int windowHeight,
                   SDL_Renderer* renderer,
                   Projectile** projectiles, int* numProjectiles, int maxProjectiles);

// Dessin du joueur
void player_draw(SDL_Renderer* renderer, Player* player);

#endif // PLAYER_H
