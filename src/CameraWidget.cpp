#include "CameraWidget.h"

#include <QProcess>
#include <QDir>

CameraWidget::CameraWidget(QWidget *parent):QWidget(parent) {
	// Let's set up the camera first to make sure modules
	// are loaded and all
	QProcess::execute(QStringLiteral("/usr/bin/camera-setup"), QStringList() << "front");

	QDir d("/sys/class/video4linux");
	QStringList devices=d.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString const &f : devices) {
		qInfo("Found potential video device: %s", qPrintable(f));
		QString const fname(QStringLiteral("/sys/class/video4linux/") + f + "/name");
		if(QFile::exists(fname)) {
			QFile n(fname);
			if(n.open(QFile::ReadOnly|QFile::Text)) {
				QString name=QString(n.readAll()).trimmed();
				qInfo("%s:%s", qPrintable(f), qPrintable(name));
				if(name == QStringLiteral("sun6i-csi")) {
					_videodev = QStringLiteral("/dev/") + f;
					qInfo("Correct video device is %s", qPrintable(_videodev));
					break;
				}
			}
		}
	}
	if(_videodev.isEmpty())
		qFatal("Couldn't find a usable video device");

	_mpv=new QProcess(this);
	_mpv->setProgram(QStringLiteral("/usr/bin/mpv"));
	_mpv->setArguments(QStringList() << _videodev);
	_mpv->start();
}
