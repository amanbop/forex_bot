/*
 * $Header:  IO2GTablesUpdatesReader.h$
 *
 * Copyright (c) 2009 FXCM, Holdings.
 * 11 Hanover Square 4th Floor, New York NY, 10005 USA
 *
 * THIS SOFTWARE IS THE CONFIDENTIAL AND PROPRIETARY INFORMATION OF
 * FXCM, LLC. ("CONFIDENTIAL INFORMATION"). YOU SHALL NOT DISCLOSE
 * SUCH CONFIDENTIAL INFORMATION AND SHALL USE IT ONLY IN ACCORDANCE
 * WITH THE TERMS OF THE LICENSE AGREEMENT YOU ENTERED INTO WITH
 * FXCM.
 *
 * File description:
 *
 * $History: $
 * 2011/01/16  shma      file is initially created
 */
#pragma once

class IO2GTablesUpdatesReader : public IAddRef
{
 protected:
    IO2GTablesUpdatesReader();
 public:
    /** Gets current server timestamp.
        @return     Server Timestamp
    */
    virtual DATE getServerTime() = 0;
    /** Gets updates size.*/
    virtual int size() = 0;
    /** Gets update type.*/
    virtual O2GTableUpdateType getUpdateType(int index) = 0;
    /** Gets update table*/
    virtual O2GTable getUpdateTable(int index) = 0;
    /** Gets offer row by index.
        @return     Offer row. If the specified row is not offer then
                    return NULL.
    */
    virtual IO2GOfferRow *getOfferRow(int index) = 0;
    /** Get account row.
        @return         Account row .If the specified row is not account then
                        return NULL.
    */
    virtual IO2GAccountRow *getAccountRow(int index) = 0;
    /** Get account row.
        @return         Order row. If the specified row is not order then
                        return NULL.
    */
    virtual IO2GOrderRow *getOrderRow(int index) = 0;
    /** Get trade row.
        @return         Trade row. If the specified row is not order then
                        return NULL.
    */
    virtual IO2GTradeRow *getTradeRow(int index) = 0;
    /** Get closed trade row.
        @return         Closed trade row. If the specified row is not order then
                        return NULL.
    */
    virtual IO2GClosedTradeRow *getClosedTradeRow(int index) = 0;
    /** Get message row.
        @return         Message row. If the specified row is not order then
                        return NULL.
    */
    virtual IO2GMessageRow *getMessageRow(int index) = 0;
};
