#pragma once

#include <windows.h>

typedef struct ThreadParameter   
{  
	LPTSTR in_directory;//��ص�·��  
	FILE_NOTIFY_INFORMATION *in_out_notification;//�洢��غ���������Ϣ��ַ  
	DWORD in_MemorySize;//���ݴ洢������Ϣ���ڴ���ֽ���  
	DWORD *in_out_BytesReturned;//�洢��غ���������Ϣ���ֽ���  
	DWORD *in_out_version;//���ذ汾��Ϣ  
	FILE_NOTIFY_INFORMATION *temp_notification;//���õ�һ������  
}ThreadParameter;  

class KMonitorThread
{
public:
	KMonitorThread(void);
	~KMonitorThread(void);
};
