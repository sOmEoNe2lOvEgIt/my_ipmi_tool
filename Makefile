NAME = ipmi_tool

SRC_FILES = ipmi_tool.c	  	\

CC	  = gcc
CFLAGS  ?= -Wall -g3 -Iinclude -I$(DEMETER_LIB_DIR)/include -I/usr/include/infiniband/
LDFLAGS ?= 

all: $(NAME)

default: $(NAME)

$(NAME): $(SRC_FILES)
		$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

send: all
		scp $(NAME) my_vm:/home/compose_fake_taranis/shared/

clean:
		rm -f $(NAME)

re: clean all