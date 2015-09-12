/**
 * @file rodsmetadataschema.cpp
 * @brief Implementation of class RodsMetadataSchema
 *
 * The RodsMetadataSchema class provides an interface for iRODS metadata
 * schema management with namespaces and attribute definitions.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMetadataSchema header
#include "rodsmetadataschema.h"

RodsMetadataSchema::RodsMetadataSchema(QString filePath)
{
    // if no schema file path specified, use platform default
    if (!filePath.length())
        filePath = SCHEMA_PATH;

    QFile schemaFile(filePath);
    QDomDocument schema;

    // TODO: error handling, refactor away from the constructor...
    if (!schemaFile.open(QIODevice::ReadOnly) || !schema.setContent(&schemaFile))
        return;

    // initialize conversion table
    this->nsConv["."] = "Global Namespace";

    // process namespaces
    QDomNodeList namespaces = schema.elementsByTagName("irods:namespace");

    for (int i = 0; i < namespaces.size(); i++)
    {
        QDomNode ns = namespaces.item(i);
        QDomNamedNodeMap nsAttr = ns.attributes();

        // for now, we use prefix and label from the schema
        QString nsPrefix = nsAttr.namedItem("prefix").nodeValue();
        QString nsLabel = nsAttr.namedItem("label").nodeValue();

        this->nsConv[nsPrefix.toStdString()] = nsLabel.toStdString();

        // process namespace attributes
        QDomNodeList attrElemList = ns.childNodes();

        for (int j = 0; j < attrElemList.size(); j++)
        {
            QDomNode attrElem = attrElemList.item(j);

            // we only process irods:attribute nodes
            if (!attrElem.nodeName().compare("irods:attribute"))
            {
                // attribute name and label are read from the DOM
                QDomNamedNodeMap attrElemAttrs = attrElem.attributes();
                QString attrName = nsPrefix + attrElemAttrs.namedItem("name").nodeValue();

                QDomElement attrLabelElem = attrElem.firstChildElement("irods:label");
                QString attrLabel = attrLabelElem.text();

                this->attrConv[attrName.toStdString()] = attrLabel.toStdString();

                // process attribute subnodes
                QDomNodeList attrSubNodes = attrElem.childNodes();

                for (int k = 0; k < attrSubNodes.size(); k++)
                {
                    QDomNode node = attrSubNodes.item(k);

                    // for now, we only process irods:displayFilter nodes
                    if (!node.nodeName().compare("irods:displayFilter"))
                    {
                        QDomNamedNodeMap filterAttrs = node.attributes();
                        if (!filterAttrs.namedItem("type").nodeValue().compare("regExp"))
                        {
                            QDomElement regExpRule = node.firstChildElement("irods:regExpRule");
                            QDomElement regExpFilter = node.firstChildElement("irods:regExpFilter");

                            // if we have a proper rule
                            if (regExpRule.text().length() > 0)
                            {
                                regExpRules[attrName.toStdString()] = regExpRule.text().toStdString();
                                regExpFilters[attrName.toStdString()] = regExpFilter.text().toStdString();
                            }
                        }
                    }
                }
            }
        }
    }
}

std::string RodsMetadataSchema::translateName(const std::string &name) const
{
    // try to find conversion from translate table for namespaces
    if (this->nsConv.find(name) != this->nsConv.end())
        return (this->nsConv.at(name));

    // try to find conversion from translate table for attributes
    if (this->attrConv.find(name) != this->attrConv.end())
        return (this->attrConv.at(name));

    // if conversion fails, return it unchanged
    return (name);
}

std::string RodsMetadataSchema::filterValue(const std::string &name, const std::string &value) const
{
    QString valueStr = value.c_str();

    // try to do regexp rule translation
    if (this->regExpRules.find(name) != regExpRules.end())
    {
        QRegularExpression re(this->regExpRules.at(name).c_str());
        valueStr.replace(re, regExpFilters.at(name).c_str());
    }

    return (valueStr.toStdString());
}

std::vector<std::string> RodsMetadataSchema::getAttributes() const
{
    std::vector<std::string> attrVector;

    for (std::map<std::string, std::string>::const_iterator i = this->attrConv.begin(); i != this->attrConv.end(); i++)
    {
        std::string attrName = i->first;
        attrVector.push_back(attrName);
    }

    return (attrVector);
}

std::vector<std::string> RodsMetadataSchema::getNamespaces() const
{
    std::vector<std::string> nsVector;

    for (std::map<std::string, std::string>::const_iterator i = this->nsConv.begin(); i != this->nsConv.end(); i++)
    {
        std::string ns = i->first;

        // we push only non-default namespaces out
        if (ns.compare("."))
            nsVector.push_back(ns);
    }

    return (nsVector);
}

QStringList RodsMetadataSchema::getAttributeList() const
{
    QStringList attrList;

    for (std::map<std::string, std::string>::const_iterator i = this->attrConv.begin(); i != this->attrConv.end(); i++)
    {
        std::string attrName = i->first;
        attrList.append(attrName.c_str());
    }

    return attrList;
}
