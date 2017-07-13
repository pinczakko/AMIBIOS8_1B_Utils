/*
 * ami_1B_splitter.c 
 *
 * This utility reads an AMIBIOS 1B module, shows information about its components 
 * and write the contents of its components into individual files or 
 * only writes one of the component into file.
 *
 * Copyright (C) 2010 Darmawan Salihun <darmawan_salihun@yahoo.com> 
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ami_1B.h"

typedef enum {
	EXTRACT_ALL = 0,	// Write all 1B components to individual files
	EXTRACT_ONE = 1,	// Write only one 1B component starting at the passed in offset
	LIST = 2,
} ACTION;


/*
 * Write all of the 1B components data into individual files. 
 * 
 * input: 
 * 	p_data 	pointer to initialized _1B_DATA_T 
 * 	
 * return value: 
 * 	ERROR 	on error
 * 	SUCCESS	on success		
 */
static STATUS write_all_components(_1B_DATA_T * p_data)
{
	u16_t i, component_count;
	_1B_COMPONENT_T *p_comp = NULL;

	if (p_data == NULL) {
		printf("ERROR: input 1B data structure is NULL\n");
		return ERROR;
	}

	component_count = get_component_count(p_data);

	// Write each component to one file (*.bin)
	for (i = 0; i < component_count; i++) {
		p_comp = get_component_from_position(p_data, i);

		if (p_comp == NULL) {
			printf("Empty component at %02Xh\n", i);
			continue;
		}

		if (is_component_data_present(p_comp) == DATA_ABSENT)
			continue;

		if (write_component_data_to_file(p_comp) == ERROR)
			continue;
	}

	return SUCCESS;
}


/*
 * Write one component of the 1B file based on the file offset of the component 
 * 
 * input: 
 * 	p_data 	pointer to initialized _1B_DATA_T 
 * 	
 * return value: 
 * 	ERROR 	on error
 * 	SUCCESS	on success		
 */
static STATUS write_one_component(_1B_DATA_T * p_data, off_t file_offset)
{
	_1B_COMPONENT_T *p_comp = NULL;

	if (p_data == NULL) {
		printf("ERROR: input file data structure is NULL\n");
		return ERROR;
	}

	p_comp = get_component_from_file_offset(p_data, file_offset);
	if (p_comp == NULL) {
		printf("ERROR: Invalid file offset. "
		       "Component not found\n");
		return ERROR;
	}
	// Write the 1B component to an individual file
	//
	return write_component_data_to_file(p_comp);
}


static void show_help(char *argv[])
{
	printf("Usage:\n"
	       "%s --extract-all 1B_filename \n"
	       "%s --extract 	1B_filename  component_offset\n"
	       "%s --list 	1B_filename\n\n"
	       "In the first variant, this program will extract all components into "
	       "individual files.\n\n"
	       "In the second variant, this program will extract only ONE component "
	       "which starts at component_offset in the 1B_file\n\n"
	       "In the third variant, this program only lists the components inside "
	       "the 1B file along with their information\n",
	       argv[0], argv[0], argv[0]);
}

int main(int argc, char *argv[])
{
/*
 * Program Invocation:  
 *  	./ami_1B_splitter --extract-all 1B_filename 
 *  	./ami_1B_splitter --extract 	1B_filename  component_offset
 *  	./ami_1B_splitter --list 	1B_filename 
 *
 *  In the first variant, this program will extract all components into individual files. 
 *
 *  In the second variant, this program will extract only ONE component which starts at 
 *  component_offset in the 1B_file
 *
 *  In the third variant, this program only lists the components inside the 1B file along with 
 *  their information
 *
 */
	off_t component_offset = 0;
	_1B_DATA_T *p_1b_data;
	ACTION act;

	// Parse input parameters
	//
	if ((argc == 3) && (!strcmp(argv[1], "--extract-all"))) {
#ifdef DEBUG
		printf("argc = 3, --extract-all\n");
#endif
		act = EXTRACT_ALL;
	} else if ((argc == 3) && (!strcmp(argv[1], "--list"))) {
#ifdef DEBUG
		printf("argc = 3, --list\n");
#endif
		act = LIST;
	} else if ((argc == 4) && (!strcmp(argv[1], "--extract"))) {
#ifdef DEBUG
		printf("argc = 4, --extract\n");
#endif
		act = EXTRACT_ONE;
		if (sscanf(argv[3], "%lX", &component_offset) == EOF) {
			printf("component_offset is incorrect\n");
			return 0;
		} else if (component_offset <= HEADER_INFO_LENGTH) {
			printf("component_offset is too small. "
			       "Set it to correct value\n");
			return 0;
		}
	} else {
		printf("ERROR: Wrong input parameters!\n");
		show_help(argv);
		return 0;
	}

	// Initialize 1B data structure, parse the input 1B file and 
	// fill the 1B data structure with the result of the parsing. 
	// Then perform the requested action.
	//
	p_1b_data = init_1B_data(argv[2]);
	if (p_1b_data == NULL) {
		printf("ERROR: Not enough memory "
		       "to create 1B data structure!\n");
	} else {
		switch (act) {

		case EXTRACT_ALL:
			// Write all components to individual files
			//
			write_all_components(p_1b_data);
			break;

		case EXTRACT_ONE:
			// Write only one component to file
			//
			write_one_component(p_1b_data, component_offset);
			break;

		case LIST:
			// Display 1B content information
			//
			list_components(p_1b_data);
			break;

		default:
			printf("ERROR: The input parameter parser "
			       "is not working correctly\n");
			break;
		}

		cleanup_1B_data(p_1b_data);
	}

	return 0;
}
