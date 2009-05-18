SRC	 = src

CXX	 = g++
CXXFLAGS = -Wall
LIBS	 = -lpthread

LIBDIR    = /usr/lib
HEADERDIR = /usr/include

ifneq (, $(DESTDIR))
 LIBDIR    = $(DESTDIR)/$(LIBDIR)
 HEADERDIR = $(DESTDIR)/$(HEADERDIR)
endif

debug = no

ifeq ($(debug), yes)
 CXXFLAGS += -g
endif

%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $?

libsocketpp.so: *.o
	$(CXX) -shared $? -o $@ $(LIBS)

all: %.o libsocketpp.so

install:
	mkdir -p $(HEADERDIR)/socket++/
	cp $(SRC)/*.h $(HEADERDIR)/socket++/
	mv $(HEADERDIR)/socket++/socket++.h $(HEADERDIR)/
	cp libsocketpp.so $(LIBDIR)/libsocketpp.so.0.0.0
	cp libsocketpp.so $(LIBDIR)/libsocketpp.a
	ln -sf $(LIBDIR)/libsocketpp.so.0.0.0 $(LIBDIR)/libsocketpp.so.0

uninstall:
	rm -rf $(HEADERDIR)/socket++ $(HEADERDIR)/socket++.h $(LIBDIR)/libsocketpp.so.0.0.0 \
	       $(LIBDIR)/libsocketpp.a $(LIBDIR)/libsocketpp.so.0

clean:
	\rm -f *.o *.so *~
