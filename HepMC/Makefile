EX             		 := hepmc_conv
EXE            		 := $(addsuffix .exe,$(EX))
DICTCXXFLAGS 		 :=-I$(HEPMCPATH)/include 
ROOTCXXFLAGS 		 := $(DICTCXXFLAGS) $(shell root-config --cflags --ldflags --glibs)
# FASTJETFLAGS 	 	 := $(shell $(FASTJETROOT)/fastjet-config --cxxflags --libs --plugins)



#$(EX): $(EX).cpp
#	 $(CXX) -std=c++11 -Ofast -g $(ROOTCXXFLAGS) -lEG \
#	 $@.cpp  -o $@.exe -L$(HEPMCPATH)/lib -lHepMC $(FASTJETFLAGS) #$(ROOTCXXFLAGS) -lEG



hepmc_conv: hepmc_conv.cpp
	 $(CXX) -std=c++11 -Ofast -g $(ROOTCXXFLAGS) -lEG \
	 $@.cpp  -o $@.exe -L$(HEPMCPATH)/lib -lHepMC $(ROOTCXXFLAGS) -lEG

