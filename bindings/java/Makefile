build:
	echo Building Header
	javac -h . Lunaris.java

	echo Building Native Library
	g++ lunaris4java.c -shared -I /usr/lib/jvm/java-23-openjdk/include/ -I /usr/lib/jvm/java-23-openjdk/include/linux/ -o liblunaris4java.so -fPIC -lwayland-client -lxkbcommon -Wno-attributes -Ofast
clean:
	rm *.class
	rm Lunaris.h
	rm *.so
