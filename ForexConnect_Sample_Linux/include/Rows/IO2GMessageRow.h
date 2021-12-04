
/*
    !!! Caution:
    Do not change anything in this source code because it was automatically generated
    from XML class description
*/
#pragma once

class IO2GMessageRow : public IAddRef
{
 protected:
  IO2GMessageRow();
 public:
    
    virtual const char* getMsgID() = 0;
    virtual DATE getTime() = 0;
    virtual const char* getFrom() = 0;
    virtual const char* getType() = 0;
    virtual const char* getFeature() = 0;
    virtual const char* getText() = 0;
    virtual const char* getSubject() = 0;
};
