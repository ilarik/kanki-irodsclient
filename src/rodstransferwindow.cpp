/**
 * @file rodstransferwindow.h
 * @brief Definition of class RodsTransferWindow
 *
 * The RodsTransferWindow class extends the Qt widget base class QWidget
 * and implements a transfer progress display window
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsTransferWindow header
#include "rodstransferwindow.h"

RodsTransferWindow::RodsTransferWindow(const QString &title) :
    QWidget(nullptr)
{
    this->progressMax = this->subProgressMax = 0;
    this->setWindowTitle(title);
    this->setMinimumWidth(600);
    this->setFixedHeight(190);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);

    this->layout = new QVBoxLayout(this);
    this->box = new QGroupBox(this);
    this->boxLayout = new QVBoxLayout(box);

    this->mainProgress = new RodsProgressWidget(QString(), "Initializing...",
						0, 0, this);

    this->subProgress = new RodsProgressWidget(QString(), "Initializing...",
					       0, 0, this);

    this->boxLayout->addWidget(mainProgress);
    this->boxLayout->addWidget(subProgress);

    this->cancelButton = new QPushButton("Cancel", this);
    this->cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->cancelButton->setShortcut(Qt::Key_Escape);

    this->layout->addWidget(box);
    this->layout->addWidget(this->cancelButton);

    connect(this->cancelButton, &QPushButton::pressed, this, &RodsTransferWindow::invokeCancel);
}

RodsTransferWindow::~RodsTransferWindow()
{
    delete (this->mainProgress);
    delete (this->subProgress);
    delete (this->boxLayout);
    delete (this->layout);
    delete (this->cancelButton);
}

void RodsTransferWindow::setupMainProgressBar(QString initialMsg, int value, int maxValue)
{
    this->progressMax = maxValue;
    this->mainText = initialMsg;

    this->mainProgress->setMax(maxValue);
    this->mainProgress->update(initialMsg, value);
}

void RodsTransferWindow::setMainProgressMarquee(QString text)
{
    this->mainProgress->update(text, 0);
}

void RodsTransferWindow::updateMainProgress(QString currentMsg, int value)
{
    QString progMsg = "Processing object ";
    progMsg += QVariant(value).toString() + " of ";
    progMsg += QVariant(this->progressMax).toString() + ": ";
    progMsg += currentMsg;

    this->mainProgress->update(currentMsg, value);

    // when main is updated, sub is set to marquee
    this->setupSubProgressBar("In Progress...", "In Progress...", 0, 0);
}

void RodsTransferWindow::increaseMainProgress()
{
    int value = this->mainProgress->value() + 1;
    QString str = this->mainText + " (" + QVariant(value).toString() + 
	" of " + QVariant(this->progressMax).toString() + ")...";

    this->mainProgress->increment();
}

void RodsTransferWindow::setupSubProgressBar(QString itemName, QString initialMsg, int value, int maxValue)
{
    this->subProgressMax = maxValue;

    this->subProgress->setMax(maxValue);
    this->updateSubProgress(itemName, initialMsg, value);
}

void RodsTransferWindow::updateSubProgress(QString itemName, QString currentMsg, int value)
{
    this->subProgress->update(currentMsg, value);
}

void RodsTransferWindow::setSubProgressMarquee(QString itemName, QString currentMsg)
{
    this->mainProgress->update("Initializing...", 0);
    this->subProgress->update(currentMsg, 0);
}

void RodsTransferWindow::invokeCancel()
{
    this->cancelRequested();
}
