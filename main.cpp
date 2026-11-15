#include <clocale>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>

#include "biblioJeu2D.h"

using namespace std;

// ----------------------------------------------------------------------------
// Paramètres du jeu
// ----------------------------------------------------------------------------

constexpr int LARGEUR{ 30 };                    // en cellules
constexpr int HAUTEUR{ 20 };                    // en cellules
constexpr int NB_CELLULES{ LARGEUR * HAUTEUR };

constexpr int LONGUEUR_INITIALE{ 3 };
constexpr int DELAI_MOUVEMENT_US{ 250'000 };    // le serpent avance toutes les 250 ms
constexpr int DELAI_CROISSANCE_US{ 1'000'000 }; // et grandit d'une case chaque seconde

// ----------------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------------

struct Couleur
{
    int r;
    int g;
    int b;
};

struct Position
{
    int x;
    int y;
};

struct Segment
{
    Position position;
    Couleur couleur;
};

struct Serpent
{
    deque<Segment> corps;       // corps.front() est la tête
    Position direction;         // direction du dernier déplacement
    Position directionDemandee; // choisie par le joueur, appliquée au prochain déplacement
};

constexpr Couleur NOIR{ 0, 0, 0 };
constexpr Couleur BLANC{ 255, 255, 255 };

constexpr Position HAUT{ 0, -1 };
constexpr Position BAS{ 0, 1 };
constexpr Position GAUCHE{ -1, 0 };
constexpr Position DROITE{ 1, 0 };

// ----------------------------------------------------------------------------
// Utilitaires
// ----------------------------------------------------------------------------

Couleur CouleurAleatoire()
{
    return { rand() % 256, rand() % 256, rand() % 256 };
}

Position DirectionAleatoire()
{
    const Position directions[]{ HAUT, BAS, GAUCHE, DROITE };
    return directions[rand() % 4];
}

// Ramène une coordonnée dans [0, taille - 1] : le serpent passe d'un bord à l'autre.
int Envelopper(int valeur, int taille)
{
    return (valeur % taille + taille) % taille;
}

Position Envelopper(Position position)
{
    return { Envelopper(position.x, LARGEUR), Envelopper(position.y, HAUTEUR) };
}

bool SontOpposees(Position a, Position b)
{
    return a.x == -b.x && a.y == -b.y;
}

bool SontEgales(Position a, Position b)
{
    return a.x == b.x && a.y == b.y;
}

// Indique si le délai est écoulé et ne garde que le reste,
// pour ne pas enchaîner plusieurs déplacements après une longue pause.
bool DelaiEcoule(int& tempsAccumule, int delai)
{
    if (tempsAccumule < delai)
    {
        return false;
    }
    tempsAccumule = (tempsAccumule - delai) % delai;
    return true;
}

// ----------------------------------------------------------------------------
// Logique du jeu (aucun affichage ici)
// ----------------------------------------------------------------------------

void InitialiserSerpent(Serpent& serpent)
{
    serpent.direction = DirectionAleatoire();
    serpent.directionDemandee = serpent.direction;
    serpent.corps.clear();

    const Position tete{ rand() % LARGEUR, rand() % HAUTEUR };
    for (int i{ 0 }; i < LONGUEUR_INITIALE; ++i)
    {
        const Position position{ tete.x - i * serpent.direction.x, tete.y - i * serpent.direction.y };
        const Couleur couleur{ i == 0 ? BLANC : CouleurAleatoire() };
        serpent.corps.push_back({ Envelopper(position), couleur });
    }
}

int Longueur(const Serpent& serpent)
{
    return static_cast<int>(serpent.corps.size());
}

// La tête reste blanche ; le reste du corps change de couleur à chaque déplacement.
void ChangerCouleurs(Serpent& serpent)
{
    for (size_t i{ 1 }; i < serpent.corps.size(); ++i)
    {
        serpent.corps[i].couleur = CouleurAleatoire();
    }
}

void DeplacerSerpent(Serpent& serpent)
{
    serpent.direction = serpent.directionDemandee;

    const Position tete{ serpent.corps.front().position };
    const Position nouvelleTete{ Envelopper({ tete.x + serpent.direction.x, tete.y + serpent.direction.y }) };

    serpent.corps.push_front({ nouvelleTete, BLANC });
    serpent.corps.pop_back();
    ChangerCouleurs(serpent);
}

// Le nouveau segment apparaît sur la queue et s'en détache au déplacement suivant.
void AgrandirSerpent(Serpent& serpent)
{
    if (Longueur(serpent) < NB_CELLULES)
    {
        serpent.corps.push_back({ serpent.corps.back().position, CouleurAleatoire() });
    }
}

bool CollisionAvecSoi(const Serpent& serpent)
{
    const Position tete{ serpent.corps.front().position };
    for (size_t i{ 1 }; i < serpent.corps.size(); ++i)
    {
        if (SontEgales(tete, serpent.corps[i].position))
        {
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
// Entrées
// ----------------------------------------------------------------------------

// Retient la première flèche appuyée qui n'est pas un demi-tour.
// La comparaison se fait avec le dernier déplacement réel : deux appuis rapides
// (par exemple Haut puis Droite en allant vers la gauche) ne font pas demi-tour.
void LireDirection(Serpent& serpent)
{
    const bool appuyees[]{ FlecheHautEstAppuyee(), FlecheBasEstAppuyee(),
                           FlecheGaucheEstAppuyee(), FlecheDroiteEstAppuyee() };
    const Position directions[]{ HAUT, BAS, GAUCHE, DROITE };

    for (int i{ 0 }; i < 4; ++i)
    {
        if (appuyees[i] && !SontOpposees(directions[i], serpent.direction))
        {
            serpent.directionDemandee = directions[i];
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// Affichage
// ----------------------------------------------------------------------------

void ColorerCellule(Position position, Couleur couleur)
{
    ColorerCellule(position.x, position.y, couleur.r, couleur.g, couleur.b);
}

void AfficherJeu(const Serpent& serpent)
{
    for (int x{ 0 }; x < LARGEUR; ++x)
    {
        for (int y{ 0 }; y < HAUTEUR; ++y)
        {
            ColorerCellule({ x, y }, NOIR);
        }
    }

    for (const Segment& segment : serpent.corps)
    {
        ColorerCellule(segment.position, segment.couleur);
    }
}

void AfficherFinDePartie(int score)
{
    cout << "------------------------------------------------\n"
         << "             Game Over! Score: " << score
         << "\n------------------------------------------------" << endl;
}

// ----------------------------------------------------------------------------
// Boucle de jeu
// ----------------------------------------------------------------------------

int main()
{
    setlocale(LC_ALL, "");
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!InitialiserFenetre(LARGEUR, HAUTEUR, "Jeu Snake"))
    {
        cout << "Erreur lors de l'initialisation de la fenêtre !" << endl;
        return EXIT_FAILURE;
    }

    Serpent serpent;
    InitialiserSerpent(serpent);
    int tempsDepuisDernierMouvement{ 0 };
    int tempsDepuisDerniereCroissance{ 0 };

    while (FenetreEstActive())
    {
        const int deltaTemps{ TempsEcouleMicrosecondes() };
        tempsDepuisDernierMouvement += deltaTemps;
        tempsDepuisDerniereCroissance += deltaTemps;

        LireDirection(serpent);

        if (DelaiEcoule(tempsDepuisDernierMouvement, DELAI_MOUVEMENT_US))
        {
            DeplacerSerpent(serpent);
            if (CollisionAvecSoi(serpent))
            {
                AfficherFinDePartie(Longueur(serpent));
                break;
            }
        }

        if (DelaiEcoule(tempsDepuisDerniereCroissance, DELAI_CROISSANCE_US))
        {
            AgrandirSerpent(serpent);
        }

        AfficherJeu(serpent);
    }

    return EXIT_SUCCESS;
}
