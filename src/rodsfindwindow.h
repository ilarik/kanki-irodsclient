/**
 * @file rodsfindwindow.h
 * @brief Definition of RodsFindWindow class
 *
 * The RodsFindWindow class extends the Qt window class QMainWindow and
 * implements an iRODS find window UI.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSFINDWINDOW_H
#define RODSFINDWINDOW_H

// C++ standard library headers
#include <chrono>
#include <vector>

// Qt framework headers
#include <QMainWindow>
#include <QInputDialog>
#include <QIcon>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "_rodsgenquery.h"

// application headers
#include "rodsmetadataschema.h"
#include "rodsconditionwidget.h"
#include "rodsstringconditionwidget.h"
#include "rodsdateconditionwidget.h"
#include "rodsmetadataconditionwidget.h"

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsFindWindow;
}

class RodsFindWindow : public QMainWindow
{
    Q_OBJECT

public:

    //
    enum SearchConditions { DataObjName, DataObjType, DataObjCreated, DataObjModified, DataObjMetadata,
                            DataObjChksum, CollName, CollCreated, CollModified, CollMetadata };

    //
    explicit RodsFindWindow(Kanki::RodsConnection *rodsConn, QWidget *parent = 0);

    //
    ~RodsFindWindow();

signals:

    // Qt signal for unregistering the find window for deletion of the object.
    void unregister();

    // Qt signal for selecting an object in the grid browser, signals out path
    void selectObj(QString objPath);

public slots:

    // Qt slot for requesting to add a condition
    void addCondition();

    // Qt slot for requesting to execute
    void executeSearch();

    // Qt slot for resetting the find window
    void resetConditions();

    // Qt slot for unregistering a condition widget
    void unregisterCondWidget(RodsConditionWidget *ptr);

    // Qt slot for invoking an object selection in the grid browser
    void resultDoubleClicked(const QModelIndex &index);

protected:

    // Qt close window event handler, invokes unregister signal.
    void closeEvent(QCloseEvent *event);

private:

    // refresh available data object attributes from iCAT
    void refreshMetadataAttrs();

    // instance of Qt UI compiler generated UI
    Ui::RodsFindWindow *ui;

    // icon for a search result
    QIcon dataIcon, collIcon;

    // our connection object
    Kanki::RodsConnection *conn;

    // our schema instance
    RodsMetadataSchema *schema;

    // container for condition widgets (to be added dynamically)
    std::vector<RodsConditionWidget*> condWidgets;

    // container for available metadata attributes
    std::map<std::string, std::string> attrMap;
};

#endif // RODSFINDWINDOW_H
