/**
 * @file rodsqueuemodel.cpp
 * @brief Implementation of class RodsQueueModel
 *
 * The RodsQueueModel class extends the Qt model class QAbstractTableModel
 * and implements a model for the iRODS rule exec queue. Quick and dirty.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsQueueModel header
#include "rodsqueuemodel.h"

// set model column count to static value
const int RodsQueueModel::numColumns = 12;

// set model column names to static values
const char *RodsQueueModel::columnNames[numColumns] = { "ID", "Name", "Rule Exec Info File Path",
                                                        "User Name", "Address", "Time", "Frequency",
                                                        "Priority", "Estimated Exec Time", "Notification Address",
                                                        "Last Exec Time", "Status" };

RodsQueueModel::RodsQueueModel(Kanki::RodsConnection *rodsConn, QObject *parent) :
    QAbstractTableModel(parent)
{
    // set conn pointer
    conn = rodsConn;

    // initialize queue model with a refresh
    refreshQueue();

    // create a timer object to refresh every second
    timer = new QTimer(this);
    timer->setInterval(1000);

    // connect timer to refresh function and start it
    connect(timer, SIGNAL(timeout()) , this, SLOT(refreshQueue()));
    timer->start();
}

RodsQueueModel::~RodsQueueModel()
{
    delete (timer);
}

int RodsQueueModel::rowCount(const QModelIndex &parent) const
{
    // ignore index argument, unused in this model
    (void)parent;

    // simply return vector size
    if (!queueData.empty())
        return (queueData.at(0).size());

    return (0);
}

int RodsQueueModel::columnCount(const QModelIndex &parent) const
{
    // ignore index argument, unused
    (void)parent;

    // return column count
    return (numColumns);
}

QVariant RodsQueueModel::data(const QModelIndex &index, int role) const
{
    // we only handle valid indices
    if (index.isValid())
    {
        if (role == Qt::DisplayRole)
        {
            // check boundaries for index row and column
            if (index.column() < (int)queueData.size())
            {
                if (index.row() < (int)queueData.at(index.column()).size())
                {
                    // simply return raw data, TODO: return formatted data
                    return (QVariant(queueData.at(index.column()).at(index.row()).c_str()));
                }
            }
        }
    }

    return (QVariant());
}

QVariant RodsQueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // return data for display role
    if (role == Qt::DisplayRole)
    {
        // for horizontal header return column name
        if (orientation == Qt::Horizontal)
            return QVariant(columnNames[section]);

        // for vertical header return row number
        else if (orientation == Qt::Vertical)
            return (QVariant(section+1));
    }

    // by default return empty variant object
    return (QVariant());
}

Qt::ItemFlags RodsQueueModel::flags(const QModelIndex &index) const
{
    // for valid indices return default flags
    if (index.isValid())
        return (Qt::ItemIsEnabled);

    // by default, return no flags
    return (0);
}

void RodsQueueModel::refreshQueue()
{
    int status = 0;

    // initialize new rods gen query object
    Kanki::RodsGenQuery query(this->conn);

    // set gen query attributes
    query.addQueryAttribute(COL_RULE_EXEC_ID);
    query.addQueryAttribute(COL_RULE_EXEC_NAME);
    query.addQueryAttribute(COL_RULE_EXEC_REI_FILE_PATH);
    query.addQueryAttribute(COL_RULE_EXEC_USER_NAME);
    query.addQueryAttribute(COL_RULE_EXEC_ADDRESS);
    query.addQueryAttribute(COL_RULE_EXEC_TIME);
    query.addQueryAttribute(COL_RULE_EXEC_FREQUENCY);
    query.addQueryAttribute(COL_RULE_EXEC_PRIORITY);
    query.addQueryAttribute(COL_RULE_EXEC_ESTIMATED_EXE_TIME);
    query.addQueryAttribute(COL_RULE_EXEC_NOTIFICATION_ADDR);
    query.addQueryAttribute(COL_RULE_EXEC_LAST_EXE_TIME);
    query.addQueryAttribute(COL_RULE_EXEC_STATUS);

    // set query condition, user name
    query.addQueryCondition(COL_RULE_EXEC_USER_NAME, Kanki::RodsGenQuery::isEqual, conn->rodsUser());

    // try to execute rods gen query
    if ((status = query.execute()) < 0)
    {
        // when query fails, stop timer if it exists
        if (this->timer)
            this->timer->stop();

        // report error via a message box
        QMessageBox errMsg;
        QString errStr = "iRODS api error in queue model refresh " + QVariant(status).toString();

        errMsg.setText(errStr);
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
    }

    // on success, get results and signal for data change (all data)
    else {
        beginResetModel();
        queueData = query.getResultSet();
        endResetModel();
    }
}
