#ifndef RODSTRANSFERWINDOW_H
#define RODSTRANSFERWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class RodsTransferWindow : public QWidget
{
    Q_OBJECT

public:

    //
    explicit RodsTransferWindow(QString title);

signals:

    //
    void cancelRequested();


public slots:

    //
    void setupMainProgressBar(QString initialMsg, int value, int maxValue);

    //
    void updateMainProgress(QString currentMsg, int value);

private:

    QVBoxLayout *layout;
    QLabel *mainProgressMsg, *subProgressMsg;
    QProgressBar *mainProgressBar, *subProgressBar;
    QPushButton *cancelButton;
};

#endif // RODSTRANSFERWINDOW_H
