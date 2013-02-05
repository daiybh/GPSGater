/* TsBaseModule.h */
// Author: Wu WenTao

#ifndef __TS_BASE_MODULE_H__
#define __TS_BASE_MODULE_H__

//////////// TS PACKETS /////////////////
#include "tsAnalyseDefine.h"

#include <vector>
#pragma warning(disable : 4251)

/////// Ts Header ////////////
struct TS_Header
{
	// public data member 
	UINT8	sync_byte;						// 8 bslbf
	
	UINT8	transport_error_indicator;		// 1 bslbf
	UINT8	payload_unit_start_indicator;	// 1 bslbf
	UINT8	transport_priority;				// 1 bslbf
	UINT16	PID;							// 13 uimsbf
	
	UINT8	transport_scrambling_control;	// 2 bslbf
	UINT8	adaptation_field_control;		// 2 bslbf
	UINT8	continuity_counter;				// 4 uimsbf

	// functions 
	TS_Header();

	BOOL is_empty();		// a empty header
	
	BOOL set(const UINT8 *pData, const int nSize);

	void get(UINT8 *&pData, int &nSize);

private:
	UINT8	data[4];
};

#endif // !__TS_BASE_MODULE_H__