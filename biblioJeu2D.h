#pragma once

#include <string>

// Petite bibliothèque d'affichage 2D en grille. Le jeu ne dépend que de cette interface :
// biblioJeu2D.cpp l'implémente avec SFML, web/biblioJeu2D_web.cpp avec un canvas HTML.

// Ouvre la fenêtre. La taille de la grille est en nombre de cellules (gros pixels),
// entre 1 et 100 dans chaque direction. Produit false si la taille est invalide.
bool InitialiserFenetre(int largeurNbCellules, int hauteurNbCellules, const std::string& titreFenetre);

// x est dans l'intervalle [0, largeurNbCellules - 1]
// y est dans l'intervalle [0, hauteurNbCellules - 1]
// r, g et b sont dans l'intervalle [0, 255]
// Une cellule hors de la grille est ignorée.
void ColorerCellule(int x, int y, int r, int g, int b);

// À appeler une fois par image (chaque itération de jeu) : traite les événements,
// affiche les cellules colorées à l'écran et indique si la fenêtre est encore ouverte.
bool FenetreEstActive();

// Produit le nombre de microsecondes écoulées depuis le dernier appel à la routine.
int TempsEcouleMicrosecondes();

// Indiquent si les flèches du clavier sont appuyées.
bool FlecheHautEstAppuyee();
bool FlecheBasEstAppuyee();
bool FlecheDroiteEstAppuyee();
bool FlecheGaucheEstAppuyee();
