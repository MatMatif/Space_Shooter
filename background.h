#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>

// Configuration de l'animation et du défilement
#define NUM_GIF_FRAMES 4             // Nombre d'images dans la boucle d'animation
#define FRAME_DELAY 100              // Durée d'une frame en millisecondes
#define BACKGROUND_SCROLL_SPEED 2.0f // Vitesse de défilement (pixels par update)

typedef struct {
    SDL_Texture** frames;    // Tableau dynamique stockant les textures de l'animation
    int numFrames;           // Nombre total de frames chargées
    int currentFrame;        // Index de l'image actuellement affichée
    Uint32 lastFrameTime;    // Timestamp du dernier changement d'image
    int frameWidth;          // Dimensions d'une frame unique
    int frameHeight;
    float scrollOffsetX;     // Position actuelle du scrolling (float pour fluidité)
} Background;

// Initialisation et chargement des ressources
Background* background_create(SDL_Renderer* renderer, int windowWidth, int windowHeight);

// Mise à jour de la logique (animation + calcul du défilement)
void background_update(Background* bg);

// Rendu à l'écran (gestion de la répétition des tuiles)
void background_draw(SDL_Renderer* renderer, const Background* bg, int windowWidth, int windowHeight);

// Nettoyage complet de la mémoire
void background_destroy(Background* bg);

#endif