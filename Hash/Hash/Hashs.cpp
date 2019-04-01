#include "StdAfx.h"
#include "Hashs.h"
#include "Hash.h"
#include <Shlwapi.h>

static char hb2hex(unsigned char hb) {
	hb = hb & 0xF;
	return hb < 10 ? '0' + hb : hb - 10 + 'A';
}

template<typename str>
static bool Hex2Str(LPBYTE lpBuf, int iSize, str & strHex)
{
	for(int i = 0; i < iSize; ++ i) {
		strHex += (hb2hex(lpBuf[i] >> 4));
		strHex += (hb2hex(lpBuf[i]));
	}
	return true;
}

CHashs::CHashs(void)
: m_lFileSize(0)
, m_dwError(0)
{
}

CHashs::~CHashs(void)
{
}

BOOL CHashs::CalcHashs( LPCTSTR lpFilePath , IHashProcess * pProcess)
{
	m_strFilePath.Empty();
	m_lFileSize = 0;
	m_dwError = 0;

	CTime t1 = CTime::GetCurrentTime();
	if (OnInit() != 0)
	{
		m_dwError = -1;
		return FALSE;
	}

	CFile f;
	if (f.Open(lpFilePath, CFile::modeRead|CFile::shareDenyWrite))
	{
		m_strFilePath = lpFilePath;
		LONG64 lSize= f.GetLength();
		m_lFileSize = lSize;
		LONG64 lReads = 0;
		int iRead = 0;
		const int iBufSize = 640 * 1024;
		BYTE * lpBuffer = new BYTE[iBufSize];
		
		while (lReads < lSize)
		{
			iRead = f.Read(lpBuffer, iBufSize);
			if (iRead == 0)
			{
				break;
			}
			if (iRead < iBufSize)
			{
				memset(lpBuffer+iRead, 0, min(128, iBufSize-iRead));
			}
			OnUpdate(lpBuffer, iRead);
			lReads += iRead;
			if (pProcess != NULL)
			{
				pProcess->OnProcess(iRead);
			}
		}
		delete [] lpBuffer;
		OnFinish();
		f.Close();

		CTime t2 = CTime::GetCurrentTime();
		m_tmUsed = t2 - t1;
		return TRUE;
	}else{
		TRACE(_T("Error: %d/n"), GetLastError());
		m_dwError = GetLastError();
	}
	return FALSE;
}

int CHashs::OnInit()
{
	if (m_listHash.empty())
	{
		return -1;
	}

	for (list<IHash*>::iterator iter = m_listHash.begin(); 
		iter != m_listHash.end();
		iter ++)
	{
		ASSERT((*iter) != NULL);
		(*iter)->Init();
	}
	return 0;
}

int CHashs::OnUpdate( const void * lpData, int iSize )
{
	for (list<IHash*>::iterator iter = m_listHash.begin(); 
		iter != m_listHash.end();
		iter ++)
	{
		ASSERT((*iter) != NULL);
		(*iter)->Update(lpData, iSize);
	}
	return 0;
}

int CHashs::OnFinish()
{
	for (list<IHash*>::iterator iter = m_listHash.begin(); 
		iter != m_listHash.end();
		iter ++)
	{
		ASSERT((*iter) != NULL);
		(*iter)->Finish();
	}
	return 0;
}

BOOL CHashs::AddHash( IHash * lpHash )
{
	if (lpHash == NULL)
	{
		return FALSE;
	}
	m_listHash.push_back(lpHash);
	return TRUE;
}

LONG64 CHashs::GetFileSize()
{
	return m_lFileSize;
}

CString CHashs::ExportHashText()
{
	if (m_dwError != 0)
	{
		return _T("计算HASH出错，文件可能无法打开！！！");
	}

	CString strFileSize;
	StrFormatByteSize(m_lFileSize, strFileSize.GetBuffer(128), 128);
	strFileSize.ReleaseBuffer();

	std::wstringstream sstream;
	sstream << _T("文件名：") << (LPCTSTR)m_strFilePath << _T("\r\n");
	sstream << _T("文件大小：") << (LPCTSTR)strFileSize << _T("(") << m_lFileSize << _T(")\r\n");
	sstream << _T("计算时间：") << m_tmUsed.GetTotalSeconds() << _T("秒") << _T("\r\n");
	
	for (list<IHash*>::iterator iter =  m_listHash.begin();
		iter != m_listHash.end();
		iter ++)
	{
		sstream << (LPCTSTR)(*iter)->GetName() << _T("：") << (LPCTSTR)(*iter)->GetHash().MakeUpper() << _T("\r\n");
	}

	return sstream.str().c_str();
}

CString CHashs::ExportHashTextRtf()
{
	std::wstringstream sstream;
	sstream << _T("{\\rtf1\\ansi\\ansicpg936\\deff0\\deflang1033\\deflangfe2052{\\fonttbl{\\f0\\fnil\\fprq1\\fcharset134 \\'d0\\'c2\\'cb\\'ce\\'cc\\'e5;}{\\f1\\fmodern\\fprq6\\fcharset134 \\'cb\\'ce\\'cc\\'e5;}}") << _T("\r\n");
	sstream << _T("{\\colortbl ;\\red240\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green128\\blue0;\\red0\\green0\\blue0;}") << _T("\r\n");
	sstream << _T("\\viewkind4\\uc1\\pard\\cf1\\lang1024");

	sstream << _T("\\cf1 ") << (LPCTSTR)TextToRtfText(_T("文件名：") + m_strFilePath + _T("\r\n")) << _T("\\cf0 ");
	sstream << _T("\\cf3 ")<< (LPCTSTR)TextToRtfText(_T("文件字节数：")) << _T("\\cf0 ") << m_lFileSize << (LPCTSTR)TextToRtfText(_T("\r\n"));

	for (list<IHash*>::iterator iter =  m_listHash.begin();
		iter != m_listHash.end();
		iter ++)
	{
		sstream << _T("\\cf3 ")<< (LPCTSTR)TextToRtfText((*iter)->GetName() + _T("：") ) << _T("\\cf0 ") << (LPCTSTR)TextToRtfText((*iter)->GetHash() + _T("\r\n")) ;
	}

	return sstream.str().c_str();
}

CString CHashs::TextToRtfText( const CString & strText )
{
	CString strRtfText;
	CStringA str = CW2A(strText);
	CString strNumber;
	unsigned char c;
	for (int i=0; i<str.GetLength(); i++)
	{
		c = str.GetAt(i);
		if (isalnum(c))
		{
			strRtfText += c;
		}else if (c == _T('\r'))
		{
			strRtfText += _T("\\par");
		}else if (c == _T('\n'))
		{
			strRtfText += _T("\r\n");
		}else if (c == _T('\\'))
		{
			strRtfText += _T("\\\\");
		}else if (c == _T('{'))
		{
			strRtfText += _T("\\{");
		}else if (c == _T('}'))
		{
			strRtfText += _T("\\}");
		}else
		{
			strNumber.Format(_T("\\'%02x"), c&0xff);
			strRtfText += strNumber;
		}
	}
	return strRtfText;
}

CCrc32::CCrc32()
: m_nCrc32(0)
{

}

CCrc32::~CCrc32()
{

}

int CCrc32::GetMinBlockSize()
{
	return 4;
}

int CCrc32::Init()
{
	m_crc32.reset();
	m_nCrc32 = 0;
	return 0;
}

int CCrc32::Update( const void * lpData, int iSize )
{
	m_crc32.update(lpData, iSize);
	return 0;
}

int CCrc32::Finish()
{
	UINT ncrc = m_crc32.finalize();

	((UINT8*)&m_nCrc32)[3] = ((UINT8*)&ncrc)[0];
	((UINT8*)&m_nCrc32)[2] = ((UINT8*)&ncrc)[1];
	((UINT8*)&m_nCrc32)[1] = ((UINT8*)&ncrc)[2];
	((UINT8*)&m_nCrc32)[0] = ((UINT8*)&ncrc)[3];

	return 0;
}

CString CCrc32::GetHash()
{
	CString strHash;
	Hex2Str((LPBYTE)&m_nCrc32, sizeof(m_nCrc32), strHash);
	return strHash;
}

CString CCrc32::GetName()
{
	return _T("CRC32");
}

CMd5::CMd5()
{

}

CMd5::~CMd5()
{

}

int CMd5::GetMinBlockSize()
{
	return 16;
}

int CMd5::Init()
{
	MD5_Init(&m_md5);
	ZeroMemory(&m_strMd5, sizeof(m_strMd5));
	return 0;
}

int CMd5::Update( const void * lpData, int iSize )
{
	MD5_Update(&m_md5, lpData, iSize);
	return 0;
}

int CMd5::Finish()
{
	MD5_Final(m_strMd5, &m_md5);
	return 0;
}

CString CMd5::GetHash()
{
	CString strHash;
	Hex2Str(m_strMd5, sizeof(m_strMd5), strHash);
	return strHash;
}

CString CMd5::GetName()
{
	return _T("MD5");
}


class membuf : public std::basic_streambuf<char> {
public:
	membuf(const uint8_t *p, size_t l) {
		setg((char*)p, (char*)p, (char*)p + l);
	}
};
class memstream : public std::istream {
public:
	memstream(const uint8_t *p, size_t l) :
	  std::istream(&_buffer),
		  _buffer(p, l) {
			  rdbuf(&_buffer);
	  }

private:
	membuf _buffer;
};

CSha1::CSha1()
{

}

CSha1::~CSha1()
{

}

int CSha1::GetMinBlockSize()
{
	return 20;
}

int CSha1::Init()
{
	m_strSha1.clear();
	return 0;
}

int CSha1::Update( const void * lpData, int iSize )
{	
	memstream s((const uint8_t *)lpData, iSize);

	m_sha1.update(s);

	return 0;
}

int CSha1::Finish()
{
	m_strSha1 = m_sha1.final();
	return 0;
}

CString CSha1::GetHash()
{
	return (LPCTSTR)CA2W((LPCSTR)m_strSha1.c_str());
}

CString CSha1::GetName()
{
	return _T("SHA1");
}

CSha256::CSha256()
{

}

CSha256::~CSha256()
{

}

int CSha256::GetMinBlockSize()
{
	return 256/8;
}

int CSha256::Init()
{
	m_sha256.init();
	ZeroMemory(m_strSha256, sizeof(m_strSha256));
	return 0;
}

int CSha256::Update( const void * lpData, int iSize )
{
	const char * lpBuf = (const char *)lpData;
	m_sha256.process(lpBuf, lpBuf+iSize);
	return 0;
}

int CSha256::Finish()
{
	m_sha256.finish();
	m_sha256.get_hash_bytes(m_strSha256, m_strSha256 + 32);
	return 0;
}

CString CSha256::GetHash()
{
	CString strHash;
	Hex2Str((LPBYTE)m_strSha256, sizeof(m_strSha256), strHash);
	return strHash;
}

CString CSha256::GetName()
{
	return _T("SHA256");
}

CSha512::CSha512()
{

}

CSha512::~CSha512()
{

}

int CSha512::GetMinBlockSize()
{
	return 512/8;
}

int CSha512::Init()
{
	m_sha512.init();
	ZeroMemory(m_strSha512, sizeof(m_strSha512));
	return 0;
}

int CSha512::Update( const void * lpData, int iSize )
{
	m_sha512.update((const unsigned char * )lpData, iSize);
	return 0;
}

int CSha512::Finish()
{
	m_sha512.final(m_strSha512);
	return 0;
}

CString CSha512::GetHash()
{
	CString strHash;
	Hex2Str(m_strSha512, sizeof(m_strSha512), strHash);
	return strHash;
}

CString CSha512::GetName()
{
	return _T("SHA512");
}

CHashsListAsyn::CHashsListAsyn()
: m_lCalcFiles(0)
, m_lTotalFiles(0)
, m_lpCallback(NULL)
{

}

CHashsListAsyn::~CHashsListAsyn()
{

}

BOOL CHashsListAsyn::AddFile( LPCTSTR lpFilePath )
{
	WIN32_FIND_DATA wd = {0};
	HANDLE h = FindFirstFile(lpFilePath, &wd);
	if (h != NULL)
	{
		m_lock.Lock();
		m_lTotalFiles += wd.nFileSizeLow;
		m_lTotalFiles += ((LONG64)wd.nFileSizeHigh) << 32;
		m_listFile.push_back(lpFilePath);
		m_lock.Unlock();
		FindClose(h);
		return TRUE;
	}
	return FALSE;
}

BOOL CHashsListAsyn::GetProcess( LONG64 & lTol, LONG64 & lCur )
{
	m_lock.Lock();
	lTol = m_lTotalFiles;
	lCur = m_lCalcFiles;
	m_lock.Unlock();
	return TRUE;
}

BOOL CHashsListAsyn::CalcHashsList()
{
	CString strPath = GetFirstFile();
	while(!strPath.IsEmpty())
	{
		if(GetFileAttributes(strPath) & FILE_ATTRIBUTE_DIRECTORY)
		{
			CalcHashFolder(strPath);
		}else{
			CalcHashFile(strPath);
		}

		strPath = GetFirstFile();
	}

	if (m_lpCallback != NULL)
	{
		m_lpCallback->OnFinish(NULL);
	}

	return 0;
}

BOOL CHashsListAsyn::CalcHashFile(LPCTSTR lpFilePath)
{
	CHashs h;
	CCrc32 crc32;
	CMd5 md5;
	CSha1 sha1;
	CSha256 sha256;
	CSha512 sha512;

	if (theApp.GetProfileInt(_T("Hash"), _T("Crc32"), 1)) h.AddHash(&crc32);
	if (theApp.GetProfileInt(_T("Hash"), _T("Md5"), 1)) h.AddHash(&md5);
	if (theApp.GetProfileInt(_T("Hash"), _T("Sha1"), 1)) h.AddHash(&sha1);
	if (theApp.GetProfileInt(_T("Hash"), _T("Sha256"), 1)) h.AddHash(&sha256);
	if (theApp.GetProfileInt(_T("Hash"), _T("Sha512"), 1)) h.AddHash(&sha512);

	h.CalcHashs(lpFilePath, this);

	CString strHash;
	strHash = h.ExportHashText();

	if (m_lpCallback != NULL)
	{
		m_lpCallback->OnFinish(strHash);
		m_lpCallback->OnProcess(m_lTotalFiles, m_lCalcFiles);
	}
	return TRUE;
}

BOOL CHashsListAsyn::CalcHashFolder(CString strFolder)
{
	CString strFindFolder;
	if (strFolder.GetAt(strFolder.GetLength()-1) == _T('\\'))
	{
		strFindFolder = strFolder + _T("*");
	}else{
		strFindFolder = strFolder + _T("\\*");
	}

	CFileFind finder;
	BOOL bWorking = finder.FindFile(strFindFolder);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			CalcHashFolder(finder.GetFilePath());
		}else{
			CalcHashFile(finder.GetFilePath());
		}
	} 
	return TRUE;
}

CString CHashsListAsyn::GetFirstFile()
{
	m_lock.Lock();
	CString str;
	if (m_listFile.size())
	{
		str = *m_listFile.begin();
		m_listFile.pop_front();
	}
	m_lock.Unlock();
	return str;
}

BOOL CHashsListAsyn::OnProcess( LONG64 lIncrease )
{
	m_lock.Lock();
	m_lCalcFiles += lIncrease;
	m_lock.Unlock();
	return TRUE;
}

BOOL CHashsListAsyn::SetCallback( IHashCallback * lpCallback )
{
	m_lpCallback = lpCallback;
	return TRUE;
}

BOOL CHashsListAsyn::Clear()
{
	m_lock.Lock();
	m_lTotalFiles = 0;
	m_lCalcFiles = 0;
	m_lock.Unlock();
	return TRUE;
}
