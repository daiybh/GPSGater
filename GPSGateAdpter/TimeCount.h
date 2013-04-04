// CTimeCount.h
// 实现精确的时间测试的函数

#ifndef TC_TIMECOUNT_H
#define TC_TIMECOUNT_H

#define TC_EXPORT_API
#include <winnt.h>
class  TC_EXPORT_API CTimeCount
{
public:
	CTimeCount(void);
	~CTimeCount(void);

	// 输出秒还是毫秒， TRUE为毫秒， FALSE为微秒
	double		GetCountTime(bool bMS);
	// 重置开始计时时间
	void		Reset(void);

private:
	LARGE_INTEGER m_lgFrequency;
	LARGE_INTEGER m_lgStart;
};

#endif /* TC_TIMECOUNT_H */