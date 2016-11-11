#include <windows.h>
#include <atlenc.h>
#include <string>
#include "..\..\Modules\RegEdit.hpp"

int main()
{
	// 分别在SOFTWARE 和SOFTWARE\WOW6432Node建立2个键值分别为64和32
	// 下面语句是获取32位程序对应的注册表，不关闭重定向，也就是SOFTWARE\WOW6432Node，读取获得32
	DWORD dwValue = REG_GET_INT(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test");
	// 下面语句是获取32位程序关闭重定向后的值，也就是SOFTWARE，读取获得64
	dwValue = CRegEdit::RegGetIntValue(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test",HKEY_LOCAL_MACHINE,TRUE);

	// 设置下2个变量
	REG_SET_INT(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test",3200);
	CRegEdit::RegSetIntValue(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",L"test",6400,HKEY_LOCAL_MACHINE,TRUE);
	
	std::string strValue = REG_GET_STR(L"SYSTEM\\CurrentControlSet\\Services\\HfSvc",L"Version");
	REG_SET_STR(L"SYSTEM\\CurrentControlSet\\Services\\HfSvc",L"Version","10.5.573");


	return 0;
}