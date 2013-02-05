#ifndef _SOBEY_CARD_PARAM_H_
#define _SOBEY_CARD_PARAM_H_

////////////////////////Application Note////////////////////////////////////////////////////////////
//LoadFactoryParam(UINT nBoardIdx,DWORD nCommand);
//	nCommand: 0xFF000000(_AUD_INPORT_PARAM/_AUD_OUTPORT_PARAM/_VID_INPORT_PARAM/_VID_OUTPORT_PARAM);
//			  0x00FF0000(Port Number)

//AdjParameter( UINT nBoardIdx,DWORD nCommand,DWORD nValue);
//GetParameter( UINT nBoardIdx,DWORD nCommand,DWORD &nValue);
//	nCommand: 0xFF000000(_AUD_INPORT_PARAM/_AUD_OUTPORT_PARAM/_VID_INPORT_PARAM/_VID_OUTPORT_PARAM);
//			  0x00FF0000(Port Number)
//			  0x0000FFFF(parameter code)
//	nValue:	  (paramter value)

//LoadUserParam(UINT nBoardIdx,DWORD nCommand);
//SaveUserParam(UINT nBoardIdx,DWORD nCommand);
//	nCommand: 0xFF000000(_AUD_INPORT_PARAM/_AUD_OUTPORT_PARAM/_VID_INPORT_PARAM/_VID_OUTPORT_PARAM);
//			  0x00FF0000(Port Number)
//			  0x0000FFFF	User Number


#define	_ID_COMMAND				0xFF000000
#define _ID_PORT				0x00FF0000
#define _ID_CHANNEL				0x0000FF00
#define _ID_PARAMETER			0x000000FF

#define	_GetIDCommand(_Cmd)			(_ID_COMMAND & _Cmd)
#define _GetIDPort(_Cmd)			((_ID_PORT & _Cmd) >> 16)
#define _GetIDChannel(_Cmd)			((_ID_CHANNEL & _Cmd) >> 8)
#define	_GetIDParameter(_Cmd)		(_ID_PARAMETER & _Cmd)

#define _SetIDCommand(_Data,_Cmd,_Channel,_Port,_Parameter){\
		_Data = 0; _Data=_Cmd | (_Port<<16) | (_Channel<<8) | _Parameter;}


////////////////////////Audio inport/outport parameter////////////////////////////////////////////////////////////
#define _AUD_INPORT_PARAM		0x05000000
#define _AUD_OUTPORT_PARAM		0x06000000
//0x05yyxxxx	yy:port number	xx:parameter code
#define _AUD_LEVEL_ADJ				0

////////////////////////Videos inport/outport parameter////////////////////////////////////////////////////////////
#define _VID_INPORT_PARAM		0x09000000
#define _VID_OUTPORT_PARAM		0x0a000000
//0x04yyxxxx	yy:port number	xxxx:parameter code
#define _VID_AGC				0
#define _VID_LIGHTNESS			1
#define _VID_CHROM				2
#define _VID_CONSTRACT			3
#define _VID_HUE				4
#define _VID_LUMA_CHROMA_DELAY	5
#define _VID_GAINY				6
#define _VID_GAINU				7
#define _VID_GAINV				8
#define _VID_SUB_FREQUENCE		9
#define _VID_SUB_LEVEL			10
#define _VID_BLACK_LEVEL		11

////////////////////////输出通道同步端口参数////////////////////////////////////////////////////////////
#define _CHANNEL_SYNC_PORT_PARAM		0x12000000
//0x03yyzzxx	yy:port number	zz:channel number xx:parameter code
#define _CHANNEL_SYNC_HOR_PHASE				3
#define _CHANNEL_SYNC_VER_PHASE				4
#define _CHANNEL_SYNC_SUB_PHASE				5

/////////////////////////_ExternFunc SobeyCard//////////////////////////////////
#define _EXT_SET_SYNC_			0		//for the boards of single output channel 
#define _EXT_SET_OUTPUT_SIGNAL	1		//for select out signal in outport
#define _EXT_SET_DVB_OUT_RATE	2		//for SetDvb Out ratio

/////////////////////////_ExternFunc OutputChannel//////////////////////////////////
#define _EXT_OUT_NEXT_FRAME				0x13000000
//0x03yyzzxx	yy: 	zz:channel number xx:

#define _EXT_OUT_SEEK_PAUSE_FRME_MODE	0x14000000		
//0x03yyzzxx	yy:	zz:channel number xx:parameter code
#define _EXT_FIELD_MODE		0
#define _EXT_FRAME_MODE		1

#define _EXT_OUT_HA						0x15000000
//0x03yyzzxx	yy:reserved	zz:channel number xx:parameter code
#define _EXT_SET_ERR_TC_INTERVAL		0		//xx = interval frame number
#define _EXT_SET_ERR_TC_LIMIT			1		//xx = max continuous error frame number
#define _EXT_SET_TRA_STATUS				2		//xx =0: play; 1: seek; 2: pause;
#define _EXT_ENABLE_HA					3		//xx =0,Disable; 1,Enable
#define _EXT_SET_ERR					4		//xx =0,Set;     1,Clear


#define _EXT_TC							0x16000000
//0x03yyzzxx	yy:reserved	zz:channel number xx:parameter code
#define _EXT_SET_INTC					5
#define _EXT_SET_OUT_VITC				6
#define _EXT_SET_OUT_LTC				7
#define _EXT_SET_OUT_SBTC				8

//VBI Data Type define
#define VBI_VITC	0x0
#define VBI_LTC		0x1
#define VBI_SBTC	0x2

#endif