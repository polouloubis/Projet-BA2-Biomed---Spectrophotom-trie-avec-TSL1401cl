import serial
arduino = serial.Serial('COM4', 115200, timeout =.1)
import matplotlib.pyplot as plt
import math

def new_list(texte):
    """
    Traduit les données envoyées par l'arduino en une liste de tensions
    """
    string=texte.decode('utf8')
    liste=[0]
    tension=[]
    i=0
    j=0
    while i<len(string):
        if string[i]==" ":
            liste.append(int(0))
            i+=1
            j+=1
        else:
            liste[j]=liste[j]*10+int(string[i])
            i+=1
    for i in liste:
         a=i
         tension.append(a)
    return tension

def lecture_capteur(Input):
    """
    Jette les 50 premières lignes analysées par le capteur puis lis les 200 suivantes.
    """
    i=0
    j=0
    intensite=[]
    if Input =='0':
        arduino.write(b'h')
        while i<25:    #nombre de lignes jettées
            garbage=arduino.readline()[:-2]
            if len(garbage)!=0:
                i+=1
        while j<100:    #nombre de lignes lues 
            x=arduino.readline()[:-2]
            if len(x)!=0:
                j+=1
                if len(intensite)==0:
                    donnees=new_list(x)
                    for n in range(128):
                        intensite.append(donnees[n])
                else:
                    données=new_list(x)
                    for k in range(128):
                       intensite[k]+=donnees[k]
        for m in range(128):
            intensite[m]=intensite[m]/100
    arduino.flushInput()
    return intensite


def absorbance(I_0,I):
    """
    Calcule l'absorbance de la solution et en renvoie le graphe.
    """
    absorbance=[]
    for i in range(128):
          A= math.log10(I_0[i]/I[i])
          absorbance.extend([A, A])
          
    axe_x=[]
    i=300
    while i<700:
       axe_x.append(i)
       i+=1.5625

          
    #graphique
    plt.plot(axe_x, absorbance)
    plt.ylabel("absorbance")
    plt.xlabel("longueur d'onde (nm)")
    plt.show()

    return absorbance


#interface
def input_msg_test_vide():
    return input("Appuyez sur [0] pour effectuer le test à vide.\n ")

def input_msg_test_sol():
    return input("Ajoutez la solution. \nAppuyez sur [0] pour effectuer le test avec la solution.\n")

def input_msg_calibration():
    return input("Appuyez sur [0] pour lancer la calibration. \n")

def test():
    I_0=lecture_capteur(input_msg_test_vide())
    print(I_0)
    I=lecture_capteur(input_msg_test_sol())
    print(I)
    abso=absorbance(I_0,I)


def calibre():
    I=lecture_capteur(input_msg_calibration())
    print(I)
    IGraphe(I)

