#include <windows.h>
#include <atlenc.h>
#include <string>
#include "..\..\Modules\RegEdit.hpp"

int main()
{
	// �ֱ���SOFTWARE ��SOFTWARE\WOW6432Node����2����ֵ�ֱ�Ϊ64��32
	// ��������ǻ�ȡ32λ�����Ӧ��ע������ر��ض���Ҳ����SOFTWARE\WOW6432Node����ȡ���32
	DWORD dwValue = REG_GET_INT(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test");
	// ��������ǻ�ȡ32λ����ر��ض�����ֵ��Ҳ����SOFTWARE����ȡ���64
	dwValue = CRegEdit::RegGetIntValue(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test",HKEY_LOCAL_MACHINE,TRUE);

	// ������2������
	REG_SET_INT(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test",3200);
	CRegEdit::RegSetIntValue(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test",6400,HKEY_LOCAL_MACHINE,TRUE);
	
	std::string strValue = REG_GET_STR(L"SYSTEM\\CurrentControlSet\\Services\\HfSvc",L"Version");
	REG_SET_STR(L"SYSTEM\\CurrentControlSet\\Services\\HfSvc",L"Version","10.5.573");


	return 0;
}