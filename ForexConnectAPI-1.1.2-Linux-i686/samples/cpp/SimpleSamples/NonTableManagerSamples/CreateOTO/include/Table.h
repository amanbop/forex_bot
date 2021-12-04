
#pragma once

template <typename TRow>
class TTable
{
    typedef std::map <std::string, TRow*> TRows;

    TRows mRows;
    threading::Mutex mMutex;

protected:
    virtual ~TTable()
    {
        typename TRows::iterator iter = mRows.begin();
        typename TRows::iterator itEnd = mRows.end();
        for (; iter != itEnd; ++iter)
            iter->second->release();
    }

    virtual std::string getComareString(TRow* pRow) = 0;

    void lock()
    {
        mMutex.lock();
    }
    void unlock()
    {
        mMutex.unlock();
    }
public:

    void update(TRow* pRow)
    {
        lock();
        pRow->addRef();

        std::string sCompare = getComareString(pRow);
        typename TRows::iterator iter = mRows.find(sCompare);
        if (iter == mRows.end())
            mRows[sCompare] = pRow;
        else
        {
            iter->second->release();
            iter->second = pRow;
        }
        unlock();
    }
    TRow* getRow(const std::string& sCompare)
    {
        TRow* pRow = NULL;
        lock();

        typename TRows::iterator iter = mRows.find(sCompare);
        if (iter != mRows.end())
        {
            pRow = iter->second;
            pRow->addRef();
        }
        unlock();

        return pRow;
    }
};

class COffersTable : public TTable <IO2GOfferRow>
{
    virtual std::string getComareString(IO2GOfferRow* pRow)
    {
        return pRow->getOfferID();
    }
};

