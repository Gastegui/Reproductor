#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"

#include "graficos.h"
#include "imagen.h"

typedef struct S_IMG
{
	SDL_Texture* texture;
	SDL_Rect dest;
	int portada;
}IMG;

IMG imagenes[MAX_IMG];

//Esto en verdad no hace falta, pero por si acaso está
void ImagenInit(char* fileName)
{
	for (int i = 0; i < MAX_IMG; i++)
		imagenes[i].texture = NULL;
}

int ImagenCargar(char* fileName, int pos, int portada)
{
	SDL_Texture* img;
	SDL_Renderer* gRenderer = getRenderer();
	if (imagenes[pos].texture != NULL)
	{
		printf("Se ha intentado colocar una imagen en una posicion ocupada (%d). Se procede a sobreescribir la anterior...\n", pos);
		ImagenDescargar(pos);
	}

	if (pos < MAX_IMG)
	{
		//surface = SDL_LoadBMP(fileName);
		img = IMG_LoadTexture(gRenderer, fileName);
		if (img == NULL)
		{
			if(portada != 1)
				fprintf(stderr, "No se pudo abrir %s: %s\n", fileName, SDL_GetError());
			return -1;
		}
		else
		{
			int w, h;

			SDL_QueryTexture(img, NULL, NULL, &w, &h);

			imagenes[pos].texture = img;
			imagenes[pos].dest.x = imagenes[pos].dest.y = 0;
			imagenes[pos].dest.w = w;
			imagenes[pos].dest.h = h;
			imagenes[pos].portada = portada;
		}
	}
	else
	{
		printf("La posicion indicada es demasiado grande.\n");
		return -1;
	}

	return pos;
}

void ImagenMover(int pos, int x, int y)
{
	imagenes[pos].dest.x = x;
	imagenes[pos].dest.y = y;
}

void ImagenDibujarTodas(void)
{
	for (int i = 0; i < MAX_IMG; i++)
	{
		if (imagenes[i].texture != NULL)
		{
			ImagenDibujarUna(imagenes[i].texture, &imagenes[i].dest, imagenes[i].portada);
		}
	}
}

void ImagenDescargar(int pos)
{
	if (imagenes[pos].texture != NULL)
	{
		SDL_DestroyTexture(imagenes[pos].texture);
	
		imagenes[pos].texture = NULL;
	}
}

void ImagenDescargarTodas(void)
{
	for (int i = 0; i < MAX_IMG; i++)
	{
		ImagenDescargar(i);
	}
}