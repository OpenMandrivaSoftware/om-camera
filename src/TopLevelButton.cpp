#include "TopLevelButton.h"

TopLevelButton::TopLevelButton(QString const &text, QWidget *parent):QPushButton(text, parent) {
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
}
