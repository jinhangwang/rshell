VPATH = src
CPPFLAGS = -Wall -Werror -ansi -pedantic
objects = $(addprefix obj/, rshell.o main.o)

all: bin/rshell bin/ls

bin/rshell: $(objects) | bin
	    g++ -o $@ $(objects)

bin/ls: obj/ls.o | bin
		g++ -o $@ obj/ls.o

obj/%.o: %.cpp
	    g++ -c -o $@ $<

obj/rshell.o: rshell.h

obj/main.o: rshell.h 

$(objects): | obj

bin:
	    mkdir bin

obj:
	    mkdir obj

clean:
	    rm -rf obj bin
