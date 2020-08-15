/**
 * @file rodstransferwindow.h
 * @brief Definition of class RodsTransferWindow
 *
 * The RodsTransferWindow class extends the Qt widget base class QWidget
 * and implements a transfer progress display window
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsTransferWindow header
#include "rodstransferwindow.h"

RodsTransferWindow::RodsTransferWindow(QString title) :
    QWidget(NULL)
{
    this->progressMax = this->subProgressMax = 0;
    this->setWindowTitle(title);
    this->setMinimumWidth(600);
    this->setFixedHeight(190);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);

    this->layout = new QVBoxLayout(this);

    this->box = new QGroupBox(this);
    this->boxLayout = new QVBoxLayout(box);

    this->mainProgressMsg = new QLabel(this);
    boxLayout->addWidget(this->mainProgressMsg);

    this->mainProgressBar = new QProgressBar(this);
    this->mainProgressBar->setMaximum(0);
    boxLayout->addWidget(this->mainProgressBar);

    this->subProgressMsg = new QLabel(this);
    boxLayout->addWidget(this->subProgressMsg);
    this->subProgressBar = new QProgressBar(this);
    this->subProgressBar->setMaximum(0);
    boxLayout->addWidget(this->subProgressBar);

    this->cancelButton = new QPushButton("Cancel", this);
    this->cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->cancelButton->setShortcut(Qt::Key_Escape);

    this->layout->addWidget(box);
    this->layout->addWidget(this->cancelButton);

    connect(this->cancelButton, &QPushButton::pressed, this, &RodsTransferWindow::invokeCancel);
}

RodsTransferWindow::~RodsTransferWindow()
{
    delete (this->layout);
    delete (this->mainProgressMsg);
    delete (this->mainProgressBar);
    delete (this->subProgressMsg);
    delete (this->subProgressBar);
    delete (this->cancelButton);
}

void RodsTransferWindow::setupMainProgressBar(QString initialMsg, int value, int maxValue)
{
    this->progressMax = maxValue;

    this->mainProgressBar->setMaximum(maxValue);
    this->updateMainProgress(initialMsg, value);
}

void RodsTransferWindow::setMainProgressMarquee(QString text)
{
    this->mainProgressBar->setMaximum(0);
    this->mainProgressMsg->setText(text);
}

void RodsTransferWindow::updateMainProgress(QString currentMsg, int value)
{
    QString progMsg = "Processing object ";
    progMsg += QVariant(value).toString() + " of ";
    progMsg += QVariant(this->progressMax).toString() + ": ";
    progMsg += currentMsg;

    this->mainProgressMsg->setText(progMsg);
    this->mainProgressBar->setValue(value);

    // when main is updated, sub is set to marquee
    this->setupSubProgressBar("In Progress...", "In Progress...", 0, 0);
}

void RodsTransferWindow::setupSubProgressBar(QString itemName, QString initialMsg, int value, int maxValue)
{
    this->subProgressMax = maxValue;

    this->subProgressBar->setMaximum(maxValue);
    this->updateSubProgress(itemName, initialMsg, value);
}

void RodsTransferWindow::updateSubProgress(QString itemName, QString currentMsg, int value)
{
    this->subProgressMsg->setText(currentMsg);
    this->subProgressBar->setValue(value);
}

void RodsTransferWindow::setSubProgressMarquee(QString itemName, QString currentMsg)
{
    this->mainProgressBar->setMaximum(0);
    this->subProgressBar->setMaximum(0);

    this->mainProgressMsg->setText("Initializing...");
    this->subProgressMsg->setText(currentMsg);
}

void RodsTransferWindow::invokeCancel()
{
    this->cancelRequested();
}
