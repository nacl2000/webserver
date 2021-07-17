CC = g++
CPPFLAG = -lmysqlclient -pthread -lhiredis -g -std=c++11 -Wall


server: redis.o mysql.o http_conn.o main.cpp *.h
	$(CC) $(CPPFLAG) redis.o mysql.o http_conn.o main.cpp -o server && make clean

http_conn.o: redis.o mysql.o http_conn.cpp
	$(CC) $(CPPFLAG) -c redis.o mysql.o http_conn.cpp

mysql.o: mysql.cpp
	$(CC) $(CPPFLAG) -c mysql.cpp

redis.o: redis.cpp
	$(CC) $(CPPFLAG) -c redis.cpp

clean : 
	rm -f *.o
