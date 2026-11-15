#include "biblioJeu2D.h"

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

namespace
{
    constexpr int CAPACITE_GRILLE{ 100 };
    constexpr float TAILLE_CELLULE{ 30.f };
    const sf::Color COULEUR_VIDE{ sf::Color::Black };

    int largeurGrille{ 0 };
    int hauteurGrille{ 0 };
    std::unique_ptr<sf::RenderWindow> fenetre;
    std::vector<sf::Color> couleurs; // une couleur par cellule, indice y * largeurGrille + x
    sf::Clock horloge;

    bool EstDansLaGrille(int x, int y)
    {
        return x >= 0 && x < largeurGrille && y >= 0 && y < hauteurGrille;
    }

    // Produit false si l'utilisateur a fermé la fenêtre.
    bool TraiterEvenements()
    {
        sf::Event evenement;
        while (fenetre->pollEvent(evenement))
        {
            if (evenement.type == sf::Event::Closed)
            {
                fenetre->close();
                return false;
            }
        }
        return true;
    }

    void DessinerGrille()
    {
        sf::RectangleShape cellule(sf::Vector2f(TAILLE_CELLULE, TAILLE_CELLULE));
        for (int y{ 0 }; y < hauteurGrille; ++y)
        {
            for (int x{ 0 }; x < largeurGrille; ++x)
            {
                cellule.setPosition(x * TAILLE_CELLULE, y * TAILLE_CELLULE);
                cellule.setFillColor(couleurs[y * largeurGrille + x]);
                fenetre->draw(cellule);
            }
        }
    }
}

bool InitialiserFenetre(int largeur, int hauteur, const std::string& titreFenetre)
{
    if (largeur <= 0 || hauteur <= 0 || largeur > CAPACITE_GRILLE || hauteur > CAPACITE_GRILLE)
    {
        return false;
    }

    largeurGrille = largeur;
    hauteurGrille = hauteur;
    couleurs.assign(largeur * hauteur, COULEUR_VIDE);

    const sf::VideoMode taille(static_cast<unsigned int>(largeur * TAILLE_CELLULE),
                               static_cast<unsigned int>(hauteur * TAILLE_CELLULE));
    fenetre = std::make_unique<sf::RenderWindow>(taille, titreFenetre);
    horloge.restart();
    return true;
}

void ColorerCellule(int x, int y, int r, int g, int b)
{
    if (EstDansLaGrille(x, y))
    {
        couleurs[y * largeurGrille + x] = sf::Color(r, g, b);
    }
}

bool FenetreEstActive()
{
    if (!fenetre || !fenetre->isOpen() || !TraiterEvenements())
    {
        return false;
    }

    fenetre->clear(COULEUR_VIDE);
    DessinerGrille();
    fenetre->display();
    return true;
}

int TempsEcouleMicrosecondes()
{
    return static_cast<int>(horloge.restart().asMicroseconds());
}

bool FlecheHautEstAppuyee() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Up); }
bool FlecheBasEstAppuyee() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Down); }
bool FlecheDroiteEstAppuyee() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Right); }
bool FlecheGaucheEstAppuyee() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Left); }
