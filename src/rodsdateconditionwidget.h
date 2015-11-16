/**
 * @file rodsdateconditionwidget.h
 * @brief Definition of RodsDateConditionWidget class
 *
 * The RodsDateConditionWidget class extends the class RodsConditionWidget and
 * implements a widget for entering a date/time condition for a GenQuery.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDATECONDITIONWIDGET_H
#define RODSDATECONDITIONWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDateTimeEdit>

#include "rodsconditionwidget.h"


class RodsDateConditionWidget : public RodsConditionWidget
{
public:

    enum DateConditions { Equals, IsNewer, IsOlder };

    //
    RodsDateConditionWidget(int rodsAttr, QString label, QWidget *parent = 0);

    //
    ~RodsDateConditionWidget();

    // evaluate genquery condition generated
    virtual void evaluateConds(Kanki::RodsGenQuery *query);

private:

    // rods attr code
    int attr;

    // label widget
    QLabel *labelWidget;

    // condition operator select
    QComboBox *condBox;

    // date/time widget
    QDateTimeEdit *dateTime;
};

#endif // RODSDATECONDITIONWIDGET_H
