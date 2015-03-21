#pragma once

class KTaskBase
{
public:
    virtual ~KTaskBase(){};
    virtual void DoTask() = 0; 
    virtual BOOL GetTaskState() = 0;
    virtual BOOL MallocMemory() = 0;
    virtual void FreeMemory() = 0;
    virtual void SetTaskState(BOOL bIsFilnish) = 0;
    virtual void SetTaskInValid() = 0;
};