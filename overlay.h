#ifndef OVERLAY_H
#define OVERLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"

// --- Configuration de l'Interface (HUD) ---
#define NUM_HEART_STATES 5    // Nombre d'images pour la barre de vie (ex: 4 PV, 3 PV... 0 PV)
#define NUM_AMMO_STATES 6     // Nombre d'images pour la jauge de munitions

typedef struct {
    // Tableaux de textures pour les états visuels
    SDL_Texture* heartTextures[NUM_HEART_STATES];
    SDL_Texture* ammoTextures[NUM_AMMO_STATES];

    TTF_Font* font;           // Police utilisée pour afficher le score

    // Dimensions des sprites (utiles pour le placement à l'écran)
    int heartWidth, heartHeight;
    int ammoWidth, ammoHeight;
} Overlay;

// --- Gestion du Cycle de Vie ---

// Initialise l'interface : charge toutes les images et la police d'écriture
Overlay* overlay_create(SDL_Renderer* renderer);

// Libère toutes les ressources (textures et police)
void overlay_destroy(Overlay* overlay);

// --- Rendu ---

// Affiche le HUD (Vie, Munitions, Score) par-dessus le jeu en fonction des stats du joueur
void overlay_draw(SDL_Renderer* renderer, Overlay* overlay, const Player* player, int windowWidth);

#endif