#pragma once
#include "Scene.h"
#include "InputManager.h"
#include <SDL_ttf.h>

class MenuScene : public Scene {
	TTF_Font* fuenteMenu = nullptr;
	SDL_Texture* texturaTitulo = nullptr;
	SDL_Texture* texturaInstruccion = nullptr;

	// Busca alguna de las fuentes instaladas en Windows para usar en el menú
	bool IntentarCargarFuente() {
		static const char* rutasFuentes[] = {
			"C:/Windows/Fonts/consola.ttf",
			"C:/Windows/Fonts/arial.ttf",
			"C:/Windows/Fonts/cour.ttf",
		};
		for (const char* ruta : rutasFuentes) {
			fuenteMenu = TTF_OpenFont(ruta, 24);
			if (fuenteMenu != nullptr) {
				return true;
			}
		}
		return false;
	}

	// Inicializa las texturas de texto si no están listas
	void AsegurarTexturas(SDL_Renderer* rend) {
		if (rend == nullptr) return;
		if (fuenteMenu == nullptr) {
			IntentarCargarFuente();
		}
		if (fuenteMenu == nullptr) return;

		if (texturaTitulo == nullptr) {
			SDL_Color colorTitulo{ 255, 255, 255, 255 }; // Blanco
			SDL_Surface* surf = TTF_RenderUTF8_Blended(fuenteMenu, "ASTEROIDS - MENU PRINCIPAL", colorTitulo);
			if (surf != nullptr) {
				texturaTitulo = SDL_CreateTextureFromSurface(rend, surf);
				SDL_FreeSurface(surf);
			}
		}

		if (texturaInstruccion == nullptr) {
			SDL_Color colorInstruccion{ 200, 200, 200, 255 }; // Gris
			SDL_Surface* surf = TTF_RenderUTF8_Blended(fuenteMenu, "PULSA 'P' PARA EMPEZAR A JUGAR", colorInstruccion);
			if (surf != nullptr) {
				texturaInstruccion = SDL_CreateTextureFromSurface(rend, surf);
				SDL_FreeSurface(surf);
			}
		}
	}

	// Limpieza de texturas y fuentes
	void LiberarRecursos() {
		if (texturaTitulo != nullptr) {
			SDL_DestroyTexture(texturaTitulo);
			texturaTitulo = nullptr;
		}
		if (texturaInstruccion != nullptr) {
			SDL_DestroyTexture(texturaInstruccion);
			texturaInstruccion = nullptr;
		}
		if (fuenteMenu != nullptr) {
			TTF_CloseFont(fuenteMenu);
			fuenteMenu = nullptr;
		}
	}

public:
	MenuScene() : Scene() {}

	~MenuScene() override {
		LiberarRecursos();
	}

	void Start(SDL_Renderer* rend) override {
		Scene::Start(rend);
		LiberarRecursos();
	}

	void Update(float dt) override {
		// Tecla P para arrancar el juego
		if (IM.GetKey(SDLK_p, DOWN)) {
			finalizada = true;
			escenaDestino = "Gameplay";
		}
	}

	void Render(SDL_Renderer* rend) override {
		if (rend == nullptr) return;
		AsegurarTexturas(rend);

		// Limpiar fondo con tono azul oscuro agradable
		SDL_SetRenderDrawColor(rend, 10, 10, 20, 255);
		SDL_RenderClear(rend);

		int anchoVentana = 800, altoVentana = 600;
		SDL_GetRendererOutputSize(rend, &anchoVentana, &altoVentana);

		// Centrar título arriba
		if (texturaTitulo != nullptr) {
			int tw = 0, th = 0;
			SDL_QueryTexture(texturaTitulo, nullptr, nullptr, &tw, &th);
			SDL_Rect dst{ (anchoVentana - tw) / 2, altoVentana / 3, tw, th };
			SDL_RenderCopy(rend, texturaTitulo, nullptr, &dst);
		}

		// Centrar instrucción abajo
		if (texturaInstruccion != nullptr) {
			int tw = 0, th = 0;
			SDL_QueryTexture(texturaInstruccion, nullptr, nullptr, &tw, &th);
			SDL_Rect dst{ (anchoVentana - tw) / 2, (altoVentana * 2 / 3), tw, th };
			SDL_RenderCopy(rend, texturaInstruccion, nullptr, &dst);
		}
	}

	void Exit() override {
		LiberarRecursos();
	}

};