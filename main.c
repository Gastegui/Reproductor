#if defined(_DEBUG)
    #define _CRT_SECURE_NO_WARNINGS
    #define _CRTDBG_MAP_ALLOC
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>

#if defined(_DEBUG)
    #include <crtdbg.h>
#endif

#include <stdio.h>
#include <windows.h>
#include <time.h>

#include "graficos.h"
#include "eventos.h"
#include "sonido.h"
#include "texto.h"
#include "imagen.h"

char PATH_ICONO[100] = { "./icono.png" };
char PATH_TECLAS_TXT[100] = { "./teclas.txt" };
char PATH_TECLAS_EXE[100] = { "./teclas.exe" };
char PATH_IMAGENES_EXE[100] = { "./imagenes.exe" };
char PATH_IMAGEN_JPG[100] = { "./imagen.jpg" };
char PATH_IMAGEN_DEFECTO_PNG[100] = { "./imagenDefecto.png" };
char PATH_SANS_SERIF[100] = { "./Fonts/Sans Serif.ttf" };
char PATH_FAVORITOS[100] = { "./favoritos.txt" };
char PATH_FAVORITOS_TMP[100] = { "./favoritosTMP.txt" };
char PATH_ESTADISTICAS[100] = { "./Estadisticas.txt" };
char PATH_ESTADISTICAS_TMP[100] = { "./EstadisticasTMP.txt" };
char PATH_MUSICA[100] = { "./Music/" };
char PATH_IMAGEN_PAUSE[100] = { "./Pause.png" };
char PATH_IMAGEN_PLAY[100] = { "./Play.png" };
char PATH_PLAYLISTS[100] = { "./Music/Playlists/" };

int TIEMPO_CANCION_ANTERIOR = 5;

typedef struct S_CANCIONES 
{
    char nombre[2048];
    char album[255];
    int numero;
    int favorito;
    int duracion;
    struct S_CANCIONES* siguiente;
}CANCIONES;

typedef struct S_REPRODUCIENDO
{
    char nombre[255];
    char path[2048];
    char album[255];
    int favorito;
    int numero;
    int duracion;
    int siguiente; //0: No reproducir nada al acabar. 1: Reproducir siguiente al acabar
    int bucle; //0: No hacer nada al acabar la cola. 1: Cola en bucle. 2: Reproducir la misma cancion en bucle
    int reproducciones;
    int omisiones;
    int repeticiones;
}REPRODUCIENDO;

typedef struct S_MENSAJE
{
    char mensaje[255];
    unsigned int borrar;
}MENSAJE;
MENSAJE mensaje = { 0 };

int Iniciar(void);
CANCIONES* CancionesAñadirAlFinal(CANCIONES* canciones, CANCIONES *_nuevo);
CANCIONES* CancionesQuitarPrimera(CANCIONES* canciones);
void Siguiente(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int bucle, int* pos, int total);
void Anterior(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int bucle, int* pos, int total);
void CancionesBorrar(CANCIONES* canciones);
void MusicaReproducir(CANCIONES* cancion, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, CANCIONES* cola, int* pos, int total);
CANCIONES* Aleatorizar(CANCIONES* disponibles, CANCIONES* cola, REPRODUCIENDO* reproduciendo, int total, int* scroll, int texto);
int randInt(int n);
int gettimeofday(struct timeval* tp, struct timezone* tzp);
void MostrarCola(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int* pos, int total, int mover);
void Numero(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int numero, int* pos, int total);
void CerrarTeclas(void);
void AbrirTeclas(int primero);
void AsignarFavorito(CANCIONES* cancion, CANCIONES* favoritos);
CANCIONES* ComprobarFavoritoPlayList(CANCIONES* cancion, CANCIONES* favoritos);
CANCIONES* QuitarCancion(CANCIONES* canciones, char* nombreAQuitar);
CANCIONES* AñadirFavorito(CANCIONES* disponibles, CANCIONES* cola, CANCIONES* favoritos, REPRODUCIENDO* reproduciendo);
CANCIONES* QuitarFavorito(CANCIONES* disponibles, CANCIONES* cola, CANCIONES* favoritos, REPRODUCIENDO* reproduciendo);
void GuardarFavoritos(CANCIONES* favoritos, char* dir);
CANCIONES* OrdenarDisponibles(CANCIONES* disponibles, int como);
CANCIONES* ReiniciarCola(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo);
void EstadisticasCambiar(REPRODUCIENDO* reproduciendo, int cambio);
void EstadisticasCargar(REPRODUCIENDO* reproduciendo);
int VentanaError(char* error);

int mostrarCola;
int main(int argc, char* argv[])
{
    printf("ABIERTO\n");
#if defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    EVENTO evento;
    int salir = 0;
    int ret = 0;
    char tmp[2048] = { "" };
    int q; //descarte

    CANCIONES* disponibles = NULL;
    CANCIONES* cola = NULL;
    CANCIONES* favoritos = NULL;
    CANCIONES cancionTMP = { 0 };

    REPRODUCIENDO reproduciendo = { 0 };
    reproduciendo.numero = -1;
    reproduciendo.bucle = 1;
    reproduciendo.siguiente = 1;

    int total = 0;
    FILE* fp = NULL;
    FILE* fp2 = NULL;
    char dir[2048] = { "" };
    WIN32_FIND_DATAA fdFile;
    HANDLE hFind = NULL;
    char sPath[2048];
    int nombreValido = 0;
    int soloFavoritas = 0;
    char playList[2048] = { "" };
    strcpy(playList, "nada");

    int pos = 0;
    POSICION posRaton = { 0 };
    int scroll = 0;
    float posClick;

    int teclasCerrado = 0;
    unsigned long segundos = 0;
    unsigned long segundosAnterior = (unsigned long)time(NULL);

    int leer = 1;
    int aleatorizado = 0;
    int orden = 1;

    int volumen = MIX_MAX_VOLUME;

    if (argc == 1)
    {
        printf("No se ha introducido ningun argumento\n");
        fgets(tmp, 255, stdin);
        return -1;
    }

    fp = fopen(strcat(getenv("USERPROFILE"), "/Documents/ReproductorConfig.txt"), "r");
    if (fp != NULL)
    {
        while (fgets(sPath, 2048, fp) != NULL)
        {
            fgets(tmp, 2048, fp);
            if (sPath[strlen(sPath) - 1] == '\n')
                sPath[strlen(sPath) - 1] = '\0';
            if (tmp[strlen(tmp) - 1] == '\n')
                tmp[strlen(tmp) - 1] = '\0';
            if (sPath[strlen(sPath) - 1] == ' ')
                sPath[strlen(sPath) - 1] = '\0';
            if (tmp[strlen(tmp) - 1] == ' ')
                tmp[strlen(tmp) - 1] = '\0';

            if (strcmp(sPath, "PATH_ICONO") == 0)
                strcpy(PATH_ICONO, tmp);
            else if (strcmp(sPath, "PATH_TECLAS_TXT") == 0)
                strcpy(PATH_TECLAS_TXT, tmp);
            else if (strcmp(sPath, "PATH_TECLAS_EXE") == 0)
                strcpy(PATH_TECLAS_EXE, tmp);
            else if (strcmp(sPath, "PATH_IMAGEN_JPG") == 0)
                strcpy(PATH_IMAGEN_JPG, tmp);
            else if (strcmp(sPath, "PATH_IMAGENES_EXE") == 0)
                strcpy(PATH_IMAGENES_EXE, tmp);
            else if (strcmp(sPath, "PATH_IMAGEN_DEFECTO_PNG") == 0)
                strcpy(PATH_IMAGEN_DEFECTO_PNG, tmp);
            else if (strcmp(sPath, "PATH_SANS_SERIF") == 0)
                strcpy(PATH_SANS_SERIF, tmp);
            else if (strcmp(sPath, "PATH_FAVORITOS") == 0)
                strcpy(PATH_FAVORITOS, tmp);
            else if (strcmp(sPath, "PATH_FAVORITOS_TMP") == 0)
                strcpy(PATH_FAVORITOS_TMP, tmp);
            else if (strcmp(sPath, "PATH_ESTADISTICAS") == 0)
                strcpy(PATH_ESTADISTICAS, tmp);
            else if (strcmp(sPath, "PATH_ESTADISTICAS_TMP") == 0)
                strcpy(PATH_ESTADISTICAS_TMP, tmp);
            else if (strcmp(sPath, "PATH_MUSICA") == 0)
            {
                if(_fullpath(PATH_MUSICA, tmp, 100) == NULL)
                    strcpy(PATH_MUSICA, tmp);
                printf("Inicio del path de musica: %s\n", PATH_MUSICA);
            }
            else if (strcmp(sPath, "PATH_IMAGEN_PAUSE") == 0)
                strcpy(PATH_IMAGEN_PAUSE, tmp);
            else if (strcmp(sPath, "PATH_IMAGEN_PLAY") == 0)
                strcpy(PATH_IMAGEN_PLAY, tmp);
            else if (strcmp(sPath, "PATH_PLAYLISTS") == 0)
            {
                if(_fullpath(PATH_PLAYLISTS, tmp, 100 == NULL))
                    strcpy(PATH_PLAYLISTS, tmp);
                printf("Inicio del path de las playlists: %s\n", PATH_MUSICA);
            }
            else if (strcmp(sPath, "TIEMPO_CANCION_ANTERIOR") == 0)
                q = sscanf(tmp, "%d", &TIEMPO_CANCION_ANTERIOR);
            else
                printf("Path desconocido. Nombre: %s, path: %s\n", sPath, tmp);
            printf("Nombre: %s, tmp: %s\n", sPath, tmp);
        }
        sPath[0] = '\0';
        tmp[0] = '\0';
        fclose(fp);
    }

    strcpy(dir, argv[1]);
    printf("Dir antes   :%s\n", dir);

    if (strcmp(dir, "favoritos") != 0 && strcmp(dir, "-p") != 0)
    {
        printf("dir[1] = [%c]\n", dir[1]);
        if(dir[1] != ':')
            sprintf(dir, "%s%s", PATH_MUSICA, argv[1]);

        for (int i = 0; i < strlen(dir); i++) //Corregir el dir para que solo use un \ entre las carpetas
        {
            if (salir != i - 1)
                salir = -1;

            if ((dir[i] == '/' || dir[i] == '\\') && salir == -1)
            {
                dir[i] = '\\';
                salir = i;
            }
            else if (dir[i] == '/' || dir[i] == '\\')
            {
                memmove(&dir[i], &dir[i + 1], strlen(dir) - i);
                i--;
                salir = i;
            }
        }
        if (dir[0] <= 'z' && dir[0] >= 'a')
            dir[0] -= 32;
    }

    printf("Dir despues :%s\n", dir);

    if (AbrirAudio() != 0)
        return;

    if (argv[1][0] == '-' && argv[1][1] == 'p') //Cargar playlist y no carpeta
    {
        strcpy(playList, argv[2]);
        printf("PLAYLIST ARRIBA: %s\n", playList);
    }

    salir = 0;
    if (strcmp(dir, "favoritos") != 0 && strcmp(playList, "nada") == 0)
    {
        printf("Intentando cargar favoritos...\n");
        fp = fopen(PATH_FAVORITOS, "r");
        if (fp != NULL)
        {
            while (leer == 1 && fgets(tmp, 2048, fp) != NULL)
            {
                printf("Comprobando %s\n", tmp);
                tmp[strlen(tmp) - 1] = '\0';
                if (strcmp(tmp, dir) == 0)
                {
                    printf("Carpeta con favoritos guardados\n");
                    while (leer == 1 && fgets(tmp, 2048, fp) != NULL)
                    {
                        if (tmp[strlen(tmp) - 1] == '\n')
                            tmp[strlen(tmp) - 1] = '\0';
                        if (tmp[0] == '\t')
                            strcpy(tmp, tmp + 1);

                        if (strrchr(tmp, '.') && !strcmp(strrchr(tmp, '.'), ".mp3"))
                        {
                            printf("Cancion favorita: %s\n", tmp);

                            fp2 = fopen(tmp, "r");
                            if (fp2 != NULL)
                            {
                                strcpy(cancionTMP.nombre, tmp);
                                cancionTMP.siguiente = NULL;
                                favoritos = CancionesAñadirAlFinal(favoritos, &cancionTMP);
                                fclose(fp2);
                            }
                            else
                                salir++;
                        }
                        else
                        {
                            printf("Todos los favoritos cargados\n");
                            leer = 0;
                        }
                    }
                }
            }
            leer = 1;
            fclose(fp);
        }
        else
            printf("No hay favoritos\n");

        for (int i = 2; i < argc; i++)
        {
            if (argv[i][0] == '-' && argv[i][1] == 'f')
            {
                soloFavoritas = 1;
                break;
            }
        }

        //Leer la carpeta de canciones
#pragma region Leer carpeta
        sprintf(sPath, "%s\\*.mp3", dir);

        if ((hFind = FindFirstFileA(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
        {
            sprintf(playList, "No existe el directorio indicado: %s", dir);
            return VentanaError(playList);
        }
        do
        {
            if (strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
            {
                sprintf(sPath, "%s\\%s", dir, fdFile.cFileName);

                if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
                else
                {
                    printf("\nCancion: %s\n", sPath);

                    pos = 0;
                    nombreValido = 1;
                    while(nombreValido == 1 && sPath[pos] != '\0')
                    {
                        if ((sPath[pos] > 0 && sPath[pos] < 127) == 0)
                        {
                            printf("\tContiene un caracter no valido\n");
                            salir++;
                            nombreValido = 0;
                        }
                        pos++;
                    }
                    if (nombreValido == 1)
                    {
                        fp = fopen(sPath, "r");
                        if (fp == NULL)
                        {
                            printf("No se puede abrir el archivo\n");
                            salir++;
                        }
                        else
                        {
                            fclose(fp);
                            printf("\tCargado\n");
                            cancionTMP.numero = total;
                            strcpy(cancionTMP.nombre, sPath);
                            cancionTMP.siguiente = NULL;
                            cancionTMP.favorito = 0;
                            AsignarFavorito(&cancionTMP, favoritos);
                            
                            CargarMusica(cancionTMP.nombre);
                            strcpy(cancionTMP.album, Mix_GetMusicAlbumTag(GetMixMusic()));
                            mostrarCola = MusicaTiempoTotal();
                            cancionTMP.duracion = mostrarCola;
                            printf("\tNumero: %d\n", cancionTMP.numero);
                            printf("\tNombre: %s\n", cancionTMP.nombre);
                            printf("\tDuracion: %d\n", mostrarCola);
                            printf("\tAlbum: %s\n", cancionTMP.album);

                            if (soloFavoritas == 1)
                            {
                                if (cancionTMP.favorito == 1)
                                {
                                    total++;
                                    disponibles = CancionesAñadirAlFinal(disponibles, &cancionTMP);
                                    cola = CancionesAñadirAlFinal(cola, &cancionTMP);
                                    printf("Favorito cargado\n");
                                }
                                else
                                    printf("No se carga ya que no es favorita\n");
                            }
                            else
                            {
                                total++;
                                disponibles = CancionesAñadirAlFinal(disponibles, &cancionTMP);
                                cola = CancionesAñadirAlFinal(cola, &cancionTMP);
                            }

                        }
                    }
                }
            }
        } while (FindNextFileA(hFind, &fdFile));
        FindClose(hFind);

        if (disponibles == NULL)
            return -1;
        total--;
    
        soloFavoritas = 0;
#pragma endregion
   
    }
    else if (strcmp(dir, "favoritos") == 0 && strcmp(playList, "nada") == 0)
    {
        printf("Modo solo favoritos\n");
        soloFavoritas = 1;
        fp = fopen(PATH_FAVORITOS, "r");
        if (fp != NULL)
        {
            while (fgets(tmp, 2048, fp) != NULL)
            {
                if (tmp[0] == '\t')
                {
                    nombreValido = 1;
                    sprintf(tmp, "%s", &tmp[1]);
                    if (tmp[strlen(tmp) - 1] == '\n')
                        tmp[strlen(tmp) - 1] = '\0';

                    favoritos = disponibles;
                    teclasCerrado = 0;                              //Si esto es 0 significa que la cancion no se ha cargado de antes
                    if (favoritos != NULL)
                    {
                        aleatorizado = (int)strlen(favoritos->nombre) - 1;   //El indice del ultimo \ de favoritos
                        orden = (int)strlen(tmp) - 1;                          //El indice del ultimo \ de tmp
                        while (favoritos != NULL && teclasCerrado == 0)
                        {
                            while (aleatorizado != 0 && favoritos->nombre[aleatorizado] != '\\')
                                aleatorizado--;
                            while (orden != 0 && tmp[orden] != '\\')
                                orden--;
                            aleatorizado++;
                            orden++;
                            if (strcmp(&(favoritos->nombre[aleatorizado]), &(tmp[orden])) == 0)
                            {
                                printf("Cargando un favorito:\n");
                                printf("\tNombre: %s\n", tmp);
                                printf("\tLa cancion esta repetida, no se añade.\n");
                                printf("\tCoincidencia: %s\n", favoritos->nombre);
                                teclasCerrado = 1;
                                salir++;
                            }
                            favoritos = favoritos->siguiente;
                        }
                    }

                    if (teclasCerrado == 0)
                    {
                        fp2 = fopen(tmp, "r");
                        if (fp2 != NULL)
                        {
                            cancionTMP.numero = total;
                            strcpy(cancionTMP.nombre, tmp);
                            cancionTMP.siguiente = NULL;
                            cancionTMP.favorito = 1;
                            CargarMusica(cancionTMP.nombre);
                            strcpy(cancionTMP.album, Mix_GetMusicAlbumTag(GetMixMusic()));
                            mostrarCola = MusicaTiempoTotal();
                            cancionTMP.duracion = mostrarCola;
                            printf("Cargando un favorito:\n");
                            printf("\tDuracion: %d\n", cancionTMP.duracion);
                            printf("\tNumero: %d\n", cancionTMP.numero);
                            printf("\tNombre: %s\n", cancionTMP.nombre);
                            printf("\tAlbum: %s\n", cancionTMP.album);
                            total++;
                            disponibles = CancionesAñadirAlFinal(disponibles, &cancionTMP);
                            cola = CancionesAñadirAlFinal(cola, &cancionTMP);
                            fclose(fp2);
                        }
                        else
                            salir++;
                    }
                }
            }
            total--;
        }

        favoritos = NULL;
        if(nombreValido == 0)
        {
            return VentanaError("No hay ningun favorito cargado");
        }
    }
    else if (strcmp(playList, "nada") != 0)
    {
        printf("Cargar playlist: %s\n", playList);
        if (playList[strlen(playList) - 1] != 't')
            sprintf(playList, "%s.txt", playList);

        sprintf(dir, "%s%s", PATH_PLAYLISTS, playList);
        printf("Cargar playlist: %s\n", dir);

        fp = fopen(dir, "r");
        if (fp == NULL)
        {
            return VentanaError("La playlist indicada no existe");
        }

        while (fgets(playList, 2048, fp) != NULL)
        {
            if (playList[strlen(playList) - 1] == '\n')
                playList[strlen(playList) - 1] = '\0';

            printf("Cargando cancion antes: %s\n", playList);

            if (playList[1] != ':')
            {
                strcpy(dir, playList);
                sprintf(playList, "%s%s", PATH_MUSICA, dir);
            }

            if (playList[strlen(playList) - 1] != '3')
                sprintf(playList, "%s.mp3", playList);

            for (int i = 0; i < strlen(playList); i++) //Corregir el dir para que solo use un \ entre las carpetas
            {
                if (volumen != i - 1)
                    volumen = -1;

                if ((playList[i] == '/' || playList[i] == '\\') && volumen == -1)
                {
                    playList[i] = '\\';
                    volumen = i;
                }
                else if (playList[i] == '/' || playList[i] == '\\')
                {
                    memmove(&playList[i], &playList[i + 1], strlen(playList) - i);
                    i--;
                    volumen = i;
                }
            }
            if (playList[0] <= 'z' && playList[0] >= 'a')
                playList[0] -= 32;

            printf("Cargando cancion bien: %s\n", playList);

            fp2 = fopen(playList, "r");
            if(fp2 == NULL)
            {
                printf("\tNo se puede cargar la cancion\n");
                salir++;
            }
            else
            {
                fclose(fp2);

                cancionTMP.numero = total;
                strcpy(cancionTMP.nombre, playList);
                cancionTMP.siguiente = NULL;
                cancionTMP.favorito = 0;
                favoritos = ComprobarFavoritoPlayList(&cancionTMP, favoritos);

                CargarMusica(cancionTMP.nombre);
                strcpy(cancionTMP.album, Mix_GetMusicAlbumTag(GetMixMusic()));
                mostrarCola = MusicaTiempoTotal();
                cancionTMP.duracion = mostrarCola;
                printf("\tNumero: %d\n", cancionTMP.numero);
                printf("\tNombre: %s\n", cancionTMP.nombre);
                printf("\tDuracion: %d\n", mostrarCola);
                printf("\tAlbum: %s\n", cancionTMP.album);

                total++;
                disponibles = CancionesAñadirAlFinal(disponibles, &cancionTMP);
                cola = CancionesAñadirAlFinal(cola, &cancionTMP);
            }

        }
        fclose(fp);
        volumen = MIX_MAX_VOLUME;
    }

    if (total == 0)
    {
        return VentanaError("No se ha cargado ni una cancion");
    }

    orden = 1;
    aleatorizado = 0;
    if (salir != 0)
    {
        if (salir != 1)
            sprintf(mensaje.mensaje, "No se han podido cargar %d canciones", salir);
        else
            sprintf(mensaje.mensaje, "No se ha podido cargar una cancion");

        if(soloFavoritas == 1)
            sprintf(mensaje.mensaje, "%s (repes?)", mensaje.mensaje);
        mensaje.borrar = (unsigned int)time(NULL) + 5;
    }
    salir = 0;

    AbrirTeclas(1);

    struct timeval tm;
    gettimeofday(&tm, NULL);
    printf("\nSeed srad: %u\n", tm.tv_sec + tm.tv_usec * 1000000ul);
    srand(tm.tv_sec + tm.tv_usec * 1000000ul);

    switch (Iniciar())
    {
    case 1:
        break;
    case -1:
        salir = 1;
        ret = -1;
        break;
    case -2:
        salir = 1;
        break;
    default:
        break;
    }

    mostrarCola = 0;
    pos = -1; //Argumentos
    for (int i = 2; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != '-') //-x
        {
            if (argv[i][1] == 'a')
            {
                cola = Aleatorizar(disponibles, cola, &reproduciendo, total, &scroll, 0);
                aleatorizado = 1;
            }
            else if (argv[i][1] == 'b' && (argv[i][2] == '0' || argv[i][2] == '1' || argv[i][2] == '2'))
                reproduciendo.bucle = argv[i][2] - '0';
            else if (argv[i][1] == 's' && (argv[i][2] == '0' || argv[i][2] == '1'))
                reproduciendo.siguiente = argv[i][2] - '0';
            else if (argv[i][1] == 'v')
            {
                q = sscanf(&argv[++i][0], "%d", &volumen);
                if (volumen > MIX_MAX_VOLUME)
                    volumen = MIX_MAX_VOLUME;
                else if (volumen < 0)
                    volumen = 0;
            }
            else if (argv[i][1] != 'f' && argv[i][i] != 'p')
                printf("Argumento numero %d desconocido: %s\n", i, argv[i]);
        }
        else if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] != '-') //--x
        {
            if (strlen(argv[i]) == 3)
                pos = argv[i][2] - '0';
            else
            {
                pos = atoi(&argv[i][2]);
                if (pos > total + 1)
                {
                    strcpy(mensaje.mensaje, "El numero de la cancion solicitada es demasiado alto");
                    mensaje.borrar = (unsigned int)time(NULL) + 5;
                    pos = -1;
                }
                else
                    Numero(cola, disponibles, &reproduciendo, pos, &scroll, total);
            }
        }

    }
    if (pos == -1)
        MusicaReproducir(cola, disponibles, &reproduciendo, cola, &scroll, total);
    else
        Numero(cola, disponibles, &reproduciendo, pos, &scroll, total);
    pos = 0;
    VaciarEventos();
    Mix_VolumeMusic(volumen);
    printf("Volumen: %d\n", volumen);
    while (salir == 0)
    {
        //Input
#pragma region Input
        evento = Evento();
        if (evento.evento == CERRAR)
            salir = 1;

        posRaton = PosRaton();
        if (leer == 1)
        {
            switch (evento.evento)
            {
                case TECLA_w:
                    EstadisticasCambiar(&reproduciendo, 2);
                    Siguiente(cola, disponibles, &reproduciendo, reproduciendo.bucle, &scroll, total);
                    break;
                case TECLA_q:
                    if (MusicaTiempoActual() < TIEMPO_CANCION_ANTERIOR)
                        Anterior(cola, disponibles, &reproduciendo, reproduciendo.bucle, &scroll, total);
                    else
                    {
                        EstadisticasCambiar(&reproduciendo, 3);
                        Mix_SetMusicPosition(0);
                    }
                    break;
                case TECLA_r:
                    EstadisticasCambiar(&reproduciendo, -1);
                    EstadisticasCargar(&reproduciendo);
                case TECLA_ESPACIO:
                    PlayPauseMusica();
                    break;
                case TECLA_o:
                    reproduciendo.bucle = ((reproduciendo.bucle + 1) == 3) ? 0 : (reproduciendo.bucle + 1);
                    printf("Bucle: %d\n", reproduciendo.bucle);
                    break;
                case TECLA_p:
                    reproduciendo.siguiente = (reproduciendo.siguiente == 0) ? 1 : 0;
                    printf("Siguiente: %d\n", reproduciendo.siguiente);
                    break;
                case TECLA_a:
                    if (aleatorizado == 1)
                    {
                        aleatorizado = 0;
                        cola = ReiniciarCola(cola, disponibles, &reproduciendo);
                        strcpy(mensaje.mensaje, "Orden de cola cambiado");
                        mensaje.borrar = (unsigned int)time(NULL) + 5;
                    }
                    else
                    {
                        aleatorizado = 1;
                        cola = Aleatorizar(disponibles, cola, &reproduciendo, total, &scroll, 1);
                    }
                    MostrarCola(cola, disponibles, &reproduciendo, &scroll, total, 1);
                    break;
                case TECLA_t:
                    if (teclasCerrado == 0)
                    {
                        teclasCerrado = 1;
                        CerrarTeclas();
                    }
                    else
                    {
                        teclasCerrado = 0;
                        AbrirTeclas(0);
                    }
                    break;
                case TECLA_f:
                    if (soloFavoritas == 0)
                    {
                        if (reproduciendo.favorito == 1)
                            favoritos = QuitarFavorito(disponibles, cola, favoritos, &reproduciendo);
                        else
                            favoritos = AñadirFavorito(disponibles, cola, favoritos, &reproduciendo);
                    }
                    break;
                case FLECHA_ABAJO:
                case RATON_SCROLL_ABAJO:
                    if (posRaton.y < 370 && total > 12 && scroll != total - 11)
                        scroll++;
                    else if (posRaton.y > 370 && volumen != 0)
                    {
                        volumen -= 5;
                        if (volumen < 0)
                            volumen = 0;
                        Mix_VolumeMusic(volumen);
                    }
                    break;
                case FLECHA_ARRIBA:
                case RATON_SCROLL_ARRIBA:
                    if (posRaton.y < 370 && total > 12 && scroll != 0)
                        scroll--;
                    else if (posRaton.y > 370 && volumen != MIX_MAX_VOLUME)
                    {
                        volumen += 5;
                        if (volumen > MIX_MAX_VOLUME)
                            volumen = MIX_MAX_VOLUME;
                        Mix_VolumeMusic(volumen);
                    }
                    break;
                case RATON_BOTON_IZQUIERDO:
                    if (posRaton.y > 0 && posRaton.y < 365)
                    {
                        posClick = ((float)posRaton.y - 15) / 30;
                        posClick++;
                        if (posClick - (int)posClick > 0.5)
                            posClick = 0;
                        if (posClick != 0 && posClick < total + 2)
                        {
                            printf("pos: %f, scroll: %d, suma: %d\n", posClick, scroll, (int)posClick + scroll);
                            if (mostrarCola == 0)
                            {
                                aleatorizado = 0;
                                cola = ReiniciarCola(cola, disponibles, &reproduciendo);
                            }
                            Numero(cola, disponibles, &reproduciendo, (int)posClick + scroll, &scroll, total);
                        }
                    }
                    if (posRaton.x >= 250 && posRaton.x <= 370 && posRaton.y >= 670 && posRaton.y < 690) //Boton Siguiente
                    {
                        reproduciendo.siguiente = (reproduciendo.siguiente == 0) ? 1 : 0;
                    }
                    if (posRaton.x >= 400 && posRaton.x <= 480 && posRaton.y >= 670 && posRaton.y < 690) //Boton Bucle
                    {
                        reproduciendo.bucle = ((reproduciendo.bucle + 1) == 3) ? 0 : (reproduciendo.bucle + 1);
                    }
                    break;
                case RATON_BOTON_DERECHO:
                    printf("x: %d y: %d\n", posRaton.x, posRaton.y);
                    break;
                case TECLA_c:
                    mostrarCola = mostrarCola == 1 ? 0 : 1;
                    MostrarCola(cola, disponibles, &reproduciendo, &scroll, total, 1);
                    break;
                case TECLA_1:
                case TECLA_2:
                case TECLA_3:
                case TECLA_4:
                case TECLA_5:
                case TECLA_6:
                    disponibles = OrdenarDisponibles(disponibles, evento.evento-48);
                    cola = ReiniciarCola(cola, disponibles, &reproduciendo);
                    aleatorizado = 0;
                    orden = evento.evento - 48;
                    strcpy(mensaje.mensaje, "Orden de cola cambiado");
                    mensaje.borrar = (unsigned int)time(NULL) + 5;
                    MostrarCola(cola, disponibles, &reproduciendo, &scroll, total, 1);
                    break;
                default:
                    break;
            }
        }
        if (leer == 1 && evento.teclaDown == 1)
            leer = 0;
        else if (evento.teclaUp == 1)
            leer = 1;

        if (evento.clickIzquierdoPulsado == 1)
        {
            if (posRaton.x >= 80 && posRaton.x <= 620 && posRaton.y >= 655 && posRaton.y < 670) //Bara de reproduccion
            {
                Mix_SetMusicPosition((double)(posRaton.x - 80) * reproduciendo.duracion / 540);
            }
            if (posRaton.x >= 675 && posRaton.x <= 690 && posRaton.y >= 450 && posRaton.y <= 450 + MIX_MAX_VOLUME) //Volumen
            {
                volumen = -(posRaton.y - 450 - MIX_MAX_VOLUME);
                Mix_VolumeMusic(volumen);
            }
        }
#pragma endregion
        
        //Comprobar si la cancion ha acabado
        if (MusicaTiempoActual() == reproduciendo.duracion)
        {
            EstadisticasCambiar(&reproduciendo, 1);

            if (reproduciendo.siguiente == 1)
            {

                if (reproduciendo.bucle == 2)
                {
                    EstadisticasCambiar(&reproduciendo, 3);
                    ReproducirMusica();
                }
                else
                    Siguiente(cola, disponibles, &reproduciendo, reproduciendo.bucle, &scroll, total);
            }
            else
            {
                Mix_SetMusicPosition(0);
                PlayPauseMusica();
            }

        }

        //Leer teclas.exe
        segundos = (unsigned long)time(NULL);
        if (teclasCerrado == 0)
        {
            if (segundos != segundosAnterior)
            {
                segundosAnterior = segundos;
                tmp[0] = '\0';
                fp = fopen(PATH_TECLAS_TXT, "r");
                if (fp != NULL)
                {
                    while (fgets(tmp, 2048, fp) != NULL)
                    {
                        tmp[strlen(tmp) - 1] = '\0';
                        printf("Leido: [%s]\n", tmp);
                        if (strcmp(tmp, "anterior") == 0)
                        {
                            if (MusicaTiempoActual() < TIEMPO_CANCION_ANTERIOR)
                                Anterior(cola, disponibles, &reproduciendo, reproduciendo.bucle, &scroll, total);
                            else
                            {
                                EstadisticasCambiar(&reproduciendo, 3);
                                Mix_SetMusicPosition(0);
                            }
                        }
                        else if (strcmp(tmp, "playpause") == 0)
                            PlayPauseMusica();
                        else if (strcmp(tmp, "siguiente") == 0)
                        {
                            EstadisticasCambiar(&reproduciendo, 2);
                            Siguiente(cola, disponibles, &reproduciendo, reproduciendo.bucle, &scroll, total);
                        }
                        else if (strcmp(tmp, "Cerrado") == 0)
                            teclasCerrado = 1;
                        else if (strcmp(tmp, "volAr") == 0 && volumen != MIX_MAX_VOLUME)
                        {
                            volumen += 5;
                            if (volumen > MIX_MAX_VOLUME)
                                volumen = MIX_MAX_VOLUME;
                            Mix_VolumeMusic(volumen);
                        }
                        else if (strcmp(tmp, "volAb") == 0 && volumen != 0)
                        {
                            volumen -= 5;
                            if (volumen < 0)
                                volumen = 0;
                            Mix_VolumeMusic(volumen);
                        }
                    }
                    fclose(fp);

                    fp = fopen(PATH_TECLAS_TXT, "w");
                    fprintf(fp, "\0");
                    fclose(fp);
                }
            }
        }

        if (PantallaConInput() == 0)
            SDL_Delay(100);

        //Region Imagen
#pragma region Imagen
            PantallaLimpiar();
            ImagenDescargar(2);
            printf("PATH_IMAGEN_PAUSE: %s\n", PATH_IMAGEN_PAUSE);
            printf("PATH_IMAGEN_PLAY: %s\n", PATH_IMAGEN_PLAY);
            if (Mix_PausedMusic() == 1)
                printf("posImagenPause = 2: %d\n",ImagenCargar(PATH_IMAGEN_PAUSE, 2, 0));
            else
                printf("posImagenPlay = 2: %d\n", ImagenCargar(PATH_IMAGEN_PLAY, 2, 0));

            ImagenMover(2, 5, 625);

            ImagenDibujarTodas();
            
           if (reproduciendo.favorito == 1)
                TextoCambiarColor(0, 255, 0);
            sprintf(tmp, "%d", reproduciendo.numero+1);
            TextoEscribir(30, 620, tmp);

            TextoCambiarColor(255, 255, 255);
            sprintf(tmp, "%s", reproduciendo.nombre);
            TextoEscribir(70, 620, tmp);

            sprintf(tmp, "Album: %s", reproduciendo.album);
            TextoEscribir(220, 400, tmp);

            {
                sprintf(sPath, "%d", (int)(MusicaTiempoActual() / 60));
                if (sPath[1] == '\0')
                {
                    sPath[2] = '\0';
                    sPath[1] = sPath[0];
                    sPath[0] = '0';
                }
                sprintf(tmp, "%s", sPath);
                sprintf(sPath, "%d", MusicaTiempoActual() - (int)(MusicaTiempoActual() / 60) * 60);
                if (sPath[1] == '\0')
                {
                    sPath[2] = '\0';
                    sPath[1] = sPath[0];
                    sPath[0] = '0';
                }
                sprintf(tmp, "%s:%s", tmp, sPath);
            }
            TextoEscribir(10, 650, tmp); //Tiempo izquierda
            
            {
                sprintf(sPath, "%d", (int)(reproduciendo.duracion / 60));
                if (sPath[1] == '\0')
                {
                    sPath[2] = '\0';
                    sPath[1] = sPath[0];
                    sPath[0] = '0';
                }
                sprintf(tmp, "%s", sPath);
                sprintf(sPath, "%d", reproduciendo.duracion - (int)(reproduciendo.duracion / 60) * 60);
                if (sPath[1] == '\0')
                {
                    sPath[2] = '\0';
                    sPath[1] = sPath[0];
                    sPath[0] = '0';
                }
                sprintf(tmp, "%s:%s", tmp, sPath);
            }
            TextoEscribir(630, 650, tmp); //Tiempo derecha

            DibujarColorDelLapiz(255, 255, 255);
            for (int i = 0; i < 5; i++)
                DibujarLinea(80, 660 + i, 620, 660 + i);
            pos = MusicaTiempoActual() * 529 / reproduciendo.duracion;
            DibujarColorDelLapiz(255, 0, 0);
            for (int i = 0; i < 15; i++)
                DibujarLinea(80 + pos, 655 + i, 90 + pos, 655 + i);

            sprintf(tmp, "Siguiente: %d", reproduciendo.siguiente);
            TextoEscribir(250, 665, tmp);
            sprintf(tmp, "Bucle: %d", reproduciendo.bucle);
            TextoEscribir(400, 665, tmp);

            sprintf(tmp, "Mostrando:"); //Info
            if (mostrarCola == 1)
                sprintf(tmp, "%s cola,", tmp);
            else
                sprintf(tmp, "%s disponibles,", tmp);
            sprintf(tmp, "%s Orden: ", tmp);
            if (aleatorizado == 1)
                sprintf(tmp, "%s aleatorio", tmp);
            else if (orden == 1)
                sprintf(tmp, "%s Nombre: A - Z", tmp);
            else if (orden == 2)
                sprintf(tmp, "%s Nombre: Z - A", tmp);
            else if (orden == 3)
                sprintf(tmp, "%s Corta - Larga", tmp);
            else if (orden == 4)
                sprintf(tmp, "%s Larga - Corta", tmp);
            else if (orden == 5)
                sprintf(tmp, "%s Album: A - Z", tmp);
            else if (orden == 6)
                sprintf(tmp, "%s Album: Z - A", tmp); 
            else
                sprintf(tmp, "%s ?", tmp);
            TextoEscribir(5, 370, tmp);

            TextoEscribir(220, 440, "Estadisticas:");
            sprintf(tmp, "Reproducciones: %d", reproduciendo.reproducciones);
            TextoEscribir(240, 470, tmp);
            sprintf(tmp, "Omisiones: %d", reproduciendo.omisiones);
            TextoEscribir(240, 500, tmp);
            sprintf(tmp, "Repeticiones: %d", reproduciendo.repeticiones);
            TextoEscribir(240, 530, tmp);

            sprintf(tmp, "%d", volumen);
            TextoEscribir(660, 420, tmp);
            DibujarColorDelLapiz(255, 255, 255);
            for (int i = 0; i < 5; i++)
                DibujarLinea(680+i, 450, 680+i, 450+MIX_MAX_VOLUME);
            DibujarColorDelLapiz(255, 0, 0);
            for (int i = 0; i < 15; i++)
                DibujarLinea(675 + i, 450 + MIX_MAX_VOLUME - volumen, 675 + i, 460 + MIX_MAX_VOLUME - volumen);

            MostrarCola(cola, disponibles, &reproduciendo, &scroll, total, 0);

            if (teclasCerrado == 1)
            {
                DibujarColorDelLapiz(255, 0, 0);
                for (int i = 0; i < 10; i++)
                {
                    DibujarLinea(0, PANTALLA_ALTO - 1 - i, PANTALLA_ANCHO - 1, PANTALLA_ALTO - 1 - i);
                }
            }

            if (mensaje.borrar > segundos)
            {
                TextoCambiarColor(255, 255, 255);
                TextoEscribir(220, 590, mensaje.mensaje);
            }
#pragma endregion
        PantallaActualizar(0);
    }

    if(soloFavoritas == 0)
        GuardarFavoritos(favoritos, dir);
    printf("Hacer algo para que los favoritos se guarden bien con el modo de playlist");

    CancionesBorrar(disponibles);
    CancionesBorrar(cola);
    CancionesBorrar(favoritos);
    CerrarAudio();
    TextoCerrar();
    VentanaCerrar();
    if (teclasCerrado == 0)
        CerrarTeclas();
    else
        printf("No se manda F13 ya que tecla.exe ya esta cerrado\n");

    printf("Cerrado\n");

    return ret;
}

int Iniciar(void)
{
    int cargado = 0;
    int primerCargado = 1;

    if (VentanaCrear(PATH_ICONO) == -1)
    {
        fprintf(stderr, "No se ha podido crear una ventana de 700x700, error: %s\n", SDL_GetError());
        return -1;
    }
    //AbrirAudio(); A este se le llama antes

    //CargarSonido("nombre para reproducirlo (30 char max)",".\\Sonidos\\Carpeta\\Nombre.wav");

#if defined(_DEBUG)
    if (TextoAbrir(20, ".\\Fonts\\Sans Serif.ttf") == -1) 
        return -1;
#else
    if (TextoAbrir(20, PATH_SANS_SERIF) == -1)
        return -1;
#endif

    return 1;

}

CANCIONES* CancionesAñadirAlFinal(CANCIONES* canciones, CANCIONES* _nuevo)
{
    CANCIONES* nuevo = (CANCIONES*)malloc(sizeof(CANCIONES));
    CANCIONES* tmp = canciones;
    if (nuevo == NULL)
    {
        printf("Ha ocurrido un error al hacer malloc \n");
        return NULL;
    }

    strcpy(nuevo->nombre, _nuevo->nombre);
    strcpy(nuevo->album, _nuevo->album);
    nuevo->numero = _nuevo->numero;
    nuevo->favorito = _nuevo->favorito;
    nuevo->duracion = _nuevo->duracion;
    nuevo->siguiente = NULL;

    if (canciones == NULL)
        return nuevo;
    else
    {
        while (tmp->siguiente != NULL)
            tmp = tmp->siguiente;

        tmp->siguiente = nuevo;
        return canciones;
    }
}

CANCIONES* CancionesQuitarPrimera(CANCIONES* canciones)
{
    CANCIONES* tmp = canciones->siguiente; 
    free(canciones);
    return tmp;
}

void Siguiente(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int bucle, int* pos, int total)
{
    CANCIONES* tmp = cola;
    int ultima = 0;
    char tmpStr[255] = { "" };

    if (cola == NULL)
    {
        strcpy(mensaje.mensaje, "Cola vacia");
        mensaje.borrar = (unsigned int) time(NULL) + 5;
        printf("Cola vacia\n");
        return;
    }

    while (tmp->numero <= reproduciendo->numero && ultima == 0)
    {
        if (tmp->siguiente != NULL)
            tmp = tmp->siguiente;
        else
            ultima = 1;
    }

    if (ultima == 1)
    {
        if (bucle == 1)
        {
            MusicaReproducir(cola, disponibles, reproduciendo, cola, pos, total);
        }
        else
        {
            printf("No hay mas canciones en la cola\n");
            strcpy(mensaje.mensaje, "No hay mas canciones en la cola");
            mensaje.borrar = (unsigned int)time(NULL) + 5;
        }
    }
    else
    {
        MusicaReproducir(tmp, disponibles, reproduciendo, cola, pos, total);
    }

    return;
}

void Anterior(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int bucle, int* pos, int total)
{
    CANCIONES* tmp = NULL;
    CANCIONES* tmpSiguiente = cola;

    if (cola == NULL)
    {
        printf("Cola vacia\n");
        strcpy(mensaje.mensaje, "Cola vacia");
        mensaje.borrar = (unsigned int)time(NULL) + 5;
        return;
    }

    while (tmpSiguiente->numero != reproduciendo->numero)
    {
        tmp = tmpSiguiente;
        tmpSiguiente = tmpSiguiente->siguiente;
    }

    if (tmp == NULL)
    {
        if (bucle == 1)
        {
            tmp = cola;
            while (tmp->siguiente != NULL)
                tmp = tmp->siguiente;
        }
        else
        {
            printf("Ya esta sonando la primera cancion\n");
            strcpy(mensaje.mensaje, "Ya esta sonando la primera cancion");
            mensaje.borrar = (unsigned int)time(NULL) + 5;
            return;
        }
    }
    MusicaReproducir(tmp, disponibles, reproduciendo, cola, pos, total);
    return;
}

void Numero(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int numero, int* pos, int total)
{
    CANCIONES* tmp = NULL;
    if (mostrarCola == 1)
        tmp = cola;
    else
        tmp = disponibles;

    while (tmp != NULL && tmp->numero != numero-1)
    {
        tmp = tmp->siguiente;
    }
    if (tmp == NULL)
        tmp = cola;
    MusicaReproducir(tmp, disponibles, reproduciendo, cola, pos, total);
}

void MusicaReproducir(CANCIONES* cancion, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, CANCIONES* cola, int* pos, int total)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char musica[2048] = { "" };
    char arg[2048] = { "" };
    ImagenDescargar(1);
    if (CargarMusica(cancion->nombre) != 0)
    {
        strcpy(reproduciendo->path, cancion->nombre);
        MusicaNombre(musica);
        strcpy(reproduciendo->nombre, musica);
        strcpy(reproduciendo->album, cancion->album);
        reproduciendo->numero = cancion->numero;
        reproduciendo->favorito = cancion->favorito;
        reproduciendo->duracion = cancion->duracion;
        EstadisticasCargar(reproduciendo);
        VentanaCambiarNombre(musica);

        sprintf(musica, PATH_IMAGENES_EXE);
        sprintf(arg, "%s \"%s\"", PATH_IMAGENES_EXE, cancion->nombre);

#if defined(_DEBUG)
        remove("imagen.jpg");
#else
        remove(PATH_IMAGEN_JPG);
#endif
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcessA(NULL, arg, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

#if defined(_DEBUG)
        if (ImagenCargar("imagen.jpg", 1, 1) == -1)
            ImagenCargar("imagenDefecto.png", 1, 1);
#else
        if (ImagenCargar(PATH_IMAGEN_JPG, 1, 1) == -1)
            ImagenCargar(PATH_IMAGEN_DEFECTO_PNG, 1, 1);

#endif
        ImagenMover(1, 0, 400);
        ReproducirMusica();
        MostrarCola(cola, disponibles, reproduciendo, pos, total, 1);

    }
    else
    {
        printf("Ha ocurrido un error al cargar la cancion %s\n", cancion->nombre);
        sprintf(mensaje.mensaje, "Ha ocurrido un error al cargar la cancion %s", cancion->nombre);
        mensaje.borrar = (unsigned int)time(NULL) + 5;
    }
    return;
}

void CancionesBorrar(CANCIONES* canciones)
{
    if (canciones == NULL)
        return;
    CANCIONES* tmp = canciones->siguiente;
    while (canciones != NULL)
    {
        free(canciones);
        canciones = tmp;
        if(tmp != NULL)
            tmp = tmp->siguiente;
    }
}

CANCIONES* Aleatorizar(CANCIONES* disponibles, CANCIONES* cola, REPRODUCIENDO* reproduciendo, int total, int* scroll, int texto)
{
    CANCIONES* tmp = NULL;
    CANCIONES* canciones = disponibles;
    int numero = -1;
    int numeroOcupado = 0;

    if (disponibles == NULL || cola == NULL)
        return cola;

    while (canciones != NULL)               //Poner todas las canciones disponibles a -1 (para indicar que no se a aleatorizado)
    {
        canciones->numero = -1;
        canciones = canciones->siguiente;
    }

    canciones = disponibles;
    
    while (canciones != NULL)
    {
        numeroOcupado = 1;
        while (numeroOcupado == 1)                    //Asignar un numero aleatorio a cada cancion de disponibles
        {
            numero = randInt(total);
            tmp = disponibles;
            numeroOcupado = 0;
            while (numeroOcupado == 0 && tmp != NULL) //Buscar si el random de ahora ya se a asignado a otra cancion
            {
                if (tmp->numero == numero)
                    numeroOcupado = 1;
                else
                    tmp = tmp->siguiente;
            }
        }

        canciones->numero = numero;
        canciones = canciones->siguiente;
    }

    canciones = disponibles;
    while (canciones != NULL)                       //Colocar las canciones disponibles aleatorizadas en orden en la cola
    {
        tmp = cola;
        for (int i = 0; i < canciones->numero; i++)
        {
            tmp = tmp->siguiente;
        }
        strcpy(tmp->nombre, canciones->nombre);
        strcpy(tmp->album, canciones->album);
        tmp->favorito = canciones->favorito;
        tmp->duracion = canciones->duracion;
        canciones = canciones->siguiente;
    }


    if (reproduciendo->numero != -1)    //Buscar en la cola donde ha quedado la canción que está sonando ahora 
    {
        if (mostrarCola == 1)
            tmp = cola;
        else
            tmp = disponibles;
        numero = 0;
        while (tmp != NULL && strcmp(tmp->nombre, reproduciendo->path) != 0)
            tmp = tmp->siguiente;

        if (tmp != NULL)
            reproduciendo->numero = tmp->numero;
        else
            printf("tmp tiene valor NULL en Aleatorizar\n");
    }

    numero = 0;
    canciones = disponibles;
    while (canciones != NULL)   //Recolocar todos los numeros en las disponibles para que esté ordenada la lista
    {
        canciones->numero = numero;
        numero++;
        canciones = canciones->siguiente;
    }
    if (texto == 1)
    {
        strcpy(mensaje.mensaje, "Cola aleatorizada");
        mensaje.borrar = (unsigned int)time(NULL) + 5;
    }

    MostrarCola(cola, disponibles, reproduciendo, scroll, total, 1);
    return cola;
}

int randInt(int n)
{
    int divisor = RAND_MAX / (n + 1);
    int retval;

    do
    {
        retval = rand() / divisor;
    } while (retval > n);


    return retval;
}

int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}

void MostrarCola(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo, int* pos, int total, int mover)
{
    CANCIONES* tmp = NULL;
    int num = 0;
    if (mostrarCola == 1)
        tmp = cola;
    else
        tmp = disponibles;

    char str[2048] = { "" };
    int i = 0;
    int j = 0;

    if (tmp == NULL)
        return;

    if (reproduciendo->numero == -1)
    {
        *pos = 0;
        return;
    }

    if (mover == 1)
    {
        if (mostrarCola == 1)
        {
            for (i = 0; i < reproduciendo->numero; i++)
                tmp = tmp->siguiente;
        }
        else
        {
            i = 0;
            while (i != total && strcmp(reproduciendo->path, tmp->nombre) != 0)
                tmp = tmp->siguiente;
        }

        if (*pos >= tmp->numero)
            *pos = tmp->numero - 1;
        else if (*pos + 12 <= tmp->numero+1)
            *pos = tmp->numero-10;
        if (tmp->numero == total)
            *pos -= 1;
        if (tmp->numero == 0)
            *pos = 0;
        if (*pos == -1)
            *pos = 0;
    }

    if (mostrarCola == 1)
        tmp = cola;
    else
        tmp = disponibles;

    for (i = 0; i < *pos; i++)
        tmp = tmp->siguiente;

    i = 0;

    while ((i + *pos) < (*pos + 12) && tmp != NULL)
    {
        TextoCambiarColor(255, 255, 255);
        strcpy(str, tmp->nombre);
        j =  (int)strlen(str);
        while (str[j] != '\\')
            j--;
        strcpy(str, &str[j+1]);
        str[strlen(str) - 4] = '\0';
        TextoEscribir(70, 10 + i * 30, str);

        sprintf(str, "%d", tmp->numero+1);
        if (tmp->favorito == 1)
            TextoCambiarColor(0, 255, 0);
        TextoEscribir(30, 10 + i * 30, str);

        j = 0;
        if (mostrarCola == 1 && tmp->numero == reproduciendo->numero)
            j = 1;
        else if (mostrarCola == 0 && strcmp(tmp->nombre, reproduciendo->path) == 0) //j: poner el cuadrado o no
            j = 1;
        if (j == 1)
        {
            DibujarColorDelLapiz(255, 255, 255);

            for (j = 0; j < 10; j++)
            {
                DibujarLinea(10, 18 + i * 30 + j, 20, 18 + i * 30 + j);
            }
        }
        i++;
        tmp = tmp->siguiente;
    }

}

void CerrarTeclas(void)
{
    printf("Mandando F13...\n");
    INPUT inputs[2] = { 0 };
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_F13;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_LWIN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("Error mandando F13\n");
        strcpy(mensaje.mensaje, "Ha ocurrido un error cerrando las teclas multimedia");
        mensaje.borrar = (unsigned int)time(NULL) + 5;
    }
    else
    {
        printf("F13 mandado, teclas.exe deberia cerrarse\n");
        strcpy(mensaje.mensaje, "Se han desactivado las teclas multimedia");
        mensaje.borrar = (unsigned int)time(NULL) + 5;
    }
}

void AbrirTeclas(int primero)
{
    STARTUPINFOA siTeclas;
    PROCESS_INFORMATION piTeclas;
    FILE* fp;

    ZeroMemory(&siTeclas, sizeof(siTeclas));
    siTeclas.cb = sizeof(siTeclas);
    ZeroMemory(&piTeclas, sizeof(piTeclas));

#if defined(_DEBUG)
    if (CreateProcessA(NULL, PATH_TECLAS_EXE, NULL, NULL, FALSE, 0, NULL, NULL, &siTeclas, &piTeclas))
    {
        CloseHandle(piTeclas.hProcess);
        CloseHandle(piTeclas.hThread);
    }
#else    
    //if (CreateProcessA(NULL, PATH_TECLAS_EXE, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &siTeclas, &piTeclas))
    if (CreateProcessA(NULL, PATH_TECLAS_EXE, NULL, NULL, FALSE, 0, NULL, NULL, &siTeclas, &piTeclas))
    {
        CloseHandle(piTeclas.hProcess);
        CloseHandle(piTeclas.hThread);
    }
#endif
    if (primero != 1)
    {
        strcpy(mensaje.mensaje, "Teclas multimedia activas");
        mensaje.borrar = (unsigned int)time(NULL) + 5;
    }
    fp = fopen(PATH_TECLAS_TXT, "w");
    fprintf(fp, "\0");
    fclose(fp);

}

void AsignarFavorito(CANCIONES* cancion, CANCIONES* favoritos)
{
    CANCIONES* tmp = favoritos;

    while (tmp != NULL && cancion->favorito == 0)
    {
        if (strcmp(tmp->nombre, cancion->nombre) == 0)
            cancion->favorito = 1;
        else
            tmp = tmp->siguiente;
    }
}

CANCIONES* QuitarCancion(CANCIONES* canciones, char* nombreAQuitar)
{
    CANCIONES* anterior = NULL;
    CANCIONES* alante = canciones;
    if (alante == NULL)
        return NULL;

    if (alante->siguiente == NULL)
    {
        if (strcmp(alante->nombre, nombreAQuitar) == 0)
        {
            free(alante);
            return NULL;
        }
        else
            return canciones;
    }
    anterior = NULL;
    alante = canciones;

    while (alante->siguiente != NULL && strcmp(alante->nombre, nombreAQuitar) != 0)
    {
        anterior = alante;
        alante = alante->siguiente;
    }

    if (strcmp(alante->nombre, nombreAQuitar) != 0)
        return canciones;
    
    if (alante->siguiente == NULL)
        anterior->siguiente = NULL;
    else
    {
        if(anterior != NULL)
            anterior->siguiente = alante->siguiente;
        else
        {
            return CancionesQuitarPrimera(canciones);
        }
    }

    free(alante);

    return canciones;
}

CANCIONES* AñadirFavorito(CANCIONES* disponibles, CANCIONES* cola, CANCIONES* favoritos, REPRODUCIENDO* reproduciendo)
{
    CANCIONES* tmp = cola;
    CANCIONES cancionTMP = { 0 };

    while (tmp != NULL && strcmp(tmp->nombre, reproduciendo->path) != 0)
        tmp = tmp->siguiente;

    if (tmp == NULL)
        return favoritos;

    tmp->favorito = 1;
    reproduciendo->favorito = 1;

    tmp = disponibles;
    while (tmp != NULL && strcmp(tmp->nombre, reproduciendo->path) != 0)
        tmp = tmp->siguiente;

    if (tmp == NULL)
        return favoritos;

    tmp->favorito = 1;

    strcpy(cancionTMP.nombre, reproduciendo->path);
    cancionTMP.favorito = 1;
    cancionTMP.numero = 0;
    cancionTMP.siguiente = NULL;
    cancionTMP.duracion = 0;
    favoritos = CancionesAñadirAlFinal(favoritos, &cancionTMP);

    return favoritos;
}

CANCIONES* QuitarFavorito(CANCIONES* disponibles, CANCIONES* cola, CANCIONES* favoritos, REPRODUCIENDO* reproduciendo)
{
    CANCIONES* tmp = cola;


    while (tmp != NULL && strcmp(tmp->nombre, reproduciendo->path) != 0)
        tmp = tmp->siguiente;

    if (tmp == NULL)
        return favoritos;

    tmp->favorito = 0;

    tmp = disponibles;

    while (tmp != NULL && strcmp(tmp->nombre, reproduciendo->path) != 0)
        tmp = tmp->siguiente;

    if (tmp == NULL)
        return favoritos;

    tmp->favorito = 0;

    favoritos = QuitarCancion(favoritos, reproduciendo->path); //Quitar de la lista de favoritos
    reproduciendo->favorito = 0;

    return favoritos;
}

void GuardarFavoritos(CANCIONES* favoritos, char* dir)
{
    FILE* fp = fopen(PATH_FAVORITOS, "r");
    FILE* fpNuevo = NULL;
    CANCIONES* tmp = favoritos;
    char str[2048] = { "" };
    char *ret;

    if (fp == NULL)
    {
        if (tmp == NULL)
            return;

        fp = fopen(PATH_FAVORITOS, "w");
        fprintf(fp, "%s\n", dir);
        while (tmp != NULL)
        {
            fprintf(fp, "\t%s\n", tmp->nombre);
            tmp = tmp->siguiente;
        }
        fclose(fp);
        return;
    }
    else
    {
        fpNuevo = fopen(PATH_FAVORITOS_TMP, "w");

        while ((ret = fgets(str, 2048, fp)) != NULL)  //Escribir todo lo que hay en el archivo hasta llegar al dir actual
        {
            if (str[strlen(str) - 1] == '\n')
                str[strlen(str) - 1] = '\0';
            if(strcmp(str, dir) == 0)
                break;
            else
                fprintf(fpNuevo, "%s\n", str);
        }
        if (tmp != NULL) //Si no hay favoritos, omitir esto
        {
            fprintf(fpNuevo, "%s\n", dir);  //Escribir el dir actual

            while (tmp != NULL) //Escribir todos los favoritos que hay guardados
            {
                fprintf(fpNuevo, "\t%s\n", tmp->nombre);
                tmp = tmp->siguiente;
            }
        }
        if (tmp != NULL || strcmp(str, dir) == 0) //Si no hay favoritos, omitir esto, excepto si hay favoritos guardados
        {
            if (fgets(str, 2048, fp) != NULL)
                printf("Omitido: %s\n", str);
            while ((ret = fgets(str, 2048, fp)) != NULL) //Buscar donde acaban los guardados anteriores, hay que ignorarlos
            {                                               //Para no escribirlos dos veces o para borrarlos
                if (str[0] != '\t')
                    break;
            }
        }
        if (ret != NULL) //Si hay algo despues de los guardados, escribirlo. ret == NULL significa que fp ha llegado al final
        {
            if(str[strlen(str)-1] == '\n')
                str[strlen(str) - 1] = '\0';
            fprintf(fpNuevo, "%s\n", str);

            while (fgets(str, 2048, fp) != NULL)
            {
                if (str[strlen(str) - 1] == '\n')
                    str[strlen(str) - 1] = '\0';
                fprintf(fpNuevo, "%s\n", str);
            }
        }

    }
    fclose(fp);
    if(fpNuevo != NULL)
        fclose(fpNuevo);

    remove(PATH_FAVORITOS);
    if(rename(PATH_FAVORITOS_TMP, PATH_FAVORITOS) != 0)
        printf("Ha ocurrido un error renombrando los favoritos\n");

}

//1: Alfabetico A-Z, 2: Alfabetico Z-A, 3: duracion Mas-Menos, 4: duracion Menos-Mas, 5: album A-Z, 6: album Z-A
CANCIONES* OrdenarDisponibles(CANCIONES* disponibles, int como)
{
    CANCIONES* actual = disponibles;
    CANCIONES* siguiente = actual->siguiente;
    CANCIONES tmp = { 0 };
    int ordenado = 0;

    while (ordenado == 0)
    {
        actual = disponibles;
        siguiente = disponibles->siguiente;
        ordenado = 1;
        while (siguiente != NULL)
        {
            if ((como == 1 && strcmp(actual->nombre, siguiente->nombre) > 0) || (como == 2 && strcmp(actual->nombre, siguiente->nombre) < 0)
                || (como == 3 && actual->duracion > siguiente->duracion) || (como == 4 && actual->duracion < siguiente->duracion)
                || (como == 5 && strcmp(actual->album, siguiente->album) > 0) || (como == 6 && strcmp(actual->album, siguiente->album) < 0))
            {
                ordenado = 0;
                tmp = *actual;
                strcpy(actual->nombre, siguiente->nombre);
                strcpy(actual->album, siguiente->album);
                actual->duracion = siguiente->duracion;
                actual->favorito = siguiente->favorito;
                actual->siguiente = siguiente;
                strcpy(siguiente->nombre, tmp.nombre);
                strcpy(siguiente->album, tmp.album);
                siguiente->duracion = tmp.duracion;
                siguiente->favorito = tmp.favorito;
                siguiente->siguiente = siguiente->siguiente;
            }
            actual = actual->siguiente;
            siguiente = siguiente->siguiente;
        }
    }
    ordenado = 0;
    actual = disponibles;
    while (actual != NULL)
    {
        actual->numero = ordenado++;
        actual = actual->siguiente;
    }
    return disponibles;
}

CANCIONES* ReiniciarCola(CANCIONES* cola, CANCIONES* disponibles, REPRODUCIENDO* reproduciendo)
{
    CANCIONES* tmp = cola;
    CANCIONES* canciones = disponibles;
    while (tmp != NULL)
    {
        strcpy(tmp->nombre, canciones->nombre);
        strcpy(tmp->album, canciones->album);
        tmp->favorito = canciones->favorito;
        tmp->duracion = canciones->duracion;
        tmp->numero = canciones->numero;
        if (strcmp(tmp->nombre, reproduciendo->path) == 0)
            reproduciendo->numero = tmp->numero;
        canciones = canciones->siguiente;
        tmp = tmp->siguiente;
    }
    return cola;
}

//0: Nada 1: reproducciones, 2: veces saltada 3: veces repetida -1: reiniciar 
void EstadisticasCambiar(REPRODUCIENDO* reproduciendo, int cambio)
{
    FILE* fp = fopen(PATH_ESTADISTICAS, "r");
    FILE* fpNuevo;
    char str[2100] = { "" };
    char nombre[2048] = { "" };
    int reproducciones = 0;
    int omisiones = 0;
    int repeticiones = 0;
    int q; //descarte
    int coincidencia = 0;
    if (fp == NULL)
    {
        fp = fopen(PATH_ESTADISTICAS, "w");
        fprintf(fp, "%s\n", reproduciendo->path);
        fprintf(fp, "%d %d %d\n", cambio == 1 ? 1 : 0, cambio == 2 ? 1 : 0, cambio == 3 ? 1 : 0);
        fclose(fp);
        return;
    }
    else
    {
        fpNuevo = fopen(PATH_ESTADISTICAS_TMP, "w");
        while (coincidencia == 0 && fgets(str, 2048, fp) != NULL)
        {
            if (str[0] != '\n')
                strcpy(nombre, str);
            else
                break;
            nombre[strlen(nombre) - 1] = '\0';
            if (strcmp(nombre, reproduciendo->path) != 0)
            {
                fgets(str, 2048, fp);
                q = sscanf(str, "%d %d %d", &reproducciones, &omisiones, &repeticiones);
                fprintf(fpNuevo, "%s\n%d %d %d\n", nombre, reproducciones, omisiones, repeticiones);
            }
            else
                coincidencia = 1;
        }
        fgets(str, 2048, fp);
        q = sscanf(str, "%d %d %d", &reproducciones, &omisiones, &repeticiones);
        if (coincidencia == 1)
        {
            if (cambio != -1)
                fprintf(fpNuevo, "%s\n%d %d %d\n", nombre, cambio == 1 ? reproducciones + 1 : reproducciones, cambio == 2 ? omisiones + 1 : omisiones, cambio == 3 ? repeticiones + 1 : repeticiones);
            else
                fprintf(fpNuevo, "%s\n0 0 0\n", nombre);

            while (fgets(str, 2048, fp) != NULL)
            {
                if (str[0] != '\n')
                    strcpy(nombre, str);
                else
                    break;
                nombre[strlen(nombre) - 1] = '\0';
                fgets(str, 2048, fp);
                q = sscanf(str, "%d %d %d", &reproducciones, &omisiones, &repeticiones);
                fprintf(fpNuevo, "%s\n%d %d %d\n", nombre, reproducciones, omisiones, repeticiones);
            }
        }
        else
            fprintf(fpNuevo, "%s\n%d %d %d", reproduciendo->path, cambio == 1 ? 1 : 0, cambio == 2 ? 1 : 0, cambio == 3 ? 1 : 0);
             
        fclose(fp);
        fclose(fpNuevo);
    }

    remove(PATH_ESTADISTICAS);
    if(rename(PATH_ESTADISTICAS_TMP, PATH_ESTADISTICAS) != 0)
        printf("Ha ocurrido un error renombrando el archivo de estadisticas\n");
}

void EstadisticasCargar(REPRODUCIENDO* reproduciendo)
{
    int reproducciones = 0;
    int omisiones = 0;
    int repeticiones = 0;
    int encontrado = 0;
    char str[2048];
    FILE* fp;

    EstadisticasCambiar(reproduciendo, 0); //Esto se asegura de que el archivo exista y que la cancion que está sonando esté en la lista.
                                              //Si el archivo no existe, se creará. Si la canción no está en el archivo, se añadirá a 0.
                                              //Si la canción ya está en el archivo, no se cambiará nada.

    fp = fopen(PATH_ESTADISTICAS, "r");

    while (encontrado == 0 && fgets(str, 2048, fp) != NULL)
    {
        str[strlen(str) - 1] = '\0';
        if (strcmp(str, reproduciendo->path) == 0)
            encontrado = 1;
    }

    if (encontrado == 1)
    {
        fgets(str, 2048, fp);
        encontrado = sscanf(str, "%d %d %d", &reproduciendo->reproducciones, &reproduciendo->omisiones, &reproduciendo->repeticiones);
    }

    fclose(fp);
    return;
}

CANCIONES* ComprobarFavoritoPlayList(CANCIONES* cancion, CANCIONES* favoritos)
{
    FILE* fp = fopen(PATH_FAVORITOS, "r");
    char str[2048] = { "" };
    CANCIONES cancionTMP = { 0 };
    if (fp == NULL)
    {
        printf("No se puede cargar el archivo: %s\n", PATH_FAVORITOS);
        return favoritos;
    }
    
    while (fgets(str, 2048, fp) != NULL)
    {
        if (str[0] == '\t')
            strcpy(str, str+1);

        if (_stricmp(str, cancion->nombre) == 0)
        {
            printf("\tEs favorito\n");
            strcpy(cancionTMP.nombre, cancion->nombre);
            strcpy(cancionTMP.album, cancion->album);
            cancion->favorito = 1;
            cancionTMP.favorito = 1;
            cancionTMP.numero = cancion->numero;
            cancionTMP.siguiente = NULL;
            cancionTMP.duracion = cancion->duracion;
            favoritos = CancionesAñadirAlFinal(favoritos, &cancionTMP);
            break;
        }
    }

    return favoritos;
}

int VentanaError(char* error)
{
    EVENTO evento;
    printf("Error: %s\n", error);
    VentanaCrear(PATH_ICONO);
    TextoAbrir(20, PATH_SANS_SERIF);
    TextoEscribir(50, 340, error);
    PantallaActualizar(0);
    evento = Evento();
    while (evento.evento != CERRAR && evento.evento != TECLA_RETURN && evento.evento != TECLA_ESCAPE)
        evento = Evento();
    TextoCerrar();
    VentanaCerrar();
    CerrarAudio();
    return -1;
}