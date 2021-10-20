/**************************************************************************
 Copyright (c) 2001, 2019, Oracle and/or its affiliates. All rights reserved.

  Program description:

  Include file for user exit API.

***************************************************************************/

#ifndef USRDECS_H__
#define USRDECS_H__

#include <limits.h>
#include "ucharset.h"
#include "SQLDataTypes.h"

#define CALLBACK_STRUCT_VERSION     6

#ifndef MAX_PATH
#define MAX_PATH 250
#endif

#ifndef MAX_REC_LEN
#define MAX_REC_LEN (512*1024)
#endif

#ifndef MAX_COL_LEN
#define MAX_COL_LEN (USHRT_MAX-32)
#endif

/* User exit parameters */
typedef short exit_call_type_def;
typedef short exit_result_def;

typedef char exit_rec_buf_def[MAX_REC_LEN];
typedef char exit_rec_buf_ascii_def[MAX_REC_LEN*2];

typedef char exit_col_buf_def[MAX_COL_LEN];
typedef char exit_col_buf_ascii_def[MAX_COL_LEN*2];

/* Timestamp string */
typedef char exit_ts_str[30];

/* User exit context */
typedef struct
{
    char program_name[MAX_PATH+1];
    char function_param[101];
    char more_recs_ind;
} exit_params_def;

/* User exit record types */
#define EXIT_REC_TYPE_SQL 1
#define EXIT_REC_TYPE_ENSCRIBE 2

/* User exit operation types */
#define DELETE_VAL                           3
#define INSERT_VAL                           5
#define UPDATE_VAL                          10
#define UPDATE_COMP_ENSCRIBE_VAL            11
#define UPDATE_COMP_SQL_VAL                 15
#define TRUNCATE_TABLE_VAL                 100
#define UPDATE_COMP_PK_SQL_VAL             115
#define UNIFIED_UPDATE_COMP_SQL_VAL        134
#define UNIFIED_UPDATE_COMP_PK_SQL_VAL     135
#define UPSERT_VAL                         140
#define SQL_DDL_VAL                        160

/* User exit statistic groups */
#define EXIT_STAT_GROUP_STARTUP  1 /* Since process startup */
#define EXIT_STAT_GROUP_DAILY    2 /* Since midnight of the current day */
#define EXIT_STAT_GROUP_HOURLY   3 /* Since the start of the current hour */
#define EXIT_STAT_GROUP_RECENT   4 /* Since the last reset command via GGSCI */
#define EXIT_STAT_GROUP_REPORT   5 /* Since the last report was generated */
#define EXIT_STAT_GROUP_USEREXIT 6 /* Since the stats reset by user exit */

/* User exit return codes */
#define EXIT_OK_VAL             1 /* Extract or Replicat will continue to process */
#define EXIT_IGNORE_VAL         2 /* Extract or Replicat will skip this record */
#define EXIT_STOP_VAL           3 /* Extract or Replicat will STOP */
#define EXIT_ABEND_VAL          4 /* Extract or Replicat will ABEND */
#define EXIT_PROCESSED_REC_VAL  5 /* Extract or Replicat will skip this record, but update stats by Table by I/O type */

/* User exit OP codes */
#define EXIT_CALL_START                 1
#define EXIT_CALL_BEGIN_TRANS           2
#define EXIT_CALL_PROCESS_RECORD        3
#define EXIT_CALL_DISCARD_ASCII_RECORD  4
#define EXIT_CALL_DISCARD_RECORD        5
#define EXIT_CALL_END_TRANS             6
#define EXIT_CALL_CHECKPOINT            7
#define EXIT_CALL_PROCESS_MARKER        8
#define EXIT_CALL_STOP                  9
#define EXIT_CALL_DISCARD_TRANS_RECORD 10
#define EXIT_CALL_ABORT_TRANS          11
#define EXIT_CALL_EVENT_RECORD         12
#define EXIT_CALL_FATAL_ERROR          99

/* User exit return codes from callback routines */
#define EXIT_FN_RET_OK                      0
#define EXIT_FN_RET_INVALID_COLUMN          1
#define EXIT_FN_RET_COLUMN_NOT_FOUND        2
#define EXIT_FN_RET_TABLE_NOT_FOUND         3
#define EXIT_FN_RET_BAD_COLUMN_DATA         4
#define EXIT_FN_RET_INVALID_CONTEXT         5
#define EXIT_FN_RET_INVALID_PARAM           6
#define EXIT_FN_RET_INVALID_CALLBACK_FNC_CD 7
#define EXIT_FN_RET_NOT_SUPPORTED           8
#define EXIT_FN_RET_FETCH_ERROR             9
#define EXIT_FN_RET_EXCEEDED_MAX_LENGTH    10
#define EXIT_FN_RET_TOKEN_NOT_FOUND        11
#define EXIT_FN_RET_INVALID_RECORD_TYPE    12
#define EXIT_FN_RET_INCOMPLETE_DDL_REC     13
#define EXIT_FN_RET_ENV_NOT_FOUND          14
#define EXIT_FN_RET_INVALID_COLUMN_TYPE    15
#define EXIT_FN_RET_SESSION_CS_CNV_ERR     16
#define EXIT_FN_RET_BAD_DATE_TIME          17
#define EXIT_FN_RET_BAD_NUMERIC_VALUE      18
#define EXIT_FN_RET_NO_SRCDB_INSTANCE      19
#define EXIT_FN_RET_NO_TRTDB_INSTANCE      20

/* Old User exit ASCII/internal indicators - deprecated */
#define EXIT_FN_ASCII_FORMAT    1
#define EXIT_FN_INTERNAL_FORMAT 2

/* New User exit ASCII/internal indicators, starting from V3 */
#define EXIT_FN_CHAR_FORMAT    1
#define EXIT_FN_RAW_FORMAT 2
#define EXIT_FN_CNVTED_SESS_CHAR_FORMAT 3

/* User exit source/target indicators */
#define EXIT_FN_SOURCE_VAL  1
#define EXIT_FN_TARGET_VAL  2
#define EXIT_FN_CURRENT_VAL 3

/* User exit stop status codes */
#define STOP_STATUS_NORMAL 0
#define STOP_STATUS_ABEND  1

/* User exit transaction indicators */
#define BEGIN_TRANS_VAL  0
#define MIDDLE_TRANS_VAL 1
#define END_TRANS_VAL    2
#define WHOLE_TRANS_VAL  3

/* User exit before/after indicators */
#define BEFORE_IMAGE_VAL 0x42 /* ASCII B */
#define AFTER_IMAGE_VAL  0x41 /* ASCII A */

#define CHAR_YES_VAL 0x59   /* ASCII Y */
#define CHAR_NO_VAL  0x4e   /* ASCII N */

/* Position types  */
#define STARTUP_CHECKPOINT     0 /* starting position in the data source */
#define CURRENT_CHECKPOINT     1 /* position of last record read in the data source (CURRENT) */
#define RECOVERY_CHECKPOINT    2 /* Tranlog position of oldest unprocessed transaction in the data source -- FUTURE USE */

/* GG internal Data types */
#define SQLDT_ASCII_F     _SQLDT_ASCII_F
#define SQLDT_ASCII_F_UP  _SQLDT_ASCII_F_UP  
#define SQLDT_DOUBLE_F    _SQLDT_DOUBLE_F
#define SQLDT_ASCII_V     _SQLDT_ASCII_V
#define SQLDT_ASCII_V_UP  _SQLDT_ASCII_V_UP
#define SQLDT_DOUBLE_V    _SQLDT_DOUBLE_V
#define SQLDT_16BIT_S     _SQLDT_16BIT_S     
#define SQLDT_16BIT_U     _SQLDT_16BIT_U     
#define SQLDT_32BIT_S     _SQLDT_32BIT_S     
#define SQLDT_32BIT_U     _SQLDT_32BIT_U     
#define SQLDT_64BIT_S     _SQLDT_64BIT_S     
#define SQLDT_64BIT_U     _SQLDT_64BIT_U     
#define SQLDT_REAL        _SQLDT_REAL        
#define SQLDT_DOUBLE      _SQLDT_DOUBLE      
#define SQLDT_TDM_REAL    _SQLDT_TDM_REAL    
#define SQLDT_TDM_DOUBLE  _SQLDT_TDM_DOUBLE  
#define SQLDT_IEEE_REAL   _SQLDT_IEEE_REAL   
#define SQLDT_IEEE_DOUBLE _SQLDT_IEEE_DOUBLE 
#define SQLDT_DEC_U       _SQLDT_DEC_U       
#define SQLDT_DEC_LSS     _SQLDT_DEC_LSS     
#define SQLDT_DEC_LSE     _SQLDT_DEC_LSE     
#define SQLDT_DEC_TSS     _SQLDT_DEC_TSS     
#define SQLDT_DEC_TSE     _SQLDT_DEC_TSE     
#define SQLDT_DEC_PACKED  _SQLDT_DEC_PACKED  
#define SQLDT_DATETIME_V  _SQLDT_DATETIME_V  
#define SQLDT_DATETIME    _SQLDT_DATETIME    

/* Interval datatype */
/* Year to Year */
#define SQLDT_INT_Y_Y     _SQLDT_INT_Y_Y 
/* Month to Month */
#define SQLDT_INT_MO_MO   _SQLDT_INT_MO_MO 
/* Year to Month */
#define SQLDT_INT_Y_MO    _SQLDT_INT_Y_MO 
/* Day to Day */
#define SQLDT_INT_D_D     _SQLDT_INT_D_D
/* Hour to Hour */
#define SQLDT_INT_H_H     _SQLDT_INT_H_H
/* Day to Hour */
#define SQLDT_INT_D_H     _SQLDT_INT_D_H
/* Minute to Minute */
#define SQLDT_INT_MI_MI   _SQLDT_INT_MI_MI
/* Hour to Minute */
#define SQLDT_INT_H_MI    _SQLDT_INT_H_MI
/* Day to Minute */
#define SQLDT_INT_D_MI    _SQLDT_INT_D_MI 
/* Second to Second */
#define SQLDT_INT_S_S     _SQLDT_INT_S_S
/* Minute to Second */
#define SQLDT_INT_MI_S    _SQLDT_INT_MI_S
/* Hour to Second */
#define SQLDT_INT_H_S     _SQLDT_INT_H_S 
/* Day to Second */
#define SQLDT_INT_D_S     _SQLDT_INT_D_S 
/* Fraction to Fraction */
#define SQLDT_INT_F_F     _SQLDT_INT_F_F 
/* Second to Fraction */
#define SQLDT_INT_S_F     _SQLDT_INT_S_F 
/* Minute to Fraction */
#define SQLDT_INT_MI_F    _SQLDT_INT_MI_F 
/* Hour to Fraction */
#define SQLDT_INT_H_F     _SQLDT_INT_H_F
/* Day to Fraction */
#define SQLDT_INT_D_F     _SQLDT_INT_D_F 

/* GG internal Sub Data types */
#define SQLSDT_DEFAULT         _SQLSDT_DEFAULT         /* Default value */
#define SQLSDT_UTF16_BE        _SQLSDT_UTF16_BE        /* UTF-16 big endian encoding */
#define SQLSDT_FLOAT           _SQLSDT_FLOAT           /* Floating-point number in string format */
#define SQLSDT_FLOAT_UTF8      _SQLSDT_FLOAT_UTF8      /* Floating-point number in UTF8 format */
#define SQLSDT_FIXED_PREC      _SQLSDT_FIXED_PREC      /* Fixed-precision number in string format */
#define SQLSDT_FIXED_PREC_UTF8 _SQLSDT_FIXED_PREC_UTF8 /* Fixed-precision number in UTF8 forat */
#define SQLSDT_BINARY          _SQLSDT_BINARY          /* Binary data */
#define SQLSDT_CHAR_TYPE       _SQLSDT_CHAR_TYPE       /* CHAR datatype written with length info */
#define SQLSDT_MVS_ASCII       _SQLSDT_MVS_ASCII       /* MVS ASCII encoding */
#define SQLSDT_UTF8            _SQLSDT_UTF8            /* UTF-8 encoding */
#define SQLSDT_ASCII_GUID_LE   _SQLSDT_ASCII_GUID_LE   /* GUID string "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" */
#define SQLSDT_PERIOD          _SQLSDT_PERIOD          /* Period data type */
#define SQLSDT_XML             _SQLSDT_XML             /* XML column */
#define SQLSDT_ANYDATA         _SQLSDT_ANYDATA         /* AnyData column */
#define SQLSDT_HTML            _SQLSDT_HTML            /* html data type */

/* Event record identifier */
#define EXIT_DATABASE_METADATA_EVENT      1   /* new or updated database metadata */
#define EXIT_TABLE_METADATA_EVENT         2   /* new or updated table metadata */
#define EXIT_REF_TABLE_METADATA_EVENT     3   /* new or updated ref-table metadata */

/* Callback function record definitions */
typedef struct
{
    char *column_value;                     /* Column value buffer */
    unsigned short max_value_length;        /* Maximum buffer length (max: MAX_COL_LEN) */
    unsigned short actual_value_length;     /* Actual buffer length (max: MAX_COL_LEN) */
    short null_value;                       /* Is the value NULL? */
    short remove_column;                    /* Remove this column from a compressed update? */
    short value_truncated;                  /* Was the value truncated? */
    short column_index;                     /* Column index */
    char *column_name;                      /* Column name */
    short column_value_mode;                /* changed from ascii_or_internal, from V3 */
    short source_or_target;                 /* Source or target value? */
   /* Version 2 CALLBACK_STRUCT_VERSION  */
    char requesting_before_after_ind;       /* request for the before or after image update indicator */
    char more_lob_data;                     /* continuation flag for more LOB chucks in memmory */
   /* Version 3 CALLBACK_STRUCT_VERSION   */
     ULibCharSet column_charset;            /* column character set, not available to set */
   /* Version 5 CALLBACK_STRUCT_VERSION   */
    short is_hidden_column;                 /* indicates hidden/invisible column if non-zero, not available to set */
} column_def;

typedef struct
{
    char *compressed_rec;   /* Compressed record */
    long compressed_len;    /* Compressed record length */
    char *decompressed_rec; /* Decompressed record */
    long decompressed_len;  /* Decompressed record length */
    short *columns_present; /* Columns present in compressed record */
    short source_or_target; /* Source or target record? */
   /* Version 2 CALLBACK_STRUCT_VERSION   */
    char requesting_before_after_ind;   /* request for the before or after image update indicator */
} compressed_rec_def;

typedef struct
{
    char *buffer;           /* Environment value buffer */
    long max_length;        /* Maximum buffer length */
    long actual_length;     /* Actual buffer length */
    short value_truncated;  /* Was value truncated? */
    short index;            /* Table or column index */
    short source_or_target; /* Source or target value? */
} env_value_def;

typedef struct
{
    char *information_type; /* Type of getenv wanted PASSED IN BY USER */
    char *env_value_name;   /* lable of getenv wanted PASSED IN BY USER */
    char *return_value;     /* return value of getenv */
    long max_return_length; /* Maximum buffer length */
    long actual_length;     /* Actual buffer length */
    short value_truncated;  /* Was value truncated? */
} getenv_value_def;

typedef struct
{
    long error_num;      /* SQL or system error number */
    char *error_msg;     /* Associated error message */
    long max_length;     /* Maximum error message length */
    long actual_length;  /* Actual error message length */
    short msg_truncated; /* Was error message truncated? */
} error_info_def;

typedef struct
{
    char *processed; /* Date/time marker processed in format 'YYYY-MM-DD HH:MI:SS' */
    char *added;     /* Date/time marker added in format 'YYYY-MM-DD HH:MI:SS' */
    char *text;      /* Marker text */
    char *group;     /* Group that processed marker */
    char *program;   /* Program that processed marker */
    char *node;      /* Originating node (Tandem) of marker */
} marker_info_def;

typedef struct record_def
{
    char *table_name;        /* Table name */
    char *buffer;            /* Record buffer */
    long length;             /* Record length */
    char before_after_ind;   /* Before/after indicator */
    short io_type;           /* Operation type */
    short record_type;       /* SQL or Enscribe? */
    short transaction_ind;   /* Transaction indicator */
    int64_t timestamp;       /* Operation timestamp */
    exit_ts_str io_datetime; /* Operation timestamp in format 'YYYY-MM-DD HH:MI:SS.FFFFFF' */
    short mapped;            /* Has the record been mapped? */
    short source_or_target;  /* Source or target record? */
   /* Version 2 CALLBACK_STRUCT_VERSION   */
    char requesting_before_after_ind;  /* request for the before or after image update indicator */
} record_def;

typedef struct statistics_def
{
    char *table_name;           /* Source table */
    short group;                /* Statistical group */
    exit_ts_str start_datetime; /* Statistics start time in format 'YYYY-MM-DD HH:MI:SS' */
    int64_t num_inserts;           /* Number of inserts since start of statistics */
    int64_t num_updates;           /* Number of updates since start of statistics */
    int64_t num_befores;           /* Number of before images since start of statistics */
    int64_t num_deletes;           /* Number of deletes since start of statistics */
    int64_t num_discards;          /* Number of discards since start of statistics */
    int64_t num_ignores;           /* Number of ignores since start of statistics */
    int64_t total_db_operations;   /* Total database operations since start of statistics */
    int64_t total_operations;      /* Total operations since start of statistics */
   /* Version 2 CALLBACK_STRUCT_VERSION   */
    int64_t num_truncates;         /* Number of truncates since start of statistics */
   /* Version 6 CALLBACK_STRUCT_VERSION   */
    int64_t num_upserts;           /* Number of upserts since start of statistics */
} statistics_def;


typedef struct
{
    char *table_name;          /* Table name */
    long  max_name_length;     /* Maximum table name length PASSED IN BY USER */
    short num_columns;         /* Number of columns in table */
    short num_key_columns;     /* Number of key columns in table */
    short *key_columns;        /* Array of key column indexes */
    short using_pseudo_key;    /* Did we create a pseudo key for a table with no unique key */
    short source_or_target;    /* Source or target table PASSED IN BY USER */
   /* Version 5 CALLBACK_STRUCT_VERSION   */
    short num_user_columns;    /* Number of user column (excluding hidden/invisible columns) */
} table_metadata_def;

typedef struct
{
    short column_index;        /* Column index PASSED IN BY USER or column name passed in */
    char *column_name;         /* Column name  PASSED IN BY USER */
    long  max_name_length;     /* Max length of column_name field, PASSED IN BY USER */
    short native_data_type;    /* Native data type (Dependent on interface) */
    short gg_data_type;        /* Goldengate data type */
    short gg_sub_data_type;    /* Goldengate data sub type */
    short is_nullable;         /* Indicates if the column is nullable */
    short is_part_of_key;      /* Part of the primary key indicator */
    short key_column_index;    /* Indicates order of index of column in key */
    long length;               /* Column length */
    long precision;            /* ODBC precision */
    short scale;               /* Numeric scale */
    short source_or_target;    /* Source or target table? PASSED IN BY USER */
   /* Version 5 CALLBACK_STRUCT_VERSION   */
    ULibCharSet column_charset; /* column character set */
    short is_hidden_column;    /* indicates hidden/invisible column if non-zero */
} col_metadata_def;

typedef struct
{
    short num_columns;      /* Number of columns in table */
    short source_or_target; /* Source or target table? */
   /* Version 2 CALLBACK_STRUCT_VERSION   */
    short num_key_columns;  /* number of key columns*/
   /* Version 5 CALLBACK_STRUCT_VERSION   */
    short num_user_columns;    /* Number of user column (excluding hidden/invisible columns) */
} table_def;

typedef struct
{
    char *position;            /* currently string of 2 binary values unsigned int32t & int32t as seqnorba for 8 bytes */
    long position_len;         /* length of char */
    short position_type;       /* see defines above */
    short ascii_or_internal;   /* ASCII or internal format? */
} position_def;

typedef struct
{
    char *token_name;       /* Token Name as input to find value for PASSED IN BY USER*/
    char *token_value;      /* Token value buffer */
    long max_length;        /* Maximum Token Name length PASSED IN BY USER */
    long actual_length;     /* Actual buffer length */
    short value_truncated;  /* Was value truncated? */
} token_value_def;

typedef struct
{
    char *ddl_type;               /* Type of the DDL operation (e.g., CREATE, ALTER) */ 
    long ddl_type_max_length;     /* Maximum Description length PASSED IN BY USER */
    long ddl_type_length;         /* Actual length */
    
    char *object_type;            /* Type of object the DDL is operating on (e.g., TABLE, INDEX) */ 
    long object_type_max_length;  /* Maximum Description length PASSED IN BY USER */
    long object_type_length;      /* Actual length */

    char *object_name;            /* Fully qualified name of the object */ 
    long object_max_length;       /* Maximum Description length PASSED IN BY USER */
    long object_length;           /* Actual length */

    char *owner_name;             /* Owner of the object */ 
    long owner_max_length;        /* Maximum Description length PASSED IN BY USER */
    long owner_length;            /* Actual length */

    char *ddl_text;               /* DDL text */
    long ddl_text_max_length;     /* Maximum Description length PASSED IN BY USER */
    long ddl_text_length;         /* Actual length */
    short ddl_text_truncated;     /* Was value truncated? */

    short source_or_target;       /* Source or target value? */
} ddl_record_def;

/* event record: callback structure version 4 or later only */
typedef struct
{
    short  event_id;              /* event record identifier */
    char*  object_name;           /* object name of the event record (database or table) */
    long   max_name_length;       /* max length of object_name field, PASSED IN BY USER */
    long   actual_length;         /* actual object name length */
} event_record_def;

/* Callback function codes */
typedef enum
{
    COMPRESS_RECORD,                /* Compress the record buffer */
    DECOMPRESS_RECORD,              /* Decompress the record buffer */
    GET_BEFORE_AFTER_IND,           /* Return before/after indicator */
    GET_COLUMN_INDEX_FROM_NAME,     /* Return column index for a specified column name */
    GET_COLUMN_NAME_FROM_INDEX,     /* Return column name for a specified column index */
    GET_COLUMN_VALUE_FROM_INDEX,    /* Return column value for a specified column index */
    GET_COLUMN_VALUE_FROM_NAME,     /* Return column value for a specified column name */
    GET_ERROR_INFO,                 /* Return error information for a discard record */
    GET_MARKER_INFO,                /* Return marker information */
    GET_OPERATION_TYPE,             /* Return insert, update, or delete */
    GET_RECORD_BUFFER,              /* Return record buffer before or after mapping */
    GET_RECORD_LENGTH,              /* Return the record buffer length */
    GET_RECORD_TYPE,                /* Return the record type (SQL or Enscribe) */
    GET_STATISTICS,                 /* Return statistics for a specified statistical group */
    GET_TABLE_COLUMN_COUNT,         /* Return the number of columns in a table */
    GET_TABLE_NAME,                 /* Return the fully qualified table name */
    GET_TIMESTAMP,                  /* Return operation timestamp */
    GET_TRANSACTION_IND,            /* Return begin, middle, end, or whole */
    SET_COLUMN_VALUE_BY_INDEX,      /* Set a column value given a column index */
    SET_COLUMN_VALUE_BY_NAME,       /* Set a column value given a column name */
    SET_OPERATION_TYPE,             /* Set the operation type to insert, update, or delete */
    SET_RECORD_BUFFER,              /* Set the record buffer and length */
    SET_TABLE_NAME,                 /* Set the table name */
    FETCH_CURRENT_RECORD,           /* Fetch the current record given the target key */
    FETCH_CURRENT_RECORD_WITH_LOCK, /* Fetch and lock the current record given the target key */
    OUTPUT_MESSAGE_TO_REPORT,       /* Output a message to the report file. */
    GET_COL_METADATA_FROM_INDEX,    /* Returns Column detail information for a specified column index */
    GET_COL_METADATA_FROM_NAME,     /* Returns Column detail information for a specified column name */
    GET_TABLE_METADATA,             /* Returns basic table information */
    GET_POSITION,                   /* Returns position info */
    GET_USER_TOKEN_VALUE,           /* Returns User token value if present */
    GET_ENV_VALUE,                  /* Returns ENV values same as @GETENV */
    GET_DDL_RECORD_PROPERTIES,      /* Returns DDL text properties and DDL text */
    RESET_USEREXIT_STATS,           /* Resets All stats bucket totals for user exit interval */
    GET_GMT_TIMESTAMP,              /* Return operation timestamp in GMT */
    SET_SESSION_CHARSET,            /* Set session character set */
    GET_SESSION_CHARSET,            /* Get session character set */
    GET_DATABASE_METADATA,          /* Get database metadata */
    GET_TABLE_NAME_ONLY,            /* Return only the table name piece of the fully qualified table name */
    GET_SCHEMA_NAME_ONLY,           /* Return only the schema name piece of the full table name */
    GET_CATALOG_NAME_ONLY,          /* Return only the catalog name piece of the full table name */
    GET_OBJECT_NAME_ONLY,           /* Return only the object name piece of the fully qualified object name*/
    GET_OBJECT_NAME,                /* Return the fuly qualified object name */
    GET_BASE_OBJECT_NAME_ONLY,      /* Return only the base object name peice */
    GET_BASE_SCHEMA_NAME_ONLY,      /* Return only the schema name peice for the base object */
    GET_BASE_OBJECT_NAME,           /* Return the fuly qualified base object name */
    GET_EVENT_RECORD                /* Return the event record detail */
} ercallback_function_codes;

/* Callback function session character set definition */
typedef struct session_def
{
    ULibCharSet session_charset;   /* session character set */
} session_def;

/* Callback function database metadata definition */
/* it's better add the dbName_len, and local_len in the structure; 
   for eaiser use when session's character set is UTF16
 */
#define MAX_DBOBJECT_TYPES      16   /* DB object types in metadata max is 16 */
typedef struct
{
    char    *dbName;       /* database name */
    long dbName_max_length;      /* Maximum length dbName */
    long dbName_actual_length;   /* Actual dbName length */
    unsigned char dbNameMetadata[MAX_DBOBJECT_TYPES];  /* database object name metadata */
    char    *locale;       /* source database locale */
    long locale_max_length;      /* Maximum length dbName */
    long locale_actual_length;   /* Actual dbName length */
} database_def;

typedef struct database_defs
{
    database_def source_db_def;      /* source database metadata */
    database_def target_db_def;      /* target database metadata */
} database_defs;

/* 
 *    @brief    Database object name metadata bits.
 *  
 *    @detail   These bits are used to store and access database
 *    object name metadata.
 *  
 *    CS - Case Sensitive
 *    CI - Case Insensitive
 *    LC - Lower Case
 *    UC - Upper Case
 *    MIXED - Mixed Case
 */
enum
{
    /* unquoted name. */
    MIXED_CI_BIT               = 0x00,
    MIXED_CS_BIT               = 0x01,
    LC_CI_BIT                  = 0x02,
    UC_CI_BIT                  = 0x04,
    MIXED_CI_MASK              = 0x0f,
    MIXED_CI_UNSET_BITS        = 0xf0,

    /* quoted name. */
    MIXED_QUOTED_CI_BIT        = 0x00,
    MIXED_QUOTED_CS_BIT        = 0x10,
    LC_QUOTED_CI_BIT           = 0x20,
    UC_QUOTED_CI_BIT           = 0x40,
    MIXED_QUOTED_CI_MASK       = 0xf0,
    MIXED_QUOTED_CI_UNSET_BITS = 0x0f
};

/* 
 * Macros used to check database object name support for each data type 
 * Given a nameMeta, and DbObjType, we check if the object of the type 
 * support queried metadata
 */
/* 
 * check whether the db treats mixed case unquoted name for spedified data 
 * type as case sensitive and as result stores the name in mixed case.
 */
#define supportsMixedCaseIndentifiers( nameMeta, DbObjType )             \
    ((nameMeta[(int)DbObjType] & (uint8_t)(MIXED_CS_BIT)) != 0)  

/* 
 * check whether the db treats mixed case quoted name for spedified data 
 * type as case sensitive and as result stores the name in mixed case.
 */
#define supportsMixedCaseQuotedIndentifiers( nameMeta, DbObjType )       \
    ((nameMeta[(int)DbObjType] & (uint8_t)(MIXED_QUOTED_CS_BIT)) != 0)

/* 
 * check whether the db treats mixed case unquoted name for spedified data 
 * type as case sensitive and as result stores the name in lower case.
 */
#define storesLowerCaseIdentifiers( nameMeta, DbObjType )                \
    ((nameMeta[(int)DbObjType] & (uint8_t)(LC_CI_BIT)) != 0)  

/* 
 * check whether the db treats mixed case quoted name for spedified data 
 * type as case insensitive and as result stores the name in lower case.
 */
#define storesLowerCaseQuotedIdentifiers( nameMeta, DbObjType )          \
    ((nameMeta[(int)DbObjType] & (uint8_t)(LC_QUOTED_CI_BIT)) != 0)  

/* 
 * check whether the db treats mixed case unquoted name for spedified data 
 * type as case insensitive and as result stores the name in mixed case.
 */
#define storesMixedCaseIdentifiers( nameMeta, DbObjType )                \
    ((nameMeta[(int)DbObjType] & (uint8_t)(MIXED_CI_MASK)) == 0)  

/* 
 * check whether the db treats mixed case quoted name for spedified data 
 * type as case insensitive and as result stores the name in mixed case.
 */
#define storesMixedCaseQuotedIdentifiers( nameMeta, DbObjType )          \
    ((nameMeta[(int)DbObjType] & (uint8_t)(MIXED_QUOTED_CI_MASK)) == 0)  

/* 
 * check whether the db treats mixed case unquoted name for spedified data 
 * type as case insensitive and as result stores the name in upper case.
 */
#define storesUpperCaseIdentifiers( nameMeta, DbObjType )                \
    ((nameMeta[(int)DbObjType] & (UC_CI_BIT)) != 0)  

/* 
 * check whether the db treats mixed case quoted name for spedified data 
 * type as case insensitive and as result stores the name in upper case.
 */
#define storesUpperCaseQuotedIdentifiers( nameMeta, DbObjType )          \
    ((nameMeta[(int)DbObjType] & (UC_QUOTED_CI_BIT)) != 0)  

/* Structure versioning function Do Not Remove */
#if !(defined GOLDENGATE__)
  #if !(defined _WIN32)
     int (fperexitversion)(void)
     {
         return CALLBACK_STRUCT_VERSION;
     }
  #else
     __declspec(dllexport) int FPEREXITVERSION(void)
     {
         return CALLBACK_STRUCT_VERSION;
     }
  #endif
#endif


#endif /* USRDECS_H__ */
