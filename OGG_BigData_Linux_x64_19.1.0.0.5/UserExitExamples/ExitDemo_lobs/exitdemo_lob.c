/**************************************************************************
 Copyright (c) 1995, 2013, Oracle and/or its affiliates. All rights reserved. 
 
  Program description:

  User exit example for read access to LOB data.  This example uses the
  TSRSLOB and TTRGVAR tables, which can be created  using the
  demo_*_lob_*.sql scripts included with the ER release.  This same script
  will add a stored procedure, which when executed will Insert and Update a
  row of data.  Please be sure to ADD TRANDATA TSRSLOB for those databases which
  allow the GGSCI command, before executing.

  This example is a hypothetical business need where upon reading a record
  the LOB columns from the source table will be broken into manageable varchars
  for the target table.

  This example is coded to process as in either a log based Extract or Extract
  pump (when Mapped target)or Replicat.  Several callbacks allow for access by both
  name or by index, examples are shown for both.

  Callbacks that are exercised
    GET_TABLE_NAME              Used to get Name
    GET_ENV_VALUE               Used to get ENV values same as GGS @GETENV() function
    GET_OPERATION_TYPE          Used to test to only manipulate needed record types of
                                INSERT_VAL, UPDATE_COMP_SQL_VAL, UPDATE_COMP_PK_SQL_VAL

    GET_TABLE_COLUMN_COUNT      Used to get number of column, number of key columns
    GET_TABLE_METADATA          Used to get keys
    GET_COLUMN_NAME_FROM_INDEX  Used to get column name
    GET_COL_METADATA_FROM_INDEX Used to get column data type, sub data type, length
    GET_COL_METADATA_FROM_NAME  Used to get column data type, sub data type, length
    GET_COLUMN_VALUE_FROM_NAME  Used to get column value
    GET_COLUMN_VALUE_FROM_INDEX Used to get column value
    SET_COLUMN_VALUE_BY_INDEX   Used to set column value
    SET_COLUMN_VALUE_BY_NAME    Used to set column value
    GET_ERROR_INFO              Used to get error info
    GET_RECORD_BUFFER           Used to get record buffer (Error mode)
    OUTPUT_MESSAGE_TO_REPORT    Used to write messages to report file

    -------------------------------------------------------------------------------------------------------------------------
  Param file options with Extract using the User Exit

                                                 Optional
  EXTRACT EXTUSER                                EXTRACT EXTPUSER                                    REPLICAT REPUSER

  USERID XYZ, password XYZ                       PASSTHRU                                            USERID XYZ, password XYZ

  TARGETDEFS dirdef\lobs.def                     EXTTRAIL dirdat\xu                                  ASSUMETARGETDEFS

  CUSEREXIT cuserexit.dll CUSEREXIT              TABLE XYZ.TTRGVAR;                                  MAP XYZ.TTRGVAR, TARGET XYZ.TTRGVAR;

  EXTTRAIL dirdat\ux

  MAP XYZ.TSRSLOB, TARGET XYZ.TTRGVAR;

    -------------------------------------------------------------------------------------------------------------------------

  Param file options with Extract Pump using the User Exit


  EXTRACT EXTUSER                                EXTRACT EXTPUSER                                    REPLICAT REPUSER

  USERID XYZ, password XYZ                       SOURCEDEFS dirdef\lobs.def                          USERID XYZ, password XYZ
                                                 TARGETDEFS dirdef\lobs.def
  EXTTRAIL dirdat\ux                                                                                 ASSUMETARGETDEFS
                                                 EXTTRAIL dirdat\xu
  TABLE XYZ.TSRSLOB;                                                                                 MAP XYZ.TTRGVAR, TARGET XYZ.TTRGVAR;
                                                 cuserexit cuserexit.dll CUSEREXIT PARAMS "EXTPUMP"

                                                 MAP XYZ.TSRSLOB, TARGET XYZ.TTRGVAR;

                                                 -- The PARAMS is one of two options the
                                                 -- other being adding the checkpoint in GGSCI
                                                 -- with a DESCRIPTION of "EXTPUMP"

    -------------------------------------------------------------------------------------------------------------------------

  Param file options with Replicat using the User Exit

                                                 Optional
  EXTRACT EXTUSER                                EXTRACT EXTPUSER                                    REPLICAT REPUSER

  USERID XYZ, password XYZ                       PASSTHRU                                            USERID XYZ, password XYZ

                                                                                                     SOURCEDEFS dirdef\lobs.def
  EXTTRAIL dirdat\ux                             EXTTRAIL dirdat\xu                                  cuserexit cuserexit.dll CUSEREXIT

  TABLE XYZ.TSRSLOB;                             TABLE XYZ.TSRSLOB;                                  MAP XYZ.TSRSLOB, TARGET XYZ.TTRGVAR;


    -------------------------------------------------------------------------------------------------------------------------


***************************************************************************/

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

/*include of specfic DB native data types */

#if defined ORACLE
#include <oci.h>
#elif defined SYBASE
#include <cstypes.h>
#elif defined DB2
#include <sqlcli.h>   /*It is included for future refrences for other DB2 data types */
#elif defined SQLSERVER
#include <sql.h>
#include <sqlext.h>
#endif

/*The below #define is needed for DB2 LOB as SQL extended data types defined in sqlcli.h does not
  match for native data types */

#ifdef DB2
#define DB2_TYPE_BLOB                        11
#define DB2_TYPE_CLOB                        13
#endif

/* LOB extracted length differs for DB2 and other database */

#ifdef DB2
#define LOB_EXTRACT_LENGTH                    2000
#else
#define LOB_EXTRACT_LENGTH                    4000
#endif

typedef struct
{
    char *column_name;
    short column_index;
    short data_type;
    short gg_data_type;
    short gg_sub_data_type;
    unsigned short length;
    short treat_as_possible_lob;
} meta_data;

#define MAX_NAME 100

static short is_log_based_extract = 0;

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
  Output a message to the report file (or console).
***************************************************************************/
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

/***************************************************************************
  get table meta data
***************************************************************************/
short get_table_metadata(short source_or_target, char *tbl_name,
                         short *num_key_cols, short *num_cols,
                         meta_data **meta)
{
    env_value_def env_ptr;
    table_def table;
    table_metadata_def table_meta;
    col_metadata_def col_meta;
    meta_data *meta_ptr;
    short result_code;
    short i;


    /* Get table information */
    memset (&table, 0, sizeof(table_def));
    table.source_or_target = source_or_target;
    call_callback (GET_TABLE_COLUMN_COUNT, &table, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table column count.\n", result_code);
        return result_code;
    }

    /* Get table meta data information */
    table_meta.table_name = (char *)malloc(MAX_NAME);
    table_meta.max_name_length = MAX_NAME;
    table_meta.table_name[0] = 0;
    table_meta.key_columns = (short *)malloc((table.num_key_columns) * sizeof(short));
    table_meta.source_or_target = source_or_target;

    call_callback (GET_TABLE_METADATA, &table_meta, &result_code);
    if (result_code == EXIT_FN_RET_OK)
    {
        *num_cols = table_meta.num_columns;
        *num_key_cols = table_meta.num_key_columns;
        tbl_name = (char *) malloc (strlen (table_meta.table_name) + 1);
        strcpy (tbl_name, table_meta.table_name);

        *meta = (meta_data *) malloc ((*num_cols + 1) * sizeof(meta_data));

        col_meta.max_name_length = MAX_NAME;
        col_meta.column_name = (char *)malloc(col_meta.max_name_length);
        col_meta.source_or_target = source_or_target;

        env_ptr.max_length = MAX_NAME;
        env_ptr.buffer = (char *)malloc(env_ptr.max_length);

        for (i = 0, meta_ptr = *meta; i < *num_cols; i++, meta_ptr++)
        {
            /* Get column meta data information
               This can be obtained by the column index or the name, examples
               of both follow, and since this loop is running for num cols and
               using that as the index, when getting by name, the name must be
               obtained first */

            col_meta.column_name[0] = 0;
            env_ptr.buffer[0] = 0;

            if ((i % 2) == 0)
            {
                env_ptr.index = i;
                env_ptr.source_or_target  = source_or_target;
                call_callback (GET_COLUMN_NAME_FROM_INDEX, &env_ptr, &result_code);
                if (result_code == EXIT_FN_RET_OK)
                {
                    strcpy (col_meta.column_name, env_ptr.buffer);
                    call_callback (GET_COL_METADATA_FROM_NAME, &col_meta, &result_code);
                }
            }
            else
            {
                col_meta.column_index = i;
                call_callback (GET_COL_METADATA_FROM_INDEX, &col_meta, &result_code);
            }

            if (result_code == EXIT_FN_RET_OK)
            {
                memset (meta_ptr, 0, sizeof (meta_data));
                meta_ptr->column_name = (char *) malloc (strlen(col_meta.column_name) + 1);
                strcpy (meta_ptr->column_name, col_meta.column_name);
                meta_ptr->column_index = col_meta.column_index;
                meta_ptr->data_type = col_meta.native_data_type;
                meta_ptr->gg_data_type = col_meta.gg_data_type;
                meta_ptr->gg_sub_data_type = col_meta.gg_sub_data_type;
                meta_ptr->length = (unsigned short )col_meta.length;

                /* The use of sourcedef will cause the GG data types and sub types to
                   be populated, these are generic types not the native types of the
                   perticular DB. */
                if (!is_log_based_extract)
                {
                    if (meta_ptr->length >= LOB_EXTRACT_LENGTH    &&
                        meta_ptr->gg_data_type == SQLDT_ASCII_V  &&
                       (meta_ptr->gg_sub_data_type == SQLSDT_DEFAULT ||
                        meta_ptr->gg_sub_data_type == SQLSDT_BINARY))
                    {
                        meta_ptr->treat_as_possible_lob = 1;
                    }
                }
#ifdef SYBASE
                else
                if (meta_ptr->data_type == CS_TEXT_TYPE ||
                    meta_ptr->data_type == CS_IMAGE_TYPE)
                {
                    meta_ptr->treat_as_possible_lob = 1;
                }

#elif defined DB2
                else
                if (meta_ptr->data_type == DB2_TYPE_BLOB ||
                    meta_ptr->data_type == DB2_TYPE_CLOB)
                {
                    meta_ptr->treat_as_possible_lob = 1;
                }

#elif defined SQLSERVER
                else
                if (meta_ptr->data_type == SQL_LONGVARCHAR ||
                    meta_ptr->data_type == SQL_LONGVARBINARY )
                {
                    meta_ptr->treat_as_possible_lob = 1;
                }

#elif defined ORACLE
                else
                if (meta_ptr->data_type == SQLT_CLOB ||
                    meta_ptr->data_type == SQLT_BLOB)
                {
                    meta_ptr->treat_as_possible_lob = 1;
                }
#endif
            }
            else
                output_msg ("Error (%hd) retrieving column metadata.\n", result_code);
        }
        memset (meta_ptr, -1, sizeof (meta_data)); /* set as ending point for later testing */
    }
    else
        output_msg ("Error (%hd) retrieving table metadata.\n", result_code);

    free (env_ptr.buffer);
    free (col_meta.column_name);
    free (table_meta.table_name);
    free (table_meta.key_columns);

    return result_code;
}
/***************************************************************************
  set target varchar column column up to 2000 bytes at a time
***************************************************************************/
short set_targ_var_column (column_def *column, short by_idx)
{
    short result_code;

    if (by_idx)
    {
        call_callback (SET_COLUMN_VALUE_BY_INDEX, column, &result_code);
    }
    else
    {
        call_callback (SET_COLUMN_VALUE_BY_NAME, column, &result_code);
    }

    return result_code;
}
/***************************************************************************
  get lob data for a column up to 2000 bytes at a time
***************************************************************************/
short get_src_lob_column (column_def *column, short by_idx)
{
    short result_code;

    if (by_idx)
    {
        call_callback (GET_COLUMN_VALUE_FROM_INDEX, column, &result_code);
    }
    else
    {
        call_callback (GET_COLUMN_VALUE_FROM_NAME, column, &result_code);
    }

    return result_code;
}

/***************************************************************************
  Process a single record by splitting up a LOB column to multiple varchars
***************************************************************************/
short process_record (void)
{
    char *src_tbl_name = NULL;
    char *trg_tbl_name = NULL;
    column_def *column = NULL;
    meta_data *src_meta = NULL;
    meta_data *smeta_ptr = NULL;
    meta_data *trg_meta = NULL;
    meta_data *tmeta_ptr = NULL;
    record_def *record = NULL;
    unsigned short buf_size = 0;
    short src_num_key_cols = 0;
    short trg_num_key_cols = 0;
    short src_num_cols = 0;
    short trg_num_cols = 0;
    short result_code = EXIT_FN_RET_OK;

    record = (record_def *) malloc(sizeof (record_def));
    record = memset (record, 0, sizeof(record));
    record->buffer = (char *) malloc(sizeof(exit_rec_buf_def));
    record->source_or_target = EXIT_FN_SOURCE_VAL;

    column = (column_def *) malloc(sizeof (column_def));
    memset (column, 0, sizeof(column_def));
    column->column_name = (char *) malloc (MAX_NAME);
    column->column_name[0] = 0;

    /* Get op type information
       Only need to read LOBs on Insert Updates */
    call_callback (GET_OPERATION_TYPE, record, &result_code);
    if (result_code == EXIT_FN_RET_OK)
    {
        if (INSERT_VAL                == record->io_type ||
            UPDATE_COMP_SQL_VAL       == record->io_type ||
            UPDATE_COMP_PK_SQL_VAL    == record->io_type)
        {
            output_msg ("\nUser exit: processing record for I/O type %hd.\n", record->io_type );

             /* Get table specs for source */
            result_code = get_table_metadata(EXIT_FN_SOURCE_VAL, src_tbl_name,
                                             &src_num_key_cols, &src_num_cols, &src_meta);
            if (result_code == EXIT_FN_RET_OK)
            {
                /* Get table specs for target */
                result_code = get_table_metadata(EXIT_FN_TARGET_VAL, trg_tbl_name,
                                                 &trg_num_key_cols, &trg_num_cols, &trg_meta);
                if (result_code == EXIT_FN_RET_OK)
                {
                    /* for all our source columns loop */
                    for (smeta_ptr = src_meta; smeta_ptr->column_index != -1; smeta_ptr++)
                    {
                        /* we only care to read LOB columns to MAP to a target of varchars */
                        if (smeta_ptr->treat_as_possible_lob)
                        {
                            /* Our hypothetical business rules is to match the first few columns of a name to the starting
                               point of a matching target ie LOB1_CLOB will Map to LOB1_VCHAR0 thru LOB1_VCHAR9 */
                            for (tmeta_ptr = trg_meta; strncmp (tmeta_ptr->column_name, smeta_ptr->column_name, 5); tmeta_ptr++);

                            /* LOBs which are not more then 4K maybe stored by GGS in such a way as to have always been
                               available from the "GET" column value callbacks.  For those over that limit multiple callbacks
                               are required to retrive all the data.  */

                            column->more_lob_data = 0;

                            for (;CHAR_NO_VAL != column->more_lob_data && !result_code; tmeta_ptr++)
                            {
                                if (buf_size == 0)
                                {
                                    column->column_value = (char *) malloc(tmeta_ptr->length);
                                    buf_size = tmeta_ptr->length;
                                }
                                else
                                if (buf_size < tmeta_ptr->length)
                                {
                                    column->column_value = (char *) realloc (column->column_value, tmeta_ptr->length);
                                    buf_size = tmeta_ptr->length;
                                }
                                column->max_value_length = tmeta_ptr->length;
                                column->source_or_target = EXIT_FN_SOURCE_VAL;
                                column->requesting_before_after_ind = AFTER_IMAGE_VAL;
                                column->column_value_mode = EXIT_FN_INTERNAL_FORMAT;

                                if (tmeta_ptr->column_index % 2 == 0)
                                {
                                    column->column_index = smeta_ptr->column_index;
                                }
                                else
                                {
                                    strcpy (column->column_name, smeta_ptr->column_name);
                                }

                                result_code = get_src_lob_column (column, (tmeta_ptr->column_index % 2 == 0));
                                if (result_code == EXIT_FN_RET_OK)
                                {
                                    if (-1 == tmeta_ptr->column_index)
                                        output_msg ("Error (%hd) bad column index for write.\n", result_code);
                                    else
                                    if (column->actual_value_length > 0)
                                    {
                                        column->source_or_target = EXIT_FN_TARGET_VAL;

                                        if (tmeta_ptr->column_index % 2 == 0)
                                        {
                                            column->column_index = tmeta_ptr->column_index;
                                        }
                                        else
                                        {
                                            strcpy (column->column_name, tmeta_ptr->column_name);
                                        }

                                        result_code = set_targ_var_column (column, (tmeta_ptr->column_index % 2 == 0));
                                        if (result_code!= EXIT_FN_RET_OK)
                                        {
                                            output_msg ("Error (%hd) setting target column value.\n", result_code);
                                            return result_code;
                                        }
                                    }
                                }
                                else
                                if (result_code == EXIT_FN_RET_COLUMN_NOT_FOUND)  /* Not found should be ok, compressed update */
                                    result_code = EXIT_FN_RET_OK;
                                else
                                {
                                    output_msg ("Error (%hd) getting source column value.\n", result_code);
                                    return result_code;
                                }

                                /* The more_lob_data flag will be set to CHAR_NO_VAL or CHAR_YES_VAL
                                   when data is over the 4K limit.  It will not be set when less then the limit. */
                                if (0 == column->more_lob_data)
                                {
                                    column->more_lob_data = CHAR_NO_VAL;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
            output_msg ("\nUser exit: skipping record for I/O type %hd.\n", record->io_type );
    }
    else
        output_msg ("Error (%hd) retrieving record operation type.\n", result_code);

    for (smeta_ptr = src_meta; -1 != smeta_ptr->column_index; smeta_ptr++)
        free(smeta_ptr->column_name);
    for (tmeta_ptr = trg_meta; -1 != tmeta_ptr->column_index; tmeta_ptr++)
        free(tmeta_ptr->column_name);

    free (src_meta);
    free (trg_meta);
    free (src_tbl_name);
    free (trg_tbl_name);
    free (column->column_value);
    free (column->column_name);
    free (column);
    free (record->buffer);
    free (record);

    return result_code;
}

/**************************************************************************
  ER user exit object called from various user exit points in extract and
  replicat.
***************************************************************************/
#ifdef WIN32
__declspec(dllexport)
void CUSEREXIT (exit_call_type_def exit_call_type,
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
    getenv_value_def ggenv_ptr;
    env_value_def env_ptr;
    char *tbl;

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

    *exit_call_result = EXIT_OK_VAL;

    switch (exit_call_type)
    {
        case EXIT_CALL_START:
            output_msg ("\nUser exit: EXIT_CALL_START.  Called from program: %s\n",
                        exit_params->program_name);

            /* PARAMS option on cuserexit parameter line
               cuserexit cuserexit.dll CUSEREXIT PARAMS "EXTPUMP" */
            if (exit_params->function_param[0] != 0)
                output_msg ("\nUser exit: Startup message string %s\n",
                            exit_params->function_param);


            /* Get type of processes Extract/Replicat, same as @getenv() */
            memset (&ggenv_ptr, 0, sizeof(getenv_value_def));
            ggenv_ptr.max_return_length = 20;
            ggenv_ptr.return_value = (char *) malloc (ggenv_ptr.max_return_length);
            ggenv_ptr.information_type = (char *) malloc (strlen ("GGENVIRONMENT") + 1);
            ggenv_ptr.env_value_name = (char *) malloc (strlen ("GROUPTYPE") + 1);
            strcpy (ggenv_ptr.information_type, "GGENVIRONMENT");
            strcpy (ggenv_ptr.env_value_name, "GROUPTYPE");
            call_callback (GET_ENV_VALUE, &ggenv_ptr, &result_code);
            if (EXIT_FN_RET_OK == result_code)
            {
                if (!strcmp (ggenv_ptr.return_value, "EXTRACT"))
                {
                    is_log_based_extract = 1;
                    /* if extract and does not have PARAMS for pump we will expect log based and Native data types */
                    if (exit_params->function_param[0] != 0 &&            /* param exists */
                       !strcmp (exit_params->function_param, "EXTPUMP"))  /* match for pump */
                        is_log_based_extract = 0;
                    else
                    {
                        /* Get description from group CHECKPOINT, GGSCI ADD/ALTER with DESC, same as @getenv() */
                        ggenv_ptr.env_value_name = (char *) realloc (ggenv_ptr.env_value_name, strlen ("GROUPDESCRIPTION") + 1);
                        strcpy (ggenv_ptr.env_value_name, "GROUPDESCRIPTION");
                        call_callback (GET_ENV_VALUE, &ggenv_ptr, &result_code);
                        if (EXIT_FN_RET_OK == result_code &&            /* description exists */
                           !strcmp (ggenv_ptr.return_value, "EXTPUMP")) /* match for pump */
                        {
                            is_log_based_extract = 0;
                        }
                    }
                }
            }

            free(ggenv_ptr.return_value);
            free(ggenv_ptr.information_type);
            free(ggenv_ptr.env_value_name);
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

            /* get table name to not work on any but TSRSLOB as source */
            env_ptr.max_length = MAX_NAME;
            env_ptr.buffer = (char *)malloc(env_ptr.max_length);
            env_ptr.buffer[0] = 0;
            env_ptr.source_or_target  = EXIT_FN_SOURCE_VAL;
            call_callback (GET_TABLE_NAME, &env_ptr, &result_code);
            if (result_code == EXIT_FN_RET_OK)
            {
                /* table names are generally in format of [CATALOG.]OWNER.TBL,
                   we don't always know your owner */
                tbl = strrchr (env_ptr.buffer, '.') + 1;
                if (!strcmp (tbl, "TSRSLOB"))
                    result_code = process_record();
            }
            else
            {
                output_msg ("Error (%hd) retrieving error table name.\n", result_code);
            }

            free (env_ptr.buffer);

            if (result_code != EXIT_FN_RET_OK)
                *exit_call_result = EXIT_ABEND_VAL;

            break;

        case EXIT_CALL_DISCARD_ASCII_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_DISCARD_ASCII_RECORD.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500;

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

            break;

        case EXIT_CALL_DISCARD_RECORD:
            output_msg ("\nUser exit: EXIT_CALL_DISCARD_RECORD.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500;

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

            break;

        case EXIT_CALL_FATAL_ERROR:
            output_msg ("\nUser exit: EXIT_CALL_FATAL_ERROR.\n");

            memset (&error_info, 0, sizeof(error_info));
            error_info.error_msg = (char *)malloc(500);
            error_info.max_length = 500;

            call_callback (GET_ERROR_INFO, &error_info, &result_code);
            if (result_code != EXIT_FN_RET_OK)
            {
                output_msg ("Error (%hd) retrieving error information.\n", result_code);
                free (error_info.error_msg);
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

    fflush (stdout);
}

