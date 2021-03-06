#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QMessageBox>
#include "ui_Search.h"
#include "MemoryMap.h"
#include "MemRead.h"
#include "Modules.h"
#include "Results.h"
#include "PointerHolder.h"
#include "DebuggedProcess.h"
#include "CheckPerformance.h"
#include <Windows.h>
#include <winnt.h>
#include <string>
#include <sstream>
#include <MemRead.h>
#include "MyMutexes.h"
#include "Logs.h"
#define CPPOUT fout
#define ALL_MEM_PROTECTS (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE | PAGE_READONLY)
#define WRITABLE_EXECUTE (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
#define WRITABLE_NO_EXECUTE (PAGE_READWRITE | PAGE_WRITECOPY)
#define EXECUTE_NO_WRITE (PAGE_EXECUTE_READ | PAGE_EXECUTE)
#define EXECUTE_WRITABLE (PAGE_EXECUTE_READ | PAGE_EXECUTE | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
#define COPY_WRITE (PAGE_EXECUTE_WRITECOPY | PAGE_WRITECOPY)
#define READONLY (PAGE_READONLY)
#define NUMBER_DISPLAYED_RESULTS 1500
class MemoryScanner;
class SearchWindow;
void OutputResultHardDisk(MEMBLOCK *mb_list, Ui_DialogResults* pResultWindow, SearchWindow* pSearchWindow, std::vector <uint64_t> &_vecResultsAddr2);
typedef enum
{
	NEXT_SCAN,
	NEW_SCAN,
} SCAN_CONDITION;
typedef enum
{
	VALUETYPE_UNKNOWN = 0,
	BYTE_ = 1,
	TWOBYTES_ = 2,
	INT32_ = 3,
	INT64_ = 4,
	FLOAT_ = 5,
	DOUBLE_ = 6,
	STRING_ = 7,
	ArrayBytes_ = 8,
} DATA_SIZE;
enum MemoryScanValueType
{
	VALUETYPE_UNKNOWN2,
	VALUETYPE_BYTE,
	VALUETYPE_2BYTE,
	VALUETYPE_4BYTE,
	VALUETYPE_8BYTE,
	VALUETYPE_FLOAT,
	VALUETYPE_DOUBLE,
	VALUETYPE_STRING,
	VALUETYPE_WSTRING,
	VALUETYPE_AOB
};

// Memory scanner comparison types.
enum MemoryScanType
{
	SCANTYPE_EXACTVALUE,
	SCANTYPE_GREATERTHAN,
	SCANTYPE_SMALLERTHAN,
	SCANTYPE_UNKNOWN_INITIAL_VALUE,
	SCANTYPE_VALUE_IN_BETWEEN,
	SCANTYPE_CHANGED,

	// The same comparetor function applies for the scan types below. Do not add a scan type that requires a different comparetor.
	SCANTYPE_UNCHANGED,
	SCANTYPE_INCREASED,
	SCANTYPE_DECREASED
};
enum SEARCH_CONDITION
{
	COND_EQUALS,
	COND_BIGGERTHAN,
	COND_LOWERTHAN,
	COND_UNCONDITIONAL,//unkowninitiavalue
	COND_INCREASED,
	COND_DECREASED,
	COND_CHANGED,
	COND_UNCHANGED,
};
// Generalisation struct that can be used to save class instances with template parameter lists.
struct ScanParameterBase
{
	// Indicates whether a fast (aligned) scan should be executed.
	bool CurrentScanFastScan;

	// The type of scan that should be executed.
	//MemoryScanType GlobalScanType;
	SEARCH_CONDITION GlobalScanType;

	// The value type (sizeof) of the value that should be scanned for.
	DATA_SIZE GlobalScanValueType;

	// Are we returning search results in hexadecimal?
	bool CurrentScanHexValues;

	// Are we doing the first scan
	bool FirstScan;
	// This parameter contains the size of an array of bytes or the length of a string in case of such a scan.
	// If the scan value type is not one of these, this parameter is ignored.
	unsigned int ValueSize;

	// When searching for strings, it is possible to match strings until null chars are hit. The threshold to
	// avoid CrySearch from reading excessive amounts of memory into a string is 0x100.
	bool ScanUntilNullChar;

	//memory regions parameter
	DWORD AcceptedMemoryState;

	uint8_t nValue8;
	uint16_t nValue16;
	uint32_t nValue32;
	uint64_t nValue64;

	//Total bytes to be read
	uint64_t TotalBytesRead;
	//template function inline (class member) which returns the value used to perform the scan as an integer
	//must return either an int64_t either an int32_t
	void GetValue(SearchWindow * pSearchWindow, SCAN_CONDITION NewOrNext);
	// Default constructor should help preventing bogus values.
	uint32_t ScanOffset;
	ScanParameterBase()
	{
		this->CurrentScanFastScan = true;
		this->FirstScan = true;
		this->GlobalScanType = COND_EQUALS;
		this->GlobalScanValueType = VALUETYPE_UNKNOWN;
		this->CurrentScanHexValues = false;
		this->ValueSize = 0;
		this->ScanUntilNullChar = false;
		this->AcceptedMemoryState = 0;
		this->ScanOffset = 1;
		this->TotalBytesRead = 0;
	};
	void UpdateSelectedScanOptions(SearchWindow * pSearchWindow, bool firstScan);
};
void print_matches(MEMBLOCK *mb_list, Ui_DialogResults* pResultWindow, SearchWindow *pSearchWindow);
int ui_run_scan(MEMBLOCK *scan, uint32_t data_size, int64_t start_val, SEARCH_CONDITION start_cond, SCAN_CONDITION scan_condition);

class SearchWindow : public QDialog
{
	Q_OBJECT
public:
	void someSlot();
	template <typename T>
	int ui_run_scan(MEMBLOCK *scan1, uint32_t data_size, T start_val, SEARCH_CONDITION start_cond, SCAN_CONDITION scan_condition);
	uint32_t ReturnDataSize();
	void ShowResults(uint64_t nResults);
	bool FilterRegions(MEMORY_BASIC_INFORMATION &meminfo);
	void IsSetOnTop();
	void FirstScan();
	void NextScan();
	SearchWindow(QMainWindow* parent);
	~SearchWindow() { }
	void closeEvent(QCloseEvent *event);
public:
	Ui::Ui_Search ui;
	Ui_DialogResults *_hResult;
	ScanParameterBase * pScanOptions;
	MEMBLOCK *_mbScan;
	Mods *ModMap;
	bool bInScan = 0;
	uint64_t _nResults;
};