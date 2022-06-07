

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  
#include <linux/gpio.h>     
#include <linux/interrupt.h>



/*Estructuras para el montado del modulo*/
dev_t dev_tp4;
static struct class *class_tp4;
static struct cdev cdev_tp4;

/*Variables para configuracion de GPIO e interrupciones*/
unsigned int GPIO_irqNumber;

/*Estructura que indica a que funciones apuntaran los llamados al driver*/
static struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = etx_read,
    .write          = etx_write,
    .open           = etx_open,
    .release        = etx_release,
};

/*Funcion de cargado del modulo, llamada cuando se utiliza el insmod*/
static int __init drv_tp4_init(void)
{

    /*Pasos necesarios para crear y registrar el CD en el sistema*/

    //Alocamos un rango de numeros de CD (Major, Minor) 
    if((alloc_chrdev_region(&dev_tp4, 0, 1, "drv_tp4")) <0){
      
        printk(KERN_INFO "DRV_TP4: No se pudo alocar el Major par el CD.\n");
        goto r_unreg;
    }
    printk(KERN_INFO "Major = %d Minor = %d.\n",MAJOR(dev_tp4), MINOR(dev_tp4));
 
    //Iniciamos la estructura cdev con las operaciones
    cdev_init(&etx_cdev,&fops);
 
    //Agregamos el CD al sistema, asociado al Major obtenido
    if((cdev_add(&cdev_tp4,dev_tp4,1)) < 0){
        printk(KERN_INFO "DRV_TP4: No se pudo agregar el CD al sistema.\n");
        goto r_del;
    }
 
    //Creamos la clase
    if((class_tp4 = class_create(THIS_MODULE,"drv_tp4_class")) == NULL){
        printk(KERN_INFO "DRV_TP4: No se pudo crear la clase.\n");
        goto r_class;
    }
 
    //Creamos el dispositivo, le asociamos el Major y la clase creada
    if((device_create(class_tp4,NULL,dev_tp4,NULL,"drv_tp4")) == NULL){
        printk(KERN_INFO "DRV_TP4: No se pudo crear el Device.\n");
        goto r_device;
    }

    /*Pasos necesarios para reservar y configurar puertos GPIO, asi como sus interrupciones*/
  
    //Chequeamos que el puerto gpio este disponible
    if(gpio_is_valid(GPIO_20) == false){
        printk(KERN_INFO "DRV_TP4: GPIO %d no valido.\n", GPIO_20);
        goto r_device;
    }
    
    //Solicitamos el puerto GPIO
    if(gpio_request(GPIO_20, GPIOF_IN) < 0){
        printk(KERN_INFO "DRV_TP4: Fallo en la solicitud de GPIO %d.\n", GPIO_20);
        goto r_gpio;
    }
  
    //Configuramos el puerto GPIO como entrada, algunos frameworks de gpio.h lo piden explicitamente por mas que se indique en la funcion anterior
    //gpio_direction_input(GPIO_20);
    
    //Seteamos el tiempo de debounce del puerto
    if(gpio_set_debounce(GPIO_20, 100) < 0){
        printk(KERN_INFO "DRV_TP4: Fallo en el set del debounce del puerto %d.\n", GPIO_20);
        goto r_gpio;
    }

    //Obtenemos el valor de interrupcion para el puerto determinado
    GPIO_irqNumber = gpio_to_irq(GPIO_20_IN);
    
    //Seteamos el handler de las interrupciones
    if (request_irq(GPIO_irqNumber,             //IRQ number
                    (void *)gpio_irq_handler,   //IRQ handler
                    IRQF_TRIGGER_RISING,        //Handler will be called in raising edge
                    "drv_tp4",                  //used to identify the device name using this IRQ
                    NULL)) {                    //device id for shared IRQ
        printk(KERN_INFO "DRV_TP4: No se pudo setear el handler de IRQ.\n");
        goto r_gpio;
    }
    
    printk(KERN_INFO "DRV_TP4: Modulo cargado correctamente.\n")
    return 0;
 
    r_gpio:
    gpio_free(GPIO_20);
    r_device:
    device_destroy(class_tp4,dev_tp4);
    r_class:
    class_destroy(class_tp4);
    r_del:
    cdev_del(&cdev_tp4);
    r_unreg:
    unregister_chrdev_region(dev_tp4,1);
    printk(KERN_INFO "DRV_TP4: Fallo al cargar el modulo.\n")
    
    return -1;
}

/*Funcion de borrado del modulo, llamada cuando se utiliza el rmmod*/
static void __exit drv_tp4_exit(void)
{
  free_irq(GPIO_irqNumber,NULL);
  gpio_free(GPIO_20);
  device_destroy(class_tp4,dev_tp4);
  class_destroy(class_tp4);
  cdev_del(&cdev_tp4);
  unregister_chrdev_region(dev_tp4, 1);
  pr_info("DRV_TP4: Modulo quitado correctamente.\n");
}


module_init(drv_tp4_init);
module_exit(drv_tp4_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ASMKiller");
MODULE_DESCRIPTION("Modulo resolucion del TP4 de SdC");
MODULE_VERSION("1");