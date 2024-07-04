#ifndef EVENTOS_H
#define EVENTOS_H
#include "SDL.h"

typedef struct S_POSICION
{
  int x;
  int y;
} POSICION;

typedef struct S_EVENTO
{
	int evento;
	int segundo;
	int ctrl;
	int alt;
	int shift;
	int clickIzquierdoPulsado;
	int teclaDown;
	int teclaUp;
	SDL_Event original;
}EVENTO;

/*
  Objetivo: detecta que se está pulsando en el momento de la llamada
  Parametros: nada
  Salida: la tecla pulsada
*/
EVENTO Evento(void);
/*
  Objetivo: sirve para leer la posicion del raton
  Parametros: nada
  Salida: la posicion del raton
*/
POSICION PosRaton();
/*
  Objetivo: sirve para vaciar la cola de eventos acumulada
  Parametros: nada
  Salida: nada
*/
void VaciarEventos(void);

#define CERRAR 12
#define RATON_MOVIMIETO  4
#define RATON_BOTON_DERECHO 5
#define RATON_BOTON_IZQUIERDO 6
#define RATON_BOTON_IZQUIERDO_DOWN 7
#define RATON_SCROLL_ARRIBA 278
#define RATON_SCROLL_ABAJO 279
#define RATON_SCROLL_IZQUIERDA 280
#define RATON_SCROLL_DERECHA 281

#define TECLA_BORRAR 8
#define TECLA_RETURN 13
#define TECLA_ESCAPE 27
#define TECLA_ESPACIO 32
#define TECLA_PUNTO 46
#define TECLA_0 48
#define TECLA_1 49
#define TECLA_2 50
#define TECLA_3 51
#define TECLA_4 52
#define TECLA_5 53
#define TECLA_6 54
#define TECLA_7 55
#define TECLA_8 56
#define TECLA_9 57
#define TECLA_a 97
#define TECLA_b 98
#define TECLA_c 99
#define TECLA_d 100
#define TECLA_e 101
#define TECLA_f 102
#define TECLA_g 103
#define TECLA_h 104
#define TECLA_i 105
#define TECLA_i_UP 1050
#define TECLA_j 106
#define TECLA_k 107
#define TECLA_k_UP 1070
#define TECLA_l 108
#define TECLA_l_UP 1080
#define TECLA_m 109
#define TECLA_n 110
#define TECLA_o 111
#define TECLA_o_UP 1110
#define TECLA_p 112
#define TECLA_q 113
#define TECLA_r 114
#define TECLA_s 115
#define TECLA_t 116
#define TECLA_u 117
#define TECLA_v 118
#define TECLA_w 119
#define TECLA_x 120
#define TECLA_y 121
#define TECLA_z 122
#define FLECHA_ARRIBA 273
#define FLECHA_ABAJO 274
#define FLECHA_DERECHA 275
#define FLECHA_IZQUIERDA 276
#define TECLA_TAB 277

#endif
