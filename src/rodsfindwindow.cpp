/**
 * @file rodsfindwindow.cpp
 * @brief Implementation of class RodsFindWindow
 *
 * The RodsFindWindow class extends the Qt window class QMainWindow and
 * implements an iRODS find window UI.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsFindWindow header
#include "rodsfindwindow.h"

// generated UI class Ui::RodsFindWindow header
#include "ui_rodsfindwindow.h"

RodsFindWindow::RodsFindWindow(Kanki::RodsConnection *rodsConn, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RodsFindWindow)
{
    this->ui->setupUi(this);
    this->ui->criteriaLayout->setAlignment(Qt::AlignTop);

    // setup combo box for condition selection
    this->ui->condSel->addItem("Data Object Name", RodsFindWindow::DataObjName);
    this->ui->condSel->addItem("Collection Name (Path)", RodsFindWindow::CollName);

    // connect ui event signals to handler slots
    connect(this->ui->condAdd, &QPushButton::clicked, this, &RodsFindWindow::addCondition);

}

RodsFindWindow::~RodsFindWindow()
{
    delete ui;
}

void RodsFindWindow::closeEvent(QCloseEvent *event)
{
    (void)event;

    // signal out unregistering
    this->unregister();
}

void RodsFindWindow::addCondition()
{
    RodsConditionWidget *widget = NULL;
    int cond = this->ui->condSel->currentData().toInt();
    QString label = this->ui->condSel->currentText();

    switch (cond)
    {
        case RodsFindWindow::DataObjName:
            widget = new RodsStringConditionWidget(COL_DATA_NAME, label);
        break;

        case RodsFindWindow::CollName:
            widget = new RodsStringConditionWidget(COL_COLL_NAME, label);
        break;

        default:
        break;
    }

    if (widget)
    {
        this->ui->criteriaLayout->addWidget(widget);
        this->condWidgets.push_back(widget);
    }
}

void RodsFindWindow::executeSearch()
{

}

void RodsFindWindow::on_actionExecute_triggered()
{

}
