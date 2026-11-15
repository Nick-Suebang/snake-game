#!/bin/sh
# Compile le jeu en WebAssembly. Resultat : docs/snake.js + docs/snake.wasm
#
# Avec Docker (rien a installer), depuis la racine du depot :
#   docker run --rm -v "$(pwd):/src" -w /src emscripten/emsdk:3.1.74 sh web/build.sh
#
# Ou avec Emscripten installe localement (emsdk) :
#   sh web/build.sh
set -e

em++ main.cpp web/biblioJeu2D_web.cpp \
  -std=c++17 -O2 \
  -o docs/snake.js \
  -sASYNCIFY \
  -sINVOKE_RUN=0 \
  -sEXIT_RUNTIME=0 \
  -sEXPORTED_RUNTIME_METHODS=callMain \
  -sENVIRONMENT=web

echo "OK : docs/snake.js et docs/snake.wasm generes."
