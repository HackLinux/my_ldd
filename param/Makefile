#################################################################
#                copyleft @ 2010 2011 Late Lee
# file name:Makefile
# A simple Makefile for device driver
# by Late Lee<www.latelee.org>
# 
# note:
# You need to change your module name & obj file(s),and you may
# also need to change 'KERNELDIR'.
# I hope you can see your module(sth like xx.ko) if you are lucky
# enough.
#################################################################

#DEBUG = y
ifeq ($(DEBUG), y)
	DEBFLAGS = -O -g
else
	DEBFLAGS = -O1
endif
INCDIR := 
#CFLAGS += $(DEBFLAGS) -I$(INCDIR)
EXTRA_CFLAGS += -DDEBUG

MODULE = GotoHell

$(MODULE)-objs := param_test.o

ifneq ($(KERNELRELEASE), )
	obj-m := $(MODULE).o

else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all:
	$(MAKE_BEGIN)
	@echo 
	@if \
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules;\
	then $(MAKE_DONE);\
	else \
	$(MAKE_ERR);\
	exit 1; \
	fi
endif

clean:
	$(CLEAN_BEGIN)
	rm -rf *.cmd *.o *.ko *.mod.c *.symvers *.order *.markers \
	.tmp_versions .*.cmd *~ .*.d .*.tmp
	$(CLEAN_END)

install:
	@echo -e "$(COLOR3)                   Note:"
	@echo -e "To install or not install,that is a question.$(RESET)"

.PHONY:all clean install fuck the world
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
### nothing
OFFSET=\x1b[21G   # 21 col
COLOR1=\x1b[0;32m # green
COLOR2=\x1b[1;35m # pink
COLOR3=\x1b[1;31m # red
RESET=\x1b[0m

CLEAN_BEGIN=@echo -e "$(OFFSET)$(COLOR2)Cleaning up ...$(RESET)"
CLEAN_END=@echo -e "$(OFFSET)$(COLOR2)[Done.]$(RESET)"

MAKE_BEGIN=@echo -ne "$(OFFSET)$(COLOR1)Compiling ...$(RESET)"
### I do not forget "@", but it DOES NOT need "@"
MAKE_DONE=echo -e "$(OFFSET)$(COLOR1)[Job done!]$(RESET)"
MAKE_ERR=echo -e "$(OFFSET)$(COLOR3)[Oops!Error occurred]$(RESET)"
### nothing end here

############# Makefile end here
