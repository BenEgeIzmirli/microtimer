#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ben Ege Izmirli (with help from Robert W. Oliver II)");
MODULE_DESCRIPTION("Times very short sets of x86 instructions");
MODULE_VERSION("0.01");

#define DEVICE_NAME "instrtmr"


/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;

#define num_regs 5
static u64 regs[num_regs];
#define bufflen 100
static char buffer_copy[bufflen];
static char* number_strings[num_regs];

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    error_count = copy_to_user(buffer, (void*)regs, 40);
  
    if (error_count==0){            // if true then have success
        printk(KERN_INFO "instrtmr: Sent %d characters to the user\n", 40);
    }
    else {
        printk(KERN_INFO "instrtmr: failed with error_count %d\n", error_count);
        return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
    }
    return 0;
}

// returns 0 if valid
static int validate_charset(const char *buffer, int len) {
    char* valid_chars = "0123456789abcdefABCDEFxX ";
    int fail_len = 0;
    int ct = 0;
    int char_ct;

    while ( valid_chars[fail_len++] );
    fail_len--;

    for ( ct = 0 ; ct < len ; ct++ ) {
        for ( char_ct = 0 ; valid_chars[char_ct] ; char_ct++ ) {
            if ( valid_chars[char_ct] == buffer[ct] ) {
                break;
            }
        }
        if ( char_ct == fail_len ) {
            return -1;
        }
    }
    return 0;
}

// returns num characters provided if invalid, 0 if valid
static int validate_length(const char *buffer, int len) {
    if ( len >= bufflen ) {
        return len;
    }
    return 0;
}

static char** split_by_spaces(const char *buffer, int len) {
    int ptrct = 0;
    int ct = 0;
    int in_hex_str = 0;
    memcpy((void*)buffer_copy,(const void*)buffer,len);

    while ( ct < len ) {
        if ( buffer_copy[ct] == ' ' ) {
            buffer_copy[ct] = (char)0;
            in_hex_str = 0;
        }
        if ( ! in_hex_str ) {
            if ( ptrct == num_regs-1 ) {
                return (char**)0;
            }
            in_hex_str = 1;
            number_strings[ptrct++] = &buffer_copy[ct];
        }
        ct++;
    }
    buffer_copy[ct] = (char)0;
    return number_strings;
}

// returns 0 if invalid, 1 if valid
static int validate_writestr(const char *buffer,int len) {
    if ( validate_length(buffer,len) ) {
        printk(KERN_ALERT "Failed length validation.\n");
        return 0;
    }
    if ( ! validate_charset(buffer,len) ) {
        printk(KERN_ALERT "Failed charset validation.\n");
        return 0;
    }
    return 1;
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    char** number_ptrs;
    int i;
    if ( ! validate_writestr(buffer,(int)len) ) {
        printk(KERN_ALERT "Failed validation.\n");
        return -EFAULT;
    }
    number_ptrs = split_by_spaces(buffer,(int)len);
    if ( ! number_ptrs ) {
        printk(KERN_ALERT "Failed splitting.\n");
        return -EFAULT;
    }
    for ( i=0 ; i<4 ; i++ ) {
        if ( kstrtoull(number_ptrs[i],16,&regs[i]) ) {
            printk(KERN_ALERT "Failed conversion of number %d to ull.\n",i);
            return -EFAULT;
        }
    }
    printk(KERN_DEBUG "instrtmr: rax=%llx rbx=%llx rcx=%llx rdx=%llx\n",regs[0],regs[1],regs[2],regs[3]);
    return len;
}

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file) {

    /* If device is open, return busy */
    if (device_open_count) {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
    /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init instrtmr_init(void) {
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_INFO "instrtmr module loaded with device major number %d\n", major_num);
    }
    return 0;
}

static void __exit instrtmr_exit(void) {
    /* Remember — we have to clean up after ourselves. Unregister the character device. */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Unloading instrtmr module\n");
}

/* Register module functions */
module_init(instrtmr_init);
module_exit(instrtmr_exit);

