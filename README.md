Absolument ! Voici un tutoriel complet au format Markdown (`.md`). Vous pouvez l'enregistrer dans un fichier `TUTORIEL.md` à la racine de votre projet pour ne jamais l'oublier.

Ce guide récapitule toutes les étapes que nous avons suivies pour créer un environnement de développement C + SDL2 robuste et professionnel sur Windows avec CLion.

---

# Tutoriel : Configurer un Projet C avec SDL2, CLion et Git sur Windows

Ce guide explique comment mettre en place un environnement de développement C pour la bibliothèque graphique SDL2 en utilisant CLion, le gestionnaire de paquets MSYS2, et Git pour le contrôle de version.

Cette méthode est la plus fiable car elle garantit que le compilateur et les bibliothèques sont parfaitement compatibles, évitant ainsi la majorité des erreurs de compilation et de liaison.

## Phase 1 : Préparation de l'Environnement (MSYS2)

C'est l'étape la plus importante. On installe une base saine pour tout notre développement.

1.  **Installer MSYS2**
    *   Téléchargez et installez MSYS2 depuis le site officiel : [https://www.msys2.org/](https://www.msys2.org/)

2.  **Mettre à jour MSYS2**
    *   Ouvrez le terminal **"MSYS2 MINGW64"** depuis le menu Démarrer.
    *   Lancez la commande de mise à jour. C'est crucial pour que le système connaisse toutes les bibliothèques disponibles.
      ```bash
      pacman -Syu
      ```
    *   Il est possible que le terminal vous demande de le fermer. Si c'est le cas, fermez-le, rouvrez-le, et **relancez la même commande `pacman -Syu`** pour terminer la mise à jour.

3.  **Installer le Compilateur et SDL2**
    *   Toujours dans le terminal MINGW64, installez la chaîne de compilation MinGW 64-bit et la bibliothèque SDL2. `pacman` s'occupe de tout pour vous.
      ```bash
      # Installe le compilateur (gcc, g++, etc.)
      pacman -S --needed mingw-w64-x86_64-toolchain

      # Installe la bibliothèque SDL2 (version stable)
      pacman -S mingw-w64-x86_64-SDL2
      ```

## Phase 2 : Configuration de l'IDE (CLion)

On indique à CLion d'utiliser le compilateur puissant que nous venons d'installer.

1.  Ouvrez CLion, puis allez dans `File` -> `Settings` -> `Build, Execution, Deployment` -> `Toolchains`.
2.  Cliquez sur le `+` pour ajouter une nouvelle toolchain.
3.  CLion devrait détecter automatiquement le chemin `C:\msys64\mingw64`. Sélectionnez cette option.
4.  Nommez cette nouvelle toolchain (ex: "MinGW MSYS2").
5.  **Faites-en la toolchain par défaut** en la faisant glisser en haut de la liste.
6.  Cliquez sur `Apply` et `OK`.

## Phase 3 : Création du Projet

Maintenant, on écrit le code de base et le script qui dit à CLion comment le compiler.

1.  **Le fichier `main.c`**
    *   Créez un fichier `main.c` avec ce code de démarrage. Il initialise SDL, crée une fenêtre, attend 3 secondes, et se ferme proprement.
    ```c
    #include <SDL2/SDL.h>
    #include <stdio.h>

    int main(int argc, char* argv[]) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            printf("Erreur d'initialisation de la SDL: %s\n", SDL_GetError());
            return 1;
        }

        SDL_Window* window = SDL_CreateWindow(
            "Mon Jeu SDL2",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            800,
            600,
            SDL_WINDOW_SHOWN
        );

        if (window == NULL) {
            printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }

        SDL_Delay(3000); // Pause de 3 secondes

        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    ```

2.  **Le fichier `CMakeLists.txt`**
    *   C'est le fichier de configuration du projet. Il doit contenir ceci :
    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(Space_Shooter C)

    set(CMAKE_C_STANDARD 11)

    # Demande à CMake de trouver la bibliothèque SDL2 que pacman a installée.
    find_package(SDL2 REQUIRED)

    # Crée l'exécutable en mode "application graphique Windows".
    add_executable(Space_Shooter WIN32 main.c)

    # Lie notre projet à SDL2. On a besoin de :
    # - SDL2::SDL2main : Gère le point d'entrée spécial (WinMain) pour Windows.
    # - SDL2::SDL2 : La bibliothèque SDL2 principale.
    target_link_libraries(Space_Shooter PRIVATE SDL2::SDL2main SDL2::SDL2)
    ```

## Phase 4 : La Touche Finale (La DLL)

Un programme SDL a besoin d'un fichier `.dll` pour fonctionner. C'est la bibliothèque dynamique.

1.  **Copiez la DLL** :
    *   Allez dans `C:\msys64\mingw64\bin\`.
    *   Trouvez et copiez le fichier `SDL2.dll`.
2.  **Collez la DLL** :
    *   Collez ce fichier dans le dossier de compilation de votre projet, qui est par défaut `cmake-build-debug`. Votre fichier `Space_Shooter.exe` et `SDL2.dll` doivent être dans le même dossier.

3.  **Compiler et Lancer**
    *   Dans CLion, cliquez sur `Build` -> `Rebuild Project`.
    *   Cliquez sur le bouton vert "Run" ▶️ pour lancer. Une fenêtre doit apparaître !

## Phase 5 : Contrôle de Version (Git et GitHub)

Pour sauvegarder votre projet et collaborer, utilisez Git.

1.  **Créer un dépôt sur GitHub**
    *   Allez sur [GitHub](https://github.com) et créez un nouveau dépôt vide (sans `README` ni `.gitignore`). Copiez son URL HTTPS.

2.  **Initialiser Git en local**
    *   Ouvrez le terminal de CLion (`View` -> `Tool Windows` -> `Terminal`).
    ```bash
    # Initialise le dépôt Git
    git init

    # Lie votre dossier local au serveur GitHub (collez votre URL)
    git remote add origin https://github.com/VOTRE_NOM/VOTRE_PROJET.git
    ```

3.  **Créer le fichier `.gitignore`**
    *   À la racine de votre projet, créez un fichier nommé `.gitignore`. Il est **crucial** pour ne pas sauvegarder les fichiers inutiles. Mettez-y ce contenu :
    ```gitignore
    # Dossiers de build de CMake
    cmake-build-*/

    # Dossier de l'IDE CLion
    .idea/
    ```

4.  **Faire le premier Commit et Push**
    ```bash
    # Ajoute tous les fichiers (sauf ceux ignorés) au suivi
    git add .

    # Crée la première sauvegarde (le commit)
    git commit -m "Premier commit : Initialisation du projet SDL2"

    # Envoie le code vers GitHub
    # Note : Votre branche locale peut être 'master'. Adaptez si besoin.
    git push --set-upstream origin main
    ```

5.  **Authentification avec un Jeton (PAT)**
    *   Git vous demandera votre nom d'utilisateur et votre mot de passe. **N'utilisez pas votre mot de passe GitHub !**
    *   Allez sur GitHub > `Settings` > `Developer settings` > `Personal access tokens` > `Tokens (classic)`.
    *   Générez un **nouveau jeton** en cochant la permission **`repo`**.
    *   Copiez ce jeton (`ghp_...`) et **collez-le à la place du mot de passe** dans le terminal.

---

Vous avez maintenant un projet C + SDL2 fonctionnel, professionnel et versionné avec Git. Bon développement 
