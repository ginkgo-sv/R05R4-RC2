cmd_net/core/built-in.o :=  bfin-uclinux-ld   -r -o net/core/built-in.o net/core/sock.o net/core/skbuff.o net/core/iovec.o net/core/datagram.o net/core/stream.o net/core/scm.o net/core/gen_stats.o net/core/gen_estimator.o net/core/flow.o net/core/dev.o net/core/ethtool.o net/core/dev_mcast.o net/core/dst.o net/core/neighbour.o net/core/rtnetlink.o net/core/utils.o net/core/link_watch.o net/core/filter.o net/core/net-sysfs.o
