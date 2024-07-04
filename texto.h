#ifndef TEXTO_H
#define TEXTO_H

#define MAX_STRLEN 255

/*
  Objetivo: sirve para inicializar el texto
  Parametros: tama�o: el tama�o que tendran las letras. letra: path a la letra a usar (.\\Fonts\\letra.ttf)
  Salida: -1: error. 1: texto inicializado
*/
int TextoAbrir(int tama�o, char* letra);
/*
  Objetivo: sirve para cambiar el tama�o del texto
  Parametros: tama�o: el tama�o que tendran las letras. letra: path a la letra a usar (.\\Fonts\\letra.ttf)
  Salida: nada
*/
void TextoCambiarTama�o(int tama�o, char* letra);
/*
  Objetivo: sirve para escribir texto en la pantalla
  Parametros: x: coordenada x. y: coordenada y. str: texto a escribir (max 255 char)
  Salida: nada
*/
void TextoEscribir(int x, int y, char *str);
/*
  Objetivo: sirve para escribir texto en la pantalla, pero caracter a caracter
  Parametros: nadax: coordenada x. y: coordenada y. tiempo: tiempo a esperar entre letra y letra. str: texto a escribir (max 255 char)
  Salida: nada
*/
void TextoEscribirLento(int x, int y, int tiempo, char *str);
/*
  Objetivo: sirve para cambiar el color del texto (rgb 0-255)
  Parametros: r: valor rojo. g: valor verde. b: valor azul
  Salida: nada
*/
void TextoCambiarColor(int r, int g, int b);
/*
  Objetivo: sirve para ver cual es el color del texto actual. El color se devuelve con los punteros de los parametros
  Parametros: r: valor rojo. g: valor verde. b: valor azul
  Salida: nada
*/
void TextoColorActual(int* r, int* g, int* b);
/*
  Objetivo: sirve para ver cual es el tama�o del texto
  Parametros: nada
  Salida: el tama�o del texto
*/
int TextoTama�oActual();
/*
  Objetivo: sirve para ver cual es la letra activa
  Parametros: nada
  Salida: el directorio de la letra cargada
*/
char* TextoLetraActual();

/*
  Objetivo: sirve para cerrar el texto
  Parametros: nada
  Salida: nada
*/
void TextoCerrar(void);

#endif
