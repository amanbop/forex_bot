#pragma once
/** Thread safe response queue.*/

class ResponseQueue
{
 public:
    /** Constructor.*/
    ResponseQueue();
    ~ResponseQueue();

    /** Push response to queue.*/
    void push(IO2GResponse* response);

    /** Pop response from queue.*/
    IO2GResponse* pop();

    /** Clear queue.*/
    void clearQueue();
 private:
    std::queue<IO2GResponse*> m_aResponses;
    threading::Mutex m_csQueue;
};


