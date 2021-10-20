/* Copyright (c) 2016, 2019, Oracle and/or its affiliates. 
All rights reserved.*/
 
/* 
   NAME 
     sqldatatypes.h - moved _SQLDT and _SQLSDT from recutl.h

   DESCRIPTION 
     <short description of facility this file declares/defines> 

   RELATED DOCUMENTS 
     <note any documents related to this facility>
 
   EXPORT FUNCTION(S) 
     <external functions declared for use outside package - one-line descriptions>

   INTERNAL FUNCTION(S)
     <other external functions declared - one-line descriptions>

   EXAMPLES

   NOTES
     <other useful comments, qualifications, etc.>

*/

#ifndef SQLDATATYPES_ORACLE
# define SQLDATATYPES_ORACLE

/* field type defines */
#define SQL_ANY_CHAR_TDM(data_type)\
    ((data_type) <= SQL_MAX_CHAR || (data_type) >= _SQLDT_DATETIME_V)

#define SQL_ANY_VARCHAR_TDM(data_type)\
    ((data_type) == _SQLDT_ASCII_V || (data_type) == _SQLDT_ASCII_V_UP || (data_type) == _SQLDT_DATETIME_V)

#define SQL_CHAR_OR_VARCHAR_TDM (data_type)\
    ((data_type) <= SQL_MAX_CHAR)

#define VC_LEN 2

#define SQL_MAX_CHAR 127
#define SQL_MAX_NUMERIC 190

/* all of the following data types have matching
   equivilent defines in usrdecs.h for client access in
   user exits they must be kept in sync */
#define _SQLDT_ASCII_F       0
#define _SQLDT_ASCII_F_UP    1
#define _SQLDT_DOUBLE_F      2
#define _SQLDT_ASCII_V      64
#define _SQLDT_ASCII_V_UP   65
#define _SQLDT_DOUBLE_V     66
#define _SQLDT_16BIT_S     130
#define _SQLDT_16BIT_U     131
#define _SQLDT_32BIT_S     132
#define _SQLDT_32BIT_U     133
#define _SQLDT_64BIT_S     134
#define _SQLDT_64BIT_U     135

#define _SQLDT_REAL        140   /* 32 bit Tandem Float */
#define _SQLDT_DOUBLE      141   /* 64 bit Tandem Float */

#define _SQLDT_TDM_REAL    140   /* 32 bit Tandem Float */
#define _SQLDT_TDM_DOUBLE  141   /* 64 bit Tandem Float */
#define _SQLDT_IEEE_REAL   142   /* 32 bit IEEE Float   */
#define _SQLDT_IEEE_DOUBLE 143   /* 64 bit IEEE Float   */

#define _SQLDT_DEC_U       150
#define _SQLDT_DEC_LSS     151
#define _SQLDT_DEC_LSE     152
#define _SQLDT_DEC_TSS     153
#define _SQLDT_DEC_TSE     154

/* SQL/MX extended precision numerics (up to 128 digits) */
#ifndef _SQLDT_NUM_BIG_U
#define SQLDT_NUM_BIG_U  155  /* for unsigned BigNum */
#define SQLDT_NUM_BIG_S  156 /*  for signed BigNum   */

#define _SQLDT_NUM_BIG_U SQLDT_NUM_BIG_U
#define _SQLDT_NUM_BIG_S SQLDT_NUM_BIG_S
#endif

#define _SQLDT_DEC_PACKED  189
#define _SQLDT_DATETIME_V  191
#define _SQLDT_DATETIME    192

/* Interval datatype */
/* Year to Year */
#define _SQLDT_INT_Y_Y 195
/* Month to Month */
#define _SQLDT_INT_MO_MO 196
/* Year to Month */
#define _SQLDT_INT_Y_MO 197
/* Day to Day */
#define _SQLDT_INT_D_D 198
/* Hour to Hour */
#define _SQLDT_INT_H_H 199
/* Day to Hour */
#define _SQLDT_INT_D_H 200
/* Minute to Minute */
#define _SQLDT_INT_MI_MI 201
/* Hour to Minute */
#define _SQLDT_INT_H_MI 202
/* Day to Minute */
#define _SQLDT_INT_D_MI 203
/* Second to Second */
#define _SQLDT_INT_S_S 204
/* Minute to Second */
#define _SQLDT_INT_MI_S 205
/* Hour to Second */
#define _SQLDT_INT_H_S 206
/* Day to Second */
#define _SQLDT_INT_D_S 207
/* Fraction to Fraction */
#define _SQLDT_INT_F_F 208
/* Second to Fraction */
#define _SQLDT_INT_S_F 209
/* Minute to Fraction */
#define _SQLDT_INT_MI_F 210
/* Hour to Fraction */
#define _SQLDT_INT_H_F 211
/* Day to Fraction */
#define _SQLDT_INT_D_F 212

/* Internal sub data type (extension to the internal data type).
 * Important:  _SQLSDT_DEFAULT (0) is reserved to maintain backward
 * compatibility.
 */
/* all of the following sub data types have matching
   equivilent defines in usrdecs.h for client access in
   user exits they must be kept in sync */
#define _SQLSDT_DEFAULT         0   /* Default value */
#define _SQLSDT_UTF16_BE        1   /* UTF-16 big endian encoding */
#define _SQLSDT_FLOAT           2   /* Floating-point number in string format */
#define _SQLSDT_FLOAT_UTF8      27  /* Floating-point number in UTF8 format */
#define _SQLSDT_FIXED_PREC      3   /* Fixed-precision number in string format */
#define _SQLSDT_FIXED_PREC_UTF8 37  /* Fixed-precision number in UTF8 format */
#define _SQLSDT_BINARY          4   /* Binary data */
#define _SQLSDT_CHAR_TYPE       5   /* CHAR datatype written with length info */
#define _SQLSDT_MVS_ASCII       6   /* MVS ASCII encoding */
#define _SQLSDT_UTF8            7   /* UTF-8 encoding */
#define _SQLSDT_ASCII_GUID_LE   8   /* GUID string "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" */
#define _SQLSDT_PERIOD          9   /* Period data type */
#define _SQLSDT_XML             10  /* XML column */
#define _SQLSDT_ANYDATA         11  /* AnyData column */
#define _SQLSDT_HTML            12  /* html data type */

#endif                                              /* SQLDATATYPES_ORACLE */
