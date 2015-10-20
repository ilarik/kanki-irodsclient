/**
 * @file rodserrorwindow.h
 * @brief Implementation of class RodsErrorWindow
 *
 * The RodsErrorWindow class extends the Qt widget base class QWidget
 * and implements a error list display window
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#include "rodserrorlogwindow.h"

RodsErrorLogWindow::RodsErrorLogWindow() :
    QWidget(NULL)
{
    this->layout = new QVBoxLayout(this);

    this->errorLog = new QListWidget(this);
    this->layout->addWidget(this->errorLog);

    this->ackButton = new QPushButton("Acknowledge", this);
    this->layout->addWidget(this->ackButton);
}

RodsErrorLogWindow::~RodsErrorLogWindow()
{
    delete (this->layout);
    delete (this->errorLog);
    delete (this->ackButton);
}

void RodsErrorLogWindow::logError(QString msgStr, QString errorStr, int errorCode)
{

}
