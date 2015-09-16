/**
 * @file rodstransferwindow.h
 * @brief Definition of class RodsTransferWindow
 *
 * The RodsTransferWindow class extends the Qt widget base class QWidget
 * and implements a transfer progress display window
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSTRANSFERWINDOW_H
#define RODSTRANSFERWINDOW_H

// Qt framework headers
#include <QVariant>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class RodsTransferWindow : public QWidget
{
    Q_OBJECT

public:

    // Constructor initializes the transfer window object and its ui,
    // sets window title as the provided title argument.
    explicit RodsTransferWindow(QString title);

signals:

    // Qt signal for signalling out the users request of cancelling.
    void cancelRequested();


public slots:

    //
    void setupMainProgressBar(QString initialMsg, int value, int maxValue);

    //
    void updateMainProgress(QString currentMsg, int value);

    //
    void progressMarquee(QString currentMsg);

    //
    void invokeCancel();

private:

    // progress window main progress mar max
    unsigned int progressMax;

    // Qt UI layout instance
    QVBoxLayout *layout;

    // Qt labels for displaying progress messages
    QLabel *mainProgressMsg, *subProgressMsg;

    // Qt progress bar objects
    QProgressBar *mainProgressBar, *subProgressBar;

    // Qt push button for cancel
    QPushButton *cancelButton;
};

#endif // RODSTRANSFERWINDOW_H
