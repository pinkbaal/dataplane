
#data plane工程根目录
export DP_ROOT_DIR ?= $(abspath $(shell dirname $$(pwd)))

#dpdk源码根目录
RTE_SDK := $(DP_ROOT_DIR)/extlib/dpdk/dpdk-stable-17.05.2

#
RTE_TARGET := x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

DIRS-y += ipv4 
DIRS-y += cmd
DIRS-y += memory
DIRS-y += quagga
DIRS-y += main


DEPDIRS-dataplane := ipv4 cmd quagga memory 

EXTRA_CFLAGS += -O0 -g -Wfatal-errors

include $(RTE_SDK)/mk/rte.extsubdir.mk



