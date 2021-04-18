#pragma once

#include <QWidget>

/**
 * Our "main widget" is just a blank QWidget -- because the real
 * UI is MPV running in fullscreen mode with a couple of widgets
 * displayed on top.
 * We just need the "main widget" to detect when the phone
 * orientation is changed.
 */
class BackgroundWidget:public QWidget {
	Q_OBJECT
public:
	BackgroundWidget(QWidget *parent = nullptr);
	void resizeEvent(QResizeEvent *e);
Q_SIGNALS:
	void orientationChanged();
};
