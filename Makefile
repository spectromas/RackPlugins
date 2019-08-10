# FLAGS will be passed to both the C and C++ compiler
FLAGS += 
CFLAGS +=
CXXFLAGS +=




# Add .cpp and .c files to the build
SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin is automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

# If RACK_DIR is not defined when calling the Makefile, default to two levels above
RACK_DIR ?= ../..

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
include $(RACK_DIR)/arch.mk

ifdef ARCH_LIN
	LDFLAGS += -lGL
endif

ifdef ARCH_MAC
	LDFLAGS += -framework OpenGL
endif

ifdef ARCH_WIN
	LDFLAGS +=  -lopengl32 
endif
