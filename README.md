# Reproductor
## CONFIGURACIÓN INICIAL:

Hay que crear el archivo "C:/Users/user/Documents/ReproductorConfig.txt", y dentro de este poner todas las direcciones o opciones
que se deseen cambiar. La estructura del archivo es (el orden no importa):
"""""
NOMBRE_DE_LA_PROPIEDAD_A_CAMBIAR
VALOR_DE_LA_PROPIEDAD_DESEADA
NOMBRE_DE_OTRA_PROPIEDAD_A_CAMBIAR
VALOR_DE_LA_OTRA_PROPIEDAD_DESEADA
...
"""""
Las variables configurables son:
    Primero (no es necesario que sea primero, pero mejor si lo es), la direccion de este ejecutable, de la siguiente manera:
        """""
        REPRODUCTOR
        X:/direccion/a/la/carpeta/Reproductor.exe
        """""
    Todas las que empiezan con "PATH_" - strings, char[100] exactamente (si son direcciones absolutas y se ponen todas mejor)
        (todos los PATH_ deberían apuntar a la carpeta con el programa, excepto PATH_MUSICA y PATH_PLAYLISTS) (los valores
            por defecto probablemente no te funcionen)
    TIEMPO_CANCION_ANTERIOR - int (este es el segundo máximo para cambiar a la cancion anterior usando el teclado)

El .txt a copiar en la carpeta de documentos está en la carpeta "Adicional", con valores predeterminados.


## QUE OTROS EJECUTABLES HACEN FALTA Y QUE HACEN

Para ejecutar este programa, se le puede llamar desde la terminal al .exe directamente con los parámetros que hagan falta,
o crear un acceso directo de musica.exe y añadirlo en la carpeta "C:\ProgramData\Microsoft\Windows\Start Menu\Programs" 
para poder ejecutar este lanzador y introducir la carpeta y los parámetros deseados para lanzar el reproductor.

Al ejecutar este programa, se ejecuta teclas.exe, un keylogger que solo escucha a las teclas: 
media previous, media playpause, media next, f13 (cerrar teclas.exe), f14 (bajar volumen) y f15 (subir volumen). Cuando el 
keylogger escucha alguna de estas pulsaciones, lo escribe en el fichero teclas.txt, para que luego este programa lo lea
y haga lo que tenga que hacer. Teclas.exe borra las pulsaciones que intercepta, es decir, ninguna otra aplicación sabrá
que se ha pulsado cualquiera de las teclas antes mencionadas.

Justo antes de que una canción empieze a sonar, se llama al programa imagenes.exe para sacar la portada del .mp3 que va 
a reproducirse, y lo guarda en imangen.jpg, para que este programa la muestre.

El código de todos estos programas adicionales, están en la carpeta "Adicional"


## PARÁMETROS DE EJECUCIÓN DEL PROGRAMA

Primero, (casi) SIEMPRE, despues del nombre del programa, hay que poner en que carpeta hay que buscar los .mp3. Se puede poner
    la dirección completa, o tan solo el nombre de la carpeta con la música dentro de la carpeta C:/Users/user/Music

Parametros adicionales (tienen que estar puestos de forma individual):
    -a: aleatoriza la cola
    -b0: pone el modo de bucle 0 (al terminar de reproducir la cola, parar)
    -b1: pone el modo de bucle 1 (al terminar de reproducir la cola, empezar desde la primera canción)
    -b2: pone el modo de bucle 2 (al terminar de reproducir la cancion, la repite)
    -s0: pone el modo de siguiente a 0 (al terminar de reproducir la canción, no hace nada)
    -s1: pone el modo de siguiente a 1 (al terminar de reproducir la canción, suena la siguiente en la cola)
    -v x: pone el volumen a x (el volumen, x, va de 0 a 128) ('v' y el número entero'x', tienen que estar separados)
    -f: reproduce solo los favoritos de la carpeta solicitada. Se puede omitir el parámetro de la dirección si se usa -f,
        para así reproducir todas las canciones favoritas, sin importar en que carpeta estén.
    -p: en vez de interpretar el primer parametro (direccion) como carpeta desde donde buscar los .mp3, lo interpreta como
        el nombre de una playlist, que buscará en la carpeta de playlists.
    --x: reproduce la canción x de la cola (x tiene que ser un entero)