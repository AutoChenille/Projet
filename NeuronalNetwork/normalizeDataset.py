import os
from PIL import Image
import numpy as np

def normalize_image(img):
    """
    Fonction pour normaliser une image.
    """
    img = img.astype(np.float32)
    img = (img - img.min()) / (img.max() - img.min())
    return img

def resize_and_normalize_image(src_path, dest_path):
    """
    Fonction pour redimensionner et normaliser une image de 92x92 à 28x28.
    """
    img = Image.open(src_path)
    img = img.resize((3, 3), resample=Image.LANCZOS)
    img = np.array(img) / 255.0
    img = normalize_image(img)
    img = (img * 255).astype(np.uint8)
    img = Image.fromarray(img)
    img.save(dest_path)

def main(src_dir, dest_dir):
    """
    Fonction principale pour parcourir tous les fichiers PNG dans le répertoire source et les redimensionner et normaliser.
    """
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)
    for filename in os.listdir(src_dir):
        if filename.endswith('.png'):
            src_path = os.path.join(src_dir, filename)
            dest_path = os.path.join(dest_dir, filename)
            resize_and_normalize_image(src_path, dest_path)

if __name__ == '__main__':
    src_dir = './dataset/SACHA/'
    dest_dir = './dataset/normalizedSACHA/'
    main(src_dir, dest_dir)