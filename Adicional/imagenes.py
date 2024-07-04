import sys
import eyed3 
import os

# def SacarImagenes(path:str):
# 	origen = eyed3.load(path)
# 	nombreAlbum = origen.tag.album
# 	artista = origen.tag.artist
# 	for imagen in origen.tag.images:
# 		archivo = open("E:/Aplicaciones/Reproductor/imagen.jpg", "wb")
# 		archivo.write(imagen.image_data)
# 		archivo.close()
# 		print("imagen.jpg creado en E:")
# 	for imagen in origen.tag.images:
# 		archivo = open("C:/Users/julen/Desktop/Programacion/C/Reproductor/imagen.jpg", "wb")
# 		archivo.write(imagen.image_data)
# 		archivo.close()
# 		print("imagen.jpg creado en el proyecto")

def SacarImagenes(path: str):
	destino: str = " "
	with open(os.path.abspath(os.environ["USERPROFILE"]+"/Documents/ReproductorConfig.txt"), 'r') as f:
		while(destino != "PATH_IMAGEN_JPG"):
			destino = f.readline()
			if destino[-1] == '\n':
				destino = destino[:-1]
			if destino[-1] == ' ':
				destino = destino[:-1]
		destino = f.readline()
		if destino[-1] == '\n':
			destino = destino[:-1]
		if destino[-1] == ' ':
			destino = destino[:-1]
		f.close()
		if not os.path.isabs(destino):
			destino = os.path.abspath(destino)

	for imagen in eyed3.load(path).tag.images:
		archivo = open(destino, "wb")
		archivo.write(imagen.image_data)
		archivo.close
		print("imagen.jpg creado en " + destino)

print("Hecho por Gastegui para el reproductor mp3")

if len(sys.argv) != 2:
	print("No se han pasado dos argumentos")
else:
	SacarImagenes(sys.argv[1])