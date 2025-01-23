# Lunaris

## Supported Targets

* Windows 10 and above<sup>1</sup>
* Linux/Wayland<sup>2</sup>
* Linux/Framebuffer
* Web/Emscripten
* HaikuOS

1. The library can be compiled for Windows 2000 and above. However it's not supported. Some parts of the library may not support legacy Windows versions. And in future, it may not compatible with Windows 2000 due to lack of modern APIs.
2. Legacy versions of Wayland may not be supported.

## License

> This project is licensed under [Mozilla Public License 2.0](./LICENSE).

- XDG protocols is licensed under MIT License. (Placed at `./thirdparty/xdg/`)
- [Hackneyed X11 Cursors](https://gitlab.com/Enthymeme/hackneyed-x11-cursors) is licensed under [X11 License Distribution Modification Variant](./lunaris/thirdparty/hackneyed/LICENSE). (Placed at `./thirdparty/hackneyed/`)
- [STB Libraries](https://github.com/nothings/stb) is licensed under [Public Domain](./lunaris/thirdparty/stb/LICENSE.txt). (Placed at `./thirdparty/stb/`)

- The `./extensions` folder contains files licensed under various licenses. For licensing details, refer to the `README.md` file located in each extension's directory (e.g., `./extensions/blur_for_plasma/README.md`).

- Video widget is dependent to [libmpv](https://github.com/mpv-player/mpv/). See [GitHub page of libmpv](https://github.com/mpv-player/mpv/) to see License. This repository and project have not included any libmpv file.
