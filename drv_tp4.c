

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

#define GPIO_IN 20



/*Estructuras para el montado del modulo*/
dev_t dev_tp4;
static struct class *class_tp4;
static struct cdev cdev_tp4;

/*Variables para configuracion de GPIO e interrupciones*/
unsigned int GPIO_irqNumber;

/*Variables para lograr la funcionalidad del modulo*/
uint8_t pulsaciones;

/* Define GPIOs for BUTTONS */
static struct gpio botones[] = {
		{ 22, GPIOF_IN, "BUTTON 1" },
};

/*Funciones del driver y de las interrupciones*/ 
static irqreturn_t gpio_irq_handler(int irq,void *dev_id){

    printk(KERN_INFO "DRV_TP4: Recibi interrupcion.\n");

    pulsaciones++;

    return IRQ_HANDLED;

}

static int drv_tp4_open(struct inode *inode, struct file *file){

    printk(KERN_INFO "DRV_TP4: Open.\n");
    return 0;

}

static int drv_tp4_release(struct inode *inode, struct file *file){

    printk(KERN_INFO "DRV_TP4: Close.\n");
    return 0;

}

static ssize_t drv_tp4_read(struct file *filp, char __user *buf, size_t len, loff_t * off){

    printk(KERN_INFO "DRV_TP4: Read.\n");

    if(copy_to_user(buf, &pulsaciones, len) > 0) {
        printk(KERN_INFO "DRV_TP4: Error en copy to user.\n");
    }

    return 0;

}

static ssize_t drv_tp4_write(struct file *filp, const char __user *buf, size_t len, loff_t * off){

    printk(KERN_INFO "DRV_TP4: Write.\n");

    pulsaciones = 0;

    return 0;

}

/*Estructura que indica a que funciones apuntaran los llamados al driver*/
static struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = drv_tp4_read,
    .write          = drv_tp4_write,
    .open           = drv_tp4_open,
    .release        = drv_tp4_release,
};

/*Funcion de cargado del modulo, llamada cuando se utiliza el insmod*/
static int __init drv_tp4_init(void)
{

    /*Pasos necesarios para crear y registrar el CD en el sistema*/

    printk(KERN_INFO "DRV_TP4: Module Init.\n");

    //Alocamos un rango de numeros de CD (Major, Minor) 
    if((alloc_chrdev_region(&dev_tp4, 0, 1, "drv_tp4")) <0){
      
        printk(KERN_INFO "DRV_TP4: No se pudo alocar el Major par el CD.\n");
        goto r_unreg;
    }
    printk(KERN_INFO "Major = %d Minor = %d.\n",MAJOR(dev_tp4), MINOR(dev_tp4));
 
    //Iniciamos la estructura cdev con las operaciones
    cdev_init(&cdev_tp4,&fops);
 
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
  
    
    //Solicitamos el puerto GPIO
    if(gpio_request_array(botones, ARRAY_SIZE(botones)) != 0){
        printk(KERN_INFO "DRV_TP4: Fallo en la solicitud de GPIO %d.\n", GPIO_IN);
        goto r_gpio;
    }
  
    //Configuramos el puerto GPIO como entrada, algunos frameworks de gpio.h lo piden explicitamente por mas que se indique en la funcion anterior
    //gpio_direction_input(GPIO_IN);
    
    //Seteamos el tiempo de debounce del puerto
    if(gpio_set_debounce(GPIO_IN, 100) < 0){
        printk(KERN_INFO "DRV_TP4: Fallo en el set del debounce del puerto %d.\n", GPIO_IN);
        goto r_gpio;
    }

    //Obtenemos el valor de interrupcion para el puerto determinado
    GPIO_irqNumber = gpio_to_irq(botones[0].gpio);

    if(GPIO_irqNumber < 0) {
		printk(KERN_INFO "DRV_TP4: no se pudo asociar irq %d.\n", GPIO_irqNumber);
		goto r_gpio;
	}
    
    //Seteamos el handler de las interrupciones
    if (request_irq(GPIO_irqNumber,             //IRQ number
                    gpio_irq_handler,           //IRQ handler
                    IRQF_TRIGGER_RISING,        //Handler will be called in raising edge
                    "drv_tp4",                  //used to identify the device name using this IRQ
                    NULL)) {                    //device id for shared IRQ
        printk(KERN_INFO "DRV_TP4: No se pudo setear el handler de IRQ.\n");
        goto r_gpio;
    }

    //Iniciamos a 0 el numero de veces que se presiono el boton
    pulsaciones = 0;
    
    printk(KERN_INFO "DRV_TP4: Modulo cargado correctamente.\n");
    return 0;
 
    r_gpio:
    gpio_free(GPIO_IN);
    r_device:
    device_destroy(class_tp4,dev_tp4);
    r_class:
    class_destroy(class_tp4);
    r_del:
    cdev_del(&cdev_tp4);
    r_unreg:
    unregister_chrdev_region(dev_tp4,1);
    printk(KERN_INFO "DRV_TP4: Fallo al cargar el modulo.\n");
    
    return -1;
}

/*Funcion de borrado del modulo, llamada cuando se utiliza el rmmod*/
static void __exit drv_tp4_exit(void)
{
        
    free_irq(GPIO_irqNumber,NULL);
    gpio_free(GPIO_IN);
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