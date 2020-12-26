// This file is taken from mpv-examples
// https://github.com/mpv-player/mpv-examples libmpv/qt_opengl
// master branch as of 2020/12/26, 00:16:30 CET
// Local modifications:
// * Fix build in C++20 mode
// * Adapt qthelper.hpp location
// * Adapt coding style

#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QtWidgets/QOpenGLWidget>
#include <mpv/client.h>
#include <mpv/render_gl.h>
#include "qthelper.hpp"

class MpvWidget Q_DECL_FINAL: public QOpenGLWidget
{
	Q_OBJECT
public:
	MpvWidget(QWidget *parent = nullptr, Qt::WindowFlags f = static_cast<Qt::WindowFlags>(0));
	~MpvWidget();
	void command(const QVariant& params);
	void setProperty(const QString& name, const QVariant& value);
	QVariant getProperty(const QString& name) const;
	QSize sizeHint() const override { return QSize(480, 270);}
Q_SIGNALS:
	void durationChanged(int value);
	void positionChanged(int value);
protected:
	void initializeGL() override;
	void paintGL() override;
private Q_SLOTS:
	void on_mpv_events();
	void maybeUpdate();
private:
	void handle_mpv_event(mpv_event *event);
	static void on_update(void *ctx);

	mpv_handle *mpv;
	mpv_render_context *mpv_gl;
};



#endif // PLAYERWINDOW_H
