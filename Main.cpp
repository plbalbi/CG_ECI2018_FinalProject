// trabajo.cpp : Defines the entry point for the console application.
//

#include "Renderer.h"

#define _DEBUG

int main() {
	
	Renderer* engine = new Renderer();

	//engine->Initialize();
	engine->Render();

	return 0;
}