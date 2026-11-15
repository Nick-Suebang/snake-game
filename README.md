# Projet Snake

## Description
Implémentation du jeu Snake en C++ avec la bibliothèque SFML. Le serpent avance tout seul, grandit d'une case chaque seconde et passe d'un bord à l'autre de la grille. La partie se termine quand il se mord la queue ; le score est sa longueur finale.

**Jouer en ligne :** https://nick-suebang.github.io/snake-game/

## Contrôles
- Flèches directionnelles : changer de direction (un demi-tour est ignoré)
- Fermer la fenêtre : quitter

## Prérequis
- Visual Studio 2022 (C++14 ou supérieur)
- SFML 2.5.1, installée automatiquement par NuGet (`packages.config`)

## Compilation
1. Ouvrir `Snake.sln` dans Visual Studio 2022
2. Sélectionner la configuration (Debug/Release) et la plateforme x64
3. Compiler et exécuter (F5 pour déboguer, Ctrl+F5 pour exécuter)

## Structure du projet
```
snake-game/
├── main.cpp                 # Logique du jeu, entrées, affichage et boucle de jeu
├── biblioJeu2D.h            # Interface d'affichage en grille utilisée par le jeu
├── biblioJeu2D.cpp          # Implémentation de cette interface avec SFML
├── web/
│   ├── biblioJeu2D_web.cpp  # Implémentation pour le navigateur (canvas HTML)
│   └── build.sh             # Compilation en WebAssembly
└── docs/                    # Version web publiée sur GitHub Pages
```

## Architecture
- `main.cpp` ne connaît que l'interface `biblioJeu2D.h`, jamais SFML directement. Changer de technologie d'affichage revient donc à fournir une autre implémentation de cette interface, ce qu'a fait la version web.
- Dans `main.cpp`, la logique du jeu (déplacer, grandir, détecter les collisions) est séparée des entrées et de l'affichage.

## Version web (WebAssembly)
Le même code C++ est compilé en WebAssembly avec [Emscripten](https://emscripten.org) pour être joué directement dans le navigateur, sans rien installer.

- `main.cpp` est partagé entre la version Visual Studio et la version web.
- `web/biblioJeu2D_web.cpp` remplace `biblioJeu2D.cpp` (SFML) : il dessine dans un `<canvas>` HTML et lit le clavier ou les glissements tactiles.
- `docs/` contient la page prête à publier : `index.html`, `snake.js` et `snake.wasm`.

### Recompiler
Avec Docker, depuis la racine du dépôt :
```bash
docker run --rm -v "$(pwd):/src" -w /src emscripten/emsdk:3.1.74 sh web/build.sh
```

### Tester en local
La page doit être servie par un serveur web (ouvrir le fichier directement ne charge pas le `.wasm`) :
```bash
npx http-server docs
```

### Publier
Avec GitHub Pages : Settings → Pages → Source « Deploy from a branch », branche `main`, dossier `/docs`.
Pour l'intégrer à un portfolio :
```html
<iframe src="https://nick-suebang.github.io/snake-game/" width="920" height="720" style="border:0"></iframe>
```

## Auteur
Nick Suebang

## Licence
Ce projet est distribué sous licence MIT.
