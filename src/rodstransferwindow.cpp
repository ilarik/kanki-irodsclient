#include "rodstransferwindow.h"

RodsTransferWindow::RodsTransferWindow(QString title) :
    QWidget(NULL)
{
    this->setWindowTitle(title);

    this->layout = new QVBoxLayout(this);

    this->mainProgressMsg = new QLabel(this);
    this->layout->addWidget(this->mainProgressMsg);

    this->mainProgressBar = new QProgressBar(this);
    this->mainProgressBar->setMaximum(0);
    this->layout->addWidget(this->mainProgressBar);

//    this->subProgressMsg = new QLabel(this);
//    this->layout->addWidget(this->subProgressMsg);
//    this->subProgressBar = new QProgressBar(this);
//    this->subProgressBar->setMaximum(0);
//    this->layout->addWidget(this->subProgressBar);

    this->cancelButton = new QPushButton("Cancel", this);
    this->layout->addWidget(this->cancelButton);
}

void RodsTransferWindow::setupMainProgressBar(QString initialMsg, int value, int maxValue)
{
    this->mainProgressBar->setMaximum(maxValue);
    this->updateMainProgress(initialMsg, value);
}

void RodsTransferWindow::updateMainProgress(QString currentMsg, int value)
{
    this->mainProgressMsg->setText(currentMsg);
    this->mainProgressBar->setValue(value);
}
