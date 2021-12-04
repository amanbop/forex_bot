/*
 * $Header:  IO2GLoginRules.h$
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
 * 2010/10/14  shma      file is initially created
 */
#pragma once

/** Provide information about loaded table.*/
class Order2Go2 IO2GLoginRules : public IAddRef
{
 protected:
    IO2GLoginRules();
 public:
    /** Check loading table during login.*/
    virtual bool isTableLoadedByDefault(O2GTable table) = 0; 
    /** Get response for loaded table.*/
    virtual IO2GResponse* getTableRefeshResponse(O2GTable table) = 0;
    /** Gets system properties.*/
    virtual IO2GResponse* getSystemPropertiesResponse() = 0;
    /** Gets permission checker. */
    virtual IO2GPermissionChecker* getPermissionChecker() = 0;
    /** Gets trading settings provider. */
    virtual IO2GTradingSettingsProvider* getTradingSettingsProvider() = 0;
};


