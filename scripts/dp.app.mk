
INCLUDE_DIR += -I $(TOP_DIR)/include -I $(TOP_DIR)/include/lib
LIB_DIR += -L $(TOP_DIR)/build/lib

CFLAGS += $(INCLUDE_DIR)
CFLAGS += $(APP_CFLAGS)

LDFLAGS +=  $(LIB_DIR)
LDFLAGS +=  $(APP_LDFLAGS)

#所有.c文件
SRC = $(wildcard *.c) 
#将所有.c 变成.o
OBJ = $(patsubst %.c,%.o,$(SRC))

.PHONY: all install clean

all: $(TARGET) install

.c.o::
	@echo "  CC" $@; $(CC) -c $< $(CFLAGS)

$(TARGET): $(OBJ)
	@echo "  LD" $@; $(CC) -o $@ $(OBJ) $(LDFLAGS)

install:
	@echo "  INSTALL-APP" $(TARGET); cp -f $(TARGET) $(TOP_DIR)/build/apps

clean:
	@rm -f *.o $(TARGET)
