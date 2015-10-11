#include "rodsobjlinkviz.h"

RodsObjLinkViz::RodsObjLinkViz(Kanki::RodsConnection *conn)
{
    this->connPtr = conn;
    this->layout = new QVBoxLayout(this);
    this->resize(720,480);
    this->setWindowTitle("iRODS Object Relations Visualizer");

    Kanki::RodsGenQuery query(this->connPtr);

    query.addQueryAttribute(COL_DATA_NAME);
    query.addQueryAttribute(COL_COLL_NAME);
    query.addQueryAttribute(COL_META_DATA_ATTR_VALUE);

    query.addQueryCondition(COL_META_DATA_ATTR_NAME, Kanki::RodsGenQuery::isEqual,
                            "fi.jyu.irods.references");

    int status = 0;

    if ((status = query.execute()) < 0)
    {
        this->layout->addWidget(new QLabel("GenQuery Error!"));
        return;
    }

    else {
        std::vector<std::string> names = query.getResultSetForAttr(COL_DATA_NAME);
        std::vector<std::string> colls = query.getResultSetForAttr(COL_COLL_NAME);
        std::vector<std::string> dests = query.getResultSetForAttr(COL_META_DATA_ATTR_VALUE);

        // vertex "array"
        std::map<std::string, vtkIdType> vertices;

        // vertex labels
        vtkSmartPointer<vtkStringArray> labels =
                vtkSmartPointer<vtkStringArray>::New();

        labels->SetName("labels");
        labels->SetNumberOfComponents(1);

        // edge colors
        vtkSmartPointer<vtkIntArray> edgeColors =
                vtkSmartPointer<vtkIntArray>::New();

        edgeColors->SetNumberOfComponents(1);
        edgeColors->SetName("Color");

        // color lookup table
        vtkSmartPointer<vtkLookupTable> lookupTable =
                vtkSmartPointer<vtkLookupTable>::New();

        lookupTable->SetNumberOfTableValues(2);
        lookupTable->SetTableValue(0, 0.0, 0.0, 0.0);
        lookupTable->SetTableValue(1, 0.5, 1.0, 1.0);

        lookupTable->Build();

        // vtk graph
        vtkSmartPointer<vtkMutableDirectedGraph> g =
                vtkSmartPointer<vtkMutableDirectedGraph>::New();

        for (unsigned int i = 0; i < names.size(); i++)
        {
            std::string objPath = colls.at(i) + "/" + names.at(i);

            if (vertices.find(objPath) == vertices.end())
            {
                std::cout << __FUNCTION__ << ": adding vertex for '" << objPath << "'"
                          << std::endl << std::flush;

                vertices[objPath] = g->AddVertex();
                labels->InsertNextValue(objPath.c_str());
            }
        }

        for (unsigned int i = 0; i < dests.size(); i++)
        {
            Kanki::RodsGenQuery targetQuery(this->connPtr);
            std::string srcPath = colls.at(i) + "/" + names.at(i);

            targetQuery.addQueryAttribute(COL_DATA_NAME);
            targetQuery.addQueryAttribute(COL_COLL_NAME);

            targetQuery.addQueryCondition(COL_META_DATA_ATTR_NAME, Kanki::RodsGenQuery::isEqual,
                                          "fi.jyu.irods.identifier");
            targetQuery.addQueryCondition(COL_META_DATA_ATTR_VALUE, Kanki::RodsGenQuery::isEqual,
                                          dests.at(i));

            if ((status = targetQuery.execute()) < 0)
            {
                this->layout->addWidget(new QLabel("GenQuery Error!"));
                return;
            }

            else {
                std::vector<std::string> nameRes = targetQuery.getResultSetForAttr(COL_DATA_NAME);
                std::vector<std::string> collRes = targetQuery.getResultSetForAttr(COL_COLL_NAME);

                if (nameRes.size() == 1 && collRes.size() == 1)
                {
                    std::string objPath = collRes.at(0) + "/" + nameRes.at(0);

                    // if add vertex for target if not yet there
                    if (vertices.find(objPath) == vertices.end())
                    {
                        vertices[objPath] = g->AddVertex();
                        labels->InsertNextValue(objPath.c_str());

                        std::cout << __FUNCTION__ << ": adding vertex for '" << objPath << "'"
                                  << std::endl << std::flush;
                    }

                    // add edge from source to target
                    g->AddEdge(vertices[srcPath], vertices[objPath]);
                    edgeColors->InsertNextValue(0);

                    std::cout << __FUNCTION__ << ": adding edge from '" << srcPath << " to "
                              << "'" << objPath << "'" << std::endl << std::flush;
                }
            }
        }

        g->GetVertexData()->AddArray(labels);
        g->GetEdgeData()->AddArray(edgeColors);

        vtkSmartPointer<vtkGraphLayoutView> graphLayoutView =
                vtkSmartPointer<vtkGraphLayoutView>::New();

        int randomSeed = 100;

        vtkSmartPointer<vtkFast2DLayoutStrategy> fast2D =
                vtkSmartPointer<vtkFast2DLayoutStrategy>::New();

        fast2D->SetRandomSeed(randomSeed);

        vtkSmartPointer<vtkViewTheme> theme =
                vtkSmartPointer<vtkViewTheme>::New();

        theme->SetCellLookupTable(lookupTable);
        graphLayoutView->SetEdgeColorArrayName("Color");
        graphLayoutView->ColorEdgesOn();
        graphLayoutView->SetLayoutStrategy(fast2D);
        graphLayoutView->AddRepresentationFromInput(g);

        graphLayoutView->SetVertexLabelVisibility(true);
        graphLayoutView->SetVertexLabelArrayName("labels");
        graphLayoutView->SetVertexLabelFontSize(12);
        graphLayoutView->SetGlyphType(VTK_CIRCLE_GLYPH);

        graphLayoutView->ApplyViewTheme(theme);
        graphLayoutView->Update();
        graphLayoutView->ResetCamera();
        graphLayoutView->Render();

        graphLayoutView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

        // VTK/Qt integration
        this->view = new QVTKWidget();
        this->view->GetRenderWindow()->AddRenderer(graphLayoutView->GetRenderer());
        this->view->GetRenderWindow()->SetAAFrames(4);
        this->layout->addWidget(this->view);
    }
}
