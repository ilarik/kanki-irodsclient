/**
 * @file rodsmetadatawindow.h
 * @brief Definition of class RodsMetadataWindow
 *
 * The RodsMetadataWindow class extends the Qt window class QMainWindow
 * and implements an iRODS object metadata manager window.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSMETADATAWINDOW_H
#define RODSMETADATAWINDOW_H

// C++ standard library headers
#include <iostream>
#include <string>
#include <vector>

// Qt framework headers
#include <QMainWindow>
#include <QWindow>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QDataWidgetMapper>
#include <QItemSelection>
#include <QDialog>
#include <QInputDialog>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsobjentry.h"
#include "rodsobjmetadata.h"

// application headers
#include "rodsmetadatamodel.h"
#include "rodsmetadataitem.h"
#include "rodsmetadataschema.h"

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsMetadataWindow;
}

class RodsMetadataWindow : public QMainWindow
{
    Q_OBJECT

public:

    // Constructor requires the conn pointer and a managed pointer to a rods object entry.
    explicit RodsMetadataWindow(Kanki::RodsConnection *theConn, Kanki::RodsObjEntryPtr theObjEntry, QWidget *parent = 0);

    ~RodsMetadataWindow();

public slots:

    // Qt slot for metadata tree selection change event handling. The metadata tree view signals the
    // metadata editor window to handle the new selection.
    void metaTreeSelectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);

signals:

    // Qt signal to unregister metadata window for deletion.
    void unregister(const std::string &objPath);

protected:

    // Qt event handler for the window close event. Invokes the unregister signal.
    void closeEvent(QCloseEvent *event);


private slots:

    // connected to the refresh action triggered signal
    void on_actionRefresh_triggered();

    // connected to the add attribute action triggered signal
    void on_actionAddAttribute_triggered();

    // connected to the remove action triggered signal
    void on_actionRemove_Attribute_triggered();

private:

    // refreshes the metadata editor
    void refresh();

    // invokes the add attribute gui
    void addAttribute();

    // invokes the remove attribute operation
    void removeAttribute();

    // reports errors to the user via a Qt message box
    void reportError(QString msg, int status) const;

    // instance of Qt UI compiler generated UI
    Ui::RodsMetadataWindow *ui;

    // a Qt mapper for mapping editor window fields to the metadata model.
    QDataWidgetMapper *metaMapper;

    // our metadata schema instance
    RodsMetadataSchema *schema;

    // pointer to rods connection object
    Kanki::RodsConnection *conn;

    // managed pointer to a rods object entry
    Kanki::RodsObjEntryPtr objEntry;

    // Kanki rods object metadata container instance
    Kanki::RodsObjMetadata *metadata;
};

#endif // RODSMETADATAWINDOW_H
