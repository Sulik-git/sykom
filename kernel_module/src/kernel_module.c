#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h> 
#include <linux/sysfs.h>


MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jakub Sulikowski");
MODULE_DESCRIPTION("kernel module built for SYKT project");         //Informacje o module
MODULE_VERSION("0.01");

#define MY_SYSFSDRV_BUFFER_LEN 4096

#define SYKT_BUF_SIZE             100
#define SYKT_MEM_SIZE             16
#define SYKT_MEM_BASE_ADDR        (0x00100000)
#define SYKT_FINISHER_FAIL        (0x3333)
#define SYKT_QEMU_CTRL_ADDR       (0x00100000)
#define SYKT_GPIO_ADDRESS_SPACE   (baseptr)                    
#define SYKT_ID_ADDR              ((SYKT_GPIO_ADDRESS_SPACE) + 4)                       
#define SYKT_GPIO_A1_ADDR         ((SYKT_GPIO_ADDRESS_SPACE) + 0x430)  
#define SYKT_GPIO_A2_ADDR         ((SYKT_GPIO_ADDRESS_SPACE) + 0x438)       //Adresy odwołujące się do odpowiednich rejestrów
#define SYKT_GPIO_W_ADDR          ((SYKT_GPIO_ADDRESS_SPACE) + 0x440)  
#define SYKT_GPIO_L_ADDR          ((SYKT_GPIO_ADDRESS_SPACE) + 0x448)  
#define SYKT_GPIO_B_ADDR          ((SYKT_GPIO_ADDRESS_SPACE) + 0x450)  



void __iomem *baseptr;
static struct kobject *SYKT;    
static uint32_t sjda1;             
static uint32_t sjda2;            //Kolejne wartości czytanych rejestrów 
static uint32_t sjdw;              
static uint32_t sjdb;              
static uint32_t sjdl;              





static ssize_t sjda1_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    sjda1 = readl(SYKT_GPIO_A1_ADDR);
    return sprintf(buf, "0x%x\n", sjda1);
}

static ssize_t sjda1_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%x", &sjda1);
    writel(sjda1, SYKT_GPIO_A1_ADDR);
    int n = count > MY_SYSFSDRV_BUFFER_LEN ? MY_SYSFSDRV_BUFFER_LEN : count;
    return n;
}

static ssize_t sjda2_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    sjda2 = readl(SYKT_GPIO_A2_ADDR);
    return sprintf(buf, "0x%x\n", sjda2);
}

static ssize_t sjda2_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%x", &sjda2);
    writel(sjda2, SYKT_GPIO_A2_ADDR);
    int n = count > MY_SYSFSDRV_BUFFER_LEN ? MY_SYSFSDRV_BUFFER_LEN : count;
    return n;
}

static ssize_t sjdw_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    sjdw = readl(SYKT_GPIO_W_ADDR);
    return sprintf(buf, "0x%x\n", sjdw);
}

static ssize_t sjdb_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {            //Odczytanie oraz zapis wartości na 
                                                                                                    //podane rejestry
    sjdb = readl(SYKT_GPIO_B_ADDR);
    return sprintf(buf, "0x%x\n", sjdb);
}

static ssize_t sjdb_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%d", &sjdb);
    writel(sjdb, SYKT_GPIO_B_ADDR);
    int n = count > MY_SYSFSDRV_BUFFER_LEN ? MY_SYSFSDRV_BUFFER_LEN : count;
    return n;
}

static ssize_t sjdl_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    sjdl = readl(SYKT_GPIO_L_ADDR);
    return sprintf(buf, "0x%x\n", sjdl);
}


static struct kobj_attribute sjda1_attr   = __ATTR_RW(sjda1); 
static struct kobj_attribute sjda2_attr   = __ATTR_RW(sjda2); 
static struct kobj_attribute sjdw_attr  = __ATTR_RO(sjdw);
static struct kobj_attribute sjdb_attr  = __ATTR_RW(sjdb);
static struct kobj_attribute sjdl_attr = __ATTR_RO(sjdl); 


int my_init_module(void){
    printk(KERN_INFO "Starting SYKT module...\n");
    baseptr = ioremap(SYKT_MEM_BASE_ADDR, SYKT_MEM_SIZE);
    SYKT = kobject_create_and_add("sykt", kernel_kobj);
    if (!SYKT) {
        return -ENOMEM;
    }   


    if (baseptr == NULL) {
        printk(KERN_ERR "ioremap failed\n");
        return -EIO;
    }

    if (sysfs_create_file(SYKT, &sjda1_attr.attr)) {
        printk(KERN_INFO"Cannot create a1 sysfs file...\n");
    }

    if (sysfs_create_file(SYKT, &sjda2_attr.attr)) {
        printk(KERN_INFO"Cannot create a2 sysfs file...\n");
    }

    if (sysfs_create_file(SYKT, &sjdw_attr.attr)) {
        printk(KERN_INFO"Cannot create w sysfs file...\n");        //Obsługa wyjątków 
    }

    if (sysfs_create_file(SYKT, &sjdb_attr.attr)) {
        printk(KERN_INFO"Cannot create b sysfs file...\n");
    }

    if (sysfs_create_file(SYKT, &sjdl_attr.attr)) {
        printk(KERN_INFO"Cannot create l sysfs file...\n");
    }
    return 0;
}

void my_cleanup_module(void) {
    printk(KERN_INFO "Cleaning up SYKT module.\n");

    kobject_put(SYKT);
    writel(SYKT_FINISHER_FAIL | ((127) << 16), baseptr);     
    sysfs_remove_file(kernel_kobj, &sjda1_attr.attr);
	sysfs_remove_file(kernel_kobj, &sjda2_attr.attr);       
	sysfs_remove_file(kernel_kobj, &sjdw_attr.attr);
	sysfs_remove_file(kernel_kobj, &sjdb_attr.attr);
    sysfs_remove_file(kernel_kobj, &sjdl_attr.attr);
	iounmap(baseptr);
}

module_init(my_init_module);
module_exit(my_cleanup_module);