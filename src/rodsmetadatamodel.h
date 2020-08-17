/**
 * @file rodsmetadatamodel.h
 * @brief Definition of class RodsMetadataModel
 *
 * The RodsMetadataModel class extends the Qt model class QAbstractItemModel
 * and implements a model for AVU metadata of an iRODS object.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSMETADATAMODEL_H
#define RODSMETADATAMODEL_H

// C++ standard library headers
#include <iostream>
#include <string>
#include <vector>
#include <map>

// Qt framework headers
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QApplication>
#include <QIcon>
#include <QMessageBox>

// Kanki iRODS C++ class library headers
#include "rodssession.h"
#include "rodsobjentry.h"
#include "_rodsgenquery.h"

// application headers
#include "rodsmetadataitem.h"
#include "rodsobjmetadata.h"
#include "rodsmetadataschema.h"

class RodsMetadataModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    // Constructor requires a managed pointer to the object entry, a pointer to a metadata container
    // of the object and a pointer to a metadata schema.
    RodsMetadataModel(Kanki::RodsObjEntryPtr theObjEntry, Kanki::RodsObjMetadata *theMetadata,
                      RodsMetadataSchema *schema, QObject *parent = 0);

    ~RodsMetadataModel();

    // Enumeration for model column indices.
    enum MetaColumns { metaName = 0, metaValue = 1, metaUnit = 2 };

    // Overrides superclass virtual function for querying rods metadata model data with
    // respect to a model index reference for a given Qt role.
    QVariant data(const QModelIndex &index, int role) const;

    // Overrides superclass virtual function for querying header data for
    // view display purposes, for a given section and orientation and Qt role.
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // Overrides superclass virtual function for querying flags
    // given a model index reference, depending on the characteristics of the item.
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // Overrides superclass virtual function for setting model data with respect to
    // a model index reference for a given Qt role. Metadata attribute values and units
    // can be set, not the attribute name itself, its immutable here.
    bool setData(const QModelIndex &index, const QVariant &value, int role);

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

    // Interface for refreshing the metadata model against the rods object metadata container.
    void refresh();

private:

    // qt icon objects used in the model
    QIcon nsIcon, attrIcon;

    // rods session pointer
//    Kanki::RodsSession *session;

    // managed pointer to a rods object entry
    Kanki::RodsObjEntryPtr objEntry;

    // our rods object metadata container instance
    Kanki::RodsObjMetadata *metadata;

    // our metadata schema instance
    RodsMetadataSchema *schema;

    // root item pointer for the tree
    RodsMetadataItem *rootItem;
};

#endif // RODSMETADATAMODEL_H
