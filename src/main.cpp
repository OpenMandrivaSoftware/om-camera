#include "Camera.h"

#include <iostream>

int main(int argc, char **argv) {
	setlocale(LC_NUMERIC, "C"); // Needed by libmpv for now
	Camera app(argc, argv);
	app.exec();
}
