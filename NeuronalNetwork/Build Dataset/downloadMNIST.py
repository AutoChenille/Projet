import os
from emnist import extract_training_samples, extract_test_samples
import cv2

# Créer un dossier pour stocker les images
if not os.path.exists('emnist_images'):
    os.makedirs('emnist_images')

# Extraire les données d'entraînement
train_images, train_labels = extract_training_samples('digits')

# Enregistrer les images d'entraînement
for i, image in enumerate(train_images):
    label = train_labels[i]
    filename = f'emnist_images/{label}_{i}.png'
    resized_image = cv2.resize(image, (24, 24), interpolation=cv2.INTER_AREA)
    cv2.imwrite(filename, resized_image)

# Extraire les données de test
test_images, test_labels = extract_test_samples('digits')

# Enregistrer les images de test
for i, image in enumerate(test_images):
    label = test_labels[i]
    filename = f'emnist_images/{label}_{i}.png'
    resized_image = cv2.resize(image, (24, 24), interpolation=cv2.INTER_AREA)
    cv2.imwrite(filename, resized_image)
