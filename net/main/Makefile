
#data plane工程根目录
export DP_ROOT_DIR ?= $(abspath $(shell dirname $$(pwd))/..)

#dpdk源码根目录
RTE_SDK := $(DP_ROOT_DIR)/extlib/dpdk/dpdk-stable-17.05.2
RTE_TARGET ?= x86_64-native-linuxapp-gcc
RTE_OUTPUT := ./x86_64-native-linuxapp-gcc


include $(RTE_SDK)/mk/rte.vars.mk

APP := dataplane

SRCS-y := dp_main.c 

#头文件目录
CFLAGS += -I$(DP_ROOT_DIR)/include -I$(DP_ROOT_DIR)/include/net -I$(DP_ROOT_DIR)/include/lib

#连接静态库
CFLAGS += -L$(DP_ROOT_DIR)/build/lib
LDLIBS += -ldp_epoll -ldp_memory 

#连接模块 函数subst 将路径$(RTE_OUTPUT)中的main替换成ipv4
#LDLIBS += $(subst main,ipv4,$(RTE_OUTPUT))/ipv4.o
LDLIBS += $(DP_ROOT_DIR)/net/ipv4/$(RTE_TARGET)/ipv4.o
LDLIBS += $(DP_ROOT_DIR)/net/cmdline/$(RTE_TARGET)/cmdline.o

EXTRA_CFLAGS += -O0 -g -Wfatal-errors

include $(RTE_SDK)/mk/rte.extapp.mk


