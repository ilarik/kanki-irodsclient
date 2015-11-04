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

#include <QApplication>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QDateTime>

class RodsErrorLogWindow : public QWidget
{
    Q_OBJECT

public:

    // Default constructor accepts no arguments, generates
    // error log window ui.
    explicit RodsErrorLogWindow();

    // Destructor cleans up after allocated resources.
    ~RodsErrorLogWindow();

public slots:

    // Qt slot for logging an error to the error log with message msgStr,
    // detail string detailStr and rods api error errorCode.
    void logError(QString msgStr, QString detailStr, int errorCode);

signals:

    // Qt signal for signaling out that we have errors with
    // a count of errorCount.
    void errorsPresent(unsigned int errorCount);

private:

    // qt layout for error log window
    QVBoxLayout *layout;

    // qt listwidget provides the error log
    QListWidget *errorLog;

    // qt icon resource for the warning icon
    QIcon warnIcon;
};

#endif // RODSERRORWINDOW_H
