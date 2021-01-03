CXX = gcc
CXXFLAGS = -xc -std=c11 -Wall -Wextra -lm -Wno-unused-parameter
OBJ = main.o eval.o
SOURCE = main.c eval.c
GTK = -IC:/msys64/mingw64/include/gtk-3.0 -IC:/msys64/mingw64/include/pango-1.0 -IC:/msys64/mingw64/include -IC:/msys64/mingw64/include/glib-2.0 -IC:/msys64/mingw64/lib/glib-2.0/include -IC:/msys64/mingw64/include/harfbuzz -IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/libpng16 -mms-bitfields -IC:/msys64/mingw64/include/fribidi -IC:/msys64/mingw64/include/cairo -IC:/msys64/mingw64/include/lzo -IC:/msys64/mingw64/include/pixman-1 -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -IC:/msys64/mingw64/include/gdk-pixbuf-2.0 -mms-bitfields -mms-bitfields -mms-bitfields -IC:/msys64/mingw64/include/atk-1.0 -mms-bitfields -mms-bitfields -mms-bitfields -pthread -mms-bitfields -LC:/msys64/mingw64/lib -lgtk-3 -lgdk-3 -lz -lgdi32 -limm32 -lshell32 -lole32 -Wl,-luuid -lwinmm -ldwmapi -lsetupapi -lcfgmgr32 -lpangowin32-1.0 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl 

#PCH_SRC = source/pch.hpp
#PCH_HEADERS = 
#PCH_OUT = source/pch.hpp.gch
all: clean
	@$(CXX) $(CXXFLAGS) $(SOURCE) -o wykresy $(GTK)
clean:
	@rm -f $(OBJ) 
	@rm -f wykresy
#pre:
	#@$(CXX) $(CXXFLAGS) $(PCH_SRC);
	#@make
	
	

