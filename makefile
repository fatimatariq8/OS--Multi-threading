compile:
	gcc file_processor_singlethreaded.c -Wall -o single
	gcc file_processor_multithreaded.c -Wall -o multi

build:
	gcc file_processor_singlethreaded.c -Wall -o single
	gcc file_processor_multithreaded.c -Wall -o multi


clean:
	rm -f single
	rm -f multi




