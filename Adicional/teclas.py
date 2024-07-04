from __future__ import print_function
from pynput import keyboard
from pynput.keyboard import Key
import os

path = " "

def on_press(key):
    try:
        if key != Key.media_previous and key != Key.media_play_pause and key != Key.media_next and key != Key.f13 and key != Key.f14 and key != Key.f15:
            return
        if key == Key.f13:
            print("(teclas.exe) f13 leido")
            return False
    except:
        return
    
    Enviar(key=key)

def Enviar(key):
    

    if key == Key.media_previous:
        output = "anterior\n"
    elif key == Key.media_play_pause:
        output = "playpause\n"
    elif key == Key.media_next:
        output = "siguiente\n"
    else:
        output = " "
    try:
        if key == Key.f14:
            output = "volAb\n"
    except:
        pass
    try:
        if key == Key.f15:
            output = "volAr\n"
    except:
        pass

    print(f"\n(teclas.exe) Mandando {output[:-1]}")

    with open(path, 'a') as f:
        f.write(output)
        f.close()

def win32_event_filter(msg, data): #anterior: 177 - pause: 179 - siguiente: 176 - f14: 125 - f15: 126
    if ((msg == 257 or msg == 256) and data.vkCode == 177) or ((msg == 257 or msg == 256) and data.vkCode == 179) or ((msg == 257 or msg == 256) and data.vkCode == 176) or ((msg == 257 or msg == 256) and data.vkCode == 125) or ((msg == 257 or msg == 256) and data.vkCode == 126):
        print("(teclas.exe) Tecla quitada\n")
        listener._suppress = True
    else:
        listener._suppress = False
    return True

print("\n(teclas.exe) Abierto\n")

listener = keyboard.Listener(on_press=on_press, 
                             win32_event_filter=win32_event_filter,
                             suppress=False)

with open(os.path.abspath(os.environ["USERPROFILE"]+"/Documents/ReproductorConfig.txt"), 'r') as f:
    string: str = ''
    i: int = 0
    while string != 'PATH_TECLAS_TXT\n' and i != 20:
        string = f.readline()
        i += 1
    path = f.readline()
    f.close()

if path[-1] == '\n':
    path = path[:-1]
path = os.path.abspath(path)

print("(teclas.exe) escribiendo en: " + path)
    

with open(path, 'w') as f:
    f.write(" ")
    f.close()


listener.start()  # start to listen on a separate thread
listener.join()  # remove if main thread is polling self.keys

print("\n(teclas.exe) Cerrado\n")
with open(path, 'w') as f:
    f.write("Cerrado\n")
    f.close()