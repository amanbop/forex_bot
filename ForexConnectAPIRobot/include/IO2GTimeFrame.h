/*
 * $Header:  IO2GTimeframe.h$
 *
 * Copyright (c) 2010 Forex Capital Markets LLC.
 * 32 Old Slip, 10th Floor, New York NY, 10005 USA
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
 * 2010/09/29  shma      file is initially created
 */
#pragma once
typedef enum
{
    Tick = 0,                    //!< tick
    Min = 1,                    //!< 1 minute
    Hour = 2,                   //!< 1 hour
    Day = 3,                    //!< 1 day
    Week = 4,                   //!< 1 week
    Month = 5,                  //!< 1 month
    Year = 6
} O2GTimeframeUnit;


/** */
class IO2GTimeframe : public IAddRef
{
 protected:
    IO2GTimeframe();
 public:
    /** Gets the unique identifier of the timeframe.*/
    virtual const char *getID() = 0;

    virtual O2GTimeframeUnit getUnit() = 0;

    /** Gets number of */
    virtual int getSize() = 0;
};



class IO2GTimeframeCollection : public IAddRef
{
 protected:
    IO2GTimeframeCollection();
 public:
    /** Get numbers of the time frames.*/
    virtual int size() = 0;
    /** Gets time frame by the index.*/
    virtual IO2GTimeframe *get(int index) = 0;
    /** Gets time frame by the unique id.*/
    virtual IO2GTimeframe *get(const char *id) = 0;
};

