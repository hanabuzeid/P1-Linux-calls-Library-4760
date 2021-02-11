# Define the object files for our program
OBJECTS := bt.o

# make for the executable
bt: $(OBJECTS)
	gcc -g -o $@ $^

%.o: %.c
	gcc -g -c -O3 $<

# Clean the .o files
clean:
	rm -f $(OBJECTS)
