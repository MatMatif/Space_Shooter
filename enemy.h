#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>

// Constantes pour les ennemis
#define ENEMY_SPEED 6.0f // Vitesse de déplacement de l'ennemi

// Structure représentant un ennemi
typedef struct {
    float x, y;
    int width, height;
    int active;
} Enemy;

// Charge la texture partagée pour tous les ennemis. Doit être appelée une fois.
int loadSharedEnemyTexture(SDL_Renderer* renderer);

// Détruit la texture partagée des ennemis. Doit être appelée à la fin.
void destroySharedEnemyTexture();

// Crée un nouvel ennemi à une position Y aléatoire, hors de l'écran (à gauche)
Enemy* enemy_create(int windowHeight);

// Met à jour la position de l'ennemi
// Retourne 1 si l'ennemi est toujours actif, 0 s'il est hors écran
int enemy_update(Enemy* e, int windowWidth);

// Dessine l'ennemi
void enemy_draw(SDL_Renderer* renderer, const Enemy* e);

// Détruit un ennemi et libère la mémoire
void enemy_destroy(Enemy* e);

#endif // ENEMY_H