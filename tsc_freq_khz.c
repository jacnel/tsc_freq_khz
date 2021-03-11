#include <linux/cpu.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/arch/x86/include/asm/tsc.h>
extern unsigned int tsc_khz;
#include <linux/kobject.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/sysfs.h>

#define DRIVER_NAME "tsc_freq_khz"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacob Nelson");
MODULE_DESCRIPTION("Exports tsc_khz via syfs");
MODULE_VERSION("0.1");

static ssize_t tsc_freq_khz_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf) {
  ssize_t r;
  r = sprintf(buf, "%d\n", tsc_khz);
  return r;
}

static ssize_t tsc_freq_khz_store(struct kobject *kobj,
                                  struct kobj_attribute *attr, const char *buf,
                                  size_t count) {
  // unimplemented
  return count;
}

static struct kobj_attribute tsc_freq_khz_attribute =
    __ATTR(tsc_freq_khz,
           S_IRUGO, // world readable, unchangeable
           tsc_freq_khz_show, tsc_freq_khz_store);

static int __init tsc_khz_init(void) {
  int error = -ENOENT;
  int cpu_id = -1;
  struct device *dev;
  printk(KERN_INFO DRIVER_NAME ": starting driver\n");
  for_each_cpu(cpu_id, cpu_online_mask) {
    dev = get_cpu_device(cpu_id);
    error = sysfs_create_file(&dev->kobj, &tsc_freq_khz_attribute.attr);
    if (error) {
      printk(KERN_INFO DRIVER_NAME ": could not register with CPU %d (% d)\n ",
             cpu_id, error);
      return error;
    } else {
      printk(KERN_INFO DRIVER_NAME ": successfully registered with CPU %d\n",
             cpu_id);
    }
  }

  return error;
}

static void __exit tsc_khz_exit(void) {
  struct device *dev;
  int cpu_id;
  printk(KERN_INFO DRIVER_NAME ": unloading driver\n");
  for_each_cpu(cpu_id, cpu_online_mask) {
    dev = get_cpu_device(cpu_id); 
    if (!dev) {
      printk(KERN_INFO DRIVER_NAME ": could not get device for CPU %d\n", cpu_id);
      return;
    }
    sysfs_remove_file(&dev->kobj, &tsc_freq_khz_attribute.attr);
  }
}

module_init(tsc_khz_init);
module_exit(tsc_khz_exit);