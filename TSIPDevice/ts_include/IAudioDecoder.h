#ifndef I_AUDIO_DECODER_H
#define I_AUDIO_DECODER_H
#pragma  once
//½âÂëÆ÷»ùÀà
class IAudioDecoder
{
public:
	virtual int Initialize() = 0;
	virtual	int BeginDecoder() = 0;
	virtual int DecodeBuffer(BYTE *pBufferIn, int nBufLenIn, BYTE **pBufferOut, int *nBufLenOut) = 0;
	virtual int GetInfo(OUT BOOL &bStereo)=0;
	virtual int EndDecoder() = 0;
	virtual int Uninitialize() = 0;
	virtual ~IAudioDecoder(){};
};
#endif