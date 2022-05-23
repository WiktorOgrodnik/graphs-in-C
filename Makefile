CC = gcc
SRC_DIR = src
INC_DIR = .
OBJ_DIR = obj
CFLAGS = -std=gnu18 -Wall -Wextra -Wpedantic -Wno-unused-parameter -O3
GTK = `pkg-config --cflags gtk4` -lm
LIBS = `pkg-config --libs gtk4` -lm
NAME = wykresy

OBJS = $(addprefix $(OBJ_DIR)/, main.o parse.o eval.o draw.o)

linux: pre $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -DLINUX -o $(NAME) $(LIBS)

windows: pre $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -DWINDOWS -o $(NAME) $(LIBS)

pre:
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi;

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c -I$(INC_DIR) $< -o $@ $(GTK)

clean:
	@rm -f $(OBJS)
	@if [ -d "$(OBJ_DIR)" ]; then rmdir $(OBJ_DIR); fi;

distclean: clean
	@rm -f $(NAME)