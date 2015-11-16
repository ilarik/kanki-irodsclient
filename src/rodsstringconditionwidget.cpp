/**
 * @file rodsstringconditionwidget.h
 * @brief Definition of RodsStringConditionWidget class
 *
 * The RodsStringConditionWidget class extends the class RodsConditionWidget and
 * implements a widget for entering a simple string condition for a GenQuery.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsStringConditionWidget header
#include "rodsstringconditionwidget.h"

RodsStringConditionWidget::RodsStringConditionWidget(int rodsAttr, QString label, QWidget *parent) :
    RodsConditionWidget(parent)
{
    this->attr = rodsAttr;

    this->labelWidget = new QLabel(label, this);
    this->layout->addWidget(this->labelWidget);

    this->condBox = new QComboBox(this);
    this->condBox->addItem("Equals", RodsStringConditionWidget::Equals);
    this->condBox->addItem("Contains", RodsStringConditionWidget::Contains);
    this->condBox->addItem("Begins With", RodsStringConditionWidget::BeginsWith);
    this->condBox->addItem("Ends With", RodsStringConditionWidget::EndsWith);
    this->condBox->addItem("Is Like (Wildcard %)", RodsStringConditionWidget::IsLike);
    this->layout->addWidget(condBox);

    this->valueField = new QLineEdit(this);
    this->layout->addWidget(this->valueField);
}

RodsStringConditionWidget::~RodsStringConditionWidget()
{
    delete (this->valueField);
    delete (this->condBox);
    delete (this->labelWidget);
}

void RodsStringConditionWidget::evaluateConds(Kanki::RodsGenQuery *query)
{
    int opr = this->condBox->currentData().toInt();
    std::string value;

    switch (opr)
    {
        case RodsStringConditionWidget::Equals:
            value = this->valueField->text().toStdString();
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isEqual, value);
        break;

        case RodsStringConditionWidget::BeginsWith:
            value = this->valueField->text().toStdString() + "%";
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isLike, value);
        break;

        case RodsStringConditionWidget::EndsWith:
            value = "%" + this->valueField->text().toStdString();
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isLike, value);
        break;

        case RodsStringConditionWidget::Contains:
            value = "%" + this->valueField->text().toStdString() + "%";
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isLike, value);
        break;

        case RodsStringConditionWidget::IsLike:
            value = this->valueField->text().toStdString();
            query->addQueryCondition(this->attr, Kanki::RodsGenQuery::isLike, value);
        break;

        default:
        break;
    }
}
