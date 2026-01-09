# 🚀 Space Shooter - C / SDL2

![Status](https://img.shields.io/badge/Status-Finished-success)
![Language](https://img.shields.io/badge/Language-C-blue)
![Library](https://img.shields.io/badge/Library-SDL2-orange)
![Build](https://img.shields.io/badge/Build-CMake-green)

Un jeu de tir spatial rétro (Shoot 'em up) développé en **C** avec la bibliothèque **SDL2**.  
Le but est de survivre le plus longtemps possible, de gérer ses munitions et de faire le meilleur score !

---

## 🎮 Démo du jeu

![Gameplay Demo](assets/demo.gif)

*(Si le GIF ne s'affiche pas, assurez-vous d'avoir ajouté un fichier .gif dans votre dossier assets)*

---

## ✨ Fonctionnalités

* **Gameplay Fluide :** Moteur bridé à **60 FPS** pour une fluidité constante.
* **Système de Munitions :** Vous n'avez pas de tirs infinis ! Ramassez les **caisses de munitions** 📦 qui traversent l'écran pour recharger.
* **Ennemis & Explosions :** Vagues d'ennemis infinies avec animations d'explosion.
* **Interface (HUD) :** Affichage en temps réel de la vie ❤️, des munitions 🔫 et du score 🏆.
* **Background Animé :** Défilement infini (Parallax) et animations.
* **Audio :** Effets sonores (tirs, explosions) et musique d'ambiance.
* **Architecture Modulaire :** Code structuré proprement (`.c` / `.h`) pour chaque entité (Player, Enemy, Background, etc.).

---

## 🕹️ Commandes

| Touche | Action |
| :---: | :--- |
| **Flèches** | Déplacer le vaisseau (Haut, Bas, Gauche, Droite) |
| **Espace** | Tirer |
| **Entrée** | Démarrer le jeu (depuis le Menu) |
| **Echap** | Retour au Menu |

---

## 🛠️ Installation et Compilation

### Prérequis
Vous devez avoir installé :
* Un compilateur C (GCC ou Clang)
* **CMake**
* La bibliothèque **SDL2** et ses modules :
    * `SDL2`
    * `SDL2_image`
    * `SDL2_ttf`
    * `SDL2_mixer`

### Compilation (Linux / Mac)

1.  Clonez le projet :
    ```bash
    git clone [https://github.com/VOTRE_NOM/Space-Shooter.git](https://github.com/VOTRE_NOM/Space-Shooter.git)
    cd Space-Shooter
    ```

2.  Créez le dossier de build et compilez :
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

3.  Lancez le jeu :
    ```bash
    ./Space_Shooter
    ```

