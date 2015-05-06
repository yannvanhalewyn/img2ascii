# Compiler
CC = g++

# Flags
CFLAGS = -std=c++11
LDLFLAGS = -L/opt/X11/lib -lX11 -I/opt/X11/include -lboost_program_options -lcurlpp -lcurl

# Dirs and files
BINDIR = bin/
EXEC = img2ascii
EXEC := $(addprefix $(BINDIR), $(EXEC))

$(EXEC): main.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDLFLAGS)

run: $(EXEC)
	@./$(EXEC) -f test_imgs/mona_lisa.jpg

