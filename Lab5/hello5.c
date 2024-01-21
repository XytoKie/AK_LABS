#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

struct timing_data {
    struct list_head list;
    ktime_t time_begin;
    ktime_t time_end;
};

MODULE_AUTHOR("IO-16 Vasyl");
MODULE_DESCRIPTION("Laba5");
MODULE_LICENSE("GPL");

static LIST_HEAD(record_list);

static uint count = 1; 
module_param(count, uint, 0444); 
MODULE_PARM_DESC(count, "count"); 

static int __init start_hello(void)
{
    uint i = 0; 
    struct timing_data *data;

    pr_info("count: %d\n", count); 

    BUG_ON(count == 0); 

    for (i = 0; i < count; i++) { 
        data = kmalloc(sizeof(struct timing_data), GFP_KERNEL);

        if (i == 5) 
            data = NULL;

        if (ZERO_OR_NULL_PTR(data))
            goto error_handling;

        data->time_begin = ktime_get();
        pr_info("Hello, world!\n");
        data->time_end = ktime_get();

        list_add_tail(&data->list, &record_list);
    }

    return 0;

error_handling:
    {
        struct timing_data *temp_data, *safe_data;

        pr_err("Memory allocation failed...\n");

        list_for_each_entry_safe(temp_data, safe_data, &record_list, list) {
            list_del(&temp_data->list);
            kfree(temp_data);
        }

        BUG();
        return -ENOMEM;
    }
}

static void __exit stop_hello(void)
{
    struct timing_data *temp_data, *safe_data;

    list_for_each_entry_safe(temp_data, safe_data, &record_list, list) {
        pr_info("Time taken: %lld", temp_data->time_end - temp_data->time_begin);

        list_del(&temp_data->list);
        kfree(temp_data);
    }

    BUG_ON(!list_empty(&record_list));
}

module_init(start_hello);
module_exit(stop_hello);

