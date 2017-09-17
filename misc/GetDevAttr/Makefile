CC:=$(CROSS)gcc
AR:=$(CROSS)ar

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
# REL_DIR = $(PWD)/../../../
REL_LIB := $(REL_DIR)/lib
REL_INC := $(REL_DIR)/include

AUDIO_LIBA := $(REL_LIB)/libVoiceEngine.a \
		$(REL_LIB)/libaec.a	\
		$(REL_LIB)/libresampler.a \
		$(REL_LIB)/libanr.a\
		$(REL_LIB)/libvqev2.a

SRC  := $(wildcard *.c)
OBJ  := $(SRC:%.c=%.o)

INC_FLAGS := -I$(REL_INC)
CFLAGS += -Wall -g -lpthread $(INC_FLAGS)
# LIBS += $(REL_LIB)/libisp.a
LIBS += $(REL_LIB)/libmpi.a

CFLAGS += -DHI_3518	

# LIBS += $(REL_DIR)/sensor_lib/libsns_ar0130_720p.a
# CFLAGS += -DSNS_AR0130

# LIBS += $(REL_LIB)/lib_hiae.a
# LIBS += $(REL_LIB)/lib_hiawb.a
# LIBS += $(REL_LIB)/lib_hiaf.a
# CFLAGS += -DISP_V2 -lm

TARGET := $(OBJ:%.o=%)

.PHONY : clean all $(SNS)

all: $(TARGET)

$(TARGET):%:%.o 
	$(CC) $(CFLAGS)  -o $@ $^ $(LIBS) $(AUDIO_LIBA) -Wl,-gc-sections
	@rm *.o
clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)
ifneq ($(SNS), clean)
$(SNS): all
	@echo
endif
