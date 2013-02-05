/*++
 Copyright (c) 2004  SOBEY Digital .Ltd

 Module Name:
		SonyCardErr.h

 Abstract:
		This module defines the result of operating by all funciton in SBMGWCardApi.h

 Author:
		PowerBicycle
		
 Revision History:
		VerNo.1.0		1-9-2004
--*/

#ifndef _SOBEY_CARD_ERROR_CODE_
#define _SOBEY_CARD_ERROR_CODE_

#ifndef	SBStatus
	#define	SBStatus				DWORD
#endif

#define _Execut(_x_)			\
	if(nStatus = _x_)			\
	{	return nStatus;			\
	}

#ifndef statusSuccess
	#define statusSuccess			0x00
	#define statusOpenTMFail				0x01
	#define statusCrtEventFail				0x02
	#define statusCrtMsgFail				0x03
	#define statusCrtSyncHandleFail			0x04
	#define statusCrtOutFileFail			0x05
	#define statusCreateExeFileFail			0x06
	#define statusNotFindExeFile			0x07
	#define statusCrtProcessFail			0x08
	#define statusNotFindOutFile			0x09
	#define statusConfFail					0x0a
	#define statusTimeCrtCruntimFail		0x0b
	#define statusStartCardFail				0x0c
	#define statusSendCmdFail				0x0d
	#define statusWaitCardAckFail			0x0e
	#define statusRecvCardAckFail			0x0f

	#define statusOpenServTmmanFail			0x20
	#define statusStartServTmmanFail		0x21
	#define statusOpenServConmFail			0x22
	#define statusStartServConmFail			0x23
	#define statusInitConmFail				0x24

	#define statusRecvErrAck				0x30
	#define statusCardNotInit				0x31
	#define statusSetCapBufEmptyFail		0x32
	#define statusSetTransBufFullFail		0x33

	#define statusTargetOpenDspFail			0x100
	#define statusTargetOpenDmaFail			0x101
	#define statusTargetOpenAIFail			0x102
	#define statusTargetOpenAOFail			0x103
	#define statusTargetOpenVIFail			0x104
	#define statusTargetOpenVOFail		   	0x105
	#define statusTargetOpenPinFail			0x106
	#define statusTargetPinSetupFail		0x107
	#define statusTargetSetPinFail			0x108
	#define statusTargetCacheAllocFail		0x109
	#define statusTargetAllocMemFail		0x10a
	#define statusTargetCreatEventSemFail	0x10b
	#define statusTargetCreateEventFail		0x10c
	#define statusTargeCreateMutexFail		0x10d
	#define statusTargetCreateMsgSemFail	0x10e
	#define statusTargetCreateMsgFail		0x10f
	#define statusTargetCreateSemFail		0x110
	#define statusTargetCreateTaskFail		0x111
	#define statusTargetStartTaskFail		0x112

	#define statusTargetI2cReadFail			0x113
	#define statusTargetI2cWriteFail		0x114
	#define statusTargetConfig7111Fail		0x115
	#define statusTargetOpenFpgaConfigFail	0x116
	#define statusTargetFpgaConfFileLenErr	0x117
	#define statusTargetFpgaConfReadFileErr	0x118
	#define statusTargetFpgaConfStatusLow	0x119
	#define statusTargetFpgaConfDoneLow		0x11a
	#define statusTargetRecvCmdFail			0x11b
	#define statusTargetNotExistCmd			0x11c 
	#define statusTargetInitDisposeFail		0x11d
	#define statusTargetAllocDmaReqFail		0x11e
	#define statusTargetTheCapModeNotExist	0x11f
	#define statusTargetViInstSetupFail		0x120
	#define statusTargetViRawSetupFail		0x121
	#define statusTargetStartVIFail			0x122
	#define statusTargetTheTransModeNotExist 0x123
	#define statusTargetStartVOFail			0x124
	#define statusTargetVoInstSetupFail		0x125
	#define statusTargetVoRawSetupFail		0x126

	#define statusSdramDetectFail			0x127
	#define statusTargetOpenLPBackFileFail	0x128
	#define statusTargetReadLPBackFileLenErr 0x129
	#define statusLoadLibFail				0x130
	#define statusTimeInitCruntimFail		0x131

	#define statusSetupAiInstanceFial		0x140
	#define statuSetupViYuvFailed			0x141
	#define statusAudInstSetupFaild			0x142
	#define statusVoInstSetupFaild			0x143
	#define statusVoYuvSetupFaild			0x144
	#define statusTargetStartAOFail			0x145
	#define statusTargetStartAIFail			0x146
	#define statusCardNumErr				0x147
#endif
	#define	statusUnsuport					0x200
//-----------------------New interface add-----------------------------
	#define statusOutOfMaxChannelNum		0x200
	#define statusChannelOutOfRange			0x201
	#define statusNotSetupChannelParam		0x202
	#define statusNotEnoughMemSupport		0x203
	#define statusUnknownCard				0x204
	#define statusNotInitChannel			0x205
	#define	statusAlreadyUsed				0x206
	#define statusNotSupportMode			0x207
	#define statusUnknownPort				0x208
	#define statusNotSupportVideoStandart	0x209
	#define statusStatusMismatch			0x210
	#define	statusMemFull					0x211
	#define statusCacheOutOfRange			0x212
	#define statusCardIdxOutOfRange			0x213
	#define statusNotDefineCmd				0x214
	#define	statusPortOutOfRange			0x215

	#define statusFailed					0xFFF;

#endif


