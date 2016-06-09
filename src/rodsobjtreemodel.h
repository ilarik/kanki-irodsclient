/**
 * @file rodsobjtreemodel.h
 * @brief Definition of class RodsObjTreeModel
 *
 * The class RodsObjTreeModel extends the Qt model class QAbstractItemModel
 * and implements a model for the iRODS object tree.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSOBJTREEMODEL_H
#define RODSOBJTREEMODEL_H

// C++ standard library headers
#include <iostream>
#include <string>

// boost library headers
#include "boost/tokenizer.hpp"

// Qt framework headers
#include <QApplication>
#include <QAbstractItemModel>
#include <QMessageBox>
#include <QIcon>
#include <QMimeData>
#include <QUrl>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsobjentry.h"

// application headers
#include "rodsobjtreeitem.h"

class RodsObjTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    // Constructor requires a Kanki rods conn pointer and an initial rods path.
    RodsObjTreeModel(Kanki::RodsConnection *conn, const std::string &path, QObject *parent = 0);

    ~RodsObjTreeModel();

    // Overrides superclass virtual function for querying rods object model data with
    // respect to a model index reference for a given Qt role.
    QVariant data(const QModelIndex &index, int role) const;

    // Overrides superclass virtual function for setting model data with respect to
    // a model index reference for a given Qt role. Only the object name is allowed to be set.
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // Overrides superclass virtual function for querying flags
    // given a model index reference, depending on the characteristics of the item.
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // Overrides superclass virtual function for querying header data for
    // view display purposes, for a given section and orientation and Qt role.
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // Overrides superclass virtual function for generating a Qt model index for
    // an item at a given row and column with respect to a parent index.
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    // Overrides superclass virtual function for accessing a Qt model index for
    // the parent of a given child item index.
    QModelIndex parent(const QModelIndex &child) const;

    // Overrides superclass virtual function for querying the row count of the model
    // at a given parent object index.
    int rowCount(const QModelIndex &parent) const;

    // Overrides superclass virtual function for querying the column count of the model,
    // passes the query along to the root item.
    int columnCount(const QModelIndex &parent) const;

    // Overrides superclass virtual function for querying whether the item at
    // a given index has any children.
    bool hasChildren(const QModelIndex &parent) const;

    // Overrides superclass virtual function for querying whether to execute the
    // lazy loading of rods object model data.
    bool canFetchMore(const QModelIndex &parent) const;

    // Overrides superclass virtual function for executing the lazy loading of
    // the rods object model data from irods to a given model index.
    void fetchMore(const QModelIndex &parent);

    // Overrides superclass virtual function for removing a count of rows
    // from the model at a given parent item index.
    bool removeRows(int row, int count, const QModelIndex &parent);

    // Overrides superclass virtual function for querying supported drop
    // actions for drag&drop functionality.
    Qt::DropActions supportedDropActions() const;

    // Overrides superclass virtual function for querying the capability of
    // dropping mime data into the model at given row, column and model index.
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action,
                         int row, int column, const QModelIndex &parent) const;

    // Overrides superclass virtual function for executing the drop of mime data
    // into the model at given row, column and index.
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);

    // Overrides superclass virtual function for generating drag&drop mime data.
    QMimeData* mimeData(const QModelIndexList &indexes) const;

    // Interface for adding an additional rods mount point to the object model.
    void addMountPoint(const std::string &path);

    // Interface for resolving a rods path to an Kanki::RodsObjEntryPtr
    Kanki::RodsObjEntryPtr resolvePathToEntry(const std::string &path);

public slots:

    // Interface for requesting the model to refresh at a certain index.
    void refreshAtIndex(QModelIndex parent);

    // Qt slot for refreshing a rods collection in the object model at given path.
    void refreshAtPath(QString path);

private:

    // qt icon objects used in the model
    QIcon mountIcon, collIcon, dataIcon;

    // rods conn pointer
    Kanki::RodsConnection *rodsConn;

    // root item pointer for the object tree
    RodsObjTreeItem *rootItem;

    // initial rods mount path
    std::string initialPath;
};

#endif // RODSOBJTREEMODEL_H
