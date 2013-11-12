#################################################################
#                copyleft @ 2010 2011 2013 Late Lee
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

### nothing
OFFSET=\x1b[21G   # 21 col
COLOR1=\x1b[0;32m # green
COLOR2=\x1b[1;35m # 
COLOR3=\x1b[1;31m # red
RESET=\x1b[0m

CLEAN_BEGIN=@echo -e "$(OFFSET)$(COLOR2)Cleaning up ...$(RESET)"
CLEAN_END=@echo -e "$(OFFSET)$(COLOR2)[Done.]$(RESET)"

MAKE_BEGIN=@echo -ne "$(OFFSET)$(COLOR1)Compiling ...$(RESET)"
MAKE_DONE="$(OFFSET)$(COLOR1)[Job done!]$(RESET)";
MAKE_ERR="$(OFFSET)$(COLOR3)[Oops!Error occurred]$(RESET)";
### nothing end here

# for cross compile
# and your kernel dir must change
#CROSS_COMPILE = arm-linux-
#CC := $(CROSS_COMPILE)gcc
#LD := $(CROSS_COMPILE)ld

#DEBUG = y
ifeq ($(DEBUG), y)
	DEBFLAGS = -O -g
else
	DEBFLAGS = -O1
endif

# head files
INCDIR := .

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I$(INCDIR_IN)

# module name here
MODULE = GotoHell

# obj-m = module
# obj-y = into kernel
# foo.o -> foo.ko
ifneq ($(KERNELRELEASE), )
	obj-m := $(MODULE).o

# your obj file(s) here
	$(MODULE)-objs := foo-char.o

else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all:
	$(MAKE_BEGIN)
	@echo 
	@if \
    $(MAKE) -C $(KERNELDIR) M=$(PWD) INCDIR_IN=$(INCDIR) modules; \
	#$(MAKE) -C $(KERNELDIR) M=$(PWD) modules;\
	then echo -e $(MAKE_DONE)\
	else \
	echo -e  $(MAKE_ERR)\
	exit 1; \
	fi
endif

clean:
	$(CLEAN_BEGIN)
	rm -rf *.cmd *.o *.ko *.mod.c *.symvers *.order *.markers \
	.tmp_versions .*.cmd *~
	$(CLEAN_END)

install:
	@echo -e "$(COLOR3)                   Note:"
	@echo -e "To install or not install,that is a question.$(RESET)"

modules:
	@echo -e "$(COLOR3)Do not need to do this.$(RESET)"

modules_install:
	@echo -e "$(COLOR3)Do not need to do this.$(RESET)"
love:
	@echo -e "$(COLOR3)To make or not to make, that is also a question.$(RESET)"

.PHONY:all clean install love modules modules_install

############# Makefile end here