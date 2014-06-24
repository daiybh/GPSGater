GPSGater
========

GpsGater

1 所需环境 MSxml4.0(或以上)，vc++2008 sp1（或以上）
2 开发工具vs2008 sp1
3 工程文件 GPSGater.sln

4 编译顺序
	1 MGBaseModule（基础模块：日志，读配置文件）
	2 TSIPDevice   （从网络取数据模块,iocp）
	3 GPSGateAdpter (与curGps接口模块，并写数据库)
	4 GpsxProtocal_DL\curGps	（协议分析模块）
	5 GpsxProtocal_DL\curGpsxConsole （主程序）
