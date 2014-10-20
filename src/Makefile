# Makefile

# Build the AGL library
agl: net.cpp net.h
	g++ -fPIC -c net.cpp
	g++ -shared -o ../lib/libagl-net.so net.o -lSDL -lSDL_net -lagl

# Build the test application
test: test.cpp
	g++ -g -o test test.cpp -lagl -lagl-net

# Install the AGL library on the system
install:
	../install.sh

# Uninstall the AGL library to the system
remove:
	../remove.sh

# Generates the AGL documentation
doc:
	doxygen doxyfile

# Cleans the directories, removing executables and objects
clean:
	rm *.o test ../lib/*.so
