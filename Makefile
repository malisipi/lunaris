compile:
	g++ -Ofast lunaris/c/lunaris.cpp -lwayland-client -lxkbcommon -shared -fPIC -o liblunaris.so -Wno-attributes

compile_unoptimized:
	g++ lunaris/c/lunaris.cpp -lwayland-client -lxkbcommon -shared -fPIC -o liblunaris.so -Wno-attributes