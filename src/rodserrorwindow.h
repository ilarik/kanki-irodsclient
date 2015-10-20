/**
 * @file rodserrorwindow.h
 * @brief Definition of class RodsErrorWindow
 *
 * The RodsErrorWindow class extends the Qt widget base class QWidget
 * and implements a error list display window
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSERRORWINDOW_H
#define RODSERRORWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

class RodsErrorWindow : public QWidget
{
    Q_OBJECT

public:

    explicit RodsErrorWindow();

    ~RodsErrorWindow();

private:

};

#endif // RODSERRORWINDOW_H
