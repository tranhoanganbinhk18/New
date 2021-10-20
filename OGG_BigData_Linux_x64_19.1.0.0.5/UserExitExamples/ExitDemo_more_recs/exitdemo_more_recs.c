/**************************************************************************************************
 Copyright (c) 1995, 2013, Oracle and/or its affiliates. All rights reserved. 
 
  Program description:

  User exit example for Windows NT and Unix. This demo will be valid only for capture processes.
  The User Exit is to receive a large record of only 4 columns. The record will consist of the key
  value, a large block of an arbitrary amount up to 4K in size, date of the record input and
  description of the record data. The large column of data should be formatted as a simplistic
  tokenized data set which contains record count value, an id, some number of columns and the data
  within, which can repeat for several varying id types.  The goal is to process this record by
  parsing the token id and its data and create a new target record for a different table. Each
  different id would be a different target table. Therefore one source record will create many
  targets records.
  This example uses the MORE_RECS_TBL table, which can be created and manipulated using the
  demo_more*.sql scripts included with the ER release.

  Source record format:
    SOURCE TABLE:
        1. MORE_RECS_TBL (int, varchar(3000), datetime/timestamp, varchar(100))

  second column input format:
        [NUM_RECS#TBL_ID|NUM_COLS|ACT_LEN:MAX_LEN:VAL|ACT_LEN:MAX_LEN:VAL|...]
            NUM_RECS - Number records in the column
            TBL_ID   - Table ID
            NUM_COLS - Number of columns in the curremt table
            ACT_LEN  - Actual column value length
            MAX_LEN  - Maximun column value length
            VAL      - The column value

  e.g. insert into MORE_RECS_TBL values
      ( 1001,
        '4#1|3|8:8:1001|14:100:XSYS, PUNE, MH|23:29:2008-01-01 00:00:00.000|
           2|4|8:8:1001|7:20:ABC XYZ|23:29:1983-01-01 12:11:54.888|8:100:PUNE, MH|
           3|2|8:8:1001|5:10:C/C++|
           4|3|8:8:1001|2:10:GG|23:29:2007-09-18 00:00:00.000',
        '2008-11-18 11:30:00.000',
        'A test to show the more_recs_ind use'
      );

  Target record format:
      TARGET TABLES:
        1. EMPOFFICEDTL   (int, varchar(30), datetime);
        2. EMPPERSONALDTL (int, varchar(30), datetime/timestamp, varchar(30));
        3. EMPSKILLSDTL   (int, varchar(10));
        4. EMPPROJECTDTL  (int, varchar(10), datetime);

      e.g. (The records will be formed from the above insert)
          insert into EMPOFFICEDTL   values (1001, 'XSYS, PUNE, MH', '2008-01-01 00:00:00.000');
          insert into EMPPERSONALDTL values (1001, 'ABC XYZ', '1983-01-01 12:11:54.888', 'PUNE, MH');
          insert into EMPSKILLSDTL   values (1001, 'C/C++');
          insert into EMPPROJECTDTL  values (1001, 'GG', '2007-09-18 00:00:00.000');

  CALLBACK function exercised:
    GET_OPERATION_TYPE              Used to get IO Types
    GET_COLUMN_VALUE_FROM_INDEX     Used to get column value
    GET_ERROR_INFO                  Used to get error info
    SET_TABLE_NAME                  Used to change the target table name
    SET_RECORD_BUFFER               Used to set the record buffer
    OUTPUT_MESSAGE_TO_REPORT        Used to write messages to report file


--------------------------------------------------------------------------------------------------
Param file options with Extract using the User Exit

      EXTRACT EQALA                                    REPLICAT RQALA

      SOURCEDB GG, USERID XYZ, PASSWORD XYZ            TARGETDB GG, USERID XYZ, PASSWORD XYZ

      CUSEREXIT userexit.dll CUSEREXIT                 ASSUMETARGETDEFS

      DISCARDFILE dirrpt\eqala.dsc, PURGE              DISCARDFILE dirrpt\rqala.dsc, PURGE

      EXTTRAIL dirdat\T1

      TABLE XYZ.MORE_RECS_TBL;                         MAP EMPOFFICEDTL,   TARGET XYZ.EMPOFFICEDTL;
                                                       MAP EMPPERSONALDTL, TARGET XYZ.EMPPERSONALDTL;
                                                       MAP EMPSKILLSDTL,   TARGET XYZ.EMPSKILLSDTL;
                                                       MAP EMPPROJECTDTL,  TARGET XYZ.EMPPROJECTDTL;
---------------------------------------------------------------------------------------------------

***************************************************************************************************/

#include <stdio.h>

#ifdef WIN32
  #define  int8_t     signed char
  #define  int16_t    short
  #define  int32_t    long
  #define  int64_t    __int64
  #define  uint8_t    unsigned char
  #define  uint16_t   unsigned short
  #define  uint32_t   unsigned long
  #define  uint64_t   unsigned __int64
  #include <windows.h>
  #if (_MSC_VER>=1400)
    #pragma warning(disable:4018)
    #pragma warning(disable:4996)
  #endif
#else
  #if defined(__linux__)
    #include <stdint.h>
    #include <stdarg.h>
  #endif

  #if defined(__MVS__)
    #include <stdarg.h>
    #include <varargs.h>
    #include <inttypes.h>
  #endif

  #if !defined(__MVS__) && !defined(__linux__)
     #include <stdarg.h>
  #endif

  #include <string.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <ctype.h>
#endif

#include "usrdecs.h"

#define MAX_NAME_SIZE            100
#define MAX_BUFF_SIZE            1000
#define MAX_VAL_LENGTH           4000
#define DEFAULT_TIMESTAMP_STRING "1900-01-01:00:00:00.000000000 +00:00"

/* User can change this default value accordingly */
const char* DEFAULT_SCHEMA = "DBO.";

/* (id, table_name) pairs*/
const char* ONE            = "EMPOFFICEDTL";
const char* TWO            = "EMPPERSONALDTL";
const char* THREE          = "EMPSKILLSDTL";
const char* FOUR           = "EMPPROJECTDTL";
const char* UNKNOWN        = "NOT FOUND";

/* Schema Name */
char* schema = NULL;

/* ER callback routine */
#ifndef WIN32
void ERCALLBACK(ercallback_function_codes function_code,
                void *buf, short *presult_code);
#else
typedef void (*FPERCALLBACK)(ercallback_function_codes function_code,
                             void *buf, short *presult_code);

HINSTANCE hEXE; /* EXE handle */
FPERCALLBACK fp_ERCallback; /* Callback function pointer */

/***********************************************************************************
*  Open the ER callback function explicitly.
* @param[in] executable_name - Program which invoked this call
* @param[in] function_name   - Which function to call
* @returns   1 if successful else 0
***********************************************************************************/
short open_callback (char *executable_name,
                     char *function_name)
{
    printf ("Opening callback for %s, %s.\n",
            executable_name,
            function_name);

    hEXE = LoadLibrary (executable_name);
    if (hEXE != NULL)
    {
        /* Function should always be exported in uppercase, since
           GetProcAddress converts to uppercase then does a case-sensitive
           search.  Don't use /NOIGNORE (/NOI) linker option. */
        fp_ERCallback = (FPERCALLBACK)GetProcAddress(hEXE, function_name);

        if (!fp_ERCallback)
        {
            FreeLibrary (hEXE);
            return 0;
        }
    }
    else
        return 0;

    return 1;
}
#endif

/***********************************************************************************
  Close the callback function.
***********************************************************************************/
void close_callback (void)
{
#ifdef WIN32
    FreeLibrary (hEXE);
#endif
}

/***********************************************************************************
*  Call the callback function.
* @param[in]      function_code - The CALLBACK function code
* @param[in/out]  buf           - Input for SET function and Output for GET functions
* @param[out]     result-code   - Result of the CALLABCK function
***********************************************************************************/
void call_callback (ercallback_function_codes function_code,
                    void *buf, short *result_code)
{
#ifdef WIN32
    fp_ERCallback (function_code, buf, result_code);
#else
    ERCALLBACK (function_code, buf, result_code);
#endif
}

/***********************************************************************************
*  Output a message to the report file (or console).
* @param[in] msg - Message to print
* @param[in] ... - Variable arguments
***********************************************************************************/
void output_msg (char *msg,...)
{
    short result_code;
    char temp_msg[1000];

    va_list args;

    va_start (args, msg);

    vsprintf (temp_msg, msg, args);

    va_end (args);

    call_callback (OUTPUT_MESSAGE_TO_REPORT, temp_msg, &result_code);
}

/*********************************************************************************************
* Retrieve the column value from the source record
* @param[in]    col_ix      - column index
* @param[out]   column      - Output column
* @returns      result_code - Result of the operation
**********************************************************************************************/
short get_column_value(column_def* column, short col_idx)
{
    short result_code;

    column->source_or_target = EXIT_FN_SOURCE_VAL;
    column->column_value_mode = EXIT_FN_ASCII_FORMAT;
    column->max_value_length = MAX_VAL_LENGTH;
    column->column_index = col_idx;
    column->requesting_before_after_ind = AFTER_IMAGE_VAL;

    call_callback (GET_COLUMN_VALUE_FROM_INDEX, column, &result_code);
    return result_code;
}

/*********************************************************************************************
* Read the data from the buffer upto the delimeter
* @param[in]    data_val - Input Buffer
* @param[in]    delim    - Delimeter
* @param[out]   buf      - Output Data buffer
**********************************************************************************************/
int read_data(char* data_val, char* buf, char delim)
{
    int count = 0;
    while (*data_val && *data_val != delim)
        buf[count++] = *data_val++;
    return count;
}

/*********************************************************************************************
* Populate the column and column metadata
* @param[in]    table_id   - Table ID to lookup table name
* @param[out]   table_name - Retrieved table name
**********************************************************************************************/
void get_table_name (int64_t table_id, char* table_name)
{
    env_value_def env_value;
    short result_code;
    int   schema_len;
    char* ptr;

    if (NULL == schema)
    {
        /* Get table name */
        memset (&env_value, 0, sizeof(env_value_def));
        env_value.source_or_target = EXIT_FN_SOURCE_VAL;
        env_value.buffer = (char *)malloc(MAX_NAME_SIZE);
        env_value.max_length = MAX_NAME_SIZE;

        call_callback (GET_TABLE_NAME, &env_value, &result_code);
        if (result_code != EXIT_FN_RET_OK)
        {
            output_msg ("Error (%hd) retrieving table name.\n", result_code);
            free (env_value.buffer);
            strcpy (table_name, UNKNOWN);
            return;
        }

        /* 
         *  find table name part:     [CATALOG.]OWNER.TABLE
         */
        ptr = strrchr (env_value.buffer, '.');
        schema = (char*) malloc (MAX_NAME_SIZE);
        if (ptr)
        {
            /* copy [CATALOG.]OWNER. (including dot) */
            schema_len = (int)(ptr - env_value.buffer + 1);
            memcpy(schema, env_value.buffer, schema_len);
            schema[schema_len] = 0;
        }
        else
            strcpy (schema, DEFAULT_SCHEMA);
        free (env_value.buffer);
    }

    strcpy (table_name, schema);
    switch(table_id)
    {
    case 1:
        strcat (table_name, ONE);
        break;

    case 2:
        strcat (table_name, TWO);
        break;

    case 3:
        strcat (table_name, THREE);
        break;

    case 4:
        strcat (table_name, FOUR);
        break;

    default:
        strcpy (table_name, UNKNOWN);
    }
}

/**********************************************************************************************
* Converts the numeric value pressented in string
* @param[in]    str   - Char pointer to string containing numeric value
* @param[in]    count - Total character count in the input string
* @return       value - The numeric value
* Note: Supports null terminated string only
**********************************************************************************************/
int64_t get_numeric_val(char* str, short count)
{
    char* ptr = str;
    short digits = 0;
    int64_t value = 0;

    while (count--)
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            if (digits++)
                value *= 10;
            value += (*ptr - '0');
        }
        ptr++;
    }

    return value;
}

/*********************************************************************************************
* Preapre a New Record Buffer
* @param[out]   out_ptr  - Char pointer to char pointer
* @param[in]    column   - Input column
* @param[in]    col_meta - Input column metadata
* @param[out]    errmsg   - Char pointer to the buffer to hold the error message if any
**********************************************************************************************/
void prepare_record (char** out_ptr, column_def* column,col_metadata_def* col_meta, char* errmsg)
{
    char* out = *out_ptr;
    short null = 0;
    short field_id = column->column_index;
    size_t field_len = 0;
    short out_len;
    float  float_value;
    double double_value;
    int64_t long_value;
    int32_t binary_field_len = column->actual_value_length; /*Input: Column data length.*/

    memcpy (out, &field_id, sizeof(field_id));
    out += sizeof(field_id) + sizeof(uint16_t); /* leave room for field length */

    output_msg("UserExit:(prepare_record) field_id = %d\n",field_id);
    output_msg("UserExit:(prepare_record) column value = %s\n", column->column_value);

    if (!binary_field_len)
        null = -1;

    /* write null indicator if column is null */
    memcpy (out, &null, sizeof(null));
    out += sizeof(null);


    switch (col_meta->gg_data_type)
    {
    case SQLDT_DOUBLE:
        if (null)
            memset (out, 0, binary_field_len);
        else
        {
            if (binary_field_len == sizeof(float))
            {
                memcpy(&float_value, column->column_value, binary_field_len);
                double_value = float_value;
                binary_field_len = sizeof(double_value);
            }
            else if (binary_field_len == sizeof(double))
            {
                memcpy(&double_value, column->column_value, binary_field_len);
            }
            memcpy (out, &double_value, binary_field_len);
        }

        out += binary_field_len;
        field_len = binary_field_len;
        break;

    case SQLDT_64BIT_S:
        if (null)
            memset (out, 0, binary_field_len);
        else
        {
            long_value = get_numeric_val(column->column_value, strlen(column->column_value));
            memcpy (out, &long_value, sizeof(long_value));
        }

        out += binary_field_len;
        field_len = binary_field_len;
        break;

    case SQLDT_ASCII_V:
        if (null)
        {
            field_len = 0;
            memcpy(out, &field_len, sizeof(field_len));
            out += sizeof(uint16_t);
            field_len += sizeof(uint16_t);
        }
        else
#ifndef ORACLE
        {
            /* Code for DB2, Sybase, SQL Server*/
            field_len = binary_field_len;
            memcpy(out, &field_len, sizeof(field_len));
            out += sizeof(uint16_t);
            memcpy (out, column->column_value, binary_field_len);
            out += binary_field_len;
            field_len += sizeof(uint16_t);
        }
#else
        {
            field_len = strlen(column->column_value);
            memcpy (out, &field_len, sizeof(field_len));
            out += sizeof(uint16_t);
            memcpy (out, column->column_value, field_len);
            out += field_len;
            field_len += sizeof(uint16_t);
        }
#endif
        break;

    case SQLDT_DATETIME:
        /* Build the date string using the default date. */
        memcpy (out, DEFAULT_TIMESTAMP_STRING, column->max_value_length);
        if (!null)
        {
            memcpy (out, column->column_value, binary_field_len);
        }
        field_len = column->max_value_length;
        out += column->max_value_length;
        break;

    case SQLDT_ASCII_F:
        if (null)
            memset (out, ' ', binary_field_len);
        else
            memcpy (out, column->column_value, binary_field_len);

        field_len = binary_field_len;
        out += field_len;

        break;

    default: /* programming error */
        sprintf ( errmsg, "Can not prepare record for column %s (datatype :%d)\n"
                  , col_meta->column_name, col_meta->gg_data_type);
        return;
    }

    /* Add to bytes of NULL indicator to field length if column is NULL */
    field_len += sizeof(null);
    out_len = (short)field_len;
    memcpy(*out_ptr + sizeof(uint16_t), &out_len, sizeof(out_len));
    *out_ptr = out;
}

/*********************************************************************************************
* Populate the column and column metadata
* @param[in]    buffer   - The data buffer
* @param[in]    col_ix   - column index
* @param[out]   column   - Output column
* @param[out]   col_meta - Output column metadata
**********************************************************************************************/
int populate_column ( column_def* col, col_metadata_def* col_meta, char* buffer, int col_idx)
{
    int len = 0, offset = 0;
    char out[MAX_NAME_SIZE];

    /* Read Max Column Value Length */
    len = read_data (buffer, out, ':');
    col->actual_value_length = (uint16_t) get_numeric_val(out, len++);
    offset = len;

    /* Read Actual Column Value Length */
    len = read_data (buffer+offset, out, ':');
    col->max_value_length = (uint16_t) get_numeric_val(out, len++);
    offset += len;

    len = read_data (buffer+offset, out, '|');
    out[len++] = 0;
    strcpy (col->column_value, out);
    offset += len;

    col->column_index = col_idx;
    switch (col_idx)
    {
    case 0:
#ifndef ORACLE
        col_meta->gg_data_type = SQLDT_64BIT_S;
#else
        col_meta->gg_data_type = SQLDT_ASCII_V;
#endif
        break;
    case 1:
        col_meta->gg_data_type = SQLDT_ASCII_V;
        break;
    case 2:
        col_meta->gg_data_type = SQLDT_DATETIME;
        break;
    case 3:
        col_meta->gg_data_type = SQLDT_ASCII_V;
    }

    return offset;
}

/************************************************************************************************
* Process the input Insert record and create new Insert record from the second column value
* @param[out]   exit_params       - more_recs_ind is set accordingly in exit_params
* @param[out]   exit_call_result  - the result of the processing
************************************************************************************************/
void process_record (exit_result_def* exit_call_result
                     , exit_params_def* exit_params)
{
    short result_code;
    exit_rec_buf_def* record_buffer = NULL;
    char* rec_ptr;
    record_def* record;
    column_def column, new_col;
    col_metadata_def new_col_meta;
    char errmsg[MAX_BUFF_SIZE];
    char buf[MAX_BUFF_SIZE];
    char table_name[MAX_NAME_SIZE];
    static unsigned int read_offset = 0;
    int bytes_read = 0;
    int64_t table_id = 0;
    int64_t num_cols = 0;
    short itr = 0;
    static int64_t rec_count = 1; /* Holds the record number in process */
    static int64_t num_records = 0; /* Holds total number of records present */

    record = (record_def *) malloc(sizeof (record_def));
    record_buffer = (exit_rec_buf_def *) malloc(sizeof(exit_rec_buf_def));

    /*Get Operation Type*/
    memset (record, 0, sizeof(record));
    record->source_or_target = EXIT_FN_SOURCE_VAL;
    record->buffer = (char*) record_buffer;

    call_callback (GET_OPERATION_TYPE, record, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("User exit:Error (%hd) retrieving operation type.\n", result_code);
        free (record);
        free (record_buffer);
        *exit_call_result = EXIT_ABEND_VAL;
        return;
    }

    /* Return without processing if not INSERT operation */
    if (INSERT_VAL != record->io_type)
    {
        output_msg ("User exit:Operation in Process is %d is not supported\n"
                    , record->io_type);
        free (record);
        free (record_buffer);
        *exit_call_result = EXIT_OK_VAL;
        return;
    }

    /*Get the Second column value*/
    memset (&column, 0, sizeof(column_def));
    column.column_value = (char*)malloc(MAX_VAL_LENGTH);
    result_code = get_column_value(&column, 1);
    if (EXIT_FN_RET_OK != result_code &&
        EXIT_FN_RET_COLUMN_NOT_FOUND != result_code)
    {
        output_msg ("User exit:Error (%hd) occurred while retrieving column value.\n", result_code);
        free (column.column_value);
        free (record);
        free (record_buffer);
        *exit_call_result = EXIT_ABEND_VAL;
        return;
    }

    /* Retrieve Number of Records */
    memset (buf, 0, MAX_BUFF_SIZE);
    if (0 == num_records)
    {
        /* The num_records will be available at the start of record. */
        bytes_read = read_data (column.column_value, buf, '#');
        num_records = get_numeric_val (buf, bytes_read);
        bytes_read++; /* adjust the delimiter */
        if (!num_records)
        {
            output_msg ("User exit: No records present in the input buffer.\n");
            free (record);
            free (record_buffer);
            free (column.column_value);
            *exit_call_result = EXIT_ABEND_VAL;
            return;
        }
        read_offset = bytes_read;
    }

    /* Retrieve Table ID */
    bytes_read = read_data (column.column_value+read_offset, buf, '|');
    table_id = get_numeric_val (buf, bytes_read);
    bytes_read++; /* adjust the delimiter */
    if (!table_id)
    {
        output_msg ("User exit: Invalid table id %d.\n", table_id);
        free (record);
        free (record_buffer);
        free (column.column_value);
        *exit_call_result = EXIT_ABEND_VAL;
        return;
    }
    read_offset += bytes_read;

    /* Set table name */
    memset (&table_name, 0, MAX_NAME_SIZE);
    get_table_name (table_id, table_name);
    if (!strcmp (table_name, UNKNOWN))
    {
        output_msg ("User exit: No table found for table id %d.\n", table_id);
        free (record);
        free (record_buffer);
        free (column.column_value);
        *exit_call_result = EXIT_ABEND_VAL;
        return;
    }

    record->table_name = (char*) (&table_name);
    record->source_or_target = EXIT_FN_TARGET_VAL;

    call_callback (SET_TABLE_NAME, record, &result_code);
    if (EXIT_FN_RET_OK != result_code)
    {
        output_msg ("User exit:Error (%hd) setting table name (%s).\n", result_code, table_name);
        free (column.column_value);
        free (record);
        free (record_buffer);
        *exit_call_result = EXIT_ABEND_VAL;
    }

    /* Retreive number of columns in this record */
    bytes_read = read_data (column.column_value+read_offset, buf, '|');
    num_cols = get_numeric_val (buf, bytes_read);
    bytes_read++;
    read_offset += bytes_read;
    new_col.column_value = (char*)malloc(MAX_NAME_SIZE);

    memset (record_buffer, 0, sizeof(exit_rec_buf_def));
    rec_ptr = (char*) record_buffer;
    for (itr = 0; itr < num_cols; itr++)
    {
        bytes_read = populate_column ( &new_col, &new_col_meta, column.column_value+read_offset
                                       , itr);
        prepare_record (&rec_ptr, &new_col, &new_col_meta, errmsg);
        read_offset += bytes_read;
    }

    /* Calculate record length */
    record->length = (long)(rec_ptr - (char*)record_buffer);
    output_msg ("UserExit:(process_record) record length = %d", record->length);

    /* Set record buffer */
    record->before_after_ind = AFTER_IMAGE_VAL;
    call_callback (SET_RECORD_BUFFER, record, &result_code);
    if (EXIT_FN_RET_OK != result_code)
    {
        output_msg ( "User exit:Error(%hd) while setting the record buffer of Insert Record.\n"
                     , result_code);
        free (record);
        free (record_buffer);
        free (column.column_value);
        free (new_col.column_value);
        *exit_call_result = EXIT_ABEND_VAL;
        return;
    }

    if (rec_count < num_records)
    {
        /* set more_recs_ind to 'Y' if same record is required for processing again */
        exit_params->more_recs_ind = CHAR_YES_VAL;
        rec_count++;
    }
    else
    {
        exit_params->more_recs_ind = CHAR_NO_VAL;
        rec_count = 1;
        num_records = 0;
        read_offset = 0;
        free (schema);
        schema = NULL;
    }

    free (column.column_value);
    free (new_col.column_value);
    free (record);
    free (record_buffer);
}

/***************************************************************************************
*  ER user exit object called from various user exit points in extract and
*  replicat.
* @param[in]  exit_call_type   - Which operation to perform
* @param[out] exit-call_result - Result of the operation
* @param[out] exit_params      - Any parameters for the calling function
****************************************************************************************/
#ifdef WIN32
__declspec(dllexport) void CUSEREXIT (exit_call_type_def exit_call_type,
                      exit_result_def    *exit_call_result,
                      exit_params_def    *exit_params)
#else
void CUSEREXIT (exit_call_type_def exit_call_type,
                exit_result_def    *exit_call_result,
                exit_params_def    *exit_params)
#endif
{
    static short callback_opened = 0;
    short result_code;
    error_info_def error_info;

    /*Proceed processing if extract is not in process*/
    if ((strstr (exit_params->program_name, "extract"))== NULL)
    {
        return;
    }

    if (!callback_opened)
    {
        callback_opened = 1;

        /* If Windows, need to load the exported callback function explicitly.
           A Unix Shared object can reference the symbols within the original
           process image file.  There's no need to retrieve a pointer to
           the callback function. */
#ifdef WIN32
        if (!open_callback (exit_params->program_name, "ERCALLBACK"))
        {
            printf ("User exit:Error opening ER callback function.\n");
            *exit_call_result = EXIT_ABEND_VAL;
            return;
        }
#endif
    }

    switch (exit_call_type)
    {
        case EXIT_CALL_START:
            output_msg ("\nUser exit: EXIT_CALL_START.  Called from program: %s\n",
                        exit_params->program_name);
            break;

        case EXIT_CALL_STOP:
            output_msg ("\nUser exit: EXIT_CALL_STOP.\n");
            close_callback();
            break;

        case EXIT_CALL_BEGIN_TRANS:
            output_msg ("\nUser exit: EXIT_CALL_BEGIN_TRANS.\n");
            break;

        case EXIT_CALL_END_TRANS:
            output_msg ("\nUser exit: EXIT_CALL_END_TRANS.\n");
            break;

        case EXIT_CALL_CHECKPOINT:
            output_msg ("\nUser exit: EXIT_CALL_CHECKPOINT.\n");
            break;

        case EXIT_CALL_PROCESS_MARKER:
            output_msg ("\nUser exit: EXIT_CALL_PROCESS_MARKER.\n");
            break;

        case EXIT_CALL_PROCESS_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_PROCESS_RECORD.\n");
            process_record (exit_call_result, exit_params);
            break;

        case EXIT_CALL_DISCARD_ASCII_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_DISCARD_ASCII_RECORD.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500;

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("User exit:Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("User exit: Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);

            break;

        case EXIT_CALL_DISCARD_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_DISCARD_RECORD.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500;

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("User exit:Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("User exit: Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);

            break;

        case EXIT_CALL_FATAL_ERROR:
            output_msg ("\nUser exit: EXIT_CALL_FATAL_ERROR.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500;

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("User exit: Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                return;
            }

            output_msg ("User exit: Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);
            close_callback();
            break;
    }

    *exit_call_result = EXIT_OK_VAL;
    fflush (stdout);
}
