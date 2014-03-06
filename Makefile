all : dmcache.out sacache.out

dmcache.out : dmcache.cpp
	g++ -Wall -o dmcache.out dmcache.cpp

sacache.out : sacache.cpp
	g++ -Wall -o sacache.out sacache.cpp

