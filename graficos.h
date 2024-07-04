#ifndef GRAFICOS_H
#define GRAFICOS_H
#include "SDL.h"

#define PANTALLA_ANCHO  700
#define PANTALLA_ALTO  700

#define DELAY58FPS 18

/*
  Objetivo: sirve para crear la ventana
  Parametros: directorio de la imagen que se mostrara
  Salida: -1: error, 1: correcto
*/
int VentanaCrear(char* fileName);
/*
  Objetivo: sirve para cerrar la ventana
  Parametros: nada
  Salida: nada
*/
void VentanaCerrar();
/*
  Objetivo: sirve para cambiar el color con el que se dibujaran las formas
  Parametros: nada
  Salida: nada
*/
void DibujarColorDelLapiz(int red, int green, int blue);
void DibujarPunto(int x, int y);
void DibujarLinea(int x1, int y1, int x2, int y2);
//void zirkuluaMarraztu(int x, int y, int r);
/*
  Objetivo: sirve para limpiar toda la pantalla
  Parametros: nada
  Salida: nada
*/
void PantallaLimpiar();
/*
  Objetivo: sirve para mostrar lo guardado en los bufferes de la pantalla
  Parametros: debug: para mostrar los bordes de las colisiones
  Salida: nada
*/
void PantallaActualizar(int debug);

/*
  Objetivo: sirve para dibujar una unica imagen (cargarla en buffer)
  Parametros: nada
  Salida: nada
*/
int ImagenDibujarUna(SDL_Texture* texture, SDL_Rect *pDest, int portada);


SDL_Renderer* getRenderer(void);
void VentanaCambiarNombre(char* nombre);
int PantallaConInput(void);


#endif


