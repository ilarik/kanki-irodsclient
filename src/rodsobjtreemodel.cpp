/**
 * @file rodsobjtreemodel.cpp
 * @brief Implementation of class RodsObjTreeModel
 *
 * The class RodsObjTreeModel extends the Qt model class QAbstractItemModel
 * and implements a model for the iRODS object tree.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsObjTreeModel header
#include "rodsobjtreemodel.h"

RodsObjTreeModel::RodsObjTreeModel(Kanki::RodsConnection *conn, const std::string &path, QObject *parent)
    : QAbstractItemModel(parent)
{
    // set iRODS connection object pointer
    rodsConn = conn;

    // create new root item (with null parent pointer)
    rootItem = new RodsObjTreeItem(NULL);

    // set initial mount point collection path
    initialPath = path;

    // initialize icons
    mountIcon = QIcon(":/tango/icons/folder-remote.png");
    collIcon = QIcon(":/tango/icons/folder.png");
    dataIcon = QIcon(":/tango/icons/text-x-generic.png");

    // setup initial object tree model mount point
    addMountPoint(path);
}

RodsObjTreeModel::~RodsObjTreeModel()
{
    // initiate recursion to delete item tree
    delete rootItem;
}

QVariant RodsObjTreeModel::data(const QModelIndex &index, int role) const
{
    RodsObjTreeItem *item = NULL;

    // for an invalid index return empty data
    if (!index.isValid())
        return (QVariant());

    // get pointer to item object
    item = static_cast<RodsObjTreeItem*>(index.internalPointer());

    // if decoration data is being requested for column 0
    if ((role == Qt::DecorationRole) && (index.column() == 0))
    {
        // if item is a mount point
        if (!item->getObjEntryPtr())
        {
            return(QVariant(mountIcon));
        }

        else {
            // get item icon depending on item type
            if (item->getObjEntryPtr()->objType == DATA_OBJ_T)
                return (QVariant(dataIcon));

            else if (item->getObjEntryPtr()->objType == COLL_OBJ_T)
                return (QVariant(collIcon));
        }
    }

    // for display purposes we query the item object
    else if (role == Qt::DisplayRole)
        return (item->data(index.column()));

    // also for editing purposes on item name we query data
    else if ((role == Qt::EditRole) && (index.column() == 0))
        return (item->data(index.column()));

    return (QVariant());
}

bool RodsObjTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // sanity check on sufficient conditions
    if (index.isValid() && role == Qt::EditRole)
    {
        // we allow data set operation only on item name
        if (index.column() == 0)
        {
            RodsObjTreeItem *item = static_cast<RodsObjTreeItem*>(index.internalPointer());
            Kanki::RodsObjEntryPtr objEntry = item->getObjEntryPtr();
            int status = 0;

            // try to rename rods object
            if ((status = this->rodsConn->renameObj(objEntry, value.toString().toStdString())) >= 0)
            {
                // in success we signal model data change
                this->dataChanged(index, index);

                // for collection items children must be refreshed!
                if (item->childCount())
                    this->refreshAtIndex(index);

                return (true);
            }

            else {
                // report error via a message box
                QMessageBox errMsg;
                QString errStr = "iRODS API error in rods object model update: " + QVariant(status).toString();

                errMsg.setText(errStr);
                errMsg.setIcon(QMessageBox::Critical);
                errMsg.exec();

                return (false);
            }
        }
    }

    return (false);
}

Qt::ItemFlags RodsObjTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret;

    // get item to be able to deduce needed flags
    RodsObjTreeItem *item = static_cast<RodsObjTreeItem*>(index.internalPointer());

    // for an invalid index we return no flags
    if (!index.isValid())
        return (0);

    // by default, certain basic flags
    ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // if item is a mount point, we add drop capability
    if (!item->getObjEntryPtr())
    {
        ret |= Qt::ItemIsDropEnabled;
    }

    // for proper rods object items
    else {
        ret |= Qt::ItemIsDragEnabled;

        // allow to edit item name
        if (index.column() == 0)
            ret |= Qt::ItemIsEditable;

        // if we have a data object as an item, no children
        if (item->getObjEntryPtr()->objType == DATA_OBJ_T)
            ret |= Qt::ItemNeverHasChildren;

        // collection objects can receive drops
        else if (item->getObjEntryPtr()->objType == COLL_OBJ_T)
            ret |= Qt::ItemIsDropEnabled;
    }

    return (ret);
}

QVariant RodsObjTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // if we have sufficient conditions for giving out header data
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
        return (rootItem->data(section));

    // by default, return empty variant object
    return (QVariant());
}

QModelIndex RodsObjTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    RodsObjTreeItem *parentItem = NULL, *childItem = NULL;

    // if there is no index, return an empty index object
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    // get parent item, select root if index has no parent
    if (!parent.isValid())
        parentItem = rootItem;

    else
        parentItem = static_cast<RodsObjTreeItem*>(parent.internalPointer());

    // if there is a child with the row index, create new index to it
    if ((childItem = parentItem->child(row)))
        return (createIndex(row, column, childItem));

    else
        return QModelIndex();
}

QModelIndex RodsObjTreeModel::parent(const QModelIndex &child) const
{
    RodsObjTreeItem *parentItem = NULL, *childItem = NULL;

    if (!child.isValid())
        return (QModelIndex());

    // get child item object from index and its parent item object
    childItem = static_cast<RodsObjTreeItem*>(child.internalPointer());
    parentItem = childItem->parent();

    // if parent is root, return empty index
    if (parentItem == rootItem)
        return (QModelIndex());

    // by default, return new index for parent node
    return (createIndex(parentItem->row(), 0, parentItem));
}

int RodsObjTreeModel::rowCount(const QModelIndex &parent) const
{
    RodsObjTreeItem *parentItem = NULL;

    // other columns besides 0 doesn't have rows in this index
    if (parent.column() > 0)
        return 0;

    // get parent item if not root
    if (!parent.isValid())
        parentItem = rootItem;

    else
        parentItem = static_cast<RodsObjTreeItem*>(parent.internalPointer());

    // return parent item child count
    return (parentItem->childCount());
}

int RodsObjTreeModel::columnCount(const QModelIndex &parent) const
{
    // get column count from parent or root item
    if (parent.isValid())
        return (static_cast<RodsObjTreeItem*>(parent.internalPointer())->columnCount());

    else
        return rootItem->columnCount();
}

bool RodsObjTreeModel::hasChildren(const QModelIndex &parent) const
{
    // for root node we return true
    if (!parent.isValid())
        return (true);

    // for other nodes we consult the item data
    else
    {
        // get item object pointer
        RodsObjTreeItem *theItem = static_cast<RodsObjTreeItem*>(parent.internalPointer());

        // mount points have children by default
        if (!theItem->getObjEntryPtr())
        {
            return (true);
        }

        // for rods items only collections may have children
        else {
            if (theItem->getObjEntryPtr()->objType == DATA_OBJ_T)
                return (false);

            else if (theItem->getObjEntryPtr()->objType == COLL_OBJ_T)
                return (true);
        }
    }

    // by default, return false
    return (false);
}

bool RodsObjTreeModel::canFetchMore(const QModelIndex &parent) const
{
    // index needs to be valid for lazy fetching
    if (parent.isValid())
    {
        RodsObjTreeItem *theItem = static_cast<RodsObjTreeItem*>(parent.internalPointer());

        // in the case of a mount point (no rods object entry data available)
        if (!theItem->getObjEntryPtr())
        {
            // we fetch if we haven't already
            if (theItem->childCount())
                return (false);

            else
                return (true);
        }

        else {
            // for data object we don't fetch children
            if (theItem->getObjEntryPtr()->objType == DATA_OBJ_T)
                return (false);

            // for collection objects children are fetched all at once
            else if (theItem->getObjEntryPtr()->objType == COLL_OBJ_T)
            {
                // if there are currently no children, allow a fetch
                if (!theItem->childCount())
                    return (true);

                else
                    return (false);
            }
        }
    }

    // by default return false
    return (false);
}

void RodsObjTreeModel::fetchMore(const QModelIndex &parent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // refresh children
    this->refreshAtIndex(parent);

    QApplication::restoreOverrideCursor();
}

bool RodsObjTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // sanity check, valid index required
    if (parent.isValid())
    {
        // item whose children are to be removed
        RodsObjTreeItem *theItem = static_cast<RodsObjTreeItem*>(parent.internalPointer());

        // if there are children
        if (theItem->childCount())
        {
            // sanity check, remove count has to be less than child count
            if (count <= theItem->childCount())
            {
                // signal view that a remove operation is starting
                beginRemoveRows(parent, row, row + count - 1);

                // remove children from the item
                theItem->removeChildren(row, count);

                // signal view that the remove operation is finished
                endRemoveRows();
                QApplication::processEvents();

                return (true);
            }
        }
    }

    // otherwise return false
    return (false);
}

void RodsObjTreeModel::refreshAtIndex(QModelIndex parent)
{
    if (!parent.isValid())
        return;

    // get item object and clear it of its children
    RodsObjTreeItem *item = static_cast<RodsObjTreeItem*>(parent.internalPointer());

    // if the item is a data object we have nothing to do
    if (item->getObjEntryPtr())
        if (item->getObjEntryPtr()->objType == DATA_OBJ_T)
            return;

    // if there are children, delete them
    if (item->childCount())
    {
        QApplication::processEvents();

        // signal view objects that model data is about to change
        beginRemoveRows(parent, 0, item->childCount()-1);

        // if the item has children, delete them
        if (item->childCount())
            item->removeChildren(0, item->childCount());

        // item data removed, indices invalidated!
        endRemoveRows();
    }

    // refresh collection items
    std::string collPath;
    int status = 0;

    // get path whether we are fetching for a mount point or collection
    if (!item->getObjEntryPtr())
        collPath = item->mountPoint();

    else if (item->getObjEntryPtr()->objType == COLL_OBJ_T)
        collPath = item->getObjEntryPtr()->objName;

    else
        return;

    std::vector<Kanki::RodsObjEntryPtr> *rodsColl = new std::vector<Kanki::RodsObjEntryPtr>;

    // try to read from rods, if it fails this is unrecoverable
    if ((status = rodsConn->readColl(collPath, rodsColl)) < 0)
    {
        // report error via a message box
        QMessageBox errMsg;
        QString errStr = "Unrecoverable iRODS API error in rods object model refresh: ";
        errStr += QVariant(status).toString() + ", exiting!";

        errMsg.setText(errStr);
        errMsg.setIcon(QMessageBox::Critical);
        errMsg.exec();

        qApp->quit();
    }

    else {
        // if there is something to insert
        if (rodsColl->size())
        {
            // let the view(s) know we are inserting into the model
            beginInsertRows(parent, 0, rodsColl->size());

            // iterate through vector and add child items
            std::vector<Kanki::RodsObjEntryPtr>::iterator theEnd = rodsColl->end();

            for (std::vector<Kanki::RodsObjEntryPtr>::iterator i = rodsColl->begin(); i != theEnd; i++)
            {
                Kanki::RodsObjEntryPtr collEntPtr = *i;
                RodsObjTreeItem *child = new RodsObjTreeItem(collEntPtr, item);

                item->appendChild(child);
            }

            // insert operation ended, process event queue
            endInsertRows();
            QApplication::processEvents();
        }
    }

    // delete temporary vector of pointers
    delete rodsColl;
}

void RodsObjTreeModel::refreshAtPath(QString path)
{
    QModelIndex curIndex;
    RodsObjTreeItem *curItem = this->rootItem;
    std::string curPath, pathStr = path.toStdString();
    boost::char_separator<char> separator("/");
    boost::tokenizer< boost::char_separator<char> > tokens(pathStr, separator);

    // iterate path tokens to find item
    for (boost::tokenizer< boost::char_separator<char> >::iterator iter = tokens.begin();
         iter != tokens.end(); iter++)
    {
        curPath += "/" + *iter;

        for (int i = 0; i < curItem->childCount(); i++)
        {
            RodsObjTreeItem *childItem = curItem->child(i);
            Kanki::RodsObjEntryPtr objEntry = childItem->getObjEntryPtr();

            // if we found an item matching path token, we break from loop
            if (!objEntry || (objEntry->objType == COLL_OBJ_T && !objEntry->objName.compare(curPath)))
            {
                curItem = childItem;
                curIndex = this->index(i, 0, curIndex);

                break;
            }
        }
    }

    // if item is a proper item and its path matches, refresh it
    if (curItem->getObjEntryPtr() && !curItem->getObjEntryPtr()->getObjectFullPath().compare(path.toStdString()))
        this->refreshAtIndex(curIndex);

    // if item is a mount point and its mount path matches, also refresh
    else if (!curItem->mountPoint().compare(path.toStdString()))
        this->refreshAtIndex(curIndex);
}

Kanki::RodsObjEntryPtr RodsObjTreeModel::resolvePathToEntry(const std::string &path)
{
    QModelIndex curIndex;
    RodsObjTreeItem *curItem = this->rootItem;
    std::string curPath;
    boost::char_separator<char> separator("/");
    boost::tokenizer< boost::char_separator<char> > tokens(path, separator);

    // iterate path tokens to find item
    for (boost::tokenizer< boost::char_separator<char> >::iterator iter = tokens.begin();
         iter != tokens.end(); iter++)
    {
        curPath += "/" + *iter;

        for (int i = 0; i < curItem->childCount(); i++)
        {
            RodsObjTreeItem *childItem = curItem->child(i);
            Kanki::RodsObjEntryPtr objEntry = childItem->getObjEntryPtr();

            // if we found an item matching path token, we break from loop
            if (!objEntry || !objEntry->getObjectFullPath().compare(curPath))
            {
                curItem = childItem;
                curIndex = this->index(i, 0, curIndex);

                break;
            }
        }
    }

    return (curItem->getObjEntryPtr());
}


Qt::DropActions RodsObjTreeModel::supportedDropActions() const
{
    // we support copy and move actions
    return (Qt::CopyAction | Qt::MoveAction);
}

bool RodsObjTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                    int row, int column, const QModelIndex &parent)
{
    // check if we are to go forward with the drop
    if (parent.isValid() && this->canDropMimeData(data, action, row, column, parent))
    {
        if (data->hasFormat("application/vnd.kanki.drag"))
        {
            // drop destination (parent item)
            RodsObjTreeItem *parentItem = static_cast<RodsObjTreeItem*>(parent.internalPointer());
            std::string destColl;

            if (parentItem->getObjEntryPtr())
                destColl = parentItem->getObjEntryPtr()->getObjectFullPath();

            else
                destColl = parentItem->mountPoint();

            // encoded drop items
            QByteArray encoded = data->data("application/vnd.kanki.drag");
            QDataStream encodedStream(&encoded, QIODevice::ReadOnly);
            QStringList objPathList;

            std::cout << __FUNCTION__ << ": received application internal drop" << std::endl << std::flush;

            while (!encodedStream.atEnd())
            {
                QString objPath;

                encodedStream >> objPath;
                objPathList << objPath;

                if (objPath.length())
                {
                    std::string objPathStr = objPath.toStdString();
                    std::string srcColl = objPathStr.substr(0, objPathStr.find_last_of('/'));

                    // we move only if src and dest differ
                    if (srcColl.compare(destColl))
                    {
                        Kanki::RodsObjEntryPtr sourceEntryPtr = this->resolvePathToEntry(objPath.toStdString());

                        // if we have a valid object to move
                        if (sourceEntryPtr)
                        {
                            int status = 0;

                            if (status = this->rodsConn->moveObjToColl(sourceEntryPtr, destColl))
                            {
                                // report error
                            }

                            else {
                                this->refreshAtPath(srcColl.c_str());
                                this->refreshAtPath(destColl.c_str());

                                return (true);
                            }
                        }
                    }
                }
            }
        }
    }

    // by default we didn't drop anything
    return (false);
}

bool RodsObjTreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action,
                                       int row, int column, const QModelIndex &parent) const
{
    (void)action;
    (void)row;
    (void)column;
    (void)parent;

    // we handle drops from the application itself
    if (data->hasFormat("application/vnd.kanki.drag"))
    {
        return (true);
    }

    // we deny dropping otherwise
    return (false);
}

QMimeData* RodsObjTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *dragData = new QMimeData();
    QByteArray encoded;

    // we encode thru a stream for convenience
    QDataStream encodeStream(&encoded, QIODevice::WriteOnly);

    // handle multiple drop items
    for (QModelIndexList::const_iterator i = indexes.begin(); i != indexes.end(); i++)
    {
        QModelIndex index = *i;

        // sanity check(s)
        if (index.isValid() && index.column() == 0)
        {
            RodsObjTreeItem *item = static_cast<RodsObjTreeItem*>(index.internalPointer());
            Kanki::RodsObjEntryPtr entry = item->getObjEntryPtr();

            // for internal drops we simply encode the rods object path
            if (entry)
            {
                std::cout << __FUNCTION__ << ": encoding " << entry->getObjectFullPath() << std::endl << std::flush;
                encodeStream << QString(entry->getObjectFullPath().c_str());
            }
        }
    }

    dragData->setData("application/vnd.kanki.drag", encoded);
    return (dragData);
}

void RodsObjTreeModel::addMountPoint(const std::string &path)
{
    // setup new mount point item
    RodsObjTreeItem *mountPoint = new RodsObjTreeItem(this->rootItem);
    mountPoint->setMountPoint(path);

    // signal view(s) that model is to be reset and commit
    beginResetModel();
    rootItem->appendChild(mountPoint);
    endResetModel();
}
