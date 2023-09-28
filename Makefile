Project4: main.o sampler.o
	g++ -o Project4 main.o sampler.o

main.o: main.cpp bstDictionary.h sampler.h
	g++ -c main.cpp

sampler.o: sampler.cpp sampler.h
	g++ -c sampler.cpp