# comments are always a great idea.

# we use this variable as $(CC) when building commands.
CC=gcc

# we keep using these flags in combination, so they're
# stuffed into a variable. Saves us from having to 
# re-type them repeatedly, and of course we can then
# change it in one place for all uses.
CFLAGS=-O0 -std=c99

FUNC_OS=func1.o func2.o func3.o func4.o func5.o

# see below for the rec4 target.
all: rec4

# the command 'make clean' should remove all generated files.
clean:
	-rm -rf *~ *# *.o rec4.s rec4.o rec4

# target rec4 builds the overall executable, rec4.
# depends on all the individual functions' object files.
rec4: rec4.o  $(FUNC_OS)
	$(CC) $(CFLAGS) -o rec4 rec4.o $(FUNC_OS)

# compiles rec4.c into rec4.s; needs no other files.
rec4.s: rec4.c
	$(CC) $(CFLAGS) -S rec4.c

# compiles rec4.s into rec4.o; needs no other files.
rec4.o: rec4.s
	$(CC) $(CFLAGS) -c rec4.s

# each of the remaining targets also compiles each
# .s file into the corresponding .o file, with no
# other dependencies on other files.

func1.o: func1.s
	$(CC) $(CFLAGS) -c func1.s

func2.o: func2.s
	$(CC) $(CFLAGS) -c func2.s

func3.o: func3.s
	$(CC) $(CFLAGS) -c func3.s

func4.o: func4.s
	$(CC) $(CFLAGS) -c func4.s

func5.o: func5.s
	$(CC) $(CFLAGS) -c func5.s



