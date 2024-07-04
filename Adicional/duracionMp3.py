import eyed3
import sys

def main(path: str, nombre: str):
    mp3 = eyed3.load(path)
    print("DuracionMp3.exe> cancion: " + path)
    print("DuracionMp3.exe> archivo: " + nombre)
    print("DuracionMp3.exe> duracion: " + str(mp3.info.time_secs))
    with open(nombre, "w") as f:
        f.write(str(mp3.info.time_secs))
        f.close()

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])