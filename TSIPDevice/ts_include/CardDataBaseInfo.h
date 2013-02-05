#pragma once


#pragma pack(push,1)
struct CardDataBaseInfo 
{
	char cFlag;
	LONGLONG llCounter;
	DWORD dwAllocVideoDataSize;
	DWORD dwVideoDataSize;
	int nVideoWidth;
	int nVideoHeight;
	int nVideoStandard;
	int nAllocAudioDataSize;//指的是MAX_AUDIO_CHANNEL中的一个
	int nAudioDataSize;
	DWORD dwSampleCount;

	CardDataBaseInfo()
	{
		Reset();
	}

	void Reset()
	{
		ZeroMemory(this,sizeof(CardDataBaseInfo));
	}
};
#pragma pack(pop)

#pragma  pack(push,1)
struct TS_PACK_DATA
{
	 	int    nBufSize;     // 内存大小
	 	int    nBufUseSize;  // 内存使用大小 
	 	BYTE*  pBuf;         // 该项的内存指针 
};
#pragma pack(pop)