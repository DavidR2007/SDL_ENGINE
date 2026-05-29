#pragma once
#include "GameObject.h"
#include <vector>
#include <string>

class Scene {
protected:
	std::vector<GameObject*> objetos; // Objetos activos en la escena
	bool finalizada = false;          // Flag para cambiar de escena
	std::string escenaDestino;        // Nombre de la escena siguiente

public:
	virtual ~Scene() {
		// Limpieza de objetos para evitar fugas de memoria
		for (GameObject* o : objetos) {
			delete o;
		}
		objetos.clear();
	}

	virtual void Start(SDL_Renderer *rend) {
		for (GameObject* o : objetos) {
			delete o;
		}
		objetos.clear();
		finalizada = false;
	}

	virtual void Update(float dt) {
		for (GameObject* var : objetos) {
			if (var != nullptr) {
				var->Update(dt);
			}
		}
	}

	virtual void Render(SDL_Renderer* rend) {
		for (GameObject* var : objetos) {
			if (var != nullptr && rend != nullptr) {
				var->Render(rend);
			}
		}
	}

	virtual void Exit() = 0;

	bool EstaFinalizada() { return finalizada; }
	std::string ObtenerEscenaDestino() { return escenaDestino; }

};