compile:
	mkdir -p lunaris
	v translate wrapper ../../lunaris/c/lunaris.h
	rm ../../lunaris/c/lunaris.json
	mv ../../lunaris/c/lunaris.v lunaris/lunaris.v
	cp ../../lunaris/c/lunaris.h lunaris/lunaris.h
	cd ../.. && make compile &&  cd -
	cp ../../liblunaris.so lunaris/liblunaris.so
	sed -i 's/pub fn lunaris_/pub fn /g' lunaris/lunaris.v
	sed -i 's/type/\/\/ type /g' lunaris/lunaris.v
	sed -i 's/module /module lunaris\n\n#flag -I. -L\@VMODROOT -llunaris -Wl,-rpath=.\n#include "\@VMODROOT\/lunaris.h"\n\/\//g' lunaris/lunaris.v
	sed -i 's/\&Lunaris_window/voidptr/g' lunaris/lunaris.v
	sed -i 's/\&Lunaris_layer/voidptr/g' lunaris/lunaris.v
	echo "" > lunaris/v.mod
install_as_module:
	rm -rdf ~/.vmodules/lunaris
	cp -r lunaris ~/.vmodules/lunaris
ready: compile install_as_module clean
clean:
	rm -rd lunaris
