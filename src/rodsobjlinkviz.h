#ifndef RODSOBJLINKVIZ_H
#define RODSOBJLINKVIZ_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include "rodsconnection.h"
#include "_rodsgenquery.h"

#include <QVTKWidget.h>

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkGraphLayoutView.h>
#include <vtkStringArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkCircularLayoutStrategy.h>

class RodsObjLinkViz : public QWidget
{
public:
    RodsObjLinkViz(Kanki::RodsConnection *conn);

signals:

public slots:

private:

    //
    Kanki::RodsConnection *connPtr;

    // Qt UI layout instance
    QVBoxLayout *layout;

    //
    QVTKWidget *view;
};

#endif // RODSOBJLINKVIZ_H
