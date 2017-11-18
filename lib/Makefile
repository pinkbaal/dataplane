DP_ROOT_DIR = ./..
DP_LIB_DIR = $(DP_ROOT_DIR)/build/lib

LIBS := libdp_memory libdp_list


all:
	@for dir in $(LIBS); do \
		set -e; \
		echo == Build $$dir; \
                make -s -C $$dir; \
		echo "  INSTALL-LIB" $(shell basename $$(pwd)).a; \
		cp -rf ./$$dir/x86_64-native-linuxapp-gcc/$$dir.a $(DP_LIB_DIR); \
        done
clean:
	@for dir in $(LIBS); do \
                make -s -C $$dir clean; \
		rm -rf $$dir/x86_64-native-linuxapp-gcc/; \
	done
	@ rm -rf $(DP_LIB_DIR)/*
