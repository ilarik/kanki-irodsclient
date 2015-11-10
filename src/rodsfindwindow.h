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

// Qt framework headers
#include <QMainWindow>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "_rodsgenquery.h"

// application headers
#include "rodscriteriawidget.h"

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsFindWindow;
}

class RodsFindWindow : public QMainWindow
{
    Q_OBJECT

public:

    //
    explicit RodsFindWindow(Kanki::RodsConnection *rodsConn, QWidget *parent = 0);

    //
    ~RodsFindWindow();

signals:

    // Qt signal for unregistering the find window for deletion of the object.
    void unregister();

protected:

    // Qt close window event handler, invokes unregister signal.
    void closeEvent(QCloseEvent *event);

private slots:

    // qt slot connected to ui add action triggered signal
    void on_actionAdd_triggered();

    // qt slot connected to ui execute action triggered signal
    void on_actionExecute_triggered();

private:

    // instance of Qt UI compiler generated UI
    Ui::RodsFindWindow *ui;
};

#endif // RODSFINDWINDOW_H
