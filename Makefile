CC = gcc
SRC_DIR = ./src
INC_DIR = .
CFLAGS = -std=gnu18 -Wall -Wextra -Wpedantic -Wno-unused-parameter
GTK = `pkg-config --cflags gtk4` -lm
LIBS = `pkg-config --libs gtk4` -lm
NAME = wykresy

OBJS = main.o parse.o eval.o draw.o types.o

all: $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBS)

%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c -I$(INC_DIR) $< -o $@ $(GTK)

clean:
	@rm -f $(NAME) $(OBJS)