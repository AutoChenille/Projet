import os
from PIL import Image
import numpy as np
import shutil

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
    img = img.resize((24, 24), resample=Image.LANCZOS)
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

    # Parcours de tous les fichiers du répertoire
    for nom_fichier in os.listdir(dest_dir):
        # On ne traite que les fichiers (pas les sous-répertoires)
        if os.path.isfile(os.path.join(dest_dir, nom_fichier)):
            # Extraction du premier caractère du nom de fichier
            premier_caractere = nom_fichier[0]
            # Chemin du sous-répertoire correspondant au premier caractère
            chemin_sous_repertoire = os.path.join(dest_dir, premier_caractere)
            # Création du sous-répertoire s'il n'existe pas déjà
            if not os.path.exists(chemin_sous_repertoire):
                os.mkdir(chemin_sous_repertoire)
            # Déplacement du fichier dans le sous-répertoire
            chemin_fichier_source = os.path.join(dest_dir, nom_fichier)
            chemin_fichier_destination = os.path.join(chemin_sous_repertoire, nom_fichier)
            shutil.move(chemin_fichier_source, chemin_fichier_destination)

    # Parcours récursif de tous les fichiers de tous les sous-répertoires
    for repertoire, sous_repertoires, fichiers in os.walk(dest_dir):
        compteur = 0
        for nom_fichier in fichiers:
            # Extraction de l'extension du fichier
            nom_sans_extension, extension = os.path.splitext(nom_fichier)
            # Nouveau nom de fichier avec le numéro séquentiel et l'extension d'origine
            nouveau_nom_fichier = str(compteur) + extension
            # Chemin complet du fichier à renommer
            chemin_fichier_source = os.path.join(repertoire, nom_fichier)
            # Chemin complet du nouveau fichier renommé
            chemin_fichier_destination = os.path.join(repertoire, nom_sans_extension[0] + '_' + nouveau_nom_fichier)
            # Renommage du fichier
            os.rename(chemin_fichier_source, chemin_fichier_destination)
            # Incrémentation du compteur pour le numéro séquentiel
            compteur += 1

if __name__ == '__main__':
    src_dir = './dataset/SACHA/'
    dest_dir = './dataset/normalizedSACHA/'
    main(src_dir, dest_dir)