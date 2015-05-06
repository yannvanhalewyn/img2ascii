CC = g++
LDLFLAGS = -std=c++11
EXEC = jpg2ascii

run: $(EXEC)
	@./$(EXEC)

$(EXEC): main.cpp
	$(CC) $(LDLFLAGS) $< -o $@
