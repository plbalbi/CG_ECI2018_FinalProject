// trabajo.cpp : Defines the entry point for the console application.
//

#include "Renderer.h"

int main() {
	
	Renderer* engine = new Renderer();

	Renderer::self = engine;

	//engine->Initialize();
	engine->Render();

	return 0;
}