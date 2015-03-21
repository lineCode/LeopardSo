#pragma once

#include "ktaskbase.h"

class KWorker
{
public:
    typedef DWORD_PTR RequestType;

    KWorker()
    {
    }

    virtual BOOL Initialize(void* pvParam)
    {  
        return TRUE;
    }

    virtual void Terminate(void* pvParam)
    {
       
    }

    void Execute(RequestType dwReqType, void* pvParam, OVERLAPPED* pOverLapped)
    {
        KTaskBase* pTask = reinterpret_cast<KTaskBase*>(dwReqType);
        pTask->DoTask();
    }
};