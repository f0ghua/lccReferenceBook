/*------------------------------------------------------------------------
 Module:        Serial.h
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
                The interface should be OS indepenend.
------------------------------------------------------------------------*/

#ifndef _SAPI_H
#define _SAPI_H

//lint -esym(534, Serial_SetAttr, Serial_Write, Serial_Read, Serial_CancelRx, Serial_Close)
//lint -u

#include <stdint.h>

#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#  define WIN32
#endif

#ifdef WIN32
#  include <windows.h>
#endif

#include <stdbool.h>

// ID for a valid structure
// SERIAL_DECLARE will write it automatically in the struct header
extern const char Serial_ID[];

// Standard buffersizes for Open (WinAPI: SetupComm)
#define TX_BUFFERSIZE 0x4000
#define RX_BUFFERSIZE 0x4000

typedef struct{
    void*    portHandle;
    char     portName[MAX_PATH+1];

    const char *SerialID;

    uint32_t Baudrate;
    uint8_t  Databits;
    uint8_t  Stopbits;  // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
    uint8_t  Parity;    // EVENPARITY, ODDPARITY, NOPARITY, MARKPARITY, SPACEPARITY

    uint32_t ReadIntervalTimeout;
    uint32_t ReadTimeoutMultiplier;
    uint32_t ReadTimeoutConstant;

    uint32_t WriteTimeoutMultiplier;
    uint32_t WriteTimeoutConstant;

    uint16_t Outputs;   // refer to "Defines for SetOutputs"

    uint16_t Errors;    // refer to "Errors" (in WinAPI)

    bool     fAbortOnError; // refer to DCB
    bool     fEcho;         // Flag for simplex/duplex mode (eleminating echoed chars from the RxQueue);*/

    bool     fSkipFifoTest; // Flag: when set fifo settings won't be testet by Serial_Open
    int16_t	 FifoEnabled;  // Flag, set true if uarts fifo is enabled, -1 means "ignore"
    int16_t	 FifoRxSize; // Size of the fifos rx queue, -1 means "ignore"
    int16_t	 FifoTxSize; // Size of the fifos tx queue, -1 means "ignore"
}Serial_Attr, *Serial_LPAttr;

typedef const Serial_Attr *Serial_LPCAttr;

// Valid declaration of the SAPI struct
// if you would declare "SAPINAME(Attr) varName;" the state of hPort will be undefined!
// Use "SAPI_DECLARE(varName);" instead!
#define SAPI_DECLARE(varName) Serial_Attr varName={INVALID_HANDLE_VALUE,"",Serial_ID}

// Defines for SetOutputs...
#define RTS_ON        1
#define RTS_OFF       2
#define RTS_HANDSHAKE 3
#define RTS_MASK      3

#define DTR_ON        (1<<2)
#define DTR_OFF       (2<<2)
#define DTR_HANDSHAKE (3<<2)
#define DTR_MASK      (3<<2)

#define TX_BREAK      (1<<4) // Break signal on Tx
#define TX_NORM       (2<<4) // Non-Break on Tx
#define TX_MASK       (3<<4)

// Defines for Inputs and States
#define CTS_STATE     0x01
#define DSR_STATE     0x02
#define DCD_STATE     0x04
#define RI_STATE      0x08

// user defined errors must have set bit 29 (MSB=31)
#define ERROR_SERIAL_ECHO_FAILED ( (DWORD) (1 | (1<<29)) )

// error flag for functions State, Inputs, and Errors
#define ERROR_FAILED   0x8000

// some error defines for function Error
// preserve the value 0x8000 !
#define ERROR_RXOVER   0x0001
#define ERROR_OVERRUN  0x0002
#define ERROR_RXPARITY 0x0004
#define ERROR_FRAME    0x0008
#define ERROR_BREAK    0x0010
#define ERROR_TXFULL   0x0020
#define ERROR_PTO      0x0040
#define ERROR_IOE      0x0080
#define ERROR_DNS      0x0100
#define ERROR_OOP      0x0200
#define ERROR_MODE     0x0400


#ifndef WIN32

#define ERROR_NOT_ENOUGH_MEMORY 8

// parity (refer to WinAPI)
#define NOPARITY     0
#define ODDPARITY    1
#define EVENPARITY   2
#define MARKPARITY   3
#define SPACEPARITY  4

// stop bits (refer to WinAPI)
#define ONESTOPBIT   0
#define ONE5STOPBITS 1
#define TWOSTOPBITS  2

// event bits (refer to WinAPI)
#define EV_BREAK     64
#define EV_CTS       8
#define EV_DSR       16
#define EV_ERR       128
#define EV_RING      256
#define EV_RLSD      32
#define EV_RX80FULL  1024
#define EV_RXCHAR    1
#define EV_RXFLAG    2
#define EV_TXEMPTY   4


#define INVALID_HANDLE_VALUE (void*)(-1)

#endif /*WIN32*/

// prototypes

bool Serial_Open(Serial_LPAttr , const char*);
bool Serial_Close(Serial_LPAttr );

double Serial_TimeOneChar( Serial_LPCAttr lpAttr );

bool Serial_GetAttr(Serial_LPAttr );
bool Serial_SetAttr(Serial_LPAttr );

bool Serial_SetOutputs(Serial_LPAttr , uint16_t );
bool Serial_SetBufferSizes(Serial_LPCAttr , uint32_t , uint32_t );
bool Serial_CancelRx(Serial_LPCAttr);

uint16_t Serial_Inputs(Serial_LPCAttr );
uint16_t Serial_State(Serial_LPAttr );
uint16_t Serial_Errors(Serial_LPAttr , bool );
uint32_t Serial_RxCount(Serial_LPAttr );

bool Serial_IsPort(const char* );
bool Serial_IsOpen(Serial_LPAttr);
bool Serial_CollectPorts(char*, uint32_t , uint32_t* );

bool Serial_Write(Serial_LPAttr , const char*, int32_t );

bool Serial_GetChar(Serial_LPCAttr , char* , uint32_t );
bool Serial_Read(Serial_LPCAttr , char* , uint32_t , uint32_t *);

#endif /*_SAPI_H*/
