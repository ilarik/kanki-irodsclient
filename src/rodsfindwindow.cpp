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
    this->conn = rodsConn;

    this->ui->setupUi(this);
    this->ui->criteriaLayout->setAlignment(Qt::AlignTop);

    // setup combo box for condition selection
    this->ui->condSel->addItem("Data Object Name", RodsFindWindow::DataObjName);
    this->ui->condSel->addItem("Collection Name (Path)", RodsFindWindow::CollName);

    // connect ui event signals to handler slots
    connect(this->ui->condAdd, &QPushButton::clicked, this, &RodsFindWindow::addCondition);
    connect(this->ui->actionExecute, &QAction::triggered, this, &RodsFindWindow::executeSearch);
}

RodsFindWindow::~RodsFindWindow()
{
    delete (this->ui);

    // delete dynamically created widgets
    for (std::vector<RodsConditionWidget*>::iterator i = this->condWidgets.begin();
         i != this->condWidgets.end(); i++)
    {
        RodsConditionWidget *ptr = *i;
        delete (ptr);
    }
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

    // depending on condition, instantiate appropriate widget
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

    // if we have a widget, add it
    if (widget)
    {
        this->ui->criteriaLayout->addWidget(widget);
        this->condWidgets.push_back(widget);
    }
}

void RodsFindWindow::executeSearch()
{
    int status = 0;
    Kanki::RodsGenQuery query(this->conn);
    query.addQueryAttribute(COL_DATA_NAME);
    query.addQueryAttribute(COL_COLL_NAME);

    // evaluate genquery conditions from the condition widgets
    for (std::vector<RodsConditionWidget*>::iterator i = this->condWidgets.begin();
         i != this->condWidgets.end(); i++)
    {
        RodsConditionWidget *widget = *i;
        widget->evaluateConds(&query);
    }

    if (this->conn->isReady())
    {
        if ((status = query.execute()) < 0)
        {
            // report error
        }

        // when success, parse thru query result set
        else {
            std::vector<std::string> names = query.getResultSetForAttr(COL_DATA_NAME);
            std::vector<std::string> colls = query.getResultSetForAttr(COL_COLL_NAME);

            for (unsigned int i = 0; i < names.size() && i < colls.size(); i++)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                std::string path = colls.at(i) + '/' + names.at(i);

                item->setText(0, path.c_str());
                this->ui->treeWidget->addTopLevelItem(item);
            }
        }
    }
}
