/**
 * @file rodsgenquery.cpp
 * @brief Implementation of Kanki library class RodsGenQuery
 *
 * The Kanki class RodsGenQuery provides an interface for creating
 * an iRODS GenQuery and processing its results.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsGenQuery header
#include "_rodsgenquery.h"

namespace Kanki {

RodsGenQuery::RodsGenQuery(Kanki::RodsConnection *theConn)
{
    // set connection object pointer
    this->conn = theConn;
}

void RodsGenQuery::addQueryAttribute(int rodsAttr)
{
    // if we already don't have the said attribute
    if (!this->hasAttribute(rodsAttr))
    {
        // push attribute to vector of query attributes
        this->queryAttrs.push_back(rodsAttr);
    }
}

void RodsGenQuery::addQueryCondition(int rodsAttr, RodsGenQuery::CondOpr rodsCondOpr, const std::string &valStr)
{
    // make new condition struct and push back of vector
    this->queryConds.push_back(RodsGenQuery::Condition(rodsAttr, rodsCondOpr, valStr));
}

int RodsGenQuery::execute()
{
    genQueryInp_t queryInput;
    genQueryOut_t *queryOutput;
    int status = 0, prevStatus = 0;

    // if we have a previous query result set, flush previous results
    if (this->resultTable.size())
        this->resultTable.clear();

    // zero rods api data structures
    memset(&queryInput, 0, sizeof(genQueryInp_t));
    queryOutput = NULL;

    // set rods api select array sizes
    queryInput.selectInp.len = this->queryAttrs.size();
    queryInput.maxRows = 100;

    // allocate new arrays for rods api
    queryInput.selectInp.inx = new int[this->queryAttrs.size()];
    queryInput.selectInp.value = new int[this->queryAttrs.size()];

    // build rods api select arrays
    for (unsigned int i = 0; i < this->queryAttrs.size(); i++)
    {
        queryInput.selectInp.inx[i] = this->queryAttrs.at(i);
        queryInput.selectInp.value[i] = 0;
    }

    // set rods api condition array sizes
    queryInput.sqlCondInp.len = this->queryConds.size();

    // allocate new arrays for rods api
    queryInput.sqlCondInp.inx = new int[this->queryConds.size()];
    queryInput.sqlCondInp.value = new char*[this->queryConds.size()];

    // build rods api cond arrays
    for (unsigned int i = 0; i < this->queryConds.size(); i++)
    {
        RodsGenQuery::Condition cond = this->queryConds.at(i);
        std::string condStr;

        // build conditions string depending on condition operator
        if (cond.condOpr == RodsGenQuery::isEqual)
            condStr += "=";
        else if (cond.condOpr == RodsGenQuery::isNotEqual)
            condStr += "!=";
        else if (cond.condOpr == RodsGenQuery::isLess)
            condStr += "<";
        else if (cond.condOpr == RodsGenQuery::isLessOrEqual)
            condStr += "<=";
        else if (cond.condOpr == RodsGenQuery::isGreater)
            condStr += ">";
        else if (cond.condOpr == RodsGenQuery::isGreaterOrEqual)
            condStr += ">=";
        else if (cond.condOpr == RodsGenQuery::isLike)
            condStr += "like ";

        // append condition value into expression
        condStr += "'" + cond.valStr + "'";

        // insert into rods api arrays
        queryInput.sqlCondInp.inx[i] = cond.attr;
        queryInput.sqlCondInp.value[i] = strdup(condStr.c_str());
    }

    // lock rods connection mutex
    this->conn->mutexLock();

    // try to execute a generic query
    if (!(status = rcGenQuery(this->conn->commPtr(), &queryInput, &queryOutput)))
    {
        // on success, initialize result buffer vectors
        for (unsigned int i = 0; i < this->queryAttrs.size(); i++)
            this->resultTable[this->queryAttrs.at(i)] = std::vector<std::string>();

        // iterate while there are results to process
        do {
            for (int i = 0; i < queryOutput->rowCnt; i++)
            {
                for (int j = 0; j < queryOutput->attriCnt; j++)
                {
                    // pointer arithmetic for attribute i result entry j
                    char *resultPtr = queryOutput->sqlResult[j].value;
                    resultPtr += i * queryOutput->sqlResult[j].len;

                    // push result entry into hashtable storage
                    (this->resultTable[this->queryAttrs.at(j)]).push_back(resultPtr);
                }
            }

            // if there are no more results to query, exit loop
            if (!queryOutput->continueInx)
                break;

            // otherwise continue fetching query results
            queryInput.continueInx = queryOutput->continueInx;
            prevStatus = status;

            status = rcGenQuery(this->conn->commPtr(), &queryInput, &queryOutput);
        } while (!status);
    }

    // release rods connection mutex
    this->conn->mutexUnlock();

    // free rods api allocated resources
    delete queryInput.selectInp.inx;
    delete queryInput.selectInp.value;

    for (unsigned int i = 0; i < this->queryConds.size(); i ++)
        delete queryInput.sqlCondInp.value[i];

    delete queryInput.sqlCondInp.inx;
    delete queryInput.sqlCondInp.value;

    // let's not return no more rows found as an error
    if (status == CAT_NO_ROWS_FOUND)
        status = prevStatus;

    // return last rods api status to caller
    return (status);
}

int RodsGenQuery::attrCount() const
{
    // simply return size of attribute vector
    return (this->queryAttrs.size());
}

bool RodsGenQuery::hasAttribute(int rodsAttr) const
{
    // check if the search for attribute was successful
    if (this->attributeIndex(rodsAttr) != -1)
        return (true);

    return (false);
}

int RodsGenQuery::attributeIndex(int rodsAttr) const
{
    // simply iterate through
    for (unsigned int i = 0; i < this->queryAttrs.size(); i++)
    {
        if (this->queryAttrs.at(i) == rodsAttr)
            return (i);
    }

    // otherwise search failed, return -1
    return (-1);
}

std::vector<std::string> RodsGenQuery::getResultSet(unsigned int attrIndex) const
{
    // if we are within bounds of the result vector
    if (attrIndex < this->queryAttrs.size())
    {
        // return a copy of result vector at requsted index
        return (this->getResultSetForAttr(this->queryAttrs.at(attrIndex)));
    }

    // otherwise return an empty vector
    else
        return (std::vector<std::string>());
}

std::vector< std::vector<std::string> > RodsGenQuery::getResultSet() const
{
    std::vector< std::vector<std::string> > results;

    // build result set from internal hashtable representation
    for (unsigned int i = 0; i < this->queryAttrs.size(); i++)
        results.push_back(this->getResultSetForAttr(this->queryAttrs.at(i)));

    return (results);
}

std::vector<std::string> RodsGenQuery::getResultSetForAttr(int rodsAttr) const
{
    // if we have a result set for the attribute, return a const reference
    if (this->resultTable.find(rodsAttr) != this->resultTable.end())
        return (this->resultTable.at(rodsAttr));

    return (std::vector<std::string>());
}

void RodsGenQuery::reset()
{
    // clear internal containers
    this->queryAttrs.clear();
    this->queryConds.clear();
    this->resultTable.clear();
}

} // namespace Kanki
