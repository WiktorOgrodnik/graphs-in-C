CXX = gcc
CXXFLAGS = -std=c11 -lm
OBJ = main.o eval.o
SOURCE = main.c eval.c
#SFML = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

#PCH_SRC = source/pch.hpp
#PCH_HEADERS = 
#PCH_OUT = source/pch.hpp.gch

all: clean
	@$(CXX) $(CXXFLAGS) -o wykresy $(SOURCE)
clean:
	@rm -f $(OBJ) 
	@rm -f wykresy
#pre:
	#@$(CXX) $(CXXFLAGS) $(PCH_SRC);
	#@make
	
	

