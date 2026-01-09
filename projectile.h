#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL2/SDL.h>

// Configuration du tir
#define PROJECTILE_SPEED 15.0f          // Vitesse de déplacement (pixels par frame)
#define NUM_PROJECTILE_FRAMES 2         // Nombre d'images dans l'animation du laser
#define PROJECTILE_FRAME_DELAY 100      // Vitesse de l'animation (en millisecondes)

typedef struct {
    float x, y;            // Position précise
    int width, height;     // Dimensions pour la collision et l'affichage
    int active;            // 1 = en vol, 0 = détruit ou hors écran (prêt à être free)
} Projectile;

// Crée une nouvelle instance de projectile à la position (x, y)
Projectile* projectile_create(SDL_Renderer* renderer, float x, float y);

// Déplace le projectile. Retourne 0 s'il sort de l'écran (doit être détruit), sinon 1.
int projectile_update(Projectile* p, int windowWidth, int windowHeight);

// Met à jour la frame d'animation GLOBALE (à appeler une seule fois par tour de boucle)
void projectile_update_animation();

// Affiche le projectile (avec rotation si nécessaire)
void projectile_draw(SDL_Renderer* renderer, const Projectile* p);

// Libère la mémoire d'une instance de projectile
void projectile_destroy(Projectile* p);

// Libère les textures partagées (static) à la fin du jeu
void destroySharedProjectileFrames();

#endif