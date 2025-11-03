
CC = gcc
CFLAGS = -Wall
TARGET = dotman
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

install: $(TARGET)
	# bser-level install
	mkdir -p $(HOME)/bin
	cp $(TARGET) $(HOME)/bin/
	chmod +x $(HOME)/bin/$(TARGET)
	@echo "Installed $(TARGET) to ~/bin. Make sure ~/bin is in your PATH."

uninstall:
	# remove user-level installation
	rm -f $(HOME)/bin/dotman
	@echo "Removed dotman from $(HOME)/bin."
	# Optionally remove dotman repo in home
	# rm -rf $(HOME)/.dotman
	# echo "Removed dotman repository in ~/.dotman."

clean:
	rm -f $(TARGET)
