# SWCP LIBRARY 

Library used to save or retrieve values to/from an EEPROM.


## Supported Hardware
- STM32L552ZET6Q (NUCLEO-L55ZE-Q)

## Functions Guide
- `swcp_initialize`: 
- `swcp_load`: load values from the storage to the rx_buffer.
- `swcp_sync`: save values into the storage from the tx_buffer.


## How to use

- [ Step 1 ] Declare your parameters
```C
	int8_t v_i8;
	int8_t v_i8_d;
	uint8_t v_u8;
	uint8_t v_u8_d;
	int16_t v_i16;
	int16_t v_i16_d;
	uint16_t v_u16;
	uint16_t v_u16_d;
	int32_t v_i32;
	int32_t v_i32_d;
	uint32_t v_u32;
	uint32_t v_u32_d;
	uint8_t v_arr[8];
	uint8_t v_arr_d[8];
```

&nbsp;

- [ Step 2 ] Set the entries array: 
	-  `.pvar` : pointer to the variable.
	-  `.pdvar` : pointer to the default-failsafe variable.
	-  `.type` : swcp_type_X.
	-  `.size` : size in bytes.

```C
	swcp_entry_t entries[] = {
		{.pvar = &v_u8, .pdvar = &v_u8_d, .size = 1, .type = swcp_type_u8},
		{.pvar = &v_i8, .pdvar = &v_i8_d, .size = 1, .type = swcp_type_i8},
		{.pvar = &v_u16, .pdvar = &v_u16_d, .size = 2, .type = swcp_type_u16},
		{.pvar = &v_i16, .pdvar = &v_i16_d, .size = 2, .type = swcp_type_i16},
		{.pvar = &v_u32, .pdvar = &v_u32_d, .size = 4, .type = swcp_type_u32},
		{.pvar = &v_i32, .pdvar = &v_i32_d, .size = 4, .type = swcp_type_i32},
		{.pvar = &v_arr, .pdvar = &v_arr_d, .size = 8, .type = swcp_type_arr}
	};
```

&nbsp;

- [ Step 3 ] Create the following function:
  -  `i_status load(uint32_t, void*, size_t)` 
  -  `i_status save(uint32_t, void*, size_t)` 
  -  `i_status error(swcp_entry_t*)` 
```C
	i_status load(uint32_t address, void* buffer, size_t sz)
	{
		memmove(buffer, &eeprom[address], sz);
		return I_OK;
	}
	i_status save(uint32_t address, void* buffer, size_t sz)
	{
		memmove(&eeprom[address], buffer, sz);
		return I_OK;
	}
	i_status error(swcp_entry_t* entry)
	{
		return I_OK;
	}
```

&nbsp;

- [ Step 4 ] Set the parameters handler :
	- `.mem_start` : start position of data to be saved or to be loaded. 
	- `.entries` : array of entries.
	- `.entries_cnt` : number of entries.
	- `.load` : pointer to the "load" function.
	- `.save` : pointer to the "save" function.
	- `.error` : pointer to the "error" function.
```C
	swcp_t h_params =
	{
		.mem_start = 0,
		.entries = entries,
		.entries_cnt = 7,
		.load = load,
		.save = save,
		.error = error
	};
```

&nbsp;

- [ Step 5 ] Initialize the SWCP handler
```C
	swcp_initialize(&h_params);
```

&nbsp;

- [ Step 6 ] Load or Sync(save) values from/to Storage 
```C
	swcp_load(&h_params);
	//
	swcp_sync(&h_params);
```


## Example

```C
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "lib_swcp.h"


// Emulate EEPROM

uint8_t eeprom[64] = { 0 };

i_status load(uint32_t address, void* buffer, size_t sz)
{
	memmove(buffer, &eeprom[address], sz);
	return I_OK;
}

i_status save(uint32_t address, void* buffer, size_t sz)
{
	memmove(&eeprom[address], buffer, sz);
	return I_OK;
}

i_status error(swcp_entry_t* entry)
{
	return I_OK;
}


// Emulate Parameters

int8_t v_i8;
int8_t v_i8_d;
uint8_t v_u8;
uint8_t v_u8_d;
int16_t v_i16;
int16_t v_i16_d;
uint16_t v_u16;
uint16_t v_u16_d;
int32_t v_i32;
int32_t v_i32_d;
uint32_t v_u32;
uint32_t v_u32_d;
uint8_t v_arr[8];
uint8_t v_arr_d[8];

swcp_entry_t entries[] = {
	{.pvar = &v_u8, .pdvar = &v_u8_d, .size = 1, .type = swcp_type_u8},
	{.pvar = &v_i8, .pdvar = &v_i8_d, .size = 1, .type = swcp_type_i8},
	{.pvar = &v_u16, .pdvar = &v_u16_d, .size = 2, .type = swcp_type_u16},
	{.pvar = &v_i16, .pdvar = &v_i16_d, .size = 2, .type = swcp_type_i16},
	{.pvar = &v_u32, .pdvar = &v_u32_d, .size = 4, .type = swcp_type_u32},
	{.pvar = &v_i32, .pdvar = &v_i32_d, .size = 4, .type = swcp_type_i32},
	{.pvar = &v_arr, .pdvar = &v_arr_d, .size = 8, .type = swcp_type_arr}
};

swcp_t h_params =
{
	.mem_start = 0,
	.entries = entries,
	.entries_cnt = 7,
	.load = load,
	.save = save,
	.error = error
};

int main()
{
	swcp_initialize(&h_params);

	swcp_load(&h_params);

	v_i8 = -10;
	v_u8 = 10;
	v_u16 = 650;
	v_i16 = -624;
	v_u32 = 275812;
	v_i32 = -275812;
	for(int i=0;i<8;i++)
		v_arr[i] = i;


	swcp_sync(&h_params);

	v_i8 = 0;
	v_u8 = 0;
	v_u16 = 0;
	v_i16 = 0;
	v_u32 = 0;
	v_i32 = 0;
	memset(v_arr, 0, 8);


	swcp_load(&h_params);
	return 0;
}
```
