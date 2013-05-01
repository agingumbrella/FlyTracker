all: runodd

clean: 
	rm -f runoddd

runodd: runodd.cpp
	g++ runodd.cpp -lzmq -laiousbcpp -lusb-1.0 -o runodd
