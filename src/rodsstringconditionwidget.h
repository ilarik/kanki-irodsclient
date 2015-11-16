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

#ifndef RODSSTRINGCONDITIONWIDGET_H
#define RODSSTRINGCONDITIONWIDGET_H

// C++ standard library headers
#include <string>

// Qt framework headers
#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

// Kanki iRODS C++ class library headers
#include "_rodsgenquery.h"

// application headers
#include "rodsconditionwidget.h"


class RodsStringConditionWidget : public RodsConditionWidget
{
    Q_OBJECT

public:

    // class local enumeration for string conditions
    enum StringConditions { Equals, BeginsWith, EndsWith, Contains, IsLike };

    // constructor instantiates a new widget for entering a string
    // condition against a given rods attribute with a given label
    RodsStringConditionWidget(int rodsAttr, QString label, QWidget *parent = 0);

    // destructor
    ~RodsStringConditionWidget();

    // evaluate genquery condition generated
    virtual void evaluateConds(Kanki::RodsGenQuery *query);

protected:

    // rods attr code
    int attr;

    // label widget
    QLabel *labelWidget;

    // condition operator select
    QComboBox *condBox;

    // text field
    QLineEdit *valueField;
};

#endif // RODSSTRINGCONDITIONWIDGET_H
