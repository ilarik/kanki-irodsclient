/**
 * @file rodsqueuewindow.h
 * @brief Definition of class RodsQueueWindow
 *
 * The RodsQueueWindow class extends Qt widget class QWidget and implements
 * a simple window for iRODS rule exec queue viewing.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSQUEUEWINDOW_H
#define RODSQUEUEWINDOW_H

// Qt framework headers
#include <QWidget>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"

// application headers
#include "rodsqueuemodel.h"

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsQueueWindow;
}

class RodsQueueWindow : public QWidget
{
    Q_OBJECT

public:

    // Constructor requires a rods connection object pointer as an argument
    // and optionally a parent widget pointer.
    explicit RodsQueueWindow(Kanki::RodsConnection *rodsConn, QWidget *parent = 0);

    ~RodsQueueWindow();

signals:

    // Qt signal for unregistering the queue window for deletion of the object.
    void unregister();

protected:

    // Qt close window event handler, invokes unregister signal.
    void closeEvent(QCloseEvent *event);

private:

    // instance of Qt UI compiler generated UI
    Ui::RodsQueueWindow *ui;

    // pointer to rods connection object
    Kanki::RodsConnection *conn;

    // our queue model object pointer
    RodsQueueModel *queue;
};

#endif // RODSQUEUEWINDOW_H
