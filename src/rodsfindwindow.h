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

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsFindWindow;
}

class RodsFindWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RodsFindWindow(QWidget *parent = 0);
    ~RodsFindWindow();

private:
    Ui::RodsFindWindow *ui;
};

#endif // RODSFINDWINDOW_H
