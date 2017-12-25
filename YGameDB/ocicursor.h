#ifndef __OCI_CURSOR_H
#define __OCI_CURSOR_H

#include "ocidatabase.h"
#define MAX_COL_COUNT	32


#define STRING_SIZE	    64

bool SCheckError(MYSQL_STMT *stmt,int& errcode, char* errbuf);

class COCICursor
{
private:
	int m_errCode;
	char m_errBuf[512];
	MYSQL_BIND m_pDefine[MAX_COL_COUNT];
	MYSQL_BIND m_pBind[MAX_COL_COUNT];
	int m_nDefineCount;
	int m_nBindCount;
public:
	int GetLastError();
	char* GetErrorMessage();
	bool ExecQuery();
	int GetRecordCount();
	int GetDefineCount() { return m_nDefineCount; };
	int GetBindCount() { return m_nBindCount; };
	MYSQL_BIND* GetDefineHandleByIndex(int index);
	MYSQL_BIND* GetBindHandleByIndex(int index);
	void FreeDefineAndBindHandles();

	 
	void DefineRaw(int ColNum, char *pVal, int BufferLength);
	void DefineDate(int ColNum, char* pVal);
	//void DefineDate(int ColNum, MYSQL_TIME* pVal);
	void DefineDouble(int ColNum, double* pVal);
	void DefineFloat(int ColNum, float* pVal);
	void DefineString(int ColNum, char *pVal,int BufferLength);
	void DefineByte(int ColNum, char *pVal);
	void DefineShort(int ColNum, short *pVal);
	void DefineInt(int ColNum,int* pVal);
	void Define(int ColNum, void *pVal, unsigned long BufferLength, enum_field_types DataType);
	void Define(int ColNum, void *pVal, unsigned long BufferLength, enum_field_types DataType, my_bool isNull);
	bool DefineFinish();

	void BindRaw(int ColNum, char *pVal, unsigned long *BufferLength);
	void BindDate(int ColNum, char* pVal);
	void BindDouble(int ColNum, double* pVal);
	void BindFloat(int ColNum, float* pVal);
	void BindString(int ColNum, char *pVal,unsigned long *BufferLength);
	void BindByte(int ColNum, char *pVal);
	void BindShort(int ColNum, short *pVal);
	void BindInt(int ColNum, int* pVal);
	void Bind(int ColNum, void *pVal, enum_field_types DataType);
	void Bind(int ColNum, void *pVal, unsigned long *BufferLength, enum_field_types DataType);
	void Bind(int ColNum, void* pVal, unsigned long *BufferLength, enum_field_types DataType,my_bool isNull);
	bool BindFinish();

	bool Fetch();
	bool Execute();
	bool ExecWithoutCommit();
	bool Commit();
	bool Rollback();
	bool Prepare(const char* StmtText);
	unsigned long GetAffectedRowCount();

	bool ExecuteBatch(int elementSize, int execTimes);
	bool ExecWithoutCommit(int elementSize, int execTimes);
	
	void COCICursor_init(COCIDatabase* pDb);

	COCICursor(COCIDatabase* pDb);
	virtual ~COCICursor();
	MYSQL_RES *result;
	

private:
	int m_RecordCount;
	MYSQL_STMT* m_pStmt;
	COCIDatabase* m_pDb;
};

#endif
