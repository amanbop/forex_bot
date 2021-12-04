#pragma once

#include "Rows/IO2GAccountRow.h"
#include "Rows/IO2GOfferRow.h"
#include "Rows/IO2GOrderRow.h"
#include "Rows/IO2GClosedTradeRow.h"
#include "Rows/IO2GTradeRow.h"
#include "Rows/IO2GMessageRow.h"
/** Generic table response reader.*/
class IO2GGenericTableResponseReader : public IAddRef
{
 protected:
    IO2GGenericTableResponseReader();
 public:
    /** Gets a number of the rows in the table.*/
    virtual int size() = 0;
    /** Gets the column of the table.*/
    virtual IO2GTableColumnCollection *columns() = 0;
    /** Gets the cell value as string.*/
    virtual const void *getCell(int row, int column) = 0;
    virtual bool isCellValid(int row, int column) = 0;
    /** Gets table type.*/
    virtual O2GTable tableType() = 0;
};

/** Offers table response reader */
class IO2GOffersTableResponseReader : public IO2GGenericTableResponseReader
{
 protected:
    IO2GOffersTableResponseReader();
 public:
    virtual IO2GOfferRow *getRow(int index) = 0;
};

/** Accounts table response reader */
class IO2GAccountsTableResponseReader : public IO2GGenericTableResponseReader
{
 protected:
    IO2GAccountsTableResponseReader();
 public:
    virtual IO2GAccountRow *getRow(int index) = 0;
 
};

/** Orders table response reader */
class IO2GOrdersTableResponseReader : public IO2GGenericTableResponseReader
{
 protected:
    IO2GOrdersTableResponseReader();
 public:
    virtual IO2GOrderRow *getRow(int index) = 0;
 
};

/** Trades table response reader */
class IO2GTradesTableResponseReader : public IO2GGenericTableResponseReader
{
 protected:
    IO2GTradesTableResponseReader();
 public:
    virtual IO2GTradeRow *getRow(int index) = 0;
};

/** Closed trades table response reader */
class IO2GClosedTradesTableResponseReader : public IO2GGenericTableResponseReader
{
 protected:
    IO2GClosedTradesTableResponseReader();
 public:
    virtual IO2GClosedTradeRow *getRow(int index) = 0;
};

/** Messages table response reader */
class IO2GMessagesTableResponseReader : public IO2GGenericTableResponseReader
{
 protected:
    IO2GMessagesTableResponseReader();
 public:
    virtual IO2GMessageRow *getRow(int index) = 0;
};

