// trabajo.cpp : Defines the entry point for the console application.
//

#include "Renderer.h"
#include <iostream>

#define _DEBUG

int main() {
	
	Renderer engine = Renderer();

	engine.Initialize();
	engine.Render();

	return 0;
}
