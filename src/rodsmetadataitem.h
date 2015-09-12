/**
 * @file rodsmetadataitem.h
 * @brief Definition of class RodsMetadataItem
 *
 * The RodsMetadataItem class represents an iRODS metadata tree item
 * containing an iRODS AVU triplet.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSMETADATAITEM_H
#define RODSMETADATAITEM_H

// Qt framework headers
#include <QString>
#include <QList>
#include <QVariant>

class RodsMetadataItem
{
public:

    // Constructor for instantiating metadata items having attribute name, value, unit and parent item as arguments.
    RodsMetadataItem(QString itemNameStr = QString(), QString itemValueStr = QString(), QString itemUnitStr = QString(), RodsMetadataItem *itemParentPtr = NULL);

    // Constructor for instantiating a valueless metadata item, having only attribute name and parent item as arguments.
    RodsMetadataItem(QString itemNameStr = QString(), RodsMetadataItem *parentItemPtr = NULL);

    ~RodsMetadataItem();

    // Interface for accessing the pointer to a child item at requested child item index.
    RodsMetadataItem* child(int index);

    // Interface for accessing the pointer to the parent item.
    RodsMetadataItem* parent();

    // Interface for querying child item count.
    int childCount() const;

    // Interface for querying the index of this item at its parent item.
    int childNumber() const;

    // Appends a child item to the end of the list of children.
    void appendChild(RodsMetadataItem *child);

    // Interface for querying the attribute name string.
    const QString& getName() const;

    // Interface for querying the attribute value string.
    const QString& getValue() const;

    // Interface for querying the attribute unit string.
    const QString& getUnit() const;

    // Interface for (re)setting the attribute name string.
    void setName(const QString &newName);

    // Interface for (re)setting the attribute value string.
    void setValue(const QString &newValue);

    // Interface for (re)setting the attribute unit string.
    void setUnit(const QString &newUnit);

private:

    // pointer to the parent item in the metadata tree
    RodsMetadataItem *parentItem;

    // container for storing pointers to child items
    QList<RodsMetadataItem*> children;

    // metadata attribute name, value and unit
    QString itemName, itemValue, itemUnit;
};

#endif // RODSMETADATAITEM_H
