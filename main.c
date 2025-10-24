#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "background.h"
#include "projectile.h"

// Taille de la fenêtre
const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 900;

// Joueur
const float PLAYER_INITIAL_SCALE = 3.0f;
const int PLAYER_INITIAL_RIGHT_MARGIN = 100;

// Nombre max de projectiles
#define MAX_PROJECTILES 50000

int main(int argc, char* argv[]) {
    // --- Init SDL ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Erreur SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // --- Fenêtre + Renderer ---
    SDL_Window* window = SDL_CreateWindow("Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Erreur renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Fond ---
    Background* background = background_create(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!background) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // --- Joueur ---
    float playerTargetX = (float)WINDOW_WIDTH - PLAYER_INITIAL_RIGHT_MARGIN;
    float playerTargetY = (float)WINDOW_HEIGHT / 2.0f;
    Player* player = player_create(renderer, playerTargetX, playerTargetY, PLAYER_INITIAL_SCALE);
    if (!player) {
        background_destroy(background);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 3;
    }

    // --- Projectiles ---
    Projectile* projectiles[MAX_PROJECTILES];
    for (int i = 0; i < MAX_PROJECTILES; ++i) projectiles[i] = NULL;
    int numActiveProjectiles = 0;

    // --- Boucle principale ---
    int running = 1;
    SDL_Event event;

    while (running) {
        // Événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        // Logique
        background_update(background);
        projectile_update_animation();
        player_update(player, WINDOW_WIDTH, WINDOW_HEIGHT,
                      renderer, projectiles, &numActiveProjectiles, MAX_PROJECTILES);

        // MAJ projectiles
        for (int i = 0; i < numActiveProjectiles;) {
            if (projectiles[i] && projectiles[i]->active) {
                if (!projectile_update(projectiles[i], WINDOW_WIDTH, WINDOW_HEIGHT)) {
                    projectile_destroy(projectiles[i]);
                    projectiles[i] = NULL;
                    if (i < numActiveProjectiles - 1) {
                        projectiles[i] = projectiles[numActiveProjectiles - 1];
                        projectiles[numActiveProjectiles - 1] = NULL;
                    }
                    numActiveProjectiles--;
                } else i++;
            } else i++;
        }

        // Rendu
        SDL_SetRenderDrawColor(renderer, 16, 20, 35, 255);
        SDL_RenderClear(renderer);
        background_draw(renderer, background, WINDOW_WIDTH, WINDOW_HEIGHT);
        player_draw(renderer, player);
        for (int i = 0; i < numActiveProjectiles; ++i)
            projectile_draw(renderer, projectiles[i]);
        SDL_RenderPresent(renderer);
    }

    // --- Nettoyage ---
    background_destroy(background);
    player_destroy(player);
    for (int i = 0; i < numActiveProjectiles; ++i)
        projectile_destroy(projectiles[i]);
    destroySharedProjectileFrames();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
