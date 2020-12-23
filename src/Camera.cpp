#include "Camera.h"

Camera::Camera(int &argc, char **&argv):QApplication(argc,argv) {
	setApplicationDisplayName("Camera");
	setOrganizationName("LinDev");
	setOrganizationDomain("lindev.ch");
	setQuitOnLastWindowClosed(true);

	_ui = new CameraWidget;
	_ui->resize(300, 400);
	_ui->show();
}
