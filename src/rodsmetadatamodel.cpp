/**
 * @file rodsmetadatamodel.cpp
 * @brief Implementation of class RodsMetadataModel
 *
 * The RodsMetadataModel class extends the Qt model class QAbstractItemModel
 * and implements a model for AVU metadata of an iRODS object.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMetadataModel header
#include "rodsmetadatamodel.h"

RodsMetadataModel::RodsMetadataModel(Kanki::RodsObjEntryPtr theObjEntry, Kanki::RodsObjMetadata *theMetadata,
                                     RodsMetadataSchema *theSchema, QObject *parent) :
    QAbstractItemModel(parent)
{
    // set pointers to rods data structures
    this->objEntry = theObjEntry;
    this->metadata = theMetadata;

    // set root node to null pointer
    rootItem = new RodsMetadataItem(QString(), NULL);

    // load icons
    nsIcon = QIcon(":/tango/icons/folder.png");
    attrIcon = QIcon(":/tango/icons/text-x-generic.png");

    this->schema = theSchema;

    // refresh model against the metadata container
    refresh();
}

RodsMetadataModel::~RodsMetadataModel()
{
    // (recursively) delete item tree
    delete rootItem;
}

QVariant RodsMetadataModel::data(const QModelIndex &index, int role) const
{
    // we only handle valid indices
    if (index.isValid())
    {
        RodsMetadataItem *item = static_cast<RodsMetadataItem*>(index.internalPointer());

        // for the decoration role we return an icon
        if (role == Qt::DecorationRole && index.column() == 0)
        {
            if (item->childCount() > 0)
                return (QVariant(nsIcon));
            else
                return (QVariant(attrIcon));
        }

        // for the display and edit roles we return string data
        else if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            QString name = item->getName();

            if (index.column() == RodsMetadataModel::metaName)
            {
                if (role == Qt::DisplayRole)
                    return (QVariant(this->schema->translateName(name.toStdString()).c_str()));

                else if (role == Qt::EditRole)
                    return (QVariant(name));
            }

            else if (index.column() == RodsMetadataModel::metaValue)
            {
                QString value = item->getValue();

                if (role == Qt::DisplayRole)
                    return (QVariant(this->schema->filterValue(name.toStdString(), value.toStdString()).c_str()));

                else if (role == Qt::EditRole)
                    return (QVariant(value));
            }

            else if (index.column() == RodsMetadataModel::metaUnit)
            {
                return (QVariant(item->getUnit()));
            }
        }
    }

    return (QVariant());
}

QVariant RodsMetadataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    (void)orientation;
    (void)role;

    // if we have sufficient conditions for giving out header data
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        if (section == RodsMetadataModel::metaName)
            return (QVariant("Name"));
        else if (section == RodsMetadataModel::metaValue)
            return (QVariant("Value"));
        else if (section == RodsMetadataModel::metaUnit)
            return (QVariant("Unit"));
    }

    return (QVariant());
}

Qt::ItemFlags RodsMetadataModel::flags(const QModelIndex &index) const
{
    (void)index;
    Qt::ItemFlags ret;

    // by default we are enabled and selectable
    ret = Qt::ItemIsEnabled| Qt::ItemIsSelectable;

    return (ret);
}

bool RodsMetadataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    (void)role;
    int status = 0;

    // sanity check
    if (index.isValid())
    {
        // get item name
        RodsMetadataItem *item = static_cast<RodsMetadataItem*>(index.internalPointer());
        QString name = item->getName();

        // handle attribute value change
        if (index.column() == RodsMetadataModel::metaValue)
        {
            // execute attribute modify targeted to the correct AVU triplet
            if ((status = this->metadata->modifyAttribute(name.toStdString(), item->getValue().toStdString(),
                                                          value.toString().toStdString())) < 0)
            {
                // error reporting via a Qt message box, TODO: signal/slot interface for reporting
                QMessageBox errMsg;
                QString errStr = "Metadata attribute value modify failure, iRODS API error: " + QVariant(status).toString();

                errMsg.setText(errStr);
                errMsg.setIcon(QMessageBox::Critical);
                errMsg.exec();

                return (false);
            }

            // when update successful, update item object and model state
            else {
                item->setValue(value.toString());
                this->dataChanged(index, index);

                return (true);
            }
        }

        // handle attribute value unit change
        else if (index.column() == RodsMetadataModel::metaUnit)
        {
            // execute attribute modify targeted to the correct AVU triplet
            if ((status = this->metadata->modifyAttribute(name.toStdString(), item->getValue().toStdString(), item->getValue().toStdString(),
                                                          item->getUnit().toStdString(), value.toString().toStdString())) < 0)
            {
                // error reporting via a Qt message box, TODO: signal/slot interface for reporting
                QMessageBox errMsg;
                QString errStr = "Metadata attribute unit modify failure, iRODS API error: " + QVariant(status).toString();

                errMsg.setText(errStr);
                errMsg.setIcon(QMessageBox::Critical);
                errMsg.exec();

                return (false);
            }

            // when update successful, update item object and model state
            else {
                item->setUnit(value.toString());
                this->dataChanged(index, index);

                return (true);
            }
        }
    }

    return (false);
}


QModelIndex RodsMetadataModel::index(int row, int column, const QModelIndex &parent) const
{
    RodsMetadataItem *parentItem = NULL, *childItem = NULL;

    // if there is no index, return an empty index object
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    // get parent item, select root if index has no parent
    if (!parent.isValid())
        parentItem = rootItem;

    else
        parentItem = static_cast<RodsMetadataItem*>(parent.internalPointer());

    // if there is a child with the row index, create new index to it
    if ((childItem = parentItem->child(row)))
        return (createIndex(row, column, childItem));

    else
        return QModelIndex();
}

QModelIndex RodsMetadataModel::parent(const QModelIndex &child) const
{
    RodsMetadataItem *parentItem = NULL, *childItem = NULL;

    if (!child.isValid())
        return (QModelIndex());

    // get child item object from index and its parent item object
    childItem = static_cast<RodsMetadataItem*>(child.internalPointer());
    parentItem = childItem->parent();

    // if parent is root, return empty index
    if (parentItem == rootItem)
        return (QModelIndex());

    // by default, return new index for parent node
    return (createIndex(parentItem->childNumber(), 0, parentItem));
}

int RodsMetadataModel::rowCount(const QModelIndex &parent) const
{
    RodsMetadataItem *parentItem = NULL;

    // other columns besides 0 doesn't have rows in this index
    if (parent.column() > 0)
        return 0;

    // get parent item if not root
    if (!parent.isValid())
        parentItem = rootItem;

    else
        parentItem = static_cast<RodsMetadataItem*>(parent.internalPointer());

    // return parent item child count
    return (parentItem->childCount());
}

int RodsMetadataModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;

    return (3);
}

void RodsMetadataModel::refresh()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // signal view objects that model is about to be reset
    beginResetModel();

    // reset metadata tree by recursively destroying the old one
    if (this->rootItem)
        delete (this->rootItem);

    this->rootItem = new RodsMetadataItem(QString(), NULL);

    // get copies of the metadata hashtables
    Kanki::RodsObjMetadata::KeyVals attrValues = this->metadata->getValues();
    Kanki::RodsObjMetadata::KeyVals attrUnits = this->metadata->getUnits();

    // initialize a hashtable for namespace for processing
    std::map< std::string, RodsMetadataItem* > nsMap;
    RodsMetadataItem *defaultNs = new RodsMetadataItem(".", rootItem);
    nsMap["."] = defaultNs;

    // fill the namespace table from the schema
    std::vector<std::string> nsVector = this->schema->getNamespaces();

    for (std::vector<std::string>::const_iterator i = nsVector.begin(); i != nsVector.end(); i++)
    {
        std::string nsPrefix = *i;
        nsMap[nsPrefix] = new RodsMetadataItem(nsPrefix.c_str(), rootItem);
    }

    // construct metadata tree, process attributes and place them into appropriate namespaces
    for (Kanki::RodsObjMetadata::KeyVals::const_iterator iter = attrValues.begin(); iter != attrValues.end(); iter++)
    {
        std::string name = iter->first;
        std::vector< std::string > values = iter->second;
        RodsMetadataItem *nsPtr = NULL;

        // find suitable namespace for attribute
        for (std::map< std::string , RodsMetadataItem* >::iterator nsi = nsMap.begin(); nsi != nsMap.end() && nsPtr == NULL; nsi++)
        {
            std::string ns = nsi->first;

            // if attribute name matches namespace, select it
            if (!name.find(ns))
                nsPtr = nsi->second;
        }

        // if no namespace is found, use default
        if (!nsPtr)
            nsPtr = defaultNs;

        // create metadata items for all instances of the attribute
        for (unsigned int i = 0; i < values.size(); i++)
        {
            RodsMetadataItem *attrItem = new RodsMetadataItem(name.c_str(), attrValues[name].at(i).c_str(),
                                                              attrUnits[name].at(i).c_str(), nsPtr);
            nsPtr->appendChild(attrItem);
        }
    }

    // place configures namespace instances into the metadata tree root
    for (std::map< std::string, RodsMetadataItem* >::iterator nsi = nsMap.begin(); nsi != nsMap.end(); nsi++)
    {
        RodsMetadataItem *nsItemPtr = nsi->second;

        if (nsItemPtr->childCount() > 0)
            rootItem->appendChild(nsItemPtr);
    }

    // model has been reset, indices invalidated
    this->endResetModel();

    QApplication::restoreOverrideCursor();
}
