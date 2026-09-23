# PROGRAM CONFIG

# can redefine build dir
BUILD_DIR    ?= build
SRC_DIR      = src
INCLUDE_DIRS = include
EXECUTABLE   = interpreter.o

# includes SOURCES variable
# do not use -include, because it ignores files that could not be found
include $(SRC_DIR)/sources.mk

OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))
DEPS = $(patsubst %.o,%.d,$(OBJS))

LIBS = 

# INCLUDE
INCLUDE_DIRS_ALL = $(INCLUDE_DIRS)

# COMPILER CONFIG

# if not defined by default, than override
ifeq ($(origin CC), default) 
	CC = g++
endif

CPPFLAGS_DEBUG 	 = -D _DEBUG -ggdb3 -O2 -g -march=native

CPPFLAGS_RELEASE = -DNDEBUG -O2 -march=native

CPPFLAGS_ASAN    = -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

ifeq "$(TARGET)" "Release"
	CPPFLAGS_TARGET = $(CPPFLAGS_RELEASE)
else
	CPPFLAGS_TARGET = $(CPPFLAGS_DEBUG) $(CPPFLAGS_ASAN)
endif

CPPFLAGS_WARNINGS = -Wall -Wextra -Werror=vla

CPPFLAGS_DEFINES = $(addprefix -D,$(DEFINE))

# because CFLAGS could be origined as external environment variable
override CFLAGS += -std=c++20 $(addprefix -I,$(INCLUDE_DIRS_ALL)) $(CPPFLAGS_WARNINGS) $(CPPFLAGS_DEFINES) $(CPPFLAGS_TARGET)

# targets which do not require dependencies
NODEPS = clean

.PHONY: all
all: $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJS)
	@echo -n Linking $@...
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo done

$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Building $@...
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

$(DEPS): $(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	# just running preprocessor to generate include-dependencies
	@$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: run
run: $(BUILD_DIR)/$(EXECUTABLE)
	@./$<

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

# in order to avoid rebuilding deps for NODEPS targets
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif
