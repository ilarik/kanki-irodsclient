/**
 * @file rodsobjtreeitem.h
 * @brief Definition of class RodsObjTreeItem
 *
 * The RodsObjTreeItem class represents an iRODS object in the
 * iRODS object tree model.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSOBJTREEITEM_H
#define RODSOBJTREEITEM_H

// C++ standard library headers
#include <string>
#include <vector>

// ANSI C standard library headers
#include <cstring>
#include <ctime>
#include <cmath>

// Qt framework headers
#include <QAbstractItemModel>
#include <QVariant>
#include <QList>

// Kanki iRODS C++ class library headers
#include "rodsobjentry.h"

class RodsObjTreeItem
{

public:

    // Constructor, parent item pointer as the only argument, null means root item.
    RodsObjTreeItem(RodsObjTreeItem *parent = 0);

    // Constructor, for constructing proper rods object items with item data.
    RodsObjTreeItem(Kanki::RodsObjEntryPtr data, RodsObjTreeItem *parent = 0);

    ~RodsObjTreeItem();

    // Appends a new child item into the list of children.
    void appendChild(RodsObjTreeItem *child);

    // Sets iRODS mount point for the item.
    void setMountPoint(const std::string &path);

    // Interface for accessing a child item pointer at a given row index.
    RodsObjTreeItem* child(int row);

    // Interface for querying the child count of the item.
    int childCount() const;

    // Interface for querying the column count of the item. TODO: get rid of this one?
    int columnCount() const;

    // Interface for querying the row index of this item at its parent.
    int row() const;

    // Interface for querying item data for show purposes.
    QVariant data(int column) const;

    // Interface for accessing a pointer of the parent item.
    RodsObjTreeItem* parent();

    // Interface for accessing (a const reference of) the mount point string of the item.
    const std::string& mountPoint() const;

    // Interface for accessing the (managed shared pointer of) the rods object entry.
    Kanki::RodsObjEntryPtr getObjEntryPtr();

    // Removes a given count of child items from the item at given row position.
    bool removeChildren(int position, int count);

private:

    // configures the item default mount point based on parent item
    void configureMountPoint();

    // formats a date string from a unix time stamp
    std::string formatDateString(std::string timeStampStr) const;

    // iRODS mount point path of the item
    std::string mountPointPath;

    // Kanki rods object entry managed shared pointer
    Kanki::RodsObjEntryPtr objEntry;

    // Qt list of child item pointers.
    QList<RodsObjTreeItem*> childItems;

    // pointer to parent item
    RodsObjTreeItem *parentItem;

    // static class constants for data column configuration
    static const char *columnNames[];
    static const char *binPrefixes[];
    static const int numColumns;
};

#endif // RODSOBJTREEITEM_H
