################################################################################
################################ Configuration #################################
################################################################################

CXX = g++
CXXFLAGS = -std=c++14
CXXLDFLAGS = -pthread
CXXFLAGS_DEBUG = -g -O0
CXXFLAGS_RELEASE = -DNDEBUG -O2

BIN = bin
EXE = run.e
SRC = src


################################################################################
################################ Gtkmm Options  ################################
################################################################################

GTKMM_INCLUDE_PATHS =                                     \
	-isystem /usr/include/gtkmm-3.0                         \
	-isystem /usr/lib/x86_64-linux-gnu/gtkmm-3.0/include    \
	-isystem /usr/include/atkmm-1.6                         \
	-isystem /usr/include/giomm-2.4                         \
	-isystem /usr/lib/x86_64-linux-gnu/giomm-2.4/include    \
	-isystem /usr/include/pangomm-1.4                       \
	-isystem /usr/lib/x86_64-linux-gnu/pangomm-1.4/include  \
	-isystem /usr/include/gtk-3.0                           \
	-isystem /usr/include/cairomm-1.0                       \
	-isystem /usr/lib/x86_64-linux-gnu/cairomm-1.0/include  \
	-isystem /usr/include/gdk-pixbuf-2.0                    \
	-isystem /usr/include/gtk-3.0/unix-print                \
	-isystem /usr/include/gdkmm-3.0                         \
	-isystem /usr/lib/x86_64-linux-gnu/gdkmm-3.0/include    \
	-isystem /usr/include/atk-1.0                           \
	-isystem /usr/include/glibmm-2.4                        \
	-isystem /usr/lib/x86_64-linux-gnu/glibmm-2.4/include   \
	-isystem /usr/include/glib-2.0                          \
	-isystem /usr/lib/x86_64-linux-gnu/glib-2.0/include     \
	-isystem /usr/include/sigc++-2.0                        \
	-isystem /usr/lib/x86_64-linux-gnu/sigc++-2.0/include   \
	-isystem /usr/include/pango-1.0                         \
	-isystem /usr/include/cairo                             \
	-isystem /usr/include/pixman-1                          \
	-isystem /usr/include/freetype2                         \
	-isystem /usr/include/libpng12                          \
	-isystem /usr/include/at-spi2-atk/2.0                   \
	-isystem /usr/include/gio-unix-2.0/                     \
	-isystem /usr/include/harfbuzz

CXXFLAGS += $(GTKMM_INCLUDE_PATHS)

GTKMM_LINK_OPTIONS =  \
	-lgtkmm-3.0         \
	-latkmm-1.6         \
	-lgdkmm-3.0         \
	-lgiomm-2.4         \
	-lpangomm-1.4       \
	-lgtk-3             \
	-lglibmm-2.4        \
	-lcairomm-1.0       \
	-lgdk-3             \
	-latk-1.0           \
	-lgio-2.0           \
	-lpangocairo-1.0    \
	-lgdk_pixbuf-2.0    \
	-lcairo-gobject     \
	-lpango-1.0         \
	-lcairo             \
	-lsigc-2.0          \
	-lgobject-2.0       \
	-lglib-2.0

CXXLDFLAGS += $(GTKMM_LINK_OPTIONS)


################################################################################
############################ Script initialization #############################
################################################################################

# Default target.
all: $(BIN)/$(EXE)

# Debug mode.
DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CXXFLAGS += $(CXXFLAGS_DEBUG)
  CCFLAGS += $(CCFLAGS_DEBUG)
else
  CXXFLAGS += $(CXXFLAGS_RELEASE)
  CCFLAGS += $(CCFLAGS_RELEASE)
endif


################################################################################
########################### Compiling normal sources ###########################
################################################################################

# All dirs in $(SRC) directory.
SRC_DIRS = $(shell find $(SRC) -type d)
# All cpp files in $(SRC) directory.
SRC_SOURCES = $(shell find $(SRC) -name '*.cpp')
# All header files in $(SRC) directory.
SRC_HEADERS = $(shell find $(SRC) -name '*.h')

# Compiles sources to object files.
$(addprefix $(BIN)/, $(addsuffix .o, $(SRC_SOURCES))): \
		$(BIN)/%.o: % $(SRC_HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC) -c $< -o $@

# Appends normal object files to the list of all objects.
OBJS = $(addprefix $(BIN)/, $(addsuffix .o, $(SRC_SOURCES)))


################################################################################
################################### Linking ####################################
################################################################################

# Links all object files into the final executable.
$(BIN)/$(EXE): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(CXXLDFLAGS)


################################################################################
################################### Cleaning ###################################
################################################################################

.PHONY: clean
clean:
	rm -rf $(BIN)
