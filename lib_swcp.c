/*!
	@file   lib_swcp.c
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

/******************************************************************************
* Includes
******************************************************************************/

#include "lib_swcp.h"

#ifdef LIB_SWCP_ENABLED

/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/

static uint8_t rx_buffer[514];
static uint8_t tx_buffer[514];

/******************************************************************************
* Declaration | Static Functions
******************************************************************************/

inline static uint16_t rev_u16(uint16_t v)
{
	uint8_t t[2];

	t[0] = (uint8_t)((v & 0xFF00) >> 8);
	t[1] = (uint8_t)((v & 0x00FF) >> 0);

	uint16_t r = 0;
	r = (((uint16_t)t[0]) << 0);
	r |= (((uint16_t)t[1]) << 8);
	return r;
}
inline static uint32_t rev_u32(uint32_t v)
{
	uint8_t t[4];

	t[0] = (uint8_t)((v & 0xFF000000) >> 24);
	t[1] = (uint8_t)((v & 0x00FF0000) >> 16);
	t[2] = (uint8_t)((v & 0x0000FF00) >> 8);
	t[3] = (uint8_t)((v & 0x000000FF) >> 0);

	uint32_t r = 0;
	r = (((uint32_t)t[0]) << 0);
	r |= (((uint32_t)t[1]) << 8);
	r |= (((uint32_t)t[2]) << 16);
	r |= (((uint32_t)t[3]) << 24);

	return r;
}

void swcp_tx_buf_gen(swcp_entry_t* entry)
{
	uint16_t val16;
	uint32_t val32;

	switch (entry->type)
	{
	case swcp_type_u8:
	case swcp_type_i8:
		memmove(&tx_buffer[1], entry->pvar, 1);
		memmove(&tx_buffer[2], entry->pvar, 1);
		tx_buffer[0] = checksum_8(&tx_buffer[1], 2);
		break;
	case swcp_type_u16:
	case swcp_type_i16:
		val16 = rev_u16(*(uint16_t*)entry->pvar);
		memmove(&tx_buffer[1], (uint8_t*)&val16, 2);
		memmove(&tx_buffer[3], (uint8_t*)&val16, 2);
		tx_buffer[0] = checksum_8(&tx_buffer[1], 4);
		break;
	case swcp_type_u32:
	case swcp_type_i32:
		val32 = rev_u32(*(uint32_t*)entry->pvar);
		memmove(&tx_buffer[1], (uint8_t*)&val32, 4);
		memmove(&tx_buffer[5], (uint8_t*)&val32, 4);
		tx_buffer[0] = checksum_8(&tx_buffer[1], 8);
		break;
	case swcp_type_arr:
		memmove(&tx_buffer[1], entry->pvar, entry->size);
		memmove(&tx_buffer[1 + entry->size], entry->pvar, entry->size);
		tx_buffer[0] = checksum_8(&tx_buffer[1], 2 * entry->size);
		break;
	}
}
/******************************************************************************
* Definition  | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Public Functions
******************************************************************************/

i_status swcp_initialize(swcp_t* h)
{
	for (size_t i = 0; i < h->entries_cnt; i++)
	{
		switch (h->entries[i].type)
		{
			case swcp_type_u8:
			case swcp_type_i8:
				h->entries[i].size = 1;
				break;
			case swcp_type_u16:
			case swcp_type_i16:
				h->entries[i].size = 2;
				break;
			case swcp_type_u32:
			case swcp_type_i32:
				h->entries[i].size = 4;
				break;
			case swcp_type_arr:
			default:
			break;
		}
	}

	return I_OK;
}


i_status swcp_load(swcp_t* h)
{
	i_status r = I_OK;
	i_status init = I_OK;

	uint32_t mem_curr = h->mem_start+2;

	if(h->load(h->mem_start, rx_buffer, 2) != I_OK)
	{
		return I_FAILED;
	}

	uint16_t params_cnt = (((uint16_t)rx_buffer[0]) << 8) | rx_buffer[1];
	if(params_cnt != h->entries_cnt)
	{
		init = I_INVALID;
	}


	for (size_t i = 0; i < h->entries_cnt; i++)
	{
		switch (h->entries[i].type)
		{
			case swcp_type_u8:
			case swcp_type_i8:
				if(init == I_INVALID)
				{
					memmove(h->entries[i].pvar, h->entries[i].pdvar, 1);
				}
				else
				{
					if(h->load(mem_curr, rx_buffer, 3) != I_OK)
						return I_FAILED;
					uint8_t c8 = checksum_8(&rx_buffer[1], 2);
					if ((c8 != rx_buffer[0]) || (memcmp(&rx_buffer[1], &rx_buffer[2], 1) != 0))
					{
						i_status re = h->error(&h->entries[i]);
						if (re != I_OK)
							return I_FAILED;
						r = I_ERROR;
						memmove(h->entries[i].pvar, h->entries[i].pdvar, 1);
					}
					else
					{
						memmove(h->entries[i].pvar, &rx_buffer[1], 1);
					}
					mem_curr += 3;
				}
				break;
			case swcp_type_u16:
			case swcp_type_i16:
				if(init == I_INVALID)
				{
					memmove(h->entries[i].pvar, h->entries[i].pdvar, 2);
				}
				else
				{
					if(h->load(mem_curr, rx_buffer, 5)!= I_OK)
						return I_FAILED;
					uint8_t c16 = checksum_8(&rx_buffer[1], 4);
					if ((c16 != rx_buffer[0]) || (memcmp(&rx_buffer[1], &rx_buffer[3], 2) != 0))
					{
						i_status re = h->error(&h->entries[i]);
						if (re != I_OK)
							return I_FAILED;
						r = I_ERROR;
						memmove(h->entries[i].pvar, h->entries[i].pdvar, 2);
					}
					else
					{
						memmove(h->entries[i].pvar, &rx_buffer[1], 2);
						(*(uint16_t*)h->entries[i].pvar) = rev_u16(*(uint16_t*)h->entries[i].pvar);
					}
					mem_curr += 5;
				}
				break;
			case swcp_type_u32:
			case swcp_type_i32:
				if(init == I_INVALID)
				{
					memmove(h->entries[i].pvar, h->entries[i].pdvar, 4);
				}
				else
				{
					if(h->load(mem_curr, rx_buffer, 9) != I_OK)
						return I_FAILED;
					uint8_t c32 = checksum_8(&rx_buffer[1], 8);
					if ((c32 != rx_buffer[0]) || (memcmp(&rx_buffer[1], &rx_buffer[5], 4) != 0))
					{
						i_status re = h->error(&h->entries[i]);
						if (re != I_OK)
							return I_FAILED;
						r = I_ERROR;
						memmove(h->entries[i].pvar, h->entries[i].pdvar, 4);
					}
					else
					{
						memmove(h->entries[i].pvar, &rx_buffer[1], 4);
						(*(uint32_t*)h->entries[i].pvar) = rev_u32(*(uint32_t*)h->entries[i].pvar);
					}
					mem_curr += 9;
				}
				break;
			case swcp_type_arr:
				if(init == I_INVALID)
				{
					memmove(h->entries[i].pvar, h->entries[i].pdvar, h->entries[i].size);
				}
				else
				{
					if(h->load(mem_curr, rx_buffer, 1+(h->entries[i].size*2)) != I_OK)
						return I_FAILED;
					uint8_t cr = checksum_8(&rx_buffer[1], (h->entries[i].size * 2));
					if ((cr != rx_buffer[0]) || (memcmp(&rx_buffer[1], &rx_buffer[1+ h->entries[i].size], h->entries[i].size) != 0))
					{
						if (h->error != NULL)
						{
							i_status re = h->error(&h->entries[i]);
							if (re != I_OK)
								return I_FAILED;
						}
						r = I_ERROR;
						memmove(h->entries[i].pvar, h->entries[i].pdvar, h->entries[i].size);
					}
					else
					{
						memmove(h->entries[i].pvar, &rx_buffer[1], h->entries[i].size);
					}
					mem_curr += (1 + (h->entries[i].size * 2));
				}
				break;
		default:
			break;
		}
	}

	return init == I_INVALID ? I_INVALID : r;
}

i_status swcp_sync(swcp_t* h)
{
	i_status r = I_OK;

	uint32_t mem_curr = h->mem_start+2;

	tx_buffer[0] = (h->entries_cnt & 0x0000FF00) >> 8;
	tx_buffer[1] = (h->entries_cnt & 0x000000FF) >> 0;

	if(h->save(h->mem_start, tx_buffer, 2)!=I_OK)
	{
		return I_FAILED;
	}

	for (size_t i = 0; i < h->entries_cnt; i++)
	{
		i_status rs = swcp_sync_entry(h->load, h->save, mem_curr, &h->entries[i]);

		if(rs == I_FAILED)
		{
			rs = swcp_sync_entry(h->load, h->save, mem_curr, &h->entries[i]);
		}
		if(rs == I_FAILED)
			return I_FAILED;

		if (rs != I_IDLE && rs != I_OK && h->error!= NULL)
		{
			i_status re = h->error(&h->entries[i]);
			if (re != I_OK)
				return I_FAILED;
			r = I_ERROR;
		}
		mem_curr += 1+ (2* h->entries[i].size);
	}

	return r;
}

i_status swcp_sync_entry(i_status(*load)(uint32_t, void*, size_t), i_status(*save)(uint32_t, void*, size_t), uint32_t address, swcp_entry_t* entry)
{
	if(load(address, rx_buffer, 1 + (entry->size*2))!=I_OK)
		return I_FAILED;

	uint8_t cr = checksum_8(&rx_buffer[1], (entry->size * 2));
	swcp_tx_buf_gen(entry);
	if ((cr != rx_buffer[0]) || (memcmp(tx_buffer, rx_buffer, 1 + (entry->size * 2)) != 0))
	{
		save(address, tx_buffer, 1 + (entry->size * 2));
		load(address, rx_buffer, 1 + (entry->size*2));
		i_status res = memcmp(tx_buffer,rx_buffer,1 + (entry->size * 2)) == 0 ? I_OK : I_FAILED;
		return res;
	}
	return I_IDLE;
}


i_status swcp_sync_entry_by_pvar(swcp_t* h, uint32_t pvar)
{
	i_status r = I_ERROR;
	uint32_t mem_curr = h->mem_start+2;

	for (size_t i = 0; i < h->entries_cnt; i++)
	{
		if((uint32_t)h->entries[i].pvar == pvar)
		{
			r = swcp_sync_entry(h->load,h->save,mem_curr,&h->entries[i]);
			break;
		}
		mem_curr += 1+ (2* h->entries[i].size);
	}

	if(r == I_IDLE || r == I_OK)
		return I_OK;
	return I_ERROR;
}


/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
#endif
