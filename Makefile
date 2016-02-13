VALID_TOOLCHAINS := pnacl

TARGET = moonlight-chrome

# Include Moonlight-Common-C makefile
include common-c.mk

EXTRA_INC_PATHS := $(EXTRA_INC_PATHS) $(COMMON_C_INCLUDE)

include $(NACL_SDK_ROOT)/tools/common.mk

# Dirty hack to allow 'make serve' to work in this directory
HTTPD_PY := $(HTTPD_PY) --no-dir-check


LIBS = ppapi_gles2 ppapi ppapi_cpp pthread nacl_io

CFLAGS = -Wall -Wno-missing-braces
SOURCES = \
    $(COMMON_C_SOURCE)       \
    libchelper.c             \
    main.cpp                 \
    input.cpp                \
    gamepad.cpp              \
    connectionlistener.cpp   \
    viddec.cpp               \

# Build rules generated by macros from common.mk:

$(foreach src,$(SOURCES),$(eval $(call COMPILE_RULE,$(src),$(CFLAGS))))

# The PNaCl workflow uses both an unstripped and finalized/stripped binary.
# On NaCl, only produce a stripped binary for Release configs (not Debug).
ifneq (,$(or $(findstring pnacl,$(TOOLCHAIN)),$(findstring Release,$(CONFIG))))
$(eval $(call LINK_RULE,$(TARGET)_unstripped,$(SOURCES),$(LIBS),$(DEPS)))
$(eval $(call STRIP_RULE,$(TARGET),$(TARGET)_unstripped))
else
$(eval $(call LINK_RULE,$(TARGET),$(SOURCES),$(LIBS),$(DEPS)))
endif

$(eval $(call NMF_RULE,$(TARGET),))
