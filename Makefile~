all: HandCounter_bgRemoval HandCounter VoteMode DialMode

FLAGS = -g
X11FLAG = -I/usr/include -L/usr/lib/x86_64-linux-gnu/ -lX11
CPPVERSION = --std=c++11
CPPFLAGS = $(shell pkg-config --cflags opencv)
LDLIBS = $(shell pkg-config --libs opencv)


clean:
	rm -f HandCounter
	rm -f HandCounter_bgRemoval
	rm -f VoteMode
	rm -f DialMode
	rm -f *.o

InteractiveObject.o: InteractiveObject.cpp
	g++ $< -o $@ -c $(CPPVERSION)

Util.o: Util.cpp
	g++ $< -o $@ -c $(X11FLAG)

HandCounter_bgRemoval: HandCounter_bgRemoval.cpp Util.o
	g++ $< -o $@  Util.o $(LDLIBS) $(X11FLAG) $(CPPVERSION) $(FLAGS)

HandCounter: HandCounter.cpp Util.o
	g++ $< -o $@  Util.o $(LDLIBS) $(X11FLAG) $(CPPVERSION) $(FLAGS)

VoteMode: VoteMode.cpp Util.o InteractiveObject.o
	g++ $< -o $@  Util.o InteractiveObject.o $(LDLIBS) $(X11FLAG) $(CPPVERSION) $(FLAGS)

DialMode: DialMode.cpp Util.o InteractiveObject.o
	g++ $< -o $@ Util.o InteractiveObject.o $(LDLIBS) $(X11FLAG)  $(CPPVERSION) $(FLAGS)
