#generate images of digits from 0 to 9 of size 24x24
#save them in a folder named "GenratedDigits"
import numpy as np
import cv2
import os
from numpy import random

PATH_TRAIN = "../../bestDataset/hexadoku/train/"
PATH_TEST = "../../bestDataset/hexadoku/test/"

s1 = 48
s2 = 24

for n in range(2):
    PATH = PATH_TRAIN if n==1 else PATH_TEST

    def main(num_nbr, hexa=False):
        #create a folder to store the generated digits
        if not os.path.exists(PATH):
            os.makedirs(PATH)

        while(num_nbr > 0):

            #loop over all the digits
            for i in range(16 if hexa else 10):
                img = np.zeros((s1,s1), np.uint8)
                #draw the digit on the image with random font
                if i != 0:
                    font = random.choice([cv2.FONT_HERSHEY_SIMPLEX, cv2.FONT_HERSHEY_DUPLEX, cv2.FONT_HERSHEY_COMPLEX, cv2.FONT_HERSHEY_TRIPLEX])
                    font_scale = random.uniform(0.7, 1.7)
                    thickness = random.randint(1, 4)
                    posx, posy = random.randint(10, 18), random.randint(34, 42)
                    cv2.putText(img, str(i) if i <= 9 else chr(i-10+ord('A')), (posx,posy), font, font_scale, (255,255,255), thickness)


                #add some noise
                for t in range(random.randint(0, 50)):
                    img[random.randint(0,s1-1)][random.randint(0,s1-1)] = random.randint(0, 50)

                #add some lines
                if random.random() > 0.5 :
                    #ligne gauche
                    x, y1, y2 = random.randint(0,10), random.randint(0,10), random.randint(38, 48)
                    cv2.line(img, (x, y1), (x, y2), (255,255,255), random.randint(1, 4))   
                if random.random() > 0.5 :
                    #ligne droite
                    x, y1, y2 = random.randint(38,48), random.randint(0,10), random.randint(38, 48)
                    cv2.line(img, (x, y1), (x, y2), (255,255,255), random.randint(1, 4))    
                if random.random() > 0.5 :
                    #ligne haut
                    y, x1, x2 = random.randint(0,10), random.randint(0,10), random.randint(38, 48)
                    cv2.line(img, (x1, y), (x2, y), (255,255,255), random.randint(1, 4))    
                if random.random() > 0.5 :
                    #ligne bas
                    y, x1, x2 = random.randint(38,48), random.randint(0,10), random.randint(38, 48)
                    cv2.line(img, (x1, y), (x2, y), (255,255,255), random.randint(1, 4))         
                
                #resize the image
                img = cv2.resize(img, (s2,s2))
                #img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)[1] / 255.0


                #save the image
                name = PATH + (str(i) if i <= 9 else chr(i-10+ord('A'))) + "_" + str(num_nbr) + ".png"
                cv2.imwrite(name, img)
                #print("wrote :", name)
            num_nbr -= 1

    main(8000 if n==1 else 50, hexa=True)
