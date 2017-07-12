/*
 * ami_1B.h 
 *
 * External include file for utilities that use AMIBIOS 1B library function
 *
 * Copyright (C) 2010 Darmawan Salihun <darmawan_salihun@yahoo.com> 
 *
 */

#ifndef __AMI_1B_H__
#define __AMI_1B_H__

#include <sys/types.h> /** Required for off_t type */

#define DEBUG
#undef DEBUG

#define MAX_PATH 	1024	// maximum length of 1B filename

#define MAX_COMPONENT_NAME 		200	// maximum length of component's string

#define COMPONENT_PRESENT_BITMASK 	0x80000000	// bit that indicates the component
							// is present in 1B file

#define MAX_COMPONENT	400	// maximum number of component which can be handled
			     // (just to simplify memory management)


// Note: All offsets are calculated from the start of the file
//
typedef enum {
	COMPONENT_COUNT_OFFSET = 0,	// number of components in the 1B module 

	HEADER_LENGTH_OFFSET = 2,	// size of the 1B header

	HEADER_CONTENTS_OFFSET = 4,	// start of header contents (components data -- 
	// runtime physical address start and size) 
} _1B_OFFSETS;

// Important lengths in bytes
//
typedef enum {
	HEADER_INFO_LENGTH = 4,	// sizeof(COMPONENT_INFO_COUNT + HEADER_LENGTH/HEADER_SIZE)

	COMPONENT_INFO_LENGTH = 8,	// length of each component entry in the header

} _1B_LENGHTS;

// Function return value code
//
typedef enum {
	ERROR = -1,
	SUCCESS = 0,
} STATUS;

// Header string presence flag
//
typedef enum {
	STRING_ABSENT = 0,
	STRING_PRESENT = 1,
} STRING_PRESENCE;

// Component presence flag
//
typedef enum {
	DATA_ABSENT = 0,
	DATA_PRESENT = 1,
} COMPONENT_DATA_PRESENCE;

// Data types
//
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;

typedef signed char s8_t;
typedef signed short s16_t;
typedef signed int s32_t;

// Hidden structures
//
struct _1B_HEADER_S;
struct _1B_COMPONENT_S;
struct _1B_DATA_S;

typedef struct _1B_HEADER_S _1B_HEADER_T;
typedef struct _1B_COMPONENT_S _1B_COMPONENT_T;
typedef struct _1B_DATA_S _1B_DATA_T;

// Exported functions
//
_1B_DATA_T *init_1B_data(const char *in_filename);

void cleanup_1B_data(_1B_DATA_T * p_data);

STATUS write_1B_data_to_file(_1B_DATA_T * p_data, const char *filename);

const char *get_1B_filename(_1B_DATA_T * p_data);

STATUS list_components(_1B_DATA_T * p_data);

// NOTE: Component count starts from 1 (even if component position starts from 0)
u16_t get_component_count(_1B_DATA_T * p_data);

_1B_COMPONENT_T *get_component_from_file_offset(_1B_DATA_T * p_data,
						off_t file_offset);

// NOTE: Component position starts from 0
_1B_COMPONENT_T *get_component_from_position(_1B_DATA_T * p_data,
					     u16_t position);

STATUS write_component_data_to_file(_1B_COMPONENT_T * p_component);

STATUS replace_component_data(_1B_DATA_T * p_data,
			      _1B_COMPONENT_T * p_component,
			      const char *filename);

COMPONENT_DATA_PRESENCE is_component_data_present(_1B_COMPONENT_T *
						  p_component);

#endif				//__AMI_1B_H__
