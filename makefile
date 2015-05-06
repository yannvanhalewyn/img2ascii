# Compiler
CC = g++

# Flags
CFLAGS = -std=c++11
LDLFLAGS = -L/opt/X11/lib -lX11 -I/opt/X11/include

# Dirs and files
BINDIR = bin/
EXEC = jpg2ascii
EXEC := $(addprefix $(BINDIR), $(EXEC))

run: $(EXEC)
	@./$(EXEC)

$(EXEC): main.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDLFLAGS)
