win_cursors:
	x86_64-w64-mingw32-windres -i ./extended_windows_cursors/cursors.rc -o ./thirdparty/hackneyed/cursors.o
xdg_headers:
	rm -rdf thirdparty/xdg
	mkdir -p thirdparty/xdg
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml thirdparty/xdg/xdg-shell.h
	wayland-scanner public-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml thirdparty/xdg/xdg-shell.c

	wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.h
	wayland-scanner public-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.c

	wayland-scanner client-header /usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml thirdparty/xdg/cursor-shape-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml thirdparty/xdg/cursor-shape-v1.c

	wayland-scanner client-header /usr/share/wayland-protocols/stable/tablet/tablet-v2.xml thirdparty/xdg/tablet-v2.h
	wayland-scanner public-code /usr/share/wayland-protocols/stable/tablet/tablet-v2.xml thirdparty/xdg/tablet-v2.c
xdg_headers_from_git_source:
	cd thirdparty ;\
	git clone https://gitlab.freedesktop.org/wayland/wayland-protocols ;\
	git pull ;\
	cd wayland-protocols ;\
	git reset --hard c7b582cb71ec145026a547dcd8abb9dfeba40541
	rm -rdf thirdparty/xdg
	mkdir -p thirdparty/xdg
	wayland-scanner client-header thirdparty/wayland-protocols/stable/xdg-shell/xdg-shell.xml thirdparty/xdg/xdg-shell.h
	wayland-scanner public-code thirdparty/wayland-protocols/stable/xdg-shell/xdg-shell.xml thirdparty/xdg/xdg-shell.c

	wayland-scanner client-header thirdparty/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.h
	wayland-scanner public-code thirdparty/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.c

	wayland-scanner client-header thirdparty/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml thirdparty/xdg/cursor-shape-v1.h
	wayland-scanner public-code thirdparty/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml thirdparty/xdg/cursor-shape-v1.c

	wayland-scanner client-header thirdparty/wayland-protocols/stable/tablet/tablet-v2.xml thirdparty/xdg/tablet-v2.h
	wayland-scanner public-code thirdparty/wayland-protocols/stable/tablet/tablet-v2.xml thirdparty/xdg/tablet-v2.c
