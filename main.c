#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "background.h"
#include "projectile.h"
#include "enemy.h"

// Taille de la fenêtre
const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 900;

// Joueur
const float PLAYER_INITIAL_SCALE = 3.0f;
const int PLAYER_INITIAL_RIGHT_MARGIN = 100;

// Nombre max de projectiles
#define MAX_PROJECTILES 50000
#define MAX_ENEMIES 100
#define ENEMY_SPAWN_INTERVAL_MS 1000

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

    if (!loadSharedEnemyTexture(renderer)) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
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

    // --- Ennemies ---
    Enemy* enemies[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; ++i) { enemies[i] = NULL; }
    int numActiveEnemies = 0;
    Uint32 lastEnemySpawnTime = 0;


    // --- Boucle principale ---
    int running = 1;
    SDL_Event event;

    while (running) {
        // Événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        // Logique
        Uint32 currentTime = SDL_GetTicks();
        background_update(background);
        projectile_update_animation();
        player_update(player, WINDOW_WIDTH, WINDOW_HEIGHT,
                      renderer, projectiles, &numActiveProjectiles, MAX_PROJECTILES);

        // Apparition des ennemis
        if (currentTime > lastEnemySpawnTime + ENEMY_SPAWN_INTERVAL_MS) {
            if (numActiveEnemies < MAX_ENEMIES) {
                Enemy* newEnemy = enemy_create(WINDOW_HEIGHT);
                if (newEnemy != NULL) {
                    enemies[numActiveEnemies] = newEnemy;
                    numActiveEnemies++;
                    lastEnemySpawnTime = currentTime;
                }
            }
        }

        // --- MISE À JOUR DES ENNEMIS ---
        for (int i = 0; i < numActiveEnemies; ) {
            if (enemies[i] != NULL && enemies[i]->active) {
                if (!enemy_update(enemies[i], WINDOW_WIDTH)) {
                    enemy_destroy(enemies[i]);
                    enemies[i] = enemies[numActiveEnemies - 1];
                    numActiveEnemies--;
                } else {
                    i++;
                }
            } else { i++; }
        }
        // --- MISE À JOUR DES PROJECTILES ---
        for (int i = 0; i < numActiveProjectiles;) {
            printf("a");
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

        // --- DÉTECTION DES COLLISIONS ---
        for (int i = 0; i < numActiveProjectiles; ++i) {
            for (int j = 0; j < numActiveEnemies; ++j) {
                if (projectiles[i]->active && enemies[j]->active) {
                    SDL_Rect projRect = { (int)projectiles[i]->x, (int)projectiles[i]->y, projectiles[i]->width, projectiles[i]->height };
                    SDL_Rect enemyRect = { (int)enemies[j]->x, (int)enemies[j]->y, enemies[j]->width, enemies[j]->height };
                    if (SDL_HasIntersection(&projRect, &enemyRect)) {
                        projectiles[i]->active = 0;
                        enemies[j]->active = 0;
                        printf("HIT!\n");
                        break;
                    }
                }
            }
        }



        // Rendu
        SDL_SetRenderDrawColor(renderer, 16, 20, 35, 255);
        SDL_RenderClear(renderer);
        background_draw(renderer, background, WINDOW_WIDTH, WINDOW_HEIGHT);
        player_draw(renderer, player);

        for (int i = 0; i < numActiveProjectiles; ++i)
            projectile_draw(renderer, projectiles[i]);

        for (int i = 0; i < numActiveEnemies; ++i) {
            enemy_draw(renderer, enemies[i]);
        }

        player_draw(renderer, player);
        for (int i = 0; i < numActiveProjectiles; ++i) {
            projectile_draw(renderer, projectiles[i]);
        }
        SDL_RenderPresent(renderer);
    }
    // --- Nettoyage ---
    background_destroy(background);
    player_destroy(player);

    for (int i = 0; i < numActiveProjectiles; ++i) projectile_destroy(projectiles[i]);
    for (int i = 0; i < numActiveEnemies; ++i) enemy_destroy(enemies[i]);

    destroySharedProjectileFrames();
    destroySharedEnemyTexture();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
