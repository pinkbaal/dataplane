#!/bin/bash

DP_ROOT_DIR="`dirname $(pwd)`"


DPDK_VERSION="17.05.2"
DPDK_ROOT_DIR="${DP_ROOT_DIR}/extlib/dpdk/dpdk-stable-${DPDK_VERSION}"
#export RTE_KERNELDIR
export RTE_SDK=${DPDK_ROOT_DIR}
export RTE_TARGET=x86_64-native-linuxapp-gcc

compile_dpdk()
{
	echo "================="
	echo "Compile DPDK"
	echo "================="
	if [ ! -d "${DPDK_ROOT_DIR}" ]; then
		cd ${DP_ROOT_DIR}/extlib/dpdk/
		if [ ! -f dpdk-${DPDK_VERSION}.tar ]; then
			wget -P ${DP_ROOT_DIR}/extlib/dpdk/ http://fast.dpdk.org/rel/dpdk-${DPDK_VERSION}.tar.xz
			xz -d dpdk-${DPDK_VERSION}.tar.xz
		fi
		tar xf dpdk-${DPDK_VERSION}.tar
		patch -Np1 -d ${DPDK_ROOT_DIR} < ${DP_ROOT_DIR}/patch/dpdk.diff 
		cd -
	fi
	cd ${DPDK_ROOT_DIR}
	#修改dpdk编译选项 调试dpdk
	find . -name Makefile |xargs sed -i "s/-O3/-O0 -g/g"
	if [ -d x86_64-native-linuxapp-gcc ]; then
		rm -rf x86_64-native-linuxapp-gcc/*
		rm -rf x86_64-native-linuxapp-gcc
	fi
	ln -s ${DP_ROOT_DIR}/build/dpdk x86_64-native-linuxapp-gcc
	make install T=x86_64-native-linuxapp-gcc
}


compile_urcu()
{
	echo "================="
	echo "Compile URCU"
	echo "================="
}

compile_uslib()
{
	echo "================="
	echo "Compile USLIB"
	echo "================="
	cd ${DP_ROOT_DIR}/lib
	make clean; make
}



compile_apps()
{
	echo "================="
	echo "Compile APPS"
	echo "================="
}

compile_dataplane()
{
	echo "================="
	echo "Compile DATA PLANE"
	echo "================="
	cd ${DP_ROOT_DIR}/net
	make clean; make
	cp -rf ${DP_ROOT_DIR}/net/main/x86_64-native-linuxapp-gcc/app/dataplane \
		${DP_ROOT_DIR}/build
	
}



compile_all()
{
	compile_dpdk
	compile_urcu
	compile_uslib
	compile_dataplane
	compile_apps
}


clean_all()
{
	echo "================="
	echo "clean ALL"
	echo "================="	
}




case $1 in
	dpdk)
		compile_dpdk
		;;
	urcu)
		compile_urcu
		;;
	lib)
		compile_uslib
		;;
	apps)
		compile_apps
		;;
	dp)
		compile_dataplane
		;;
	all)
		compile_all
		;;
	clean)
		clean_all
		;;
	*)
		echo "Usage: $0 all|dpdk|urcu|lib|apps|dp"
		;;
esac
