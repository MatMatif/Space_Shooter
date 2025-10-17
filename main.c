#include <SDL2/SDL.h>
#include <stdio.h>

// Définir des constantes pour la taille de la fenêtre est une bonne pratique.
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // --- INITIALISATION ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur d'initialisation de la SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Space Shooter",                    // Titre de la fenêtre
        SDL_WINDOWPOS_CENTERED,             // Position x centrée
        SDL_WINDOWPOS_CENTERED,             // Position y centrée
        WINDOW_WIDTH,                       // Largeur
        WINDOW_HEIGHT,                      // Hauteur
        SDL_WINDOW_SHOWN                    // Flags (fenêtre visible)
    );

    if (window == NULL) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // On crée un "renderer", c'est notre "pinceau" pour dessiner dans la fenêtre.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- BOUCLE DE JEU PRINCIPALE ---
    int running = 1; // Un "flag" pour savoir si le jeu doit continuer à tourner.
    SDL_Event event; // Une variable pour stocker les événements (clavier, souris, etc.)

    while (running) {
        // 1. GESTION DES ÉVÉNEMENTS
        // On vérifie tous les événements qui se sont produits depuis la dernière frame.
        while (SDL_PollEvent(&event)) {
            // Si l'utilisateur clique sur la croix pour fermer la fenêtre...
            if (event.type == SDL_QUIT) {
                running = 0; // On met le flag à 0 pour quitter la boucle de jeu.
            }
        }

        // 2. LOGIQUE DU JEU (Mises à jour)
        // (Pour l'instant, il n'y a rien ici, mais c'est là que vous mettriez
        //  le code pour déplacer le vaisseau, les tirs, etc.)

        // 3. RENDU GRAPHIQUE
        // On choisit une couleur de fond (un bleu nuit pour un jeu spatial !)
        SDL_SetRenderDrawColor(renderer, 16, 20, 35, 255);
        // On efface tout l'écran avec cette couleur.
        SDL_RenderClear(renderer);

        // (C'est ici que vous mettriez le code pour dessiner le vaisseau,
        //  les ennemis, les étoiles, etc.)

        // On affiche le résultat à l'écran.
        SDL_RenderPresent(renderer);
    }

    // --- NETTOYAGE ET FERMETURE ---
    // On détruit les objets dans l'ordre inverse de leur création.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}