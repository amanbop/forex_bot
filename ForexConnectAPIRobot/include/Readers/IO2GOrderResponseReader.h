/*
 * $Header:  IO2GOrderResponseReader.h$
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
 * 2011/01/18  shma      file is initially created
 */
#pragma once

class IO2GOrderResponseReader : public IAddRef
{
 protected:
    IO2GOrderResponseReader();
 public:
    virtual const char* getOrderID() = 0;
    virtual bool isUnderDealerIntervention() = 0;
};

