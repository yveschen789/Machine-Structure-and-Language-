#
# Makefile for the UM Test lab
#
INCLUDES =

# Do all C compies with gcc (at home you could try clang)
CC = gcc -O1

# Comp 40 directory
COMP40 = /comp/40

# Hanson directory
HANSON = /usr/sup/cii40

# Updating include path to use current directory
# (which is .), Comp 40 .h files, and CII interfaces
IFLAGS = -I. -I$(COMP40)/build/include -I$(HANSON)/include/cii


# the next three lines enable you to compile and link against course software
CFLAGS =  -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)
LIBS = $(CIILIBS) -lm
LFLAGS = -L$(COMP40)/build/lib

# Linking flags, used in the linking step
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L$(COMP40)/build/lib -L$(HANSON)/lib64

# Libraries needed for any of the programs that will be linked
# Both programs need cii40 (Hanson binaries) and *may* need -lm (math)
# Only brightness requires the binary for pnmrdr.
LDLIBS =  -lcii40-O1 -lm

EXECS   = um writetests

all: $(EXECS)

um: um.o load.o bitpack.o execute.o operations.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlabwrite.o umlab.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o
