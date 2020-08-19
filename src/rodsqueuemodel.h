/**
 * @file rodsqueuemodel.h
 * @brief Definition of class RodsQueueModel
 *
 * The RodsQueueModel class extends the Qt model class QAbstractTableModel
 * and implements a model for the iRODS rule exec queue.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSQUEUEMODEL_H
#define RODSQUEUEMODEL_H

// C++ standard library headers
#include <vector>
#include <string>
#include <mutex>

// Qt framework headers
#include <QAbstractTableModel>
#include <QMessageBox>
#include <QTimer>

// new-age iRODS headers
#include "query_builder.hpp"

// Kanki iRODS C++ class library headers
#include "rodssession.h"

class RodsQueueModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit RodsQueueModel(Kanki::RodsSession *theSession, QObject *parent = nullptr);
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

    void invokeRefresh();

private:

    // we take up types from irods
    using row_type = irods::query<rcComm_t>::value_type;

    // Refreshes model data from iRODS via an iRODS query and resets model.
    void refreshQueue();
    
    // Qt timer object pointer for periodical refreshing of model data
    QTimer *timer;
    
    // pointer to rods connection object for communications
    Kanki::RodsSession *session;
    
    // lock for updating the data
    std::mutex updateMutex;
    
    // simple container for model data
    std::vector<row_type> queueData;

    // set model column count to static value
    static constexpr int numColumns = 12;

    // set model column names to static values
    static constexpr char *columnNames[] = { "ID", "Name", "Rule Exec Info File Path",
					     "User Name", "Address", "Time", "Frequency",
					     "Priority", "Estimated Exec Time", "Notification Address",
					     "Last Exec Time", "Status" };
};

#endif // RODSQUEUEMODEL_H
