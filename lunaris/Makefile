win_cursors:
	x86_64-w64-mingw32-windres -i ./extended_windows_cursors/cursors.rc -o ./thirdparty/hackneyed/cursors.o
xdg_headers:
	mkdir thirdparty/xdg
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml thirdparty/xdg/xdg-shell.h
	wayland-scanner public-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml thirdparty/xdg/xdg-shell.c

	wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.h
	wayland-scanner public-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.c

	wayland-scanner client-header /usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml thirdparty/xdg/cursor-shape-v1.h
	wayland-scanner public-code /usr/share/wayland-protocols/staging/cursor-shape/cursor-shape-v1.xml thirdparty/xdg/cursor-shape-v1.c

	wayland-scanner client-header /usr/share/wayland-protocols/stable/tablet/tablet-v2.xml thirdparty/xdg/tablet-v2.h
	wayland-scanner public-code /usr/share/wayland-protocols/stable/tablet/tablet-v2.xml thirdparty/xdg/tablet-v2.c
