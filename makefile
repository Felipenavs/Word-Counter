CC = gcc


words: myprocessor.o myhashmap.o myutility.o words.c
	$(CC) $(CFLAGS) myprocessor.o myhashmap.o myutility.o words.c -o words

myprocessor.o: myprocessor.c myhashmap.h myutility.h
	$(CC) $(CFLAGS) -c myprocessor.c

myhashmap.o: myhashmap.c myhashmap.h myutility.h
	$(CC) $(CFLAGS) -c myhashmap.c

myutility.o: myutility.c myutility.h
	$(CC) $(CFLAGS) -c myutility.c

clean:
	rm -f *.o words