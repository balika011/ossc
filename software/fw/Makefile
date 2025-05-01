#------------------------------------------------------------------------------
#              VARIABLES APPENDED TO BY INCLUDED MAKEFILE FRAGMENTS
#------------------------------------------------------------------------------

# List of include directories for -I compiler option (-I added when used).
# Includes the BSP.
ALT_INCLUDE_DIRS :=

# List of library directories for -L linker option (-L added when used).
# Includes the BSP.
ALT_LIBRARY_DIRS :=

# List of library names for -l linker option (-l added when used).
# Includes the BSP.
ALT_LIBRARY_NAMES :=

# List of library names for -msys-lib linker option (-msys-lib added when used).
# These are libraries that might be located in the BSP and depend on the BSP
# library, or vice versa
ALT_BSP_DEP_LIBRARY_NAMES := libhal_bsp.a

# List of dependencies for the linker.  This is usually the full pathname
# of each library (*.a) file.
# Includes the BSP.
ALT_LDDEPS :=

# List of root library directories that support running make to build them.
# Includes the BSP and any ALT libraries.
MAKEABLE_LIBRARY_ROOT_DIRS :=

# Generic flags passed to the compiler for different types of input files.
ALT_CFLAGS :=
ALT_CXXFLAGS :=
ALT_CPPFLAGS :=
ALT_ASFLAGS :=
ALT_LDFLAGS :=


#------------------------------------------------------------------------------
#                         The adjust-path macro
#
# If COMSPEC/ComSpec is defined, Make is launched from Windows through
# Cygwin.  The adjust-path macro converts absolute windows paths into
# unix style paths (Example: c:/dir -> /c/dir). This will ensture
# paths are readable by GNU Make.
#
# If COMSPEC/ComSpec is not defined, Make is launched from linux, and no
# adjustment is necessary
#
#------------------------------------------------------------------------------

ifndef COMSPEC
ifdef ComSpec
COMSPEC = $(ComSpec)
endif # ComSpec
endif # COMSPEC

ifdef COMSPEC # if Windows OS

ifeq ($(MAKE_VERSION),3.81)
#
# adjust-path/adjust-path-mixed for Mingw Gnu Make on Windows
#
# Example Usage:
# $(call adjust-path,c:/aaa/bbb) => /c/aaa/bbb
# $(call adjust-path-mixed,/c/aaa/bbb) => c:/aaa/bbb
# $(call adjust-path-mixed,/cygdrive/c/aaa/bbb) => c:/aaa/bbb
#

#
# adjust-path
#  - converts back slash characters into forward slashes
#  - if input arg ($1) is an empty string then return the empty string
#  - if input arg ($1) does not contain the string ":/", then return input arg
#  - using sed, convert mixed path [c:/...] into mingw path [/c/...]
define adjust-path
$(strip \
$(if $1,\
$(if $(findstring :/,$(subst \,/,$1)),\
$(shell echo $(subst \,/,$1) | sed -e 's,^\([a-zA-Z]\):/,/\1/,'),\
$(subst \,/,$1))))
endef

#
# adjust-path-mixed
#  - converts back slash characters into forward slashes
#  - if input arg ($1) is an empty string then return the empty string
#  - if input arg ($1) does not begin with a forward slash '/' char, then
#    return input arg
#  - using sed, convert mingw path [/c/...] or cygwin path [/c/cygdrive/...]
#    into a mixed path [c:/...]
define adjust-path-mixed
$(strip \
$(if $1,\
$(if $(findstring $(subst \,/,$1),$(patsubst /%,%,$(subst \,/,$1))),\
$(subst \,/,$1),\
$(shell echo $(subst \,/,$1) | sed -e 's,^/cygdrive/\([a-zA-Z]\)/,\1:/,' -e 's,^/\([a-zA-Z]\)/,\1:/,'))))
endef

else # MAKE_VERSION != 3.81 (MAKE_VERSION == 3.80 or MAKE_VERSION == 3.79)
#
#  adjust-path for Cygwin Gnu Make
# $(call adjust-path,c:/aaa/bbb) = /cygdrive/c/aaa/bbb
# $(call adjust-path-mixed,/cygdrive/c/aaa/bbb) = c:/aaa/bbb
#
adjust-path = $(if $1,$(shell cygpath -u "$1"),)
adjust-path-mixed = $(if $1,$(shell cygpath -m "$1"),)
endif

else # !COMSPEC

adjust-path = $1
adjust-path-mixed = $1

endif # COMSPEC


#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#                           GENERATED SETTINGS START                         v
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

#START GENERATED
ACTIVE_BUILD_CONFIG := default
BUILD_CONFIGS := default

ELF := fw.elf

# Paths to C, C++, and assembly source files.
C_SRCS += av_controller.c
C_SRCS += src/avconfig.c
C_SRCS += src/controls.c
C_SRCS += src/fat16_export.c
C_SRCS += src/firmware.c
C_SRCS += src/flash.c
C_SRCS += src/lcd.c
ifeq ($(OSDLANG),JP)
C_SRCS += src/menu_sjis.c
else
C_SRCS += src/menu.c
endif
C_SRCS += src/sdcard.c
ifeq ($(OSDLANG),JP)
C_SRCS += src/userdata_sjis.c
else
C_SRCS += src/userdata.c
endif
C_SRCS += src/utils.c
C_SRCS += src/video_modes.c
C_SRCS += src/osd.c

CXX_SRCS :=
ASM_SRCS := crt0_init.S crt0_ram.S


# Path to root of object file tree.
OBJ_ROOT_DIR := obj

# Options to enable/disable optional files.
CREATE_LINKER_MAP := 1

# Common arguments for ALT_CFLAGSs
APP_CFLAGS_DEFINED_SYMBOLS :=
ifeq ($(OSDLANG),JP)
APP_CFLAGS_DEFINED_SYMBOLS += -DOSDLANG_JP
endif
APP_CFLAGS_UNDEFINED_SYMBOLS :=
APP_CFLAGS_OPTIMIZATION := -Os
APP_CFLAGS_DEBUG_LEVEL :=
APP_CFLAGS_WARNINGS := -Wall -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-function -Wno-packed-bitfield-compat -Wno-switch -Wno-multichar
APP_CFLAGS_USER_FLAGS := -g -fdata-sections -ffunction-sections -fshort-enums -fgnu89-inline -flto

APP_ASFLAGS_USER :=
APP_LDFLAGS_USER := -Wl,--gc-sections -g


LINKER_SCRIPT := link.riscv.ld

# Define path to the root of the BSP.
BSP_ROOT_DIR := ../bsp/

# List of application specific include directories, library directories and library names
APP_INCLUDE_DIRS += config
APP_INCLUDE_DIRS += inc
APP_INCLUDE_DIRS += memory
APP_LIBRARY_DIRS :=
APP_LIBRARY_NAMES :=

# Pre- and post- processor settings.
BUILD_PRE_PROCESS :=
BUILD_POST_PROCESS :=

QUARTUS_PROJECT_DIR := ../../


#------------------------------------------------------------------------------
#                           DEFAULT TARGET
#------------------------------------------------------------------------------

# Define the variable used to echo output if not already defined.
ifeq ($(ECHO),)
ECHO := echo
endif

# Put "all" rule before included makefile fragments because they may
# define rules and we don't want one of those to become the default rule.
.PHONY : all

all:
	@$(ECHO) [$(APP_NAME) build complete]

all : build_pre_process libs app build_post_process


#------------------------------------------------------------------------------
#                 VARIABLES DEPENDENT ON GENERATED CONTENT
#------------------------------------------------------------------------------

# Define object file directory per build configuration
CONFIG_OBJ_DIR := $(OBJ_ROOT_DIR)/$(ACTIVE_BUILD_CONFIG)

ifeq ($(BSP_ROOT_DIR),)
$(error Edit Makefile and provide a value for BSP_ROOT_DIR)
endif

ifeq ($(wildcard $(BSP_ROOT_DIR)),)
$(error BSP directory does not exist: $(BSP_ROOT_DIR))
endif

# Define absolute path to the root of the BSP.
ABS_BSP_ROOT_DIR := $(call adjust-path-mixed,$(shell cd "$(BSP_ROOT_DIR)"; pwd))

# Include makefile fragments.  Define variable ALT_LIBRARY_ROOT_DIR before
# including each makefile fragment so that it knows the path to itself.
BSP_INCLUDE_FILE := $(BSP_ROOT_DIR)/public.mk
ALT_LIBRARY_ROOT_DIR := $(BSP_ROOT_DIR)
include $(BSP_INCLUDE_FILE)
# C2H will need this to touch the BSP public.mk and avoid the sopc file
# out-of-date error during a BSP make
ABS_BSP_INCLUDE_FILE := $(ABS_BSP_ROOT_DIR)/public.mk

# Name of ELF application.
APP_NAME := $(basename $(ELF))

# Create list of linker dependencies (*.a files).
APP_LDDEPS := $(ALT_LDDEPS) $(LDDEPS)

# Take lists and add required prefixes.
APP_INC_DIRS := $(addprefix -I, $(ALT_INCLUDE_DIRS) $(APP_INCLUDE_DIRS) $(INC_DIRS))
ASM_INC_PREFIX := -Wa,-I
APP_ASM_INC_DIRS := $(addprefix $(ASM_INC_PREFIX), $(ALT_INCLUDE_DIRS) $(APP_INCLUDE_DIRS) $(INC_DIRS))
APP_LIB_DIRS := $(addprefix -L, $(ALT_LIBRARY_DIRS) $(APP_LIBRARY_DIRS) $(LIB_DIRS))
APP_LIBS := $(addprefix -l, $(ALT_LIBRARY_NAMES) $(APP_LIBRARY_NAMES) $(LIBS))

APP_BSP_DEP_LIBS := $(addprefix -l:, $(ALT_BSP_DEP_LIBRARY_NAMES))

# Arguments for the C preprocessor, C/C++ compiler, assembler, and linker.
APP_CFLAGS := $(APP_CFLAGS_DEFINED_SYMBOLS) \
              $(APP_CFLAGS_UNDEFINED_SYMBOLS) \
              $(APP_CFLAGS_OPTIMIZATION) \
              $(APP_CFLAGS_DEBUG_LEVEL) \
              $(APP_CFLAGS_WARNINGS) \
              $(APP_CFLAGS_USER_FLAGS) \
              $(ALT_CFLAGS) \
              $(CFLAGS)

# Arguments only for the C++ compiler.
APP_CXXFLAGS := $(ALT_CXXFLAGS) $(CXXFLAGS)

# Arguments only for the C preprocessor.
# Prefix each include directory with -I.
APP_CPPFLAGS := $(APP_INC_DIRS) \
                $(ALT_CPPFLAGS) \
                $(CPPFLAGS)

# Arguments only for the assembler.
APP_ASFLAGS := $(APP_ASM_INC_DIRS) \
               $(ALT_ASFLAGS) \
               $(APP_ASFLAGS_USER) \
               $(ASFLAGS)

# Arguments only for the linker.
APP_LDFLAGS := $(APP_LDFLAGS_USER) --specs=nano.specs -nostartfiles

ifneq ($(LINKER_SCRIPT),)
APP_LDFLAGS += -T'$(LINKER_SCRIPT)'
endif

APP_LDFLAGS += \
           $(APP_LIB_DIRS) \
           $(LDFLAGS)

LINKER_MAP_NAME := $(APP_NAME).map
ifeq ($(CREATE_LINKER_MAP), 1)
APP_LDFLAGS += -Wl,-Map=$(LINKER_MAP_NAME)
endif



#------------------------------------------------------------------------------
#           MAKEFILE INCLUDES DEPENDENT ON GENERATED CONTENT
#------------------------------------------------------------------------------
# Create list of object files to be built using the list of source files.
# The source file hierarchy is preserved in the object tree.
# The supported file extensions are:
#
# .c            - for C files
# .cxx .cc .cpp - for C++ files
# .S .s         - for assembler files
#
# Handle source files specified by --src-dir & --src-rdir differently, to
# save some processing time in calling the adjust-path macro.

OBJ_LIST_C 		:= $(patsubst %.c,%.o,$(filter %.c,$(C_SRCS)))
OBJ_LIST_CPP	:= $(patsubst %.cpp,%.o,$(filter %.cpp,$(CXX_SRCS)))
OBJ_LIST_CXX 	:= $(patsubst %.cxx,%.o,$(filter %.cxx,$(CXX_SRCS)))
OBJ_LIST_CC 	:= $(patsubst %.cc,%.o,$(filter %.cc,$(CXX_SRCS)))
OBJ_LIST_S 		:= $(patsubst %.S,%.o,$(filter %.S,$(ASM_SRCS)))
OBJ_LIST_SS		:= $(patsubst %.s,%.o,$(filter %.s,$(ASM_SRCS)))

OBJ_LIST := $(sort $(OBJ_LIST_C) $(OBJ_LIST_CPP) $(OBJ_LIST_CXX) \
				$(OBJ_LIST_CC) $(OBJ_LIST_S) $(OBJ_LIST_SS))

SDIR_OBJ_LIST_C		:= $(patsubst %.c,%.o,$(filter %.c,$(SDIR_C_SRCS)))
SDIR_OBJ_LIST_CPP	:= $(patsubst %.cpp,%.o,$(filter %.cpp,$(SDIR_CXX_SRCS)))
SDIR_OBJ_LIST_CXX 	:= $(patsubst %.cxx,%.o,$(filter %.cxx,$(SDIR_CXX_SRCS)))
SDIR_OBJ_LIST_CC 	:= $(patsubst %.cc,%.o,$(filter %.cc,$(SDIR_CXX_SRCS)))
SDIR_OBJ_LIST_S		:= $(patsubst %.S,%.o,$(filter %.S,$(SDIR_ASM_SRCS)))
SDIR_OBJ_LIST_SS	:= $(patsubst %.s,%.o,$(filter %.s,$(SDIR_ASM_SRCS)))

SDIR_OBJ_LIST := $(sort $(SDIR_OBJ_LIST_C) $(SDIR_OBJ_LIST_CPP) \
				$(SDIR_OBJ_LIST_CXX) $(SDIR_OBJ_LIST_CC) $(SDIR_OBJ_LIST_S) \
				$(SDIR_OBJ_LIST_SS))

# Relative-pathed objects that being with "../" are handled differently.
#
# Regular objects are created as
#   $(CONFIG_OBJ_DIR)/<path>/<filename>.o
# where the path structure is maintained under the obj directory.  This
# applies for both absolute and relative paths; in the absolute path
# case this means the entire source path will be recreated under the obj
# directory.  This is done to allow two source files with the same name
# to be included as part of the project.
#
# Note: On Cygwin, the path recreated under the obj directory will be
# the cygpath -u output path.
#
# Relative-path objects that begin with "../" cause problems under this
# scheme, as $(CONFIG_OBJ_DIR)/../<rest of path>/ can potentially put the object
# files anywhere in the system, creating clutter and polluting the source tree.
# As such, their paths are flattened - the object file created will be
# $(CONFIG_OBJ_DIR)/<filename>.o.  Due to this, two files specified with
# "../" in the beginning cannot have the same name in the project.  VPATH
# will be set for these sources to allow make to relocate the source file
# via %.o rules.
#
# The following lines separate the object list into the flatten and regular
# lists, and then handles them as appropriate.

FLATTEN_OBJ_LIST := $(filter ../%,$(OBJ_LIST))
FLATTEN_APP_OBJS := $(addprefix $(CONFIG_OBJ_DIR)/,$(notdir $(FLATTEN_OBJ_LIST)))

REGULAR_OBJ_LIST 		:= $(filter-out $(FLATTEN_OBJ_LIST),$(OBJ_LIST))
REGULAR_OBJ_LIST_C 		:= $(filter $(OBJ_LIST_C),$(REGULAR_OBJ_LIST))
REGULAR_OBJ_LIST_CPP	:= $(filter $(OBJ_LIST_CPP),$(REGULAR_OBJ_LIST))
REGULAR_OBJ_LIST_CXX 	:= $(filter $(OBJ_LIST_CXX),$(REGULAR_OBJ_LIST))
REGULAR_OBJ_LIST_CC 	:= $(filter $(OBJ_LIST_CC),$(REGULAR_OBJ_LIST))
REGULAR_OBJ_LIST_S 		:= $(filter $(OBJ_LIST_S),$(REGULAR_OBJ_LIST))
REGULAR_OBJ_LIST_SS		:= $(filter $(OBJ_LIST_SS),$(REGULAR_OBJ_LIST))

FLATTEN_SDIR_OBJ_LIST := $(filter ../%,$(SDIR_OBJ_LIST))
FLATTEN_SDIR_APP_OBJS := $(addprefix $(CONFIG_OBJ_DIR)/,$(notdir $(FLATTEN_SDIR_OBJ_LIST)))

REGULAR_SDIR_OBJ_LIST 		:= $(filter-out $(FLATTEN_SDIR_OBJ_LIST),$(SDIR_OBJ_LIST))
REGULAR_SDIR_OBJ_LIST_C 	:= $(filter $(SDIR_OBJ_LIST_C),$(REGULAR_SDIR_OBJ_LIST))
REGULAR_SDIR_OBJ_LIST_CPP	:= $(filter $(SDIR_OBJ_LIST_CPP),$(REGULAR_SDIR_OBJ_LIST))
REGULAR_SDIR_OBJ_LIST_CXX 	:= $(filter $(SDIR_OBJ_LIST_CXX),$(REGULAR_SDIR_OBJ_LIST))
REGULAR_SDIR_OBJ_LIST_CC 	:= $(filter $(SDIR_OBJ_LIST_CC),$(REGULAR_SDIR_OBJ_LIST))
REGULAR_SDIR_OBJ_LIST_S 	:= $(filter $(SDIR_OBJ_LIST_S),$(REGULAR_SDIR_OBJ_LIST))
REGULAR_SDIR_OBJ_LIST_SS	:= $(filter $(SDIR_OBJ_LIST_SS),$(REGULAR_SDIR_OBJ_LIST))

VPATH := $(sort $(dir $(FLATTEN_OBJ_LIST)) $(dir $(FLATTEN_SDIR_OBJ_LIST)))

APP_OBJS_C := $(addprefix $(CONFIG_OBJ_DIR)/,\
	$(REGULAR_SDIR_OBJ_LIST_C) \
	$(foreach s,$(REGULAR_OBJ_LIST_C),$(call adjust-path,$s)))

APP_OBJS_CPP := $(addprefix $(CONFIG_OBJ_DIR)/,\
	$(REGULAR_SDIR_OBJ_LIST_CPP) \
	$(foreach s,$(REGULAR_OBJ_LIST_CPP),$(call adjust-path,$s)))

APP_OBJS_CXX := $(addprefix $(CONFIG_OBJ_DIR)/,\
	$(REGULAR_SDIR_OBJ_LIST_CXX) \
	$(foreach s,$(REGULAR_OBJ_LIST_CXX),$(call adjust-path,$s)))

APP_OBJS_CC := $(addprefix $(CONFIG_OBJ_DIR)/,\
	$(REGULAR_SDIR_OBJ_LIST_CC) \
	$(foreach s,$(REGULAR_OBJ_LIST_CC),$(call adjust-path,$s)))

APP_OBJS_S := $(addprefix $(CONFIG_OBJ_DIR)/,\
	$(REGULAR_SDIR_OBJ_LIST_S) \
	$(foreach s,$(REGULAR_OBJ_LIST_S),$(call adjust-path,$s)))

APP_OBJS_SS := $(addprefix $(CONFIG_OBJ_DIR)/,\
	$(REGULAR_SDIR_OBJ_LIST_SS) \
	$(foreach s,$(REGULAR_OBJ_LIST_SS),$(call adjust-path,$s)))

APP_OBJS := $(APP_OBJS_C) $(APP_OBJS_CPP) $(APP_OBJS_CXX) $(APP_OBJS_CC) \
	$(APP_OBJS_S) $(APP_OBJS_SS) \
	$(FLATTEN_APP_OBJS) $(FLATTEN_SDIR_APP_OBJS)

# Add any extra user-provided object files.
APP_OBJS += $(OBJS)

# Create list of dependancy files for each object file.
APP_DEPS := $(APP_OBJS:.o=.d)


#------------------------------------------------------------------------------
#                           BUILD PRE/POST PROCESS
#------------------------------------------------------------------------------
build_pre_process :
	$(BUILD_PRE_PROCESS)

build_post_process :
	$(BUILD_POST_PROCESS)

.PHONY: build_pre_process build_post_process


#------------------------------------------------------------------------------
#                                 TOOLS
#------------------------------------------------------------------------------

#
# Set tool default variables if not already defined.
# If these are defined, they would typically be defined in an
# included makefile fragment.
#
ifeq ($(DEFAULT_CROSS_COMPILE),)
DEFAULT_CROSS_COMPILE := riscv32-unknown-elf-
endif

ifeq ($(DEFAULT_DOWNLOAD),)
DEFAULT_DOWNLOAD := anios2-download
endif

ifeq ($(DEFAULT_FLASHPROG),)
DEFAULT_FLASHPROG := anios2-flash-programmer
endif

ifeq ($(DEFAULT_RM),)
DEFAULT_RM := rm -f
endif

ifeq ($(DEFAULT_CP),)
DEFAULT_CP := cp -f
endif

ifeq ($(DEFAULT_MKDIR),)
DEFAULT_MKDIR := mkdir -p
endif

#
# Set tool variables to defaults if not already defined.
# If these are defined, they would typically be defined by a
# setting in the generated portion of this makefile.
#
ifeq ($(CROSS_COMPILE),)
CROSS_COMPILE := $(DEFAULT_CROSS_COMPILE)
endif

ifeq ($(origin CC),default)
CC := $(CROSS_COMPILE)gcc -xc
endif

ifeq ($(origin CXX),default)
CXX := $(CROSS_COMPILE)gcc -xc++
endif

ifeq ($(origin AS),default)
AS := $(CROSS_COMPILE)gcc
endif

ifeq ($(origin AR),default)
AR := $(CROSS_COMPILE)ar
endif

ifeq ($(origin LD),default)
LD := $(CROSS_COMPILE)g++
endif

ifeq ($(origin RM),default)
RM := $(DEFAULT_RM)
endif

ifeq ($(NM),)
NM := $(CROSS_COMPILE)nm
endif

ifeq ($(CP),)
CP := $(DEFAULT_CP)
endif

ifeq ($(OBJDUMP),)
OBJDUMP := $(CROSS_COMPILE)objdump
endif

ifeq ($(OBJCOPY),)
OBJCOPY := $(CROSS_COMPILE)objcopy
endif

ifeq ($(DOWNLOAD),)
DOWNLOAD := $(DEFAULT_DOWNLOAD)
endif

ifeq ($(FLASHPROG),)
FLASHPROG := $(DEFAULT_FLASHPROG)
endif

ifeq ($(MKDIR),)
MKDIR := $(DEFAULT_MKDIR)
endif

RV_OBJCOPY = riscv32-unknown-elf-objcopy

#------------------------------------------------------------------------------
#                     PATTERN RULES TO BUILD OBJECTS
#------------------------------------------------------------------------------

define compile.c
@$(ECHO) Info: Compiling $< to $@
@$(MKDIR) $(@D)
$(CC) -MP -MMD -c $(APP_CPPFLAGS) $(APP_CFLAGS) -o $@ $<
$(CC_POST_PROCESS)
endef

define compile.cpp
@$(ECHO) Info: Compiling $< to $@
@$(MKDIR) $(@D)
$(CXX) -MP -MMD -c $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<
$(CXX_POST_PROCESS)
endef

# If assembling with the compiler, ensure "-Wa," is prepended to all APP_ASFLAGS
ifeq ($(AS),$(patsubst %as,%,$(AS)))
COMMA := ,
APP_ASFLAGS :=  $(filter-out $(APP_CFLAGS),$(addprefix -Wa$(COMMA),$(patsubst -Wa$(COMMA)%,%,$(APP_ASFLAGS))))
endif

define compile.s
@$(ECHO) Info: Assembling $< to $@
@$(MKDIR) $(@D)
$(AS) -MP -MMD -c $(APP_CPPFLAGS) $(APP_CFLAGS) $(APP_ASFLAGS) -o $@ $<
$(AS_POST_PROCESS)
endef

ifeq ($(MAKE_VERSION),3.81)
.SECONDEXPANSION:

$(APP_OBJS_C): $(CONFIG_OBJ_DIR)/%.o: $$(call adjust-path-mixed,%.c)
	$(compile.c)

$(APP_OBJS_CPP): $(CONFIG_OBJ_DIR)/%.o: $$(call adjust-path-mixed,%.cpp)
	$(compile.cpp)

$(APP_OBJS_CC): $(CONFIG_OBJ_DIR)/%.o: $$(call adjust-path-mixed,%.cc)
	$(compile.cpp)

$(APP_OBJS_CXX): $(CONFIG_OBJ_DIR)/%.o: $$(call adjust-path-mixed,%.cxx)
	$(compile.cpp)

$(APP_OBJS_S): $(CONFIG_OBJ_DIR)/%.o: $$(call adjust-path-mixed,%.S)
	$(compile.s)

$(APP_OBJS_SS): $(CONFIG_OBJ_DIR)/%.o: $$(call adjust-path-mixed,%.s)
	$(compile.s)

endif # MAKE_VERSION != 3.81

$(CONFIG_OBJ_DIR)/%.o: %.c
	$(compile.c)

$(CONFIG_OBJ_DIR)/%.o: %.cpp
	$(compile.cpp)

$(CONFIG_OBJ_DIR)/%.o: %.cc
	$(compile.cpp)

$(CONFIG_OBJ_DIR)/%.o: %.cxx
	$(compile.cpp)

$(CONFIG_OBJ_DIR)/%.o: %.S
	$(compile.s)

$(CONFIG_OBJ_DIR)/%.o: %.s
	$(compile.s)


#------------------------------------------------------------------------------
#                     PATTERN RULES TO INTERMEDIATE FILES
#------------------------------------------------------------------------------

$(CONFIG_OBJ_DIR)/%.s: %.c
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CC) -S $(APP_CPPFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.s: %.cpp
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CXX) -S $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.s: %.cc
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CXX) -S $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.s: %.cxx
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CXX) -S $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.i: %.c
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CC) -E $(APP_CPPFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.i: %.cpp
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CXX) -E $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.i: %.cc
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CXX) -E $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<

$(CONFIG_OBJ_DIR)/%.i: %.cxx
	@$(ECHO) Info: Compiling $< to $@
	@$(MKDIR) $(@D)
	$(CXX) -E $(APP_CPPFLAGS) $(APP_CXXFLAGS) $(APP_CFLAGS) -o $@ $<


#------------------------------------------------------------------------------
#                        TARGET RULES
#------------------------------------------------------------------------------

.PHONY : help
help :
	@$(ECHO) "Summary of Makefile targets"
	@$(ECHO) "  Build targets:"
	@$(ECHO) "    all (default)     - Application and all libraries (including BSP)"
	@$(ECHO) "    bsp               - Just the BSP"
	@$(ECHO) "    libs              - All libraries (including BSP)"
	@$(ECHO) "    flash             - All flash files"
	@$(ECHO) "    mem_init_generate - All memory initialization files"
	@$(ECHO) "    mem_init_install  - Copy memory initialization files to Quartus II project"
	@$(ECHO)
	@$(ECHO) "  Clean targets:"
	@$(ECHO) "    clean_all         - Application and all libraries (including BSP)"
	@$(ECHO) "    clean             - Just the application"
	@$(ECHO) "    clean_bsp         - Just the BSP"
	@$(ECHO) "    clean_libs        - All libraries (including BSP)"
	@$(ECHO)
	@$(ECHO) "  Run targets:"
	@$(ECHO) "    download-elf      - Download and run your elf executable"
	@$(ECHO) "    program-flash     - Program flash contents to the board"

# Handy rule to skip making libraries and just make application.
.PHONY : app
app : $(ELF)

.PHONY: elf_derived_files
elf_derived_files: default_mem_init

# Handy rule for making just the BSP.
.PHONY : bsp
bsp :
	@$(ECHO) Info: Building $(BSP_ROOT_DIR)
	@$(MAKE) --no-print-directory -C $(BSP_ROOT_DIR)


# Make sure all makeable libraries (including the BSP) are up-to-date.
LIB_TARGETS := $(patsubst %,%-recurs-make-lib,$(MAKEABLE_LIBRARY_ROOT_DIRS))

.PHONY : libs
libs : $(LIB_TARGETS)

ifneq ($(strip $(LIB_TARGETS)),)
$(LIB_TARGETS): %-recurs-make-lib:
	@$(ECHO) Info: Building $*
	$(MAKE) --no-print-directory -C $*
endif

ifneq ($(strip $(APP_LDDEPS)),)
$(APP_LDDEPS): libs
	@true
endif

# Rules to force your project to rebuild or relink
# .force_relink file will cause any application that depends on this project to relink
# .force_rebuild file will cause this project to rebuild object files
# .force_rebuild_all file will cause this project and any project that depends on this project to rebuild object files

FORCE_RELINK_DEP  := .force_relink
FORCE_REBUILD_DEP := .force_rebuild
FORCE_REBUILD_ALL_DEP := .force_rebuild_all
FORCE_REBUILD_DEP_LIST := $(CONFIG_OBJ_DIR)/$(FORCE_RELINK_DEP) $(CONFIG_OBJ_DIR)/$(FORCE_REBUILD_DEP) $(FORCE_REBUILD_ALL_DEP)

$(FORCE_REBUILD_DEP_LIST):

$(APP_OBJS): $(wildcard $(CONFIG_OBJ_DIR)/$(FORCE_REBUILD_DEP)) $(wildcard $(addsuffix /$(FORCE_REBUILD_ALL_DEP), . $(ALT_LIBRARY_DIRS)))

$(ELF): $(wildcard $(addsuffix /$(FORCE_RELINK_DEP), $(CONFIG_OBJ_DIR) $(ALT_LIBRARY_DIRS)))


# Clean just the application.
.PHONY : clean

clean :
	@$(RM) -r $(ELF) $(LINKER_MAP_NAME) $(OBJ_ROOT_DIR) $(FORCE_REBUILD_DEP_LIST) src/menu_sjis.c src/userdata_sjis.c
	@$(ECHO) [$(APP_NAME) clean complete]

# Clean just the BSP.
.PHONY : clean_bsp
clean_bsp :
	@$(ECHO) Info: Cleaning $(BSP_ROOT_DIR)
	@$(MAKE) --no-print-directory -C $(BSP_ROOT_DIR) clean

# Clean all makeable libraries including the BSP.
LIB_CLEAN_TARGETS := $(patsubst %,%-recurs-make-clean-lib,$(MAKEABLE_LIBRARY_ROOT_DIRS))

.PHONY : clean_libs
clean_libs : $(LIB_CLEAN_TARGETS)

ifneq ($(strip $(LIB_CLEAN_TARGETS)),)
$(LIB_CLEAN_TARGETS): %-recurs-make-clean-lib:
	@$(ECHO) Info: Cleaning $*
	$(MAKE) --no-print-directory -C $* clean
endif

.PHONY: clean_elf_derived_files
clean_elf_derived_files: mem_init_clean

# Clean application and all makeable libraries including the BSP.
.PHONY : clean_all
clean_all : clean mem_init_clean clean_libs

# Include the dependency files unless the make goal is performing a clean
# of the application.
ifneq ($(firstword $(MAKECMDGOALS)),clean)
ifneq ($(firstword $(MAKECMDGOALS)),clean_all)
-include $(APP_DEPS)
endif
endif

# Delete the target of a rule if it has changed and its commands exit
# with a nonzero exit status.
.DELETE_ON_ERROR:

# Rules for flash programming commands
PROGRAM_FLASH_SUFFIX := -program
PROGRAM_FLASH_TARGET := $(addsuffix $(PROGRAM_FLASH_SUFFIX), $(FLASH_FILES))

.PHONY : program-flash
program-flash : $(PROGRAM_FLASH_TARGET)

.PHONY : $(PROGRAM_FLASH_TARGET)
$(PROGRAM_FLASH_TARGET) : flash
	@if [ "$(FLASHPROG)" = "none" ]; \
	then \
		$(ECHO) Programming flash not supported; \
	else \
		$(ECHO) Info: Programming $(basename $@).flash; \
		if [ -z "$($(basename $@)_EPCS_FLAGS)" ]; \
		then \
			$(ECHO) $(FLASHPROG) $(SOPC_SYSID_FLAG) --base=$($(basename $@)_START) $(basename $@).flash; \
			$(FLASHPROG) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) --base=$($(basename $@)_START) $(basename $@).flash; \
		else \
			$(ECHO) $(FLASHPROG) $(SOPC_SYSID_FLAG) --epcs --base=$($(basename $@)_START) $(basename $@).flash; \
			$(FLASHPROG) $(DOWNLOAD_CABLE_FLAG) $(SOPC_SYSID_FLAG) --epcs --base=$($(basename $@)_START) $(basename $@).flash; \
		fi \
	fi


#------------------------------------------------------------------------------
#                         ELF TARGET RULE
#------------------------------------------------------------------------------
# Rule for constructing the executable elf file.
$(ELF) : $(APP_OBJS) $(LINKER_SCRIPT) $(APP_LDDEPS)
	@$(ECHO) Info: Linking $@
	$(LD) $(APP_LDFLAGS) $(APP_CFLAGS) -o $@ $(APP_OBJS) $(APP_LIBS) $(APP_BSP_DEP_LIBS)

# Rule for printing the name of the elf file
.PHONY: print-elf-name
print-elf-name:
	@$(ECHO) $(ELF)


#------------------------------------------------------------------------------
#                         CUSTOM TARGETS
#------------------------------------------------------------------------------
src/menu_sjis.c: src/menu.c
	iconv -f UTF-8 -t SHIFT-JIS src/menu.c > src/menu_sjis.c

src/userdata_sjis.c: src/userdata.c
	iconv -f UTF-8 -t SHIFT-JIS src/userdata.c > src/userdata_sjis.c

fw.bin: fw.elf
	$(RV_OBJCOPY) --change-addresses -0x10000 -O binary --gap-fill 0 $< fw.bin

.PHONY: mem_init_generate_new
mem_init_generate_new: fw.bin

.PHONY: generate_bin
generate_bin: clean mem_init_generate_new

