import tensorflow as tf
import numpy as np
import os
import cv2

# Charger les données MNIST
(train_images, train_labels), (test_images, test_labels) = tf.keras.datasets.mnist.load_data()

PATH = './dataset/mnist_images'

# Créer un dossier pour stocker les images
if not os.path.exists(PATH):
    os.mkdir(PATH)
if not os.path.exists(PATH + '/training'):
    os.mkdir(PATH + '/training')
if not os.path.exists(PATH + '/test'):
    os.mkdir(PATH + '/test')

# Fonction pour sauvegarder une image
def save_image(image, label, filepath):
    cv2.imwrite(filepath, image)

# Redimensionner les images à une taille de 28x28 pixels
train_images = np.array([cv2.resize(img, (24, 24)) for img in train_images])
test_images = np.array([cv2.resize(img, (24, 24)) for img in test_images])

# Sauvegarder les images d'entraînement
for i, (image, label) in enumerate(zip(train_images, train_labels)):
    filepath = f'{PATH}/training/{label}_{i}.png'
    save_image(image, label, filepath)

# Sauvegarder les images de test
for i, (image, label) in enumerate(zip(test_images, test_labels)):
    filepath = f'{PATH}/test/{label}_{i}.png'
    save_image(image, label, filepath)

