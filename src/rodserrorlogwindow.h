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
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>

class RodsErrorLogWindow : public QWidget
{
    Q_OBJECT

public:

    //
    explicit RodsErrorLogWindow();

    //
    ~RodsErrorLogWindow();

public slots:

    //
    void logError(QString msgStr, QString errorStr, int errorCode);

private:

    //
    QVBoxLayout *layout;

    //
    QListWidget *errorLog;

    //
    QPushButton *ackButton;

    static const QIcon warnIcon;
};

#endif // RODSERRORWINDOW_H
