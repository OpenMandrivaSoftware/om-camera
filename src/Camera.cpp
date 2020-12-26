#include "Camera.h"
#include <QProcess>
#include <QDir>
#include <QFile>

Camera::Camera(int &argc, char **&argv):QApplication(argc,argv) {
	setApplicationDisplayName("Camera");
	setOrganizationName("LinDev");
	setOrganizationDomain("lindev.ch");
	setQuitOnLastWindowClosed(true);

	// libmpv needs LC_NUMERIC set to C, and it has to be
	// done after Qt is done initializing QApplication
	// (which sets LC_NUMERIC to the locale Qt thinks we
	// should be using)
	setlocale(LC_NUMERIC, "C");

	_ui = new CameraWidget(nullptr);
	_ui->resize(300, 400);
	_ui->show();

	_ui->start();
}
