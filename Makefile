all: exampleRR exampleRR2 exampleRetro exampleFCFS schedulerRR.o schedulerRR2.o schedulerRetro.o schedulerFCFS.o libthreads.o

exampleRR: example.c libthreads.o schedulerRR.o
	gcc -o exampleRR example.c libthreads.o schedulerRR.o

exampleRR2: example.c libthreads.o schedulerRR2.o
	gcc -o exampleRR2 example.c libthreads.o schedulerRR2.o
	
exampleRetro: example.c libthreads.o schedulerRetro.o
	gcc -o exampleRetro example.c libthreads.o schedulerRetro.o

exampleFCFS: example.c libthreads.o schedulerFCFS.o
	gcc -o exampleFCFS example.c libthreads.o schedulerFCFS.o
	
schedulerRR.o: schedulerRR.c libthreads.h commondata.h
	gcc -c schedulerRR.c

schedulerRR2.o: schedulerRR2.c libthreads.h commondata.h
	gcc -c schedulerRR2.c
	
schedulerRetro.o: schedulerRetro.c libthreads.h commondata.h
	gcc -c schedulerRetro.c

schedulerFCFS.o: schedulerFCFS.c libthreads.h commondata.h
	gcc -c schedulerFCFS.c

libthreads.o: libthreads.c libthreads.h commondata.h
	gcc -c libthreads.c

clean:
	rm exampleRR exampleRR2 exampleFCFS exampleRetro
	rm *.o