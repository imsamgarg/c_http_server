CC      = gcc
CFLAGS  = -Wall -O2
LDFLAGS =
TARGET  = server

SRC = crypto.c http_request.c main.c parse_http.c read_http.c utils.c ws.c server.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
