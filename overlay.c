#include "overlay.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Initialise l'overlay (charge les images Cœurs + Munitions + Police)
Overlay* overlay_create(SDL_Renderer* renderer) {
    Overlay* overlay = (Overlay*)malloc(sizeof(Overlay));
    if (!overlay) return NULL;

    // --- 1. Chargement des textures de Cœurs (Vie) ---
    for (int i = 0; i < NUM_HEART_STATES; i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "assets/hearts/heart_%d.png", i);

        SDL_Surface* heartSurf = IMG_Load(filename);
        if (!heartSurf) {
            printf("Erreur chargement %s : %s\n", filename, IMG_GetError());
            overlay->heartTextures[i] = NULL;
        } else {
            overlay->heartTextures[i] = SDL_CreateTextureFromSurface(renderer, heartSurf);
            // On récupère la taille de l'image (une seule fois suffit)
            if (i == 0) {
                SDL_QueryTexture(overlay->heartTextures[i], NULL, NULL, &overlay->heartWidth, &overlay->heartHeight);
            }
            SDL_FreeSurface(heartSurf);
        }
    }

    // --- 2. Chargement des textures de Munitions ---
    for (int i = 0; i < NUM_AMMO_STATES; i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "assets/ammos/Ammos_%d.png", i);

        SDL_Surface* ammoSurf = IMG_Load(filename);
        if (!ammoSurf) {
            printf("Erreur chargement %s : %s\n", filename, IMG_GetError());
            overlay->ammoTextures[i] = NULL;
        } else {
            overlay->ammoTextures[i] = SDL_CreateTextureFromSurface(renderer, ammoSurf);
            if (i == 0) {
                SDL_QueryTexture(overlay->ammoTextures[i], NULL, NULL, &overlay->ammoWidth, &overlay->ammoHeight);
            }
            SDL_FreeSurface(ammoSurf);
        }
    }

    // --- 3. Chargement de la Police (Score) ---
    overlay->font = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 28);

    return overlay;
}

// Dessine tous les éléments de l'interface par-dessus le jeu
void overlay_draw(SDL_Renderer* renderer, Overlay* overlay, const Player* player, int windowWidth) {
    if (!overlay || !player) return;

    // --- A. GESTION DES CŒURS ---
    // Calcul de l'index de l'image selon la vie restante
    // (Suppose que heart_0.png = Plein et heart_4.png = Vide)
    int maxHP = 4;
    int heartIndex = maxHP - player->hp;

    // Sécurité bornes
    if (heartIndex < 0) heartIndex = 0;
    if (heartIndex >= NUM_HEART_STATES) heartIndex = NUM_HEART_STATES - 1;

    SDL_Texture* textureToDraw = overlay->heartTextures[heartIndex];

    if (textureToDraw) {
        float scale = 4.0f;
        SDL_Rect heartRect = {
            20, 20, // Position X, Y
            (int)(overlay->heartWidth * scale),
            (int)(overlay->heartHeight * scale)
        };
        SDL_RenderCopy(renderer, textureToDraw, NULL, &heartRect);
    }

    // --- B. GESTION DES MUNITIONS ---
    // Calcul proportionnel pour choisir l'image du chargeur
    int maxImageIndex = NUM_AMMO_STATES - 1; // ex: 5 images (0 à 5)
    int ammoIndex = 0;

    if (player->maxAmmo > 0) {
        // Règle de trois pour trouver l'image correspondante
        int ratio = (player->ammo * maxImageIndex) / player->maxAmmo;
        ammoIndex = maxImageIndex - ratio;
    }

    // Sécurité bornes
    if (ammoIndex < 0) ammoIndex = 0;
    if (ammoIndex > maxImageIndex) ammoIndex = maxImageIndex;

    SDL_Texture* ammoTextureToDraw = overlay->ammoTextures[ammoIndex];

    if (ammoTextureToDraw) {
        float ammoScale = 0.7f;
        SDL_Rect ammoRect = {
            20,
            20 + (int)(overlay->heartHeight * 4.0f) + 10, // Juste en dessous du cœur
            (int)(overlay->ammoWidth * ammoScale),
            (int)(overlay->ammoHeight * ammoScale)
        };
        SDL_RenderCopy(renderer, ammoTextureToDraw, NULL, &ammoRect);
    }

    // --- C. AFFICHAGE DU SCORE ---
    if (overlay->font) {
        SDL_Color white = {255, 255, 255, 255};
        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "SCORE: %05d", player->score);

        // Création de la texture de texte (Note: coûteux en perfs de le faire à chaque frame,
        // mais acceptable pour un petit projet)
        SDL_Surface* textSurf = TTF_RenderText_Solid(overlay->font, scoreText, white);
        if (textSurf) {
            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
            SDL_Rect textRect = {
                windowWidth - textSurf->w - 30, // Aligné à droite
                20,
                textSurf->w,
                textSurf->h
            };
            SDL_RenderCopy(renderer, textTex, NULL, &textRect);

            // Nettoyage immédiat de la surface et texture temporaires
            SDL_FreeSurface(textSurf);
            SDL_DestroyTexture(textTex);
        }
    }
}

// Nettoyage complet de la mémoire
void overlay_destroy(Overlay* overlay) {
    if (overlay) {
        for (int i = 0; i < NUM_HEART_STATES; i++) {
            if (overlay->heartTextures[i]) SDL_DestroyTexture(overlay->heartTextures[i]);
        }
        for (int i = 0; i < NUM_AMMO_STATES; i++) {
            if (overlay->ammoTextures[i]) SDL_DestroyTexture(overlay->ammoTextures[i]);
        }
        if (overlay->font) TTF_CloseFont(overlay->font);
        free(overlay);
    }
}