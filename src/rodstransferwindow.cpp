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
    box(new QGroupBox(this)),
    layout(new QVBoxLayout(this)),
    boxLayout(new QVBoxLayout(box))
{
    this->progressMax = this->subProgressMax = 0;

    this->setWindowTitle(title);
    this->setMinimumWidth(600);

    this->setWindowFlags(Qt::CustomizeWindowHint | 
			 Qt::WindowTitleHint | 
			 Qt::WindowMinimizeButtonHint);

    this->mainProgress = new RodsProgressWidget(std::string(), "Initializing...",
						0, 0, this);

    this->boxLayout->addWidget(mainProgress);

    this->cancelButton = new QPushButton("Cancel", this);
    this->cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->cancelButton->setShortcut(Qt::Key_Escape);

    this->layout->addWidget(box);
    this->layout->addWidget(this->cancelButton);

    connect(this->cancelButton, &QPushButton::pressed,
	    this, &RodsTransferWindow::invokeCancel);
}

RodsTransferWindow::~RodsTransferWindow()
{
    for (auto&& [id, widget] : this->progressItems)
	delete (widget);

    delete (this->mainProgress);
    delete (this->boxLayout);
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
    this->boxLayout->addWidget(widget);
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
	{
	    this->mainProgress->update("Initializing...", 0);
	    widget->update(currentMsg, 0);
	}
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
	    this->boxLayout->removeWidget(widget);
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
