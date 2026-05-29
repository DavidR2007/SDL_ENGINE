#include "../GameEngine.h"

int SDL_main(int argc, char* argv[]) {
	GameEngine engine(800,800);

	engine.Update();
	engine.Finish();

	return 0;
}