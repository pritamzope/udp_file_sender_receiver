GCC=gcc
BUILD=udp_file_sendreceive
OBJFILES=crc.o main.o receiver.o sender.o

${BUILD} : ${OBJFILES}
	${GCC} ${OBJFILES} -o $@ 
	
crc.o : crc.c
	${GCC} -c crc.c -o $@

receiver.o : receiver.c
	${GCC} -c receiver.c -o $@

sender.o : sender.c
	${GCC} -c sender.c -o $@

main.o : main.c
	${GCC} -c main.c -o $@

clean:
	rm *.o
	
