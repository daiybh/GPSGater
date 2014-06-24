GPSGater
========

GpsGater

1 所需环境 MSxml4.0(或以上)，vc++2008 sp1（或以上）
2 开发工具vs2008 sp1
3 工程文件 GPSGater.sln

4 编译顺序
	1 MGBaseModule（基础模块）
		XMLParser xml解析器
		MGBufferlistEx 内存链表
		MGLogWriter		写日志模块
	2 TSIPDevice   （从网络取数据模块,iocp）
		Socket_IOCP 接口类
	3 GPSGateAdpter (与curGps接口模块，并写数据库)
		CGPS_Socket	从TSIPDevice 模块交互，获取网络数据，下发网络数据
		DataOpter	从curGps模块获取解析完毕的数据，写入数据库
	4 GpsxProtocal_DL
		1 curGps	（协议分析模块,从GPSGateAdpter获取报文数据，解析成结构数据回写进GPSGateAdpter）
			干活线程：threadGetGPSData(LPVOID lpParameter)
						
		2 Protocol\ProtocolMan	(协议解析插件)
				GpsBase.h\GPSClass  协议插件的基础类
				GPS_MeiTrack.h		for meitrack gps
				GpsJTT808.h			for 808协议
				GpsXingRui.h		for 兴瑞
				
		3 curGpsxConsole （主程序）
