/**
 * @file rodsgenquery.h
 * @brief Definition of Kanki library class RodsGenQuery
 *
 * The Kanki class RodsGenQuery provides an interface for creating
 * an iRODS GenQuery and processing its results.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSGENQUERY_H
#define RODSGENQUERY_H

// C++ standard library headers
#include <iostream>
#include <string>
#include <vector>
#include <map>

// iRODS client library headers
#include "rodsClient.h"
#include "rodsPath.h"

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"

namespace Kanki {

class RodsGenQuery
{
public:

    // Class local public enumerated type for query condition operators.
    enum CondOpr { isEqual, isNotEqual, isLess, isLessOrEqual, isGreater, isGreaterOrEqual, isLike };

    // Constructor receives only a pointer to a Kanki connection object for GenQuery execution.
    RodsGenQuery(Kanki::RodsConnection *theConn);

    // Adds a rods attribute into the GenQuery object for querying.
    void addQueryAttribute(int rodsAttr);

    // Adds a query condition to the GenQuery with a string value.
    void addQueryCondition(int rodsAttr, RodsGenQuery::CondOpr rodsCondOpr, const std::string &valStr);

    // Adds a query condition to the GenQuery with an integer value (converted to string).
    void addQueryCondition(int rodsAttr, RodsGenQuery::CondOpr rodsCondOpr, int val);

    // Executes the iRODS GenQuery and fetches the results into local storage.
    int execute();

    // Resets the GenQuery object.
    void reset();

    // Returns the attribute count in the GenQuery.
    int attrCount() const;

    // Interface for querying whether the GenQuery object has a certain attribute.
    bool hasAttribute(int rodsAttr) const;

    // Interface for querying the index of a specified attribute in the GenQuery.
    int attributeIndex(int rodsAttr) const;

    // Interface for querying the attribute id at the specific index.
    int attributeAtIndex(int attrIndex) const;

    // Interface for retrieving the query results for a given attribute rods api code.
    std::vector<std::string> getResultSetForAttr(int rodsAttr) const;

    // Interface for getting a copy of a GenQuery result set corresponding to the attribute index.
    std::vector<std::string> getResultSet(unsigned int attrIndex) const;

    // Interface for getting a copy of the GenQuery entire result set.
    std::vector< std::vector<std::string> > getResultSet() const;

private:

    // Class local private data structure for a GenQuery condition in the format
    // attribute id, operator code, value str.
    struct Condition {
        Condition(int theAttr, RodsGenQuery::CondOpr theCondOpr, std::string theValStr) {
            attr = theAttr;
            condOpr = theCondOpr;
            valStr = theValStr;
        }

        int attr;
        CondOpr condOpr;
        std::string valStr;
    };

    // pointer to Kanki rods connection object
    Kanki::RodsConnection *conn;

    // container for rods query attribute codes
    std::vector<int> queryAttrs;

    // container for query conditions
    std::vector<RodsGenQuery::Condition> queryConds;

    // hashtable container for query results
    std::map< int, std::vector<std::string> > resultTable;
};

} // namespace Kanki

#endif // RODSGENQUERY_H
