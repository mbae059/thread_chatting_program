all: csechatread csechatwrite 

test: test.cpp
	g++ -std=c++20 -o test test.cpp -lncurses

csechatread : csechatread.o chatshm.o
	g++ csechatread.o chatshm.o -o csechatread -lncurses

csechatread.o : csechatread.cpp chatshm.h
	g++ -c csechatread.cpp

csechatwrite : csechatwrite.o chatshm.o
	g++ csechatwrite.o chatshm.o -o csechatwrite -lncurses

csechatwrite.o : csechatwrite.cpp chatshm.h
	g++ -c csechatwrite.cpp

cseshm.o : cseshm.cpp cseshm.h
	g++ -c cseshm.cpp

clean:
	rm -f *.o csechatread
	rm -f *.o csechatwrite
