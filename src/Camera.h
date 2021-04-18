#pragma once

#include <QApplication>
#include <QLabel>
#include <QProcess>
#include "BackgroundWidget.h"
#include "TopLevelButton.h"

class Camera:public QApplication {
	Q_OBJECT
public:
	Camera(int &argc, char **&argv);
	~Camera();
	static QString detectCamera();
public Q_SLOTS:
	void stop();
	void setCamera(bool front=false);
	void switchCamera();
	void photo();
	void record();
	void stopRecording();
	void orientationChanged();
	void quitRequested();
private Q_SLOTS:
	void recordingStarted();
protected:
	void reencodingFinished(int exitCode, QProcess::ExitStatus exitStatus);
protected:
	BackgroundWidget *	_bg;
	bool			_usingFrontCamera;
	TopLevelButton *	_photo;
	TopLevelButton *	_record;
	TopLevelButton *	_switch;
	TopLevelButton *	_quit;
	QProcess *		_mpv;
	QProcess *		_vRecorder;
	QProcess *		_aRecorder;
	QProcess *		_reencoder;
	QLabel *		_processing;
};
