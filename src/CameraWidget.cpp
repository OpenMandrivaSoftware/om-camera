#include "CameraWidget.h"

#include <QProcess>
#include <QDir>
#include <QDateTime>
#include <QFile>

CameraWidget::CameraWidget(QWidget *parent):QWidget(parent),_layout(this),_usingFrontCamera(false) {
	_video=new MpvWidget(parent);
	_layout.addWidget(_video, 0, 0, 1, 2);
	_switch=new QPushButton(tr("Switch"), this);
	_layout.addWidget(_switch, 1, 0);
	connect(_switch, &QPushButton::clicked, this, &CameraWidget::switchCamera);
	_photo=new QPushButton(tr("Photo"), this);
	connect(_photo, &QPushButton::clicked, this, &CameraWidget::takePhoto);
	_layout.addWidget(_photo, 1, 1);

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
#if 1
		_videodev="/home/bero/Videos/Russia.mkv"; // fake it while testing on desktop...
#else
		qFatal("Couldn't find a usable video device");
#endif

	// Due to what appears to be a bug in libmpv, we can't just
	// start playing the video here -- that opens a separate
	// window.
	// But we can start() a bit later, after the window is shown.
	// but (again for reasons not yet debugged), not in an overloaded
	// showEvent().
}

void CameraWidget::start() {
	// Let's set up the camera first to make sure modules
	// are loaded and all
	QProcess::execute(QStringLiteral("/usr/bin/camera-setup"), QStringList() << (_usingFrontCamera ? "front" : "rear"));

	_video->command(QStringList() << "loadfile" << _videodev);
}

void CameraWidget::switchCamera() {
	// playlist-clear followed by playlist-remove seems to be the only
	// way to get "stop"
	_video->command(QStringList() << "playlist-clear");
	_video->command(QStringList() << "playlist-remove" << "current");
	_usingFrontCamera = !_usingFrontCamera;
	start();
}

void CameraWidget::takePhoto() {
	_video->command(QStringList() << "screenshot-to-file" << QDir::homePath() + "/Pictures/Photo-" + QDateTime::currentDateTime().toString(Qt::ISODateWithMs) + ".jpg");
}
