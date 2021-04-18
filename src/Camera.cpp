#include "Camera.h"
#include <QProcess>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTimer>

Camera::Camera(int &argc, char **&argv):QApplication(argc,argv),_mpv(nullptr),_vRecorder(nullptr),_aRecorder(nullptr),_reencoder(nullptr),_processing(nullptr) {
	setApplicationDisplayName("Camera");
	setOrganizationName("LinDev");
	setOrganizationDomain("lindev.ch");

	_bg=new BackgroundWidget(nullptr);
	connect(_bg, &BackgroundWidget::orientationChanged, this, &Camera::orientationChanged);

	_photo = new TopLevelButton("🖼" /*"Photo"*/);
	connect(_photo, &TopLevelButton::clicked, this, &Camera::photo);
	_photo->show();

	_record = new TopLevelButton("◉");
	connect(_record, &TopLevelButton::clicked, this, &Camera::record);
	_record->show();

	_switch = new TopLevelButton("⤾");
	connect(_switch, &TopLevelButton::clicked, this, &Camera::switchCamera);
	_switch->show();

	_quit = new TopLevelButton("X");
	connect(_quit, &TopLevelButton::clicked, this, &Camera::quitRequested);
	_quit->show();

	setQuitOnLastWindowClosed(true);

	_bg->showFullScreen();

	setCamera(false);
}

Camera::~Camera() {
	stop();
	delete _photo;
	delete _record;
	delete _switch;
	delete _quit;
	delete _bg;
	if(_processing)
		delete _processing;
}

void Camera::stop() {
	if(_mpv) {
		_mpv->terminate();
		_mpv->waitForFinished(1000);
		if(_mpv->state() == QProcess::Running)
			_mpv->kill();
		_mpv->waitForFinished(1000);
		delete _mpv;
		_mpv = nullptr;
	}
}

void Camera::orientationChanged() {
	setCamera(_usingFrontCamera);

	_photo->setGeometry(_bg->width()*.1, _bg->height()*.8, _photo->minimumSizeHint().width(), _photo->minimumSizeHint().height());
	_record->setGeometry(_bg->width()*.3, _bg->height()*.8, _record->minimumSizeHint().width(), _record->minimumSizeHint().height());
	_switch->setGeometry(_bg->width()*.5, _bg->height()*.8, _switch->minimumSizeHint().width(), _switch->minimumSizeHint().height());
	_quit->setGeometry(_bg->width()*.7, _bg->height()*.8, _quit->minimumSizeHint().width(), _quit->minimumSizeHint().height());
}

void Camera::setCamera(bool front) {
	_usingFrontCamera=front;

	stop();

	QProcess::execute(QStringLiteral("/usr/bin/camera-setup"), QStringList() << (_usingFrontCamera ? "front" : "rear"));

	_mpv = new QProcess(this);
	_mpv->setProgram("/usr/bin/mpv");
	_mpv->setArguments(QStringList() << "--vo=xv" << "-fs" << "--no-ontop" << "--no-border" << "--no-window-dragging" << "--no-osc" << "--no-osd-bar" <<  "--x11-netwm=no" << detectCamera());
	_mpv->start();
}

void Camera::switchCamera() {
	_switch->setDown(false);
	setCamera(!_usingFrontCamera);
}

void Camera::photo() {
	_photo->setDown(false);
	stop();
	/* The PinePhone camera takes a bit to start up -- the first 3 captured
	 * frames are always just an overlay background.
	 * To be on the safe side, we also skip frame 4.
	 * In a better world, we'd just use -frames:v 1 and capture directly
	 * to the destination.
	 */
	QProcess *ffmpeg = new QProcess(this);
	ffmpeg->setProgram("/usr/bin/ffmpeg");
	ffmpeg->setArguments(QStringList() << "-s" << "1280x720" << "-f" << "video4linux2" << "-i" << detectCamera() << "-frames:v" << "5" << "/tmp/photo%03d.jpg");
	ffmpeg->start();
	ffmpeg->waitForFinished(5000);
	delete ffmpeg;
	QFile::copy("/tmp/photo005.jpg", "/home/omv/Pictures/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.z") + ".jpg");
	for(int i=0; i<=5; ++i)
		QFile::remove("/tmp/photo00" + QString::number(i) + ".jpg");
	setCamera(_usingFrontCamera);
}

void Camera::record() {
	_record->setDown(false);
	_record->setText("⏹");
	disconnect(_record, &TopLevelButton::clicked, this, &Camera::record);
	connect(_record, &TopLevelButton::clicked, this, &Camera::stopRecording);

	// Don't kill mpv while we're recording
	disconnect(_bg, &BackgroundWidget::orientationChanged, this, &Camera::orientationChanged);

	// Unfortunately the v4l device can't be shared - so stop
	// playback for now and play the recorded stream when we
	// have enough data.
	stop();
	
	QFile::remove("/home/omv/vrecord.mpg");
	QFile::remove("/home/omv/arecord.wav");

	_vRecorder = new QProcess(this);
	_vRecorder->setProgram("/usr/bin/ffmpeg");
	_vRecorder->setArguments(QStringList() << "-s" << "1280x720" << "-f" << "video4linux2" << "-i" << detectCamera() << "-c:v" << "libx264" << "-r" << "24" << "-preset" << "ultrafast" << "-y" << "/home/omv/vrecord.mpg");
	_vRecorder->start();

	_aRecorder = new QProcess(this);
	_aRecorder->setProgram("/usr/bin/arecord");
	_aRecorder->setArguments(QStringList() << "-t" << "wav" << "--format=s16_le" << "--channels=1" << "--rate=48000" << "/home/omv/arecord.wav");
	_aRecorder->start();

	// Delay playback until we have a few frames and can be reasonably
	// sure we won't run out of stuff to display
	QTimer::singleShot(2000, this, &Camera::recordingStarted);
}

void Camera::recordingStarted() {
	_mpv = new QProcess(this);
	_mpv->setProgram("/usr/bin/mpv");
	_mpv->setArguments(QStringList() << "--vo=xv" << "-fs" << "--no-ontop" << "--no-border" << "--no-window-dragging" << "--no-osc" << "--no-osd-bar" <<  "--x11-netwm=no" << "--framedrop=decoder+vo" << "/home/omv/vrecord.mpg");
	_mpv->start();
}

void Camera::stopRecording() {
	_record->setText("◉");
	disconnect(_record, &TopLevelButton::clicked, this, &Camera::stopRecording);
	connect(_record, &TopLevelButton::clicked, this, &Camera::record);
	_record->setDown(false);

	if(_aRecorder) {
		_aRecorder->terminate();
		_aRecorder->waitForFinished(10000);
		if(_aRecorder->state() == QProcess::Running)
			_aRecorder->kill();
		_aRecorder->waitForFinished(1000);
		delete _aRecorder;
		_aRecorder = nullptr;
	}
	if(_vRecorder) {
		_vRecorder->write("q");
		_vRecorder->waitForFinished(10000);
		if(_vRecorder->state() == QProcess::Running) {
			_vRecorder->terminate();
			_vRecorder->waitForFinished(10000);
			if(_vRecorder->state() == QProcess::Running)
				_vRecorder->kill();
			_vRecorder->waitForFinished(1000);
		}
		delete _vRecorder;
		_vRecorder = nullptr;
	}

	_processing=new QLabel(tr("Processing, please wait..."), _bg);
	_processing->showFullScreen();

	_reencoder=new QProcess(this);
	_reencoder->setProgram("/usr/bin/ffmpeg");
	_reencoder->setArguments(QStringList() << "-y" << "-i" << "/home/omv/vrecord.mpg" << "-i" << "/home/omv/arecord.wav" << "-c:a" << "libopus" << ("/home/omv/Videos/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.z") + ".mkv") << "-vf" << "transpose=1");
	_reencoder->start();
	connect(_reencoder, &QProcess::finished, this, &Camera::reencodingFinished);

	_photo->hide();
	_record->hide();
	_switch->hide();
}

void Camera::reencodingFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	QFile::remove("/home/omv/vrecord.mpg");
	QFile::remove("/home/omv/arecord.wav");
	delete _processing;
	_processing = nullptr;
	connect(_bg, &BackgroundWidget::orientationChanged, this, &Camera::orientationChanged);
	orientationChanged();

	_photo->show();
	_record->show();
	_switch->show();
}

void Camera::quitRequested() {
	if(_vRecorder) {
		stopRecording();
	} else {
		quit();
	}
}

QString Camera::detectCamera() {
	QString videoDev;
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
					videoDev = QStringLiteral("/dev/") + f;
					qInfo("Correct video device is %s", qPrintable(videoDev));
					break;
				}
			}
		}
	}
	return videoDev;
}
