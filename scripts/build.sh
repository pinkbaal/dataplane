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

compile_quagga()
{
	echo "================="
	echo "Compile QUAGGA"
	echo "================="
	QUAGGA_VERSION="1.2.2"
	QUAGGA_ROOT_DIR="${DP_ROOT_DIR}/apps/quagga/quagga-${QUAGGA_VERSION}"
	cd ${QUAGGA_ROOT_DIR}
	QUAGGA_CONFIG=" --disable-bgpd \
			--disable-ripd \
			--disable-ripngd \
			--disable-ospf6d \
			--disable-nhrpd \
			--disable-watchquagga \
			--disable-isisd \
			--disable-pimd \
			--disable-bgp-announce \
			--disable-ospfapi \
			--disable-ospfclient \
			--disable-rtadv \
			--disable-capabilities \
			--disable-rusage \
			"
	${QUAGGA_ROOT_DIR}/configure ${QUAGGA_CONFIG}
	find . -name Makefile |xargs sed -i "s/-O2/-O0 -g/g"
	find . -name Makefile |xargs sed -i "s/-Os/-O0 -g/g"
	find . -name Makefile |xargs sed -i "s/-D_FORTIFY_SOURCE=2/-D_FORTIFY_SOURCE=0 -g/g"
	make
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
	cd ${DP_ROOT_DIR}/apps
	make clean; make
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
}


clean_all()
{
	echo "================="
	echo "clean ALL"
	echo "================="	
	cd ${DP_ROOT_DIR}/net
        make clean;
	cd ${DP_ROOT_DIR}/lib
        make clean; 
}




case $1 in
	dpdk)
		compile_dpdk
		;;
	urcu)
		compile_urcu
		;;
	quagga)
		compile_quagga
		;;
	lib)
		compile_uslib
		;;
	app)
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
		echo "Usage: $0 all|dpdk|urcu|lib|app|dp|quagga"
		;;
esac
