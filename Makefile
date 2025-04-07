CC=g++
CFLAGS=-Wall -Werror -Wno-error=unused-variable -g

client: client.cpp helpers.cpp buffer.cpp requests.cpp
	$(CC) -o client client.cpp helpers.cpp buffer.cpp requests.cpp $(CFLAGS)

clean:
	rm -rf main
run: client
	./client
zip:
	zip 323CC_Iacob_Ioana-Delia_Tema4PC Makefile Readme.md *.cpp *.h *.hpp