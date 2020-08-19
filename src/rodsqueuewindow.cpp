/**
 * @file rodsqueuewindow.cpp
 * @brief Implementation of class RodsQueueWindow
 *
 * The RodsQueueWindow class extends Qt widget class QWidget and implements
 * a simple window for iRODS rule exec queue viewing.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsQueueWindow header
#include "rodsqueuewindow.h"

// generated UI class Ui::RodsQueueWindow header
#include "ui_rodsqueuewindow.h"

RodsQueueWindow::RodsQueueWindow(Kanki::RodsSession *theSession, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RodsQueueWindow)
{
    // set session pointer and create new queue model
    this->session = theSession;
    this->queue = new RodsQueueModel(session);

    // setup qt ui, instantiate and confgure ui objects
    this->ui->setupUi(this);
    this->ui->queueTable->setModel(queue);
    this->ui->queueTable->resizeColumnsToContents();
}

RodsQueueWindow::~RodsQueueWindow()
{
    delete (this->ui);
    delete (this->queue);
}

void RodsQueueWindow::closeEvent(QCloseEvent *event)
{
    event->accept();

    // signal out unregistering
    this->unregister();
}
