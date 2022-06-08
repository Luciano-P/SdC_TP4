import matplotlib.pyplot as plt
import time

def update(arr,x): # Actualizacion del array, elimino valor antiguo, ingreso x
    del arr[0]
    arr.append(x)

f = open("/dev/drv_tp4","r+")
r_values = []   # Valores leidos de /dev/drv_tp4
pulse0 = []     # Pulsadores
pulse1 = []
switch = 1      # Switch de grafico
pulses = []     # Pulsos a graficar

while True:
    # Read para leer el valor, write para ponerlo a 0
    r_values = f.read(3)
    f.write("000")

    # Actualizo los valores
    update(pulse0, r_values[0])
    update(pulse1, r_values[1])
    switch = r_values[2]

    # Eleccion de que pulsador graficar
    if switch == 0:
        pulses = pulse0
        title = "Pulsador 1"
    else:
        pulses = pulse1
        title = "Pulsador 2"

    # Ploteo
    plt.xlim(0,50)
    plt.ylim(0,20)
    plt.title(title)
    plt.xlabel("Tiempo")
    plt.ylabel("Pulsaciones")
    plt.bar(list(range(10)) ,pulses)
    plt.style.use('fivethirtyeight')
    plt.show()

    # Sleep de 5 segundos entre graficos
    time.sleep(5)

f.close()
