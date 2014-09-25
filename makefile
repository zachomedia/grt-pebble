all: clean build install
	
build: clean
	pebble build
	
install: build
	pebble install --phone 10.0.1.101 --logs
	
clean:
	pebble clean
	