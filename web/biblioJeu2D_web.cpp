// Version web (Emscripten / WebAssembly) de biblioJeu2D.
// Meme interface que biblioJeu2D.h : main.cpp n'a pas besoin de changer.
// La fenetre SFML est remplacee par un <canvas> HTML (id "canvas"),
// et le clavier par des ecouteurs JavaScript definis dans index.html.

#include "../biblioJeu2D.h"

#include <emscripten.h>
#include <vector>

static const int CAPACITE_GRILLE{ 100 };

static int largeurGrille{ 0 };
static int hauteurGrille{ 0 };
static std::vector<unsigned char> pixels; // RGBA, une valeur par cellule
static double dernierTempsMs{ 0.0 };

// Prepare le canvas : un pixel par cellule, agrandi par le CSS (image-rendering: pixelated).
EM_JS(void, js_initialiserCanvas, (int largeur, int hauteur, const char* titre), {
    const canvas = document.getElementById('canvas');
    canvas.width = largeur;
    canvas.height = hauteur;
    canvas.setAttribute('aria-label', UTF8ToString(titre));
    if (Module.onFenetreInitialisee) Module.onFenetreInitialisee();
});

// Copie le tampon RGBA de la grille dans le canvas.
EM_JS(void, js_dessiner, (const unsigned char* donnees, int largeur, int hauteur), {
    const canvas = document.getElementById('canvas');
    const ctx = canvas.getContext('2d');
    const image = new ImageData(new Uint8ClampedArray(HEAPU8.buffer, donnees, largeur * hauteur * 4).slice(), largeur, hauteur);
    ctx.putImageData(image, 0, 0);
});

// Rend la main au navigateur jusqu'a la prochaine image (comme display() en SFML).
// Grace a ASYNCIFY, la boucle while de main.cpp peut rester telle quelle.
EM_ASYNC_JS(void, js_attendreProchaineImage, (), {
    await new Promise(resolve => requestAnimationFrame(resolve));
});

// Etat des fleches, mis a jour par index.html (clavier et glissements tactiles).
// "appuisRecents" garde un appui tres bref (entre deux images) jusqu'a ce qu'il soit lu.
EM_JS(int, js_flecheAppuyee, (int code), {
    const enfoncee = Module.fleches && Module.fleches[code];
    const recente = Module.appuisRecents && Module.appuisRecents[code];
    if (recente) Module.appuisRecents[code] = false;
    return (enfoncee || recente) ? 1 : 0;
});

bool InitialiserFenetre(int largeur, int hauteur, const std::string& titreFenetre)
{
    if (largeur > CAPACITE_GRILLE || hauteur > CAPACITE_GRILLE)
    {
        return false;
    }

    largeurGrille = largeur;
    hauteurGrille = hauteur;
    pixels.assign(static_cast<size_t>(largeur) * hauteur * 4, 0);
    for (size_t i = 3; i < pixels.size(); i += 4)
    {
        pixels[i] = 255; // opaque, noir par defaut
    }

    js_initialiserCanvas(largeur, hauteur, titreFenetre.c_str());
    dernierTempsMs = emscripten_get_now();
    return true;
}

void ColorerCellule(int x, int y, int r, int g, int b)
{
    if (x < 0 || x >= largeurGrille || y < 0 || y >= hauteurGrille)
    {
        return;
    }
    const size_t i = (static_cast<size_t>(y) * largeurGrille + x) * 4;
    pixels[i] = static_cast<unsigned char>(r);
    pixels[i + 1] = static_cast<unsigned char>(g);
    pixels[i + 2] = static_cast<unsigned char>(b);
    pixels[i + 3] = 255;
}

bool FenetreEstActive()
{
    if (pixels.empty())
    {
        return false;
    }
    js_dessiner(pixels.data(), largeurGrille, hauteurGrille);
    js_attendreProchaineImage();
    return true;
}

int TempsEcouleMicrosecondes()
{
    const double maintenant = emscripten_get_now();
    const double ecouleMs = maintenant - dernierTempsMs;
    dernierTempsMs = maintenant;
    return static_cast<int>(ecouleMs * 1000.0);
}

// Codes partages avec index.html : 0 = haut, 1 = bas, 2 = gauche, 3 = droite.
bool FlecheHautEstAppuyee() { return js_flecheAppuyee(0) != 0; }
bool FlecheBasEstAppuyee() { return js_flecheAppuyee(1) != 0; }
bool FlecheGaucheEstAppuyee() { return js_flecheAppuyee(2) != 0; }
bool FlecheDroiteEstAppuyee() { return js_flecheAppuyee(3) != 0; }
