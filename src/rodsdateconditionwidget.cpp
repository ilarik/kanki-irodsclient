/**
 * @file rodsdateconditionwidget.h
 * @brief Implementation of RodsDateConditionWidget class
 *
 * The RodsDateConditionWidget class extends the class RodsConditionWidget and
 * implements a widget for entering a date/time condition for a GenQuery.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsDateConditionWidget header
#include "rodsdateconditionwidget.h"

RodsDateConditionWidget::RodsDateConditionWidget(int rodsAttr, QString label, QWidget *parent)
{
    this->attr = rodsAttr;

    this->layout = new QHBoxLayout(this);
    this->layout->setMargin(0);

    this->labelWidget = new QLabel(label, this);
    this->layout->addWidget(this->labelWidget);

    this->condBox = new QComboBox(this);
    //this->condBox->addItem("Equals", RodsStringConditionWidget::Equals);

    this->layout->addWidget(condBox);

    this->dateTime = new QDateTimeEdit();
    this->layout->addWidget(this->dateTime);
}

RodsDateConditionWidget::~RodsDateConditionWidget()
{
    delete (this->dateTime);
    delete (this->condBox);
    delete (this->labelWidget);
    delete (this->layout);
}
