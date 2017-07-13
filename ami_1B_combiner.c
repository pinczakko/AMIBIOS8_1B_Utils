/*
 * ami_1B_combiner.c 
 *
 * This utility reads an AMIBIOS 1B module, shows information about its components 
 * and write the contents of its components into individual files.
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
	LIST,
	REPLACE_COMPONENT,
} ACTION;

/*
 * Insert 1B component from input file named component_filename to 
 * the 1B file represented by p_1b_data starting at file offset  
 * component_offset
 *
 *  input: 
 *
 *  p_data 		pointer to _1B_DATA_T structure representing the 1B file 
 *
 *  component_offset	offset of the component inside the 1B file
 *
 *  component_filename	name of the component file to be inserted into the 1B file
 *
 *  return value:
 *   SUCCESS 	on success
 *   ERROR	on error
 *   
 */
static STATUS
replace_component(_1B_DATA_T * p_data, off_t component_offset,
		  char *component_filename)
{
	int i;
	off_t component_size = 0;
	_1B_COMPONENT_T *p_comp = NULL;
	STATUS status;

	// Check _1B_DATA_T pointer validity 
	//
	if (p_data == NULL) {
		printf("ERROR: 1B data pointer is not initialized\n");
		return ERROR;
	}
	// Check component_filename validity
	//
	if (component_filename == NULL) {
		printf("ERROR: Invalid component filename\n");
		return ERROR;
	}
	// Check whether component_offset is a valid component offset in the 1B file
	// and get pointer to the component at that file offset
	//
	p_comp = get_component_from_file_offset(p_data, component_offset);
	if (p_comp == NULL) {
		printf("Invalid component offset. "
		       "Matching component not found. \n");
		return ERROR;
	}
	// Replace the contents of the component in 1B file with the contents of the 
	// input component file.
	//
	status =
	    replace_component_data(p_data, p_comp, component_filename);
	if (status == ERROR) {
		printf("ERROR: Unable to replace component data "
		       "with new one\n");
		return ERROR;
	}
	// Write the modified 1B file buffer to its original file
	//
	if (write_1B_data_to_file(p_data, get_1B_filename(p_data)) == 0) {
		printf("Successfully writing modified 1B file\n");
		return SUCCESS;
	} else {
		printf("ERROR: Failed writing modified 1B file\n");
		return ERROR;
	}

}

static void show_help(char *argv[])
{
	printf("Usage:\n"
	       "%s --replace  1B_filename  component_filename  component_offset \n"
	       "%s --list   1B_filename \n\n"
	       "In the first variant, this program will replace the component named component_filename\n"
	       "in the 1B file starting at offset component offset. The program checks \n"
	       "the replaced component size and start offset, if the program found the start offset\n"
	       "is incorrect, it will bail out with error message.\n\n"
	       "In the second variant, this program only lists the components inside the 1B file\n"
	       "along with their information\n", argv[0], argv[0]);
}


int main(int argc, char *argv[])
{
/*
 * Program Invocation:  
 *  	./ami_1B_combiner  --replace  1B_filename  component_filename  component_offset
 *  	./ami_1B_combiner  --list   1B_filename 
 *
 *  In the first variant, this program will replace the component named component_filename 
 *  in the 1B file starting at offset component offset. The program checks the replaced component 
 *  size and start offset, if the program found the start offset is incorrect, it will bail out with 
 *  error message. 
 *  
 *  In the second variant, this program only lists the components inside the 1B file along with 
 *  their information
 *
 */
	int f_out;
	off_t component_offset = 0;
	_1B_DATA_T *p_1b_data;
	ACTION act;
	char path[MAX_PATH];

	if ((argc != 3) && (argc != 5)) {
		show_help(argv);
		return 0;
	}
	// Parse the input parameters here
	//
	if ((argc == 3) && (!strcmp(argv[1], "--list"))) {
#ifdef DEBUG
		printf("argc = 3, --list\n");
#endif
		act = LIST;
	} else if ((argc == 5) && (!strcmp(argv[1], "--replace"))) {
#ifdef DEBUG
		printf("argc = 5, --replace\n");
#endif
		act = REPLACE_COMPONENT;

		if (sscanf(argv[3], "%s", &path) == EOF) {
			printf("component_filename is incorrect\n");
			return 0;
		}

		if (sscanf(argv[4], "%X", &component_offset) == EOF) {
			printf("component_offset is incorrect\n");
			return 0;
		}

		if (component_offset <= HEADER_INFO_LENGTH) {
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
		printf("ERROR: Not enough memory to create "
		       "1B data structure!\n");
	} else {
		switch (act) {
		case REPLACE_COMPONENT:
			// replace component file to 1B and write the result to the 1B file 
			//
			replace_component(p_1b_data, component_offset,
					  path);
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
