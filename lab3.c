#include <linux/module.h>
#include <linux/printk.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Govorov Aleksandr");
MODULE_DESCRIPTION("Falling of the phone from astankinski tower");
MODULE_VERSION("1.0");

int __init my_module_init(void){
  pr_info("Welcome to the Tomsk State University\n");
  return 0;
}

void __exit my_module_exit(void){
  pr_info("Tomsk State University forever!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
