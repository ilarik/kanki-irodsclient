/**
 * @file rodsobjtreeitem.cpp
 * @brief Implementation of class RodsObjTreeItem
 *
 * The RodsObjTreeItem class represents an iRODS object in the
 * iRODS object tree model.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsObjTreeItem header
#include "rodsobjtreeitem.h"

// initialize static class variables
const int RodsObjTreeItem::numColumns = 7;
const char *RodsObjTreeItem::columnNames[numColumns] = { "Name", "Type", "Size", "Created", "Modified", "Replication Status", "Replica#" };
const char *RodsObjTreeItem::binPrefixes[] = { "", "K", "M", "G", "T", "P", "E" };

RodsObjTreeItem::RodsObjTreeItem(RodsObjTreeItem *parent)
{
    parentItem = parent;

    this->configureMountPoint();
}

RodsObjTreeItem::RodsObjTreeItem(Kanki::RodsObjEntryPtr data, RodsObjTreeItem *parent)
{
    objEntry = data;
    parentItem = parent;

    this->configureMountPoint();
}

void RodsObjTreeItem::configureMountPoint()
{
    // if there is a parent, get mount point
    if (this->parentItem)
        this->mountPointPath = this->parentItem->mountPointPath;

    // by default, set mount point /
    else
        this->mountPointPath = "/";
}

RodsObjTreeItem::~RodsObjTreeItem()
{
    // simply delete all the children in the list
    qDeleteAll(childItems);
}

void RodsObjTreeItem::appendChild(RodsObjTreeItem *item)
{
    // push new item to back of the list
    childItems.append(item);
}

void RodsObjTreeItem::setMountPoint(std::string path)
{
    this->mountPointPath = path;
}

const std::string& RodsObjTreeItem::mountPoint() const
{
    return (this->mountPointPath);
}

RodsObjTreeItem* RodsObjTreeItem::child(int row)
{
    // get vector entry at requested position
    return childItems.at(row);
}

int RodsObjTreeItem::childCount() const
{
    // return the size of child list
    return childItems.count();
}

int RodsObjTreeItem::row() const
{
    // if parent exists, return index of this item at parent
    if (parentItem)
        return (parentItem->childItems.indexOf(const_cast<RodsObjTreeItem*>(this)));

    // by default return zero for root entry
    return (0);
}

int RodsObjTreeItem::columnCount() const
{
    // return a static number of columns
    return (numColumns);
}

QVariant RodsObjTreeItem::data(int column) const
{
    // if this is the root item
    if (!parentItem)
        return (QVariant(columnNames[column]));

    // if this is a mount point item (no item data)
    else if (!objEntry)
    {
        // return mount point
        if (column == 0)
            return (QVariant(mountPointPath.c_str()));

        else if (column == 1)
            return (QVariant("Mount Point"));

        else
            return (QVariant("--"));
    }

    // if this is a mount point child item (there is item data)
    else if (objEntry)
    {
        // depending on column position return an appropriate QVariant object
        if (column == 0)
        {
            // return object name
            return (QVariant((objEntry->getObjectName().c_str())));
        }

        else if (column == 1)
        {
            // get pointer to extension part of object name string
            const char *extPart = strrchr(objEntry->objName.c_str(), '.');

            // string to describe object type
            std::string objTypeStr;

            if (objEntry->objType == DATA_OBJ_T)
            {
                objTypeStr = "Data Object";

                // if we have a file extension, TODO: proper file ext handling
                if (extPart)
                {
                    if (!strcmp(extPart, ".dcm"))
                        objTypeStr += " (DICOM Image)";
                }
            }

            else if (objEntry->objType == COLL_OBJ_T)
            {
                objTypeStr = "Collection";

                // proof-of-concept, TODO: specifications of typed collections
                if (extPart)
                {
                    if (!strcmp(extPart, ".study"))
                        objTypeStr += " (Study)";
                }
            }

            return (QVariant(objTypeStr.c_str()));
        }

        // object size
        else if (column == 2)
        {
            if (objEntry->objType == DATA_OBJ_T)
            {
                unsigned int tmp = objEntry->objSize;
                unsigned int p = 0;

                // get order of magnitude in binary exponents
                for (p = 0; tmp / 1024; p++)
                    tmp = tmp / 1024;

                // construct size string from floored down value
                double order = floor(objEntry->objSize / pow(1024, p));
                QString sizeStr = QVariant(order).toString() + " " + RodsObjTreeItem::binPrefixes[p] + "B";

                return (QVariant(sizeStr));
            }

            else
                return (QVariant("--"));

        }

        else if (column == 3)
        {
            return (QVariant(formatDateString(objEntry->createTime).c_str()));
        }

        else if (column == 4)
        {
            return (QVariant(formatDateString(objEntry->modifyTime).c_str()));
        }

        else if (column == 5)
        {
            if (objEntry->objType == DATA_OBJ_T)
                return (QVariant(objEntry->replStatus ? "Replicated" : "Not replicated"));

            else
                return (QVariant("--"));
        }

        else if (column == 6)
        {
            if (objEntry->objType == DATA_OBJ_T)
            {
                if (objEntry->replStatus)
                    return (QVariant(objEntry->replNum));

                else
                    return (QVariant("N/A"));
            }

            else {
                return (QVariant("--"));
            }
        }
    }

    // by default, return an empty QVariant object
    return (QVariant());
}

RodsObjTreeItem* RodsObjTreeItem::parent()
{
    // return pointer to parent
    return parentItem;
}

std::string RodsObjTreeItem::formatDateString(std::string timeStampStr) const
{
    // get time stamp from string (convert string to integer)
    time_t timeStamp = (time_t)atoi(timeStampStr.c_str());

    // get pointer to converted string using thread safe ctime_r
    char *buffer = (char*)malloc(64);
    char *timeStrData = ctime_r(&timeStamp, buffer);

    // make new string object and sanitize (remove endline)
    std::string timeStr(timeStrData);
    timeStr.erase(timeStr.end()-1);

    // return sanitized string
    return (timeStr);
}

Kanki::RodsObjEntryPtr RodsObjTreeItem::getObjEntryPtr()
{
    // return pointer to rods object entry
    return (objEntry);
}

bool RodsObjTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return (false);

    for (int i = position ; i < position + count; i++)
        delete (childItems.takeAt(position));

    return (true);
}
