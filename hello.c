
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// SPDX-License-Identifier: BSD-2-Clause
/*
 * Simple Linux Kernel Module Example
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/list.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

struct TimeNode {
	struct list_head list;
	ktime_t timestamp;
};

static unsigned int hello_count = 1;
static LIST_HEAD(list_head);

static void append_time(void)
{
	struct TimeNode *new_node = kmalloc(sizeof(struct TimeNode), GFP_KERNEL);

	if (!new_node) {
		pr_err("Memory allocation failed for new_node\n");
		return;
	}

	new_node->timestamp = ktime_get();
	list_add_tail(&new_node->list, &list_head);
}

static void show_time(ktime_t time)
{
	pr_info("Timestamp: %lld ns\n", ktime_to_ns(time));
}

static void delete_list(void)
{
	struct TimeNode *current_node, *tmp;

	list_for_each_entry_safe(current_node, tmp, &list_head, list) {
		show_time(current_node->timestamp);
		list_del(&current_node->list);
		kfree(current_node);
	}
}

static int __init hello_init(void)
{
	int i;

	if (hello_count > 10) {
		pr_err("Err: hello_count too high (%d > 10)\n", hello_count);
		return -EINVAL;
	}

	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10))
		pr_warn("Warning: hello_count = %d (should be between 1 and 5)\n",
			hello_count);

	for (i = 0; i < hello_count; i++) {
		append_time();
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	delete_list();
}

module_param(hello_count, uint, 0444);
MODULE_PARM_DESC(hello_count, "Number of times to print 'Hello, world!'");

module_init(hello_init);
module_exit(hello_exit);

