#include "rodsobjlinkviz.h"

RodsObjLinkViz::RodsObjLinkViz(Kanki::RodsConnection *conn)
{
    this->connPtr = conn;
    this->layout = new QVBoxLayout(this);

    Kanki::RodsGenQuery query(this->connPtr);

    query.addQueryAttribute(COL_DATA_NAME);
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
        std::vector<std::string> sources = query.getResultSetForAttr(COL_DATA_NAME);
        std::vector<std::string> dests = query.getResultSetForAttr(COL_META_DATA_ATTR_VALUE);

        // vertex "array"
        std::map<std::string, vtkIdType> vertices;

        // vertex labels
        vtkSmartPointer<vtkStringArray> labels =
                vtkSmartPointer<vtkStringArray>::New();
        labels->SetName("labels");
        labels->SetNumberOfComponents(1);

        // vtk graph
        vtkSmartPointer<vtkMutableUndirectedGraph> g =
                vtkSmartPointer<vtkMutableUndirectedGraph>::New();

        for (unsigned int i = 0; i < sources.size(); i++)
        {
            if (vertices.find(sources.at(i)) == vertices.end())
            {
                std::cout << __FUNCTION__ << ": adding vertex for '" << sources.at(i) << "'"
                          << std::endl << std::flush;

                vertices[sources.at(i)] = g->AddVertex();
                labels->InsertNextValue(sources.at(i).c_str());
            }
        }

        for (unsigned int i = 0; i < dests.size(); i++)
        {
            Kanki::RodsGenQuery targetQuery(this->connPtr);

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
                    std::string name = nameRes.at(0);
                    std::string coll = collRes.at(0);

                    // if add vertex for target if not yet there
                    if (vertices.find(name) == vertices.end())
                    {
                        vertices[name] = g->AddVertex();
                        labels->InsertNextValue(name.c_str());

                        std::cout << __FUNCTION__ << ": adding vertex for '" << name << "'"
                                  << std::endl << std::flush;
                    }


                    // add edge from source to target
                    g->AddEdge(vertices[sources.at(i)], vertices[name]);

                    std::cout << __FUNCTION__ << ": adding edge from '" << sources.at(i) << " to "
                              << "'" << name << "'" << std::endl << std::flush;
                }
            }
        }

        g->GetVertexData()->AddArray(labels);

        vtkSmartPointer<vtkCircularLayoutStrategy> circularLayoutStrategy =
            vtkSmartPointer<vtkCircularLayoutStrategy>::New();

        vtkSmartPointer<vtkGraphLayoutView> graphLayoutView =
            vtkSmartPointer<vtkGraphLayoutView>::New();

        graphLayoutView->SetLayoutStrategy(circularLayoutStrategy);
        graphLayoutView->AddRepresentationFromInput(g);
        graphLayoutView->SetVertexLabelVisibility(true);
      //  graphLayoutView->SetVertexLabelArrayName("labels");
        graphLayoutView->Update();
        graphLayoutView->ResetCamera();
        graphLayoutView->Render();

        // VTK/Qt integration
        this->view = new QVTKWidget();
        this->view->GetRenderWindow()->AddRenderer(graphLayoutView->GetRenderer());
        this->layout->addWidget(this->view);
    }
}

