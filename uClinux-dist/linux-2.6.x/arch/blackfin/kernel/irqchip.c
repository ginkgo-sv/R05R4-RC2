/*
 * File:         arch/blackfin/kernel/irqchip.c
 * Based on:
 * Author:
 *
 * Created:
 * Description:  This file contains the simple DMA Implementation for Blackfin
 *
 * Rev:          $Id: irqchip.c,v 1.12 2005/11/30 12:25:04 hennerich Exp $
 *
 * Modified:
 *               Copyright 2004-2005 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http:    //blackfin.uclinux.org/
 *
 * This program is free software ;  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation ;  either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ;  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program ;  see the file COPYING.
 * If not, write to the Free Software Foundation,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>

#include <asm/irqchip.h>

/*#define IRQ_DEBUG*/
#undef IRQ_DEBUG

#ifdef IRQ_DEBUG
#define IRQ_DPRINTK(x...)	printk(x)
#else
#define IRQ_DPRINTK(x...)	do { } while (0)
#endif

/*
 * Maximum IRQ count.  Currently, this is arbitary. However, it should
 * not be set too low to prevent false triggering. Conversely, if it
 * is set too high, then you could miss a stuck IRQ.
 *
 * Maybe we ought to set a timer and re-enable the IRQ at a later time?
 */
#define MAX_IRQ_CNT	100000

static volatile unsigned long irq_err_count;
static spinlock_t irq_controller_lock = SPIN_LOCK_UNLOCKED;
static LIST_HEAD(irq_pending);

struct irqdesc irq_desc[NR_IRQS];

extern int init_arch_irq(void);
/*
 * Dummy mask/unmask handler
 */
void dummy_mask_unmask_irq(unsigned int irq)
{
}

irqreturn_t no_action(int irq, void *dev_id, struct pt_regs *regs)
{
	return IRQ_NONE;
}

void do_bad_IRQ(unsigned int irq, struct irqdesc *desc, struct pt_regs *regs)
{
	irq_err_count += 1;
	printk(KERN_ERR "IRQ: spurious interrupt %d\n", irq);
}

static struct irqchip bad_chip = {
	.ack = dummy_mask_unmask_irq,
	.mask = dummy_mask_unmask_irq,
	.unmask = dummy_mask_unmask_irq,
};

static struct irqdesc bad_irq_desc = {
	.chip = &bad_chip,
	.handle = do_bad_IRQ,
	.pend = LIST_HEAD_INIT(bad_irq_desc.pend),
	.disable_depth = 1,
};

/*
 *	disable_irq - disable an irq and wait for completion
 *	@irq: Interrupt to disable
 *
 *	Disable the selected interrupt line.  Enables and disables
 *	are nested. We do this lazily.
 *
 *	This function may be called from IRQ context.
 */
void disable_irq(unsigned int irq)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned long flags;
	spin_lock_irqsave(&irq_controller_lock, flags);
	if (!desc->disable_depth++) 
		desc->chip->mask(irq);
	list_del_init(&desc->pend);
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

EXPORT_SYMBOL(disable_irq);

/*
 *	enable_irq - enable interrupt handling on an irq
 *	@irq: Interrupt to enable
 *
 *	Re-enables the processing of interrupts on this IRQ line.
 *	Note that this may call the interrupt handler, so you may
 *	get unexpected results if you hold IRQs disabled.
 *
 *	This function may be called from IRQ context.
 */
void enable_irq(unsigned int irq)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned long flags;

	spin_lock_irqsave(&irq_controller_lock, flags);
	if (unlikely(!desc->disable_depth)) {
		printk("enable_irq(%u) unbalanced from %p\n", irq,
		       __builtin_return_address(0));
	} else if (!--desc->disable_depth) {
		desc->probing = 0;
		desc->chip->unmask(irq);

		/*
		 * If the interrupt is waiting to be processed,
		 * try to re-run it. We can't directly run it
		 * from here since the caller might be in an
		 * interrupt-protected region.
		 */
		if (desc->pending && list_empty(&desc->pend)) {
			desc->pending = 0;
			if (!desc->chip->retrigger
			    || desc->chip->retrigger(irq))
				list_add(&desc->pend, &irq_pending);
		}
	}
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

EXPORT_SYMBOL(enable_irq);

/*
 * Enable wake on selected irq
 */
void enable_irq_wake(unsigned int irq)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned long flags;

	spin_lock_irqsave(&irq_controller_lock, flags);
	if (desc->chip->wake)
		desc->chip->wake(irq, 1);
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

EXPORT_SYMBOL(enable_irq_wake);

void disable_irq_wake(unsigned int irq)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned long flags;

	spin_lock_irqsave(&irq_controller_lock, flags);
	if (desc->chip->wake)
		desc->chip->wake(irq, 0);
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

EXPORT_SYMBOL(disable_irq_wake);

int show_interrupts(struct seq_file *p, void *v)
{
	int i = *(loff_t *) v;
	struct irqaction *action;
	unsigned long flags;

	if (i < NR_IRQS) {
		spin_lock_irqsave(&irq_controller_lock, flags);
		action = irq_desc[i].action;
		if (!action)
			goto unlock;

		seq_printf(p, "%3d: %10u ", i, kstat_irqs(i));
		seq_printf(p, "  %s", action->name);
		for (action = action->next; action; action = action->next)
			seq_printf(p, ", %s", action->name);

		seq_putc(p, '\n');
	      unlock:
		spin_unlock_irqrestore(&irq_controller_lock, flags);
	} else if (i == NR_IRQS) {
		seq_printf(p, "Err: %10lu\n", irq_err_count);
	}
	return 0;
}

/*
 * IRQ lock detection.
 *
 * Hopefully, this should get us out of a few locked situations.
 * However, it may take a while for this to happen, since we need
 * a large number if IRQs to appear in the same jiffie with the
 * same instruction pointer (or within 2 instructions).
 */
static int check_irq_lock(struct irqdesc *desc, int irq, struct pt_regs *regs)
{
	unsigned long instr_ptr = instruction_pointer(regs);

	if (desc->lck_jif == jiffies &&
	    desc->lck_pc >= instr_ptr && desc->lck_pc < instr_ptr + 8) {
		desc->lck_cnt += 1;

		if (desc->lck_cnt > MAX_IRQ_CNT) {
			printk(KERN_ERR
			       "IRQ LOCK: IRQ%d is locking the system, disabled\n",
			       irq);
			return 1;
		}
	} else {
		desc->lck_cnt = 0;
		desc->lck_pc = instruction_pointer(regs);
		desc->lck_jif = jiffies;
	}
	return 0;
}

static void
report_bad_irq(unsigned int irq, struct pt_regs *regs,
	       struct irqdesc *desc, int ret)
{
	static int count = 100;
	struct irqaction *action;

	if (!count)
		return;

	count--;

	if (ret != IRQ_HANDLED && ret != IRQ_NONE) {
		printk(KERN_WARNING "irq%u: bogus retval mask %x\n", irq, ret);
	} else {
		printk(KERN_WARNING "irq%u: nobody cared\n", irq);
	}
	show_regs(regs);
	dump_stack();
	printk(KERN_ERR "handlers:");
	action = desc->action;
	do {
		printk("\n" KERN_ERR "[<%p>]", action->handler);
		print_symbol(" (%s)", (unsigned long)action->handler);
		action = action->next;
	}
	while (action);
	printk("\n");
}

static int
__do_irq(unsigned int irq, struct irqaction *action, struct pt_regs *regs)
{
	unsigned int status;
	int retval = 0;

	spin_unlock(&irq_controller_lock);

	if (!(action->flags & SA_INTERRUPT)) {
		local_irq_enable();
	}

	status = 0;
	do {
		status |= action->flags;
		retval |= action->handler(irq, action->dev_id, regs);
		action = action->next;
	}
	while (action);

	if (status & SA_SAMPLE_RANDOM)
		add_interrupt_randomness(irq);

	spin_lock_irq(&irq_controller_lock);

	return retval;
}

/*
 * This is for software-decoded IRQs. The caller is expected to
 * handle the ack, clear, mask and unmask issues.
 */
void do_simple_IRQ(unsigned int irq, struct irqdesc *desc, struct pt_regs *regs)
{
	struct irqaction *action;
	const int cpu = smp_processor_id();

	desc->triggered = 1;

	kstat_cpu(cpu).irqs[irq]++;

	action = desc->action;
	if (action) {
		int ret = __do_irq(irq, action, regs);
		if (ret != IRQ_HANDLED)
			report_bad_irq(irq, regs, desc, ret);
	}
}

/*
 * Most edge-triggered IRQ implementations seem to take a broken
 * approach to this.  Hence the complexity.
 */
void do_edge_IRQ(unsigned int irq, struct irqdesc *desc, struct pt_regs *regs)
{
	const int cpu = smp_processor_id();

	desc->triggered = 1;

	/*
	 * If we're currently running this IRQ, or its disabled,
	 * we shouldn't process the IRQ. Instead, turn on the
	 * hardware masks.
	 */
	if (unlikely(desc->running || desc->disable_depth))
		goto running;

	/*
	 * Acknowledge and clear the IRQ, but don't mask it.
	 */
	desc->chip->ack(irq);

	/*
	 * Mark the IRQ currently in progress.
	 */
	desc->running = 1;

	kstat_cpu(cpu).irqs[irq]++;

	do {
		struct irqaction *action;
		int ret;

		action = desc->action;
		if (!action)
			break;

		if (desc->pending && !desc->disable_depth) {
			desc->pending = 0;
			desc->chip->unmask(irq);
		}

		ret = __do_irq(irq, action, regs);
		if (ret != IRQ_HANDLED)
			report_bad_irq(irq, regs, desc, ret);
	}
	while (desc->pending && !desc->disable_depth);

	desc->running = 0;

	/*
	 * If we were disabled or freed, shut down the handler.
	 */
	if (likely(desc->action && !check_irq_lock(desc, irq, regs)))
		return;

      running:
	/*
	 * We got another IRQ while this one was masked or
	 * currently running. Delay it.
	 */
	desc->pending = 1;
	desc->chip->mask(irq);
	desc->chip->ack(irq);
}

/*
 * Level-based IRQ handler. Nice and simple.
 */
void do_level_IRQ(unsigned int irq, struct irqdesc *desc, struct pt_regs *regs)
{
	struct irqaction *action;
	const int cpu = smp_processor_id();

	desc->triggered = 1;

	/*
	 * Acknowledge, clear _AND_ disable the interrupt.
	 */
	desc->chip->ack(irq);

	if (likely(!desc->disable_depth)) {
		kstat_cpu(cpu).irqs[irq]++;

		/*
		 * Return with this interrupt masked if no action
		 */
		action = desc->action;
		if (action) {
			int ret = __do_irq(irq, desc->action, regs);

			if (ret != IRQ_HANDLED)
				report_bad_irq(irq, regs, desc, ret);
			if (likely(!desc->disable_depth &&
				   !check_irq_lock(desc, irq, regs)))
				desc->chip->unmask(irq);
		}
	}
}

static void do_pending_irqs(struct pt_regs *regs)
{
	struct list_head head, *l, *n;

	do {
		struct irqdesc *desc;

		/*
		 * First, take the pending interrupts off the list.
		 * The act of calling the handlers may add some IRQs
		 * back onto the list.
		 */
		head = irq_pending;
		INIT_LIST_HEAD(&irq_pending);
		head.next->prev = &head;
		head.prev->next = &head;

		/*
		 * Now run each entry.  We must delete it from our
		 * list before calling the handler.
		 */
		list_for_each_safe(l, n, &head) {
			desc = list_entry(l, struct irqdesc, pend);
			list_del_init(&desc->pend);
			desc->handle(desc - irq_desc, desc, regs);
		}

		/*
		 * The list must be empty.
		 */
		BUG_ON(!list_empty(&head));
	}
	while (!list_empty(&irq_pending));
}

extern asmlinkage void lower_to_irq14(void);

/*
 * do_IRQ handles all hardware IRQ's.  Decoded IRQs should not
 * come via this function.  Instead, they should provide their
 * own 'handler'
 */
asmlinkage void asm_do_IRQ(unsigned int irq, struct pt_regs *regs)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned short pending, other_ints;

	/*
	 * Some hardware gives randomly wrong interrupts.  Rather
	 * than crashing, do something sensible.
	 */
	if (irq >= NR_IRQS)
		desc = &bad_irq_desc;

	irq_enter();
	spin_lock(&irq_controller_lock);
	desc->handle(irq, desc, regs);

	/*
	 * Now re-run any pending interrupts.
	 */
	if (!list_empty(&irq_pending))
		do_pending_irqs(regs);

	spin_unlock(&irq_controller_lock);

	/* If we're the only interrupt running (ignoring IRQ15 which is for
	   syscalls), lower our priority to IRQ14 so that softirqs run at
	   that level.  If there's another, lower-level interrupt, irq_exit
	   will defer softirqs to that.  */
	__builtin_bfin_csync();
	pending = *pIPEND & ~0x8000;
	other_ints = pending & (pending - 1);
	if (other_ints == 0)
		lower_to_irq14();
	irq_exit();
}

void __set_irq_handler(unsigned int irq, irq_handler_t handle, int is_chained)
{
	struct irqdesc *desc;
	unsigned long flags;

	if (irq >= NR_IRQS) {
		printk(KERN_ERR "Trying to install handler for IRQ%d\n", irq);
		return;
	}

	if (handle == NULL)
		handle = do_bad_IRQ;

	desc = irq_desc + irq;

	if (is_chained && desc->chip == &bad_chip)
		printk(KERN_WARNING
		       "Trying to install chained handler for IRQ%d\n", irq);

	spin_lock_irqsave(&irq_controller_lock, flags);
	if (handle == do_bad_IRQ) {
		desc->chip->mask(irq);
		desc->chip->ack(irq);
		desc->disable_depth = 1;
	}
	desc->handle = handle;
	if (handle != do_bad_IRQ && is_chained) {
		desc->valid = 0;
		desc->probe_ok = 0;
		desc->disable_depth = 0;
		desc->chip->unmask(irq);
	}
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

void set_irq_chip(unsigned int irq, struct irqchip *chip)
{
	struct irqdesc *desc;
	unsigned long flags;

	if (irq >= NR_IRQS) {
		printk(KERN_ERR "Trying to install chip for IRQ%d\n", irq);
		return;
	}

	if (chip == NULL)
		chip = &bad_chip;

	desc = irq_desc + irq;
	spin_lock_irqsave(&irq_controller_lock, flags);
	desc->chip = chip;
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

int set_irq_type(unsigned int irq, unsigned int type)
{
	struct irqdesc *desc;
	unsigned long flags;
	int ret = -ENXIO;

	if (irq >= NR_IRQS) {
		printk(KERN_ERR "Trying to set irq type for IRQ%d\n", irq);
		return -ENODEV;
	}

	desc = irq_desc + irq;
	if (desc->chip->type) {
		spin_lock_irqsave(&irq_controller_lock, flags);
		ret = desc->chip->type(irq, type);
		spin_unlock_irqrestore(&irq_controller_lock, flags);
	}

	return ret;
}

EXPORT_SYMBOL(set_irq_type);

void set_irq_flags(unsigned int irq, unsigned int iflags)
{
	struct irqdesc *desc;
	unsigned long flags;

	if (irq >= NR_IRQS) {
		printk(KERN_ERR "Trying to set irq flags for IRQ%d\n", irq);
		return;
	}

	desc = irq_desc + irq;
	spin_lock_irqsave(&irq_controller_lock, flags);
	desc->valid = (iflags & IRQF_VALID) != 0;
	desc->probe_ok = (iflags & IRQF_PROBE) != 0;
	desc->noautoenable = (iflags & IRQF_NOAUTOEN) != 0;
	spin_unlock_irqrestore(&irq_controller_lock, flags);
}

int setup_irq(unsigned int irq, struct irqaction *new)
{
	int shared = 0;
	struct irqaction *old, **p;
	unsigned long flags;
	struct irqdesc *desc;

	/*
	 * Some drivers like serial.c use request_irq() heavily,
	 * so we have to be careful not to interfere with a
	 * running system.
	 */
	if (new->flags & SA_SAMPLE_RANDOM) {
		/*
		 * This function might sleep, we want to call it first,
		 * outside of the atomic block.
		 * Yes, this might clear the entropy pool if the wrong
		 * driver is attempted to be loaded, without actually
		 * installing a new handler, but is this really a problem,
		 * only the sysadmin is able to do this.
		 */
		rand_initialize_irq(irq);
	}

	/*
	 * The following block of code has to be executed atomically
	 */
	desc = irq_desc + irq;
	spin_lock_irqsave(&irq_controller_lock, flags);
	p = &desc->action;
	if ((old = *p) != NULL) {
		/* Can't share interrupts unless both agree to */
		if (!(old->flags & new->flags & SA_SHIRQ)) {
			spin_unlock_irqrestore(&irq_controller_lock, flags);
			return -EBUSY;
		}

		/* add new interrupt at end of irq queue */
		do {
			p = &old->next;
			old = *p;
		}
		while (old);
		shared = 1;
	}

	*p = new;

	if (!shared) {
		desc->probing = 0;
		desc->running = 0;
		desc->pending = 0;
		desc->disable_depth = 1;
		if (!desc->noautoenable) {
			desc->disable_depth = 0;
			desc->chip->unmask(irq);
		}
	}

	spin_unlock_irqrestore(&irq_controller_lock, flags);
	return 0;
}

/**
 *	request_irq - allocate an interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs
 *	@irqflags: Interrupt type flags
 *	@devname: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. From the point this
 *	call is made your handler function may be invoked. Since
 *	your handler function must clear any interrupt the board
 *	raises, you must take care both to initialise your hardware
 *	and to set up the interrupt handler in the right order.
 *
 *	Dev_id must be globally unique. Normally the address of the
 *	device data structure is used as the cookie. Since the handler
 *	receives this value it makes sense to use it.
 *
 *	If your interrupt is shared you must pass a non NULL dev_id
 *	as this is required when freeing the interrupt.
 *
 *	Flags:
 *
 *	SA_SHIRQ		Interrupt is shared
 *
 *	SA_INTERRUPT		Disable local interrupts while processing
 *
 *	SA_SAMPLE_RANDOM	The interrupt can be used for entropy
 *
 */
int
request_irq(unsigned int irq,
	    irqreturn_t(*handler) (int, void *, struct pt_regs *),
	    unsigned long irq_flags, const char *devname, void *dev_id)
{
	unsigned long retval;
	struct irqaction *action;

	IRQ_DPRINTK
	    ("request_irq: irq=%d handler=%p irq_flags=%#lx devname=%s devid=%p\n",
	     irq, handler, irq_flags, devname, dev_id);
	if (irq >= NR_IRQS || !irq_desc[irq].valid || !handler
	    || (irq_flags & SA_SHIRQ && !dev_id))
		return -EINVAL;

	action = kmalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;

	action->handler = handler;
	action->flags = irq_flags;
	cpus_clear(action->mask);
	action->name = devname;
	action->next = NULL;
	action->dev_id = dev_id;

	retval = setup_irq(irq, action);

	if (retval)
		kfree(action);
	return retval;
}

EXPORT_SYMBOL(request_irq);

/*
 *	free_irq - free an interrupt
 *	@irq: Interrupt line to free
 *	@dev_id: Device identity to free
 *
 *	Remove an interrupt handler. The handler is removed and if the
 *	interrupt line is no longer in use by any driver it is disabled.
 *	On a shared IRQ the caller must ensure the interrupt is disabled
 *	on the card it drives before calling this function.
 *
 *	This function must not be called from interrupt context.
 */
void free_irq(unsigned int irq, void *dev_id)
{
	struct irqdesc *desc = irq_desc + irq;
	struct irqaction *action, **p;
	unsigned long flags;

	if (irq >= NR_IRQS || !irq_desc[irq].valid) {
		printk(KERN_ERR "Trying to free IRQ%d\n", irq);
		dump_stack();
		return;
	}

	spin_lock_irqsave(&irq_controller_lock, flags);
	for (p = &desc->action; (action = *p) != NULL;
	     p = &action->next) {
		if (action->dev_id != dev_id)
			continue;

		/* Found it - now free it */
		*p = action->next;
		if (!desc->action)
			desc->chip->mask(irq);
		break;
	}
	spin_unlock_irqrestore(&irq_controller_lock, flags);

	if (!action) {
		printk(KERN_ERR "Trying to already freed IRQ%d\n", irq);
		dump_stack();
	} else {
		synchronize_irq(irq);
		kfree(action);
	}
}

EXPORT_SYMBOL(free_irq);

static DECLARE_MUTEX(probe_sem);

/* Start the interrupt probing. Unlike other architectures,
 * we don't return a mask of interrupts from probe_irq_on,
 * but return the number of interrupts enabled for the probe.
 * The interrupts which have been enabled for probing is
 * instead recorded in the irq_desc structure.
 */
unsigned long probe_irq_on(void)
{
	unsigned int i, irqs = 0;
	unsigned long delay;

	down(&probe_sem);

	/*
	 * first snaffle up any unassigned but probe-able interrupts
	 */
	spin_lock_irq(&irq_controller_lock);
	for (i = 0; i < NR_IRQS; i++) {
		if (!irq_desc[i].probe_ok || irq_desc[i].action)
			continue;

		irq_desc[i].probing = 1;
		irq_desc[i].triggered = 0;
		if (irq_desc[i].chip->type)
			irq_desc[i].chip->type(i, IRQT_PROBE);
		irq_desc[i].chip->unmask(i);
		irqs += 1;
	}
	spin_unlock_irq(&irq_controller_lock);

	/*
	 * wait for spurious interrupts to mask themselves out again
	 */
	for (delay = jiffies + HZ / 10; time_before(jiffies, delay);)
		/* min 100ms delay */ ;

	/*
	 * now filter out any obviously spurious interrupts
	 */
	spin_lock_irq(&irq_controller_lock);
	for (i = 0; i < NR_IRQS; i++) {
		if (irq_desc[i].probing && irq_desc[i].triggered) {
			irq_desc[i].probing = 0;
			irqs--;
		}
	}
	spin_unlock_irq(&irq_controller_lock);

	return irqs;
}

EXPORT_SYMBOL(probe_irq_on);

unsigned int probe_irq_mask(unsigned long irqs)
{
	unsigned int mask = 0, i;

	spin_lock_irq(&irq_controller_lock);
	for (i = 0; i < 16 && i < NR_IRQS; i++)
		if (irq_desc[i].probing && irq_desc[i].triggered)
			mask |= 1 << i;
	spin_unlock_irq(&irq_controller_lock);

	up(&probe_sem);

	return mask;
}

EXPORT_SYMBOL(probe_irq_mask);

/*
 * Possible return values:
 *  >= 0 - interrupt number
 *    -1 - no interrupt/many interrupts
 */
int probe_irq_off(unsigned long irqs)
{
	unsigned int i;
	int irq_found = NO_IRQ;

	/*
	 * look at the interrupts, and find exactly one
	 * that we were probing has been triggered
	 */
	spin_lock_irq(&irq_controller_lock);
	for (i = 0; i < NR_IRQS; i++) {
		if (irq_desc[i].probing && irq_desc[i].triggered) {
			if (irq_found != NO_IRQ) {
				irq_found = NO_IRQ;
				goto out;
			}
			irq_found = i;
		}
	}

	if (irq_found == -1)
		irq_found = NO_IRQ;
      out:
	spin_unlock_irq(&irq_controller_lock);

	up(&probe_sem);

	return irq_found;
}

EXPORT_SYMBOL(probe_irq_off);

void __init init_irq_proc(void)
{
}

void __init init_IRQ(void)
{
	struct irqdesc *desc;
	extern void init_dma(void);
	int irq;

	for (irq = 0, desc = irq_desc; irq < NR_IRQS; irq++, desc++) {
		*desc = bad_irq_desc;
		INIT_LIST_HEAD(&desc->pend);
	}

	init_arch_irq();
}
