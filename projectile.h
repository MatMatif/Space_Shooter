#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL2/SDL.h>

// Constantes du projectile
#define PROJECTILE_SPEED 15.0f
#define NUM_PROJECTILE_FRAMES 2      // Nombre d’images d’animation
#define PROJECTILE_FRAME_DELAY 100   // Délai entre chaque frame (ms)

// Structure d’un projectile
typedef struct {
    float x, y;
    int width, height;
    int active; // Actif ou non
} Projectile;

// Création d’un projectile
Projectile* projectile_create(SDL_Renderer* renderer, float x, float y);

// Mise à jour de la position
int projectile_update(Projectile* p, int windowWidth, int windowHeight);

// Mise à jour de l’animation partagée
void projectile_update_animation();

// Dessin du projectile
void projectile_draw(SDL_Renderer* renderer, const Projectile* p);

// Destruction d’un projectile
void projectile_destroy(Projectile* p);

// Libération des textures partagées
void destroySharedProjectileFrames();

#endif // PROJECTILE_H
