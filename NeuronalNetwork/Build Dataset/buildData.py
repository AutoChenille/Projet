import cv2
import freetype

face = freetype.Face('/usr/share/fonts/truetype/dejavu/comicsansms.ttf') # changer le chemin selon votre installation
face.set_char_size(16*64)

print("Liste des polices disponibles:")
for name in sorted(face.get_sfnt_names()):
    print(name)
