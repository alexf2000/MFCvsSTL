// MFCvsSTL.cpp : Performance comparison between MFC and STL containers
// (c) Alex Fedorov http://alexf.name/ 2013

#include "stdafx.h"
#include "MFCvsSTL.h" // nothing special here

#include "fast_c_map_str.h"

#include <algorithm>
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// The one and only application object

CWinApp theApp;
static int g_nLetters = 4;
static int g_nBits = 5;
static int g_nMem = 0;
using namespace std;


// typedef map<DWORD, void*> stlmp;
typedef unordered_map<DWORD, void*> stlmp;
// typedef unordered_map<CString, CString> stlStMp;
typedef unordered_map<string, string> stlStMp;

int stlStrMap(int nCount)
{
	char buff[100] = "";
	int nSize = (int)(1.2 *(float)nCount);
	stlStMp a(nSize);
	for (int i = 0; i < nCount; ++i)
	{
		sprintf(buff, "%d key", i);
		string s1 = buff;
		sprintf(buff, "%d val", i);
		string s2 = buff;
		a[s1] = s2;
	}
	stlStMp::iterator iter;
	for (int j = 0; j < nCount; ++j)
	{
		int i = abs(rand() * j) % nCount;
		sprintf(buff, "%d key", i);
		string s1 = buff;
		iter = a.find(s1);
	}
	return 1;
}

int stlMap(int nCount)
{
	int nSize = (int)(1.2 *(float)nCount);
	stlmp a(nSize);
	// unordered_map <DWORD, void*> a(nSize);
	for (int i = 0; i < nCount; ++i)
	{
		a[i] = (void *)i;
	}
	stlmp::iterator iter;
	for (int j = 0; j < nCount; ++j)
	{
		iter = a.find((abs(rand() * j) % nCount));
	}
	return 1;
}

int mfcMap(int nCount)
{
	int nSize = (int)(1.2 *(float)nCount); // recommended in MSDN for collision avoidance
	// CMapWordToPtr a;
	CMap<DWORD, DWORD, void*, void*> a;
	a.InitHashTable(nSize);
	for (int i = 0; i < nCount; ++i)
	{
		a[i] = (void *)i;
	}
	void * val;
	for (int j = 0; j < nCount; ++j)
	{
		a.Lookup((abs(rand() * j) % nCount), val);
	}
	return 0;
}

int cccStrMap(int nCount)
{
	char buff1[100] = "";
	char buff2[100] = "";
	fast_c_map_strings * map = map_new(g_nLetters, g_nBits, 1000 * 1000);
	for (int i = 0; i < nCount; ++i)
	{
		sprintf(buff1, "%d key", i);
		sprintf(buff2, "%d val", i);
		map_set(map, buff1, buff2);
	}
	for (int j = 0; j < nCount; ++j)
	{
		CStringA str2;
		int i = abs(rand() * j) % nCount;
		sprintf(buff1, "%d key", i);
		strcpy(buff2, map_get(map, buff1));
		// printf("%s %s \n", buff1, buff2);
	}
	g_nMem = map->m_nMemUsed;
	map_del(map);
	return 0;
}

int mfcStrMap(int nCount)
{
	int nSize = (int)(1.2 *(float)nCount); // recommended in MSDN for collision avoidance
	CMapStringToString mapStringToString;
	mapStringToString.InitHashTable(nSize);
	for (int i = 0; i < nCount; ++i)
	{
		CString str1;
		CString str2;
		str1.Format(_T("%d key"), i);
		str2.Format(_T("%d val"), i);
		mapStringToString[str1] = str2;
	}
	for (int j = 0; j < nCount; ++j)
	{
		CString str1;
		CString str2;
		int i = abs(rand() * j) % nCount;
		str1.Format(_T("%d key"), i);
		mapStringToString.Lookup(str1, str2);
	}
	return 0;
}

int stlArray(int nCount)
{
	vector <int> bigarray;
	int nMs = bigarray.max_size();
	try
	{
		bigarray.reserve(nCount);
	}
	catch (...)
	{
		CString str;
		str.Format(_T("Memory allocation error trying to reserve %d elements. vector.max_size=%d\r\n"), nCount, nMs);
		_tprintf(str);
		return 0;
	}
	for (int k = 0; k < nCount; ++k)
	{
		bigarray.push_back(k);
		// bigarray[k] = k;
	}
	int ret = bigarray.size();
	return ret;
}

int mfcArray(int nCount)
{
	// CArray<int,int> arr;
	// OCArray<int> arr;
	CUIntArray arr;
	arr.SetSize(0, nCount);
	for (int k = 0; k < nCount; ++k)
	{
		arr.Add(k);
		// arr[k] = k;
	}
	int ret = arr.GetCount();
	return ret;
}

int mfcList(int nCount)
{
	CList<int, int> a;
	for (int k = 0; k < nCount; ++k)
	{
		a.AddHead(k);
	}
	for (int j = 0; j < nCount; ++j)
	{
		int n = abs(rand() * j) % nCount;
		POSITION pos = a.Find(n);
	}
	return 1;
}

int stlList(int nCount)
{
	list<int> mylist;
	for (int k = 0; k < nCount; ++k)
	{
		mylist.push_front(k);
	}
	for (int j = 0; j < nCount; ++j)
	{
		int n = abs(rand() * j) % nCount;
		std::list<int>::iterator findIter = std::find(mylist.begin(), mylist.end(), n);
	}
	return 1;
}

class CTimeHelper2013
{
public:
	SYSTEMTIME m_st1;
	SYSTEMTIME m_st2;
	BOOL m_bStarted;
	//
	int Start();
	int DiffMS();
	//
	CTimeHelper2013();
	virtual ~CTimeHelper2013();
};

int timeCount(int nCount, int(*fnPtr)(int))
{
	CTimeHelper2013 th;
	th.Start();
	fnPtr(nCount);
	return th.DiffMS();
}

// http://forums.codeguru.com/showthread.php?129321-STL-vs-MFC
int testHash(int nCount)
{
	int d2 = timeCount(nCount, stlMap);
	int d1 = timeCount(nCount, mfcMap);
	CString str;
	str.Format(_T("CMap vs unordered_map. Fill & find %d elements. MFC (%d)ms STL (%d)ms\r\n"), nCount, d1, d2);
	_tprintf(str);
	return 1;
}

int testStringMap(int nCount)
{
	int d3 = timeCount(nCount, stlStrMap);
	g_nLetters = 4;
	g_nBits = 5;
	int d2 = timeCount(nCount, cccStrMap);
	int d1 = timeCount(nCount, mfcStrMap);
	CString str;
	str.Format(_T("CMapStringToString vs fast_c_map_strings vs STL. Fill & find %d elements.\r\nMFC (%d)ms Pure C (%d)ms STL (%d)ms\r\n"), nCount, d1, d2, d3);
	_tprintf(str);
	return 1;
}

int testArray(int nCount)
{
	int d2 = timeCount(nCount, stlArray);
	int d1 = timeCount(nCount, mfcArray);
	CString str;
	str.Format(_T("CArray vs vector. Fill %d elements. MFC (%d)ms STL (%d)ms\r\n"), nCount, d1, d2);
	_tprintf(str);
	return 1;
}

int testList(int nCount)
{
	int d2 = timeCount(nCount, stlList);
	int d1 = timeCount(nCount, mfcList);
	CString str;
	str.Format(_T("CList vs list. Fill & find %d elements. MFC (%d)ms STL (%d)ms\r\n"), nCount, d1, d2);
	_tprintf(str);
	return 1;
}

// http://blogs.msdn.com/b/vcblog/archive/2009/06/23/stl-performance.aspx
void perf_startup()
{
	SetThreadAffinityMask(GetCurrentThread(), 1);
	SetThreadIdealProcessor(GetCurrentThread(), 0);
	Sleep(1);
}

CTimeHelper2013::CTimeHelper2013()
{
	m_bStarted = 0;
}

CTimeHelper2013::~CTimeHelper2013()
{
}

int CTimeHelper2013::Start()
{
	GetLocalTime(&m_st1);
	m_bStarted = 1;
	return 1;
}

int CTimeHelper2013::DiffMS()
{
	GetLocalTime(&m_st2);
	m_bStarted = 0;
	int nRet1 = m_st2.wMilliseconds - m_st1.wMilliseconds;
	int nRet2 = m_st2.wSecond - m_st1.wSecond;
	int nRet3 = m_st2.wMinute - m_st1.wMinute;
	int nRet4 = m_st2.wHour - m_st1.wHour;
	int nRet5 = m_st2.wDay - m_st1.wDay;
	int nRet6 = m_st2.wMonth - m_st1.wMonth;
	int nRet7 = m_st2.wYear - m_st1.wYear;
	int nRet = 0;
	nRet += nRet1;
	nRet += nRet2 * 1000;
	nRet += nRet3 * 1000 * 60;
	nRet += nRet4 * 1000 * 60 * 60;
	nRet += nRet5 * 1000 * 60 * 60 * 24;
	nRet += nRet6 * 1000 * 60 * 60 * 24 * 30; // approximation
	nRet += nRet7 * 1000 * 60 * 60 * 24 * 365; // approximation
	return nRet;
}

int testCompate()
{
	_tprintf(_T("MFC vs STL performance test\n"));
	perf_startup(); // unnecessary
	testList(1000 * 10);
	testList(1000 * 100);
	testList(1000 * 200);
	testArray(1000 * 100);
	testArray(1000 * 1000);
	testArray(1000 * 1000 * 200);
	testHash(1000 * 100);
	testHash(1000 * 1000);
	// testHash(1000 * 1000 * 20);
	testStringMap(1000 * 100);
	testStringMap(1000 * 1000);
	return 1;
}

int testFastHash()
{
	int nCount = 1000 * 100;
	for (int b = 2; b < 8; b++)
	for (int l = 2; l < 8; l++)
	{
		g_nLetters = l;
		g_nBits = b;
		int d2 = timeCount(nCount, cccStrMap);
		CString str;
		str.Format(_T("FastHash l[%d] b[%d] c[%d] Time (%d)ms Mem[%d]\r\n"), l, b, nCount, d2, g_nMem);
		_tprintf(str);
	}
	return 1;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (NULL == hModule != NULL)
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		return 1;
	}
	if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return 2;
	}
	testCompate();
	testFastHash();
	return nRetCode;
}
