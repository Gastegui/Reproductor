#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GRAPHICS

#include "SDL_ttf.h"
#include "SDL_image.h"

#include "imagen.h"
#include "graficos.h"

SDL_Window* window = NULL;
SDL_Renderer* gRenderer;
int colorR, colorG, colorB;

SDL_Renderer* getRenderer(void) { return gRenderer; }

int VentanaCrear(char *fileName)
{
	int ret = 1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "No se ha podido abrir SLD, error: %s\n", SDL_GetError());
		return -1;
	}
	
	atexit(SDL_Quit);
	
	window = SDL_CreateWindow("Reproductor mp3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PANTALLA_ANCHO, PANTALLA_ALTO, SDL_WINDOW_SHOWN);
	
	if (window == NULL)
	{
		fprintf(stderr, "No se ha podido crear la ventana: %s\n", SDL_GetError());
		return -1;
	}
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

	SDL_SetWindowIcon(window, IMG_Load(fileName));

	TTF_Init();
	ImagenInit(fileName);
	atexit(TTF_Quit);
	return ret;
}

void VentanaCambiarNombre(char* nombre)
{
	SDL_SetWindowTitle(window, nombre);
}

void VentanaCerrar()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void DibujarColorDelLapiz(int red, int green, int blue)
{
	colorR = red;
	colorG = green;
	colorB = blue;
}


void DibujarLinea(int x1, int y1, int x2, int y2)
{
	SDL_SetRenderDrawColor(gRenderer, colorR, colorG, colorB, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(gRenderer, x1, y1, x2, y2);
}

void DibujarPunto(int x, int y)
{
	SDL_SetRenderDrawColor(gRenderer, colorR, colorG, colorB, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawPoint(gRenderer, x, y);
}

int ImagenDibujarUna(SDL_Texture* texture, SDL_Rect *pDest, int portada)
{
	SDL_Rect src = *pDest;
	SDL_Rect dest = *pDest;
	src.x = 0;
	src.y = 0;
	src.w = pDest->w;
	src.h = pDest->h;
	if (portada == 1)
	{
		dest.w /= 3;
		dest.h /= 3;
		SDL_RenderCopy(gRenderer, texture, &src, &dest);
	}
	else
	{
		SDL_RenderCopy(gRenderer, texture, &src, pDest);
	}
	return 0;
}

void PantallaLimpiar()
{
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gRenderer);
}


void PantallaActualizar(int debug)
{
	SDL_RenderPresent(gRenderer);
}

int PantallaConInput(void)
{
	return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) == 512;
}