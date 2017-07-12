/*
 * ami_1B_internal.h
 *
 * Internal data structure definition. Should be used only by files which 
 * modify the data structure contents.
 * 
 * Copyright (C) 2010 Darmawan Salihun <darmawan_salihun@yahoo.com> 
 *
 */

#include "ami_1B.h"

struct _1B_HEADER_S {

	u16_t component_info_count;	// number of components info in the header (_including 
	// info for components which don't exist in the 1B binary file_)  

	u16_t length;		// length of the 1B header in bytes

	STRING_PRESENCE string_status;	// flag that indicate whether component string 
	// exists in the 1B (1 = exist; 0 = absent)

	u16_t string_pad_length;	// length of "pad" bytes between component strings 
	// (version 4.00 header has 5 bytes pad, newer ones have 4 bytes pad)

	void *p_buf;		// pointer to buffer which contains the header 

};


struct _1B_COMPONENT_S {

	char name[MAX_COMPONENT_NAME];	// the name of the component (assuming it 
	// won't be more than MAX_COMPONENT_NAME)

	u32_t physical_address;	// target physical address of the component 
	// after being relocated by the BIOS code

	u32_t length;		// length of the component in bytes (whether 
	// the component present in 1B or not, it doesn't matter)

	COMPONENT_DATA_PRESENCE data_presence;	// flag to indicate whether the 
	// component data/content is present in the 1B file

	off_t file_offset;	// offset of the component in the file (if the component is present)

	void *p_buf;		// pointer to buffer that holds the contents of the component

};


struct _1B_DATA_S {

	char filename[MAX_PATH];	// 1B filename      

	off_t size;		// 1B file size obtained from fstat()

	off_t calculated_size;	// 1B file size obtained from parsing the file header

	_1B_HEADER_T header;	// header info and buffer that holds the header data

	_1B_COMPONENT_T component[MAX_COMPONENT];	// components info and buffer that holds 
	// components data
};
