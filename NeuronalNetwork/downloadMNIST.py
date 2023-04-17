import os
import urllib.request
import gzip
import shutil
import struct
import numpy as np
from PIL import Image
import cv2


def download_mnist(path):
    base_url = "http://yann.lecun.com/exdb/mnist/"
    file_names = ["train-images-idx3-ubyte.gz", "train-labels-idx1-ubyte.gz",
                  "t10k-images-idx3-ubyte.gz", "t10k-labels-idx1-ubyte.gz"]
    
    if not os.path.exists(path):
        os.makedirs(path)
    
    for file_name in file_names:
        print(f"Downloading {file_name}...")
        url = base_url + file_name
        file_path = os.path.join(path, file_name)
        urllib.request.urlretrieve(url, file_path)
        
        with gzip.open(file_path, 'rb') as f_in:
            with open(os.path.join(path, file_name[:-3]), 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
        
        os.remove(file_path)
        
    print("Done.")

def load_mnist(path, kind='train'):
    """Charger les données MNIST depuis un fichier"""
    labels_path = os.path.join(path, f'{kind}-labels-idx1-ubyte')
    images_path = os.path.join(path, f'{kind}-images-idx3-ubyte')
    with open(labels_path, 'rb') as lbpath:
        magic, n = struct.unpack('>II', lbpath.read(8))
        labels = np.fromfile(lbpath, dtype=np.uint8)
    with open(images_path, 'rb') as imgpath:
        magic, num, rows, cols = struct.unpack('>IIII', imgpath.read(16))
        images = np.fromfile(imgpath, dtype=np.uint8).reshape(len(labels), 784)
        images = ((images / 255.) - .5) * 2
    return images, labels

def save_images(images, labels, folder_path):
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)
    for i in range(len(images)):
        img = images[i].reshape((28, 28))
        img = cv2.normalize(img, None, 0, 255, cv2.NORM_MINMAX, cv2.CV_8U)
        img_path = os.path.join(folder_path, f'{labels[i]}_{i}.png')
        cv2.imwrite(img_path, img)
        
path = './dataset/MNIST/'

download_mnist(path)

# Charger les données d'entraînement MNIST
X_train, y_train = load_mnist(path, kind='train')

# Enregistrer les images dans un dossier
save_images(X_train, y_train, path)

