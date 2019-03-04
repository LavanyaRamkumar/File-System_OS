username = $(shell whoami)
SRCDIR = src
files = $(SRCDIR)/imp.c
opflag = -o ex
flags = `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=25 -lm -g 

all: run

run: compile
	./ex -f -d /home/$(username)/Desktop/mymount

debugrun: dcompile
	valgrind --track-origins=yes ./ex -d -f -s /home/$(username)/Desktop/mymount

compile: checkdir
	gcc -Wall -g $(files) $(opflag) $(flags)

dcompile: checkdir	
	gcc -Wall -g $(files) $(opflag) $(flags)

checkdir:
	if [ -d "/home/$(username)/Desktop/mymount" ]; then echo "mountpoint exists"; else mkdir /home/$(username)/Desktop/mymount; fi
