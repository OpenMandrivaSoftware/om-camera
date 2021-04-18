#include "BackgroundWidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent):QWidget(parent) {
	setPalette(Qt::black);
}

void BackgroundWidget::resizeEvent(QResizeEvent *e) {
	QWidget::resizeEvent(e);
	Q_EMIT orientationChanged();
}
