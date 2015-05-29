all: server client 
	
server:
	arm-linux-gnueabi-gcc LinkedList.c badbuf.c -o badbuf
	
client:
	gcc client2.c -w -o client -lpthread

clean:
	rm -f badbuf client
