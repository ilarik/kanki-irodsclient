/**
 * @file rodsstringcriteriawidget.h
 * @brief Definition of RodsStringCriteriaWidget class
 *
 * The RodsStringCriteriaWidget class extends the class RodsCriteriaWidget and
 * implements a widget for entering a simple string condition for a GenQuery.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSSTRINGCRITERIAWIDGET_H
#define RODSSTRINGCRITERIAWIDGET_H

// C++ standard library headers
#include <string>

// Qt framework headers
#include <QWidget>
#include <QString>

// Kanki iRODS C++ class library headers
#include "_rodsgenquery.h"

// application headers
#include "rodscriteriawidget.h"


class RodsStringCriteriaWidget : public RodsCriteriaWidget
{
    Q_OBJECT

public:

    // constructor instantiates a new widget for entering a string
    // condition against a given rods attribute with a given label
    RodsStringCriteriaWidget(int rodsAttr, QString label);

    // evaluate genquery condition generated
    virtual void evaluateConds(Kanki::RodsGenQuery *query);

};

#endif // RODSSTRINGCRITERIAWIDGET_H
