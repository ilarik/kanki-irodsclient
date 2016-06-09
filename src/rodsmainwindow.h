/**
 * @file rodsmainwindow.h
 * @brief Definition of class RodsMainWindow
 *
 * The RodsMainWindow class extends the Qt window class QMainWindow
 * and implements an iRODS Grid Browser window.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSMAINWINDOW_H
#define RODSMAINWINDOW_H

// C++ standard library headers
#include <iostream>

// Qt framework headers
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAction>
#include <QList>
#include <QProgressBar>
#include <QProgressDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QThread>
#include <QStyleFactory>
#include <QPalette>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsobjentry.h"
#include "_rodsgenquery.h"

// application headers
#include "rodsmetadatawindow.h"
#include "rodsqueuewindow.h"
#include "rodsconnectthread.h"
#include "rodsuploadthread.h"
#include "rodsdownloadthread.h"
#include "rodsobjtreemodel.h"
#include "rodstransferwindow.h"
#include "rodserrorlogwindow.h"
#include "rodsfindwindow.h"
#include "version.h"

// Qt UI compiler namespace for generated classes
namespace Ui {
class RodsMainWindow;
}

class RodsMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    // Constructor has only the parent Qt widget pointer as argument.
    explicit RodsMainWindow(QWidget *parent = 0);

    ~RodsMainWindow();

    // Qt event handler for a drag enter event. Overrides superclass virtual function.
    void dragEnterEvent(QDragEnterEvent *event);

    // Qt event handler for a drop event. Overrides superclass virtual function.
    void dropEvent(QDropEvent *event);

protected:

    // Qt event handler for a window close event. Overrides superclass virtual function.
    void closeEvent(QCloseEvent *event);

public slots:

    // Qt slot for entering into a modal state with a progress dialog with a given message
    // text, initial value and max value.
    void startModalProgressDialog(QString text, int value, int maxValue);

    // Qt slot for updating the modal progress dialog state with a message text and value.
    void setProgress(QString text, int currentValue);

    // Qt slot for setting the modal progress dialog into a marquee mode.
    void setProgressMarquee(QString text);

    // Qt slot for exiting from the modal progress dialog state.
    void endModalProgressDialog();

    // Qt slot for reporting an error with a message string, error string and error code.
    void reportError(QString msgStr, QString errorStr, int errorCode);

    // Qt slot for setting the Kanki rods connection object for the grid browser window.
    void setConnection(Kanki::RodsConnection *newConn);

    // Qt slot for unregistering a metadata editor window from the grid browser window.
    void unregisterMetadataWindow(std::string objPath);

    // Qt slot for unregistering the queue stat window from the grid browser window.
    void unregisterQueueWindow();

    // Qt slot for entering the grid browser window into a connected state.
    void enterConnectedState();

    // Qt slot for entering the grid browser window into a disconnected state.
    void enterDisconnectedState();

    // Qt slot for for invoking the mount irods path gui operation.
    void mountPath();

    // Qt slot for invoking the connect to irods gui operation.
    void doRodsConnect();

    // Qt slot for invoking the disconnect gui operation.
    void doRodsDisconnect();

    // Qt slot for invoking the open metadata editor gui operation.
    void doMetadataEditorOpen();

    // Qt slot for invoking the upload gui operation.
    void doUpload(bool uploadDirectory = false);

    // Qt slot for invoking the refresh gui operation.
    void doRefreshTreeView(QModelIndex atIndex = QModelIndex());

    // Qt slot for invoking the create collection gui operation.
    void doCreateCollection();

    // Qt slot for invoking the delete object gui operation.
    void doDelete();

    // Qt slot for invoking the open queue stat window gui operation.
    void doQueueStatOpen();

    // Qt slot for invoking the download gui operation.
    void doDownload();

    // Qt slot for showing an about dialog
    void showAbout();

    // Qt slot for refreshing available storage resources from the iRODS grid.
    void refreshResources();

    // Qt slot for opening error log window owned by the grid browser.
    void openErrorLog();

    // Qt slot for opening find window owned by the grid browser.
    void openFindWindow();

    // Qt slot for unregistering a find window to be destroyed.
    void unregisterFindWindow();

    // Qt slot for repoting error count to grid browser window ui.
    void errorsReported(unsigned int errorCount);

    // Qt slot for selecting an object in the browser window.
    void selectRodsObject(QString objPath);

private slots:

    // qt slot which connects to connect action triggered signal
    void on_actionConnect_triggered();

    // qt slot which connects to disconnect action triggered signal
    void on_actionDisconnect_triggered();

    // qt slot which connects to upload action triggered signal
    void on_actionUpload_triggered();

    // qt slot which connects to delete action triggered signal
    void on_actionDelete_triggered();

    // qt slot which connects to refresh action triggered signal
    void on_actionRefresh_triggered();

    // qt slot which connects to new collection action triggered signal
    void on_actionNewColl_triggered();

    // qt slot which connects to open metadata editor action triggered signal
    void on_actionMetadata_triggered();

    // qt slot which connects to open queue window action triggered signal
    void on_actionQueueStat_triggered();

    // qt slot which connects to download action triggered signal
    void on_actionDownload_triggered();

    // qt slot which connects to mount action triggered signal
    void on_actionMount_triggered();

    // qt slot which connects to view size slider value change signal
    void on_viewSize_valueChanged(int value);

    // qt slot which connects to verify checksum checkbox toggled signal
    void on_verifyChecksum_toggled(bool checked);

    // qt slot which connects to allow overwrite checkbox toggled signal
    void on_allowOverwrite_toggled(bool checked);

    // qt slot which connects to about action triggered signal
    void on_actionAbout_triggered();

    // qt slot which connects to upload directory triggered signal
    void on_actionUploadDirectory_triggered();

    // qt slot which connects to storage resource list activated signal
    void on_storageResc_activated(const QString &arg1);

    // qt slot which connects to error log open triggered signal
    void on_actionErrorLog_triggered();

    // qt slot which connects to find action triggered signal
    void on_actionFind_triggered();

signals:

    // signal for requesting object model refresh
    void refreshObjectModelAtIndex(QModelIndex index);

    // signal for adding an error to log
    void logError(QString msgStr, QString errorStr, int errorCode);

private:

    // gets the current user selected rods path from the gui
    std::string getCurrentRodsCollPath();

    // gets the current rods object tree model index
    QModelIndex getCurrentRodsObjIndex();

    // instance of Qt UI compiler generated UI
    Ui::RodsMainWindow *ui;

    // our modal progress dialog
    QProgressDialog *progress;

    // our queue window instance
    RodsQueueWindow *queueWindow;

    // our error log window instance
    RodsErrorLogWindow *errorLogWindow;

    // our find window instance
    RodsFindWindow *findWindow;

    // a container of metadata window object pointers
    std::map<std::string, RodsMetadataWindow*> metaEditors;

    // our object tree model instance
    RodsObjTreeModel *model;

    // pointer to rods connection object
    Kanki::RodsConnection *conn;

    // settings from the gui
    bool verifyChecksum, allowOverwrite;

    // current selected resource
    std::string currentResc;
};

#endif // RODSMAINWINDOW_H
