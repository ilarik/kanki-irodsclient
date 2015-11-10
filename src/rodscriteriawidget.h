/**
 * @file rodscriteriawidget.h
 * @brief Definition of RodsCriteriaWidget class
 *
 * The RodsCriteriaWidget class extends the Qt widget class QWidget and
 * implements an abstract widget for entering an iRODS GenQuery condition.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSCRITERIAWIDGET_H
#define RODSCRITERIAWIDGET_H

// C++ standard library headers
#include <string>

// Qt framework headers
#include <QWidget>

// Kanki iRODS C++ class library headers
#include <rodsconnection.h>
#include <_rodsgenquery.h>

class RodsCriteriaWidget : public QWidget
{
    Q_OBJECT

public:

    //
    explicit RodsCriteriaWidget(QWidget *parent = 0);

    // pure virtual function to get the criteria widget's
    // corresponding rods attribute code
    virtual int getAttr() = 0;

    // pure virtual function to evaluate the criteria widget's
    // selected condition operator
    virtual Kanki::RodsGenQuery::CondOpr getCondOpr() = 0;

    // pure virtual function to evaluate the criteria widget's
    // current rods attribute value
    virtual std::string getValue() = 0;

};

#endif // RODSCRITERIAWIDGET_H
