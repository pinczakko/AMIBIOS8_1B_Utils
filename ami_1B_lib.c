/*
 * ami_1B_lib.c 
 *
 * Utility functions for AMIBIOS 1B file manipulation 
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

#include "ami_1B_internal.h"

static STATUS update_header_data(_1B_DATA_T * p_data)
{
	u16_t i;
	u32_t len, offset;

	// Input parameter sanity check 
	//
	if (p_data == NULL) {
		printf("ERROR: %s() p_data input parameter is empty\n",
		       __func__);
		return ERROR;
	}
	// Check  header buffer validity
	//
	if (p_data->header.p_buf == NULL) {
		printf("ERROR: %s() header buffer is empty\n", __func__);
		return ERROR;
	}
	// Check the contents of the components members against the data 
	// stored in the header buffer
	//
	offset = HEADER_CONTENTS_OFFSET + 4;
	for (i = 0; i < p_data->header.component_info_count; i++) {
		len = *((u32_t *) (p_data->header.p_buf + offset));

		if (p_data->component[i].data_presence == DATA_PRESENT) 
			len |= COMPONENT_PRESENT_BITMASK;	

		if ((len != p_data->component[i].length) &&  
		    (p_data->component[i].data_presence == DATA_PRESENT)){
			len = p_data->component[i].length;
			len |= COMPONENT_PRESENT_BITMASK;
			*((u32_t *) (p_data->header.p_buf + offset)) = len;
		}

		offset += COMPONENT_INFO_LENGTH;
	}

	return SUCCESS;
}


/*
 * Write the contents of buffer p_buf to __an already opened__ file f_out.
 *
 * NOTE: This function should not alter the file pointer and 
 *       file pointer position in anyway, including it must not close the file. 
 *
 * input: 
 *      f_out	pointer to an already opened file, ready for writing
 *      p_buf	pointer to buffer to be written
 *      len	length of p_buf in bytes
 *      mode    the write mode 	to be used
 *
 *  return value: 
 *      ERROR 	on error
 *      SUCCESS on success
 */
static STATUS write_buffer_to_file(FILE * f_out, void *p_buf,
				   const u32_t len)
{
	size_t processed_size, written_size;

	// Input parameters sanity check
	//
	if ((f_out == NULL) || (p_buf == NULL) || (len == 0)) {
		printf("ERROR: function %s() invalid input parameter(s)\n",
		       __func__);
		return ERROR;
	}

	processed_size = 0;
	while (processed_size < len) {
		written_size = fwrite(p_buf, sizeof(u8_t),
				      len - processed_size, f_out);
		if (ferror(f_out))
			return ERROR;

		processed_size += written_size;
		p_buf += written_size;
	}
	return SUCCESS;
}


STATUS write_1B_data_to_file(_1B_DATA_T * p_data, const char *filename)
{
	u16_t i = 0;
	u32_t len = 0;
	FILE *f_out = NULL;
	void *p_buf = NULL;

	// Sanity check on the input parameters 
	if ((p_data == NULL) || (filename == NULL)) {
		printf("ERROR: function %s() "
		       "invalid p_data/filename pointer\n", __func__);
		return ERROR;
	}
	// Sanity check on 1B file size
	if ((p_data->calculated_size == 0) || (p_data->size == 0)) {
		printf("ERROR: function %s() p_data contents is empty\n",
		       __func__);
		return ERROR;
	}

	f_out = fopen(filename, "wb");
	if (f_out == NULL) {
		printf("ERROR: function %s() unable to open output file "
		       "for writing\n", __func__);
		return ERROR;
	}


	printf("%s: Writing 1B binary data to %s ..\n", __func__,
	       filename);

	// Write header to output file
	p_buf = p_data->header.p_buf;
	len = p_data->header.length;
	printf("Writing header to file %s\n", filename);
	if (write_buffer_to_file(f_out, p_buf, len) == ERROR) {
		printf("%s: Error writing header to output file %s\n",
		       __func__, filename);
		fclose(f_out);
		return ERROR;
	}
	// Write components data to output file
	for (i = 0; i < p_data->header.component_info_count; i++) {
		if (p_data->component[i].data_presence == DATA_PRESENT) {
			p_buf = p_data->component[i].p_buf;
			len = p_data->component[i].length;

			if (write_buffer_to_file(f_out, p_buf, len) ==
			    ERROR) {
				printf("%s: Error writing component "
				       "[0x%02X] to output file %s\n",
				       __func__, i, filename);
				fclose(f_out);
				return ERROR;
			}
			
			printf("Writing component[%02Xh] of length %Xh "
				"to file %s\n", i, len, filename);
		}

	}
	fclose(f_out);
	return SUCCESS;
}

const char *get_1B_filename(_1B_DATA_T * p_data)
{
	if (p_data == NULL) {
		printf("ERROR: function %s() Empty 1B data structure\n",
		       __func__);
		return NULL;
	}

	return p_data->filename;
}


u16_t get_component_count(_1B_DATA_T * p_data)
{
	if (p_data == NULL) {
		printf("ERROR: function %s() Empty 1B data structure\n",
		       __func__);
		return 0;
	}

	return p_data->header.component_info_count;
}


_1B_COMPONENT_T *get_component_from_file_offset(_1B_DATA_T *
						p_data, off_t file_offset)
{
	u16_t i;

	if (p_data == NULL) {
		printf("ERROR: function %s() Invalid p_data pointer \n",
		       __func__);
		return NULL;
	}
#ifdef DEBUG
	printf("%s: file_offset = 0x%X\n", __func__, file_offset);
#endif
	for (i = 0; i < p_data->header.component_info_count; i++) {
		if (p_data->component[i].file_offset == file_offset) {
#ifdef DEBUG
			printf("%s: component found at i=0x%X\n",
			       __func__, i);
#endif
			return &(p_data->component[i]);
		}
	}
	return NULL;
}


_1B_COMPONENT_T *get_component_from_position(_1B_DATA_T * p_data,
					     u16_t position)
{
	if (p_data == NULL) {
		printf("ERROR: function %s() Invalid p_data pointer\n",
		       __func__);
		return NULL;
	}

	return &(p_data->component[position]);
}


COMPONENT_DATA_PRESENCE
is_component_data_present(_1B_COMPONENT_T * p_component)
{
	if (p_component == NULL) {
		printf("ERROR: Invalid p_component pointer\n");
		return DATA_ABSENT;
	}

	return p_component->data_presence;
}

/*
 * Write contents of the component data buffer to output file with the same name 
 * as the component name/string
 *
 * input: 
 * 	p_component	pointer to the component contains the data to be written
 * 	filename	the name of the file to write into
 *
 * return value: 
 * 	ERROR 	on error
 * 	SUCCESS	on success		
 */
STATUS write_component_data_to_file(_1B_COMPONENT_T * p_component)
{
	void *p_buf = NULL;
	u32_t len = 0;
	FILE *f_out = NULL;

	// Input buffer sanity check
	//
	if (p_component == NULL) {
		printf("ERROR: Input component is empty\n");
		return ERROR;
	}

	if ((p_component->p_buf == NULL) || (p_component->length == 0)) {
		printf("ERROR: Input buffer is empty\n");
		return ERROR;
	}

	if (strlen(p_component->name) <= 0) {
		printf("ERROR: Invalid component name\n");
		return ERROR;
	}
	// Open output file and truncate it
	f_out = fopen(p_component->name, "wb");
	if (f_out == NULL) {
		printf("ERROR: Unable to create output file"
		       " for writing\n");
		return ERROR;
	}
	// Write the output buffer to the output file
	printf("%s: Writing component data to %s ..\n",
	       __func__, p_component->name);

	// Write header to output file
	p_buf = p_component->p_buf;
	len = p_component->length;
	if (write_buffer_to_file(f_out, p_buf, len) == ERROR) {
		printf("%s: Error writing to output file %s\n",
		       __func__, p_component->name);
		fclose(f_out);
		return ERROR;
	} else {
		fclose(f_out);
		return SUCCESS;
	}

}


/*
 * Read the contents of the file starting at start_offset to internally 
 * allocated buffer of size size. 
 *
 * NOTE: Caller of this function must free the buffer by calling 
 * cleanup_file_chunk_buffer() when done using the file buffer.
 *
 * input: 
 * 	filename	the name of the file to be read
 * 	start_offset	file offset to start reading file contents 
 * 	                (chunk start offset)
 * 	size		size of the chunk of file to be read
 *
 * return value: 
 * 	NULL 				on error
 * 	pointer_to_allocated_buffer  	on success		
 */
static void *init_file_chunk_buffer(const char *filename,
				    off_t start_offset, size_t size)
{
	void *p_chunk = NULL;
	FILE *f_in = NULL;
	struct stat f_stat;
	size_t processed_size, read_size;

	if (stat(filename, &f_stat) != 0) {
		printf("ERROR: unable to get input file statistics\n");
		return NULL;
	}

	if ((start_offset + size) > f_stat.st_size) {
		printf("ERROR: Requested file chunk offset/size "
		       "is out of range\n");
		return NULL;
	}

	f_in = fopen(filename, "rb");
	if (f_in == NULL) {
		printf("ERROR: Unable to open input file\n");
		return NULL;
	}

	if (fseek(f_in, start_offset, SEEK_SET) != 0) {
		printf("ERROR seeking the input file\n");
		fclose(f_in);
		return NULL;
	}
	// init output buffer
	p_chunk = (void *) malloc(size);
	if (p_chunk == NULL) {
		printf("ERROR: Unable to allocate memory for file chunk."
		       " Exiting..\n");
		fclose(f_in);
		return NULL;
	}
#ifdef DEBUG
	printf("File chunk size = 0x%X\n", size);
#endif
	// Read the requested range to the buffer 
	//
	processed_size = 0;
	while (processed_size < size) {
		read_size = fread(p_chunk, sizeof(u8_t),
				  size - processed_size, f_in);

		if (ferror(f_in)) {
			printf("ERROR reading from input file\n");
			free(p_chunk);
			fclose(f_in);
			return NULL;
		} else {
			processed_size += read_size;
		}
	}
	fclose(f_in);
	return p_chunk;
}


/*
 * Cleanup file chunk buffer
 *
 * input: 
 * 	p_chunk pointer to allocated file chunk buffer
 * 
 */
static void cleanup_file_chunk_buffer(void *p_chunk)
{
	if (p_chunk != NULL) {
		free(p_chunk);
	}
}

/*
 * Replace the component's data with data read from the input file filename
 *
 * input: 
 *  p_data		pointer to the 1B data structure 
 *  p_component		pointer to the component with data to be replaced
 *  filename		filename of the file which contains the new data
 *
 *  return value:
 *  ERROR	on error
 *  SUCCESS	on success
 */
STATUS replace_component_data(_1B_DATA_T * p_data,
			      _1B_COMPONENT_T * p_component,
			      const char *filename)
{
	s32_t delta = 0;
	u32_t old_len = 0, new_len = 0, i = 0;
	struct stat f_stat;
	void *p_buf = NULL;

	// Sanity check on input parameters 
	//
	if ((p_component == NULL) || (filename == NULL)) {
		printf("ERROR: %s() invalid input parameter\n", __func__);
		return ERROR;
	}
	// Make sure the replaced component is present in the 1B file
	//
	if ((p_component->data_presence != DATA_PRESENT) ||
	    (p_component->p_buf == NULL)) {
		printf("ERROR: %s() component data not present\n",
		       __func__);
		return ERROR;
	}
	// Save current component data length 
	//
	old_len = p_component->length;

	// Read input file to buffer
	//
	if (stat(filename, &f_stat) != 0) {
		printf("ERROR: %s() unable to get new component file "
		       "statistics\n", __func__);
		return ERROR;
	}

	new_len = f_stat.st_size;
	p_buf = init_file_chunk_buffer(filename, 0, new_len);
	if (p_buf == NULL) {
		printf("ERROR: %s() unable to allocate buffer for the "
		       " new component file\n", __func__);
		return ERROR;

	}
	// Delete old data buffer
	//
	cleanup_file_chunk_buffer(p_component->p_buf);

	// Compare old data buffer size to new data buffer size
	// Display warning message if they don't match
	// 
	if (new_len != old_len) {
		printf("Warning: The new component have different size"
		       " with the current component.\n");
	}
	// Assign new data buffer to the component
	//
	p_component->p_buf = p_buf;
	p_component->length = new_len;
	// Update header data and p_data size-related members 
	// to reflect the change.
	return update_header_data(p_data);
}


/*
 * Check thoroughly whether component string is present. 
 * The check is a comprehensive check, instead of simple check
 */
static STRING_PRESENCE
get_component_string_start(void *p_header_buf,
			   const u16_t header_len,
			   const u16_t
			   component_info_count,
			   u16_t * p_offset, u8_t * p_pad_length)
{
	u16_t offset = 0;

	if ((p_header_buf == NULL) || (header_len == 0) ||
	    (component_info_count == 0) || (p_offset == NULL)
	    || (p_pad_length == NULL)) {
		printf("ERROR: Invalid input parameter for %s\n",
		       __func__);
		return STRING_ABSENT;
	}

	offset = component_info_count * COMPONENT_INFO_LENGTH +
	    HEADER_INFO_LENGTH;

	if (offset < header_len) {
		// Check the header version and  set component string pad size 
		// to the right value (header newer than 4.00 uses 4 bytes pad, 
		// version 4.00 uses 5 bytes pad)
		//
		offset = component_info_count * COMPONENT_INFO_LENGTH +
		    HEADER_INFO_LENGTH + 2;

		if (!strncmp((char *) (p_header_buf + offset), "00", 2))
			*p_pad_length = 5;
		else
			*p_pad_length = 4;


		// Find the start of component string in the header 
		// by looking for "RUN_" string which is part of the 
		// first component name (RUN_CSEG)
		//
		offset = component_info_count * COMPONENT_INFO_LENGTH +
		    HEADER_INFO_LENGTH + 4;

		while (offset < header_len) {
			if (!strncmp((char *) (p_header_buf + offset),
				     "RUN_", 4))
				break;
			else
				offset++;
		}

		// Handle unwanted cases where the component string component 
		// is faulty or empty
		if (offset < header_len) {
			*p_offset = offset;
			return STRING_PRESENT;
		}
	} else {
		*p_pad_length = 0;
		return STRING_ABSENT;
	}

}

/*
 * Parse the 1B header contents. Fill the _1B_DATA_T object and its associated
 * components objects with correct data
 *
 * NOTE: p_data->header.p_buf must contain the header of the 1B file prior to 
 * calling this function and header_len must contain the length of the buffer.
 *
 * input: 
 * 	p_data			pointer to allocated 1B data structure
 *      header_len 		length of the header buffer in bytes
 *      component_info_count 	number of component info in the header
 *
 * returns: 
 * 	ERROR	on error 
 * 	SUCCESS	on success	 
 */
static STATUS
parse_header(_1B_DATA_T * p_data, const u16_t header_len,
	     const u16_t component_info_count)
{
	u32_t i, file_offset, t;
	u16_t info_offset, string_offset, string_len;
	u8_t pad_length;
	char *p_str = NULL;

	if ((p_data == NULL) || (p_data->header.p_buf == NULL) ||
	    (header_len == 0) || (component_info_count == 0)) {
		printf("ERROR: function %s() Invalid "
		       "input parameter(s)\n", __func__);
		return ERROR;
	}

	if (component_info_count > MAX_COMPONENT) {
		printf("ERROR: function %s() maximum supported "
		       "number of 1B components exceeded\n", __func__);
		return ERROR;
	}

	p_data->header.length = header_len;
	p_data->header.component_info_count = component_info_count;

	// initialize component name buffer
	//
	for (i = 0; i < component_info_count; i++) {
		memset(p_data->component[i].name, '\0',
		       sizeof(p_data->component[i].name));
	}

	// Check whether the header contains component string or not
	//
	if (get_component_string_start(p_data->header.p_buf,
				       header_len,
				       component_info_count,
				       &string_offset,
				       &pad_length) == STRING_PRESENT) {
		p_data->header.string_status = STRING_PRESENT;
		p_data->header.string_pad_length = pad_length;
	} else {
		p_data->header.string_status = STRING_ABSENT;
		p_data->header.string_pad_length = 0;
	}

	// Parse the components data
	//
	info_offset = HEADER_CONTENTS_OFFSET;
	file_offset = header_len;

	for (i = 0; i < component_info_count; i++) {
		t = *((u32_t *) (p_data->header.p_buf + info_offset));
		p_data->component[i].physical_address = t;
#ifdef DEBUG
		printf("1B component: Target physical address: 0x%X , ",
		       p_data->component[i].physical_address);
#endif
		t = *((u32_t *) (p_data->header.p_buf + info_offset + 4));
		p_data->component[i].length = t;

		info_offset += COMPONENT_INFO_LENGTH;

		if (p_data->header.string_status == STRING_PRESENT) {
			p_str = (char *) (p_data->header.p_buf +
					  string_offset);
			string_len = strlen(p_str);
			snprintf(p_data->component[i].name, string_len + 1,
				 "%s", p_str);
			p_str = NULL;

			string_offset = string_offset + string_len +
			    p_data->header.string_pad_length;

		} else {
			string_len = strlen("_1B_component_XXh");
			snprintf(p_data->component[i].name, string_len + 1,
				 "_1B_component_%02Xh", i);
		}
#ifdef DEBUG
		printf("Name: %s, ", p_data->component[i].name);
#endif

		if (p_data->component[i].length &
		    COMPONENT_PRESENT_BITMASK) {
			p_data->component[i].data_presence = DATA_PRESENT;
#ifdef DEBUG
			printf("Present in 1B, ");
#endif
			t = p_data->component[i].length;
			t &= ~COMPONENT_PRESENT_BITMASK;
			p_data->component[i].length = t;

			p_data->component[i].file_offset = file_offset;

			file_offset += p_data->component[i].length;

#ifdef DEBUG
			printf("Offset: 0x%X, ",
			       p_data->component[i].file_offset);
#endif
		} else {
			p_data->component[i].data_presence = DATA_ABSENT;
			p_data->component[i].file_offset = 0;
		}
#ifdef DEBUG
		printf("Length: 0x%X", p_data->component[i].length);
		printf("\n");
#endif
	}

	p_data->calculated_size = file_offset;

	return SUCCESS;
}

/* 
 * Read the header information required to create header buffer
 *
 * input: 
 *	filename 	name of the 1B file
 *
 * output:
 *	p_header_size		pointer to size of the header
 *	p_component_info_count 	pointer to number of component info in the header
 *
 * return value: 
 *	ERROR		on error
 * 	SUCCESS		on success
 */
static STATUS
get_header_info(const char *filename, u16_t * p_header_len,
		u16_t * p_component_info_count)
{
	void *p_buf = NULL;
	u16_t hdr_len = 0;
	u16_t component_cnt = 0;

	if ((filename == NULL) || (p_header_len == NULL)
	    || (p_component_info_count == NULL)) {
		printf("ERROR: invalid input parameter\n");
		return ERROR;
	}

	p_buf = init_file_chunk_buffer(filename, 0, HEADER_INFO_LENGTH);
	if (p_buf == NULL) {
		printf("ERROR: unable to read header info\n");
		return ERROR;
	}
	// Read header information from the input file
	//
	component_cnt = *((u16_t *) (p_buf + COMPONENT_COUNT_OFFSET));
	hdr_len = *((u16_t *) (p_buf + HEADER_LENGTH_OFFSET));

#ifdef DEBUG
	printf("component_info_count = 0x%02X\n", component_cnt);
	printf("header_length = 0x%02X\n", hdr_len);
#endif

	// Sanity check on the header info value
	//
	if ((hdr_len == 0) || (component_cnt == 0)) {
		printf("ERROR: Invalid header info\n");
		cleanup_file_chunk_buffer(p_buf);
		return ERROR;
	} else {
		*p_component_info_count = component_cnt;
		*p_header_len = hdr_len;

		cleanup_file_chunk_buffer(p_buf);
		return SUCCESS;
	}
}

/*
 * Initialize data structures describing the 1B components.
 * 
 * NOTE: You must call cleanup cleanup_1B_data() when you're finished using 
 * 	 the dynamic data structures created by this function.
 *
 * input: 
 * 	filename	1B filename string
 *
 * returns: 
 * 	NULL	on error 
 * 	Pointer to initialized _1B_DATA_T on success	 
 */
_1B_DATA_T *init_1B_data(const char *filename)
{
	_1B_DATA_T *p_data = NULL;
	u16_t hdr_len = 0;
	u16_t component_cnt = 0;
	u32_t i;
	struct stat f_stat;

	if (stat(filename, &f_stat) != 0) {
		printf("ERROR: unable to get 1B input file statistics\n");
		return NULL;
	}

	if (f_stat.st_size < HEADER_INFO_LENGTH) {
		printf("ERROR: Invalid 1B input file\n");
		return NULL;
	}

	p_data = (_1B_DATA_T *) malloc(sizeof(_1B_DATA_T));
	if (p_data == NULL) {
		printf("ERROR: unable to allocate memory for 1B "
		       "file data\n");
		return NULL;
	}

	if (get_header_info(filename, &hdr_len, &component_cnt) == ERROR) {
		printf("ERROR: unable to get header info from "
		       "the 1B file\n");
		cleanup_1B_data(p_data);
		return NULL;
	}

	p_data->header.p_buf = init_file_chunk_buffer(filename, 0,
						      hdr_len);
	if (p_data->header.p_buf == NULL) {
		printf("ERROR: Unable to read header "
		       "to internal buffer\n");
		cleanup_1B_data(p_data);
		return NULL;
	}


	if (parse_header(p_data, hdr_len, component_cnt) == ERROR) {
		printf("ERROR: Unable to parse header correctly\n");
		cleanup_1B_data(p_data);
		return NULL;
	}
	// Put file statistics in the 1B data structure
	//
	strncpy(p_data->filename, filename, strlen(filename) + 1);
	p_data->size = f_stat.st_size;

	// Read the components data to buffer for components with data present in the 1B file
	//
	for (i = 0; i < component_cnt; i++) {
		if (p_data->component[i].data_presence == DATA_ABSENT) {
			continue;
		} else if (p_data->component[i].data_presence ==
			   DATA_PRESENT) {
			p_data->component[i].p_buf =
			    init_file_chunk_buffer(p_data->filename,
						   p_data->component[i].
						   file_offset,
						   p_data->component[i].
						   length);

			if (p_data->component[i].p_buf == NULL) {
				printf("ERROR: Unable to allocate buffer "
				       "for component[%02Xh]\n", i);
				cleanup_1B_data(p_data);
				return NULL;
			}
		} else {
			printf("ERROR: Unknown component data presence "
			       "status [%02Xh]\n", i);
			cleanup_1B_data(p_data);
			return NULL;
		}
	}
	return p_data;

}


/*
 * Function to cleanup memory used by the _1B_DATA_T structure
 *
 * input: 
 * 	p_data	pointer to data structure to be cleaned up
 */
void cleanup_1B_data(_1B_DATA_T * p_data)
{
	u32_t i;

	if (p_data == NULL)
		return;

	// Cleanup the header file buffer 
	//
	if (p_data->header.p_buf != NULL) {
		cleanup_file_chunk_buffer(p_data->header.p_buf);
	}
	// Cleanup the components file buffer 
	//
	for (i = 0; i < p_data->header.component_info_count; i++) {
		if (p_data->component[i].p_buf != NULL) {
			cleanup_file_chunk_buffer(p_data->
						  component[i].p_buf);
		}
	}

	// Cleanup the _1B_DATA_T structure
	//
	free(p_data);
}

/*
 * List the components in the passed 1B_file content buffer 
 * along with information about the components
 *
 * input: 
 * 	p_data 	pointer to initialized _1B_DATA_T 
 * 	
 * return value: 
 * 	SUCCESS	on success
 * 	ERROR	on failure		
 */
STATUS list_components(_1B_DATA_T * p_data)
{
	u32_t i;

	if (p_data == NULL) {
		printf("ERROR: 1B data structure is NULL\n");
		return ERROR;
	}
	// Display 1B file information
	//
	printf("Name of 1B file: %s\n", p_data->filename);
	printf("Length of 1B header: 0x%X bytes\n", p_data->header.length);
	printf("Number of components (including the non-present): "
	       "0x%X\n", p_data->header.component_info_count);
	printf("Calculated 1B file size: 0x%X\n", p_data->calculated_size);
	printf("1B file size (from fstat): 0x%X\n", p_data->size);

	if (p_data->header.string_status == STRING_PRESENT) {
		printf("Component string exist \n");
	} else {
		printf("Component string doesn't exist \n");
	}

	// Display each component information 
	//
	for (i = 0; i < p_data->header.component_info_count; i++) {
		printf("1B component: ");
		printf("Target physical address: 0x%X, ",
		       p_data->component[i].physical_address);

		if (p_data->header.string_status == STRING_PRESENT) {
			printf("Name: %s, ", p_data->component[i].name);
		}

		if (p_data->component[i].data_presence == DATA_PRESENT) {
			printf("Present in 1B, ");
		}

		printf("File offset: 0x%X, ",
		       p_data->component[i].file_offset);
		printf("Size: 0x%X\n", p_data->component[i].length);

	}

	return SUCCESS;
}
