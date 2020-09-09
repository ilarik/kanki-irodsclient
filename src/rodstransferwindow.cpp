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

// application class RodsTransferWindow header
#include "rodstransferwindow.h"

RodsTransferWindow::RodsTransferWindow(const QString &title) :
    QWidget(nullptr),
    layout(new QVBoxLayout(this)),
    scroll(new QScrollArea(this)),
    scrollWidget(new QWidget(this)),
    scrollLayout(new QVBoxLayout(scrollWidget))
{
    this->progressMax = this->subProgressMax = 0;

    this->setWindowTitle(title);
    this->setMinimumSize(QSize(584, 192));
    this->setMaximumSize(QSize(584, 1168));
    
    this->setWindowFlags(Qt::CustomizeWindowHint | 
			 Qt::WindowTitleHint | 
			 Qt::WindowMinimizeButtonHint);

    this->mainProgress = new RodsProgressWidget(std::string(), "Initializing...",
						0, 0, this);

    this->scroll->setSizePolicy(QSizePolicy::MinimumExpanding ,QSizePolicy::MinimumExpanding);
    this->scroll->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    this->scroll->setWidget(scrollWidget);
    this->scroll->setAlignment(Qt::AlignTop);

    this->scroll->setWidgetResizable(true);
    this->scroll->setMinimumSize(QSize(568, 96));
    this->scroll->setSizeIncrement(QSize(0, 96));
    this->scroll->setMaximumSize(QSize(568, 1168));
    
    this->scrollLayout->insertWidget(0, mainProgress, Qt::AlignTop);
    this->scrollLayout->insertSpacerItem(1, new QSpacerItem(584, 1168,
							    QSizePolicy::Maximum, QSizePolicy::Maximum));

    this->cancelButton = new QPushButton("Cancel", this);
    this->cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->cancelButton->setShortcut(Qt::Key_Escape);

    this->layout->addWidget(scroll);
    this->layout->addWidget(this->cancelButton);

    connect(this->cancelButton, &QPushButton::pressed,
	    this, &RodsTransferWindow::invokeCancel);
}

RodsTransferWindow::~RodsTransferWindow()
{
    for (auto&& [id, widget] : this->progressItems)
	delete (widget);

    delete (this->mainProgress);
    delete (this->scrollLayout);
    delete (this->layout);
    delete (this->cancelButton);
}

void RodsTransferWindow::setupMainProgressBar(QString initialMsg, int value, int maxValue)
{
    this->progressMax = maxValue;
    this->mainText = initialMsg;

    this->mainProgress->setMax(maxValue);
    this->mainProgress->update(initialMsg, value);
}

void RodsTransferWindow::setMainProgressMarquee(QString text)
{
    this->mainProgress->update(text, 0);
}

void RodsTransferWindow::updateMainProgress(QString currentMsg, int value)
{
    QString progMsg = "Processing object ";
    progMsg += QVariant(value).toString() + " of ";
    progMsg += QVariant(this->progressMax).toString() + ": ";
    progMsg += currentMsg;

    this->mainProgress->update(currentMsg, value);
}

void RodsTransferWindow::increaseMainProgress()
{
    int value = this->mainProgress->value() + 1;
    QString str = this->mainText + " (" + QVariant(value).toString() + 
	" of " + QVariant(this->progressMax).toString() + ")...";

    this->mainProgress->increment();
}

void RodsTransferWindow::setupSubProgressBar(QString itemName, QString initialMsg,
					     int value, int maxValue)
{
    std::string id = itemName.toStdString();

    RodsProgressWidget *widget = new RodsProgressWidget(id, initialMsg,
							value, maxValue, this);

    this->progressItems[id] = widget;
    this->scrollLayout->insertWidget(1, widget);
}

void RodsTransferWindow::updateSubProgress(QString itemName, QString currentMsg, int value)
{
    std::string id = itemName.toStdString();

    try
    {
	RodsProgressWidget *widget = this->progressItems[id];
	
	if (widget)
	    widget->update(currentMsg, value);
    }
    catch (const std::exception &e)
    {
	// handle exception
    }
}

void RodsTransferWindow::setSubProgressMarquee(QString itemName, QString currentMsg)
{
    std::string id = itemName.toStdString();

    try 
    {
	RodsProgressWidget *widget = this->progressItems[id];
	
	if (widget)
	    widget->setMarquee(currentMsg);
    }
    catch (const std::exception &e)
    {
	// handle exception
    }
}

void RodsTransferWindow::finalizeSubProgressBar(QString itemName)
{
    std::string id = itemName.toStdString();
    
    try
    {
	RodsProgressWidget *widget = this->progressItems[id];

	if (widget)
	{
	    this->scrollLayout->removeWidget(widget);
	    delete (widget);
	}

	this->progressItems.erase(id);
    }
    catch (const std::exception &e)
    {
	// handle exception
    }
}

void RodsTransferWindow::invokeCancel()
{
    this->cancelRequested();
}
