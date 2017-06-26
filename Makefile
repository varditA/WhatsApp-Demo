CC = g++
CFLAGS = -g -Wall -std=c++11

make: whatsappClient whatsappServer

whatsappClient: whatsappServer.o
	$ (CC) $ (FLAGS) -o whatsappClient

whatsappClient.o: whatsappClient.cpp whatsappClient.h
	$ (CC) $ (FLAGS) -c whatsappClient.cpp

whatsappServer: whatsappServer.o
	$ (CC) $ (FLAGS) -o whatsappServer

whatsappServer.o: whatsappServer.cpp whatsappServer.h Group.h Group.cpp ClientInfo.cpp ClientInfo.h
	$ (CC) $ (FLAGS) -c whatsappServer.cpp

tar:
	tar cvf ex5.tar Makefile README *.cpp *.h

clean:
	rm -f *.o whatsappClient whatsappServer
