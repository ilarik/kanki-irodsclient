/**
 * @file rodsmetadataschema.h
 * @brief Definition of class RodsMetadataSchema
 *
 * The RodsMetadataSchema class provides an interface for iRODS metadata
 * schema management with namespaces and attribute definitions.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSMETADATASCHEMA_H
#define RODSMETADATASCHEMA_H

// C++ standard library headers
#include <string>
#include <vector>
#include <map>

// Qt framework headers
#include <QApplication>
#include <QFile>
#include <QtXml/QDomDocument>
#include <QString>
#include <QStringList>
#include <QRegularExpression>

// platform default schema XML file location
#ifdef osx_platform
#define SCHEMA_PATH     QApplication::applicationDirPath() + "/../Resources/schema.xml"
#else
#define SCHEMA_PATH     "/etc/irods/schema.xml"
#endif

class RodsMetadataSchema
{

public:

    // Constructor has an optional argument of file path of the schema XML,
    // for now the schema is loaded on instantiation, if possible (TODO: FIXME).
    RodsMetadataSchema(QString filePath = QString());

    // Interface to query metadata attribute names from the schema.
    std::vector<std::string> getAttributes() const;

    // Interface to query metadata namespace prefixes from the schema.
    std::vector<std::string> getNamespaces() const;

    // Interface to get a Qt string list of attribute names, for UI purposes.
    QStringList getAttributeList() const;

    // Interface for schema name translation, for both attributes and namespaces.
    std::string translateName(const std::string &name) const;

    // Interface for filtering metadata attribute values per schema.
    std::string filterValue(const std::string &name, const std::string &value) const;

private:

    // attribute value translate table
    std::map< std::string, std::string > attrConv;

    // namespace prefix translate table
    std::map< std::string, std::string > nsConv;

    // regexp rule table, has attribute name as key
    std::map< std::string, std::string > regExpRules;

    // regexp filter table, has attribute name as key
    std::map< std::string, std::string > regExpFilters;
};

#endif // RODSMETADATASCHEMA_H
