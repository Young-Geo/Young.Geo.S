#include "ocidatabase.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
//#include "frlib/utility.h"

#ifdef __OS_WIN32_
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

int OrclDateToTime_t(char* pVal)
{
	MYSQL_TIME tmp;	
	struct tm when;
	
    memcpy(&tmp, pVal, MYSQL_TIME_LEN);
	
	when.tm_year = tmp.year - 1900;
	when.tm_mon = tmp.month - 1;
	when.tm_mday = tmp.day;
	when.tm_hour = tmp.hour;
	when.tm_min = tmp.minute;
	when.tm_sec = tmp.second;
	return mkseconds(&when);
}

void OrclDateToTime_Sec_Msec(char* pVal,b_i32& Sec,b_i16& Msec)
{
	//    sb2 year;
	//    sb1 month, day, hour, min, sec,Msec;
	int year;
	int month, day, hour, min, sec,msec;
	sscanf(pVal,"%04d%02d%02d%02d%02d%02d.%06d",
			&year,&month, &day, &hour, &min, &sec,&msec);
	Msec = msec/1000;

	/*
	   char pDataValue[10];

	//year
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal,4);
	year = atoi(pDataValue);

	//month
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal + 4,2);
	month = atoi(pDataValue);

	//day
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal + 6,2);
	day = atoi(pDataValue);

	//hour
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal + 8,2);
	hour = atoi(pDataValue);

	//min
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal + 10,2);
	min = atoi(pDataValue);

	//second
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal + 12,2);
	sec = atoi(pDataValue);


	//Msec
	memset(pDataValue,'\0',sizeof(char) * 10);
	memcpy(pDataValue,pVal + 15,6);
	Msec = atoi(pDataValue);

*/
	struct tm when;
	when.tm_year = year - 1900;
	when.tm_mon = month - 1;
	when.tm_mday = day;
	when.tm_hour = hour;
	when.tm_min = min;
	when.tm_sec = sec;
	Sec = mkseconds(&when);
}

void Time_tToOrclDate(int t, char* pVal)
{
	struct tm when;

	gettime(t, &when);

    (*(MYSQL_TIME*)pVal).year = when.tm_year + 1900;
    (*(MYSQL_TIME*)pVal).month = when.tm_mon + 1;
    (*(MYSQL_TIME*)pVal).day = when.tm_mday;
    (*(MYSQL_TIME*)pVal).hour = when.tm_hour;
    (*(MYSQL_TIME*)pVal).minute = when.tm_min;
    (*(MYSQL_TIME*)pVal).second = when.tm_sec;
}


void OrclDateToIDPTime(char* pVal, int& ymd, int& hms)
{
	time_t t = OrclDateToTime_t(pVal);
	struct tm when;
	gettime(t, &when);
	
	int year;
	int month, day, hour, min, sec;

	year = when.tm_year + 1900;
	month = when.tm_mon + 1;
	day = when.tm_mday;
	hour = when.tm_hour;
	min = when.tm_min;
	sec = when.tm_sec;
	ymd = year * 10000 + month * 100 + day;
	hms = hour * 10000 + min * 100 + sec; 
}

void IDPTimeToOrclDate(int ymd, int hms, char* pVal)
{

	int year;
	int month, day, hour, min, sec;
	int val;

	val = ymd;
	year = val / 10000;
	val -= year * 10000;
	month = val / 100;
	val -= month * 100;
	day = val;

	val = hms;
	hour = val / 10000;
	val -= hour * 10000;
	min = val / 100;
	val -= min * 100;
	sec = val;

	struct tm when;
	when.tm_year = year - 1900;
	when.tm_mon = month - 1;
	when.tm_mday = day;
	when.tm_hour = hour;
	when.tm_min = min;
	when.tm_sec = sec;
	time_t t = mkseconds(&when);
	Time_tToOrclDate(t, pVal);
}


void Sec_Msec_Time_tToOrclDate(int t, char* pVal)
{
	struct tm when;

	gettime(t, &when);
	char *pTmpChar = pVal;

	sprintf(pTmpChar,"%04d",(when.tm_year + 1900));
	sprintf(pTmpChar+4,"%02d",(when.tm_mon + 1));
	sprintf(pTmpChar+6,"%02d",(when.tm_mday));

	sprintf(pTmpChar+8,"%02d",(when.tm_hour ));
	sprintf(pTmpChar+10,"%02d",(when.tm_min ));
	sprintf(pTmpChar+12,"%02d",(when.tm_sec ));

	sprintf(pTmpChar+14,".");

	for(int i = 0;i<6;i++)
	{
		sprintf(pTmpChar+15+i,"0");
	}
}


bool FCheckError(MYSQL *mysql,int& errcode, char* errbuf)
{
	errcode = mysql_errno(mysql);

	memset(errbuf, 0, 512);

	if (errcode)
	{
		strcpy(errbuf,mysql_error(mysql));
		return true;
	}
	else 
	{
		return false;
	}
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



COCIDatabase::COCIDatabase()
{

	mysql_init(&mysql);
	m_IsOpen = false;
	m_errCode = 0;
}

COCIDatabase::~COCIDatabase()
{
	Close();
}

bool COCIDatabase::IsOpen()
{
	return  m_IsOpen;
}


void COCIDatabase::Close()
{
	mysql_close(&mysql);
	m_IsOpen = false;
}



bool COCIDatabase::Open(const char *csid,const char *cuser, const char *cpasswd)
{
	if (IsOpen())
	{
		return true;
	}

	char *sid = const_cast<char*>(csid);
	char *user= const_cast<char*>(cuser);
	char *passwd = const_cast<char*>(cpasswd);

	char *tmp = strchr(sid, '/') + 2;
	char *db = strchr(tmp, '/') + 1;

	char host[16] = {0};
	int length = db - tmp - 1;
	strncpy(host,tmp, length);
	host[length] = '\0';


	if (!mysql_real_connect(&mysql, host, user, passwd, db, 0, NULL, 0))
	{
		FCheckError(&mysql, m_errCode, m_errBuf);
		return false;
	}

    if (mysql_set_character_set(&mysql, "gb2312") != 0)
    {
        FCheckError(&mysql, m_errCode, m_errBuf);
        return false;
    }

	m_IsOpen = true;
	return true;
}


bool COCIDatabase::BeginTrans()
{
	if (IsOpen())
	{
		if (mysql_real_query(&mysql, "set autocommit=0", (unsigned int)strlen("set autocommit=0")))
		{
			FCheckError(&mysql, m_errCode, m_errBuf);
			return false;
		}
		else 
		{
			return true;
		}
	}
	else 
	{
		return false;
	}
}

bool COCIDatabase::CommitTrans()
{
	if (IsOpen())
	{
		if (mysql_real_query(&mysql, "commit;", (unsigned int)strlen("commit;")))
		{
			FCheckError(&mysql, m_errCode, m_errBuf);
			return false;
		}
		else 
		{
			return true;
		}
	}
	else 
	{
		return false;
	}
}

bool COCIDatabase::RollbackTrans()
{
	if (IsOpen())
	{
		if (mysql_real_query(&mysql, "rollback;", (unsigned int)strlen("rollback;")))
		{
			FCheckError(&mysql, m_errCode, m_errBuf);
			return false;
		}
		else 
		{
			return true;
		}
	}
	else 
	{
		return false;
	}
}


int COCIDatabase::GetLastError()
{
	return m_errCode;
}

char* COCIDatabase::GetErrorMessage()
{
	return m_errBuf;
}

void COCIDatabase::SetLastError(int errCode, char* errBuf)
{
	m_errCode = errCode;
	strncpy(m_errBuf, errBuf, 512);
}


