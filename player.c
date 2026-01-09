#include "player.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int NUM_LASER_SOUNDS = 3;
const int NUM_EXPLOSION_SOUNDS = 3;

// Initialise le joueur (sprite, position, stats)
Player* player_create(SDL_Renderer* renderer, float initialX, float initialY, float scaleFactor) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (player == NULL) return NULL;

    // Chargement de la texture du vaisseau
    SDL_Surface* surface = IMG_Load("assets/player/player_ship.png");
    if (surface == NULL) {
        printf("Erreur image joueur: %s\n", IMG_GetError());
        free(player);
        return NULL;
    }

    player->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Calcul des dimensions avec le facteur d'échelle (zoom)
    int originalWidth, originalHeight;
    SDL_QueryTexture(player->texture, NULL, NULL, &originalWidth, &originalHeight);
    player->width = (int)(originalWidth * scaleFactor);
    player->height = (int)(originalHeight * scaleFactor);

    // Positionnement initial (centré sur le point donné)
    player->x = initialX - player->width;
    player->y = initialY - (player->height / 2);
    player->lastShotTime = 0;

    // Stats de départ
    player->hp = 4;
    player->score = 0;
    player->maxAmmo = MAX_AMMO;
    player->ammo = MAX_AMMO;

    printf("Joueur cree. Munitions : %d/%d\n", player->ammo, player->maxAmmo);
    return player;
}

// Libère la mémoire (texture + structure)
void player_destroy(Player* player) {
    if (player != NULL) {
        SDL_DestroyTexture(player->texture);
        free(player);
    }
}

// Gère les déplacements, les collisions avec les bords et le tir
void player_update(Player* player, int windowWidth, int windowHeight,
                   SDL_Renderer* renderer,
                   Projectile** projectiles, int* numProjectiles, int maxProjectiles, Mix_Chunk* laserSounds[]) {

    const Uint8* state = SDL_GetKeyboardState(NULL);

    // --- 1. Gestion du Mouvement ---
    float dx = 0, dy = 0;
    if (state[SDL_SCANCODE_UP]) dy -= PLAYER_SPEED;
    if (state[SDL_SCANCODE_DOWN]) dy += PLAYER_SPEED;
    if (state[SDL_SCANCODE_LEFT]) dx -= PLAYER_SPEED;
    if (state[SDL_SCANCODE_RIGHT]) dx += PLAYER_SPEED;

    // Normalisation diagonale (pour ne pas aller plus vite en biais)
    if (dx != 0 && dy != 0) {
        float diag = 1.0f / sqrtf(2.0f);
        dx *= diag;
        dy *= diag;
    }

    player->x += dx;
    player->y += dy;

    // --- 2. Contraintes de position (Limites de l'écran) ---
    // Axe Y (Haut / Bas)
    if (player->y < 0) player->y = 0;
    if (player->y + player->height > windowHeight) player->y = windowHeight - player->height;

    // Axe X (Gauche / Droite) - Le joueur est bloqué dans la moitié droite
    int minX = windowWidth / 2;
    int maxX = windowWidth - player->width;
    if (player->x < minX) player->x = minX;
    if (player->x > maxX) player->x = maxX;

    // --- 3. Gestion du Tir (Espace) ---
    if (state[SDL_SCANCODE_SPACE]) {
        Uint32 currentTime = SDL_GetTicks();

        // Vérification du délai (Cadence de tir)
        if (currentTime > player->lastShotTime + FIRE_RATE_MS) {

            // Vérification des munitions
            if (player->ammo > 0) {

                if (*numProjectiles < maxProjectiles) {
                    float projectileStartX = player->x;
                    float projectileStartY = player->y + (player->height / 2);

                    Projectile* newProjectile = projectile_create(renderer, projectileStartX, projectileStartY);
                    if (newProjectile != NULL) {
                        // Ajout au tableau de projectiles
                        projectiles[*numProjectiles] = newProjectile;
                        (*numProjectiles)++;

                        player->lastShotTime = currentTime;
                        player->ammo--; // On décrémente les balles

                        // Son aléatoire
                        int soundIndex = rand() % NUM_LASER_SOUNDS;
                        Mix_PlayChannel(-1, laserSounds[soundIndex], 0);
                    }
                }
            } else {
                // (Optionnel) Jouer un son "clic" si vide
            }
        }
    }
}

// Affiche le joueur avec rotation
void player_draw(SDL_Renderer* renderer, Player* player) {
    SDL_Rect destRect = { (int)player->x, (int)player->y, player->width, player->height };
    SDL_Point center = { player->width / 2, player->height / 2 };

    // Rotation de -90° car le sprite regarde vers le haut, et on veut qu'il regarde à gauche
    SDL_RenderCopyEx(renderer, player->texture, NULL, &destRect, -90.0, &center, SDL_FLIP_NONE);
}