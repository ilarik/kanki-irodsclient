/**
 * @file rodsfindwindow.h
 * @brief Definition of RodsFindWindow class
 *
 * The RodsFindWindow class extends the Qt window class QMainWindow and
 * implements an iRODS find window UI.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSFINDWINDOW_H
#define RODSFINDWINDOW_H

#include <vector>

// Qt framework headers
#include <QMainWindow>
#include <QInputDialog>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "_rodsgenquery.h"

// application headers
#include "rodsconditionwidget.h"
#include "rodsstringconditionwidget.h"

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsFindWindow;
}

class RodsFindWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum SearchConditions { DataObjName, CollName };

    //
    explicit RodsFindWindow(Kanki::RodsConnection *rodsConn, QWidget *parent = 0);

    //
    ~RodsFindWindow();

signals:

    // Qt signal for unregistering the find window for deletion of the object.
    void unregister();

public slots:

    // Qt slot for requesting to add a condition
    void addCondition();

    // Qt slot for requesting to execute
    void executeSearch();

protected:

    // Qt close window event handler, invokes unregister signal.
    void closeEvent(QCloseEvent *event);

private:

    // instance of Qt UI compiler generated UI
    Ui::RodsFindWindow *ui;

    // our connection object
    Kanki::RodsConnection *conn;

    // container for condition widgets (to be added dynamically)
    std::vector<RodsConditionWidget*> condWidgets;

};

#endif // RODSFINDWINDOW_H
