/* Copyright (c) 2011, 2018, Oracle and/or its affiliates. 
All rights reserved.*/

/*

   NAME
     exitdemo_utf16.c - <one-line expansion of the name>

   DESCRIPTION
     User exit example for Windows NT and Unix.  This example uses the
     TCUSTMER and TCUSTORD tables, which can be created and manipulated using
     the demo*.sql scripts included with the ER release.

   EXPORT FUNCTION(S)
     <external functions defined for use outside package - one-line descriptions>

   INTERNAL FUNCTION(S)
     <other external functions defined - one-line descriptions>

   STATIC FUNCTION(S)
     <static functions defined - one-line descriptions>

   NOTES
     <other useful comments, qualifications, etc.>

*/

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

  #if defined (_OSS_HOST)
     #include <stdint.h>
  #endif

  #include <string.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <ctype.h>
#endif

#include "usrdecs.h"

/* ER callback routine */
#ifndef WIN32
void ERCALLBACK(ercallback_function_codes function_code,
                void *buf, short *presult_code);
#else
typedef void (*FPERCALLBACK)(ercallback_function_codes function_code,
                             void *buf, short *presult_code);

HINSTANCE hEXE; /* EXE handle */
FPERCALLBACK fp_ERCallback; /* Callback function pointer */

/***************************************************************************
  Open the ER callback function explicitly.
***************************************************************************/
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

/***************************************************************************
  Close the callback function.
***************************************************************************/
void close_callback (void)
{
#ifdef WIN32
    FreeLibrary (hEXE);
#endif
}

/***************************************************************************
  Call the callback function.
***************************************************************************/
void call_callback (ercallback_function_codes function_code,
                    void *buf, short *result_code)
{
#ifdef WIN32
    fp_ERCallback (function_code, buf, result_code);
#else
    ERCALLBACK (function_code, buf, result_code);
#endif
}

/***************************************************************************
  Test for Little Endian machine format
***************************************************************************/
short LittleEndian( void )
{
    short little_endian = 1;
    char *little_endian_c = (char *)&little_endian;

    if (*little_endian_c)
    {
        return 1;
    }
    return 0;
}

/***************************************************************************
  Reverse bytes for Little Endian/Big Endian
***************************************************************************/
void ReverseBytes (void *buf, short cnt)
{
    char *b = (char *)buf, *e = (char *)buf + cnt - 1;
    char temp;

    for (; b < e; b++, e--)
    {
        temp = *b;
        *b = *e;
        *e = temp;
    }
}

/***************************************************************************
  Output a message to the report file (or console).
***************************************************************************/
void output_msg (char *msg,...)
{
    short result_code;
    char temp_msg[1000];
    uint16_t utf16_msg[1000];   /* the message we send to core for printing
                                   should be in session's charset.
                                   For this demo program, we hard-code the
                                   session charset to UTF16 */
    int i, msgLen;

    va_list args;

    va_start (args, msg);

    vsprintf (temp_msg, msg, args);

    va_end (args);
  
    msgLen = strlen(temp_msg);
    for (i = 0; i < msgLen; i++)
    {
       utf16_msg[i] = temp_msg[i];
    }
    utf16_msg[i] = 0;

    call_callback (OUTPUT_MESSAGE_TO_REPORT, utf16_msg, &result_code);
}

/***************************************************************************
  An ugly way to convert src to a UTF16BE string
***************************************************************************/
void cnvt_utf16(char *dstBuf, short dstLen, char *src)
{
typedef unsigned short uint16_t;
   uint16_t *dst;
   char *p;

   dst = (uint16_t *)dstBuf;

   if (LittleEndian())
     *dst = 0xfeff;
   else
     *dst = 0xfffe;

   dst++;
   for (p = src; *p; p++) {
     *dst = *p;
     if (LittleEndian())
        ReverseBytes ((void *)dst, sizeof(uint16_t));
     dst++;
   }

   /* null terminate */
   *dst = 0;
}

short IsPrint(int c)
{
    if (c != EOF) c = (unsigned char)c;
    return isprint(c) ? 1 : 0;
}

/***************************************************************************
  Display record information.
***************************************************************************/
short display_record (short source_or_target,
                      short ascii_or_internal)
{
    int i, j, k;
    unsigned short col_len;
    char hex_str[31];
    char ascii_str[11];
    char *a;
    short result_code;
    table_def table;
    column_def column;
    env_value_def env_value;

    char tempMsg[2000];
    short temp_result_code;

    /* Get table name */
    memset (&env_value, 0, sizeof(env_value_def));
    if (source_or_target == EXIT_FN_CURRENT_VAL)
        env_value.source_or_target = EXIT_FN_TARGET_VAL;
    else
        env_value.source_or_target = source_or_target;
    env_value.buffer = (char *)malloc(500);
    env_value.max_length = 500;

    call_callback (GET_TABLE_NAME, &env_value, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table name.\n", result_code);
        free (env_value.buffer);
        return result_code;
    }

    /* Get table column information */
    memset (&table, 0, sizeof(table_def));
    if (source_or_target == EXIT_FN_CURRENT_VAL)
        table.source_or_target = EXIT_FN_TARGET_VAL;
    else
        table.source_or_target = source_or_target;

    call_callback (GET_TABLE_COLUMN_COUNT, &table, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table column count.\n", result_code);
        free (env_value.buffer);
        return result_code;
    }

    output_msg ("Table ");
    memcpy(tempMsg, env_value.buffer, env_value.actual_length);
    call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);
    output_msg (" has %hd columns.\n", table.num_columns);

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
            return result_code;
        }

        output_msg ("Column index (%d) = ", i);
        memcpy(tempMsg, env_value.buffer, env_value.actual_length); 
        call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);
        output_msg (".\n");

        call_callback (GET_COLUMN_VALUE_FROM_INDEX, &column, &result_code);
        if (result_code != EXIT_FN_RET_OK &&
            result_code != EXIT_FN_RET_COLUMN_NOT_FOUND)
        {
            output_msg ("Error (%hd) retrieving column value.\n", result_code);
            free (env_value.buffer);
            free (column.column_value);
            return result_code;
        }

        if (result_code == EXIT_FN_RET_COLUMN_NOT_FOUND)
            output_msg ("Column not found in record.\n");
        else
        if (column.null_value)
            output_msg ("Column value is NULL.\n");
        else
        {
            if (ascii_or_internal == EXIT_FN_ASCII_FORMAT)
                output_msg ("Column value = %s.\n", column.column_value);
            else
            {
                /* Ouput internal value in hex */
                output_msg ("Internal source column value = \n");

                col_len = column.actual_value_length;

                for (j = 0; j < col_len; j += 10)
                {
                    for (a = column.column_value + j, k = 0;
                         k < 10 && j + k < col_len;
                         a++, k++)
                    {
                        ascii_str[k] =
                            IsPrint (column.column_value[j + k]) ?
                                     column.column_value[j + k] : '.';
                        sprintf (hex_str + (k * 3), "%02X ",
                                 (unsigned char)column.column_value[j + k]);
                    }
                    ascii_str[k] = 0;

                    output_msg ("%5d:  %-10s   %-30s\n",
                                j, ascii_str, hex_str);
                }
            }
        }
    }

    free (env_value.buffer);
    free (column.column_value);

    return EXIT_FN_RET_OK;
}

/***************************************************************************
  ER user exit object called from various user exit points in extract and
  replicat.
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
    char column_name_buf[200];
    char print_msg[500];
    exit_rec_buf_def *record_buffer = NULL;
    exit_rec_buf_ascii_def *ascii_record_buffer = NULL;
    record_def *record = NULL;
    position_def *position_rec = NULL;
    token_value_def *token_rec = NULL;
    marker_info_def marker_info;
    error_info_def error_info;
    statistics_def statistics;
    env_value_def env_value;
    column_def column;
    col_metadata_def col_meta;
    table_metadata_def table_meta;
    table_def table;
    session_def    session_cs;
    database_defs  dbMetadata;
    short i;
    char *tbl_name;
    char srcDB_name[200];
    char tgtDB_name[200];
    char srcDB_locale[200];
    char tgtDB_locale[200];
    size_t actualLen;
   
    uint32_t seqno;
    int32_t rba;

    typedef struct
    {
        char ch_seqno[4];
        char ch_rba[4];
    } char_position;
    char_position position_char;

    char  tempMsg[2000];
    short temp_result_code;
    short msgLen;

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

    record = (record_def *) malloc(sizeof (record_def));
    record_buffer = (exit_rec_buf_def *) malloc(sizeof(exit_rec_buf_def));
    ascii_record_buffer  = (exit_rec_buf_ascii_def *) malloc(sizeof (exit_rec_buf_ascii_def));

    switch (exit_call_type)
    {
        case EXIT_CALL_START:
            output_msg ("\nUser exit: EXIT_CALL_START.  Called from program: %s\n",
                        exit_params->program_name);

            /* Set session character set */
            memset (&session_cs, 0, sizeof(session_cs));
            session_cs.session_charset = ULIB_CS_UTF16;
            call_callback (SET_SESSION_CHARSET, &session_cs, &result_code);
            if (result_code != EXIT_FN_RET_OK) {
              output_msg ("\nUser exit: SET SESSION CHARSET: (0x%x) Is Not a Valid Character Set ID\n",
                        (unsigned int)session_cs.session_charset);
              output_msg ("\n           UES DEFAULT SESSION CHARSET\n");
            }

            break;

        case EXIT_CALL_STOP:
            output_msg ("\nUser exit: EXIT_CALL_STOP.\n");

            memset (&statistics, 0, sizeof(statistics));

            /* Retrieve statistics since application startup */
            statistics.group = EXIT_STAT_GROUP_STARTUP;
            cnvt_utf16 (table_name, 200, "TCUSTMER");
            statistics.table_name = table_name;

            call_callback (GET_STATISTICS, &statistics, &result_code);
            if (result_code != EXIT_FN_RET_OK &&
                result_code != EXIT_FN_RET_TABLE_NOT_FOUND)
            {
                output_msg ("Error (%hd) retrieving statistics.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
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

            memset (&statistics, 0, sizeof(statistics));
            statistics.group = EXIT_STAT_GROUP_STARTUP;
            /* use UTF-16 string of "TCUSTORD"
            strcpy (table_name, "TCUSTORD");
             */
            cnvt_utf16(table_name, 200, "TCUSTORD");
            statistics.table_name = table_name;

            call_callback (GET_STATISTICS, &statistics, &result_code);
            if (result_code != EXIT_FN_RET_OK &&
                result_code != EXIT_FN_RET_TABLE_NOT_FOUND)
            {
                output_msg ("Error (%hd) retrieving statistics.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
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
                            "Total operations:    \t%ld\n",
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

            position_rec = (position_def *) malloc (sizeof(position_def));
            position_rec->position = (char *) malloc (sizeof (uint32_t) + sizeof (int32_t));  /* current expected size of seqno and rba */

            position_rec->ascii_or_internal = EXIT_FN_INTERNAL_FORMAT;
            position_rec->position_type = CURRENT_CHECKPOINT;
            call_callback (GET_POSITION,  position_rec,&result_code);
            if (!result_code)
            {
                memcpy (&position_char, position_rec->position, position_rec->position_len);
                if (LittleEndian())
                {
                    ReverseBytes (&position_char.ch_seqno, sizeof (position_char.ch_seqno));
                    ReverseBytes (&position_char.ch_rba, sizeof (position_char.ch_rba));
                }

                memcpy (&seqno, position_char.ch_seqno, sizeof (seqno));
                memcpy (&rba, &position_char.ch_rba[0], sizeof (rba));
                output_msg ("\nGET_POSITION CURRENT_CHECKPOINT, values seqno %ld rba %ld\n",
                             (long) seqno, (long) rba );

            }
            free(position_rec->position);
            free(position_rec);
            break;

        case EXIT_CALL_PROCESS_MARKER:
            output_msg ("\nUser exit: EXIT_CALL_PROCESS_MARKER.\n");

            memset (&marker_info, 0, sizeof(marker_info));

            call_callback (GET_MARKER_INFO, &marker_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving marker information.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("Marker information:\n");

            output_msg ("Date/time added: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(marker_info.added[msgLen] == 0 &&
                     marker_info.added[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, marker_info.added, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);

            output_msg ("\nDate/time processed: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(marker_info.processed[msgLen] == 0 &&
                     marker_info.processed[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, marker_info.processed, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);

            output_msg ("\nProgram name: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(marker_info.program[msgLen] == 0 &&
                     marker_info.program[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, marker_info.program, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);

            output_msg ("Group name: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(marker_info.group[msgLen] == 0 &&
                     marker_info.group[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, marker_info.group, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);

            output_msg ("\nNode name: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(marker_info.node[msgLen] == 0 &&
                     marker_info.node[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, marker_info.node, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);

            output_msg ("\nMarker text: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(marker_info.text[msgLen] == 0 &&
                     marker_info.text[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, marker_info.text, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);
            break;

        case EXIT_CALL_PROCESS_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_PROCESS_RECORD.\n");

            position_rec = (position_def *) malloc (sizeof(position_def));
            position_rec->position = (char *) malloc (sizeof (uint32_t) + sizeof (int32_t));  /* current expected size of seqno and rba */

            position_rec->ascii_or_internal = EXIT_FN_INTERNAL_FORMAT;
            position_rec->position_type = STARTUP_CHECKPOINT;
            call_callback (GET_POSITION,  position_rec,&result_code);
            if (!result_code)
            {
                memcpy (&position_char, position_rec->position, position_rec->position_len);
                if (LittleEndian())
                {
                    ReverseBytes (&position_char.ch_seqno, sizeof (position_char.ch_seqno));
                    ReverseBytes (&position_char.ch_rba, sizeof (position_char.ch_rba));
                }
                memcpy (&seqno, position_char.ch_seqno, sizeof (seqno));
                memcpy (&rba, position_char.ch_rba, sizeof (rba));
                output_msg ("\nGET_POSITION STARTUP_CHECKPOINT, values seqno %ld rba %ld\n",
                            (long) seqno, (long) rba );
            }

            position_rec->ascii_or_internal = EXIT_FN_INTERNAL_FORMAT;
            position_rec->position_type = CURRENT_CHECKPOINT;
            call_callback (GET_POSITION,  position_rec,&result_code);
            if (!result_code)
            {
                memcpy (&position_char, position_rec->position, position_rec->position_len);
                if (LittleEndian())
                {
                    ReverseBytes (&position_char.ch_seqno, sizeof (position_char.ch_seqno));
                    ReverseBytes (&position_char.ch_rba, sizeof (position_char.ch_rba));
                }
                memcpy (&seqno, position_char.ch_seqno, sizeof (seqno));
                memcpy (&rba, position_char.ch_rba, sizeof (rba));
                output_msg ("\nGET_POSITION CURRENT_CHECKPOINT, values seqno %ld rba %ld\n",
                            (long) seqno, (long) rba );
            }

            free(position_rec->position);
            free(position_rec);

            memset (record, 0, sizeof(record));
            record->source_or_target = EXIT_FN_SOURCE_VAL;
            record->buffer = (char *) record_buffer;

            call_callback (GET_OPERATION_TYPE, record, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving operation type.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            /* Example of getting a token placed in a prior extract as
               TABLE OWNER.TABLE, tokens (TK-HOST = @GETENV ("GGENVIRONMENT" , "HOSTNAME"));
            */

            token_rec = (token_value_def *) malloc (sizeof(token_value_def));
            token_rec->max_length = 50;
            token_rec->token_value = (char *) malloc (token_rec->max_length + 1);
            token_rec->token_name = (char *) malloc (sizeof ("TK-HOST") * 2);
            /* use UTF-16 string of "TK-HOST"
            strcpy (token_rec->token_name, "TK-HOST");
             */
            cnvt_utf16 (token_rec->token_name,  sizeof("TK-HOST") * 2, "TK-HOST");
            call_callback (GET_USER_TOKEN_VALUE, token_rec, &result_code);

            if (token_rec->actual_length > 0)
            {
                output_msg ("\nGET_TOKEN_VALUE for Token TK-HOST giving HOSTNAME %s ", token_rec->token_value);
            }
            free(token_rec->token_name);
            free(token_rec->token_value);
            free(token_rec);

            /* An example of how a user exit parameter can be used...

               We can ignore deletes on the TCUSTMER table by specifying the
               following in the EXTRACT/REPLICAT parameter file:
               MAP TCUSTMER, TARGET TCUSTMER, EXITPARAM "IGNOREDELETES" */

            if (!strcmp (exit_params->function_param, "IGNOREDELETES") &&
                record->io_type == DELETE_VAL)
            {
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_IGNORE_VAL;
                return;
            }

            call_callback (GET_RECORD_BUFFER, record, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving record buffer.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("Processing record with operation type (%hd).\n",
                        record->io_type);

            output_msg ("\n*** SOURCE RECORD ***\n");
            result_code = display_record (EXIT_FN_SOURCE_VAL,
                                          EXIT_FN_INTERNAL_FORMAT);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) displaying source column values.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            memset (&table, 0, sizeof(table_def));

            if (record->mapped)
                table.source_or_target = EXIT_FN_TARGET_VAL;
            else
                table.source_or_target = EXIT_FN_SOURCE_VAL;

            call_callback (GET_TABLE_COLUMN_COUNT, &table, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving table column count.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
               return;
            }

            memset (&table_meta, 0, sizeof(table_meta));
            if (record->mapped)
                table_meta.source_or_target = EXIT_FN_TARGET_VAL;
            else
                table_meta.source_or_target = EXIT_FN_SOURCE_VAL;
            table_meta.table_name = (char *)malloc(100);
            table_meta.max_name_length = 100;
            table_meta.key_columns = (short *)malloc( table.num_key_columns *  sizeof(short)); /* Must know the number of expected keys */

            call_callback (GET_TABLE_METADATA, &table_meta, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving table metadata.\n", result_code);
                free (table_meta.table_name);
                free (table_meta.key_columns);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("*** GET_TABLE_METADATA ***\n");
            output_msg ("table_name: ");
            /* get the length of the utf16 msg string */
            msgLen =0;
            while (!(table_meta.table_name[msgLen] == 0 &&
                     table_meta.table_name[msgLen + 1] == 0))
               msgLen += 2;
            memcpy(tempMsg, table_meta.table_name, msgLen);
            call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);
            output_msg ("\n  num_columns: (%hd)\n"
                "num_key_columns (%hd)\n key_columns0 (%hd)\n using_pseudo_key (%hd)\n"
                "source_or_target (%hd)\n", 
                table_meta.num_columns,
                table_meta.num_key_columns, table_meta.key_columns[0],
                table_meta.using_pseudo_key, table_meta.source_or_target);

            free (table_meta.table_name);
            free (table_meta.key_columns);

            memset (&col_meta, 0, sizeof(col_meta));
            if (record->mapped)
                col_meta.source_or_target = EXIT_FN_TARGET_VAL;
            else
                col_meta.source_or_target = EXIT_FN_SOURCE_VAL;
            col_meta.source_or_target = EXIT_FN_SOURCE_VAL;
            col_meta.column_name = (char *)malloc(100);
            col_meta.max_name_length = 100;


            for (i=0; i < table.num_columns;i++)
            {
                col_meta.column_index = i;

                call_callback (GET_COL_METADATA_FROM_INDEX, &col_meta, &result_code);
                if (result_code != EXIT_FN_RET_OK)
                {
                    output_msg ("Error (%hd) retrieving column metadata.\n", result_code);
                    free (col_meta.column_name);
                    free (record);
                    free (record_buffer);
                    free (ascii_record_buffer);
                    *exit_call_result = EXIT_ABEND_VAL;
                    return;
                }

                output_msg ("*** GET_COL_METADATA_FROM_INDEX *** \n");
                output_msg ("column_name: (");
                /* get the length of the utf16 msg string */
                msgLen =0;
                while (!(col_meta.column_name[msgLen] == 0 &&
                         col_meta.column_name[msgLen + 1] == 0))
                   msgLen += 2;
                memcpy(tempMsg, col_meta.column_name, msgLen);
                call_callback(OUTPUT_MESSAGE_TO_REPORT, tempMsg, &temp_result_code);
                output_msg (")\n native_data_type: (%hd)\n"
                                "gg_data_type (%hd)\n gg_sub_data_type (%hd)\n is_nullable (%hd)\n"
                                "is_part_of_key (%hd)\n length (%hd)\n precision (%hd)\n scale (%hd)\n"
                                "source_or_target (%hd).\n KEYCOLIDX %hd\n", 
                    col_meta.native_data_type,
                    col_meta.gg_data_type, col_meta.gg_sub_data_type, col_meta.is_nullable,
                    col_meta.is_part_of_key, col_meta.length, col_meta.precision, col_meta.scale,
                    col_meta.source_or_target, col_meta.key_column_index);
            }
            free (col_meta.column_name);

            if (record->mapped) /* We have a target record */
            {
                output_msg ("\n*** TARGET RECORD ***\n");
                result_code = display_record (EXIT_FN_TARGET_VAL,
                                              EXIT_FN_INTERNAL_FORMAT);
                if (result_code != EXIT_FN_RET_OK)
                {
                    output_msg ("Error (%hd) displaying target column values.\n", result_code);
                    free (record);
                    free (record_buffer);
                    free (ascii_record_buffer);
                    *exit_call_result = EXIT_ABEND_VAL;
                    return;
                }

                output_msg ("\n*** CURRENT RECORD ***\n");

                column.source_or_target = EXIT_FN_CURRENT_VAL;

                memset (&error_info, 0, sizeof(error_info));
                error_info.error_msg = (char *)malloc(500);
                error_info.max_length = 500; /* Including null terminator */

                call_callback (FETCH_CURRENT_RECORD_WITH_LOCK, &error_info, &result_code);
                if (result_code != EXIT_FN_RET_OK &&
                    result_code != EXIT_FN_RET_NOT_SUPPORTED)
                {
                    if (result_code == EXIT_FN_RET_FETCH_ERROR)
                        output_msg ("SQL error (%ld, %s) fetching current record.\n",
                                    error_info.error_num, error_info.error_msg);
                    else
                        output_msg ("Error (%hd) fetching current record.\n", result_code);

                    free (record);
                    free (record_buffer);
                    free (ascii_record_buffer);
                    *exit_call_result = EXIT_ABEND_VAL;
                    free (error_info.error_msg);
                    return;
                }

                free (error_info.error_msg);

                if (result_code == EXIT_FN_RET_OK)
                {
                    result_code = display_record (EXIT_FN_CURRENT_VAL,
                                                  EXIT_FN_INTERNAL_FORMAT);
                    if (result_code != EXIT_FN_RET_OK)
                    {
                        output_msg ("Error (%hd) displaying current column values.\n", result_code);
                        free (record);
                        free (record_buffer);
                        free (ascii_record_buffer);
                        *exit_call_result = EXIT_ABEND_VAL;
                        return;
                    }
                }
            }

            /************************************************/
            /* Perform example record transformations here. */
            /************************************************/

            /* Get target table name */
            memset (&env_value, 0, sizeof(env_value_def));
            env_value.source_or_target = EXIT_FN_TARGET_VAL;
            env_value.buffer = (char *)malloc(500);
            env_value.max_length = 500;

            call_callback (GET_TABLE_NAME, &env_value, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving table name.\n", result_code);
                free (env_value.buffer);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            /* Get the customer name */
            memset (&column, 0, sizeof(column_def));
            if (record->mapped)
                column.source_or_target = EXIT_FN_TARGET_VAL;
            else
                column.source_or_target = EXIT_FN_SOURCE_VAL;
            column.column_value_mode = EXIT_FN_ASCII_FORMAT;
            column.column_value = (char*)malloc(4000);
            column.max_value_length = 4000;
            column.column_name = column_name_buf;

            /* use UTF-16 string of "CUST_CODE"
            strcpy (column.column_name, "CUST_CODE");
             */
            cnvt_utf16 (column.column_name, 200, "CUST_CODE");

            call_callback (GET_COLUMN_VALUE_FROM_NAME, &column, &result_code);
            if (result_code != EXIT_FN_RET_OK &&
                result_code != EXIT_FN_RET_COLUMN_NOT_FOUND)
            {
                output_msg ("Error (%hd) retrieving column value.\n", result_code);
                free (env_value.buffer);
                free (column.column_value);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            /* Let's modify the record if the customer is "WILL" */
            if (!memcmp (column.column_value, "WILL", column.actual_value_length))
            {
                /* Change customer "WILL" to customer "JILL" on both tables
                   (TCUSTMER and TCUSTORD). */
                /* use UTF-16 string of "CUST_CODE"
                strcpy (column.column_value, "JILL");
                 */
                cnvt_utf16 (column.column_value, 100,  "JILL");
                column.actual_value_length = (unsigned short) strlen ("JILL") * 2;

                call_callback (SET_COLUMN_VALUE_BY_NAME, &column, &result_code);
                if (result_code != EXIT_FN_RET_OK)
                {
                    output_msg ("Error (%hd) setting column value.\n", result_code);
                    free (env_value.buffer);
                    free (column.column_value);
                    free (record);
                    free (record_buffer);
                    free (ascii_record_buffer);
                    *exit_call_result = EXIT_ABEND_VAL;
                    return;
                }

                /* table names are generally in format of OWNER.TBL,
                   we don't always know your owner, you should remove this and compare the owner.table name */
                tbl_name = strchr (env_value.buffer, '.'); /* move passed owner */
                if (tbl_name == NULL)
                    tbl_name = env_value.buffer;
                else
                    tbl_name++; /* move passed . */


                /* Change the product amount to 100 on Jill's order. */
                if (!strcmp (tbl_name, "TCUSTORD"))
                {
                    cnvt_utf16 (column.column_name, 100, "PRODUCT_AMOUNT");
                    cnvt_utf16 (column.column_value, sizeof("100") * 2, "100");
                    column.actual_value_length = (unsigned short) strlen (column.column_value);

                    call_callback (SET_COLUMN_VALUE_BY_NAME, &column, &result_code);
                    if (result_code != EXIT_FN_RET_OK)
                    {
                        output_msg ("Error (%hd) setting column value.\n", result_code);
                        free (env_value.buffer);
                        free (column.column_value);
                        free (record);
                        free (record_buffer);
                        free (ascii_record_buffer);
                        *exit_call_result = EXIT_ABEND_VAL;
                        return;
                    }
                }
            }

            free (env_value.buffer);
            free (column.column_value);
            break;

        case EXIT_CALL_DISCARD_ASCII_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_DISCARD_ASCII_RECORD.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500; /* Including null terminator */

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);

            memset (record, 0, sizeof(record));
            record->source_or_target = EXIT_FN_SOURCE_VAL;
            record->buffer = (char *)ascii_record_buffer;

            call_callback (GET_RECORD_BUFFER, record, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving ASCII record buffer.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("ASCII discard record: %s.\n", record->buffer);
            break;

        case EXIT_CALL_DISCARD_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_DISCARD_RECORD.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500; /* Including null terminator */

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);

            memset (record, 0, sizeof(record));
            record->source_or_target = EXIT_FN_SOURCE_VAL;
            record->buffer = (char *) record_buffer;

            call_callback (GET_RECORD_BUFFER, record, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving ASCII record buffer.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            result_code = display_record (EXIT_FN_SOURCE_VAL,
                                          EXIT_FN_ASCII_FORMAT);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) displaying source column values.\n", result_code);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            if (record->mapped) /* We have a target record */
            {
                result_code = display_record (EXIT_FN_TARGET_VAL,
                                              EXIT_FN_ASCII_FORMAT);
                if (result_code != EXIT_FN_RET_OK)
                {
                    output_msg ("Error (%hd) displaying target column values.\n", result_code);
                    free (record);
                    free (record_buffer);
                    free (ascii_record_buffer);
                    *exit_call_result = EXIT_ABEND_VAL;
                    return;
                }
            }
            break;

        case EXIT_CALL_FATAL_ERROR:
            output_msg ("\nUser exit: EXIT_CALL_FATAL_ERROR.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500; /* Including null terminator */

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
                free (record);
                free (record_buffer);
                free (ascii_record_buffer);
                *exit_call_result = EXIT_ABEND_VAL;
                return;
            }

            output_msg ("Error information:\n"
                        "Error number: %ld.\n"
                        "Error message: %s.\n\n",
                        error_info.error_num,
                        error_info.error_msg);

            free (error_info.error_msg);
            close_callback();
            break;
    }

    free (record);
    free (record_buffer);
    free (ascii_record_buffer);
    *exit_call_result = EXIT_OK_VAL;
    fflush (stdout);
}

