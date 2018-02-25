/*------------------------------------------------------------------------
 Module:        sapi.c
 Author:        Andreas Martin
 Project:       Serial-API (SAPI)
 State:         Stable
 Version:       V1.0
 Creation Date: 01.12.2004
 Description:   With the SAPI interface you'll be able simply to write
                your applications using the com port.
                The standard Windows API to communicate via a serial
                interface are collected here in a straight manner, for
                common use.
                This implementation bases on Windows and is thread save.
------------------------------------------------------------------------*/

//lint -esym(785, dcb) few initializers for dcb
//lint -u
//lint +fem

#include <ctype.h>
#include <stdio.h>
#include <mmsystem.h>
#include "sapi.h"

#ifndef WIN32
#  error This implementation of SAPI bases on Windows
#else
#  include <windows.h>
#  include <winspool.h>
#  include <setupapi.h>
#endif

#ifdef __LCC__
#  pragma lib <winspool.lib>
#endif

#define Serial_MutexName "SerialMutex_"
#define ERROR_STRING_BAD_INIT                                                   \
        "Failure: \n\nA port structure given to functions of the SAPI lib\n "  \
        "have to be initialized via SAPI_DECLARE(<identifier>)!\n"

const char Serial_ID[] = "SAPI_LIB";

static int      is_WinNT_os;
static DWORD    dwPlatformId;

// Debuglevel
// 0 = Show no debug info (OutputDebugString)
// 1 = Show normal debug info (OutputDebugString)
// 2 = Show verbose debug info (OutputDebugString)
// 10 and above = Errors displayed per MessageBox
#define SAPI_DEBUGLEVEL 0

#ifndef __LCC__
# define FUNCTION_NAME ""
#else
# define FUNCTION_NAME __func__
#endif

#define NO_FUNCTION "<unknown>"

static const char *DBG_FUNC = NO_FUNCTION;

#define CLOSEHANDLE(h) if ( (h) && (INVALID_HANDLE_VALUE != (h))) CloseHandle(h)

/************** Error & Debug Functions **************/

#if (SAPI_DEBUGLEVEL == 0)
# define OutDebugString(str) (void)0
# define dbgprintf (void)/*lint --e(505)*/
# define DisplayLastError(nr) (void)0
#else
# define OutDebugString(str) OutputDebugString(str)
# define dbgprintf _dbgprintf
# define DisplayLastError(nr) _DisplayLastError(nr)
#endif

static void _dbgprintf( int level, const char* str, ... )
{
  va_list va = va_start( va, str );
  static char buffer[512], *p;
  unsigned cchMax = sizeof(buffer);
  int cch;

  // If SAPI_DEBUGLEVEL is 0 _dbgprintf (dbgprintf) will never be called!
  if( SAPI_DEBUGLEVEL < level ) // Level of 0 means display always
    return;

  p = buffer;

  cch = snprintf(p, (unsigned)cchMax, "SAPI(%s): ", DBG_FUNC);

  if( (cch>0) && ((unsigned)cch<=cchMax) )
  {
    cchMax -= (unsigned)cch;
    (void)vsnprintf( p, (unsigned)cchMax, str, va );
    OutDebugString( buffer );
  }

  va_end(va);
}

//lint -e{529} lpBuffer maybe not subsequently used
static void _DisplayLastError(DWORD lastError)
{
  PVOID lpBuffer;

  if( 0 == lastError )
    lastError = GetLastError();

  if( NO_ERROR == lastError )
    return;

  if( lastError & (1<<28) ) // User defined?
  {
    switch( lastError )
    {
      default:
      break;
    }
  }

  dbgprintf( 0, "failed, code: %d", GetLastError());

  #if ( SAPI_DEBUGLEVEL >= 10 )

    if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM + 100,
                       NULL,
                       GetLastError(),
                       0, // MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US /*for always english output*/
                       (LPSTR)&lpBuffer,
                       0,
                       NULL ) )
    {
      (void)MessageBox( GetDesktopWindow(),
                        lpBuffer,
                        "SAPI Lib",
                        MB_ICONERROR|MB_OK );
    }
    else
    {
      (void)MessageBox( GetDesktopWindow(),
                        "DisplayLastError failed!",
                        "SAPI Lib",
                        MB_ICONERROR|MB_OK );
    }

    LocalFree(lpBuffer);

  #endif
}

/************** Time functions **************/

static int nPortsOpen = 0; // Number of opened Ports (for this process)
static int timeMode;   // 0: timeGetTime, 1: PerformanceCounter
static double QPCfreq; // Divider for Performance Counters

// function to check the machine to be able handling with
// performance counters (QPC)
bool timeQPCsupported(void)
{
    bool multiProcessor; // more than 1 processor?
    SYSTEM_INFO systemInfo;
    uint64_t freq;

    memset(&systemInfo, 0, sizeof(systemInfo));
    GetSystemInfo(&systemInfo);

    if ( 1 == systemInfo.dwNumberOfProcessors )
        multiProcessor = false;
    else
        multiProcessor = true;

    if( !QueryPerformanceFrequency((LARGE_INTEGER*)&freq) )
      return false;

    // QPCfreq are the number of counts per 0.1ms
    QPCfreq = (double)freq/10000.0;

    return ( !multiProcessor && ( 0.0 != QPCfreq) );
}

void timeInit(void)
{

  /*
  TIMECAPS timeCaps;

  (void)timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));

  // timeBeginPeriod needs about 16ms to ensure
  // the accuracy for timeGetTime.

  // This setting is system wide. If you want to increase time
  // accuracy, please call "timeBeginPeriod" in your own source.
  // "timeBeginPeriod" increases system-wide the accuracy!
  // So all time measuring API-functions will be more precise,
  // QueryPerformanceCounter too! But this will also slow down
  // your system, so be careful...

  (void) timeBeginPeriod(timeCaps.wPeriodMin);
  */

  if( timeQPCsupported() )
  {
    timeMode = 1; // Use QPC
  }
}

void timeDeinit(void)
{
  TIMECAPS timeCaps;

  (void)timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));
  (void)timeEndPeriod(timeCaps.wPeriodMin);
}

// timeGet returns the current time in 0.1ms intervals
uint64_t timeGet(void)
{
  uint64_t t0, t1 = 0;

  switch( timeMode )
  {
    case 0:
      t0 = (uint64_t)timeGetTime()*10;
      break;

    case 1:
      (void)QueryPerformanceCounter((LARGE_INTEGER*)&t1);

      // Two different values, which may not differ more than 1ms
      do
      {
        t0 = t1;

        (void)QueryPerformanceCounter((LARGE_INTEGER*)&t1);

      } while( (t1-t0 > QPCfreq) || (t0==t1) );

      t0 = (uint64_t) ((double)t0 / QPCfreq);

      break;

    default:
      t0 = 0;
      break;
  }

  return t0;
}

/************** Process- and Thread synchronisation **************/

/* synchonisation is necessary for:

  GetAttr
  SetAttr
  SetOutputs
  SetBufferSizes
  Open
  Close
  Write
  Read

  The results for Inputs, State, RxCount und Errors are
  volatile at all and have not to be watched...
*/

/********************** mutual exclusion **************************
  A mutex is a process-comprehensive object. Always only one
  thread have ownership of the mutex object. The ownership is
  assigned over the wait-functions (WaitForSingleObjekt,
  WaitForMultipleObjects). A mutex is in signaled state providing
  no thread has ownership of it.
  A wait-Funktion therefore returns back immediately.
  After that the Mutex is automatically in the non-signaled-state.
  The ownership is released by the function ReleaseMutex again.

  Mutex objects are as said process comprehensive. That is any
  thread that takes usage of a SAPI function will prevent any
  SAPI call from another thread for that com port while it is pending.
  (Only for that port, because a mutex for each opened port will
  be initialized.)

  This does not represent, however, any disadvantage, since this SAPI
  library works in the non-overlapped-mode and windows anyhow prevent
  operations on the serial port process comprehensive while
  in a pending operation. This predominating quality anyhow is
  expanded therefore onto this SAPI.

  The thread save parts of the SAPI lib are enclosed in
  BEGIN_CRITICAL_FUNC and END_CRITICAL_FUNC. The macros generate
  an named mutex object an wait to get ownership of them.
  Since END_CRITICAL_FUNC completes a thread save function and all
  these function return either true or false, it also returns the
  result of the according function.

  The macro EXIT_CRITICAL_FUNC prematurely exits the thread
  save function in a regular manner: releases the mutex, sets the
  error result and finishes the function with the boolean value
  indicating the success of the function.
*******************************************************************/

// prevent ommitting "unreachable code"
//lint -emacro(527, END_CRITICAL_FUNC, EXIT_CRITICAL_FUNC)
//lint -emacro(527, END_CRITICAL_FUNC_VAL, EXIT_CRITICAL_FUNC_VAL)

#define BEGIN_CRITICAL_FUNC( lpAttr )                   \
        {                                               \
          HANDLE hMutex = EnterCriticalFunction(lpAttr);\
          DBG_FUNC = FUNCTION_NAME

#define EXIT_CRITICAL_FUNC(errCode)                     \
        {                                               \
          DWORD __errCode = (errCode);                  \
          SetLastError(__errCode);                      \
          LeaveCriticalFunction(hMutex);                \
          DisplayLastError(__errCode);									\
          DBG_FUNC = NO_FUNCTION;                       \
          return (NO_ERROR == __errCode) ? true : false;\
        }

#define EXIT_CRITICAL_FUNC_VAL(value)                   \
        {                                               \
          LeaveCriticalFunction(hMutex);                \
          DisplayLastError(GetLastError());							\
          DBG_FUNC = NO_FUNCTION;                       \
          return (value);                               \
        }

#define END_CRITICAL_FUNC(errCode)                      \
          EXIT_CRITICAL_FUNC(errCode);                  \
        }

#define END_CRITICAL_FUNC_VAL(value)                    \
          EXIT_CRITICAL_FUNC_VAL(value);                \
        }


static HANDLE EnterCriticalFunction(Serial_LPCAttr lpAttr)
{
  HANDLE hMutex;
  char mutexName[MAX_PATH+1];

  (void)snprintf( mutexName, sizeof(mutexName),
                  "%s%.*s",
                  Serial_MutexName,
                  MAX_PATH - strlen(Serial_MutexName),
                  lpAttr->portName );

  // Get a new handle for a mutex object. If the mutex existed
  // already we get a duplicte.
  hMutex = CreateMutex(NULL, FALSE, mutexName);

  if( INVALID_HANDLE_VALUE == hMutex )
    return hMutex;

  // Getting ownership of the mutex
  WaitForSingleObject(hMutex, (DWORD)INFINITE);

  // ok, in safe area now, no other process can now execute
  // any SAPI function on named com port...
  return hMutex;
}

static void LeaveCriticalFunction(HANDLE hMutex)
{
  ReleaseMutex( hMutex );
  CloseHandle( hMutex );
}

/************** Internal functions **************/

/* Return values

Functions will return following values:
a) numeric values (8-bit, 16-bit or 32-bit), or
b) boolean values (true, false)

In case b) the error value the API function returned at last call
(GetLastError) will be transfered into SAPI struct (if possible).

*/


/*------------------------------------------------------------------------
 Function:      SetThresholds
 Purpose:       Calculates the thresholds for internal handshaking (RTS)
                Thresholds will be set to 50% and 75%.
                Internal use only!
 Input:         Pointer to DCB (Data Control Block),
                size of the RX buffer.
 Output:
 Errors:
------------------------------------------------------------------------*/
static void SetThresholds(DCB *dcb, uint32_t dwRxBufSize){
  // Since there is a bug in the Win API the thresholds have to be
  // changed too.
  // Reference: http://www.edavies.nildram.co.uk/win32-serial-bug

  dcb->XonLim  = LOWORD(max(50L*dwRxBufSize/100L, 1));
  dcb->XoffLim = LOWORD(max(75L*dwRxBufSize/100L, 1));
}

/*------------------------------------------------------------------------
 Function:      IsValidAttrParam
 Purpose:       Validates the SAPI struct (test of correct use of
                SERIAL_DECLARE).
                Internal use only!
 Input:         Pointer to SAPI struct.
 Output:        Success flag.
 Errors:        If the serial ID in the SAPI struct is invalid a message
                box for developers will be shown.
                ERROR_BAD_ARGUMENTS: lpAttr is NULL or serial ID is
                                     incorrect.
------------------------------------------------------------------------*/

static bool IsValidAttrParam(Serial_LPCAttr lpAttr){

  if (NULL == lpAttr)
  {
    SetLastError(ERROR_BAD_ARGUMENTS);
    return false;
  }

  if (Serial_ID == lpAttr->SerialID)
    return true;

  SetLastError(ERROR_BAD_ARGUMENTS);

  MessageBox( NULL,
              ERROR_STRING_BAD_INIT,
              "Serial API",
              MB_OK|MB_ICONERROR|MB_TOPMOST );

  return false;
}

/*------------------------------------------------------------------------
 Function:      CheckAttrOpened
 Purpose:       Checks for valid pointer to SAPI struct. An opened
                port is assumed.
                Internal use only!
 Input:         Pointer to a SAPI struct
 Output:        True if checks were successful
 Errors:        ERROR_INVALID_HANDLE if port handle is invalid
                Others: refer to IsValidAttrParam
------------------------------------------------------------------------*/
static bool CheckAttrOpened(Serial_LPCAttr lpAttr){

  if (!IsValidAttrParam(lpAttr))
    return false;

  if (INVALID_HANDLE_VALUE == lpAttr->portHandle) // valid port handle?
  {
    SetLastError(ERROR_INVALID_HANDLE);
    return false;
  }

  return true;
}

/*------------------------------------------------------------------------
 Function:      ClearAkkuCommError
 Purpose:       Replacement for ClearCommError.
                Occuring errors will cause the setting of the error flags.
                In contrast already rised error flags won't be unset.
                (Therefore an akkumulating bahviour)
                Internal use only!
 Input:         Pointer to the SAPI struct
                Pointer to a COMSTAT-Struktur (may be NULL).
 Output:        True, if operation succeeded
 Errors:        refer to ClearCommError
------------------------------------------------------------------------*/
static bool ClearAkkuCommError(Serial_LPAttr lpAttr, COMSTAT *cstat){
  uint16_t i;
  DWORD newErrors;
  #define ERROR_PAIR(name) {CE_##name, ERROR_##name}

  // Corresponding error flags to the windows error flags
  static uint16_t ErrorMatrix[][2]={
    ERROR_PAIR(RXOVER),
    ERROR_PAIR(OVERRUN),
    ERROR_PAIR(RXPARITY),
    ERROR_PAIR(FRAME),
    ERROR_PAIR(BREAK),
    ERROR_PAIR(TXFULL),
    ERROR_PAIR(PTO),
    ERROR_PAIR(IOE),
    ERROR_PAIR(DNS),
    ERROR_PAIR(OOP),
    ERROR_PAIR(MODE),
  };
  #undef ERROR_PAIR

  if (!ClearCommError(lpAttr->portHandle, &newErrors, cstat))
    return false;

  // testing of each error flag an translation
  for (i=0; i<(sizeof(ErrorMatrix)/sizeof(ErrorMatrix[0])); i++)
  {
    if (newErrors & ErrorMatrix[i][0]) // Windows-Fehlerbit gesetzt?
    {
      lpAttr->Errors |= ErrorMatrix[i][1]; // Dann eigenes Bit setzen
    }
  }

  return true;
}

// Some prototypes for fifo access
static HKEY GetDeviceKey( LPCSTR portName, int access );
static bool TouchFifoEnable( HKEY hKey, int16_t *fEnable );
static bool TouchFifoSize( HKEY hKey, int16_t *rxSize, int16_t *txSize );

/************** Exported functions **************/

/*------------------------------------------------------------------------
 Function:      TimeOneChar
 Purpose:       Returns the time in ms neded to send a char (theretically)
 Input:         Pointer to the SAPI struct
 Output:        The time in milliseconds (double)
 Errors:        The function will return 0.0 if lpAttr is invalid
------------------------------------------------------------------------*/
double Serial_TimeOneChar( Serial_LPCAttr lpAttr )
{
  double stopbits, result;

  BEGIN_CRITICAL_FUNC( lpAttr );

  if( !IsValidAttrParam( lpAttr ) )
  {
    EXIT_CRITICAL_FUNC_VAL( 0.0 );
  }

  switch( lpAttr->Stopbits )
  {
    case ONESTOPBIT:   stopbits = 1.0; break;
    case ONE5STOPBITS: stopbits = 1.5; break;
    case TWOSTOPBITS:  stopbits = 2.0; break;
    default:           stopbits = 1.0; break;
  }

  result = 1000.0 * (lpAttr->Databits + stopbits + 1.0) / lpAttr->Baudrate;

  END_CRITICAL_FUNC_VAL( result );
}

/*------------------------------------------------------------------------
 Function:      GetAttr
 Purpose:       Fills the SAPI struct with the ports actual settings
 Input:         Pointer to the SAPI struct
 Output:        True if operation succeeded
 Errors:        refer to:
                CheckAttrOpened, GetCommState, GetCommTimeouts,
                ClearAkkuCommError
------------------------------------------------------------------------*/
bool Serial_GetAttr(Serial_LPAttr lpAttr){
  DCB dcb={sizeof(DCB)};
  COMMTIMEOUTS to;

  BEGIN_CRITICAL_FUNC( lpAttr );

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommState(lpAttr->portHandle, &dcb)) // ask for DCB block for the port
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommTimeouts(lpAttr->portHandle, &to)) // the timeouts, too
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!ClearAkkuCommError(lpAttr, NULL)) // and the error flags
    EXIT_CRITICAL_FUNC(GetLastError());

  // Now copying the relevant data
  lpAttr->Baudrate = dcb.BaudRate;
  lpAttr->Databits = dcb.ByteSize;
  lpAttr->Stopbits = dcb.StopBits;
  lpAttr->Parity = dcb.Parity;

  lpAttr->ReadIntervalTimeout = to.ReadIntervalTimeout;
  lpAttr->ReadTimeoutMultiplier = to.ReadTotalTimeoutMultiplier;
  lpAttr->ReadTimeoutConstant = to.ReadTotalTimeoutConstant;

  lpAttr->WriteTimeoutMultiplier = to.WriteTotalTimeoutMultiplier;
  lpAttr->WriteTimeoutConstant = to.WriteTotalTimeoutConstant;

  // Update of the RTS and DTR states
  lpAttr->Outputs &= ~(RTS_MASK | DTR_MASK); // mask the bits

  switch(dcb.fRtsControl){
    case RTS_CONTROL_ENABLE: // RTS is ON
      lpAttr->Outputs |= RTS_ON;
      break;

    case RTS_CONTROL_HANDSHAKE: // RTS is set by internal handshaking
      lpAttr->Outputs |= RTS_HANDSHAKE;
      break;

    case RTS_CONTROL_DISABLE: // RTS is OFF
    default:
      lpAttr->Outputs |= RTS_OFF;
      break;

    // RTS_CONTROL_TOGGLE will be ignored, since it's not supported by
    // earlier versions of Windows
  }

  // Now updating DTR stae
  switch(dcb.fDtrControl){
    case DTR_CONTROL_ENABLE:
      lpAttr->Outputs |= DTR_ON;
      break;

    case DTR_CONTROL_HANDSHAKE:
      lpAttr->Outputs |= DTR_HANDSHAKE;
      break;

    case DTR_CONTROL_DISABLE:
    default:
      lpAttr->Outputs |= DTR_OFF;
      break;
  }

  lpAttr->fAbortOnError = dcb.fAbortOnError;

  END_CRITICAL_FUNC(NO_ERROR); // signal no error
}

/*------------------------------------------------------------------------
 Function:      SetAttr
 Purpose:       Updates the Windows port settings through the SAPI struct
 Input:         Pointer to the SAPI struct
 Output:        True, if operation succeeded
 Errors:        Refer to:
                CheckAttrOpened, GetCommState, GetCommTimeouts,
                SetCommState, SetCommTimeouts
------------------------------------------------------------------------*/
bool Serial_SetAttr(Serial_LPAttr lpAttr){
  DCB dcb={sizeof(DCB)};
  COMMTIMEOUTS to;
  uint32_t result=NO_ERROR;

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommState(lpAttr->portHandle, &dcb)) // build a valid DCB
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommTimeouts(lpAttr->portHandle, &to)) // get actual timeouts
    EXIT_CRITICAL_FUNC(GetLastError());

  // transfering the SAPI struct to Windows structures
  dcb.BaudRate = lpAttr->Baudrate;
  dcb.ByteSize = lpAttr->Databits;
  dcb.StopBits = lpAttr->Stopbits;
  dcb.Parity = lpAttr->Parity;
  dcb.fParity = (lpAttr->Parity != NOPARITY);

  if (!SetCommState(lpAttr->portHandle, &dcb)) // all settings valid?
    result = GetLastError();


  to.ReadIntervalTimeout = lpAttr->ReadIntervalTimeout;
  to.ReadTotalTimeoutMultiplier = lpAttr->ReadTimeoutMultiplier;
  to.ReadTotalTimeoutConstant = lpAttr->ReadTimeoutConstant;

  to.WriteTotalTimeoutMultiplier = lpAttr->WriteTimeoutMultiplier;
  to.WriteTotalTimeoutConstant = lpAttr->WriteTimeoutConstant;

  if (!SetCommTimeouts(lpAttr->portHandle, &to)) // timeouts valid?
    result = (NO_ERROR == result) ? GetLastError() : result;

  if (!Serial_GetAttr(lpAttr)) // retrieve the settings for validation
    result = (NO_ERROR == result) ? GetLastError() : result;

  /*
  This part should change the fifo settings.
  Since on one hand Windows NT need to be restarted after settings are made
  in addition only from users with admin rights,
  and on the other Windows non NT based does not have failures on the uarts
  fifo, changings are dangerous, make no sense and are avoided here...

  if( fChangeFifoProps && (NO_ERROR == result) )
  {
    HKEY hKey = GetDeviceKey( lpAttr->portName, KEY_WRITE );
    bool success = false;

    if( INVALID_HANDLE_VALUE != hKey )
    {
      success = true;

      if( !TouchFifoEnable( hKey, &lpAttr->FifoEnabled ) )
      {
        success = false;
        lpAttr->FifoEnabled = -1;
      }
      else
      {
        if( !TouchFifoSize( hKey, &lpAttr->FifoRxSize, &lpAttr->FifoTxSize ) )
        {
          success = false;
          lpAttr->FifoRxSize = -1;
          lpAttr->FifoTxSize = -1;
        }
      }
    }

    (void)RegCloseKey( hKey );

    if( !success )
      result = GetLastError();
  }
  */

  END_CRITICAL_FUNC(result);
}

/*------------------------------------------------------------------------
 Function:      SetOutputs
 Purpose:       Update the output lines (DTR, RTS and TxD (break signal))
 Input:         Pointer to the SAPI struct
                Signal flags (to omit some states leave the corresponding
                set to zero)
 Output:        True, if operation succeeded
 Errors:        Refer to:
                CheckAttrOpened, GetCommState, SetCommState
                ClearCommBreak, SetCommBreak
------------------------------------------------------------------------*/
bool Serial_SetOutputs(Serial_LPAttr lpAttr, uint16_t bFlags){
  DCB dcb={sizeof(DCB)};
  uint16_t newOutputs;

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommState(lpAttr->portHandle, &dcb))
    EXIT_CRITICAL_FUNC(GetLastError());

  newOutputs = lpAttr->Outputs; // first copy actual setting

  // Now the changes
  if (bFlags & RTS_MASK){ // Change of RTS state forced?
    switch(bFlags & RTS_MASK){ // which state is demanded
      case RTS_ON:
        dcb.fRtsControl=RTS_CONTROL_ENABLE;
        break;

      case RTS_HANDSHAKE:
        dcb.fRtsControl=RTS_CONTROL_HANDSHAKE;
        break;

      case RTS_OFF:
      default:
        dcb.fRtsControl=RTS_CONTROL_DISABLE;
        break;
    }

    newOutputs &= ~RTS_MASK; // mask RTS flags
    newOutputs |= (bFlags & RTS_MASK); // update the flags
  }

  if (bFlags & DTR_MASK){ // do so with DTR
    switch(bFlags & DTR_MASK){
      case DTR_ON:
        dcb.fDtrControl=DTR_CONTROL_ENABLE;
        break;

      case DTR_HANDSHAKE:
        dcb.fDtrControl=DTR_CONTROL_HANDSHAKE;
        break;

      case DTR_OFF:
      default:
        dcb.fDtrControl=DTR_CONTROL_DISABLE;
        break;
    }

    newOutputs &= ~DTR_MASK;
    newOutputs |= (bFlags & DTR_MASK);
  }

  if ( lpAttr->Outputs != newOutputs) // any states changed?
    // if so, tell Windows the new states
    if (!SetCommState(lpAttr->portHandle, &dcb))
      EXIT_CRITICAL_FUNC(GetLastError());

  lpAttr->Outputs=newOutputs; // port is set, so put settings in the SAPI struct

  if ( (bFlags & TX_NORM) != (newOutputs & TX_NORM) ){
    if (!ClearCommBreak(lpAttr->portHandle)) // clear any break flag on TX line
      EXIT_CRITICAL_FUNC(GetLastError());
  }
  else if ( (bFlags & TX_BREAK) != (newOutputs & TX_BREAK) ) // TX_NORM is more weighty by "else"
    if (!SetCommBreak(lpAttr->portHandle)) // set TX to break signaled
      EXIT_CRITICAL_FUNC(GetLastError());

  newOutputs &= ~TX_MASK; // mask bits for TxD
  newOutputs |= (bFlags & TX_MASK); // set new status flags

  lpAttr->Outputs = newOutputs; // TxD is now valid, so update the SAPI struct

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      Inputs
 Purpose:       Retrieves the states of modem registers (CTS, DSR,
                RI, and DCD)
 Input:         Pointer to SAPI struct
 Output:        Flags of the modem registers.
                In case of an error ERROR_FAILED will be returned instead

                Usage:
                if ( (uiInputs=Inputs(&port)) & ERROR_FAILED )
                {
                  switch(GetLastError())
                  {
                    // error condition
                  }
                }
                else
                {
                  // doing something with uiInputs
                }
 Errors:        Refer to:
                CheckAttrOpened, GetCommModemStatus
------------------------------------------------------------------------*/
uint16_t Serial_Inputs(Serial_LPCAttr lpAttr){
  DWORD mstat;
  uint16_t uiInputs = 0;

  BEGIN_CRITICAL_FUNC( lpAttr );

  if (!CheckAttrOpened(lpAttr))
  {
    EXIT_CRITICAL_FUNC_VAL( ERROR_FAILED );
  }

  if (!GetCommModemStatus(lpAttr->portHandle, &mstat)) // retrieve modem register
  {
    EXIT_CRITICAL_FUNC_VAL( ERROR_FAILED );
  }

  uiInputs |= (mstat & MS_CTS_ON)  ? CTS_STATE : 0; // set the corresponding flags
  uiInputs |= (mstat & MS_DSR_ON)  ? DSR_STATE : 0;
  uiInputs |= (mstat & MS_RING_ON) ? RI_STATE : 0;
  uiInputs |= (mstat & MS_RLSD_ON) ? DCD_STATE : 0;

  END_CRITICAL_FUNC_VAL( uiInputs );
}

/*------------------------------------------------------------------------
 Function:      State
 Purpose:       Retrieves the states for (CTS, DSR,
                DCD).
                This function assumes a call to ClearAkkuCommError.
                The retrieved flags determine the flags set in the SAPI struct.
 Input:         Pointer to SAPI struct
 Output:        Flags of the state register
                In case of an error ERROR_FAILED will be returned instead

                Usage:
                if ( (uiStates=State(&port)) & ERROR_FAILED )
                {
                  switch(GetLastError())
                  {
                    // error condition
                  }
                }
                else
                {
                  // doing something with uiStates
                }
 Errors:        Refer to:
                CheckAttrOpened, ClearAkkuCommError
------------------------------------------------------------------------*/
uint16_t Serial_State(Serial_LPAttr lpAttr){
  uint16_t uiState = 0;
  COMSTAT cstat;

  BEGIN_CRITICAL_FUNC( lpAttr );

  if (!CheckAttrOpened(lpAttr))
  {
    EXIT_CRITICAL_FUNC_VAL( ERROR_FAILED );
  }

  if (!ClearAkkuCommError(lpAttr, &cstat))
  {
    EXIT_CRITICAL_FUNC_VAL( ERROR_FAILED );
  }

  uiState |= cstat.fCtsHold ? CTS_STATE : 0; // set the corresponding flags
  uiState |= cstat.fDsrHold ? DSR_STATE : 0;
  uiState |= cstat.fRlsdHold ? DCD_STATE : 0;

  END_CRITICAL_FUNC_VAL( uiState );
}

/*------------------------------------------------------------------------
 Function:      Errors
 Purpose:       Returns the actual (accumulated) error flags
 Input:         Pointer to SAPI struct
                Flag error flags will be set to zero after returned
 Output:        Error flags.
                In case of a local error ERROR_FAILED will be returned instead

                Usage:
                if ( (uiErrors=Errors(&port, true)) & ERROR_FAILED )
                {
                  switch(GetLastError())
                  {
                    // error condition
                  }
                }
                else
                {
                  // doing something with uiErrors
                }
                If a local error occured und fErase was set true, the error
                flag won't be set to zero!
 Errors:        Refer to:
                CheckAttrOpened, ClearAkkuCommError
------------------------------------------------------------------------*/
uint16_t Serial_Errors(Serial_LPAttr lpAttr, bool fErase){
  uint16_t uiResult;

  BEGIN_CRITICAL_FUNC( lpAttr );

  if (!CheckAttrOpened(lpAttr))
  {
    EXIT_CRITICAL_FUNC_VAL( ERROR_FAILED );
  }

  if (!ClearAkkuCommError(lpAttr, NULL)) // retrieve actual error flags
  {
    EXIT_CRITICAL_FUNC_VAL( ERROR_FAILED );
  }

  uiResult = lpAttr->Errors;

  if (fErase)
    lpAttr->Errors=0;

  END_CRITICAL_FUNC_VAL( uiResult ); // return the sum of error fags
}

/*------------------------------------------------------------------------
 Function:      RxCount
 Purpose:       Returns the fill level of the RX buffer.
                This function assumes a function call to ClearAkkuComError.
                So error flags will be accumulated.
 Input:         Pointer to the SAPI struct
 Output:        Amount of available chars in the RX buffer.
                In case of an error: -1
 Errors:        Refer to:
                CheckAttrOpened, ClearAkkuCommError
------------------------------------------------------------------------*/
uint32_t Serial_RxCount(Serial_LPAttr lpAttr){
  COMSTAT cstat;

  BEGIN_CRITICAL_FUNC( lpAttr );

  if (!CheckAttrOpened(lpAttr))
  {
    EXIT_CRITICAL_FUNC_VAL( (uint32_t)-1 );
  }

  if (!ClearAkkuCommError(lpAttr, &cstat)) // level is stored in COMSTAT
  {
    EXIT_CRITICAL_FUNC_VAL( (uint32_t)-1 );
  }

  END_CRITICAL_FUNC_VAL( cstat.cbInQue );
}

/*------------------------------------------------------------------------
 Function:      SetBufferSizes
 Purpose:       Sets the TX and RX buffersizes.
                The thresholds for RX handshaking will be
                set to 50% and 74%, too
 Input:         Pointer to the SAPI struct
                size of the RX buffer (4kB assumed if zero)
                size of the TX buffer (4kB assumed if zero)
 Output:        True, if operation succeeded
 Errors:        Refer to:
                CheckAttrOpened, SetupComm, GetCommState,
                SetThresholds, SetCommState
------------------------------------------------------------------------*/
bool Serial_SetBufferSizes(Serial_LPCAttr lpAttr, uint32_t dwRxBufSize, uint32_t dwTxBufSize){
  DCB dcb={sizeof(DCB)};

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (!dwRxBufSize)
    dwRxBufSize = RX_BUFFERSIZE;

  if (!dwTxBufSize)
    dwTxBufSize = TX_BUFFERSIZE;

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!SetupComm(lpAttr->portHandle, dwRxBufSize, dwTxBufSize)) // new buffersizes
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommState(lpAttr->portHandle, &dcb)) // build valid DCB
    EXIT_CRITICAL_FUNC(GetLastError());

  SetThresholds(&dcb, dwRxBufSize);

  if (!SetCommState(lpAttr->portHandle, &dcb)) // set new DCB
    EXIT_CRITICAL_FUNC(GetLastError());

  END_CRITICAL_FUNC(NO_ERROR);
}


/*------------------------------------------------------------------------
 Function:      IsOpen
 Purpose:       Checks if the port is opened
 Input:         Pointer to the SAPI struct
 Output:        True, if port is opened
 Errors:        Refer to: SAPI function Errors
------------------------------------------------------------------------*/
bool Serial_IsOpen(Serial_LPAttr lpAttr){

  return (Serial_Errors(lpAttr, (bool)false) & ERROR_FAILED) ? false : true;
}

/*------------------------------------------------------------------------
 Function:      IsPort
 Purpose:       Checks a given string, if a so called serial port exits.
                This implementation doesn't cause any message boxes for
                DOS applications already accessing this port.
 Input:         The zero-terminated name of the port
 Output:        True, if the given name is valid.
 Errors:        Refer to: GetDefaultCommConfig
------------------------------------------------------------------------*/
bool Serial_IsPort(const char* lpszPortName){
  COMMCONFIG cc;
  DWORD ccSize = sizeof(cc);
  BOOL success;
  char portName[MAX_PATH+1] = {0};
  char *lpChar = NULL;

  if (NULL == lpszPortName)
    return false;

  strncpy(portName, lpszPortName, MAX_PATH);

  // a given ':' marks the end of the string
  // itself and the rest will be truncated
  lpChar = (char*)strchr(portName, ':');

  if (NULL != lpChar )
    *lpChar = '\0';

  success = GetDefaultCommConfigA(portName, &cc, &ccSize);

  return success==TRUE;
}

/*------------------------------------------------------------------------
 Function:      CollectPorts
 Purpose:       Returns a list of names for accessable serial ports.
                All names are zero-terminated. The last string has
                a length of zero.
                If buffer is NULL or cbSize is zero, only the required
                size (see Input) will be stored and the the function returns
                false. If then *cbNeeded is zero an error occured while
                getting the name list (EnumPorts).
 Input:         Pointer to memory, where the list will be stored
                (NULL allowed).
                Size of the given memory block (zero allowed)
                Pointer to uint32_t variable receiving the required space
                for the whole list.
 Output:        True, if the name list was successfully copied to buffer.
                *cbNeeded will hold the size of the copied list.
                If buffer had been NULL or cbSize had been zero, no copy
                would have been done, only the minimal required space
                will be stored in *cbNeeded.
 Errors:        Refer to:
                EnumPorts, LocalAlloc
------------------------------------------------------------------------*/
bool Serial_CollectPorts(char *buffer, uint32_t cbSize, uint32_t *cbNeeded){

  DWORD cReturned;

  if (NULL == cbNeeded) // cbNeeded required!
    return false;

  // No copy, only getting the size of needed memory space
  if ( !EnumPorts(NULL, 1, NULL, 0, (DWORD*)cbNeeded, &cReturned) )
  {
    if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
    {
      *cbNeeded = 0;
      return false;
    }
  }

  (*cbNeeded)++; // additional space for trailing zero-length string

  // if only the size is requested
  if ( (NULL == buffer) || (cbSize < *cbNeeded) )
  {
    return false;
  }
  else
  {
    PORT_INFO_1 *pi;

    pi = LocalAlloc(LPTR, *cbNeeded); // get local memory block

    if ( NULL == pi )
      return false;

    // copying the list
    EnumPorts(NULL, 1, (LPBYTE)pi, *(DWORD*)cbNeeded, (DWORD*)cbNeeded, &cReturned);

    (*(DWORD*)cbNeeded)++;

    // filtering serial ports off any others...
    for( DWORD i = 0; i < cReturned; i++)
    {
      if (Serial_IsPort(pi[i].pName)){
        strcpy(buffer, pi[i].pName);
        buffer += strlen(buffer) + 1;
      }
    }

    // end-mark of the list
    *buffer = '\0';

    LocalFree(pi);
    return true;
  }
}

/*------------------------------------------------------------------------
 Function:      Close
 Purpose:       Attempt to close the port.
                Internal use only!
 Input:         Pointer to SAPI struct
 Output:        True, if operation succeeded
 Errors:        Refer to:
                CloseHandle, ZeroMemory
------------------------------------------------------------------------*/
static void Close(Serial_LPAttr lpAttr){
  CLOSEHANDLE(lpAttr->portHandle); // close port

  ZeroMemory(lpAttr, sizeof(*lpAttr)); // clear the struct (now invalid)
  lpAttr->portHandle = INVALID_HANDLE_VALUE; // make handle invalid
  lpAttr->SerialID = Serial_ID; // and the struct valid again!
}

/*------------------------------------------------------------------------
 Function:      Open
 Purpose:       Tries to open a named port. The port name have to be zero
                terminated! Otherwise you could involve bad memory access...
                TX will be in non-signaled break-state.
 Input:         Pointer to the SAPI struct.
                Zero-terminated string (name of the port)
 Output:        True, if operation succeeded
 Errors:        ERROR_ALREADY_ASSIGNED: if member portHandle is not INVALID_HANDLE_VALUE
                ERROR_BAD_ARGUMENTS: if there is no serial port named by given string
                Refer to:
                IsValidAttrParam, CreateFile, SetupComm,
                GetCommState, SetThresholds, SetCommState
                ClearCommBreak, GetAttr
------------------------------------------------------------------------*/
bool Serial_Open(Serial_LPAttr lpAttr, const char* lpszPortName){
  DCB dcb={sizeof(DCB)};
  HKEY hKey;
  //lint -e{785} few initializers for osv
  OSVERSIONINFO osv = {sizeof(OSVERSIONINFO)};

  (void) GetVersionEx( &osv );

  dwPlatformId = osv.dwPlatformId;

  if( VER_PLATFORM_WIN32_NT == dwPlatformId )
  {
    dbgprintf( 2, "OS is WinNT" );
    is_WinNT_os = true;
  }

  if (!IsValidAttrParam(lpAttr))
    return false;

  // Since named objects are case sensitive, but port names are
  // not, it's urgent to make an unique name from that port name:
  if( lpszPortName )
  {
    char *p;

    // skip leading space characters
    for( ; *lpszPortName && isspace(*lpszPortName); lpszPortName++ )
      ;

    (void)snprintf( lpAttr->portName, sizeof(lpAttr->portName), "%.*s", MAX_PATH, lpszPortName);

    // a given ':' marks the end of the string
    // itself and the rest will be truncated
    p = (char*)strchr(lpAttr->portName, ':');

    if( NULL != p )
      *p = '\0';

    p = lpAttr->portName;

    if( strlen(p) )
    {
      p = &p[strlen(p)-1];

      // erase trailing white space;
      while( p > lpAttr->portName )
      {
        if( ' ' != *p )
          break;

        *p-- = 0;
      }

      (void)CharUpperA( lpAttr->portName );
    }
  }
  else
  {
    SetLastError(ERROR_BAD_ARGUMENTS);
    DisplayLastError(0);
    return false;
  }

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (lpAttr->portHandle != INVALID_HANDLE_VALUE)
    EXIT_CRITICAL_FUNC(ERROR_ALREADY_ASSIGNED);

  if (!Serial_IsPort(lpAttr->portName)) // portname valid?
    EXIT_CRITICAL_FUNC(ERROR_BAD_ARGUMENTS);

  // Retrieve fifo settings
  // You need administrator rights to access the information, if the uarts
  // fifo is enabled. If you're not sure and want to keep your app save
  // you can set these values manually _after_ the Serial_Open function.

  if(	!lpAttr->fSkipFifoTest ) // Skip fifo-test?
  {
    lpAttr->FifoEnabled = -1;
    lpAttr->FifoRxSize = -1;
    lpAttr->FifoTxSize = -1;

    hKey = GetDeviceKey( lpAttr->portName, KEY_READ );

    if( INVALID_HANDLE_VALUE != hKey )
    {
      (void)TouchFifoEnable( hKey, &lpAttr->FifoEnabled );
      (void)TouchFifoSize( hKey, &lpAttr->FifoRxSize, &lpAttr->FifoTxSize );

      (void)RegCloseKey( hKey );
    }
  }

  lpAttr->portHandle=CreateFileA( // open port
    lpAttr->portName,
    GENERIC_READ | GENERIC_WRITE, 0,
    0, OPEN_EXISTING,
    0, NULL);

  if (INVALID_HANDLE_VALUE == lpAttr->portHandle) // successful?
    EXIT_CRITICAL_FUNC(GetLastError());

  if( !nPortsOpen )
  {
    nPortsOpen++;
    timeInit();
  }

  // SetBufferSizes is deliberately not called, because this would emit
  // two calls to SetCommState which requires several seconds due to Windows.
  // So the executing time for Open would be more as necessary.
  if (!SetupComm(lpAttr->portHandle, RX_BUFFERSIZE, TX_BUFFERSIZE)) // set default buffersizes
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!GetCommState(lpAttr->portHandle, &dcb)) // get valid DCB
    EXIT_CRITICAL_FUNC(GetLastError());

  /*

  Set no flow control by default ?

  dcb.fOutxCtsFlow=false;
  dcb.fOutxDsrFlow=false;
  dcb.fDsrSensitivity=false;

  dcb.fRtsControl=RTS_CONTROL_DISABLE;
  dcb.fDtrControl=DTR_CONTROL_DISABLE;

  dcb.fOutX=false;
  dcb.fInX=false;

  */

  SetThresholds(&dcb, RX_BUFFERSIZE);

  if (!SetCommState(lpAttr->portHandle, &dcb)) // set new DCB
    EXIT_CRITICAL_FUNC(GetLastError());

  // Clear a possible break state of the TX line
  if (!ClearCommBreak(lpAttr->portHandle))
    EXIT_CRITICAL_FUNC(GetLastError());

  lpAttr->Outputs = 0;

  if (!Serial_GetAttr(lpAttr)) // update the SAPI struct
    EXIT_CRITICAL_FUNC(GetLastError());

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      Close
 Purpose:       Attempt to close a previusly opened port.
 Input:         Pointer to SAPI struct
 Output:        True, if operation succeeded
 Errors:        Refer to:
                CheckAttrOpened, Close
------------------------------------------------------------------------*/
bool Serial_Close(Serial_LPAttr lpAttr){

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  Close(lpAttr);

  if( !--nPortsOpen )
  {
    timeDeinit();
  }

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      CancelRx
 Purpose:       Discard all pending chars in any RX buffers
 Input:         Pointer to SAPI struct
 Output:        True, if operation succeeded
 Errors:        Refer to:
                CheckAttrOpened, PurgeComm
------------------------------------------------------------------------*/
bool Serial_CancelRx(Serial_LPCAttr lpAttr){

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  // Reset RX buffers
  if (!PurgeComm(lpAttr->portHandle, PURGE_RXABORT | PURGE_RXCLEAR))
    EXIT_CRITICAL_FUNC(GetLastError());

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      Write
 Purpose:       Sends a string to the port. The length of the string must
                be given by Cnt. If Cnt is negative the string is assumed to
                be zero-terminated.
                If flag fEcho in the SAPI struct is set, Write expects
                the sent chars incoming too and removes them from the buffer.
                Any errors (no chars in RX buffer, or no agreement with
                the sent characters) will cause a faulty return.
                If any chars still left in RX buffer before the write
                operation begins the function will return with an error.
                This behaviour is necessary, because the echo detection
                depends on the RX buffer which is organized as fifo.
                Note that the read timeouts regulate here the echo
                detection! The function will omit ERROR_SERIAL_ECHO_FAILED,
                if a read timeout occures.
 Input:         Pointer to SAPI struct
                Pointer to memory where the string is located
                Amount of chars the string holds (value zero allowed)
 Output:        True, if operation succeeded
 Errors:        ERROR_BAD_ARGUMENTS: lpstrBuffer is NULL
                ERROR_WRITE_FAULT: not all chars successfully sent
                ERROR_SERIAL_ECHO_FAILED: error reading echo chars
                Refer to:
                CheckAttrOpened, SetOutputs, CancelRx
                WriteFile, ReadFile,
------------------------------------------------------------------------*/
bool Serial_Write(Serial_LPAttr lpAttr, const char* lpstrBuffer, int32_t Cnt){
  DWORD nWritten, nRead;
  char buffer[100];
  char *lpBuffer;
  DWORD readCount;

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  if (NULL == lpstrBuffer) // valid pointer?
    EXIT_CRITICAL_FUNC(ERROR_BAD_ARGUMENTS);

  if (!Serial_SetOutputs(lpAttr, TX_NORM)) // remove break signal, if any
    EXIT_CRITICAL_FUNC(GetLastError());

  if ( Cnt < 0 ) // string length unknown? (zero-terminated?)
    Cnt = (int32_t)strlen(lpstrBuffer);

  if ( 0 == Cnt )
    EXIT_CRITICAL_FUNC(NO_ERROR); // No chars to send? Then already done....

  if (lpAttr->fEcho) // echo detection? (Assumes RX buffer to be empty!)
    if ( 0 != Serial_RxCount(lpAttr))
    {
      EXIT_CRITICAL_FUNC(GetLastError());
    }

  if (!WriteFile(lpAttr->portHandle, lpstrBuffer, (uint16_t)Cnt, &nWritten, NULL)) // send string
      EXIT_CRITICAL_FUNC(GetLastError()); // an error occured

  // There is no trick to validate all chars to be sent truely.
  // The only possibility is to check the returned value of
  // the function WriteFile that in turn takes no regard on
  // any possibly available hardware buffers...
  if(nWritten != (uint16_t)Cnt) // Count equals amount had to be sent?
    EXIT_CRITICAL_FUNC(ERROR_WRITE_FAULT);

  if (!lpAttr->fEcho) // echo detection?
    EXIT_CRITICAL_FUNC(NO_ERROR); // No, then we're ready here

  while(Cnt>0) // remove all remaining echo chars...
  {
    readCount = min((uint16_t)Cnt, sizeof(buffer)); // read as much as possible

    if (!Serial_Read(lpAttr, buffer, readCount, (uint32_t*)&nRead))
      EXIT_CRITICAL_FUNC(ERROR_SERIAL_ECHO_FAILED);

    if (nRead != readCount)
      EXIT_CRITICAL_FUNC(ERROR_SERIAL_ECHO_FAILED);

    Cnt -= (int32_t)nRead; // any remaining chars to read?

    lpBuffer=buffer;

    while (readCount--)
      if (*lpBuffer++ != *lpstrBuffer++) // echo chars must equal
        EXIT_CRITICAL_FUNC(ERROR_SERIAL_ECHO_FAILED);
  }

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      WaitChar
 Purpose:       Waits for a character for a max. specified time. Common
                timeouts were disregarded! Only the given time is used.
                The difference between WaitChar and GetChar is, that WaitChar
                leaves the char in the queue.
                This function is not available, cause here are time measurents
                which maybe don't work on a high loaded system!
                Be careful when using this function. Windows may take several
                milliseconds between any instruction, and this will not be
                detectable! If you want to use it, you must declare the function
                in your source.
 Input:         Pointer to the SAPI struct
                The timeout value in ms.
 Output:        True, if operation succeeded (char is placed in rx queue)
 Errors:        Refer to:
                CheckAttrOpened
------------------------------------------------------------------------*/
int32_t Serial_WaitChar(Serial_LPAttr lpAttr, uint32_t dwTimeout)
{
  uint32_t rxCount = (uint32_t)-1;
  int32_t result;

  BEGIN_CRITICAL_FUNC( lpAttr );

  //lint -e{717} do...while(0) loop
  do
  {
    // Only open ports allowed
    if (!CheckAttrOpened(lpAttr))
      break;

    // Reset error flag
    SetLastError( NO_ERROR );

    // zero means no timeout
    if( !dwTimeout )
    {
      rxCount = Serial_RxCount( lpAttr );
    }
    else
    {
      uint64_t now, target;

      now = timeGet();

      // calculate the absolute timeout
      target = now + (uint64_t)dwTimeout*10;

      rxCount = 0;

      while( !rxCount && (now < target) )
      {
        now = timeGet();

        // Any chars in the rx queue ?
        rxCount = Serial_RxCount( lpAttr );

        if( (uint32_t)-1 == rxCount )
          break;

        if( now + 10 < target )
          Sleep(1); // Save some cpu time
      }
    }
  }while(0);

  if( (uint32_t)-1 == rxCount )
    result = -1;
  else
    result = (int32_t)rxCount;

  END_CRITICAL_FUNC_VAL( result );
}

/*------------------------------------------------------------------------
 Function:      GetChar
 Purpose:       Tries to read only one char from the port in a given
                time window.
 Input:         Pointer to the SAPI struct
                Pointer to memory space receiving the char
                The timeout value in ms.
 Output:        True, if operation succeeded (char could be read)
                The char will be put at *lpChar (if not NULL)
 Errors:        ERROR_READ_FAULT: if no char is available
                Refer to:
                CheckAttrOpened, ReadFile
------------------------------------------------------------------------*/
bool Serial_GetChar(Serial_LPCAttr lpAttr, char * lpChar, uint32_t dwTimeout)
{
  DWORD nRead;
  CHAR ch;
  COMMTIMEOUTS oldTO, newTO;
  bool success;

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (NULL == lpChar) // caller not interested in that char?
    lpChar = &ch;

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  (void) GetCommTimeouts(lpAttr->portHandle, &oldTO); // save old timeouts

  newTO = oldTO;

  newTO.ReadIntervalTimeout = 0;
  newTO.ReadTotalTimeoutMultiplier = 0;
  newTO.ReadTotalTimeoutConstant = dwTimeout;

  (void) SetCommTimeouts(lpAttr->portHandle, &newTO);

  success = ReadFile(lpAttr->portHandle, lpChar, 1, &nRead, NULL);

  (void) SetCommTimeouts(lpAttr->portHandle, &oldTO); // restore old timeouts

  // If ReadFile times out, it will return TRUE!
  // Otherwise an error occured
  if (!success)
    EXIT_CRITICAL_FUNC(GetLastError());

  if (!nRead) // any char available?
    EXIT_CRITICAL_FUNC(ERROR_READ_FAULT); // No, so a timeout occured

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      Read
 Purpose:       Read of several char from the port.
 Input:         Pointer to the SAPI struct
                Pointer to memory space that receives the amount of
                successfully read chars (NULL allowed to get no info about).
                Count of chars to read.
                Pointer to a 32 bit variable (or NULL) that receives the
                amount of successfully read chars.
                For the overlapped implementation an echo detection has
                to be implemented. See section echo handling                    //! Implement echo handling
 Output:        True, if operation succeeded
                The count of chars read will be returned by lpiCntRead,
                if not NULL
 Errors:        ERROR_BAD_ARGUMENTS: if lpstrBuffer is NULL
                ERROR_READ_FAULT: if a read timeout occured
                Refer to:
                CheckAttrOpened, ReadFile
------------------------------------------------------------------------*/
bool Serial_Read(Serial_LPCAttr lpAttr, char* lpstrBuffer, uint32_t Cnt, uint32_t *lpCntRead)
{
  DWORD nRead;

  BEGIN_CRITICAL_FUNC(lpAttr);

  if (NULL != lpCntRead) // Return the count of successfully read chars?
    *lpCntRead = 0;      // First set to zero

  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  if (NULL == lpstrBuffer) // memory block valid?
    EXIT_CRITICAL_FUNC(ERROR_BAD_ARGUMENTS);

  if (0 == Cnt) // no chars to read?
    EXIT_CRITICAL_FUNC(NO_ERROR);

  if (!ReadFile(lpAttr->portHandle, lpstrBuffer, (DWORD)(uint16_t)Cnt, (DWORD*)&nRead, NULL))
    EXIT_CRITICAL_FUNC(GetLastError());

  if( (DWORD)Cnt != nRead )
  {
    dbgprintf(0, "Timeout\n");
    SetLastError( ERROR_READ_FAULT );
  }

  if( NULL != lpCntRead )
  {
    dbgprintf(0, "Timeout\n");
    *lpCntRead = (uint32_t)nRead;
  }

  if (nRead != (DWORD)(uint16_t)Cnt) // All chars received?
    EXIT_CRITICAL_FUNC(ERROR_READ_FAULT); // No, Timeout!

  if (NULL != lpCntRead) // Return the count of successfully read chars?
    *lpCntRead = (uint32_t)nRead;

  END_CRITICAL_FUNC(NO_ERROR);
}

/*------------------------------------------------------------------------
 Function:      SenseRead
 Purpose:       Read of several chars from the port.
 Input:         Pointer to the SAPI struct
                Pointer to memory space that receives the amount of
                successfully read chars (NULL allowed to get no info about).
                Count of chars to read.
                Pointer to a 32 bit variable (or NULL) that receives the
                amount of successfully read chars.
                You may change the read timouts (SAPI struct) before you
                call Read _without_ explicitely call SettAttr!
                The timouts given in the SAPI struct will be used.
                For the overlapped implementation an echo detection has
                to be implemented. See section echo handling
                This function is not exported, cause here are time measurents
                which maybe don't work on a high loaded system!
                Be careful when using this function. Windows may take several
                milliseconds between any instruction, and this will not be
                detectable! If you want to use it, you must declare the function
                in your source.
 Output:        True, if operation succeeded
                The count of chars read will be returned by lpCntRead,
                if not NULL
 Errors:        ERROR_BAD_ARGUMENTS: if lpstrBuffer is NULL
                ERROR_READ_FAULT: if a read timeout occured
                Refer to:
                CheckAttrOpened, ReadFile
------------------------------------------------------------------------*/
bool Serial_SenseRead( Serial_LPAttr lpAttr,
                       char* lpstrBuffer,
                       uint32_t Cnt,
                       uint32_t *lpCntRead )
{
  bool fCriticalFifo = false;
  COMMTIMEOUTS oldTO, newTO;
  bool fBufferAllocated = false;

  BEGIN_CRITICAL_FUNC(lpAttr);

  // Only a valid opened port is accepted
  if (!CheckAttrOpened(lpAttr))
    EXIT_CRITICAL_FUNC(GetLastError());

  // Caller wants to know, how many chars will be read
  if (NULL != lpCntRead)
    *lpCntRead = 0;

  // No chars to read, then we're ready here...
  if ( 0 == Cnt )
    EXIT_CRITICAL_FUNC(NO_ERROR);

  // Caller gave no buffer, so allocate an own
  if ( NULL == lpstrBuffer )
  {
    lpstrBuffer = malloc( (size_t)Cnt );

    if( NULL != lpstrBuffer )
    {
      fBufferAllocated = true;
    }
    else
    {
      EXIT_CRITICAL_FUNC( GetLastError() );
    }
  }

  // Get the old timeouts
  (void) GetCommTimeouts(lpAttr->portHandle, &oldTO);

  newTO = oldTO;

  // Check if we will have trouble with the fifo. That is on WinNt OS
  // with fifo enabled. Timout may occur although there were still chars
  // in fifo buffer. There is no way to check for chars in the fifo.
  // The only way is to wait, till there must not be any chars left...
  // You need administrator rights to access the information, if the uarts
  // fifo is enabled. If you're not sure and want to keep your app save
  // you can set these value manually _after_ the Serial_Open function.
  if( is_WinNT_os )
  {
    // Unknown if fifo is enabed? Then safe mode!
    // To avoid this, set FifoEnabled to zero manually!
    // Set settings for fifoTrigger too!
    if( -1 == lpAttr->FifoEnabled )
    {
      // Worst case?
      fCriticalFifo = true;
    }

    if( ( lpAttr->FifoEnabled > 0 ) && ( lpAttr->FifoRxSize > 1 ) )
    {
      // if no timeouts are used, we pay no attention to the fifo conflict...
      if(    lpAttr->ReadTimeoutConstant || lpAttr->ReadTimeoutMultiplier )
      {
        fCriticalFifo = true; // Ok, use own read loop
      }
      else
      {
        if(    lpAttr->ReadIntervalTimeout
            && (MAXDWORD != lpAttr->ReadIntervalTimeout ) )
        {
          fCriticalFifo = true; // Ok, use own read loop
        }
      }
    }
  }

  // May be there some trouble with the fifo?
  if( !fCriticalFifo )
  {
    DWORD nRead;

    // No, use default read operation
    (void) ReadFile( lpAttr->portHandle,
                     lpstrBuffer,
                     (DWORD)Cnt,
                     &nRead,
                     NULL );

    if( (DWORD)Cnt != nRead )
    {
      dbgprintf(0, "Timeout\n");
      SetLastError( ERROR_READ_FAULT );
    }

    if( NULL != lpCntRead )
    {
      dbgprintf(0, "Timeout\n");
      *lpCntRead = (uint32_t)nRead;
    }
  }
  else
  {
    uint64_t timeoutInterval, timeout;
    uint64_t timeOffset, now, timeLastRcv;
    bool fTimeOut;
    uint16_t fifoTrigger;
    uint32_t nCharsReceived;
    char *lpBuffer;
    double timeOneChar;
    uint16_t fifoDelayChars;

    // the maximum wait time occurs, if fifo is load with
    // at least one char less. Then system waits then for
    // 4 character lengths before an interrupt is generated.
    // So maximum time is fifosize + 3 character lengths

    // Get the count of chars the fifo max holds
    // Not greater than 14? I hope so...
    fifoTrigger = (uint16_t)min( 14, lpAttr->FifoRxSize );

    // Fifo size or usage is unknown
    if( (uint16_t)-1 == fifoTrigger )
    {
      // Worst case?
      fifoTrigger = 14;
    }

    // Additional delaying chars by fifo
    fifoDelayChars = (Cnt % fifoTrigger) ? (uint16_t)(((Cnt/fifoTrigger+1)*fifoTrigger+3)-Cnt) : 0;

    timeOneChar = Serial_TimeOneChar( lpAttr );

    // estimate the common timeout for the job
    timeout =    (uint64_t)Cnt * lpAttr->ReadTimeoutMultiplier
               + lpAttr->ReadTimeoutConstant;

    // Increase timeout for chars, the user don't know about
    if( timeout )
      timeout += (DWORD)(fifoDelayChars * timeOneChar);

    timeoutInterval = lpAttr->ReadIntervalTimeout;

    // in 0.1ms intervals!
    timeout *= 10;
    timeoutInterval *= 10;

    // Only to save cpu time: a read operation will abort, if chars were read
    // or one millisecond elapsed
    newTO.ReadTotalTimeoutMultiplier = 1;
    newTO.ReadTotalTimeoutConstant = 0;
    newTO.ReadIntervalTimeout = 0;

    // Set the new timeouts
    (void)SetCommTimeouts( lpAttr->portHandle, &newTO );

    timeOffset = timeGet();

    // An accuracy of 1 ms would be nice, but on some machines
    // perhaps not possible. Then quantization error becomes huge...
    // So minimize time quantization error:
    do
    {
      // Minimize time quantization error
      now = timeGet();

    }while( now-timeOffset < 10 ); // Systems' accuracy is at least 1ms!

    // Now at a boundary!
    timeLastRcv = timeOffset = now;

    fTimeOut = false;
    lpBuffer = lpstrBuffer;
    nCharsReceived = 0;

    // absolute timout used?
    if( timeout )
      timeout += timeOffset;

    while( Cnt && !fTimeOut)
    {
      DWORD averageTimePerChar;
      DWORD rxCount;
      DWORD nRead;

      // First attempt with a max delay time of 1 ms (multiplier)
      // (better than Sleep, cause delay may be smaller!)
      (void)ReadFile( lpAttr->portHandle, lpBuffer, 1, &nRead, NULL );

      if( nRead )
      {
        // Get the amount of chars ready to read
        //lint -e{666} side effects in next statement
        rxCount = min( (DWORD)Cnt-1, Serial_RxCount( lpAttr ) );

        // Second attempt read all available chars
        (void)ReadFile( lpAttr->portHandle,
                        &lpBuffer[1],
                        rxCount,
                        &nRead,
                        NULL );

        nRead ++; // One char already read before...
      }

      now = timeGet();

      lpBuffer += nRead;
      Cnt -= nRead;
      nCharsReceived += nRead;

      // average time elapsed for one char
      averageTimePerChar = (DWORD)((now - timeLastRcv) * 10); // Factor 10 to increase resolution
      // If nRead>0 measurement ist more exact...

      averageTimePerChar /= nRead ? ( (nRead % fifoTrigger) ? (nRead+4) : nRead ) : (fifoTrigger + 3);

      if( nRead )
        timeLastRcv = now;

      dbgprintf(1, "tAbs: %7.3f | tChr: %7.3f | Rcv: %d\n",
                (double)(now-timeOffset)/10.0,
                (double)averageTimePerChar/100.0,
                nRead );

      // Test interval timeout
      if( timeoutInterval && ( averageTimePerChar > timeoutInterval*10 ) )
      {
        dbgprintf(0, "Interval timeout!\n" );
        fTimeOut = true;
        SetLastError( ERROR_READ_FAULT );
      }

      // Test total timeout
      if( timeout && ( now > timeout ) )
      {
        dbgprintf(0, "Total timeout!\n" );
        fTimeOut = true;
        SetLastError( ERROR_READ_FAULT );
      }
    }

    // Caller wants to know, how many chars were read?
    if( NULL != lpCntRead )
      *lpCntRead = nCharsReceived;

    // restore the old timeouts
    (void) SetCommTimeouts(lpAttr->portHandle, &oldTO);
  }

  // free the allocated space, if any
  if( fBufferAllocated )
    free( lpstrBuffer );

  END_CRITICAL_FUNC( GetLastError() );
}


/////////////////////////////////////////////////////////////////////////
//
// Fifo access functions
//
// Idea is taken from the site
// http://www.codeproject.com/useritems/Serial_portsEnum_FIFO.asp
// enumerate serial ports and manage FIFO using Device Manager
// and Configuration Manager
// Thanks to Johnny Bowlegs



// Function GetDeviceKey
//-----------------------
// Returns the registry key for the device named by portName
// portName must not have any space charaters around
// the string compare is not case sensitive, trailing ":" is
// allowed
static HKEY GetDeviceKey( LPCSTR portName, int access )
{
  typedef DWORD (WINAPI *CM_Open_DevNode_Key)( DWORD ,
                                               DWORD ,
                                               DWORD ,
                                               DWORD ,
                                               PHKEY ,
                                               DWORD );

  HINSTANCE CfgMan;
  CM_Open_DevNode_Key OpenDevNodeKey=NULL;
  #ifndef RegDisposition_OpenExisting
  #  define RegDisposition_OpenExisting (0x00000001)
  #endif
  #ifndef CM_REGISTRY_HARDWARE
  #  define CM_REGISTRY_HARDWARE (0x00000000)
  #endif
  HDEVINFO deviceInfoSet = INVALID_HANDLE_VALUE;
  HKEY keyDevice = INVALID_HANDLE_VALUE;
  int state = -1; // -1=failed, 0=still searching, 1=found

  if( (access != KEY_READ) && (access != KEY_WRITE) )
  {
    dbgprintf(1,"Only KEY_READ or KEY_WRITE allowed!");
    return INVALID_HANDLE_VALUE;
  }

  if( KEY_WRITE == access )
    access |= KEY_READ;

  if( !portName )
  {
    dbgprintf(1,"Portname is NULL!");
    return INVALID_HANDLE_VALUE;
  }

  if( !strlen(portName) )
  {
    dbgprintf(1,"Portname has no chars!");
    return INVALID_HANDLE_VALUE;
  }

  // DLL of configuration manager must be loaded at runtime!
  // Win98 doesn't serve this dll, so your application using SAPI
  // will terminate immediately, cause the dll is missing at startup!
  CfgMan=LoadLibrary("cfgmgr32");

  if(!CfgMan)
  {
    dbgprintf(1,"Unable to load configuration manager dynamic library");
    return INVALID_HANDLE_VALUE;
  }

  OpenDevNodeKey=(CM_Open_DevNode_Key)GetProcAddress(CfgMan,"CM_Open_DevNode_Key");

  if(!OpenDevNodeKey)
  {
    (void)FreeLibrary(CfgMan);
    dbgprintf(1,"Can't locate proc address of CM_Open_DevNode_Key");
    return INVALID_HANDLE_VALUE;
  }

  // Get the device info set
  //lint -e{717} do{}while(0)
  do
  {
    GUID *buffer = NULL;
    DWORD requiredSize = 0;
    const char *className = "Ports";

    // First retrieve the size needed for Class GUIDs
    (void)SetupDiClassGuidsFromNameA(className,0,0,&requiredSize);

    if( requiredSize < 1 )
      break;

    buffer = (GUID*) malloc( requiredSize * sizeof(GUID) );

    if( SetupDiClassGuidsFromNameA(  className,
                                     buffer,
                                     requiredSize*sizeof(GUID),
                                     &requiredSize
                                   ) )
    {
      deviceInfoSet = SetupDiGetClassDevsA( buffer,
                                            NULL,
                                            NULL,
                                            DIGCF_PRESENT
                                          );
    }

    free( buffer );

  }while(0);

  // Valid device info set?
  if( INVALID_HANDLE_VALUE != deviceInfoSet )
  {
    state = 0; // searching
    char portDelim = portName[strlen(portName)-1];

    // Now enumerate for the needed port and its device info
    for( int i=0; 0 == state; i++ )
    {
      //lint -e{785} few initializers for deviceInfoData
      SP_DEVINFO_DATA deviceInfoData = {sizeof(SP_DEVINFO_DATA)};
      char devName[MAX_PATH+1] = {0};
      DWORD len = sizeof(devName)-1;
      const char *regName = "PortName";

      state = -1; // "failed" as preset

      // Retrieve device info data
      if( SetupDiEnumDeviceInfo( deviceInfoSet,
                                 (DWORD)i,
                                 &deviceInfoData
                                ) )
      {
        // Query the registry key name for the device
        if( NO_ERROR == OpenDevNodeKey( deviceInfoData.DevInst,
                                        KEY_QUERY_VALUE | access, // KEY_READ or KEY_WRITE
                                        0,
                                        RegDisposition_OpenExisting,
                                        &keyDevice,
                                        CM_REGISTRY_HARDWARE
                                      ) )
        {
          // Open the registry key
          if( NO_ERROR == RegQueryValueExA( keyDevice,	     // handle of key to query
                                            regName,	     // address of name of value to query
                                            NULL,	         // reserved
                                            NULL,	         // address of buffer for value type
                                            (LPBYTE)devName, // address of data buffer
                                            &len 	         // address of data buffer size
                                          ) )
          {
            if( strlen(devName) )
            {
              char *p2 = &devName[strlen(devName)-1];

              // Test trailing ":"
              if( portDelim != *p2 )
              {
                if( ':' == portDelim )
                  strcat( p2, ":" );
                else
                  *p2 = (char)0;
              }

              // Compare if the name of the device is the one we want...
              state = (0 == stricmp( devName, portName )) ? 1 : 0;

              if(!state)
                (void)RegCloseKey( keyDevice );
            }
          }
        }
      }
    }

    (void)SetupDiDestroyDeviceInfoList(deviceInfoSet);
  }

  (void)FreeLibrary(CfgMan);

  if( -1 == state )
    dbgprintf(2,"Failed to access registry!");

  return (1 == state) ? keyDevice : INVALID_HANDLE_VALUE;
}

// Function TouchFifoEnable
//--------------------------
// changes FIFO usage: enable=0 -> turn off, enable>0 -> turn on, enable<0 -> don't change
// returns the success of the function (key access)
// if fEnable the old setting will be stored (-1 means error while getting the flag)
static bool TouchFifoEnable( HKEY hKey, int16_t *fEnable )
{
  unsigned char data[100];
  DWORD cbData = sizeof( data );
  DWORD type;
  const char *keyName_NT = "ForceFifoEnable";
  const char *keyName_W32 = "Settings";
  const char *keyName;
  int16_t fCurrent = -1;

  switch( dwPlatformId )
  {
    case VER_PLATFORM_WIN32_NT:
      keyName = keyName_NT;
      break;

    case VER_PLATFORM_WIN32_WINDOWS:
      keyName = keyName_W32;
      break;

    default:
      return false;
  }

  if( NO_ERROR == RegQueryValueExA( hKey,	        // handle of key to query
                                    keyName,	      // address of name of value to query
                                    NULL,         	// reserved
                                    &type,        	// address of buffer for value type
                                    data,	        // address of data buffer
                                    &cbData 	      // address of data buffer size
                                  ) )
  {
    switch( dwPlatformId )
    {
      case VER_PLATFORM_WIN32_NT:
        fCurrent =  data[0] ? 1 : 0;

        // Change current setting?
        if( *fEnable >=0 )
        {
          data[0] = *fEnable ? 1 : 0;
        }
        break;

      case VER_PLATFORM_WIN32_WINDOWS:
        if( 0x02 == data[0] )
          fCurrent = 1;

        if( 0x00 == data[0] )
          fCurrent = 0;

        // Change current setting?
        if( (*fEnable >= 0) && (fCurrent >= 0) )
        {
          if( *fEnable )
          {
            data[0] = 0x02;
          }
          else
          {
            data[0] = 0x00;
          }
        }
        break;

      default:
        break;
    }

    *fEnable = fCurrent;

    // Change th current stting?
    // Only if read was successful
    if( (*fEnable >= 0) && (fCurrent >= 0) )
    {
      if( !RegSetValueExA( hKey,
                           keyName,
                           0,
                           type,
                           data,
                           cbData
                         ) )
      {
        fCurrent = -1; // Indicate the error
      }
    }
  }

  if( -1 == fCurrent )
    dbgprintf(2,"Failed to access the registry!");

  return fCurrent >= 0;
}

// Function TouchFifoSize_NT
//---------------------------
// Retrieves the actual size of the uart fifo buffer.
// Returns true if success or false, if a failure occured while reading
// or writing the registry key
// negative sizes will not change the actual setting.
// However, the current setting will be written in rxSize and txSize
// rxSize and txSize my be NULL
// Dont call this function explicitly! Use TouchFifoSize!
static bool TouchFifoSize_NT( HKEY hKey, int16_t *rxSize, int16_t *txSize )
{
  bool success = false;
  bool fWriteAccess;

  switch( *rxSize )
  {
    case -1:
    case 1:
    case 4:
    case 8:
    case 14:
      break;

    default:
      return false;
  }

  if( (*txSize != -1) && ((*txSize < 1) || (*txSize > 16)) )
    return false;

  fWriteAccess = (*rxSize>0) || (*txSize>0);

  if( INVALID_HANDLE_VALUE != hKey )
  {
    success = true;

    for( int i=0; (i<2) && success; i++ )
    {
      const char *keys[2] = {"RxFIFO", "TxFIFO"};
      DWORD current;
      DWORD cbCurrent = sizeof(current);

      success = false;

      if( NO_ERROR == RegQueryValueExA( hKey,
                                        keys[i],
                                        NULL,
                                        NULL,
                                        (LPBYTE)&current,
                                        &cbCurrent
                                      ) )
      {
        success = true;

        if( i )
        {
          *txSize = (int16_t)current;
        }
        else
        {
          *rxSize = (int16_t)current;
        }

        if( fWriteAccess )
        {
          DWORD size = (uint16_t)( i ? *txSize : *rxSize );

          if( NO_ERROR != RegSetValueExA( hKey,	          // handle of key to set value for
                                          keys[i],	        // address of value to set
                                          0,	              // reserved
                                          REG_DWORD,	      // flag for value type
                                          (LPBYTE)&size, 	// address of value data
                                          sizeof(size)   	// size of value data
                                        ) )
          {
            success = false;
          }
        }
      }
    }
  }

  if( !success )
    dbgprintf(2,"Failed to access the registry!");

  return success;
}

// Function TouchFifoSize_W32
//----------------------------
// See TouchFifoSize_NT, but this version ist for non NT based
// Windows OS
static bool TouchFifoSize_W32( HKEY hKey, int16_t *rxSize, int16_t *txSize )
{
  bool success = false;
  bool fWriteAccess;

  switch( *rxSize )
  {
    case -1:
    case 1:
    case 4:
    case 8:
    case 14:
      break;

    default:
      return false;
  }

  switch( *txSize )
  {
    case -1:
    case 1:
    case 4:
    case 8:
    case 16:
      break;

    default:
      return false;
  }

  fWriteAccess = (*rxSize>0) || (*txSize>0);

  if( INVALID_HANDLE_VALUE != hKey )
  {
    unsigned char data[100];
    DWORD cbData = sizeof(data);
    DWORD valueType;
    int16_t currentRx=-1, currentTx=-1;

    if( NO_ERROR == RegQueryValueExA( hKey,	     // handle of key to query
                                      "Settings", // address of name of value to query
                                      NULL,       // reserved
                                      &valueType, // address of buffer for value type
                                      data,       // address of data buffer
                                      &cbData 	   // address of data buffer size
                                    ) )
    {
      success = true;

      switch(data[3])
      {
        case 0x00: currentRx =  1; break;
        case 0x40: currentRx =  4; break;
        case 0x80: currentRx =  8; break;
        case 0xc0: currentRx = 14; break;

        default: break;
      }

      switch(data[1])
      {
        case 0x00: currentTx =  1; break;
        case 0x06: currentTx =  4; break;
        case 0x0b: currentTx =  8; break;
        case 0xc0: currentTx = 16; break;

        default: break;
      }

      switch(*rxSize)
      {
        case 1:  data[3]=0x00;break;
        case 4:  data[3]=0x40;break;
        case 8:  data[3]=0x80;break;
        case 14: data[3]=0xc0;break;

        default: break;
      }

      switch(*txSize)
      {
        case 1:  data[1]=0x01;break;
        case 4:  data[1]=0x06;break;
        case 8:  data[1]=0x0b;break;
        case 16: data[1]=0x10;break;

        default: break;
      }

      if ( fWriteAccess )
      {
        if( NO_ERROR != RegSetValueExA( hKey,      	// handle of key to set value for
                                        "Settings",	// address of value to set
                                        0,	        // reserved
                                        valueType,	// flag for value type
                                        data,     	// address of value data
                                        cbData 	    // size of value data
                                      ) )
        {
          success = false;
        }
      }
    }

    *rxSize = currentRx;
    *txSize = currentTx;

    (void)RegCloseKey( hKey );
  }

  if( !success )
    dbgprintf(2,"Failed to access the registry!");

  return success;
}

// Function TouchFifoSize
//-----------------------
// Changes and/or retrieves the sizes for rx and rx queues fifo
static bool TouchFifoSize( HKEY hKey, int16_t *rxSize, int16_t *txSize )
{
  int16_t dummy = -1;

  if( !rxSize )
    rxSize = &dummy;

  if( !txSize )
    txSize = &dummy;

  switch( dwPlatformId )
  {
    case VER_PLATFORM_WIN32_NT:
      return TouchFifoSize_NT( hKey, rxSize, txSize );

    case VER_PLATFORM_WIN32_WINDOWS:
      return TouchFifoSize_W32( hKey, rxSize, txSize );

    default:
      break;
  }

  dbgprintf(2,"Failed to access the registry!");

  return false;
}


