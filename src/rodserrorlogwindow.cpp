/**
 * @file rodserrorwindow.h
 * @brief Implementation of class RodsErrorWindow
 *
 * The RodsErrorWindow class extends the Qt widget base class QWidget
 * and implements a error list display window
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#include "rodserrorlogwindow.h"

RodsErrorLogWindow::RodsErrorLogWindow() :
    QWidget(NULL)
{
    this->setWindowTitle("iRODS Client Error Log");
    this->setMinimumWidth(600);

    this->layout = new QVBoxLayout(this);

    this->errorLog = new QListWidget(this);
    this->layout->addWidget(this->errorLog);

    this->clearButton = new QPushButton("Clear Error Log", this);
    this->layout->addWidget(this->clearButton);

    connect(this->clearButton, &QPushButton::pressed, this, &RodsErrorLogWindow::clearLog);
    this->clearButton->setDisabled(true);

    this->warnIcon = QIcon(":/tango/icons/dialog-warning.svg");
}

RodsErrorLogWindow::~RodsErrorLogWindow()
{
    delete (this->layout);
    delete (this->errorLog);
}

void RodsErrorLogWindow::logError(QString msgStr, QString detailStr, int errorCode)
{
    QString itemStr = QDateTime::currentDateTime().toString() + "\n";
    itemStr += msgStr;

    if (detailStr.length())
        itemStr += "\nDetails: " + detailStr;

    itemStr += "\nStatus: " + QVariant(errorCode).toString();

    QListWidgetItem *item = new QListWidgetItem(this->warnIcon, itemStr, this->errorLog);
    this->errorLog->insertItem(0, item);
    this->errorLog->scrollToItem(item);

    this->clearButton->setDisabled(false);
    this->errorsPresent(this->errorLog->count());
}

void RodsErrorLogWindow::clearLog()
{
    // take out items and delete objects
    while (this->errorLog->count())
    {
        QListWidgetItem *ptr = this->errorLog->takeItem(0);
        delete (ptr);
    }

    this->clearButton->setDisabled(true);

    // signal main window of zero error count
    this->errorsPresent(0);
}
