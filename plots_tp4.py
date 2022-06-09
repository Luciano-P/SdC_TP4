import time
import numpy
import matplotlib.pyplot as plt

# Se saca del archivo el valor de los 3 pulsadores y se lo guarda en un array
def get_v():
    f = open("/dev/drv_tp4", "rb")
    lista = f.readline(12)
    f.close()

    g = open("/dev/drv_tp4", "w")
    g.write("a")
    g.close()

    switch = lista[0]
    grapha = lista[4]
    graphb = lista[8]

    valores = [switch, grapha, graphb]

    return valores

# Actualizacion del array, elimino valor antiguo, ingreso x
def update(arr,x):
    del arr[0]
    arr.append(x)

# Se grafica los valores del pulsador solicitado
def graficar(pulses, title):
    plt.xlim(0,10)
    plt.title(title)
    plt.xlabel("Tiempo")
    plt.ylabel("Pulsaciones")
    plt.bar(list(range(10)) ,pulses)    
    plt.show()

# Se crea un array de 10 ceros para los dos posibles valores del switch
pulsador0 = [0,0,0,0,0,0,0,0,0,0]
pulsador1 = [0,0,0,0,0,0,0,0,0,0]


while True:
    valores = get_v()

    update(pulsador0, valores[1])
    update(pulsador1, valores[2])

    if valores[0] == 0:
        graficar(pulsador0, "pulsador0")
        print("El switch vale: %d.", valores[0])
        print("El pulsador 0 tiene el siguiente valor:")
        print(pulsador0)
    else:
        graficar(pulsador1, "pulsador1")
        print("El switch vale: %d.", valores[0])
        print("El pulsador 1 tiene el siguiente valor:")
        print(pulsador1)

    time.sleep(2)
