#include <time.h>
#include "eventos.h"

POSICION posRaton = {0,0};
SDL_Event event;

POSICION PosRaton() { return posRaton;}

int ctrl = 0, alt = 0, shift = 0;
int clickIzquierdoPulsado = 0;
int teclaDown = 0;
int teclaUp = 0;

extern int escribiendo;

EVENTO Evento(void){
	int ret = 0;
	teclaDown = 0;
	teclaUp = 0;
	EVENTO retEvento = {0};
	if( SDL_PollEvent( &event ))
	{
		//Mientras no escribe
		if (escribiendo == 0)
		{
			if (teclaUp == 1 || teclaDown == 1)
			{
				teclaUp = 0;
				teclaDown = 0;
			}
			switch( event.type )
			{
				case SDL_KEYDOWN:
					teclaDown = 1;
					switch(event.key.keysym.sym)
					{
						case SDLK_LEFT:
							ret = FLECHA_IZQUIERDA;
							break;
						case SDLK_RIGHT:
							ret = FLECHA_DERECHA;
							break;
						case SDLK_DOWN:
							ret = FLECHA_ABAJO;	
							break;
						case SDLK_UP:
							ret = FLECHA_ARRIBA;
							break;
						case SDLK_RCTRL:
						case SDLK_LCTRL:
							ctrl = 1;
							break;
						case SDLK_RALT:
						case SDLK_LALT:
							alt = 1;
							break;
						case SDLK_RSHIFT:
						case SDLK_LSHIFT:
							shift = 1;
							break;
						case SDLK_TAB:
							ret = TECLA_TAB;
							break;
						case SDLK_SPACE:
							ret = TECLA_ESPACIO;
							break;
						default:
							ret = event.key.keysym.sym;
							break;
					}
					break;
				case SDL_KEYUP:
					teclaUp = 1;
					switch (event.key.keysym.sym)
					{
						case SDLK_RCTRL:
						case SDLK_LCTRL:
							ctrl = 0;
							break;
						case SDLK_RALT:
						case SDLK_LALT:
							alt = 0;
							break;
						case SDLK_RSHIFT:
						case SDLK_LSHIFT:
							shift = 0;
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					ret = CERRAR;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						clickIzquierdoPulsado = 1;
						ret = RATON_BOTON_IZQUIERDO_DOWN;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					switch ( event.button.button ){
						case SDL_BUTTON_LEFT:
							clickIzquierdoPulsado = 0;
							ret = RATON_BOTON_IZQUIERDO;
							break;
						case SDL_BUTTON_RIGHT:
							ret = RATON_BOTON_DERECHO;
							break;
						default:
							ret = event.button.button;
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					//Si el return es RATON_MOVIMIENTO la cosa es llamar a PosRaton() para sacar la posicion del raton
					posRaton.x = event.motion.x;
					posRaton.y =event.motion.y;
					ret = RATON_MOVIMIETO;
					break;
				case SDL_MOUSEWHEEL:
					if(event.wheel.y > 0)
						ret = RATON_SCROLL_ARRIBA;
					else if (event.wheel.y < 0)
						ret = RATON_SCROLL_ABAJO;
					else if (event.wheel.x > 0)
						ret = RATON_SCROLL_DERECHA;
					else if (event.wheel.x < 0)
						ret = RATON_SCROLL_IZQUIERDA;
					break;
				default:
					break;
			}
		}
		//Mientras escribe
		else
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					teclaDown = 1;
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
						case SDLK_KP_ENTER:
							ret = TECLA_RETURN;
							break;
						case SDLK_SPACE:
							ret = TECLA_ESPACIO;
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
					teclaUp = 1;
					switch (event.key.keysym.sym)
					{
					case SDLK_RCTRL:
					case SDLK_LCTRL:
						ctrl = 0;
						break;
					case SDLK_RALT:
					case SDLK_LALT:
						alt = 0;
						break;
					case SDLK_RSHIFT:
					case SDLK_LSHIFT:
						shift = 0;
						break;
					default:
						break;
					}
					break;
				case SDL_QUIT:
					ret = CERRAR;
					break;
				case SDL_MOUSEBUTTONUP:
					switch (event.button.button)
					{
						case SDL_BUTTON_RIGHT:
							ret = RATON_BOTON_DERECHO;
						case SDL_BUTTON_LEFT:
							ret = RATON_BOTON_IZQUIERDO;
							break;
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					//Si el return es RATON_MOVIMIENTO la cosa es llamar a PosRaton() para sacar la posicion del raton
					posRaton.x = event.motion.x;
					posRaton.y = event.motion.y;
					ret = RATON_MOVIMIETO;
					break;
				default:
					break;
			}
		} 
	}
	retEvento.evento = ret;
	retEvento.original = event;
	retEvento.segundo = (int)clock() / CLOCKS_PER_SEC;
	retEvento.ctrl = ctrl;
	retEvento.alt = alt;
	retEvento.shift = shift;
	retEvento.clickIzquierdoPulsado = clickIzquierdoPulsado;
	retEvento.teclaUp = teclaUp;
	retEvento.teclaDown = teclaDown;
	return retEvento;
}

void VaciarEventos(void)
{

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_MOUSEMOTION)
		{
			posRaton.x = event.motion.x;
			posRaton.y = event.motion.y;
		}
	}

	return;
}