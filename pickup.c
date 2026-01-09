#include "pickup.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Texture partagée (Flyweight pattern)
static SDL_Texture* pickupTexture = NULL;

// Charge la texture une seule fois en mémoire
int pickup_init_texture(SDL_Renderer* renderer) {
    if (pickupTexture != NULL) return 1; // Déjà chargé

    SDL_Surface* surface = IMG_Load("assets/ammos/Ammos_6.png");
    if (!surface) {
        printf("ERREUR CHARGEMENT TEXTURE PICKUP : %s\n", IMG_GetError());
        return 0;
    }

    pickupTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return (pickupTexture != NULL);
}

// Libère la mémoire à la fin du jeu
void pickup_cleanup_texture() {
    if (pickupTexture) {
        SDL_DestroyTexture(pickupTexture);
        pickupTexture = NULL;
    }
}

// Crée une nouvelle caisse de munitions
Pickup* pickup_create(int windowWidth, int windowHeight) {
    Pickup* p = (Pickup*)malloc(sizeof(Pickup));
    if (!p) return NULL;

    p->width = PICKUP_WIDTH;
    p->height = PICKUP_HEIGHT;

    // --- LOGIQUE GAUCHE -> DROITE ---
    // On fait apparaître l'objet à GAUCHE, juste en dehors de l'écran (négatif)
    p->x = -PICKUP_WIDTH;

    // Position Y aléatoire avec une marge pour ne pas toucher les bords haut/bas
    int margin = 50;
    p->y = (float)(rand() % (windowHeight - 2 * margin) + margin);

    p->active = 1;
    return p;
}

// Met à jour la position. Retourne 0 si l'objet est sorti de l'écran.
int pickup_update(Pickup* p, int windowWidth) {
    if (!p || !p->active) return 0;

    // Déplacement vers la DROITE (+)
    p->x += PICKUP_SPEED;

    // Si l'objet dépasse le bord DROIT de l'écran, il est perdu
    if (p->x > windowWidth) {
        p->active = 0; // On le marque pour destruction
        return 0;      // On signale qu'il est sorti
    }
    return 1; // Toujours actif
}

// Affiche la munition
void pickup_draw(SDL_Renderer* renderer, Pickup* p) {
    if (!p || !p->active || !pickupTexture) return;

    SDL_Rect destRect = { (int)p->x, (int)p->y, p->width, p->height };
    SDL_RenderCopy(renderer, pickupTexture, NULL, &destRect);
}

// Nettoie la mémoire de l'objet
void pickup_destroy(Pickup* p) {
    if (p) free(p);
}