#ifndef IMAGEN_H
#define IMAGEN_H

#define MAX_IMG 4 //Como maximo se puede asignar hasta n-1


/*
  Objetivo: sirve para inicializar el array de imagenes
  Parametros: filename: una imagen cualquiera, no importa cual se ponga (creo)
  Salida: -1: error, sino pos
*/
void ImagenInit(char* fileName);

/*
  Objetivo: sirve para cargar una nueva imagen. 1: portada, 2: playPause
  Parametros: filename: directorio de la imagen. pos: altura de la imagen
  Salida: -1: error, sino pos
*/
int ImagenCargar(char* fileName, int pos, int portada);
/*
  Objetivo: sirve para mover una imagen cargada
  Parametros: pos: altura de la imagen. x: destino x. y: destino y
  Salida: nada
*/
void ImagenMover(int pos, int x, int y);
/*
  Objetivo: sirve para dibujar todas las imagenes (cargarlas en el buffer)
  Parametros: nada
  Salida: nada
*/
void ImagenDibujarTodas(void);
/*
  Objetivo: sirve para descargar una imagen
  Parametros: nada
  Salida: nada
*/
void ImagenDescargar(int pos);
/*
  Objetivo: sirve para descargar todas las imagenes
  Parametros: nada
  Salida: nada
*/
void ImagenDescargarTodas(void);

#endif
