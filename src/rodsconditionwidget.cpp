/**
 * @file rodsconditionwidget.cpp
 * @brief Implementation of RodsConditionWidget class
 *
 * The RodsConditionWidget class extends the Qt widget class QWidget and
 * implements an abstract widget for entering an iRODS GenQuery condition.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsConditionWidget header
#include "rodsconditionwidget.h"

RodsConditionWidget::RodsConditionWidget(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::Panel | QFrame::Raised);

    this->layout = new QHBoxLayout(this);
    this->layout->setMargin(4);
    this->layout->setAlignment(Qt::AlignLeft);

    this->remove = new QPushButton("Remove");
    this->layout->addWidget(remove);

    connect(this->remove, &QPushButton::clicked, this, &RodsConditionWidget::invokeUnregister);
}

RodsConditionWidget::~RodsConditionWidget()
{
    delete (this->layout);
}

void RodsConditionWidget::invokeUnregister()
{
    this->unregister(this);
}
