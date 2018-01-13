
#data plane工程根目录
export DP_ROOT_DIR ?= $(abspath $(shell dirname $$(pwd))/..)

RTE_SDK := $(DP_ROOT_DIR)/extlib/dpdk/dpdk-stable-17.05.2
RTE_TARGET ?= x86_64-native-linuxapp-gcc
RTE_OUTPUT := ./x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

SRCS-y := quagga_core.c
SRCS-y += quagga_rtnl.c

OBJ = quagga.o

CFLAGS += -I$(DP_ROOT_DIR)/include -I$(DP_ROOT_DIR)/include/net -I$(DP_ROOT_DIR)/include/lib 

EXTRA_CFLAGS += -O0 -g -Wfatal-errors

include $(RTE_SDK)/mk/rte.extobj.mk
