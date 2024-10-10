CC = cc
CFLAGS = -Wall

# define the executable name(s)
EXE=allocate

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:.c=.o)

# the first target:
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

format:
	clang-format -style=file -i *.c

clean:
	rm -f $(OBJS) $(EXE)