// IAudioBufDecoder.h: interface for the IAudioBufDecoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IAUDIOBUFDECODER_H__9E579847_0571_4C94_A608_D67ED30AD67B__INCLUDED_)
#define AFX_IAUDIOBUFDECODER_H__9E579847_0571_4C94_A608_D67ED30AD67B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SBT_AudioStructDefine.h"

#ifdef _XH_AUDIO_DEC_EXPORTS
#define XH_AUDIO_DEC_API __declspec(dllexport)
#else
#define XH_AUDIO_DEC_API __declspec(dllimport)
#endif

#define PURE = 0

class IAudioBufDecoder  
{
public:
	IAudioBufDecoder();
	virtual ~IAudioBufDecoder();

	//初始化
	//返回值： > 0    ----   success
	//         other  ----   fail
	virtual  int Initialize() PURE;

	//得到帧的信息
	//[IN]pFrameBuf,帧buf的起始地址; dwFrameLen指定数据大小;pstFrameInfo返回帧信息
	virtual  int GetFrameInfo(const BYTE* pFrameBuf, DWORD dwFrameLen, void* pstFrameInfo) PURE;
	
	
	//解码一帧
	//[IN]pFrameBuf , 帧buf的起始地址
	//[IN]dwFrameLen, 帧的大小
	//[OUT]ppBufOut, 解码后，存放PCM数据的起始地址
	//[OUT]dwBufOut, 解码一帧后，PCM数据的大小
	//[OUT]pWavOut, 解码后，PCM数据的格式
	//返回值：  >1 -- success,      others -- fail 
	virtual  int DecodeFrame(const BYTE* pFrameBuf, DWORD dwFrameLen, BYTE** ppBufOut, DWORD& dwBufOut,
		                      WAVEFORMATEX** pWavOut = NULL) PURE;

	//反初始化
	virtual  int UnInitialize() PURE;
};

extern "C" int XH_AUDIO_DEC_API CreateAudioBufDecoder(IAudioBufDecoder ** ppDec,int nDecoderType);
extern "C" void XH_AUDIO_DEC_API DestroyAudioBufDecoder(IAudioBufDecoder * pDec);

#endif // !defined(AFX_IAUDIOBUFDECODER_H__9E579847_0571_4C94_A608_D67ED30AD67B__INCLUDED_)
