#include "StdAfx.h"
#include "GetCommand.h"

#pragma comment(lib,"TSIPDevice.lib")
GetCommand::GetCommand(void)
{
}

GetCommand::~GetCommand(void)
{
	m_pCommandList=NULL;
	m_pXmlParser = NULL;
	m_pITSDevice_card = NULL;
}

int GetCommand::StartWork()
{
	ITsDevice_Card::CreateInstance(&m_pITSDevice_card);

	CoInitialize(NULL);
	I_XMLParser::CreateInstance(&m_pXmlParser);

	m_pXmlParser->Create();
	m_pCommandList = new MGBufferListEx();
	DWORD dTotalSize = 3*1024;
	m_pCommandList->Initialize(dTotalSize,512,1000);
	int nListenPort=GetPrivateProfileInt(_T("GPSSet"),_T("listenPort_cmd"),120,GetMgConfigFileName());

	if(!m_pITSDevice_card->InitDevice(nListenPort))
		return -2;
	m_pITSDevice_card->SetMemShareBuffer(m_pCommandList,100);
	if(m_pITSDevice_card->StartInput()<=0)
		return -3;
	return 1;
}


int GetCommand::getCmd( GPSCommand *pGpsCmd )
{
	LISTITEMINFOEX *dataItem = m_pCommandList->FetchDataHeadItem();
	if(dataItem == NULL)
		return 0;
	int nLen = dataItem->nBufSizeUsedV0;
	
	if(nLen<1)
	{
		m_pCommandList->AddItem2EmptyTail(dataItem);
		return 0;
	}
	CStringA strCMd;
	strCMd.Format("%s",dataItem->pBufV0);
	m_pCommandList->AddItem2EmptyTail(dataItem);
	//在这里把xml 翻译成GPSCommand
	
	int nRet = 	processXML(CString(strCMd),pGpsCmd);	
	return nRet;
}
int copyData(char *pDest,CString strSrc)
{
	CStringA strA(strSrc);
	int nLen = strA.GetLength();
	if(pDest==NULL)
		pDest = new char[nLen];
	strcpy(pDest,strA.GetBuffer(0));
	strA.ReleaseBuffer(0);
	return nLen;
}

int CoverStringToInt(const CString string)
{
	return _ttoi((LPCTSTR)string);
}
int GetCommand::processXML( CString strCMd ,GPSCommand *pGpsCmd )
{
	int nRet = 0;
	/*
	strCMd = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>
		<SetArea><Id>402881f13846dc53013846de6ee10001</Id>
		<Sim>15096606319</Sim>
		<deviceID>5110323053</deviceID>
		<Type>2</Type>
		<LeftLng>102.71822</LeftLng>
		<LeftLat>25.021373</LeftLat>
		<RightLng>102.761478</RightLng>
		<RightLat>25.058078</RightLat>
		<AlertType>1</AlertType></SetArea>");
		/**/
	pGpsCmd->commandType = cmdType_ToGPS;
	
	CString strCmdLine;
	pGpsCmd->nLenCommandLine = pGpsCmd->nLenCommandXML = 0;

	do 
	{
		strCMd.MakeLower();
		if(!m_pXmlParser->Parse(strCMd))
		{
			break;
		}
		CString SS;
		TCHAR pRootName[100];
		int nLen = (m_pXmlParser->GetRootName(pRootName));
		
		CString strRoot(pRootName);
		CString strContent;
		/*if(!m_pXmlParser->GetRoot(strRoot,strContent))
		{
			break;
		}
		/**/
		nRet = 2;

		pGpsCmd->nLenCommandXML = copyData(pGpsCmd->pstrCommandXMl,strCMd);
		
		TCHAR *pTemp=(TCHAR*)pRootName/*+nLen+1*/;
		copyData(pGpsCmd->strDevID,m_pXmlParser->GetNodeText(_T("deviceid"),pTemp));
		

// 		//如果这里直接调用DevxGps的接口来format 字符串就OK
// 		if(_T("setarea")==strRoot)
// 		{
// 			//*XX,YYYYYYYYYY,S21,HHMMSS,Lx,M,D,Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
// 			//*HQ,6110916012,S21,130305,17,1,N,2245.318,2246.452,E,11233.232,11365.175#
// 			if(!m_pXmlParser->Parse(strCMd)) break;
// 			/*
// 			 *	
// 				-------------  right(max,max)
// 				|			|
// 				|			|
// 				|			|
// 				|			|
// 				-------------
// 				left(min,min)
// 			 */
// 
// 			CString strDeviceID =m_pXmlParser->GetNodeText(_T("sim"),pTemp);
// 			copyData(pGpsCmd->strSim,strDeviceID);
// 			
// 			strDeviceID =m_pXmlParser->GetNodeText(_T("deviceid"),pTemp);
// 			copyData(pGpsCmd->strDevID,strDeviceID);
// 
// 			copyData(pGpsCmd->strCMDID,(m_pXmlParser->GetNodeText(_T("id"),pTemp)));
// 
// 			strcpy(pGpsCmd->strCMDID,"S21");
// 
// 			copyData(pGpsCmd->strCMDID,(m_pXmlParser->GetNodeText(_T("type"),pTemp)));
// 		
// 			CString alertType = m_pXmlParser->GetNodeText(_T("alerttype"),pTemp);
// 			//默认为 出区域报警
// 			
// 			if(alertType ==_T("1"))//进区域报警
// 			{
// 				alertType= _T("3");
// 				alertType= _T("4");
// 			}
// 			else
// 			{
// 				alertType= _T("1");
// 				alertType= _T("2");
// 			}
// 			
// 			//copyData(pGpsCmd->strCMDID,());
// 			CString strCmdID ;
// 			strCmdID.Format(_T("s21_1_%s"),m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
// 
// 			copyData(pGpsCmd->strCMDID,strCmdID);
// 
// 			//formatParam
// 			CString sParam;
// 			//				 围栏号	条件 纬度标志
// 			//               Lx,	M,	D,		Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
// 			sParam.Format(_T("%s,%s,%s,%s,%s,%s,%s,%s"),
// 				m_pXmlParser->GetNodeText(_T("areaid"),pTemp),
// 				alertType,
// 				_T("N"),
// 				m_pXmlParser->GetNodeText(_T("leftlat"),pTemp),
// 				m_pXmlParser->GetNodeText(_T("rightlat"),pTemp),
// 				_T("E"),
// 				m_pXmlParser->GetNodeText(_T("leftlng"),pTemp),
// 				m_pXmlParser->GetNodeText(_T("rightlng"),pTemp));
// 
// 			pGpsCmd->nLenCmdParameters =  copyData(pGpsCmd->commandParameters,sParam);
// 			
// 			CTime curTime = CTime::GetCurrentTime();
// 			strCmdLine.Format(_T("*HQ,%s,S21,%s,%s#"),strDeviceID,curTime.Format(_T("%H%M%S")),sParam);
// 			pGpsCmd->nLenCommandLine = copyData(pGpsCmd->strCommandLine,strCmdLine);
// 			
// 
// 			int nRecordID = CoverStringToInt(m_pXmlParser->GetNodeText(_T("id"),pTemp));
// 			pGpsCmd->pVoid = (int*)nRecordID;			
// 			nRet = 1;
// 		}
// 		else if(_T("cancelarea")==strRoot)
// 		{
// 			if(!m_pXmlParser->Parse(strCMd)) break;
// 
// 
// 			pGpsCmd->commandType = CmdType(cmdType_ToGPS+1);
// 
// 			CString strDeviceID =m_pXmlParser->GetNodeText(_T("sim"),pTemp);
// 			copyData(pGpsCmd->strSim,strDeviceID);
// 
// 			strDeviceID =m_pXmlParser->GetNodeText(_T("deviceid"),pTemp);
// 			copyData(pGpsCmd->strDevID,strDeviceID);
// 
// 			
// 			CString strCmdID ;
// 			strCmdID.Format(_T("s21_0_%s"),m_pXmlParser->GetNodeText(_T("areaid"),pTemp));
// 			copyData(pGpsCmd->strCMDID,strCmdID);
// 
// 			//*XX,YYYYYYYYYY,S21,HHMMSS,Lx,M,D,Minlatitude,Maxlatitude,G,Minlongitude,Maxlongitude#
// 			//*HQ,6110916012,S21,130305,17,1,N,2245.318,2246.452,E,11233.232,11365.175#
// 			CTime curTime = CTime::GetCurrentTime();
// 			strCmdLine.Format(_T("*HQ,%s,S21,%s,%s,0,N,2245.318,2246.452,E,11233.232,11365.175#"),strDeviceID,curTime.Format(_T("%H%M%S")),
// 				m_pXmlParser->GetNodeText(_T("areaid"),pTemp)	);
// 			pGpsCmd->nLenCommandLine = copyData(pGpsCmd->strCommandLine,strCmdLine);
// 
// 			int nRecordID = CoverStringToInt(m_pXmlParser->GetNodeText(_T("id"),pTemp));
// 			pGpsCmd->pVoid = (int*)nRecordID;
// 			nRet = 1;
// /*
// 			<?xml version="1.0"?>
// 				<CancelArea>
// 				<ID>
// 				<!--区域ID-->
// 				</ID>
// 				<SIM>
// 				<!--sim卡号-->
// 				</SIM>
// 				<DeviceID>
// 				</DeviceID>
// 				<AreaID>
// 				</AreaID>
// 				</CancelArea>
// */
// 
// 		}
		if(_T("overspeed")==strRoot)
		{
			pGpsCmd->commandType = cmdType_ToService_OverSpeed;
			//*XX,YYYYYYYYYY,S14,HHMMSS,Max_speed,Min_speed,M,countinue #
			//*HQ,0000000000,S14,130305,100,10,1,3#
			if(!m_pXmlParser->Parse(strCMd)) break;

			strcpy(pGpsCmd->strCMDID,"S14");

			CString strDeviceID =m_pXmlParser->GetNodeText(_T("sim"),pTemp);
			copyData(pGpsCmd->strSim,strDeviceID);
			
			strDeviceID =m_pXmlParser->GetNodeText(_T("deviceid"),pTemp);
			copyData(pGpsCmd->strDevID,strDeviceID);

			copyData(pGpsCmd->strCMDID,(m_pXmlParser->GetNodeText(_T("id"),pTemp)));
			
			struct tag_OverSpeed 
			{
				int nMaxSpeed;
				int nMinSpeed;
				int nCountinue;
			};
			
			tag_OverSpeed *pstOverSpeed = new tag_OverSpeed;

			pstOverSpeed->nMaxSpeed = CoverStringToInt(m_pXmlParser->GetNodeText(_T("maxspeed"),pTemp));
			pstOverSpeed->nMinSpeed = CoverStringToInt(m_pXmlParser->GetNodeText(_T("minspeed"),pTemp));
			pstOverSpeed->nCountinue = CoverStringToInt(m_pXmlParser->GetNodeText(_T("countinue"),pTemp));
			
			pGpsCmd->pVoid = pstOverSpeed;
			//formatParam
// 			CString sParam;
// 			sParam.Format(_T("%s,%s,%s,%s"),
// 				m_pXmlParser->GetNodeText(_T("maxspeed"),pTemp),
// 			m_pXmlParser->GetNodeText(_T("minspeed"),pTemp),
// 			m_pXmlParser->GetNodeText(_T("m"),pTemp),
// 			m_pXmlParser->GetNodeText(_T("countinue"),pTemp));
// 
// 			pGpsCmd->nLenCmdParameters = copyData(pGpsCmd->commandParameters,sParam);
// 
// 			CTime curTime = CTime::GetCurrentTime();
// 			strCmdLine.Format(_T("*HQ,%s,S14,%s,%s#"),strDeviceID,curTime.Format(_T("%H%M%S")),sParam);
// 			pGpsCmd->nLenCommandLine = copyData(pGpsCmd->strCommandLine,strCmdLine);
			
			nRet = 1;
		}
		else
		{
			break;
		}

	} while (0);

	//if(nRet==1)
	{
		CString strLog;
		strLog.Format(_T("getCmd [%s]--%d-\r\n-%s"),strCMd,nRet,strCmdLine);
		WriteLog(_T("TSCmd"),logLevelError,strLog);
	}
	return nRet;
}
