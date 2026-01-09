#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>

// Paramètres de gameplay
#define ENEMY_SPEED 6.0f          // Vitesse de déplacement vers la droite (pixels/update)
#define NUM_EXPLOSION_FRAMES 8    // Nombre d'images dans la séquence d'explosion

typedef struct {
    float x, y;             // Position (float pour des mouvements fluides)
    int width, height;      // Dimensions (hitbox et affichage)
    int active;             // 1 = vivant/visible, 0 = inactif (prêt à être free)

    // Système d'animation d'explosion
    int isExploding;        // 1 = animation en cours (ne bouge plus, joue l'anim)
    int explosionFrame;     // Index de la frame d'explosion actuelle (0 à 7)
    Uint32 lastFrameTime;   // Timer pour gérer la vitesse de l'animation
} Enemy;

// --- Gestion des ressources statiques (Shared Textures) ---
// À appeler une seule fois au début/fin du programme (pas par ennemi)
int loadSharedEnemyTexture(SDL_Renderer* renderer);
void destroySharedEnemyTexture();

// --- Gestion des instances d'ennemis ---
Enemy* enemy_create(int windowHeight);

// Retourne 1 si l'ennemi est toujours actif, 0 s'il doit être supprimé (sortie d'écran ou fin d'explosion)
int enemy_update(Enemy* e, int windowWidth);

void enemy_draw(SDL_Renderer* renderer, const Enemy* e);
void enemy_destroy(Enemy* e);

// Déclenche la séquence de mort
void enemy_start_explosion(Enemy* e);

#endif