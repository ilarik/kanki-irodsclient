/**
 * @file rodstransferwindow.h
 * @brief Definition of class RodsTransferWindow
 *
 * The RodsTransferWindow class extends the Qt widget base class QWidget
 * and implements a transfer progress display window
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSTRANSFERWINDOW_H
#define RODSTRANSFERWINDOW_H

// C++ standard headers
#include <map>

// Qt framework headers
#include <QVariant>
#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QGroupBox>
#include <QScrollArea>

class RodsTransferWindow : public QWidget
{
    Q_OBJECT

public:

    class RodsProgressWidget : public QFrame
    {

    public:
	
	explicit RodsProgressWidget(const std::string &_id, 
				    const QString &_text,
				    const unsigned &_value,
				    const unsigned &_max,
				    QWidget *parent = nullptr)
	    : QFrame(parent),
	      id(_id),
	      text(_text),
	      current(_value),
	      max(_max),
	      layout(new QVBoxLayout(this)),
	      msg(new QLabel(this)),
	      bar(new QProgressBar(this))
	{
	    this->setFrameStyle(QFrame::Panel | QFrame::Raised);
	    this->setMinimumHeight(64);
	    this->setMaximumHeight(64);

	    msg->setText(text);
	    bar->setMaximum(max);
	    bar->setValue(current);
	    
	    layout->addWidget(msg);
	    layout->addWidget(bar);
	}

	void setMsg(const QString &newMsg)
	{
	    text = newMsg;
	    msg->setText(text);
	}

	void setMax(unsigned newMax)
	{
	    max = newMax;
	    bar->setMaximum(max);
	}

	void increment()
	{
	    current++;
	    bar->setValue(current);
	}

	unsigned value() const
	{
	    return (current);
	}

	void setMarquee(QString newMsg)
	{
	    setMax(0);
	    setMsg(newMsg);
	}

	void setProgress(unsigned newVal)
	{
	    current = newVal;
	    bar->setValue(current);
	}

	void update(QString newMsg, unsigned newVal)
        {
	    setMsg(newMsg);
	    setProgress(newVal);
	}
	
	~RodsProgressWidget()
	{
	    delete (bar);
	    delete (msg);
	    delete (layout);
	}

    private:

	std::string id;

	QString text;
	unsigned current, max;
	
	QVBoxLayout *layout;
	QLabel *msg;
	QProgressBar *bar;
    };

    using RodsProgressWidgetTable = std::map<std::string, RodsProgressWidget*>;

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

    void finalizeSubProgressBar(QString itemName);

    // Qt slot for updating the transfer window main progress bar
    // for current message and current value.
    void updateSubProgress(QString itemName, QString currentMsg, int value);

    // Qt slot for entering the transfer window progress bars into
    // marquee mode.
    void setSubProgressMarquee(QString itemName, QString currentMsg);

    // Qt slot for UI cancel action invoking cancel requested signal.
    void invokeCancel();

private:

    // layouts
    QVBoxLayout *layout;

    // box for progress displays
//    QGroupBox *box;
    QScrollArea *scroll;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;

    // window state
    QString mainText;
    unsigned int progressMax, subProgressMax;

    // progress bar objects
    RodsProgressWidget *mainProgress;
    RodsProgressWidgetTable progressItems; 

    // cancel button
    QPushButton *cancelButton;
};

#endif // RODSTRANSFERWINDOW_H
