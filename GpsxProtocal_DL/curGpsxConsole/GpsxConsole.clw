; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGpsxConsoleDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "GpsxConsole.h"

ClassCount=3
Class1=CGpsxConsoleApp
Class2=CGpsxConsoleDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_GPSXCONSOLE_DIALOG

[CLS:CGpsxConsoleApp]
Type=0
HeaderFile=GpsxConsole.h
ImplementationFile=GpsxConsole.cpp
Filter=N

[CLS:CGpsxConsoleDlg]
Type=0
HeaderFile=GpsxConsoleDlg.h
ImplementationFile=GpsxConsoleDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_PROGRUN

[CLS:CAboutDlg]
Type=0
HeaderFile=GpsxConsoleDlg.h
ImplementationFile=GpsxConsoleDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_GPSXCONSOLE_DIALOG]
Type=1
Class=CGpsxConsoleDlg
ControlCount=5
Control1=IDC_BSTART,button,1342242816
Control2=IDC_BSTOP,button,1342242816
Control3=IDC_PROGRUN,msctls_progress32,1342177280
Control4=IDC_EINFO,edit,1342244992
Control5=IDC_SSTARTTIME,static,1342308352

