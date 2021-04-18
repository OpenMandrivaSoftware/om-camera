#include "Camera.h"

#include <iostream>

int main(int argc, char **argv) {
	setlocale(LC_NUMERIC, "C"); // Needed by libmpv for now
	setenv("QT_QPA_PLATFORM", "xcb", 1); // mpv works better on X11 than on Wayland
	Camera app(argc, argv);
	app.exec();
}
