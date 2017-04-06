################################################################################
################################ Configuration #################################
################################################################################

CXX = g++
CXXFLAGS = -std=c++14
CXXLDFLAGS = -pthread -lpng
CXXFLAGS_DEBUG = -g -O0
CXXFLAGS_RELEASE = -DNDEBUG -O2

# Default target.
all: all_things

# Debug mode.
DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CXXFLAGS += $(CXXFLAGS_DEBUG)
  CCFLAGS += $(CCFLAGS_DEBUG)
  MODE = debug
else
  CXXFLAGS += $(CXXFLAGS_RELEASE)
  CCFLAGS += $(CCFLAGS_RELEASE)
  MODE = release
endif

OUTER_BIN = bin
BIN = $(OUTER_BIN)/$(MODE)
EXE = prog.e
GRID_SRC = grid
COMMUNICATION_SRC = communication
SRC = src

GRID_HEADERS_DIR = $(BIN)/grid
COMMUNICATION_HEADERS_DIR = $(BIN)/communication


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
############################ Compiling grid sources ############################
################################################################################

# All dirs in $(GRID_SRC) directory.
GRID_SRC_DIRS = $(shell find $(GRID_SRC) -type d)
# All cpp files in $(GRID_SRC) directory.
GRID_SRC_SOURCES = $(shell find $(GRID_SRC) -name '*.cpp')
# All header files in $(GRID_SRC) directory.
GRID_SRC_HEADERS = $(shell find $(GRID_SRC) -name '*.h')

# Compiles sources to object files.
$(addprefix $(BIN)/, $(addsuffix .o, $(GRID_SRC_SOURCES))): \
		$(BIN)/%.o: % $(GRID_SRC_HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GRID_SRC) -c $< -o $@

# Appends normal object files to the list of all objects.
OBJS += $(addprefix $(BIN)/, $(addsuffix .o, $(GRID_SRC_SOURCES)))


################################################################################
################################ Copying headers ###############################
################################################################################

GRID_COPIED_HEADERS = $(addprefix $(GRID_HEADERS_DIR)/, $(GRID_SRC_HEADERS))

$(GRID_COPIED_HEADERS): $(GRID_HEADERS_DIR)/%: %
	@mkdir -p $(dir $@)
	cp $^ $@


################################################################################
######################## Compiling communication sources #######################
################################################################################

# All dirs in $(COMMUNICATION_SRC) directory.
COMMUNICATION_SRC_DIRS = $(shell find $(COMMUNICATION_SRC) -type d)
# All cpp files in $(COMMUNICATION_SRC) directory.
COMMUNICATION_SRC_SOURCES = $(shell find $(COMMUNICATION_SRC) -name '*.cpp')
# All header files in $(COMMUNICATION_SRC) directory.
COMMUNICATION_SRC_HEADERS = $(shell find $(COMMUNICATION_SRC) -name '*.h')

# Compiles sources to object files.
$(addprefix $(BIN)/, $(addsuffix .o, $(COMMUNICATION_SRC_SOURCES))): \
		$(BIN)/%.o: % $(COMMUNICATION_SRC_HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(COMMUNICATION_SRC) -c $< -o $@

# Appends normal object files to the list of all objects.
OBJS += $(addprefix $(BIN)/, $(addsuffix .o, $(COMMUNICATION_SRC_SOURCES)))


################################################################################
################################ Copying headers ###############################
################################################################################

COMMUNICATION_COPIED_HEADERS = \
		$(addprefix $(COMMUNICATION_HEADERS_DIR)/, $(COMMUNICATION_SRC_HEADERS))

$(COMMUNICATION_COPIED_HEADERS): $(COMMUNICATION_HEADERS_DIR)/%: %
	@mkdir -p $(dir $@)
	cp $^ $@


################################################################################
########################### Compiling normal sources ###########################
################################################################################

# All dirs in $(SRC) directory.
SRC_DIRS = $(shell find $(GRID_SRC) -type d)
# All cpp files in $(SRC) directory.
SRC_SOURCES = $(shell find $(SRC) -name '*.cpp')
# All header files in $(SRC) directory.
SRC_HEADERS = $(shell find $(SRC) -name '*.h')

# Compiles sources to object files.
$(addprefix $(BIN)/, $(addsuffix .o, $(SRC_SOURCES))): \
		$(BIN)/%.o: % \
				$(SRC_HEADERS) $(GRID_COPIED_HEADERS) $(COMMUNICATION_COPIED_HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ \
			$(CXXFLAGS) -I$(SRC) -I$(BIN)/$(GRID_SRC)/ -I$(BIN)/$(COMMUNICATION_SRC)

# Appends normal object files to the list of all objects.
OBJS += $(addprefix $(BIN)/, $(addsuffix .o, $(SRC_SOURCES)))


################################################################################
################################### Linking ####################################
################################################################################

# Creates the global object file.
$(BIN)/$(EXE): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(CXXLDFLAGS)

$(OUTER_BIN)/$(EXE): $(BIN)/$(EXE)
	cp $^ $@


################################################################################
################################### Cleaning ###################################
################################################################################

.PHONY: clean
clean:
	rm -rf $(OUTER_BIN)


################################################################################
################################## All things ##################################
################################################################################

all_things: $(OUTER_BIN)/$(EXE)
