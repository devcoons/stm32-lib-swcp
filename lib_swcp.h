/*!
	@file   lib_swcp.h
	@brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------
	MIT License
	Copyright (c) 2022 Ioannis Deligiannis | Devcoons Blog
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/******************************************************************************
* Preprocessor Definitions & Macros
******************************************************************************/

#ifndef LIB_SWCP_H_
#define LIB_SWCP_H_

/******************************************************************************
* Includes
******************************************************************************/

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#if __has_include("lib_crypto.h")
#include "lib_crypto.h"
	#ifdef LIB_CRYPTO_ENABLE_CRC 
		#define LIB_SWCP_ENABLED
	#endif
#endif

#ifdef LIB_SWCP_ENABLED

/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/

#if !defined(ENUM_I_STATUS)
#define ENUM_I_STATUS
typedef enum
{
	I_OK 			= 0x00,
	I_INVALID 		= 0x01,
	I_EXISTS 		= 0x02,
	I_NOTEXISTS 		= 0x03,
	I_FAILED 		= 0x04,
	I_EXPIRED 		= 0x05,
	I_UNKNOWN 		= 0x06,
	I_INPROGRESS 		= 0x07,
	I_IDLE			= 0x08,
	I_FULL			= 0x09,
	I_EMPTY			= 0x0A,
	I_YES			= 0x0B,
	I_NO			= 0x0C,
	I_SKIP			= 0x0D,
	I_LOCKED 		= 0x0E,
	I_INACTIVE 		= 0x0F,
	I_ACTIVE 		= 0x10,
	I_READY		 	= 0x11,
	I_WAIT 			= 0x12,
	I_OVERFLOW 		= 0x13,
	I_CONTINUE 		= 0x14,
	I_STOPPED 		= 0x15,
	I_WARNING 		= 0x16,
	I_SLEEP 		= 0x17,
	I_DEEPSLEEP 		= 0x18,
	I_STANDBY 		= 0x19,
	I_GRANTED 		= 0x1A,
	I_DENIED 		= 0x1B,
	I_DEBUG_01 		= 0xE0,
	I_DEBUG_02 		= 0xE1,
	I_DEBUG_03 		= 0xE2,
	I_DEBUG_04 		= 0xE3,
	I_DEBUG_05 		= 0xE4,
	I_DEBUG_06 		= 0xE5,
	I_DEBUG_07 		= 0xE6,
	I_DEBUG_08 		= 0xE7,
	I_DEBUG_09 		= 0xE8,
	I_DEBUG_10 		= 0xE9,
	I_DEBUG_11 		= 0xEA,
	I_DEBUG_12 		= 0xEB,
	I_DEBUG_13 		= 0xEC,
	I_DEBUG_14 		= 0xED,
	I_DEBUG_15 		= 0xEE,
	I_DEBUG_16 		= 0xEF,
	I_MEMALIGNED		= 0xFC,
	I_MEMUNALIGNED		= 0xFD,
	I_NOTIMPLEMENTED 	= 0xFE,
	I_ERROR 		= 0xFF
}i_status;
#endif

typedef enum
{
	swcp_type_u8 = 0x11,
	swcp_type_i8 = 0x12,
	swcp_type_u16 = 0x21,
	swcp_type_i16 = 0x22,
	swcp_type_u32 = 0x41,
	swcp_type_i32 = 0x42,
	swcp_type_arr = 0x19,
}swcp_vtp;

typedef struct
{
	uint8_t size; // in bytes (not cnt of elements in case of array)
	swcp_vtp type;
	void* pvar; // pointer of actual variable
	void* pdvar; // pointer of default-failsafe value
}swcp_entry_t;

typedef struct
{
	uint32_t mem_start;
	uint32_t mem_end;
	swcp_entry_t* entries;
	size_t        entries_cnt;
	i_status (*load)(uint32_t address, void* buffer, size_t sz);
	i_status (*save)(uint32_t address, void* buffer, size_t sz);
	i_status (*error)(swcp_entry_t*);
}swcp_t;

/******************************************************************************
* Declaration | Public Functions
******************************************************************************/

i_status swcp_initialize(swcp_t*);
i_status swcp_load(swcp_t*);
i_status swcp_sync(swcp_t*);
i_status swcp_sync_entry(i_status(*)(uint32_t, void*, size_t), i_status(*)(uint32_t, void*, size_t), uint32_t, swcp_entry_t*);
i_status swcp_sync_entry_by_pvar(swcp_t* h, uint32_t pvar);

/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
#endif
#endif
