#pragma once

#include <QApplication>
#include "CameraWidget.h"

class Camera:public QApplication {
	Q_OBJECT
public:
	Camera(int &argc, char **&argv);
protected:
	CameraWidget *	_ui;
};
