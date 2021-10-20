/**************************************************************************
 Copyright (c) 1995, 2013, Oracle and/or its affiliates. All rights reserved. 
 
  Program description:

  This is a User exit example for access to "Primary Key Updates" and "before images"
  from Updates.  This example uses the PK_BF_TIMESRC and PK_BF_TIMETRG tables,
  which can be created using the demo_*pk_befores_create.sql scripts included
  with the ER release.  This same script will add a stored procedure, which when
  executed will Insert and Update a row of data.  Please be sure to "ADD TRANDATA" for
  the PK_BF_TIMESRC table for those databases which allow the GGSCI command, before executing.

  This example is a hypothetical business solution when a target database does not
  allow the Primary Key Updates.  Therefore, when reading a PKUpdate record, it will
  change this operation into two separate records of Delete and Insert.  While this is not
  necessary for most databases, for those databases which PKUpdates are not allow
  this example is not necessarily a recommended solution to this problem. It is only
  an example to demonstrate access to PKupdate records.  This example will work for Extract
  and Replicat.

  This example will also show access to the before images of an Update.  These records
  are typically included with the use of GETUPDATEBEFORES or the "cuserexit" option of
  INCLUDEUPDATEBEFORES.  This is intended as a hypothetical business solution of conflict
  detection base on a specific column. The is an example which can only function in Replicat.

  The examples when used in a Replicat will also make use of the SQLEXEC functionality.
  Several user exit callbacks allow for access by both name or by index, examples are shown for both.

  Callbacks that are exercised
    GET_TABLE_NAME              Used to get Name
    GET_ENV_VALUE               Used to get ENV values same as GGS @GETENV() function
    GET_OPERATION_TYPE          Used to test to only manipulate needed record types of
                                UPDATE_COMP_SQL_VAL, UPDATE_COMP_PK_SQL_VAL
    SET_OPERATION_TYPE          Used to change the current IO type
    GET_TABLE_COLUMN_COUNT      Used to get number of column, number of key columns
    GET_TABLE_METADATA          Used to get keys
    GET_BEFORE_AFTER_IND        Used to get the type of record image
    GET_COLUMN_NAME_FROM_INDEX  Used to get column name
    GET_COLUMN_VALUE_FROM_NAME  Used to get column value
    GET_COLUMN_VALUE_FROM_INDEX Used to get column value
    SET_COLUMN_VALUE_BY_INDEX   Used to set column value
    SET_COLUMN_VALUE_BY_NAME    Used to set column value
    GET_ERROR_INFO              Used to get error info
    OUTPUT_MESSAGE_TO_REPORT    Used to write messages to report file

    -------------------------------------------------------------------------------------------------------------------------
  Param file options with Extract using the User Exit (only for PKUpdate processing)

                                                 Optional
  EXTRACT EXTPKUP                                EXTRACT EXTPUMP

  USERID XYZ, password XYZ                       PASSTHRU

  EXTTRAIL dirdat\ux                             EXTTRAIL dirdat\xu

  GETUPDATEBEFORES                               TABLE XYZ.*;

  --Next option is DB specific,
  --         this is for Oracle
  -- if the Extract is processing
  -- PKUpdates it must have a full
  -- image for Insert
  FETCHOPTIONS FETCHPKUPDATECOLS

  CUSEREXIT cuserexit.dll CUSEREXIT

  TABLE XYZ.PK_BF_TIMESRC;

    -------------------------------------------------------------------------------------------------------------------------

  Param file options with Extract Pump using the User Exit (only for PKUpdate processing)

  EXTRACT EXTPKUP                                EXTRACT EXTPUMP

  USERID XYZ, password XYZ                       SOURCEDEFS dirdef\pk.def

  EXTTRAIL dirdat\ux                             EXTTRAIL dirdat\xu

  GETUPDATEBEFORES                               CUSEREXIT cuserexit.dll CUSEREXIT, INCLUDEUPDATEBEFORES

  -- if the Extract is processing                TABLE XYZ.*;
  -- PKUpdates it must have a full
  -- image for Insert
  -- Next option is DB specific,
  -- plese check the manual as needed.
  FETCHOPTIONS FETCHPKUPDATECOLS

  TABLE XYZ.PK_BF_TIMESRC;

  ------------------------------
  These Replicat params are for either of the first two examples with Extract processing the PKUpdate
  REPLICAT REPPKBF

  DISCARDFILE dirrpt\repdisc.txt, purge

  USERID XYZ, password XYZ

  ALLOWDUPTARGETMAP

  ASSUMETARGETDEFS

  CUSEREXIT cuserexit.dll CUSEREXIT, INCLUDEUPDATEBEFORES

  IGNOREINSERTS
  IGNOREDELETES

  MAP XYZ.PK_BF_TIMESRC, TARGET XYZ.PK_BF_TIMETRG,
    sqlexec (ID fetchUP, on update, trace all, error fatal,
          QUERY " select LAST_UPDATE_DATETIME from XYZ.PK_BF_TIMETRG "
                " where key_num = :key_num and cat_code = :cat_code and first_num_data_col = :first_num_data_col ",
          params (key_num = before.key_num, cat_code = before.cat_code, first_num_data_col = before.first_num_data_col) ) ,

    colmap (usedefaults,
            LAST_UPDATE_DATETIME = @date ("YYYY-MM-DD:HH:MI:SS", "YYYY-MM-DD HH:MI:SS",
                                                                  fetchUP.LAST_UPDATE_DATETIME)
           ),

   filter ( @streq ( "SQL COMPUPDATE",  @getenv ("GGHEADER", "OPTYPE")   )  ) ;


  IGNOREUPDATES
  GETDELETES
  GETINSERTS

  MAP XYZ.PK_BF_TIMESRC, TARGET XYZ.PK_BF_TIMETRG;

    -------------------------------------------------------------------------------------------------------------------------

  Param files options for Replicat using the User Exit.

  EXTRACT EXTPKUP

  USERID XYZ, password XYZ

  EXTTRAIL dirdat\ux

  GETUPDATEBEFORES

  TABLE XYZ.PK_BF_TIMESRC;

  ------------------------------

  REPLICAT REPPKBF

  DISCARDFILE dirrpt\repdisc.txt, purge

  USERID XYZ, password XYZ

  ALLOWDUPTARGETMAP

  ASSUMETARGETDEFS

  CUSEREXIT cuserexit.dll CUSEREXIT, INCLUDEUPDATEBEFORES

  IGNOREINSERTS
  IGNOREDELETES

  MAP XYZ.PK_BF_TIMESRC, TARGET XYZ.PK_BF_TIMETRG,
    sqlexec (ID fetchPK, on update, trace all, error fatal,
          QUERY " select "
                " FIRST_VAR_DATA_COL , "
                " SECOND_VAR_DATA_COL, "
                " SECOND_NUM_DATA_COL, "
                " LAST_UPDATE_DATETIME "
                " FROM XYZ.PK_BF_TIMETRG "
                " where key_num = :key_num and cat_code = :cat_code and first_num_data_col = :first_num_data_col ",
          params (key_num = before.key_num, cat_code = before.cat_code, first_num_data_col = before.first_num_data_col) ) ,

    colmap (usedefaults,
            FIRST_VAR_DATA_COL  = fetchPK.FIRST_VAR_DATA_COL
            SECOND_VAR_DATA_COL = fetchPK.SECOND_VAR_DATA_COL
            SECOND_NUM_DATA_COL = fetchPK.SECOND_NUM_DATA_COL

            LAST_UPDATE_DATETIME = @date ("YYYY-MM-DD:HH:MI:SS", "YYYY-MM-DD HH:MI:SS",
                                                                 fetchPK.LAST_UPDATE_DATETIME)
           ) ,

   filter ( @streq ( "PK UPDATE",  @getenv ("GGHEADER", "OPTYPE")   )  ) ;


  MAP XYZ.PK_BF_TIMESRC, TARGET XYZ.PK_BF_TIMETRG,
    sqlexec (ID fetchUP, on update, trace all, error fatal,
          QUERY " select LAST_UPDATE_DATETIME from XYZ.PK_BF_TIMETRG "
                " where key_num = :key_num and cat_code = :cat_code and first_num_data_col = :first_num_data_col ",
          params (key_num = before.key_num, cat_code = before.cat_code, first_num_data_col = before.first_num_data_col) ) ,

    colmap (usedefaults,
            LAST_UPDATE_DATETIME = @date ("YYYY-MM-DD:HH:MI:SS", "YYYY-MM-DD HH:MI:SS",
                                                                  fetchUP.LAST_UPDATE_DATETIME)
           ),

   filter ( @streq ( "SQL COMPUPDATE",  @getenv ("GGHEADER", "OPTYPE")   )  ) ;


  IGNOREUPDATES
  GETDELETES
  GETINSERTS

  MAP XYZ.PK_BF_TIMESRC, TARGET XYZ.PK_BF_TIMETRG;

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
  #define I64_FMT  "%I64d"
  #include <windows.h>
  #if (_MSC_VER>=1400)
    #pragma warning(disable:4018)
    #pragma warning(disable:4996)
  #endif
#else


  #if defined(__linux__)
    #include <stdint.h>
    #include <stdarg.h>
    #define I64_FMT  "%Ld"
  #endif

  #if defined(__MVS__)
    #include <stdarg.h>
    #include <varargs.h>
    #include <inttypes.h>
    #define I64_FMT  "%lld"
  #endif

  #if !defined(__MVS__) && !defined(__linux__)
    #include <stdarg.h>
    #define I64_FMT  "%lld"
  #endif

   #include <string.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <ctype.h>
#endif

#include "usrdecs.h"

static short set_delete = 1;
static short is_extract = 0;

#define MAX_NAME 100
#define MAX_LENGTH 4000
#define DEMO_TBL_NAME "PK_BF_TIMESRC"
#define CONFLICT_COLUMN_NAME "LAST_UPDATE_DATETIME"

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

short IsPrint(int c)
{
    if (c != EOF) c = (unsigned char)c;
    return isprint(c) ? 1 : 0;
}

/***************************************************************************
  Display record information.
***************************************************************************/
short display_record (short source_or_target,
                      short ascii_or_internal,
                      char before_after)
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

    /* Get table name */
    memset (&env_value, 0, sizeof(env_value_def));
    env_value.source_or_target = source_or_target;
    env_value.max_length = MAX_NAME;
    env_value.buffer = (char *)malloc(env_value.max_length);

    call_callback (GET_TABLE_NAME, &env_value, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table name.\n", result_code);
        free (env_value.buffer);
        return result_code;
    }

    /* Get table column information */
    table.source_or_target = source_or_target;

    call_callback (GET_TABLE_COLUMN_COUNT, &table, &result_code);
    if (result_code != EXIT_FN_RET_OK)
    {
        output_msg ("Error (%hd) retrieving table column count.\n", result_code);
        free (env_value.buffer);
        return result_code;
    }

    output_msg ("Table %.*s has %hd columns.\n",
                env_value.actual_length, env_value.buffer,
                table.num_columns);

    memset(&column, 0, sizeof (column_def));
    column.source_or_target = source_or_target;
    column.column_value_mode = ascii_or_internal;
    column.max_value_length = MAX_LENGTH;
    column.column_value = (char*)malloc(column.max_value_length);
    column.requesting_before_after_ind = before_after;

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

        output_msg ("Column index (%d) = %.*s.\n",
                    i, env_value.actual_length, env_value.buffer);

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

/*****************************************************************************
*  Replaces target columns with newer columns values from source after       *
*  image if present. If a newer col from source is not present from          *
*  a compressed update, the target will retain the original value.           *
*****************************************************************************/
short replace_cols_from_after_image(void)
{
    table_def table;
    column_def column;
    col_metadata_def col_meta;
    int64_t tmp_num,
            dec,
            intp;
    int32_t pw10 = 1;
    short scale;
    short i;
    short result_code = EXIT_FN_RET_OK;

    table.source_or_target = EXIT_FN_SOURCE_VAL;
    /* Need to know number columns to replace as many as needed */
    call_callback (GET_TABLE_COLUMN_COUNT, &table, &result_code);
    if (result_code == EXIT_FN_RET_OK)
    {
        memset(&column, 0, sizeof (column_def));
        column.max_value_length = MAX_LENGTH;
        column.column_value = (char*)malloc(column.max_value_length);
        column.requesting_before_after_ind = AFTER_IMAGE_VAL;
        col_meta.max_name_length = MAX_NAME;
        col_meta.column_name = (char *)malloc(col_meta.max_name_length);

        /* loop thru all columns */
        for (i = 0; i < table.num_columns; i++)
        {
            col_meta.source_or_target = EXIT_FN_TARGET_VAL;
            col_meta.column_name[0] = 0;
            col_meta.column_index = i;

            /* getting column meta data, need to know which key columns to skip as well as the
            one column data type which need a bit of help to properly handle */
            call_callback (GET_COL_METADATA_FROM_INDEX, &col_meta, &result_code);
            if (result_code == EXIT_FN_RET_OK)
            {
                /* if it's not a key which is always present with trandata) */
                if (!col_meta.is_part_of_key)
                {
                    column.column_index = i;
                    column.source_or_target = EXIT_FN_SOURCE_VAL;
                    column.column_value_mode = EXIT_FN_INTERNAL_FORMAT;
                    /* get the source column value */
                    call_callback (GET_COLUMN_VALUE_FROM_INDEX, &column, &result_code);

                    if (result_code == EXIT_FN_RET_OK)
                    {
                        /* if column result is not NULL then we have new data from after image */
                        if (!column.null_value &&
                            column.actual_value_length >= 0)
                        {
                            column.column_value[column.actual_value_length] = 0;
                            column.source_or_target = EXIT_FN_TARGET_VAL;
                            column.column_value_mode = EXIT_FN_ASCII_FORMAT;

                            /* the column SECOND_NUM_DATA_COL can be binary as a number say in Oracle
                               and needs to be written to the target as ascii.  In this case the native and
                               ggs data types are both the target native values, to be generic the
                               sub data type is my clue this needs to be dealt with
                               ie..  a value of 0000001b5f0 hex or 112112 ascii, but to write
                               as ascii to the target with scale should be 1121.12  */
                            if (result_code == EXIT_FN_RET_OK &&
                                col_meta.gg_sub_data_type == SQLSDT_FIXED_PREC)
                            {
                                memcpy (&tmp_num, column.column_value, column.actual_value_length);
                                for (scale = col_meta.scale; scale > 0; scale--, pw10 *= 10);
                                intp = tmp_num / pw10;
                                dec = tmp_num % pw10;

                                /* you may need to add a format define depending on your machine/OS */
                                sprintf (column.column_value, I64_FMT"."I64_FMT, intp, dec);
                            }

                            /* set the target with the new col value, overlaying the fetched data from sqlexec */
                            call_callback (SET_COLUMN_VALUE_BY_INDEX, &column, &result_code);
                            if (result_code != EXIT_FN_RET_OK)
                                break;
                        }
                    }
                    else
                        output_msg ("Error (%hd) getting source column value for table %s, index %hd.\n", result_code,
                                                 DEMO_TBL_NAME, column.column_index);
                }
            }
            else
                output_msg ("Error (%hd) retrieving column meta data for %s.\n", result_code, DEMO_TBL_NAME);
        }
    }
    else
        output_msg ("Error (%hd) retrieving table column count for %s.\n", result_code, DEMO_TBL_NAME);

    free (col_meta.column_name);
    free (column.column_value);

    return result_code;
}

/***************************************************************************
  Changes the IO type for a PKUpdate (115) into an Delete and then insert.
***************************************************************************/
short process_pk_as_delete_insert (record_def *record, char *more_recs)
{
    short result_code = EXIT_FN_RET_OK;

    output_msg ("\nUser exit: processing record for I/O type %hd.\n", record->io_type );

    if (record->mapped)
    {
        record->source_or_target = EXIT_FN_TARGET_VAL;
    }
    else
    {
        record->source_or_target = EXIT_FN_SOURCE_VAL;
    }

    if (set_delete)
    {
        /* display the before image portion of the PKupdate */
        display_record (record->source_or_target, EXIT_FN_INTERNAL_FORMAT, BEFORE_IMAGE_VAL);
        output_msg ("\nUser exit: Adding Delete Record\n");
        record->io_type = DELETE_VAL;
        *more_recs = CHAR_YES_VAL;
        set_delete = 0;
    }
    else
    {
        /* display the after image portion of the PKupdate */
        display_record (record->source_or_target, EXIT_FN_ASCII_FORMAT, AFTER_IMAGE_VAL);
        output_msg ("\nUser exit: Adding Insert Record\n");
        record->io_type = INSERT_VAL;
        set_delete = 1;
        *more_recs = CHAR_NO_VAL;
    }

    /* change the PKUPDATE IO/Type from 115 to Delete or Insert */
    call_callback (SET_OPERATION_TYPE, record, &result_code);

    if (result_code == EXIT_FN_RET_OK)
    {
        /* if this is a replicat, then we can use the sqlexec to get target values,
           if this is extract, then the update needed to be in a full record image
           and nothing to be replaced. */
        if (record->io_type == INSERT_VAL &&
            !is_extract)
        {
            result_code = replace_cols_from_after_image();
            if (result_code == EXIT_FN_RET_OK)
            {
                display_record (record->source_or_target, EXIT_FN_ASCII_FORMAT, AFTER_IMAGE_VAL);
            }
        }
    }
    else
        output_msg ("Error (%hd) retrieving operation type.\n", result_code);

    return result_code;
}
/***************************************************************************
  Tests a data column to see if the target record is more current then the
  source.
***************************************************************************/
short check_conflict_resolution(record_def *record)
{
    column_def *column = NULL;
    char target_fetched_ts[30];
    char source_before_ts[30];
    char source_after_ts[30];
    short result_code = EXIT_FN_RET_OK;

    /* I only want to work on After images when process_record happends,
       before images may or may not be in the trail depending on use of
       GETUPDATEBEFORES vs INCLUDEUPDATEBEFORES */
    call_callback (GET_BEFORE_AFTER_IND, record, &result_code);
    if (result_code == EXIT_FN_RET_OK &&
        record->before_after_ind == AFTER_IMAGE_VAL)
    {
        column = (column_def *) malloc(sizeof (column_def));
        memset (column, 0, sizeof(column_def));
        column->max_value_length = MAX_NAME;
        column->column_value = (char *) malloc(column->max_value_length);

        column->column_name = (char *) malloc (strlen (CONFLICT_COLUMN_NAME) + 1);
        strcpy (column->column_name, CONFLICT_COLUMN_NAME);
        column->source_or_target = EXIT_FN_TARGET_VAL;
        column->column_value_mode = EXIT_FN_ASCII_FORMAT;
        column->requesting_before_after_ind = AFTER_IMAGE_VAL;

        /* Get the value for the target col we want to test */
        call_callback (GET_COLUMN_VALUE_FROM_NAME, column, &result_code);
        if (result_code == EXIT_FN_RET_OK)
        {
            strcpy (target_fetched_ts, column->column_value);

            column->source_or_target = EXIT_FN_SOURCE_VAL;
            column->column_value_mode = EXIT_FN_INTERNAL_FORMAT;
            column->requesting_before_after_ind = BEFORE_IMAGE_VAL;

            /* get the before image from the source to test with */
            call_callback (GET_COLUMN_VALUE_FROM_NAME, column, &result_code);
            if (result_code == EXIT_FN_RET_OK)
            {
                memcpy (source_before_ts, column->column_value, column->actual_value_length);
                /* I asked for the source column in the internal format, I expect the buffer not to be
                   terminated. */
                source_before_ts[column->actual_value_length] = 0;

                /* if the source before image value for this col is older then the target value
                   then we don't want to replicate this record otherwise we do, this is an arbitrary
                   business rule to show getting a before value col from saved before image as well
                   as getting col value for a sqlexec fetched value.  */
                if ((memcmp (source_before_ts, target_fetched_ts, strlen (source_before_ts))) < 0)
                {
                    result_code = EXIT_IGNORE_VAL;
                    output_msg ("\nUser exit: conflict... record is newer on target, skipping Update.");
                }
                else
                {
                    output_msg ("\nUser exit: passed conflict.. processing update.\n");

                    /* in the case of a PKUpdate, columns will be dealt with in that section of code,
                       this was just to valiadate the record was good to go */
                    if (UPDATE_COMP_PK_SQL_VAL == record->io_type)
                    {
                        free (column->column_value);
                        free (column->column_name);
                        free (column);
                        return result_code;
                    }

                    column->source_or_target = EXIT_FN_SOURCE_VAL;
                    column->column_value_mode = EXIT_FN_INTERNAL_FORMAT;
                    column->requesting_before_after_ind = AFTER_IMAGE_VAL;
                    /* get the new after image value from the source to set the target correctly */
                    call_callback (GET_COLUMN_VALUE_FROM_NAME, column, &result_code);

                    if (result_code == EXIT_FN_RET_OK)
                    {
                        memcpy (source_after_ts, column->column_value, column->actual_value_length);
                        source_after_ts[column->actual_value_length] = 0;

                        column->source_or_target = EXIT_FN_TARGET_VAL;
                        column->column_value_mode = EXIT_FN_ASCII_FORMAT;
                        strcpy (column->column_value, source_after_ts);
                        /* set the target with the new after image col value */
                        call_callback (SET_COLUMN_VALUE_BY_NAME, column, &result_code);
                    }
                    else
                        output_msg ("Error (%hd) getting source \"before\" column value for %s.\n", result_code, column->column_name);
                }
            }
            else
                output_msg ("Error (%hd) getting target \"after\" column value for %s.\n", result_code, column->column_name);
        }

        free (column->column_value);
        free (column->column_name);
        free (column);
    }
    else
    if (result_code != EXIT_FN_RET_OK)
        output_msg ("Error (%hd) getting before or after indicator\n", result_code);

    return result_code;
}
/***************************************************************************
  Makes determination if the record is to be processed by this exit.
***************************************************************************/
short process_record(char *more_recs)
{
    env_value_def env_value;
    record_def *record = NULL;
    char *tbl_name;
    short result_code;

    /* Get table name */
    memset (&env_value, 0, sizeof(env_value_def));
    env_value.max_length = MAX_NAME;
    env_value.source_or_target = EXIT_FN_SOURCE_VAL;
    env_value.buffer = (char *)malloc(env_value.max_length);

    call_callback (GET_TABLE_NAME, &env_value, &result_code);
    if (result_code == EXIT_FN_RET_OK)
    {
        /* table names are generally in format of [CATALOG.]OWNER.TBL,
         we don't always know your owner, you should remove this and compare the owner.table name */
        tbl_name = strrchr (env_value.buffer, '.'); /* move passed owner */
        if (tbl_name == NULL)
            tbl_name = env_value.buffer;
        else
            tbl_name++; /* move passed . */

        /* Just checking to be our test table for this exit example */
        if (!strcmp (tbl_name, DEMO_TBL_NAME))
        {
            record = (record_def *) malloc(sizeof (record_def));
            record = memset (record, 0, sizeof(record));
            record->buffer = (char *) malloc(sizeof(exit_rec_buf_def));
            record->source_or_target = EXIT_FN_SOURCE_VAL;

            /* only need to process for updates and pkupdates */
            call_callback (GET_OPERATION_TYPE, record, &result_code);
            if (result_code == EXIT_FN_RET_OK)
            {
                if (UPDATE_COMP_PK_SQL_VAL == record->io_type)
                {
                    /* can only fetch target with SQLEXEC from Replciat
                       and I only need to check conflict once and the same
                       record will be replyed 2 times for Delete and Insert */
                    if (!is_extract && set_delete)
                    {
                        result_code = check_conflict_resolution (record);
                    }
                    if (result_code == EXIT_FN_RET_OK)
                    {
                        result_code = process_pk_as_delete_insert (record, more_recs);
                    }
                }
                else
                if (UPDATE_COMP_SQL_VAL == record->io_type &&
                    !is_extract) /* can only fetch target with SQLEXEC from Replciat */
                {
                    result_code = check_conflict_resolution (record);
                }
                else
                    output_msg ("\nUser exit: skipping processing record for I/O type %hd.\n", record->io_type );
            }
            else
                output_msg ("Error (%hd) retrieving operation type.\n", result_code);

            free (record->buffer);
            free (record);
        }
    }
    else
        output_msg ("Error (%hd) retrieving table name.\n", result_code);

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
                    is_extract = 1;
                }
            }
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

            result_code = process_record(&exit_params->more_recs_ind);
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
            error_info.max_length = MAX_LENGTH;
            error_info.error_msg = (char *)malloc(error_info.max_length);

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

    fflush (stdout);
}

