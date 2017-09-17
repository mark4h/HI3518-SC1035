CC:=$(CROSS)gcc
AR:=$(CROSS)ar
LD:=$(CROSS)ld

MPP_CFLAGS:= -Wall
MPP_CFLAGS += -O2
#********************* Macro for version management***************************** 
VER_X ?= 1
VER_Y ?= 0
VER_Z ?= 0
VER_P ?= 0
MPP_CFLAGS += -DVER_X=$(VER_X) -DVER_Y=$(VER_Y) -DVER_Z=$(VER_Z) -DVER_P=$(VER_P)
#******************************************************************************* 

# SDK_PATH?=$(shell cd $(PWD)/`dirname $(PARAM_FILE)`/..; pwd)
# export MPP_PATH?=$(SDK_PATH)/mpp2
# REL_DIR = $(MPP_PATH)
# REL_DIR = $(PWD)/../../
REL_LIB := $(REL_DIR)/lib
REL_INC := $(REL_DIR)/include

# AUDIO_LIBA := $(REL_LIB)/libVoiceEngine.a \
		# $(REL_LIB)/libaec.a	\
		# $(REL_LIB)/libresampler.a \
		# $(REL_LIB)/libanr.a\
		# $(REL_LIB)/libvqev2.a

#INC_FLAGS := -I$(REL_INC)
#CFLAGS += -Wall -g -lpthread $(INC_FLAGS)
# LIBS += $(REL_LIB)/libisp.a
#LIBS += $(REL_LIB)/libmpi.a

#CFLAGS += -DHI_3518	

#LIBS += $(REL_DIR)/sensor_lib/libsns_ar0130_720p.a
#CFLAGS += -DSNS_AR0130

# LIBS += $(REL_LIB)/lib_hiae.a
# LIBS += $(REL_LIB)/lib_hiawb.a
# LIBS += $(REL_LIB)/lib_hiaf.a
# CFLAGS += -DISP_V2 -lm

obj-m := hi_i2c.o
hi_i2c-y += hii2c.o

export HI_ARCH?=3518
EXTRA_CFLAGS += -DHI_ARCH=$(HI_ARCH)
export ARCH=arm
export HIARCH?=hi3518
export LIBC?=uclibc

all:
	$(CC) -g -Wall -o i2c_read i2c_read.c	strfunc.c
	$(CC) -g -Wall -o i2c_write i2c_write.c	strfunc.c
	# make -C $(LINUX_ROOT) modules_prepare
	make -C $(LINUX_ROOT) M=$(PWD) modules

clean:
	make -C $(LINUX_ROOT) M=$(PWD) clean
	rm -f  i2c_read i2c_write

