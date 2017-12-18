
SUBDIRS = libdp_epoll libdp_memory libdp_quagga

.PHONY: all install clean


all:
	@for subdir in $(SUBDIRS); do \
		test "$$subdir" = . || \
		(echo "== $$subdir"; cd $$subdir && make --no-print-directory) || \
		exit 1; \
	done

install:
	@for subdir in $(SUBDIRS); do \
		test "$$subdir" = . || \
		(echo "== $$subdir"; cd $$subdir && make --no-print-directory install) || \
		exit 1; \
	done


clean:
	@for subdir in $(SUBDIRS); do \
		test "$$subdir" = . || \
		(echo "== $$subdir"; cd $$subdir && make --no-print-directory clean); \
	done
