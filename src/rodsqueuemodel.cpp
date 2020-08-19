/**
* @file rodsqueuemodel.cpp
 * @brief Implementation of class RodsQueueModel
 *
 * The RodsQueueModel class extends the Qt model class QAbstractTableModel
 * and implements a model for the iRODS rule exec queue. Quick and dirty.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsQueueModel header
#include "rodsqueuemodel.h"

RodsQueueModel::RodsQueueModel(Kanki::RodsSession *theSession, QObject *parent) :
    QAbstractTableModel(parent),
    session(theSession)
{
    // create a timer to invoke refresh
    timer = new QTimer(this);
    timer->setInterval(1000);

    // refresh once
    this->invokeRefresh();

    // connect timer to handler and start
    connect(timer, SIGNAL(timeout()) , this, SLOT(invokeRefresh()));
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
    return (queueData.size());
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
    if (index.isValid() && role == Qt::DisplayRole)
    {
	// check boundaries for index row and column
	if (index.row() < (int)queueData.size() &&
	    index.column() < (int)queueData.at(index.row()).size())
	{
	    // simply return raw data, TODO: return formatted data
	    return (QVariant(queueData.at(index.row()).at(index.column()).c_str()));
	}
    }

    return (QVariant());
}

QVariant RodsQueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // return data for display role
    if (role == Qt::DisplayRole)
    {
        // to horizontal header put column name
        if (orientation == Qt::Horizontal)
            return QVariant(columnNames[section]);
    }

    return (QVariant());
}

Qt::ItemFlags RodsQueueModel::flags(const QModelIndex &index) const
{
    // for valid indices return default flags
    if (index.isValid())
        return (Qt::ItemIsEnabled);

    // by default no flags
    return (0);
}

void RodsQueueModel::invokeRefresh()
{
    this->session->scheduleTask([&] { this->refreshQueue(); });
}

void RodsQueueModel::refreshQueue()
{
    int status = 0;
    QString errStr;
    std::vector<row_type> tempData;

    // we get all the rule exec queue attrs
    std::string queryStr = "SELECT RULE_EXEC_ID, RULE_EXEC_NAME, RULE_EXEC_REI_FILE_PATH, RULE_EXEC_USER_NAME, "
	"RULE_EXEC_ADDRESS, RULE_EXEC_TIME, RULE_EXEC_FREQUENCY, RULE_EXEC_PRIORITY, RULE_EXEC_ESTIMATED_EXE_TIME, "
	"RULE_EXEC_NOTIFICATION_ADDR, RULE_EXEC_LAST_EXE_TIME, RULE_EXEC_STATUS";

    // try to acquire a free conn
    Kanki::RodsSession::connection_proxy conn = this->session->getConnection();

    if (conn)
    {
	rcComm_t *comm = static_cast<rcComm_t*>(conn);

	// query into temp storage
	try {
	    using query_builder = irods::experimental::query_builder;
	    
	    for (const row_type &row : query_builder().build(*comm, queryStr))
		tempData.push_back(row);
	}
	catch (const irods::exception &e)
	{
	    status = e.code();
	    errStr = "iRODS API error in rule exec queue refresh, status: " + QVariant(status).toString();
	}
	catch (const std::exception &e)
	{
	    status = SYS_INTERNAL_ERR;
	    errStr = "Client-side internal error in rule exec queue refresh, status: " + QVariant(status).toString();
	}
    }
    
    // report errors to UI
    if (status < 0)
    {
	// if query fails, stop timer! 
        if (this->timer)
            this->timer->stop();
	
	// report error via a message box
        QMessageBox errMsg;
        errMsg.setText(errStr);
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();
    }

    // threads will be held off here
    std::lock_guard mutexGuard(this->updateMutex);
   
    // make data live
    beginResetModel();
    this->queueData = tempData;
    endResetModel();
}
