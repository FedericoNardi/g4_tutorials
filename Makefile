# Build with the LCG_108 Geant4 (source env.sh first).
# Plain Makefile instead of CMake: Geant4Config.cmake in LCG_108 requires
# OpenGL headers (Qt vis), which this headless machine doesn't have.
# So we build with -DNO_VIS (batch mode only) and link only the non-graphics
# Geant4 libraries. CMakeLists.txt is kept for machines with a full Geant4
# install, where vis and the interactive UI work as in the original B4a.

G4LIBDIR := $(shell geant4-config --prefix)/lib64
G4LIBS   := $(filter-out -lG4OpenGL -lG4visQt3D -lG4Tree -lG4FR -lG4GMocren -lG4visHepRep \
              -lG4RayTracer -lG4VRML -lG4ToolsSG -lG4vis_management -lG4modeling -lG3toG4 \
              -lG4gdml -lG4interfaces, $(shell geant4-config --libs))

CXX      ?= g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -DNO_VIS -Iinclude $(shell geant4-config --cflags-without-gui)
LDFLAGS  := $(G4LIBS) -Wl,-rpath,$(G4LIBDIR)

SRCS := exampleB4a.cc $(wildcard src/*.cc)
OBJS := $(patsubst %.cc,build/%.o,$(SRCS))

exampleB4a: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

build/%.o: %.cc $(wildcard include/*.hh)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf build exampleB4a

.PHONY: clean
