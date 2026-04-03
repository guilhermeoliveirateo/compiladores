# Guilherme Teodoro de Oliveira - 10425362
# Vinícius Brait Lorimier - 10420046
# Compilação: make
# Limpeza:    make clean

CC     = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = salc

SRCS = main.c opt.c diag.c log.c lex.c symtab.c parser.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
