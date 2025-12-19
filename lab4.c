#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/math64.h>

#define PROC_FILENAME "tsulab"

static char time_buffer[128];
static int time_buffer_len;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Govorov Aleksandr");
MODULE_DESCRIPTION("Phone falling from Ostankino Tower");
MODULE_VERSION("2.0");

void calc_time(char* buffer, size_t buffer_size) {
    u64 height_cm = 54000;
    u64 g_cm = 980;

    u64 t_squared = div_u64(2 * height_cm * 100, g_cm);
    u32 t_cs = int_sqrt(t_squared);

    int seconds = t_cs / 100;
    int hundredths = t_cs % 100;

    if (seconds == 1) {
        snprintf(buffer, buffer_size,
            "Телефон упадет на землю через 1 секунду\n"
            "Будет падать 1 секунду\n");
    }
    else if (seconds >= 2 && seconds <= 4) {
        snprintf(buffer, buffer_size,
            "Телефон упадет на землю через %d секунды\n"
            "Будет падать %d секунды\n",
            seconds, seconds);
    }
    else {
        snprintf(buffer, buffer_size,
            "Телефон упадет на землю через %d секунд\n"
            "Будет падать %d секунд\n",
            seconds, seconds);
    }
}

static ssize_t procfile_read(struct file* file_pointer, char __user* buffer,
    size_t buffer_len, loff_t* offset) {
    if (*offset > 0 || buffer_len < time_buffer_len)
        return 0;

    if (*offset == 0) {
        calc_time(time_buffer, sizeof(time_buffer));
        time_buffer_len = strlen(time_buffer);
    }

    if (copy_to_user(buffer, time_buffer + *offset, time_buffer_len - *offset))
        return -EFAULT;

    pr_info("Чтение procfile %s\n", file_pointer->f_path.dentry->d_name.name);

    *offset += time_buffer_len - *offset;

    return time_buffer_len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

int __init my_module_init(void) {
    pr_info("Welcome to the Tomsk State University\n");

    proc_create(PROC_FILENAME, 0, NULL, &proc_file_fops);

    pr_info("/proc/%s создан\n", PROC_FILENAME);
    return 0;
}

void __exit my_module_exit(void) {
    remove_proc_entry(PROC_FILENAME, NULL);
    pr_info("Tomsk State University forever!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);