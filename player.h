#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "projectile.h"

// Variables globales définies dans player.c (pour les sons partagés)
extern const int NUM_LASER_SOUNDS;
extern const int NUM_EXPLOSION_SOUNDS;

// --- Configuration du Joueur ---
#define PLAYER_SPEED 7.0f   // Vitesse de déplacement (pixels par update)
#define FIRE_RATE_MS 200    // Temps minimum entre deux tirs (en millisecondes)
#define MAX_AMMO 35         // Capacité du chargeur

typedef struct {
    float x, y;             // Position précise du vaisseau
    int width, height;      // Dimensions (Hitbox pour les collisions)
    SDL_Texture* texture;   // Sprite (Image) du vaisseau
    Uint32 lastShotTime;    // Timestamp du dernier tir (gestion de la cadence)

    // Stats de jeu
    int hp;                 // Points de vie (Hit Points)
    int score;              // Score actuel du joueur
    int ammo;               // Munitions restantes dans le chargeur actuel
    int maxAmmo;            // Capacité maximale (pour l'affichage HUD)
} Player;

// --- Gestion du Cycle de Vie ---

// Crée le joueur, charge sa texture et l'initialise à la position donnée
Player* player_create(SDL_Renderer* renderer, float initialX, float initialY, float scaleFactor);

// Libère la mémoire et la texture du joueur
void player_destroy(Player* player);

// --- Logique de Jeu ---

// Gère les entrées clavier (Mouvements), les limites de l'écran et le tir
void player_update(Player* player, int windowWidth, int windowHeight,
                   SDL_Renderer* renderer,
                   Projectile** projectiles, int* numProjectiles, int maxProjectiles, Mix_Chunk* laserSounds[]);

// --- Rendu ---

// Affiche le vaisseau à l'écran (avec rotation éventuelle)
void player_draw(SDL_Renderer* renderer, Player* player);

#endif