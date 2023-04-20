import tensorflow as tf
import numpy as np
import os
import matplotlib.pyplot as plt

# Charger les données MNIST
(train_images, train_labels), (test_images, test_labels) = tf.keras.datasets.mnist.load_data()

# Créer un dossier pour stocker les images
if not os.path.exists('./dataset/mnist_images'):
    os.mkdir('./dataset/mnist_images')

# Fonction pour sauvegarder une image
def save_image(image, label, filepath):
    plt.imshow(image, cmap='gray')
    plt.title(f'Label: {label}')
    plt.axis('off')
    plt.savefig(filepath)
    plt.close()

# Sauvegarder les images d'entraînement
for i, (image, label) in enumerate(zip(train_images, train_labels)):
    filepath = f'./dataset/mnist_images/train_{i}.png'
    save_image(image, label, filepath)

# Sauvegarder les images de test
for i, (image, label) in enumerate(zip(test_images, test_labels)):
    filepath = f'./dataset/mnist_images/test_{i}.png'
    save_image(image, label, filepath)

