#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>

// Constantes de l’animation
#define NUM_GIF_FRAMES 4 // Nombre de frames
#define FRAME_DELAY 100  // Délai entre frames (ms)
#define BACKGROUND_SCROLL_SPEED 2.0f

// Structure du fond animé
typedef struct {
    SDL_Texture** frames;  // Tableau de textures
    int numFrames;         // Total des frames
    int currentFrame;      // Frame actuelle
    Uint32 lastFrameTime;  // Dernier changement de frame
    int frameWidth;        // Largeur d’une frame
    int frameHeight;       // Hauteur d’une frame
    float scrollOffsetX;   // Décalage horizontal
} Background;

// Création et chargement du fond
Background* background_create(SDL_Renderer* renderer, int windowWidth, int windowHeight);

// Mise à jour de l’animation
void background_update(Background* bg);

// Dessin du fond en tiling
void background_draw(SDL_Renderer* renderer, const Background* bg, int windowWidth, int windowHeight);

// Destruction du fond
void background_destroy(Background* bg);

#endif // BACKGROUND_H
