#generate images of digits from 0 to 9 of size 24x24
#save them in a folder named "GenratedDigits"
import numpy as np
import cv2
import os
from numpy import random


def main(num_nbr):
    #create a folder to store the generated digits
    if not os.path.exists("GeneratedDigits"):
        os.makedirs("GeneratedDigits")

    while(num_nbr > 0):

        #loop over all the digits
        for i in range(10):
            img = np.zeros((48,48), np.uint8)
            #draw the digit on the image with random font
            font = random.choice([cv2.FONT_HERSHEY_SIMPLEX, cv2.FONT_HERSHEY_PLAIN, cv2.FONT_HERSHEY_DUPLEX, cv2.FONT_HERSHEY_COMPLEX, cv2.FONT_HERSHEY_TRIPLEX, cv2.FONT_HERSHEY_COMPLEX_SMALL, cv2.FONT_HERSHEY_SCRIPT_SIMPLEX, cv2.FONT_HERSHEY_SCRIPT_COMPLEX])
            font_scale = random.uniform(1.4, 1.7)
            thickness = random.randint(1, 4)
            cv2.putText(img, str(i), (14,38), font, font_scale, (255,255,255), thickness)


            #add some noise
            for t in range(100):
                img[random.randint(0,47)][random.randint(0,47)] = 255

            #add some lines
            for t in range(1):
                cv2.line(img, (random.randint(0,47), random.randint(0,47)), (random.randint(0,47), random.randint(0,47)), (255,255,255), 1)            
            
            #resize the image
            img = cv2.resize(img, (24,24))

            #save the image
            cv2.imwrite("/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/SACHA/" + str(i) + "_" + str(num_nbr) + ".png", img)
        num_nbr -= 1

main(1000)
