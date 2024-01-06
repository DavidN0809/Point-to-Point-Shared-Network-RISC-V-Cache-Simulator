# Compiler and flags
CXX      := -c++
CXXFLAGS := -std=c++17 -Wall
LDFLAGS  := -L/usr/lib -lstdc++ -lm
OBJ_DIR  := ./obj
DEP_DIR  := ./dep
TARGET   := test
INCLUDE  := -Iinclude/
SRC      := $(wildcard src/*.cpp)

OBJECTS  := $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:$(OBJ_DIR)/%.o=$(DEP_DIR)/%.d)

all: build $(TARGET)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(DEP_DIR)/%.d: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -M $< -MT $(@:$(DEP_DIR)/%.d=$(OBJ_DIR)/%.o) > $@

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(OBJ_DIR) $(DEP_DIR)

clean:
	rm -rf $(OBJ_DIR) $(DEP_DIR) $(TARGET)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Dependency dir:  ${DEP_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"