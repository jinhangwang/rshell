objects = rshell.o 
program: $(objects)
	g++ -o program $(objects)


rshell.o: rshell.cpp type_prompt.h
	g++ -c rshell.cpp
