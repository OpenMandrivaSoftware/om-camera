#pragma once

#include <QWidget>
#include <QProcess>

class CameraWidget:public QWidget {
	Q_OBJECT
public:
	CameraWidget(QWidget *parent=nullptr);
protected:
	QString		_videodev;
	QProcess*	_mpv;
};
