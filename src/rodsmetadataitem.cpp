/**
 * @file rodsmetadataitem.cpp
 * @brief Implementation of class RodsMetadataItem
 *
 * The RodsMetadataItem class represents an iRODS metadata tree item
 * containing an iRODS AVU triplet.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMetadataItem header
#include "rodsmetadataitem.h"

RodsMetadataItem::RodsMetadataItem(QString itemNameStr, QString itemValueStr, QString itemUnitStr, RodsMetadataItem *itemParentPtr)
{
    // associate parent item
    parentItem = itemParentPtr;

    // set properties
    itemName = itemNameStr;
    itemValue = itemValueStr;
    itemUnit = itemUnitStr;
}

RodsMetadataItem::RodsMetadataItem(QString itemNameStr, RodsMetadataItem *parentItemPtr)
{
    itemName = itemNameStr;
    parentItem = parentItemPtr;
}

RodsMetadataItem::~RodsMetadataItem()
{
    // simply delete all child items in the list
    qDeleteAll(children);
}

RodsMetadataItem *RodsMetadataItem::parent()
{
    // return pointer to parent
    return parentItem;
}

RodsMetadataItem* RodsMetadataItem::child(int index)
{
    // return pointer to child of requested index
    return (children.at(index));
}

int RodsMetadataItem::childCount() const
{
    // get size of the list of children
    return (children.size());
}

int RodsMetadataItem::childNumber() const
{
    // if there is a parent, get my index from parent
    if (parentItem)
        return (parentItem->children.indexOf(const_cast<RodsMetadataItem*>(this)));

    // otherwise index is 0
    return (0);
}

void RodsMetadataItem::appendChild(RodsMetadataItem *child)
{
    // simply append to the end of list
    children.append(child);
}

const QString& RodsMetadataItem::getName() const
{
    return (this->itemName);
}

const QString& RodsMetadataItem::getValue() const
{
    return (this->itemValue);
}

const QString& RodsMetadataItem::getUnit() const
{
    return (this->itemUnit);
}

void RodsMetadataItem::setName(const QString &newName)
{
    this->itemName = newName;
}

void RodsMetadataItem::setValue(const QString &newValue)
{
    this->itemValue = newValue;
}

void RodsMetadataItem::setUnit(const QString &newUnit)
{
    this->itemUnit = newUnit;
}
