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
    : RodsConditionWidget(parent)
{
    this->attr = rodsAttr;

    this->labelWidget = new QLabel(label, this);
    this->layout->addWidget(this->labelWidget);

    this->condBox = new QComboBox(this);
    this->condBox->addItem("After", RodsDateConditionWidget::IsNewer);
    this->condBox->addItem("Before", RodsDateConditionWidget::IsOlder);
    this->condBox->addItem("Exactly", RodsDateConditionWidget::Equals);
    this->layout->addWidget(condBox);

    this->dateTime = new QDateTimeEdit();
    this->layout->addWidget(this->dateTime);
}

RodsDateConditionWidget::~RodsDateConditionWidget()
{
    delete (this->dateTime);
    delete (this->condBox);
    delete (this->labelWidget);
}

void RodsDateConditionWidget::evaluateConds(Kanki::RodsGenQuery *query)
{
    int opr = this->condBox->currentData().toInt();

    // get unix timestamp and pad it with zeros (for string comparison...)
    std::string value = QVariant(this->dateTime->dateTime().toTime_t()).toString().toStdString();
    value.insert(0, 11 - value.length(), '0');

    switch (opr)
    {
        case RodsDateConditionWidget::Equals:
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isEqual, value);
        break;

        case RodsDateConditionWidget::IsNewer:
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isGreaterOrEqual, value);
        break;

        case RodsDateConditionWidget::IsOlder:
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isLessOrEqual, value);
        break;

        default:
        break;
    }
}
