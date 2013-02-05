// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Connection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnection::CConnection()
{
	errMsg[0]='\0';
}

CConnection::~CConnection()
{

}

//ServerAddr ip
//vsp dbname at the server machine
int CConnection::connect(char *ServerAddr,char *vsp,char *user,char *passwd)
{
    char connect_str[1024] = {0};

    int i = 0;
    try
    {    
        sprintf(connect_str, "(DESCRIPTION= (ADDRESS= (PROTOCOL= TCP)(HOST= %s)(PORT= 1521))(CONNECT_DATA= (SERVICE_NAME=%s)))",ServerAddr,vsp);
        i = connect(connect_str, user, passwd);
    }
    catch(...)
    {
        printf("catch a error!/n");
    }     
    
    return i;    
}


int CConnection::connect(char *Server, char *user,char *passwd)
{
//    char connect_str[1024] = {0};
    sword status;

    try
    {
        OCIEnvCreate(&myenvhp, OCI_DEFAULT|OCI_THREADED, (dvoid *)0, 0, 0, 0, (size_t)0, (dvoid **)0);
        
        OCIInitialize(OCI_THREADED|OCI_OBJECT, NULL, NULL, NULL, NULL);
        OCIEnvInit(&myenvhp, OCI_DEFAULT, 0, NULL);
            
        OCIHandleAlloc((dvoid*)myenvhp, (dvoid**)&myerrhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid**) 0);
        OCIHandleAlloc((dvoid *)myenvhp, (dvoid**)&mystmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid**)0);    
//        sprintf(connect_str, "(DESCRIPTION= (ADDRESS= (PROTOCOL= TCP)(HOST= %s)(PORT= 1521))(CONNECT_DATA= (SERVICE_NAME= %s)))",ServerAddr,vsp);

        
        OCIHandleAlloc((dvoid*)myenvhp, (dvoid**)&mysrvhp, OCI_HTYPE_SERVER, (size_t)0, (dvoid**) 0);
        OCIHandleAlloc((dvoid*)myenvhp, (dvoid**)&mysvchp, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid**) 0);        

        status = OCIServerAttach(mysrvhp, myerrhp, (text*)Server, strlen(Server), OCI_DEFAULT);
        if (status != OCI_SUCCESS)
        {
            sb4  errcodep;
            char errbuf[1024] = {0};
            
            OCIErrorGet(myerrhp, (ub4)1, NULL, &errcodep, (text*)errbuf, (ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
			sprintf(errMsg,"OCIServerAttach error[%s]!/nserver:%s",errbuf,Server);
			printf(errMsg);
            OCIHandleFree(mysrvhp, OCI_HTYPE_SERVER);

            return -2;
        }

        OCIAttrSet((dvoid*)mysvchp, OCI_HTYPE_SVCCTX, (dvoid*)mysrvhp, (ub4)0, OCI_ATTR_SERVER, myerrhp);
        OCIHandleAlloc((dvoid*)myenvhp, (dvoid**)&myusrhp, OCI_HTYPE_SESSION, (size_t)0, (dvoid**) 0);
        OCIAttrSet((dvoid*)myusrhp, OCI_HTYPE_SESSION, (dvoid*)user, (ub4)strlen(user), OCI_ATTR_USERNAME, myerrhp);
        OCIAttrSet((dvoid*)myusrhp, OCI_HTYPE_SESSION, (dvoid*)passwd, (ub4)strlen(passwd), OCI_ATTR_PASSWORD, myerrhp);

        status = OCISessionBegin(mysvchp, myerrhp, myusrhp, OCI_CRED_RDBMS, OCI_DEFAULT);
        if (status != OCI_SUCCESS)
        {
            sb4  errcodep;
            char errbuf[1024] = {0};
            
            OCIErrorGet(myerrhp, (ub4)1, NULL, &errcodep, (text*)errbuf, (ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
            sprintf(errMsg,"OCISessionBegin error[%s]!/n",errbuf);
			printf(errMsg);

            printf("OCISessionBegin failed!/n");
            OCIServerDetach(mysrvhp, myerrhp, OCI_DEFAULT);
            OCIHandleFree(myusrhp, OCI_HTYPE_SESSION);
            OCIHandleFree(mysvchp, OCI_HTYPE_SVCCTX);
            OCIHandleFree(mysrvhp, OCI_HTYPE_SERVER);

            return -1;
        }
        OCIAttrSet((dvoid*)mysvchp, OCI_HTYPE_SVCCTX, (dvoid*)myusrhp, (ub4)0, OCI_ATTR_SESSION, myerrhp);
                
        OCIHandleAlloc((dvoid *)myenvhp, (dvoid**)&mytxnhp, OCI_HTYPE_TRANS, (size_t)0, (dvoid**)0);
        OCIAttrSet((dvoid*)mysvchp, OCI_HTYPE_SVCCTX, (dvoid*)mytxnhp, (ub4)0, OCI_ATTR_TRANS, myerrhp);
    }
    catch(...)
    {
        printf("catch a error!/n");
    }     
    
    return 1;    
}

int CConnection::rollback()
{
    sword status = 0;
    status = OCITransRollback(mysvchp, myerrhp, OCI_DEFAULT);    
    if (status != OCI_SUCCESS)
    {
        printf("OCITransCommit error !/n");
        return -3;
    }
    return 1;
}

int CConnection::commit()
{
    sword status = 0;
    status = OCITransCommit(mysvchp, myerrhp, OCI_DEFAULT);    
    if (status != OCI_SUCCESS)
    {
        printf("OCITransCommit error !/n");
        return -3;
    }
    return 1;
}

int CConnection::execute(char *query)
{
    sword status;
    ub2   stType;
//    ub4   parmub4;

    if (stord_sql_stmt)
    {
//        free(stord_sql_stmt);
        delete stord_sql_stmt;
        stord_sql_stmt = NULL;
    }
//    stord_sql_stmt = (char*)malloc(strlen(query)+16);
    stord_sql_stmt = new char[strlen(query)+16];
    memset(stord_sql_stmt, 0, strlen(query)+16);
    strcpy(stord_sql_stmt, query);

    try
    {        
        status = OCIStmtPrepare(mystmthp, myerrhp, (text*)stord_sql_stmt, (ub4)strlen(stord_sql_stmt), OCI_NTV_SYNTAX, OCI_DEFAULT);
        if (status != OCI_SUCCESS)
        {
//            free(stord_sql_stmt);
            delete stord_sql_stmt;
            stord_sql_stmt = NULL;            
            printf("DBCON_QUERY_ERROR failed/n");
            return -1;
        }

        OCIAttrGet((dvoid*)mystmthp, OCI_HTYPE_STMT, (dvoid*)&stType, (size_t *)0, OCI_ATTR_STMT_TYPE, myerrhp);


        status = OCIStmtExecute(mysvchp, mystmthp, myerrhp, 1, 0, NULL, NULL, OCI_DEFAULT);
        // if status < 0, error
        if (status != OCI_SUCCESS)
        {
//            free(stord_sql_stmt);
            delete stord_sql_stmt;
            stord_sql_stmt = NULL;
            printf("OCIStmtExecute no select !/n");
//                return -2;
        }
        

         
    } 
    catch(...)
    {
        printf("An exception was thrown./n");
    }    
    
//    free(stord_sql_stmt);
    delete stord_sql_stmt;
    stord_sql_stmt = NULL;
    return 1;
}

int CConnection::disconnect()
{
    sword status;
    
    try
    {
        status = OCISessionEnd(mysvchp, myerrhp, myusrhp, OCI_DEFAULT);
        if (status != OCI_SUCCESS) 
        {
            printf("DBCON_ERROR_DISCONNECTING failed!/n");            
        }
        
        OCIServerDetach(mysrvhp, myerrhp, OCI_DEFAULT);
        OCIHandleFree(mytxnhp, OCI_HTYPE_TRANS);
        OCIHandleFree(myusrhp, OCI_HTYPE_SESSION);
        OCIHandleFree(mysrvhp, OCI_HTYPE_SERVER);
        OCIHandleFree(mysvchp, OCI_HTYPE_SVCCTX);

        OCIHandleFree(myerrhp, OCI_HTYPE_ERROR);
        OCIHandleFree(myenvhp, OCI_HTYPE_ENV);
        
        myerrhp = NULL;
        myenvhp = NULL;    
        mytxnhp = NULL;
        mysrvhp = NULL;
        myusrhp = NULL;
        mysvchp = NULL;    
    }
    catch(...)
    {
        printf("DBCON_ERROR_DISCONNECTING failed/n");
    }
    
    return 0;    
}
