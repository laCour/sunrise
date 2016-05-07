CC := g++
LD := ld -r -b binary
SRC_DIR := src
BUILD_DIR := build
TARGET := bin/sunrise
SOURCES := $(shell find $(SRC_DIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.cpp=.o))
RESOURCES := $(shell find $(SRC_DIR) -type f -name *.xml)
STATIC_OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(RESOURCES:.xml=.o))
CFLAGS := -g $(shell pkg-config --cflags dbus-1)
LIB := -pthread $(shell pkg-config --libs dbus-1)

$(TARGET): $(STATIC_OBJECTS) $(OBJECTS)
	@mkdir -p bin/
	@echo "$(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "$(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.xml
	@mkdir -p $(BUILD_DIR)
	@echo "$(LD) -o $@ $<"; $(LD) -o $@ $<

clean:
	@echo "$(RM) -r $(BUILD_DIR) $(TARGET)"; $(RM) -r $(BUILD_DIR) $(TARGET)
