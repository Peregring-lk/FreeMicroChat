CC = g++
CFLAGS = -std=c++0x

LIBS = -lboost_system -lboost_thread -lboost_serialization
EXE = FreeMicroChat

OBJS = FreeMicroChat.o Server.o Client.o Message.o ContactList.o ConnectionList.o

$(info :: Si se produce error de enlazado, pruebe con:)
$(info ::   ** sudo apt-get install libboost-thread1.46-dev libbost-system1.46-dev libasio-dev)
$(info :: Si su gestor de paquetes no tiene esta versión de las librerías, intente quizás con versiones más antiguas o modernas.)

all : $(EXE)
	@echo > /dev/null
	@echo Compilación terminada.

$(EXE) : $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

%.cpp : %.hpp Headers.hpp

FreeMicroChat.cpp : Server.cpp Client.cpp
Server.cpp : Message.cpp ContactList.cpp ConnectionList.cpp
Client.cpp : Message.cpp ContactList.cpp ConnectionList.cpp
ConnectionList.cpp : Message.cpp

clean:
	rm -f *.o *~ $(EXE)