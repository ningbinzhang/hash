#pragma once
#include "Hash/CRC32.h"
#include "Hash/md5.h"
#include "Hash/sha1.hpp"
#include "Hash/picosha2.h"
#include "Hash/sha512.h"
#include <list>
#include <afxmt.h>

using namespace std;

class IHashProcess
{
public:
	virtual BOOL OnProcess(LONG64 lIncrease) = 0;
};

class IHashCallback
{
public:
	virtual BOOL OnFinish(LPCTSTR lpText) = 0;
	virtual BOOL OnProcess(LONG64 lTol, LONG64 lCur) = 0;
};


class IHash
{
public:
	virtual int GetMinBlockSize() = 0;
	virtual int Init() = 0;
	virtual int Update(const void * lpData, int iSize) = 0;
	virtual int Finish() = 0;
	virtual CString GetHash() = 0;
	virtual CString GetName() = 0;
};


class CHashs
{
public:
	CHashs(void);
	~CHashs(void);

	BOOL AddHash(IHash * lpHash);

	BOOL CalcHashs(LPCTSTR lpFilePath, IHashProcess * pProcess = NULL);

	LONG64 GetFileSize();

	CString ExportHashText();
	CString ExportHashTextRtf();
	CString TextToRtfText(const CString & strText);


protected:
	int OnInit();
	int OnUpdate(const void * lpData, int iSize);
	int OnFinish();

	list<IHash*> m_listHash;
	CString m_strFilePath;
	LONG64 m_lFileSize;
	CTimeSpan m_tmUsed;
	volatile DWORD m_dwError;
};

class CHashsListAsyn : public IHashProcess
{
public:
	CHashsListAsyn();
	~CHashsListAsyn();

	BOOL AddFile(LPCTSTR lpFilePath);
	BOOL GetProcess(LONG64 & lTol, LONG64 & lCur);
	virtual BOOL CalcHashsList();
	BOOL SetCallback(IHashCallback * lpCallback);

	virtual BOOL OnProcess(LONG64 lIncrease);

	BOOL Clear();

protected:
	CString GetFirstFile();
	virtual BOOL CalcHashFile(LPCTSTR lpFilePath);
	BOOL CalcHashFolder(CString strFolder);

	list<CString> m_listFile;
	volatile LONG64 m_lTotalFiles;
	volatile LONG64 m_lCalcFiles;
	CCriticalSection m_lock;
	IHashCallback * m_lpCallback;
};


class CCrc32 : public IHash
{
public:
	CCrc32();
	virtual ~CCrc32();

	virtual int GetMinBlockSize();
	virtual int Init();
	virtual int Update(const void * lpData, int iSize);
	virtual int Finish();
	virtual CString GetHash();
	virtual CString GetName();
protected:
	CRC32 m_crc32;
	UINT32 m_nCrc32;

};

class CMd5 : public IHash
{
public:
	CMd5();
	virtual ~CMd5();

	virtual int GetMinBlockSize();
	virtual int Init();
	virtual int Update(const void * lpData, int iSize);
	virtual int Finish();
	virtual CString GetHash();
	virtual CString GetName();

protected:
	MD5_CTX m_md5;
	unsigned char m_strMd5[16];
};

class CSha1 : public IHash
{
public:
	CSha1();
	virtual ~CSha1();

	virtual int GetMinBlockSize();
	virtual int Init();
	virtual int Update(const void * lpData, int iSize);
	virtual int Finish();
	virtual CString GetHash();
	virtual CString GetName();

protected:
	SHA1 m_sha1;
	string m_strSha1;
};

class CSha256 : public IHash
{
public:
	CSha256();
	virtual ~CSha256();

	virtual int GetMinBlockSize();
	virtual int Init();
	virtual int Update(const void * lpData, int iSize);
	virtual int Finish();
	virtual CString GetHash();
	virtual CString GetName();

protected:
	
	picosha2::hash256_one_by_one m_sha256;
	char m_strSha256[32];
};

class CSha512 : public IHash
{
public:
	CSha512();
	virtual ~CSha512();

	virtual int GetMinBlockSize();
	virtual int Init();
	virtual int Update(const void * lpData, int iSize);
	virtual int Finish();
	virtual CString GetHash();
	virtual CString GetName();

protected:
	SHA512 m_sha512;
	unsigned char m_strSha512[64];
};