CC = gcc
SRC_DIR = src
INC_DIR = .
OBJ_DIR = obj
PARSER_DIR = graphs-in-C-core
CFLAGS = -std=gnu18 -Wall -Wextra -Wpedantic -Wno-unused-parameter -O3
GTK = `pkg-config --cflags gtk4` -lm
LIBS = `pkg-config --libs gtk4` -lm
NAME = graphs

OBJS = $(addprefix $(OBJ_DIR)/, main.o draw.o eval.o parse.o) #default

linux: pre $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBS)

pre:
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi;

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if [ -d "$(PARSER_DIR)" ]; then \
		$(CC) $(CFLAGS) -c -I$(INC_DIR) -DPARSER_INCLUDED -DLINUX $< -o $@ $(GTK); \
	else \
		$(CC) $(CFLAGS) -c -I$(INC_DIR) -DLINUX $< -o $@ $(GTK); \
	fi;

clean:
	@if [ -d "$(OBJ_DIR)" ]; then rm -r -f $(OBJ_DIR); fi;

distclean: clean
	@rm -f $(NAME) $(NAME).exe