#pragma once

#include "MpvWidget.h"
#include <QGridLayout>
#include <QPushButton>

class CameraWidget : public QWidget {
	Q_OBJECT
public:
	CameraWidget(QWidget *parent=nullptr);
public Q_SLOTS:
	void start();
	void switchCamera();
	void takePhoto();
protected:
	QGridLayout	_layout;
	MpvWidget *	_video;
	QPushButton *	_switch;
	QPushButton *	_photo;

	QString		_videodev;
	bool		_usingFrontCamera;
};
