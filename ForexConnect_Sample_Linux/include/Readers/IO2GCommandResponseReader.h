#pragma once


class IO2GCommandResponseReader : public IAddRef
{
 protected:
    IO2GCommandResponseReader();
 public:
    virtual bool isSuccessful() = 0;
    virtual const char* getErrorDescription() = 0;
};

