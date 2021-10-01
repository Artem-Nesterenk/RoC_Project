server.o: server.c 
	gcc -c -o server.o server.c  
camera.o: camera.c
	gcc -c -o  camera.o camera.c  
main.o: main.c server.c 
	gcc -c -o main.o main.c 
json.o: json.c
	gcc -c -o json.o json.c 
json-parser.o: json-parser.c
	gcc -c -o json-parser.o json-parser.c
main: server.o camera.o main.o json.o json-parser.o
	gcc -o main server.o camera.o main.o json.o json-parser.o -lcurl -lpthread -ljson -lrt -lm
clean: 
	rm -rf main server.o camera.o json.o main.o
