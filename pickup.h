#ifndef PICKUP_H
#define PICKUP_H

#include <SDL2/SDL.h>

// Configuration des objets à ramasser (Munitions)
#define PICKUP_SPEED 5.0f         // Vitesse de déplacement (pixels par frame)
#define PICKUP_WIDTH 40           // Largeur de la zone de collision/affichage
#define PICKUP_HEIGHT 40          // Hauteur de la zone de collision/affichage

typedef struct {
    float x, y;            // Position précise de l'objet
    int width, height;     // Dimensions (utilisées pour les collisions AABB)
    int active;            // 1 = actif/visible, 0 = ramassé ou sorti de l'écran
} Pickup;

// --- Gestion des Ressources (Texture partagée) ---

// Charge la texture unique pour toutes les munitions (à appeler une seule fois au début)
// Retourne 1 si succès, 0 si erreur.
int pickup_init_texture(SDL_Renderer* renderer);

// Libère la texture partagée (à appeler à la fin du programme)
void pickup_cleanup_texture();

// --- Gestion des Instances (Cycle de vie) ---

// Crée une nouvelle caisse de munitions à une position Y aléatoire (et X hors écran gauche)
Pickup* pickup_create(int windowWidth, int windowHeight);

// Met à jour la position (déplacement vers la droite)
// Retourne 0 si l'objet sort de l'écran (doit être détruit), sinon 1.
int pickup_update(Pickup* p, int windowWidth);

// Affiche l'objet à l'écran
void pickup_draw(SDL_Renderer* renderer, Pickup* p);

// Libère la mémoire d'une instance spécifique
void pickup_destroy(Pickup* p);

#endif