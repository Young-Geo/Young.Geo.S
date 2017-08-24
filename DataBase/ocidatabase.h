#ifndef __OCI_DATABASE_H
#define __OCI_DATABASE_H



#include "mysql/mysql.h"

#ifdef __OS_WIN32_
#define MYSQL_TIME_LEN	36
#else
#define MYSQL_TIME_LEN	40
#endif


#define ONE_DAY_TIME	86400

int OrclDateToTime_t(char* pVal);
void OrclDateToTime_Sec_Msec(char* pVal, int& Sec, short& Msec);
void Time_tToOrclDate(int t, char* pVal);
void OrclDateToIDPTime(char* pVal, int& ymd, int& hms);
void IDPTimeToOrclDate(int ymd, int hms, char* pVal);
void Sec_Msec_Time_tToOrclDate(int t, char* pVal);


bool FCheckError(MYSQL *mysql,int& errcode, char* errbuf);

class COCIDatabase
{
public:
	COCIDatabase();
	virtual ~COCIDatabase();
	
	void COCIDatabase_init();
	int GetLastError();
    char* GetErrorMessage();
    bool RollbackTrans();
	bool CommitTrans();
	bool BeginTrans();
	void Close();
	bool IsOpen();
    bool Open(const char *csid,const char *cuser, const char *cpasswd);
  
public:
	void SetLastError(int errCode, char* errBuf);
	
	MYSQL mysql;
  
protected:

private:
	bool m_IsOpen;
	int m_errCode;
	char m_errBuf[512];
	
};


#endif
