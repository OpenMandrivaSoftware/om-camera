#pragma once
#include <QPushButton>

class TopLevelButton:public QPushButton {
	Q_OBJECT
public:
	TopLevelButton(QString const &text, QWidget *parent = nullptr);
};
