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

    ~RodsTransferWindow();

signals:

    // Qt signal for signalling out the users request of cancelling.
    void cancelRequested();


public slots:

    // Qt slot for configuring the transfer window main progress bar
    // for its initial message, initial value and maxValue.
    void setupMainProgressBar(QString initialMsg, int value, int maxValue);

    // Qt slot for updating the transfer window main progress bar
    // for current message and current value.
    void updateMainProgress(QString currentMsg, int value);

    // Qt slot for configuring the transfer window secondary progress bar
    // for its initial message, initial value and maxValue.
    void setupSubProgressBar(QString initialMsg, int value, int maxValue);

    // Qt slot for updating the transfer window main progress bar
    // for current message and current value.
    void updateSubProgress(QString currentMsg, int value);

    // Qt slot for entering the transfer window progress bars into
    // marquee mode.
    void progressMarquee(QString currentMsg);

    // Qt slot for UI cancel action invoking cancel requested signal.
    void invokeCancel();

private:

    // progress window progress mar maxima
    unsigned int progressMax, subProgressMax;

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
