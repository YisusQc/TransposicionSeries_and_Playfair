CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system
BUILD_DIR := build

COMMON := src/procesamiento.cpp src/interfaz.cpp

.PHONY: all go_t go_p clean

all: go_t go_p

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

go_t: $(COMMON) src/permutacion_series.cpp src/go_transp.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/go_t $(SFML_LIBS)

go_p: $(COMMON) src/playfair.cpp src/go_playfair.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/go_p $(SFML_LIBS)

clean:
	rm -rf $(BUILD_DIR)

