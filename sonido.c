#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sonido.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "eventos.h"

void DescargarMusica(void);

Mix_Music* musica=NULL;
char musicaFileName[255] = { "" };

extern int ctrl;
unsigned int siguienteCambioDeVolumen = 0, cooldownCambioVolumen = 250;
int volumenMusica = 8, volumenSonidos = 8;

int AbrirAudio()
{
	Mix_Init(MIX_INIT_MP3);
	SDL_Init(SDL_INIT_AUDIO);
	if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4086) < 0) 
	{
		printf("SDL_mixer no se ha podido abrir, error: %s\n", Mix_GetError());
		return 1;
	}

	return 0;
}


int CargarMusica(const char *fileName)
{
	if (musicaFileName == fileName && strcmp(musicaFileName, "") != 0)
		return 1;

	strcpy(musicaFileName, fileName);

	if (musica != NULL)
	{
		Mix_HaltMusic();
		//Mix_FreeMusic(musica);
	}
	if ((musica = Mix_LoadMUS(fileName)) == NULL) 
		return 0;
	
	return 1;
}

int ReproducirMusica(void)
{
	if (musica != NULL)
	{
		Mix_PlayMusic(musica, 0);
		return 1;
	}
	return 0;
}

void PlayPauseMusica(void)
{
	if (musica != NULL)
	{
		if (Mix_PlayingMusic() != 0)
		{
			if (Mix_PausedMusic()) Mix_ResumeMusic();
			else Mix_PauseMusic();
		}
		else ReproducirMusica();
	}
}

void DescargarMusica(void)
{
	if (musica != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(musica);
	}
}

void CerrarAudio(void)
{
	DescargarMusica();
	Mix_Quit();
}

void PararMusica(void)
{
	Mix_HaltMusic();
}

int CambiarVolumenMusica(int vol)
{
	if (vol < 0 || vol > 8)
		return 0;

	volumenMusica = vol;
	
	Mix_VolumeMusic(16 * vol);
	return 1;
}

int LeerVolumenMusica(void)
{
	return Mix_VolumeMusic(-1) / 16;
}

int MusicaTiempoActual()
{
	return (int)Mix_GetMusicPosition(musica);
}

int MusicaTiempoTotal()
{
	return (int)Mix_MusicDuration(musica);
}

void MusicaNombre(char* salida)
{
	const char* nombre = Mix_GetMusicTitle(musica);
	const char* artista = Mix_GetMusicArtistTag(musica);
	char ret[1024] = { "" };

	if (strcmp(artista, "") == 0)
		strcpy(ret, "Desconocido - ");
	else
	{
		strcpy(ret, artista);
		strcat(ret, " - ");
	}

	if (strcmp(nombre, "") == 0)
		strcat(ret, "Desconocido");
	else
		strcat(ret, nombre);

	strcpy(salida, ret);
}

Mix_Music* GetMixMusic(void)
{
	return musica;
}