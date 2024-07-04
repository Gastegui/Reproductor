#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "SDL_TTF.h"
#include "SDL.h"

#include "texto.h"
#include "graficos.h"
#include "imagen.h"
#include "eventos.h"

TTF_Font *font=0;
int tamañoActual;
char letraActual[50];
int escribiendo = 0;
SDL_Color color = { 255, 255, 255 };
SDL_Color colorNegro = { 0,0,0 };

int TextoAbrir(int tamaño, char* letra)
{
	font=TTF_OpenFont(letra, tamaño);
	strcpy(letraActual, letra);
	if(!font) 
	{
		printf("No se ha podido cargar la fuente de la letra, error: %s\n", TTF_GetError());
		return -1;
	}
	tamañoActual = tamaño;
	return 1;
}

void TextoCambiarTamaño(int tamaño, char* letra)
{
	if (font != 0)
	{
		strcpy(letraActual, letra);
		TextoCerrar();
		font = TTF_OpenFontIndex(letra, tamaño, 0);
		if (!font)
		{
			printf("No se ha podido cargar la fuente de la letra, error: %s\n", TTF_GetError());
		}
	}
	else
		printf("Se ha intentado cambiar el tamaño del texto sin abrirlo antes.");

	tamañoActual = tamaño;
}

void TextoEscribir(int x, int y, char *str)
{
	SDL_Surface* textSurface;
	SDL_Texture *mTexture;
	SDL_Rect dst = {0};
	SDL_Renderer* gRenderer;

	if (font == 0) 
		return;
	if (strlen(str) > MAX_STRLEN)
		str = "ERROR: El texto es demasiado largo.";
	gRenderer = getRenderer();
	textSurface = TTF_RenderText_Blended(font, str, color);
	mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	dst.x = x;
	dst.y = y;
	dst.w = textSurface->w;
	dst.h = textSurface->h;
	SDL_RenderCopy(gRenderer, mTexture, NULL, &dst);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(mTexture);
}

void TextoEscribirLento(int x, int y, int tiempo, char* str)
{
	SDL_Surface* textSurface;
	SDL_Texture* mTexture;
	SDL_Rect dst = { 0 };
	SDL_Renderer* gRenderer;
	EVENTO evento = {0};

	unsigned int siguienteLetra = 0;
	char str2[MAX_STRLEN] = { "" };

	if (font == 0)
		return;
	gRenderer = getRenderer();

	if (strlen(str) > MAX_STRLEN)
	{
		str = "ERROR: El texto es demasiado largo.";
		textSurface = TTF_RenderText_Blended(font, str, color);
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		dst.x = x;
		dst.y = y;
		dst.w = textSurface->w;
		dst.h = textSurface->h;
		SDL_RenderCopy(gRenderer, mTexture, NULL, &dst);
		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(mTexture);
	}
	else
	{
		escribiendo = 1;
		siguienteLetra = SDL_GetTicks();
		for (size_t i = 0; i < strlen(str); i++)
		{
			while (siguienteLetra > SDL_GetTicks())
			{
			
			}
			ImagenDibujarTodas();
			evento = Evento();
			if (evento.evento == TECLA_RETURN || evento.evento == TECLA_ESPACIO || evento.evento == RATON_BOTON_IZQUIERDO)
				break;
			siguienteLetra += tiempo;
			str2[i] = str[i];
			str2[i + 1] = '\0';
			textSurface = TTF_RenderText_Blended(font, str2, color);
			mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			dst.x = x;
			dst.y = y;
			dst.w = textSurface->w;
			dst.h = textSurface->h;
			SDL_RenderCopy(gRenderer, mTexture, NULL, &dst);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(mTexture);
			PantallaActualizar(0);
		}

		if (evento.evento == TECLA_RETURN || evento.evento == TECLA_ESPACIO || evento.evento == RATON_BOTON_IZQUIERDO)
		{
			TextoEscribir(x, y, str);
			PantallaActualizar(0);
		}

		escribiendo = 0;
	}

}

void TextoCerrar(void)
{
	if (font != 0) TTF_CloseFont(font);
	font = 0;
}

void TextoCambiarColor(int r, int g, int b)
{
	color.r = r;
	color.g = g;
	color.b = b;
}

void TextoColorActual(int* r, int* g, int* b)
{
	*r = (int) color.r;
	*g = (int) color.g;
	*b = (int) color.b;
}

int TextoTamañoActual()
{
	return tamañoActual;
}

char* TextoLetraActual()
{
	return letraActual;
}