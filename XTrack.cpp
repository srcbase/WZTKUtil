//作者：
//2011-1-10		uni修改。
//2011-3-11		uni修改。
//
//改动说明：
//2011-1-10		uni
//将XTrack接受所有类型输入的模板函数去除，添加接受不同类型参数的函数。
//2011-3-11		uni
//接受double类型数据。
//增加了log操纵符用于写入到日志文件。

#include <iomanip>
#include <fstream>
#include "util/XTrack.h"
#include "util/Logger.h"

using namespace std;

namespace UtilTracker
{

string CxTrack::sm_projectName;


FunTypeExternLogFunction g_pFunLog = NULL;

void SetFunLog(FunTypeExternLogFunction funPtr)
{
	g_pFunLog = funPtr;
}

void* GetFunLog()
{
	return g_pFunLog;
}

CUtilCriticalLock CxTrack::sm_lock;

CxTrack::CxTrack()
{
	m_lastError = GetLastError();
	m_stm = new stringstream;
}

CxTrack::~CxTrack()
{
	delete m_stm;
}

void CxTrack::setProjectName(const string &projectName)
{
	CUtilScopeLock lock(sm_lock);
	sm_projectName = projectName;
}

string CxTrack::projectName() const
{
	return sm_projectName;
}

void CxTrack::setFunctionName(const string &functionName)
{
	m_functionName = functionName;
}

string CxTrack::functionName() const
{
	return m_functionName;
}

string CxTrack::message() const
{
	return m_stm->str();
}

CxTrack &dbg(CxTrack &xTrack)
{
	stringstream message;
	message << xTrack.message();
	OutputDebugStringA(message.str().c_str());
	return xTrack;
}

CxTrack &log(CxTrack &xTrack)
{
	stringstream message;
	message << xTrack.message();
	if(g_pFunLog)
	{
		g_pFunLog(message.str());
	}else
	{
		LOG_INFO(message.str());
	}
	return xTrack;
}

CxTrack &lasterr(CxTrack &xTrack)
{
	DWORD error = xTrack.lastError();
	HLOCAL hLocal = NULL;
	DWORD systemLocale = MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL);
	BOOL ok = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,error,systemLocale,
		(LPSTR)&hLocal,0,NULL);

	if(!ok)
	{
		HMODULE hDll = LoadLibraryExW(L"netmsg.dll",NULL,LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE);
		if(hDll)
		{
			ok = FormatMessageA(
				FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER,
				hDll,error,systemLocale,(LPSTR)&hLocal,0,NULL);

			FreeLibrary(hDll);
		}
	}

	if(ok && hLocal)
	{
		const char * szErrorMsg = (LPCSTR)LocalLock(hLocal);
		xTrack<<L"| 错误号："<<error<<L" 错误描述："<<szErrorMsg;

		LocalFree(hLocal);
	}
	else
	{
		xTrack<<L"| 错误号："<<error<<L" 错误描述：找不到对应的错误描述";
	}

	return xTrack;
}

CxTrack &hexdisp(CxTrack &xTrack)
{
	xTrack<<showbase<<hex<<setfill('0');
	return xTrack;
}

CxTrack &decdisp(CxTrack &xTrack)
{
	xTrack<<noshowbase<<dec<<setfill(' ');
	return xTrack;
}

void setFunctionName(CxTrack &xTrack,const std::string &functionName)
{
	xTrack.setFunctionName(functionName);
}

XTrackManip<const string &> func(const std::string &functionName)
{
	return XTrackManip<const string &>(&setFunctionName,functionName);
}

CxTrack &operator<<(CxTrack &xTrack,ostream &(*ostreamManipulator)(ostream &stm))
{
	stringstream &stm = *xTrack.m_stm;
	stm<<ostreamManipulator;
	return xTrack;
}


CxTrack& operator<<(CxTrack &xTrack,
				   basic_ios<char,char_traits<char> >& (*basicIosManipulator)(basic_ios<char,char_traits<char> >&))
{
	stringstream &stm = *xTrack.m_stm;
	stm<<basicIosManipulator;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,
				   std::ios_base& (*iosBaseManipulator)(std::ios_base&))
{
	stringstream &stm = *xTrack.m_stm;
	stm<<iosBaseManipulator;
	return xTrack;
}


CxTrack &operator<<(CxTrack &xTrack,const _Fillobj<char>& manip)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<manip;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,CxTrack &(__cdecl *xTrackManipulator)(CxTrack &xTrack))
{
	return (*xTrackManipulator)(xTrack);
}

CxTrack &operator<<(CxTrack &xTrack,const void *ptr)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<ptr;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,int intData)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<intData;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,float floatData)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<floatData;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,double data)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<data;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,unsigned int uintData)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<uintData;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,unsigned long ulongData)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<ulongData;
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,const string &message)
{
	return xTrack<<message.c_str();
}

CxTrack &operator<<(CxTrack &xTrack,const char *message)
{
	stringstream &stm = *xTrack.m_stm;
	stm<<message;
	return xTrack;
}

CxTrack &operator<<(const CxTrack &xTrack,const char *message)
{
	CxTrack &xTrack1 = const_cast<CxTrack &>(xTrack);
	stringstream &stm = *xTrack1.m_stm;
	stm<<message;
	return xTrack1;
}

CxTrack &operator<<(CxTrack &xTrack,const wstring &message)
{
	xTrack<<message.c_str();
	return xTrack;
}

CxTrack &operator<<(CxTrack &xTrack,const wchar_t *message)
{
	if(!message)
	{
		xTrack<<" (NULL) ";
		return xTrack;
	}

	const size_t mbsLength = wcslen(message)*4+1;
	size_t convertedChars = 0;
	char *mbsMessage = new char[mbsLength];
	//memset(mbsMessage,0,mbsLength);
	//区域设为机器默认
	locale default("");
	locale former = locale::global(default);
	wcstombs_s(&convertedChars,mbsMessage,mbsLength,message,_TRUNCATE);
	locale::global(former);
	xTrack<<mbsMessage;
	delete [] mbsMessage;
	return xTrack;
}

}
