all:
	cd src && make

clean:
	cd src && make clean

clobber:
	cd src && make clobber
	rm -f lib/*
