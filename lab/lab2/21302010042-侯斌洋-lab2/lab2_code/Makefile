CC = gcc 
CFLAGS = -g -O0
INCLUDES = ush.h ush-env.h ush-parse.h ush-prt.h ush-sig.h
spsh:ush.o ush-env.o ush-parse.o ush-prt.o ush-sig.o
	${CC} ush.o ush-env.o ush-parse.o ush-prt.o ush-sig.o -o spsh
all :spsh
clean:
	rm *.o spsh
