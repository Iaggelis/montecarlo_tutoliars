EX              := converter
EXE             := $(addsuffix .exe,$(EX))
ROOTCXXFLAGS    := $(shell root-config --cflags --glibs)


$(EX): $(EX).cpp
	$(CXX) -Ofast -g $@.cpp -o $@.exe $(ROOTCXXFLAGS)