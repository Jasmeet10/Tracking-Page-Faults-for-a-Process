#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include<linux/syscalls.h>
#include <linux/slab.h>

#define MAX_SYMBOL_LEN	64
#define PROBE_NAME "pf_probe_I1"
int process_ID;
static int number_of_bytes = 0;
module_param(process_ID,int, 0644);
static char* kbuffer = NULL;
struct timespec ctime; // struct to calculate time.
struct proc_dir_entry *dev_file_entry;
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

static ssize_t device_read(struct file *filp,char *buffer,size_t length,loff_t * offset)
{
	int i = copy_to_user(buffer, kbuffer, strlen(kbuffer));
	return i;
}

static struct file_operations fops = {
  .read = device_read,
};

/* For each probe you need to allocate a kprobe structure */
static struct kprobe kp = {
	.symbol_name = "handle_mm_fault",
};

/* kprobe pre_handler: called just before the probed instruction is executed */
/*if the PID matches then only print the virtual address, in command line argument*/

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	#ifdef CONFIG_X86
		getnstimeofday(&ctime);
		if(current->pid == process_ID){
			char temp[100] = {0};
			int size;
			
			//regs->si -- capturing virtual address
			printk(KERN_INFO "<%s> pre_handler: p->addr = 0x%p, virtual_address = %lu time = %lu\n",
			p->symbol_name, p->addr, regs->si, ctime.tv_nsec);
			
			size = snprintf(temp, 50, "%lu %lu\n", ctime.tv_nsec,regs->si);
			
			number_of_bytes += (size+1);
			if(kbuffer == NULL){
				kbuffer = (char*)kzalloc(sizeof(char)* number_of_bytes, GFP_KERNEL);
			}
			else{
				kbuffer = (char*)krealloc(kbuffer, number_of_bytes, GFP_KERNEL);
			}
			strcat(kbuffer, temp);
		}
	#endif
	return 0;
}

static int __init kprobe_init(void)
{
	int ret;
	kp.pre_handler = handler_pre;
	kp.post_handler = NULL;
	kp.fault_handler = NULL;

	//kbuffer = NULL;
	dev_file_entry = proc_create(PROBE_NAME, 0755, NULL, &fops);

	//checking if proc file was created.
	if(dev_file_entry == NULL){
		remove_proc_entry(PROBE_NAME, NULL);
		printk(KERN_ERR "Error: Could not initialize /proc/%s file\n", PROBE_NAME);
		return -1;
	}

	ret = register_kprobe(&kp);
	if (ret < 0) {
		printk(KERN_ERR "register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	printk(KERN_INFO "Planted kprobe at %p\n", kp.addr);
	return 0;
}

static void __exit kprobe_exit(void)
{
	kfree(kbuffer);
	remove_proc_entry(PROBE_NAME, NULL);
	unregister_kprobe(&kp);
	printk(KERN_INFO "kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");