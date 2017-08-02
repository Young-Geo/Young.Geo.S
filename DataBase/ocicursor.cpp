#include "ocicursor.h"
#include <string.h>


bool SCheckError(MYSQL_STMT *stmt,int& errcode, char* errbuf)
{
	errcode = mysql_stmt_errno(stmt);

	memset(errbuf, 0, 512);

	if (errcode)
	{
		strcpy(errbuf,mysql_stmt_error(stmt));
		return true;
	}
	else 
	{
		return false;
	}
}


COCICursor::COCICursor( COCIDatabase* pDb )
{
	m_pDb = pDb;
	m_pStmt = 0;
	m_errCode = 0;
	result = NULL;

	FCheckError(&(m_pDb->mysql),m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode, m_errBuf);
	m_pStmt = mysql_stmt_init(&(m_pDb->mysql));
	SCheckError(m_pStmt,m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode, m_errBuf);

	m_nDefineCount = 0;
	m_nBindCount = 0;	
    memset(m_pDefine, 0, MAX_COL_COUNT * sizeof(MYSQL_BIND));
    memset(m_pBind, 0, MAX_COL_COUNT * sizeof(MYSQL_BIND));
	m_RecordCount = -1;

}

COCICursor::~COCICursor()
{
	FreeDefineAndBindHandles();

	if (m_pStmt != NULL)
	{
		mysql_stmt_close(m_pStmt);
		m_pStmt = NULL;
	}

}

void COCICursor::FreeDefineAndBindHandles()
{
	if (m_pStmt)
	{
		mysql_stmt_free_result(m_pStmt);
	}
//	if (result)
//	{
//		mysql_free_result(result);
//	}
	m_nDefineCount = 0;
	m_nBindCount = 0;	
    memset(m_pDefine, 0, MAX_COL_COUNT * sizeof(MYSQL_BIND));
    memset(m_pBind, 0, MAX_COL_COUNT * sizeof(MYSQL_BIND));
}

MYSQL_BIND* COCICursor::GetDefineHandleByIndex( int index )
{
	if (index < 0 || index >=m_nDefineCount)
	{
		return NULL;
	}
	return &m_pDefine[index];

}

MYSQL_BIND* COCICursor::GetBindHandleByIndex( int index )
{
	if (index < 0 || index >= m_nBindCount)
	{
		return NULL;
	}
	return &m_pBind[index];
}

bool COCICursor::Prepare( const char* StmtText )
{
	mysql_stmt_prepare(m_pStmt,StmtText,(unsigned long)strlen(StmtText));
	bool haveError = SCheckError(m_pStmt,m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	if (haveError)
	{
		FreeDefineAndBindHandles();
	}
	return !haveError;

}

bool COCICursor::ExecWithoutCommit()
{
	m_pDb->BeginTrans();
	mysql_stmt_execute(m_pStmt);
	bool IsError = SCheckError(m_pStmt,m_errCode,m_errBuf);
	return !IsError;
}


bool COCICursor::Commit()
{
	bool haveError = m_pDb->CommitTrans();
	return haveError;
}

bool COCICursor::Rollback()
{
	bool haveError = m_pDb->RollbackTrans();
	return haveError;
}

bool COCICursor::Execute()
{
	mysql_stmt_execute(m_pStmt);
	bool haveError = SCheckError(m_pStmt,m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	
	return !haveError;
}

bool COCICursor::ExecQuery()
{
	result = mysql_stmt_result_metadata(m_pStmt);
	bool haveError = SCheckError(m_pStmt,m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	if (!haveError)
	{
		mysql_stmt_execute(m_pStmt);
		bool isError = SCheckError(m_pStmt,m_errCode,m_errBuf);
		m_pDb->SetLastError(m_errCode,m_errBuf);
		if (!isError)
		{
			mysql_stmt_store_result(m_pStmt);
		}
		return !isError;	
	}
	return !haveError;
}

bool COCICursor::Fetch()
{
	int res = mysql_stmt_fetch(m_pStmt);
	SCheckError(m_pStmt,m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	if (res == MYSQL_NO_DATA)
	{
		m_pDb->SetLastError(MYSQL_NO_DATA, "MYSQL_NO_DATA");
	}
	else if (res == MYSQL_DATA_TRUNCATED)
	{
		m_pDb->SetLastError(MYSQL_DATA_TRUNCATED, "MYSQL_DATA_TRUNCATED");
	}
	if (!res)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void COCICursor::Bind(int ColNum, void *pVal, enum_field_types DataType)
{
	m_pBind[ColNum-1].buffer_type = DataType;
	m_pBind[ColNum-1].buffer = (char *)pVal;
	m_pBind[ColNum-1].buffer_length = STRING_SIZE;
	m_pBind[ColNum-1].is_null = 0;
	m_pBind[ColNum-1].length = 0;
	m_nBindCount++;

}

void COCICursor::Bind( int ColNum, void *pVal, unsigned long *BufferLength, enum_field_types DataType )
{
	m_pBind[ColNum-1].buffer_type = DataType;
	m_pBind[ColNum-1].buffer = (char *)pVal;
	m_pBind[ColNum-1].buffer_length = STRING_SIZE;
	m_pBind[ColNum-1].is_null = 0;
	m_pBind[ColNum-1].length = BufferLength;
	m_nBindCount++;
	
}

void COCICursor::Bind( int ColNum, void* pVal, unsigned long *BufferLength, enum_field_types DataType,my_bool isNull )
{
	m_pBind[ColNum-1].buffer_type = DataType;
	m_pBind[ColNum-1].buffer = (char *)pVal;
	m_pBind[ColNum-1].buffer_length = STRING_SIZE;
	m_pBind[ColNum-1].is_null = &isNull;
	m_pBind[ColNum-1].length = 0;
	if ((DataType == MYSQL_TYPE_STRING) || (DataType == MYSQL_TYPE_TINY_BLOB))
	{
		m_pBind[ColNum-1].length = BufferLength;
	}
	m_nBindCount++;
}

bool COCICursor::BindFinish()
{
	mysql_stmt_bind_param(m_pStmt,m_pBind);
	bool haveError = SCheckError(m_pStmt, m_errCode, m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	return !haveError;
}

void COCICursor::BindInt( int ColNum, int* pVal )
{
	Bind(ColNum,(void*)pVal,MYSQL_TYPE_LONG);
}

void COCICursor::BindShort( int ColNum, short *pVal )
{
	Bind(ColNum,(void*)pVal,MYSQL_TYPE_SHORT);
}

void COCICursor::BindByte( int ColNum, char *pVal )
{
	Bind(ColNum,(void*)pVal,MYSQL_TYPE_TINY);
}

void COCICursor::BindString( int ColNum, char *pVal,unsigned long *BufferLength )
{
	Bind(ColNum, (void*)pVal, BufferLength, MYSQL_TYPE_STRING);
}

void COCICursor::BindFloat( int ColNum, float* pVal )
{
	Bind(ColNum,(void*)pVal,MYSQL_TYPE_FLOAT);
}

void COCICursor::BindDouble( int ColNum, double* pVal )
{
	Bind(ColNum,(void*)pVal,MYSQL_TYPE_DOUBLE);
}

void COCICursor::BindRaw( int ColNum, char *pVal, unsigned long *BufferLength )
{
	Bind(ColNum,(void*)pVal, BufferLength,MYSQL_TYPE_TINY_BLOB);
}

void COCICursor::BindDate( int ColNum, char* pVal )
{
	//unsigned long length = sizeof(MYSQL_TIME);
	Bind(ColNum,(void*)pVal,MYSQL_TYPE_DATETIME);
}

void COCICursor::Define( int ColNum, void *pVal, unsigned long BufferLength, enum_field_types DataType )
{
	m_pDefine[ColNum-1].buffer_type = DataType;
	m_pDefine[ColNum-1].buffer = (char *)pVal;
	m_pDefine[ColNum-1].buffer_length = STRING_SIZE;
  //m_pDefine[ColNum-1].length = &BufferLength; 17227
  if (DataType == MYSQL_TYPE_STRING || DataType == MYSQL_TYPE_TINY_BLOB)
  {
  	m_pDefine[ColNum-1].length = &BufferLength;
  }
  m_nDefineCount++;
  //

}

void COCICursor::Define( int ColNum, void *pVal, unsigned long BufferLength, enum_field_types DataType, my_bool isNull )
{

	m_pDefine[ColNum-1].buffer_type = DataType;
	m_pDefine[ColNum-1].buffer = (char *)pVal;
	m_pDefine[ColNum-1].buffer_length = STRING_SIZE;
	m_pDefine[ColNum-1].is_null = &isNull;
	m_pDefine[ColNum-1].length = &BufferLength;
	m_nDefineCount++;

}

bool COCICursor::DefineFinish()
{
	mysql_stmt_bind_result(m_pStmt,m_pDefine);
	bool haveError = SCheckError(m_pStmt, m_errCode, m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	return !haveError;
}

void COCICursor::DefineInt( int ColNum,int* pVal )
{
	Define(ColNum,(void *)pVal,sizeof(int),MYSQL_TYPE_LONG);
}
void COCICursor::DefineShort( int ColNum, short *pVal )
{
	Define(ColNum,(void*)pVal,sizeof(short),MYSQL_TYPE_SHORT);
}

void COCICursor::DefineByte( int ColNum, char *pVal )
{
	Define(ColNum,(void*)pVal,sizeof(char),MYSQL_TYPE_TINY);
}

void COCICursor::DefineString( int ColNum, char *pVal,int BufferLength )
{
	Define(ColNum,(void*)pVal,BufferLength,MYSQL_TYPE_STRING);
}

void COCICursor::DefineFloat( int ColNum, float* pVal )
{
	Define(ColNum,(void*)pVal,sizeof(float),MYSQL_TYPE_FLOAT);
}

void COCICursor::DefineDouble( int ColNum, double* pVal )
{
	Define(ColNum,(void*)pVal,sizeof(double),MYSQL_TYPE_DOUBLE);
}

void COCICursor::DefineDate( int ColNum, char* pVal )
{
	unsigned long length = sizeof(MYSQL_TIME);
	Define(ColNum,(void*)pVal,length,MYSQL_TYPE_DATETIME);
}
//void COCICursor::DefineDate( int ColNum, MYSQL_TIME* pVal )
//{
//	Define(ColNum,(void*)pVal,8,MYSQL_TYPE_DATETIME);
//}

void COCICursor::DefineRaw( int ColNum, char *pVal, int BufferLength )
{
	Define(ColNum,(void*)pVal,BufferLength,MYSQL_TYPE_TINY_BLOB);
}



unsigned long COCICursor::GetAffectedRowCount()
{
	unsigned long rowCount = 0;
	rowCount = (unsigned long)mysql_stmt_affected_rows(m_pStmt);
	bool haveError = SCheckError(m_pStmt,m_errCode,m_errBuf);
	m_pDb->SetLastError(m_errCode,m_errBuf);
	if (!haveError)
	{
		return rowCount;
	}
	else
	{
		return 0;
	}

}


bool COCICursor::ExecuteBatch(int elementSize, int execTimes)
{
	int i,j;
	int columnCount = mysql_stmt_param_count(m_pStmt);

	for (i = 0; i < execTimes; i++)
	{
		for (j = 0; j < columnCount; j++)
		{
			if (i > 0)
			{
				m_pBind[j].buffer = (char *)m_pBind[j].buffer + elementSize;
			}
			if ((m_pBind[j].buffer_type == MYSQL_TYPE_STRING) || (m_pBind[j].buffer_type == MYSQL_TYPE_TINY_BLOB))
			{
				unsigned long length = strlen((char*)m_pBind[j].buffer);
				m_pBind[j].length = &length;
			}
		}
		BindFinish();
		if (!Execute())
		{
			break;
		}
		
	}
	if (i == execTimes)
	{
		return true;
	}
	return false;
	
}

bool COCICursor::ExecWithoutCommit(int elementSize, int execTimes)
{
	int i,j;
	int columnCount = mysql_stmt_param_count(m_pStmt);

	for (i = 0; i < execTimes; i++)
	{
		for (j = 0; j < columnCount; j++)
		{
			if (i > 0)
			{
				m_pBind[j].buffer = (char *)m_pBind[j].buffer + elementSize;
			}
			if ((m_pBind[j].buffer_type == MYSQL_TYPE_STRING) || (m_pBind[j].buffer_type == MYSQL_TYPE_TINY_BLOB))
			{
				unsigned long length = strlen((char*)m_pBind[j].buffer);
				m_pBind[j].length = &length;
			}
		}
		BindFinish();

		if (!Execute())
		{
			break;
		}
		
	}
	if (i == execTimes)
	{
		return true;
	}
	return false;
	
}


int COCICursor::GetRecordCount()
{
	return m_RecordCount;
}

int COCICursor::GetLastError()
{
	return m_errCode;
}


char* COCICursor::GetErrorMessage()
{
	return m_errBuf;
}
