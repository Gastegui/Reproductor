#ifndef SONIDO_H
#define SONIDO_H

//#define MAX_SOUNDS 100

#include <SDL_mixer.h>
#include "eventos.h"

typedef struct S_SONIDOS
{
	char nombre[30];
	Mix_Chunk* sonido;
	struct S_SONIDOS* siguiente;
}SONIDOS;

/*
  Objetivo: sirve para inicializar el audio
  Parametros: nada
  Salida: 0: todo bien, 1: error
*/
int AbrirAudio();
/*
  Objetivo: sirve para cerrar el audio
  Parametros: nada
  Salida: nada
*/
void CerrarAudio();

/*
  Objetivo: sirve para cargar musica y reproducirla
  Parametros: filename: directorio de la cancion
  Salida: 1: cambio correcto. 0: nombre incorrecto
*/
int CargarMusica(const char *fileName);
/*
  Objetivo: sirve para reproducir la cancion guardada
  Parametros: nada
  Salida: 1: musica reproduciendose. 0: no hay musica guardada
*/
int ReproducirMusica(void);
/*
  Objetivo: sirve para poner en pause o play la musica
  Parametros: nada
  Salida: nada
*/
void PlayPauseMusica(void);
/*
  Objetivo: sirve para parar la musica
  Parametros: nada
  Salida: nada
*/
void PararMusica(void);
/*
  Objetivo: sirve para cambiar el volumen de la musica
  Parametros: vol: volumen deseado (0 a 8 inclusivos)
  Salida: 1: volumen valido. 0: volumen invalido
*/
int CambiarVolumenMusica(int vol);
/*
  Objetivo: sirve para ver el voluemn de la musica
  Parametros: nada
  Salida: volumen de la musica
*/
int LeerVolumenMusica(void);


int MusicaTiempoActual();
int MusicaTiempoTotal();
void MusicaNombre(char* salida);
void DescargarMusica(void);
Mix_Music* GetMixMusic(void);


#endif