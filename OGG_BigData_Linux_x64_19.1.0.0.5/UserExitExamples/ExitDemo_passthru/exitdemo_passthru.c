/*************************************************************************************************
 Copyright (c) 1995, 2018, Oracle and/or its affiliates. All rights reserved.
 
  Program description:

  User exit example for Windows NT and Unix, and valid only for the extract pump.
  This is a hypothetical example shows the use of PASSTHRU mode. This example uses the TCUSTMER
  table, which can be created and manipulated using the demo*.sql scripts included with the ER
  release.
  The record is processed and written to the flat file in a self describing format. This output
  file is in readable text format.

  The record format example:
    Record No# 1
    Operation Type: 3/5/15/115
    Table Name: XYZ.TCUSTMER
    Operation Timestamp: 2008-11-07 07:37:58.000000
    Number of Columns:
    Number of Key Columns:

    col_idx. Col_name: col_value

  Callbacks that are exercised
        GET_OPERATION_TYPE                -Used to get IO Types
        GET_TABLE_NAME                    -Used to get the table name
        GET_TABLE_COLUMN_COUNT            -Used to get number of cols, number of key cols
        GET_COLUMN_NAME_FROM_INDEX        -Used to get the column name from column index
        GET_COLUMN_VALUE_FROM_INDEX       -Used to get column value
        GET_RECORD_BUFFER                 -Used to get record buffer
        GET_GMT_TIMESTAMP                 -Used to get GMT commit time of the record
        GET_STATISTICS                    -Used to get total statistics of the process
        GET_ERROR_INFO                    -Used to get error info
        OUTPUT_MESSAGE_TO_REPORT          -Used to write messages to report file

--------------------------------------------------------------------------------------------------
Param file options with Extract Pump using the User Exit

      EXTRACT EQALA                                     EXTRACT EQALAP

      SOURCEDB GG, USERID XYZ, PASSWORD XYZ             SOURCEDEFS dirdef\deftcust.def

      DISCARDFILE dirrpt\eqala.dsc, PURGE               DISCARDFILE dirrpt\eqalap.dsc, PURGE

      EXTTRAIL dirdat\T1                                CUSEREXIT userexit.dll CUSEREXIT PASSTHRU

      TABLE XYZ.TCUSTMER;                               TABLE XYZ.TCUSTMER;

---------------------------------------------------------------------------------------------------

**************************************************************************************************/

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

#define FILE_IO_ERROR    1

/* Output file */
const char* filename = "./dirdat/flatfile.dat";

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

/***********************************************************************************
*  Check whether printable character.
* @param[in] c - The character to test
* @returns   1 if printable else 0
***********************************************************************************/
short IsPrint(int c)
{
    if (c != EOF) c = (unsigned char)c;
    return isprint(c) ? 1 : 0;
}

/***********************************************************************************
*  Writes the record to the Flatfile
* @param[in] source_or_target - Whether is record is from source or for target
* @returns   1 if printable else 0
***********************************************************************************/
int out_to_file(short source_or_target, short ascii_or_internal)
{
    FILE* fp = NULL;
    record_def* record = NULL;
    table_def table;
    column_def column;
    env_value_def env_value;
    char ascii_str[1000];
    char buf[1000];
    int bytes_written = 0;
    int buf_size = 0;
    int i, j;
    unsigned short col_len;
    short result_code;
    static short rec_count = 1;

    record = (record_def*)malloc(sizeof(record_def));

    /* Get Operation type */
    memset (record, 0, sizeof(record));
    record->source_or_target = EXIT_FN_SOURCE_VAL;

    call_callback (GET_OPERATION_TYPE, record, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving operation type.\n", result_code);
        free (record);
        return result_code;
    }

    /* Create/Open(in Append Mode) Flat File */
    fp = fopen (filename, "at");
    if (!fp)
    {
        output_msg ("Error creating file flatfile.dat\n");
        free (record);
        return FILE_IO_ERROR;
    }

    /* Get table name */
    memset (&env_value, 0, sizeof(env_value_def));
    env_value.source_or_target = source_or_target;
    env_value.buffer = (char *)malloc(500);
    env_value.max_length = 500;

    call_callback (GET_TABLE_NAME, &env_value, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table name.\n", result_code);
        free (env_value.buffer);
        free (record);
        return result_code;
    }

    /* Write Operation Type and Table Name to file */
    buf_size = sprintf ( buf, "  Record No#%d\n  Operation Type: %d\n  Table Name:%s\n"
                              ,rec_count++, record->io_type, env_value.buffer);
    bytes_written = fwrite (buf, 1, buf_size, fp);
    if (bytes_written != buf_size)
    {
        output_msg ("Error while writing Operation type to file.\n");
        free (env_value.buffer);
        free (record);
        return FILE_IO_ERROR;
    }

    /* Get Operation Timestamp */
    call_callback (GET_GMT_TIMESTAMP, record, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving operation timestamp.\n", result_code);
        free (env_value.buffer);
        free (record);
        return result_code;
    }

    /* Write Operation Timestamp to file */
    buf_size = sprintf ( buf, "  Operation Timestamp: %s\n",record->io_datetime);
    bytes_written = fwrite (buf, 1, buf_size, fp);
    if (bytes_written != buf_size)
    {
        output_msg ("Error while writing Operation timestamp to file.\n");
        free (env_value.buffer);
        free (record);
        return FILE_IO_ERROR;
    }

    /* Get table column information */
    memset (&table, 0, sizeof(table_def));
    table.source_or_target = EXIT_FN_SOURCE_VAL;

    call_callback (GET_TABLE_COLUMN_COUNT, &table, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table column count.\n", result_code);
        free (env_value.buffer);
        free (record);
        return result_code;
    }

    /* Write Comlumn and Key Column Count to file */
    buf_size = sprintf ( buf, "  Number of Columns: %d\n  Number of Key Columns: %d\n\n"
                        ,table.num_columns, table.num_key_columns);
    bytes_written = fwrite (buf, 1, buf_size, fp);
    if (bytes_written != buf_size)
    {
        output_msg ("Error while writing Column count to file.\n");
        free (env_value.buffer);
        free (record);
        return FILE_IO_ERROR;
    }

    output_msg ("Table %.*s has %hd columns.\n",
                env_value.actual_length, env_value.buffer,
                table.num_columns);

    memset(&column, 0, sizeof (column_def));
    column.source_or_target = source_or_target;
    column.column_value_mode = ascii_or_internal;
    column.column_value = (char*)malloc(4000);
    column.max_value_length = 4000;

    for (i = 0; i < table.num_columns; i++)
    {
        env_value.index = column.column_index = i;

        call_callback (GET_COLUMN_NAME_FROM_INDEX, &env_value, &result_code);
        if (result_code != EXIT_FN_RET_OK)
        {
            output_msg ("Error (%hd) retrieving column name.\n", result_code);
            free (env_value.buffer);
            free (column.column_value);
            free (record);
            return result_code;
        }

        output_msg ("Column index (%d) = %.*s.\n",
                    i, env_value.actual_length, env_value.buffer);

        call_callback (GET_COLUMN_VALUE_FROM_INDEX, &column, &result_code);
        if (result_code != EXIT_FN_RET_OK &&
            result_code != EXIT_FN_RET_COLUMN_NOT_FOUND)
        {
            output_msg ("Error (%hd) retrieving column value.\n", result_code);
            free (env_value.buffer);
            free (column.column_value);
            free (record);
            return result_code;
        }

        /* Write Comlumn Index and Column Name to file */
        buf_size = sprintf ( buf, "  %d. %s: ", column.column_index, env_value.buffer);
        bytes_written = fwrite (buf, 1, buf_size, fp);
        if (bytes_written != buf_size)
        {
            output_msg ("Error while writingColumn count to file.\n");
            free (env_value.buffer);
            free (column.column_value);
            free (record);
            return FILE_IO_ERROR;
        }

        if (result_code == EXIT_FN_RET_COLUMN_NOT_FOUND)
        {
            strcpy (buf, "Column not found in record.\n");
            buf_size = strlen(buf);
            output_msg (buf);
        }
        else
        if (column.null_value)
        {
            strcpy (buf, "Column value is NULL.\n");
            buf_size = strlen(buf);
            output_msg (buf);
        }
        else
        {
            if (ascii_or_internal == EXIT_FN_ASCII_FORMAT)
            {
                output_msg ("Column value = %s.\n", column.column_value);
                buf_size = sprintf (buf, "%s", column.column_value);
            }
            else
            {
                /* Ouput internal value in hex */
                output_msg ("Internal source column value = \n");

                col_len = column.actual_value_length;

                for (j = 0; j < col_len; j++)
                {
                    ascii_str[j] = IsPrint (column.column_value[j]) ?
                                   column.column_value[j] : '.';
                }
                ascii_str[j] = 0;
                buf_size = sprintf (buf, "%s\n", ascii_str);
            }
        }
        /* Write Comlumn value */
        bytes_written = fwrite (buf, 1, buf_size, fp);
        if (bytes_written != buf_size)
        {
            output_msg ("Error while writing Column value to file.\n");
            free (env_value.buffer);
            free (column.column_value);
            free (record);
            return FILE_IO_ERROR;
        }
    }
    buf_size = sprintf ( buf, "***************************************************\n\n");
    bytes_written = fwrite (buf, 1, buf_size, fp);

    free (env_value.buffer);
    free (column.column_value);
    free (record);
    fclose (fp);
    return EXIT_FN_RET_OK;
}

/***************************************************************************
*  ER user exit object called from various user exit points in extract and
*  replicat.
* @param[in]  exit_call_type   - Which operation to perform
* @param[out] exit-call_result - Result of the operation
* @param[out] exit_params      - Any parameters for the calling function
***************************************************************************/
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
    char table_name[200];
    error_info_def error_info;
    statistics_def statistics;

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
            printf ("Error opening ER callback function.\n");
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

            memset (&statistics, 0, sizeof(statistics));

            /* Retrieve statistics since application startup */
            statistics.group = EXIT_STAT_GROUP_STARTUP;
            strcpy (table_name, "TCUSTMER");
            statistics.table_name = table_name;

            call_callback (GET_STATISTICS, &statistics, &result_code);
            if (result_code != EXIT_FN_RET_OK &&
                result_code != EXIT_FN_RET_TABLE_NOT_FOUND)
            {
                output_msg ("Error (%hd) retrieving statistics.\n", result_code);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            if (result_code == EXIT_FN_RET_OK)
            {
                output_msg ("Statistics since process startup at %s for %s:\n"
                            "Total inserts:       \t%ld\n"
                            "Total updates:       \t%ld\n"
                            "Total deletes:       \t%ld\n"
                            "Total upserts:       \t%ld\n"
                            "Total discards:      \t%ld\n"
                            "Total ignores:       \t%ld\n"
                            "Total truncates:     \t%ld\n"
                            "Total DB operations: \t%ld\n"
                            "Total operations:    \t%ld\n\n",
                            statistics.start_datetime, table_name,
                            statistics.num_inserts, statistics.num_updates,
                            statistics.num_deletes, statistics.num_upserts, 
                            statistics.num_discards,
                            statistics.num_ignores, statistics.num_truncates,
                            statistics.total_db_operations, statistics.total_operations);
            }

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
            result_code = out_to_file(EXIT_FN_SOURCE_VAL, EXIT_FN_INTERNAL_FORMAT);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) while processing record.\n", result_code);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }
            break;

        case EXIT_CALL_DISCARD_ASCII_RECORD:
        case EXIT_CALL_DISCARD_RECORD:
        case EXIT_CALL_FATAL_ERROR:

            if (exit_call_type == EXIT_CALL_DISCARD_ASCII_RECORD)
                output_msg ("\nUser exit: EXIT_CALL_DISCARD_ASCII_RECORD.\n");
            else
            if (exit_call_type == EXIT_CALL_DISCARD_RECORD)
                output_msg ("\nUser exit: EXIT_CALL_DISCARD_RECORD.\n");
            else
                output_msg ("\nUser exit: EXIT_CALL_FATAL_ERROR.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500; /* Including null terminator */

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);
            if (exit_call_type == EXIT_CALL_FATAL_ERROR)
                close_callback();
            break;
    }

    *exit_call_result = EXIT_OK_VAL;
    fflush (stdout);
}
