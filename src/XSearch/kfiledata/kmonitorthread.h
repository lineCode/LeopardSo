#pragma once

#include <windows.h>

typedef struct ThreadParameter   
{  
	LPTSTR in_directory;//监控的路径  
	FILE_NOTIFY_INFORMATION *in_out_notification;//存储监控函数返回信息地址  
	DWORD in_MemorySize;//传递存储返回信息的内存的字节数  
	DWORD *in_out_BytesReturned;//存储监控函数返回信息的字节数  
	DWORD *in_out_version;//返回版本信息  
	FILE_NOTIFY_INFORMATION *temp_notification;//备用的一个参数  
}ThreadParameter;  

class KMonitorThread
{
public:
	KMonitorThread(void);
	~KMonitorThread(void);
};
