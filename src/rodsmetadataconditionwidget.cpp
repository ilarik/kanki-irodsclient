/**
 * @file rodsmetadataconditionwidget.h
 * @brief Implementation of RodsMetadataConditionWidget class
 *
 * The RodsMetadataConditionWidget class extends the class RodsConditionWidget and
 * implements a widget for entering a metadata condition for a GenQuery.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMetadataConditionWidget header
#include "rodsmetadataconditionwidget.h"

RodsMetadataConditionWidget::RodsMetadataConditionWidget(objType_t type, const std::map<std::string, std::string> &attrs,
                                                         QWidget *parent) :
     RodsStringConditionWidget(type == DATA_OBJ_T ? COL_META_DATA_ATTR_VALUE : COL_META_COLL_ATTR_VALUE,
                               type == DATA_OBJ_T ? "Data Object Metadata Attribute" : "Collection Metadata Attribute", parent)
{
    this->objType = type;
    this->attrMap = attrs;

    this->attrSel = new QComboBox(this);

    for (std::map<std::string,std::string>::iterator i = this->attrMap.begin();
         i != this->attrMap.end(); i++)
    {
        std::string attrName = i->first;
        std::string attrLabel = i->second;
        std::string label;

        if (attrName.compare(attrLabel))
            label = attrName + " (" + attrLabel + ")";

        else
            label = attrName;

        this->attrSel->addItem(label.c_str(), attrName.c_str());
    }

    this->layout->insertWidget (2, this->attrSel);
}

RodsMetadataConditionWidget::~RodsMetadataConditionWidget()
{
    delete (this->attrSel);
}

void RodsMetadataConditionWidget::evaluateConds(Kanki::RodsGenQuery *query)
{
    // attribute name condition
    std::string attrName = this->attrSel->currentData().toString().toStdString();

    if (this->objType == DATA_OBJ_T)
        query->addQueryCondition(COL_META_DATA_ATTR_NAME, Kanki::RodsGenQuery::isEqual, attrName);

    else if (this->objType == COLL_OBJ_T)
        query->addQueryCondition(COL_META_COLL_ATTR_NAME, Kanki::RodsGenQuery::isEqual, attrName);

    // attribute value condition
    RodsStringConditionWidget::evaluateConds(query);
}
