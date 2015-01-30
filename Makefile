VPATH = src
#CPPFLAGS = -Wall -Werror -ansi -pedantic
objects = $(addprefix obj/, rshell.o main.o)

bin/rshell: $(objects) | bin
	    g++ -o $@ $(objects)

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
