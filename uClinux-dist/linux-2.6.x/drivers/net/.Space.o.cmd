cmd_drivers/net/Space.o := bfin-uclinux-gcc -Wp,-MD,drivers/net/.Space.o.d  -nostdinc -isystem /opt/uClinux/bfin-uclinux/lib/gcc/bfin-uclinux/3.4.4/include -D__KERNEL__ -Iinclude  -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -ffreestanding -O2     -fomit-frame-pointer -fno-builtin -D__linux__ -Wdeclaration-after-statement     -DKBUILD_BASENAME=Space -DKBUILD_MODNAME=Space -c -o drivers/net/Space.o drivers/net/Space.c

deps_drivers/net/Space.o := \
  drivers/net/Space.c \
    $(wildcard include/config/ultra32.h) \
    $(wildcard include/config/ac3200.h) \
    $(wildcard include/config/es3210.h) \
    $(wildcard include/config/lne390.h) \
    $(wildcard include/config/ne2/mca.h) \
    $(wildcard include/config/elmc.h) \
    $(wildcard include/config/elmc/ii.h) \
    $(wildcard include/config/skmc.h) \
    $(wildcard include/config/hp100.h) \
    $(wildcard include/config/3c515.h) \
    $(wildcard include/config/ultra.h) \
    $(wildcard include/config/wd80x3.h) \
    $(wildcard include/config/el2.h) \
    $(wildcard include/config/hplan.h) \
    $(wildcard include/config/hplan/plus.h) \
    $(wildcard include/config/e2100.h) \
    $(wildcard include/config/ne2000.h) \
    $(wildcard include/config/ne/h8300.h) \
    $(wildcard include/config/lance.h) \
    $(wildcard include/config/smc9194.h) \
    $(wildcard include/config/seeq8005.h) \
    $(wildcard include/config/cs89x0.h) \
    $(wildcard include/config/at1700.h) \
    $(wildcard include/config/fmv18x.h) \
    $(wildcard include/config/eth16i.h) \
    $(wildcard include/config/eexpress.h) \
    $(wildcard include/config/eexpress/pro.h) \
    $(wildcard include/config/ewrk3.h) \
    $(wildcard include/config/apricot.h) \
    $(wildcard include/config/mvme16x/net.h) \
    $(wildcard include/config/bvme6000/net.h) \
    $(wildcard include/config/el1.h) \
    $(wildcard include/config/wavelan.h) \
    $(wildcard include/config/arlan.h) \
    $(wildcard include/config/el16.h) \
    $(wildcard include/config/elplus.h) \
    $(wildcard include/config/sk/g16.h) \
    $(wildcard include/config/ni5010.h) \
    $(wildcard include/config/ni52.h) \
    $(wildcard include/config/ni65.h) \
    $(wildcard include/config/de620.h) \
    $(wildcard include/config/atarilance.h) \
    $(wildcard include/config/sun3lance.h) \
    $(wildcard include/config/sun3/82586.h) \
    $(wildcard include/config/apne.h) \
    $(wildcard include/config/atari/bionet.h) \
    $(wildcard include/config/atari/pamsnet.h) \
    $(wildcard include/config/mvme147/net.h) \
    $(wildcard include/config/macmace.h) \
    $(wildcard include/config/macsonic.h) \
    $(wildcard include/config/mac8390.h) \
    $(wildcard include/config/mac89x0.h) \
    $(wildcard include/config/tr.h) \
    $(wildcard include/config/skisa.h) \
    $(wildcard include/config/proteon.h) \
    $(wildcard include/config/smctr.h) \
    $(wildcard include/config/ibmtr.h) \
    $(wildcard include/config/sbni.h) \
    $(wildcard include/config/cops.h) \
    $(wildcard include/config/ltpc.h) \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/linux/netdevice.h \
    $(wildcard include/config/ax25.h) \
    $(wildcard include/config/ax25/module.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/ipv6/module.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/net/divert.h) \
    $(wildcard include/config/netpoll/trap.h) \
    $(wildcard include/config/sysctl.h) \
  include/linux/if.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
  include/linux/compiler-gcc3.h \
  include/linux/compiler-gcc.h \
  include/asm/posix_types.h \
  include/asm/types.h \
  include/linux/socket.h \
    $(wildcard include/config/compat.h) \
  include/linux/linkage.h \
  include/asm/linkage.h \
  include/asm/socket.h \
  include/asm/sockios.h \
  include/linux/sockios.h \
  include/linux/uio.h \
  include/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/skbuff.h \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/vlan/8021q/module.h) \
    $(wildcard include/config/netfilter/debug.h) \
    $(wildcard include/config/hippi.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/have/arch/dev/alloc/skb.h) \
    $(wildcard include/config/highmem.h) \
  include/linux/kernel.h \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/printk.h) \
  /opt/uClinux/bfin-uclinux/lib/gcc/bfin-uclinux/3.4.4/include/stdarg.h \
  include/linux/bitops.h \
  include/asm/bitops.h \
  include/asm/system.h \
    $(wildcard include/config/debug/hwerr.h) \
    $(wildcard include/config/smp.h) \
  include/asm/blackfin.h \
  include/asm/macros.h \
  include/asm/mach/blackfin.h \
  include/asm/mach/bf533.h \
    $(wildcard include/config/bf533.h) \
    $(wildcard include/config/bf532.h) \
    $(wildcard include/config/bank/1.h) \
    $(wildcard include/config/bank/0.h) \
    $(wildcard include/config/bank/3.h) \
    $(wildcard include/config/bank/2.h) \
    $(wildcard include/config/c/amben/all.h) \
    $(wildcard include/config/c/amben.h) \
    $(wildcard include/config/c/amben/b0.h) \
    $(wildcard include/config/c/amben/b0/b1.h) \
    $(wildcard include/config/c/amben/b0/b1/b2.h) \
    $(wildcard include/config/c/amcken.h) \
    $(wildcard include/config/c/cdprio.h) \
    $(wildcard include/config/bfin/kernel/clock.h) \
    $(wildcard include/config/vco/mult.h) \
    $(wildcard include/config/clkin/half.h) \
    $(wildcard include/config/vco/hz.h) \
    $(wildcard include/config/clkin/hz.h) \
    $(wildcard include/config/pll/bypass.h) \
    $(wildcard include/config/cclk/hz.h) \
    $(wildcard include/config/cclk/div.h) \
    $(wildcard include/config/sclk/hz.h) \
    $(wildcard include/config/sclk/div.h) \
    $(wildcard include/config/cclk/act/div.h) \
    $(wildcard include/config/cclk/div/not/defined/properly.h) \
    $(wildcard include/config/bf531.h) \
    $(wildcard include/config/mem/size.h) \
    $(wildcard include/config/blkfin/wt.h) \
    $(wildcard include/config/blkfin/wb.h) \
  include/asm/mach/mem_map.h \
    $(wildcard include/config/blkfin/cache.h) \
    $(wildcard include/config/blkfin/dcache.h) \
  include/asm/mach/defBF532.h \
  include/asm/mach-common/def_LPBlackfin.h \
  include/asm/mach/cdefBF532.h \
  include/asm/mach-common/cdef_LPBlackfin.h \
  include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/byteorder/swab.h \
  include/linux/byteorder/generic.h \
  include/asm/bug.h \
  include/linux/time.h \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/lockmeter.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
  include/linux/thread_info.h \
  include/asm/thread_info.h \
  include/asm/page.h \
  include/asm/page_offset.h \
    $(wildcard include/config/bfin.h) \
  include/asm/io.h \
  include/asm/entry.h \
  include/asm/setup.h \
  include/linux/stringify.h \
  include/asm/processor.h \
  include/asm/segment.h \
  include/linux/cache.h \
  include/asm/cache.h \
  include/asm/atomic.h \
  include/linux/mm.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/debug/pagealloc.h) \
  include/linux/sched.h \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/magic/sysrq.h) \
    $(wildcard include/config/pm.h) \
  include/asm/param.h \
  include/linux/capability.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/timex.h \
    $(wildcard include/config/time/interpolation.h) \
  include/asm/timex.h \
  include/linux/jiffies.h \
  include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/rbtree.h \
  include/linux/cpumask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
  include/asm/string.h \
  include/linux/errno.h \
  include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/nodemask.h \
  include/linux/numa.h \
  include/asm/semaphore.h \
  include/linux/wait.h \
  include/linux/list.h \
  include/linux/prefetch.h \
  include/asm/current.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/asm/ptrace.h \
  include/asm/mmu.h \
  include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/smp.h \
  include/linux/sem.h \
    $(wildcard include/config/sysvipc.h) \
  include/linux/ipc.h \
  include/asm/ipcbuf.h \
  include/asm/sembuf.h \
  include/linux/signal.h \
  include/asm/signal.h \
  include/asm/sigcontext.h \
  include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/securebits.h \
  include/linux/fs_struct.h \
  include/linux/completion.h \
  include/linux/pid.h \
  include/linux/percpu.h \
  include/linux/slab.h \
    $(wildcard include/config/.h) \
  include/linux/gfp.h \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
  include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/kmalloc_sizes.h \
    $(wildcard include/config/large/allocs.h) \
  include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  include/linux/param.h \
  include/linux/resource.h \
  include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/timer.h \
  include/linux/aio.h \
  include/linux/workqueue.h \
  include/linux/aio_abi.h \
  include/linux/prio_tree.h \
  include/linux/fs.h \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/auditsyscall.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  include/asm/ioctl.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rcupdate.h \
  include/linux/stat.h \
  include/asm/stat.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
    $(wildcard include/config/sysfs.h) \
  include/linux/kref.h \
  include/linux/kobject_uevent.h \
    $(wildcard include/config/kobject/uevent.h) \
  include/linux/radix-tree.h \
  include/linux/quota.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/fcntl.h \
  include/asm/fcntl.h \
  include/linux/err.h \
  include/asm/pgtable.h \
  include/asm-generic/4level-fixup.h \
  include/linux/page-flags.h \
    $(wildcard include/config/swap.h) \
  include/linux/highmem.h \
  include/asm/cacheflush.h \
  include/asm/cplb.h \
  include/linux/poll.h \
  include/asm/poll.h \
  include/asm/uaccess.h \
  include/linux/net.h \
  include/net/checksum.h \
  include/asm/checksum.h \
  include/linux/if_packet.h \
  include/linux/device.h \
  include/linux/ioport.h \
  include/linux/module.h \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/kallsyms.h) \
  include/linux/kmod.h \
    $(wildcard include/config/kmod.h) \
  include/linux/elf.h \
  include/asm/elf.h \
  include/asm/user.h \
  include/linux/moduleparam.h \
  include/asm/local.h \
  include/asm-generic/local.h \
  include/linux/hardirq.h \
    $(wildcard include/config/preempt/bkl.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
  include/linux/smp_lock.h \
    $(wildcard include/config/lock/kernel.h) \
  include/asm/hardirq.h \
  include/linux/irq_cpustat.h \
  include/asm/module.h \
  include/linux/pm.h \
  include/linux/interrupt.h \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/generic/irq/probe.h) \
  include/linux/notifier.h \
  include/linux/etherdevice.h \
  include/linux/random.h \
  include/linux/trdevice.h \
  include/linux/if_tr.h \
  include/linux/netlink.h \
  include/linux/divert.h \

drivers/net/Space.o: $(deps_drivers/net/Space.o)

$(deps_drivers/net/Space.o):
