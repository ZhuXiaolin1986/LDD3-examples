/*
 * complete.c -- the writers awake the readers
 *
 * Copyright (C) 2003 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2003 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: complete.c,v 1.2 2004/09/26 07:02:43 gregkh Exp $
 */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/sched.h>  /* current and everything */
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/types.h>  /* size_t */
#include <linux/completion.h>
#include <linux/device.h>

MODULE_LICENSE("Dual BSD/GPL");

static int complete_major = 0;
static struct class *complete_class = NULL;

DECLARE_COMPLETION(comp);

ssize_t complete_read (struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	printk(KERN_DEBUG "process %i (%s) going to sleep\n",
			current->pid, current->comm);
	wait_for_completion(&comp);
	printk(KERN_DEBUG "awoken %i (%s)\n", current->pid, current->comm);
	return 0; /* EOF */
}

ssize_t complete_write (struct file *filp, const char __user *buf, size_t count,
		loff_t *pos)
{
	printk(KERN_DEBUG "process %i (%s) awakening the readers...\n",
			current->pid, current->comm);
	complete(&comp);
	return count; /* succeed, to avoid retrial */
}


struct file_operations complete_fops = {
	.owner = THIS_MODULE,
	.read =  complete_read,
	.write = complete_write,
};


int complete_init(void)
{
	int result;

	/*
	 * Register your major, and accept a dynamic number
	 */
	result = register_chrdev(complete_major, "complete", &complete_fops);
	if (result < 0)
		return result;
	if (complete_major == 0)
		complete_major = result; /* dynamic */

	complete_class = class_create(THIS_MODULE, "complete");
	if(complete_class)
		device_create(complete_class, NULL, MKDEV(complete_major, 0), NULL, "complete");

	return 0;
}

void complete_cleanup(void)
{
	if(complete_class) {
		device_destroy(complete_class, MKDEV(complete_major, 0));
		class_destroy(complete_class);
	}
	unregister_chrdev(complete_major, "complete");
}

module_init(complete_init);
module_exit(complete_cleanup);

