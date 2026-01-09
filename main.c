#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "overlay.h"
#include "player.h"
#include "background.h"
#include "projectile.h"
#include "enemy.h"
#include "pickup.h"

// --- CONSTANTES GLOBALES ---
const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 900;
const int POINTS_PER_KILL = 100;

// Configuration Joueur
const float PLAYER_INITIAL_SCALE = 3.0f;
const int PLAYER_INITIAL_RIGHT_MARGIN = 100;

// Limites
#define MAX_PROJECTILES 2000
#define MAX_ENEMIES 10
#define ENEMY_SPAWN_INTERVAL_MS 1000

// Munitions (Ravitaillement)
#define MAX_PICKUPS 5
#define AMMO_SPAWN_INTERVAL_MS 30000

// Fluidité
#define TARGET_FPS 60
#define FRAME_DELAY_TIME (1000 / TARGET_FPS)

// États du Jeu
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING
} GameState;

// Fonction utilitaire pour centrer du texte
void draw_text_centered(SDL_Renderer* renderer, TTF_Font* font, const char* text, int y, SDL_Color color, int windowWidth) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = { (windowWidth - surface->w) / 2, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

int main(int argc, char* argv[]) {
    // --- 1. INITIALISATION SDL ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) return 1;
    if (TTF_Init() == -1) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 1;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // --- 2. CHARGEMENT RESSOURCES ---
    // Polices
    TTF_Font* titleFont = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 60);
    TTF_Font* menuFont = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 24);
    if (!titleFont || !menuFont) printf("Attention: Police non trouvée (assets/fonts/...)\n");

    // Sons
    Mix_Chunk* laserSounds[NUM_LASER_SOUNDS];
    Mix_Chunk* explosionSounds[NUM_EXPLOSION_SOUNDS];

    laserSounds[0] = Mix_LoadWAV("assets/sons/Benjamin_1.wav");
    laserSounds[1] = Mix_LoadWAV("assets/sons/Benjamin_2.wav");
    laserSounds[2] = Mix_LoadWAV("assets/sons/Benjamin_3.wav");

    explosionSounds[0] = Mix_LoadWAV("assets/sons/Present_1.wav");
    explosionSounds[1] = Mix_LoadWAV("assets/sons/Present_2.wav");
    explosionSounds[2] = Mix_LoadWAV("assets/sons/Present_3.wav");

    // Textures partagées
    if (!loadSharedEnemyTexture(renderer)) printf("Erreur texture ennemi\n");
    if (!pickup_init_texture(renderer)) printf("Erreur texture munitions\n");

    // --- 3. CRÉATION ENTITÉS ---
    Background* background = background_create(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    float startX = (float)WINDOW_WIDTH - PLAYER_INITIAL_RIGHT_MARGIN;
    float startY = (float)WINDOW_HEIGHT / 2.0f;
    Player* player = player_create(renderer, startX, startY, PLAYER_INITIAL_SCALE);
    Overlay* overlay = overlay_create(renderer);

    // Tableaux dynamiques
    Projectile* projectiles[MAX_PROJECTILES] = {NULL};
    int nProj = 0;

    Enemy* enemies[MAX_ENEMIES] = {NULL};
    int nEnemies = 0;

    Pickup* pickups[MAX_PICKUPS] = {NULL};
    int nPickups = 0;

    // --- 4. BOUCLE DE JEU ---
    Uint32 lastEnemySpawnTime = 0;
    Uint32 lastAmmoSpawnTime = 0;
    int running = 1;
    SDL_Event event;
    srand(time(NULL));

    GameState currentState = GAME_STATE_MENU;
    Uint32 frameStart;
    int frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        // --- A. EVENTS ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            if (event.type == SDL_KEYDOWN) {
                // MENU -> JEU
                if (currentState == GAME_STATE_MENU) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        currentState = GAME_STATE_PLAYING;

                        // RESET COMPLET
                        player->hp = 4;
                        player->score = 0;
                        player->ammo = player->maxAmmo;
                        player->x = startX - player->width;
                        player->y = startY - (player->height / 2);

                        // Nettoyage tableaux
                        for(int i=0; i<nEnemies; i++) { enemy_destroy(enemies[i]); enemies[i]=NULL; } nEnemies=0;
                        for(int i=0; i<nProj; i++) { projectile_destroy(projectiles[i]); projectiles[i]=NULL; } nProj=0;
                        for(int i=0; i<nPickups; i++) { pickup_destroy(pickups[i]); pickups[i]=NULL; } nPickups=0;

                        printf(">>> START GAME <<<\n");
                    }
                }
                // JEU -> MENU (Pause/Quit)
                else if (currentState == GAME_STATE_PLAYING) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        currentState = GAME_STATE_MENU;
                    }
                }
            }
        }

        // --- B. RENDU COMMUN ---
        SDL_SetRenderDrawColor(renderer, 16, 20, 35, 255);
        SDL_RenderClear(renderer);

        background_update(background);
        background_draw(renderer, background, WINDOW_WIDTH, WINDOW_HEIGHT);

        // --- C. LOGIQUE PAR ÉTAT ---
        if (currentState == GAME_STATE_MENU) {
            SDL_Color gold = {255, 200, 50, 255};
            SDL_Color white = {255, 255, 255, 255};

            draw_text_centered(renderer, titleFont, "SPACE SHOOTER", 250, gold, WINDOW_WIDTH);

            if ((SDL_GetTicks() / 500) % 2 == 0) {
                draw_text_centered(renderer, menuFont, "PRESS ENTER TO START", 500, white, WINDOW_WIDTH);
            }
            draw_text_centered(renderer, menuFont, "ARROWS: Move | SPACE: Shoot | GET AMMO BOXES!", 650, white, WINDOW_WIDTH);
        }
        else if (currentState == GAME_STATE_PLAYING) {
            Uint32 currentTime = SDL_GetTicks();
            projectile_update_animation();

            // 1. Update Player
            player_update(player, WINDOW_WIDTH, WINDOW_HEIGHT, renderer, projectiles, &nProj, MAX_PROJECTILES, laserSounds);

            // 2. Spawn Ennemis
            if (currentTime > lastEnemySpawnTime + ENEMY_SPAWN_INTERVAL_MS) {
                if (nEnemies < MAX_ENEMIES) {
                    enemies[nEnemies++] = enemy_create(WINDOW_HEIGHT);
                    lastEnemySpawnTime = currentTime;
                }
            }

            // 3. Spawn Munitions
            if (currentTime > lastAmmoSpawnTime + AMMO_SPAWN_INTERVAL_MS) {
                if (nPickups < MAX_PICKUPS) {
                    pickups[nPickups] = pickup_create(WINDOW_WIDTH, WINDOW_HEIGHT);
                    if (pickups[nPickups]) {
                        printf(">>> CAISSE MUNITION APPARUE (Y: %.0f)\n", pickups[nPickups]->y);
                        nPickups++;
                        lastAmmoSpawnTime = currentTime;
                    }
                }
            }

            // 4. Update Ennemis (Collisions / Mouvement)
            for (int i = 0; i < nEnemies; ) {
                if (enemies[i] && enemies[i]->active) {
                    if (!enemy_update(enemies[i], WINDOW_WIDTH)) {
                        enemy_destroy(enemies[i]);
                        enemies[i] = enemies[--nEnemies];
                    } else {
                        enemy_draw(renderer, enemies[i]);
                        i++;
                    }
                } else i++;
            }

            // 5. Update Pickups (Munitions)
            for (int i = 0; i < nPickups; ) {
                if (pickups[i] && pickups[i]->active) {
                    // Note: On passe WINDOW_WIDTH pour gérer la sortie à droite
                    if (!pickup_update(pickups[i], WINDOW_WIDTH)) {
                        printf("Caisse munition ratee !\n");
                        pickup_destroy(pickups[i]);
                        pickups[i] = pickups[--nPickups];
                    } else {
                        pickup_draw(renderer, pickups[i]);

                        // Collision Joueur vs Munition
                        SDL_Rect pRect = {(int)player->x, (int)player->y, player->width, player->height};
                        SDL_Rect iRect = {(int)pickups[i]->x, (int)pickups[i]->y, pickups[i]->width, pickups[i]->height};

                        if (SDL_HasIntersection(&pRect, &iRect)) {
                            player->ammo = player->maxAmmo;
                            player->score += 50;
                            printf("RECHARGE ! Ammo: %d\n", player->ammo);

                            pickup_destroy(pickups[i]);
                            pickups[i] = pickups[--nPickups];
                            continue;
                        }
                        i++;
                    }
                } else i++;
            }

            // 6. Update Projectiles + Collisions Ennemis
            for (int i = 0; i < nProj; ) {
                if (projectiles[i] && projectiles[i]->active) {
                    if (!projectile_update(projectiles[i], WINDOW_WIDTH, WINDOW_HEIGHT)) {
                        projectile_destroy(projectiles[i]);
                        projectiles[i] = projectiles[--nProj];
                    } else {
                        projectile_draw(renderer, projectiles[i]);

                        // Collision Tir vs Ennemi
                        SDL_Rect bRect = {(int)projectiles[i]->x, (int)projectiles[i]->y, projectiles[i]->width, projectiles[i]->height};

                        for (int j = 0; j < nEnemies; j++) {
                            if (enemies[j]->active && !enemies[j]->isExploding) {
                                SDL_Rect eRect = {(int)enemies[j]->x, (int)enemies[j]->y, enemies[j]->width, enemies[j]->height};
                                if (SDL_HasIntersection(&bRect, &eRect)) {
                                    projectiles[i]->active = 0;
                                    enemy_start_explosion(enemies[j]);
                                    player->score += POINTS_PER_KILL;
                                    Mix_PlayChannel(-1, explosionSounds[rand()%3], 0);
                                    break;
                                }
                            }
                        }
                        i++;
                    }
                } else i++;
            }

            // 7. Collision Joueur vs Ennemi (Dégâts)
            SDL_Rect pRect = {(int)player->x, (int)player->y, player->width, player->height};
            for (int i = 0; i < nEnemies; i++) {
                if (enemies[i]->active && !enemies[i]->isExploding) {
                    SDL_Rect eRect = {(int)enemies[i]->x, (int)enemies[i]->y, enemies[i]->width, enemies[i]->height};
                    if (SDL_HasIntersection(&pRect, &eRect)) {
                        player->hp--;
                        enemy_start_explosion(enemies[i]);
                        Mix_PlayChannel(-1, explosionSounds[rand()%3], 0);

                        if (player->hp <= 0) {
                            printf("GAME OVER - Score final: %d\n", player->score);
                            currentState = GAME_STATE_MENU;
                        }
                    }
                }
            }

            // 8. Dessin HUD et Joueur
            player_draw(renderer, player);
            overlay_draw(renderer, overlay, player, WINDOW_WIDTH);
        }

        SDL_RenderPresent(renderer);

        // --- D. GESTION FPS ---
        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY_TIME > frameTime) {
            SDL_Delay(FRAME_DELAY_TIME - frameTime);
        }
    }

    // --- 5. NETTOYAGE FINAL ---
    background_destroy(background);
    player_destroy(player);
    overlay_destroy(overlay);

    if (titleFont) TTF_CloseFont(titleFont);
    if (menuFont) TTF_CloseFont(menuFont);
    TTF_Quit();

    for(int i=0; i<nProj; i++) projectile_destroy(projectiles[i]);
    for(int i=0; i<nEnemies; i++) enemy_destroy(enemies[i]);
    for(int i=0; i<nPickups; i++) pickup_destroy(pickups[i]);

    for(int i=0; i<3; i++) { Mix_FreeChunk(laserSounds[i]); Mix_FreeChunk(explosionSounds[i]); }

    destroySharedProjectileFrames();
    destroySharedEnemyTexture();
    pickup_cleanup_texture();

    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}