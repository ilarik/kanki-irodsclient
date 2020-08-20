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
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSQUEUEWINDOW_H
#define RODSQUEUEWINDOW_H

// Qt framework headers
#include <QWidget>
#include <QCloseEvent>

// Kanki iRODS C++ class library headers
#include "rodssession.h"

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

    explicit RodsQueueWindow(Kanki::RodsSession *theSession, QWidget *parent = nullptr);
    ~RodsQueueWindow();

public slots:

    void reportError(QString errStr, int status);

signals:

    // Qt signal for unregistering the queue window for deletion of the object.
    void unregister();

protected:

    // Qt close window event handler, invokes unregister signal.
    void closeEvent(QCloseEvent *event);

private:

    // instance of Qt UI compiler generated UI
    Ui::RodsQueueWindow *ui;

    // pointer to rods session object
    Kanki::RodsSession *session;

    // our queue model object pointer
    RodsQueueModel *queue;
};

#endif // RODSQUEUEWINDOW_H
