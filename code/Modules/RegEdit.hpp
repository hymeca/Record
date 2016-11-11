#pragma once
#include <string>
#include <Windows.h>
#include <atlenc.h>

//#define REG_DEF_SUB_ITEM_PATH
			
#ifdef  REG_DEF_SUB_ITEM_PATH
#define REG_SUB_ITEM			""

#define	REG_GET_STR(Key)		CRegEdit::RegGetStringValue(REG_SUB_ITEM,Key)
#define REG_GET_WSTR(Key)		CRegEdit::RegGetWideStringValue(REG_SUB_ITEM,Key)
#define	REG_GET_INT(Key)		CRegEdit::RegGetIntValue(REG_SUB_ITEM,Key)
#define REG_SET_STR(Key,Data)	CRegEdit::RegSetStringValue(REG_SUB_ITEM,Key,Data)
#define REG_SET_WSTR(Key,Data)	CRegEdit::RegSetWideStringValue(REG_SUB_ITEM,Key,Data)
#define REG_SET_INT(Key,Data)	CRegEdit::RegSetIntValue(REG_SUB_ITEM,Key,Data)
#else
#define	REG_GET_STR(Path,Key)		CRegEdit::RegGetStringValue(Path,Key)
#define REG_GET_WSTR(Path,Key)		CRegEdit::RegGetWideStringValue(Path,Key)
#define	REG_GET_INT(Path,Key)		CRegEdit::RegGetIntValue(Path,Key)

#define REG_SET_STR(Path,Key,Data)	CRegEdit::RegSetStringValue(Path,Key,Data)
#define REG_SET_WSTR(Path,Key,Data)	CRegEdit::RegSetWideStringValue(Path,Key,Data)
#define REG_SET_INT(Path,Key,Data)	CRegEdit::RegSetIntValue(Path,Key,Data)
#endif



/***********************************************************************
* PS：
*	直接单文件实现注册表的各种操作,不产生实例，直接定义为类的static函数
*   直接调用static函数是不会进入构造函数的，所以不要在构造函数做任何事情
*	最后用宏来简写函数的调用，不考虑超过MAX_PATH缓存的情况
*	编码格式统一为 Unicode,编译器默认的就是这种定义方式
*   不使用WideCharToMultiByte转换，因为W2A转换的结果二进制内容和前者一样
************************************************************************/

/***********************************************************************
**
** 64位系统中32位程序读写注册表，会被系统重定向，除了关闭重定向之外，
** 参数bWow64可以来控制是否关闭注册表重定向，默认是FALSE不关闭重定向
**
************************************************************************/


class CRegEdit
{
public:
	CRegEdit(void)
	{
		printf("1\n");
	}
	~CRegEdit(void){}

	static std::wstring RegGetWideStringValue(LPCWSTR lpSubItem,LPCWSTR lpKeyName,HKEY hKey=HKEY_LOCAL_MACHINE,BOOL bWow64=FALSE)
	{
		HKEY hSubKey = NULL;
		LONG lRtn = ERROR_SUCCESS;
		std::wstring strValue;
		REGSAM	RegSam = bWow64?(KEY_READ|KEY_WOW64_64KEY):KEY_READ;

		lRtn = RegOpenKeyEx(hKey,lpSubItem,0, RegSam, &hSubKey);
		if( ERROR_SUCCESS == lRtn )
		{
			WCHAR  szData[MAX_PATH] = {0};
			DWORD  dwLength	= MAX_PATH;
			DWORD  dwType	= REG_SZ;

			lRtn = RegQueryValueEx(hSubKey,lpKeyName,NULL,&dwType, (LPBYTE)szData,&dwLength);
			if( ERROR_SUCCESS == lRtn )
			{
				strValue = szData;
			}
			RegCloseKey(hSubKey);
		}

		return strValue;
	}

	static std::string RegGetStringValue(LPCWSTR lpSubItem,LPCWSTR lpKeyName,HKEY hKey=HKEY_LOCAL_MACHINE,BOOL bWow64=FALSE)
	{
		std::wstring strValue;
		USES_CONVERSION;
		strValue = RegGetWideStringValue(lpSubItem,lpKeyName,hKey,bWow64);
		// W2A和WideCharToMultiByte转换结果一致
		// 返回的字符串虽然是栈内存，但是会进入string构造函数，生成对象返回，返回后是一个拷贝构造函数
		return W2A(strValue.c_str());
	}

	static DWORD RegGetIntValue(LPCWSTR lpSubItem,LPCWSTR lpKeyName,HKEY hKey=HKEY_LOCAL_MACHINE,BOOL bWow64=FALSE)
	{
		HKEY  hSubKey = NULL;
		LONG  lRtn = ERROR_SUCCESS;
		DWORD dwValue = 0;
		REGSAM	RegSam = bWow64?(KEY_READ|KEY_WOW64_64KEY):KEY_READ;

		lRtn = RegOpenKeyEx(hKey,lpSubItem,0, RegSam, &hSubKey);
		if( ERROR_SUCCESS == lRtn )
		{
			DWORD dwType	= REG_DWORD;
			DWORD dwLength	= sizeof(DWORD);
			DWORD dwError	= 0;
			lRtn = RegQueryValueEx(hSubKey,lpKeyName,NULL,&dwType,(LPBYTE)&dwValue,&dwLength);
			if( ERROR_SUCCESS != lRtn)
			{
				dwError = GetLastError();
			}
			RegCloseKey(hSubKey);
		}

		return dwValue;
	}

	static BOOL RegSetStringValue(LPCWSTR lpSubItem,LPCWSTR lpKeyName,LPCSTR lpData,HKEY hKey=HKEY_LOCAL_MACHINE,BOOL bWow64=FALSE)
	{
		BOOL bRetVal = FALSE;

		USES_CONVERSION;
		bRetVal = RegSetWideStringValue(lpSubItem,lpKeyName,A2W(lpData),hKey,bWow64);
		return bRetVal;
	}

	static BOOL RegSetWideStringValue(LPCWSTR lpSubItem,LPCWSTR lpKeyName,LPCWSTR lpData,HKEY hKey=HKEY_LOCAL_MACHINE,BOOL bWow64=FALSE)
	{
		HKEY hSubKey = NULL;
		BOOL bRetVal = FALSE;
		LONG lRtn = ERROR_SUCCESS;
		REGSAM	RegSam = bWow64?(KEY_WRITE|KEY_WOW64_64KEY):KEY_WRITE;
		// REG_OPTION_NON_VOLATILE 创建的注册表是永久的，不会因为计算机的重启而消失
		// 如果不指明这个标志，计算机重启后注册表信息丢失
		lRtn = RegCreateKeyEx(hKey,lpSubItem,0,NULL,REG_OPTION_NON_VOLATILE,RegSam,NULL,&hSubKey,NULL);
		if( ERROR_SUCCESS == lRtn )
		{
			DWORD dwLength = wcslen(lpData)*sizeof(WCHAR) + sizeof(WCHAR);

			if ( ERROR_SUCCESS == RegSetValueEx(hSubKey,lpKeyName,0,REG_SZ,(LPBYTE)lpData,dwLength) )
			{
				bRetVal = TRUE;
			}
			RegCloseKey(hSubKey);
		}

		return bRetVal;
	}

	static BOOL RegSetIntValue(LPCWSTR lpSubItem,LPCWSTR lpKeyName,DWORD lpData,HKEY hKey=HKEY_LOCAL_MACHINE,BOOL bWow64=FALSE)
	{
		HKEY hSubKey = NULL;
		BOOL bRetVal = FALSE;
		LONG lRtn = ERROR_SUCCESS;
		REGSAM	RegSam = bWow64?(KEY_WRITE|KEY_WOW64_64KEY):KEY_WRITE;

		lRtn = RegCreateKeyEx(hKey,lpSubItem,0,NULL,REG_OPTION_NON_VOLATILE,RegSam,NULL,&hSubKey,NULL);
		if( ERROR_SUCCESS == lRtn )
		{
			DWORD dwValue = lpData;
			if( ERROR_SUCCESS == RegSetValueEx(hSubKey,lpKeyName,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD)) )
			{
				bRetVal = TRUE;
			}
			RegCloseKey(hSubKey);
		}

		return bRetVal;
	}
};
