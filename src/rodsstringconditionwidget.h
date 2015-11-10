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

// Kanki iRODS C++ class library headers
#include "_rodsgenquery.h"

// application headers
#include "rodsconditionwidget.h"


class RodsStringConditionWidget : public RodsConditionWidget
{
    Q_OBJECT

public:

    // constructor instantiates a new widget for entering a string
    // condition against a given rods attribute with a given label
    RodsStringConditionWidget(int rodsAttr, QString label, QWidget *parent = 0);

    // evaluate genquery condition generated
    virtual void evaluateConds(Kanki::RodsGenQuery *query);

private:

    // rods attr code
    int attr;
};

#endif // RODSSTRINGCONDITIONWIDGET_H
