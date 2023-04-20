#generate images of digits from 0 to 9 of size 24x24
#save them in a folder named "GenratedDigits"
import numpy as np
import cv2
import os
from numpy import random

PATH = "/home/maclow/Documents/EPITA/S3#/Projet/NeuronalNetwork/dataset/normalizedSACHA/"

s1 = 48
s2 = 24

def main(num_nbr):
    #create a folder to store the generated digits
    if not os.path.exists(PATH):
        os.makedirs(PATH)

    while(num_nbr > 0):

        #loop over all the digits
        for i in range(10):
            img = np.zeros((s1,s1), np.uint8)
            #draw the digit on the image with random font
            font = random.choice([cv2.FONT_HERSHEY_SIMPLEX, cv2.FONT_HERSHEY_DUPLEX, cv2.FONT_HERSHEY_COMPLEX, cv2.FONT_HERSHEY_TRIPLEX, cv2.FONT_HERSHEY_SCRIPT_SIMPLEX, cv2.FONT_HERSHEY_SCRIPT_COMPLEX])
            font_scale = random.uniform(0.6, 1.7)
            thickness = random.randint(1, 4)
            cv2.putText(img, str(i), (14,38), font, font_scale, (255,255,255), thickness)


            #add some noise
            for t in range(random.randint(0, 100)):
                img[random.randint(0,s1-1)][random.randint(0,s1-1)] = random.randint(0, 255)

            #add some lines
            if random.random() > 1 :
            	for t in range(1):
            		cv2.line(img, (random.randint(0,s1-1), random.randint(0,s1-1)), (random.randint(0,s1-1), random.randint(0,s1-1)), (255,255,255), 1)            
            
            #resize the image
            img = cv2.resize(img, (s2,s2))
            #img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)[1] / 255.0


            #save the image
            cv2.imwrite(PATH + str(i) + "_" + str(num_nbr) + ".png", img)
        num_nbr -= 1

main(4000)
