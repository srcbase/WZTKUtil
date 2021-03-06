/*! \file XTrack.h
    \brief 定义了输出调试信息的类和相关函数。

    \author uni
    \date 2011-3-11
	\duo 20161026 修改以应用于util库
*/
#pragma once


#include <sstream>
#include <string>
#include <set>
#include <iomanip>

#include "util/scope_lock.h"

#pragma warning(disable:4127)


//! XTrack用于输出信息到调试器，或是其他地方。
/*!
    使用时直接用后面定义的宏XDEBUG，XTRACE，XWARNING等来输出信息。
    定义NO_DEBUG_OUTPUT等宏来禁止相应类型信息的输出。（/D "NO_DEBUG_OUTPUT"）。

    \section xtrack_usage_sec 使用方法
    \code
    #include "XTrack.h"

    using namespace UtilTracker

    XDEBUG<<"OpenProcess"<<lasterr<<dbg;		//输出"OpenProcess"加最后错误信息到调试器
    wstring unitName = L"BossA";
    XTRACE<<"Find Unit:"<<unitName<<dbg;		//输出"Find Unit:BossA"到调试器，XTRACE还会输出函数和行
    \endcode
    其中dbg类似于cout中的endl操纵符。
    lasterr为一元操纵符，输出最后的错误信息（GetLastError）。

    \section xtrack_manipulator_sec 操纵符
    XTrack支持cout支持的操纵符，同时还增加了以下几个操纵符。
    - lasterr 输出最后的错误信息。
    - dbg 输出调试信息到调试器。

    \deprecated 该类使用了较长时间，使用上基本没有问题，但已不再使用。
	\ingroup LOG_AND_DEBUG_MSG
*/
namespace UtilTracker
{

class CxTrack
{
public:
	CxTrack();
	~CxTrack();

    //! 设置当前工程名，设置后之后的每一句输出都会输出当前工程名。
    /*!
        \param projectName 工程名。
    */
	static void setProjectName(const std::string &projectName);

    //! 返回设置的工程名。
    /*!
        \return 返回用 setProjectName() 设置的工程名。
    */
	std::string projectName() const;

    //! 设置调试信息所属的函数名。
    /*! 
        \param 调试信息所属的函数名。
    */
	void setFunctionName(const std::string &functionName);

    //! 返回调试信息所属的函数名。
    /*!
        \return 调试信息所属的函数名。
    */
	std::string functionName() const;

    //! 返回调试信息的信息部分。
    /*!
        \return 调试信息的信息部分。
    */
	std::string message() const;

    //! 返回上一次的错误信息，内部使用。
	unsigned long lastError() const { return m_lastError; }

    //! 输入void *。
	friend CxTrack &operator<<(CxTrack &xTrack,const void *ptr);

    //! 输入int。
	friend CxTrack &operator<<(CxTrack &xTrack,int intData);

    //! 输入float。
	friend CxTrack &operator<<(CxTrack &xTrack,float floatData);

    //! 输入double。
	friend CxTrack &operator<<(CxTrack &xTrack,double data);

    //! 输入unsigned int。
	friend CxTrack &operator<<(CxTrack &xTrack,unsigned int uintData);

    //! 输入unsigned long。
	friend CxTrack &operator<<(CxTrack &xTrack,unsigned long lintData);

    //! 输入const char *。
	friend CxTrack &operator<<(CxTrack &xTrack,const char *message);

    //! 输入const char *。
	friend CxTrack &operator<<(const CxTrack &xTrack,const char *message);

	//! 接受ostream操纵符。
	friend CxTrack &operator<<(CxTrack &xTrack,std::ostream &(*ostreamManipulator)(std::ostream &));

	//! basic_ios操纵符。
	friend CxTrack& operator<<(CxTrack &xTrack,
		std::basic_ios<char,std::char_traits<char> >& (*basicIosManipulator)(std::basic_ios<char,std::char_traits<char> >&));

	//! ios_base操纵符。
	friend CxTrack& operator<<(CxTrack &xTrack,
		std::ios_base& (*iosBaseManipulator)(std::ios_base&));

    //! 接受std::_Smanip。
    /*!
        \param manip 一元操纵符，如setw，setprecision之类。
    */
	template<class _Arg> inline
	friend CxTrack &operator<<(CxTrack &xTrack,const std::_Smanip<_Arg>& manip)
	{
		stringstream &stm = *xTrack.m_stm;
		stm<<manip;
		return xTrack;
	}

    //! 接受setfill操纵符。
	friend CxTrack &operator<<(CxTrack &xTrack,const std::_Fillobj<char>& manip);

private:
	static std::string sm_projectName;
	static CUtilCriticalLock sm_lock;
	//流的构造函数会改变LastError，所以这里要做一些特别的处理。
	//以指针的形式持有流，在构造函数中先保存LastError，再创建流。
	std::stringstream *m_stm;
	std::string m_functionName;
	unsigned long m_lastError;

	CxTrack (const CxTrack&);
	CxTrack& operator=(const CxTrack&);
};

//接受无参数的XTrack操纵符
CxTrack &operator<<(CxTrack &xTrack,CxTrack &(__cdecl *xTrackManipulator)(CxTrack &));

//接受一个参数的操纵符
template<typename ArgType>
struct XTrackManip
{
	XTrackManip(void (__cdecl *fun)(CxTrack &,ArgType),ArgType arg)
		:m_fun(fun)
		,m_arg(arg)
	{
	}

	void (__cdecl *m_fun)(CxTrack &,ArgType);
	ArgType m_arg;
private:
	XTrackManip &operator=(const XTrackManip &);

};

template<typename ArgType>
inline CxTrack & operator<<(CxTrack &xTrack,const XTrackManip<ArgType> &manip)
{
	(*manip.m_fun)(xTrack,manip.m_arg);
	return xTrack;
}

void setFunctionName(CxTrack &xTrack,const std::string &functionName);
XTrackManip<const std::string &> func(const std::string &functionName);

CxTrack &operator<<(CxTrack &xTrack,const std::string &message);

//接受宽字符串
CxTrack &operator<<(CxTrack &xTrack,const std::wstring &message);
CxTrack &operator<<(CxTrack &xTrack,const wchar_t *message);

//操纵子
CxTrack &dbg(CxTrack &xTrack);			//输出到调试器
CxTrack &log(CxTrack &xTrack);			//输出到日志文件
CxTrack &lasterr(CxTrack &xTrack);		//最后错误信息（GetLastError()）
CxTrack &hexdisp(CxTrack &xTrack);		//8位16进制显示（例如0x00234321）
CxTrack &decdisp(CxTrack &xTrack);		//10进制显示

//供外部使用的宏

// #ifdef NO_OUTPUT
// #undef NO_OUTPUT
// #endif

//XDEBUG用于输出调试信息。
//定义NO_DEBUG_OUTPUT来禁止输出Debug信息。
#if defined(NO_DEBUG_OUTPUT) || defined(NO_OUTPUT)
#  define XDEBUG while(false) CxTrack()<<"[debug]"
#else
#  define XDEBUG CxTrack()<<"[debug]"<<func(__FUNCTION__)
#endif

//XINFO用于输出一些正常的信息，这些信息通常要显示在游戏中。
//定义NO_INFO_OUTPUT来禁止输出这些信息。
#if defined(NO_INFO_OUTPUT) || defined(NO_OUTPUT)
#  define XINFO while(false) CxTrack()<<"[info]"
#else
#  define XINFO CxTrack()<<"[info]"<<func(__FUNCTION__)
#endif

//XTRACE用于输出程序中的追踪信息。
//用于标明程序当前执行到了哪一步。
//定义NO_TRACE_OUTPUT来禁止输出Trace信息。
#if defined(NO_TRACE_OUTPUT) || defined(NO_OUTPUT)
#  define XTRACE while(false) CxTrack()<<"[trace]"
#else
#  define XTRACE CxTrack()<<"[trace]"<<func(__FUNCTION__)<<"["<<__LINE__<<"]	"
#endif

//XINFORM用于输出Inform信息，这些信息代表一些被处理的错误，但仍需明确告知调试者。
//定义NO_INFORM_OUTPUT来禁止输出Inform信息。
#if defined(NO_INFORM_OUTPUT) || defined(NO_OUTPUT)
#  define XINFORM while(false) CxTrack()<<"[inform]"
#else
#  define XINFORM CxTrack()<<"[inform]"<<func(__FUNCTION__)
#endif


//XWARNING用于输出警告信息，或是可以恢复的错误。
//定义NO_WARNING_OUTPUT来禁止输出警告信息。
#if defined(NO_WARNING_OUTPUT) || defined(NO_OUTPUT)
#  define XWARNING while(false) CxTrack()<<"[warning]"
#else
#  define XWARNING CxTrack()<<"[warning]"<<func(__FUNCTION__)
#endif

//XERROR用于输出错误信息，这些错误不致于立即使程序退出。
//ERROR错误会导致操作终止，一般要通知用户。
//定义NO_ERROR_OUTPUT来禁止输出错误信息。
#if defined(NO_ERROR_OUTPUT) || defined(NO_OUTPUT)
#  define XERROR while(false) CxTrack()<<"[error]"
#else
#  define XERROR CxTrack()<<"[error]"<<func(__FUNCTION__)<<"["<<__LINE__<<"]	"
#endif

//XFATAL用于输出致命错误，该错误信息在进程结束前输出。
//定义NO_FATAL_OUTPUT来禁止输出该信息。
#if defined(NO_FATAL_OUTPUT) || defined(NO_OUTPUT)
#  define XFATAL while(false) CxTrack()<<"[fatal]"
#else
#  define XFATAL CxTrack()<<"[fatal]"<<func(__FUNCTION__)
#endif

//XTEST用于输出临时测试信息，例如变量的值之类。
//定义NO_TEST_OUTPUT来禁止输出该类信息。
#if defined(NO_TEST_OUTPUT) || defined(NO_OUTPUT)
#  define XTEST while(false) CxTrack()<<"[test]"
#else
#  define XTEST CxTrack()<<"[test]"<<func(__FUNCTION__)
#endif

#if defined(NO_TEST_OUTPUT) || defined(NO_OUTPUT)
#  define XASSERT(condition) while(false) CxTrack()<<"[assert]"
#else
#  define XASSERT(condition) if(!condition) CxTrack()<<"[assert]"<<func(__FUNCTION__)\
	<<"["<<__LINE__<<"]	"
#endif

//XFEEDBACK用于输出需要反馈的信息。
//当不用时需自己删除。定义NO_OUTPUT不会屏蔽该输出。
#if defined(NO_FEEDBACK_OUTPUT)
#  define XFEEDBACK while(false) CxTrack()<<"[feedback]"
#else
#  define XFEEDBACK CxTrack()<<"[feedback]"<<func(__FUNCTION__)
#endif


typedef void (*FunTypeExternLogFunction)(const std::string&);
/**
 * \brief 通过SetFunLog可以设置操纵子log输出日志到哪里
 * \param
 * \return
 */
void SetFunLog(FunTypeExternLogFunction funPtr);
void* GetFunLog();

#define xtra CxTrack()

}
