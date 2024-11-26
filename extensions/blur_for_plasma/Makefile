plasma_protocol_headers:
	mkdir thirdparty thirdparty/plasma_protocols
	wayland-scanner client-header /usr/share/plasma-wayland-protocols/blur.xml thirdparty/plasma_protocols/blur.h
	wayland-scanner public-code /usr/share/plasma-wayland-protocols/blur.xml thirdparty/plasma_protocols/blur.c
compile_lgpl3:
	echo The procuded library is licensed under LGPL 3 or later
	gcc lgpl3/blur_for_plasma.cpp -I. -Impl2 -shared -fPIC -lwayland-client -o libblur_for_plasma.so
compile_mpl2:
	echo The produced library is licensed under MPL2
	echo The produced library is actually is just a placeholder to preventing crash due to unable to find the dynamic library.
	gcc mpl2/placeholder_blur_for_plasma.cpp -Impl2 -shared -fPIC -o libblur_for_plasma.so
