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

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    QLabel *labelWidget = new QLabel(label, this);
    layout->addWidget(labelWidget);

    QComboBox *condBox = new QComboBox(this);
    condBox->addItem("Equals", RodsStringConditionWidget::Equals);
    condBox->addItem("Begins With", RodsStringConditionWidget::BeginsWith);
    condBox->addItem("Ends With", RodsStringConditionWidget::EndsWith);
    condBox->addItem("Contains", RodsStringConditionWidget::Contains);
    condBox->addItem("Is Like (Wildcard %)", RodsStringConditionWidget::IsLike);
    layout->addWidget(condBox);

    QLineEdit *valueField = new QLineEdit(this);
    layout->addWidget(valueField);
}

void RodsStringConditionWidget::evaluateConds(Kanki::RodsGenQuery *query)
{

}
