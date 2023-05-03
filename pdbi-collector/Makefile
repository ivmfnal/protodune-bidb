#
# Makefile for c++ examples
#
#

DIPHOME=/home/ivm/dip-5.6.0

CC = g++
CFLAGS = -Wall -ansi -I $(DIPHOME)/include

DLIB = $(DIPHOME)/lib64/libdip.so -lpthread -ldl -llog4cplus

all:	dip-client

dip-client:	dip-client.cpp
	$(CC) $(CFLAGS) dip-client.cpp -L$(DIPHOME)/lib64 $(DLIB) -o $@ $(LIBS) 
	
