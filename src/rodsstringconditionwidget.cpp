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

}

void RodsStringConditionWidget::evaluateConds(Kanki::RodsGenQuery *query)
{

}
