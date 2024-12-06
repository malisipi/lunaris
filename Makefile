compile:
	g++ -Ofast lunaris/c/lunaris.cpp -lwayland-client -lxkbcommon -shared -fPIC -o liblunaris.so -Wno-attributes

compile_unoptimized:
	g++ -O0 lunaris/c/lunaris.cpp -lwayland-client -lxkbcommon -shared -fPIC -o liblunaris.so -Wno-attributes

compile_ui:
	g++ -Ofast lunaris/c/lunaris_ui.cpp -lwayland-client -lxkbcommon -shared -fPIC -o liblunaris_ui.so -Wno-attributes

compile_ui_unoptimized:
	g++ -O0 lunaris/c/lunaris_ui.cpp -lwayland-client -lxkbcommon -shared -fPIC -o liblunaris_ui.so -Wno-attributes