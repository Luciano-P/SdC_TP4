
import time
import numpy
import matplotlib.pyplot as plt


def get_v():

    f = open("/dev/drv_tp4", "r", encoding = 'raw_unicode_escape')
    lista = f.readline(12)
    f.close()

    g = open("/dev/drv_tp4", "w",)
    g.write("a")
    g.close()

    switch = ord(lista[0])
    grapha = ord(lista[4]) + (ord(lista[5]) * 256) + (ord(lista[6]) * 65536) + (ord(lista[7]) * 16777216)
    graphb = ord(lista[8]) + (ord(lista[9]) * 256) + (ord(lista[10]) * 65536) + (ord(lista[11]) * 16777216)

    valores = [switch, grapha, graphb]

    return valores


def update(arr,x): #Actualizacion del array, elimino valor antiguo, ingreso x
    del arr[0]
    arr.append(x)


def graficar(pulses, title):
    plt.xlim(0,10)
    plt.title(title)
    plt.xlabel("Tiempo")
    plt.ylabel("Pulsaciones")
    plt.bar(list(range(10)) ,pulses)
    
    plt.show()



pulsador0 = [0,0,0,0,0,0,0,0,0,0]
pulsador1 = [0,0,0,0,0,0,0,0,0,0]


while True:

    valores = get_v()

    update(pulsador0, valores[1])
    update(pulsador1, valores[2])

    print(pulsador0)
    print(pulsador1)    

    if valores[0] == 0:
        graficar(pulsador0, "pulsador0")
    else:
        graficar(pulsador1, "pulsador1")


    time.sleep(2)
