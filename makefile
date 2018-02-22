all : server client
.PHONY : clean

server : server.o
	g++ server.cpp fun.cpp -o server -lpthread

client : client.o
	g++ client.cpp fun.cpp -o client -lpthread

clean : 
	rm *.o