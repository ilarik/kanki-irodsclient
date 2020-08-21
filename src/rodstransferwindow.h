/**
 * @file rodstransferwindow.h
 * @brief Definition of class RodsTransferWindow
 *
 * The RodsTransferWindow class extends the Qt widget base class QWidget
 * and implements a transfer progress display window
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
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
#include <QGroupBox>

class RodsTransferWindow : public QWidget
{
    Q_OBJECT

public:

    // we take no parents
    explicit RodsTransferWindow(const QString &title);
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
    
    void increaseMainProgress();

    // Qt slot for setting the main progress bar into marquee mode
    void setMainProgressMarquee(QString text);

    // Qt slot for configuring the transfer window secondary progress bar
    // for its initial message, initial value and maxValue.
    void setupSubProgressBar(QString itemName, QString initialMsg, int value, int maxValue);

    // Qt slot for updating the transfer window main progress bar
    // for current message and current value.
    void updateSubProgress(QString itemName, QString currentMsg, int value);

    // Qt slot for entering the transfer window progress bars into
    // marquee mode.
    void setSubProgressMarquee(QString itemName, QString currentMsg);

    // Qt slot for UI cancel action invoking cancel requested signal.
    void invokeCancel();

private:

    // progress window progress mar maxima
    unsigned int progressMax, subProgressMax;

    QString mainText;

    // layouts
    QVBoxLayout *layout;

    // Qt group box for progress displays
    QGroupBox *box;

    // Qt layout for the group box
    QVBoxLayout *boxLayout;

    // Qt labels for displaying progress messages
    QLabel *mainProgressMsg, *subProgressMsg;

    // Qt progress bar objects
    QProgressBar *mainProgressBar, *subProgressBar;

    // Qt push button for cancel
    QPushButton *cancelButton;
};

#endif // RODSTRANSFERWINDOW_H
