///

// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTION_H__4381555C_32AD_4F54_9240_9FD5A00EA2AB__INCLUDED_)
#define AFX_CONNECTION_H__4381555C_32AD_4F54_9240_9FD5A00EA2AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <oci.h>


enum DBFieldType 
{
    FT_NULL,
    FT_BIT, 
    FT_CHAR,
    FT_STRING,
    FT_WSTRING,            //Wide String for unicode.
    FT_BLOB,
    FT_TIMESTAMP,
    FT_DATE,
    FT_TIME,
    FT_DATETIME,
    FT_FLOAT,
    FT_DOUBLE,
    FT_INTEGER,
    FT_SMALLINT,
    FT_WORD,
    FT_BOOLEAN
};


class CConnection  
{
public:
    CConnection();
    virtual ~CConnection();

    int connect(char *ServerAddr,char *vsp,char *user,char *passwd);
    int disconnect();
    int initial();
    int execute(char *query);    
    int getFieldsInfo(char *para);
    int Rtrim(char *para);
    int ConvertFeild(char *src,int srctype,char *dst,int length);
//    int disconnect();
    int getFields(char *para);
    int connect(char *Server, char *user,char *passwd);
    int commit();
    int rollback();
	const char * getErrorMsg(){return errMsg;};
    
public:
    OCIEnv      *myenvhp;     /* the environment handle */
    OCIError    *myerrhp;     /* the error handle */
    OCIServer   *mysrvhp;     /* the server handle */
    OCISession  *myusrhp;     /* user session handle */
    OCISvcCtx   *mysvchp;     /* the service handle */
    OCIStmt     *mystmthp;    /* the statment handle */
    OCITrans    *mytxnhp;     /* the transaction handle */    
    char        *stord_sql_stmt;
    int         fieldCount; //ÁÐÊý
    int         iRecord;
    char        ValueList[1000][512];
    
private:
    char errMsg[1024];
};

#endif // !defined(AFX_CONNECTION_H__4381555C_32AD_4F54_9240_9FD5A00EA2AB__INCLUDED_)


