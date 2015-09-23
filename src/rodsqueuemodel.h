/**
 * @file rodsqueuemodel.h
 * @brief Definition of class RodsQueueModel
 *
 * The RodsQueueModel class extends the Qt model class QAbstractTableModel
 * and implements a model for the iRODS rule exec queue. Quick and dirty.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSQUEUEMODEL_H
#define RODSQUEUEMODEL_H

// C++ standard library headers
#include <vector>
#include <string>

// Qt framework headers
#include <QAbstractTableModel>
#include <QMessageBox>
#include <QTimer>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "_rodsgenquery.h"

class RodsQueueModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    // Constructor requires a rods connection object pointer as an arument
    // and optionally a Qt parent object pointer.
    explicit RodsQueueModel(Kanki::RodsConnection *rodsConn, QObject *parent = 0);

    ~RodsQueueModel();

    // Overrides superclass virtual function for querying model data with
    // respect to a model index reference for a given Qt role.
    QVariant data(const QModelIndex &index, int role) const;

    // Overrides superclass virtual function for querying header data for
    // view display purposes, for a given section and orientation and Qt role.
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // Overrides superclass virtual function for querying current model
    // data row count given a model index reference (which is ignored here).
    int rowCount(const QModelIndex &parent) const;

    // Overrides superclass virtual function for querying current model
    // column count given a model index reference (ignored), returns a constant.
    int columnCount(const QModelIndex &parent) const;

    // Overrides superclass virtual function for querying flags
    // given a model index reference, constant flags are given however here.
    Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:

    // Refreshes model data from iRODS via a GenQuery and resets model.
    void refreshQueue();

private:

    // Qt timer object pointer for periodical refreshing of model data
    QTimer *timer;

    // pointer to rods connection object for communications
    Kanki::RodsConnection *conn;

    // simple STL instantiated container for model data
    std::vector< std::vector< std::string> > queueData;

    // static class data for model column configuration
    static const char *columnNames[];
    static const int numColumns;
};

#endif // RODSQUEUEMODEL_H
