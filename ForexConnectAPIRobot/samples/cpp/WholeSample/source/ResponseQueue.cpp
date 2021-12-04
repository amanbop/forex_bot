#include "stdafx.h"
#include "ResponseQueue.h"

using threading::Mutex;

/** Constructor.*/
ResponseQueue::ResponseQueue()
{
}

ResponseQueue::~ResponseQueue()
{
    clearQueue();
}

/** Push response to queue.*/
void ResponseQueue::push(IO2GResponse* response)
{
    Mutex::Lock l(m_csQueue);
    m_aResponses.push(response);
}

/** Pop response from queue.*/
IO2GResponse* ResponseQueue::pop()
{
    IO2GResponse *response = NULL;
    Mutex::Lock l(m_csQueue);
    if (!m_aResponses.empty())
    {
        response = m_aResponses.front();
        m_aResponses.pop();
    }
    return response;
}

/** Clear queue.*/
void ResponseQueue::clearQueue()
{
    Mutex::Lock l(m_csQueue);
    while (!m_aResponses.empty())
    {
        IO2GResponse *response = m_aResponses.front();
        response->release();
        m_aResponses.pop();
    }
}


