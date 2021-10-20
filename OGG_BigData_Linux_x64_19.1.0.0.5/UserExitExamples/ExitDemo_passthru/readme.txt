/ Copyright (C) 1995, 2010, Oracle and/or its affiliates. All rights reserved./

  User exit example for Windows NT and Unix, and valid only for the extract pump. 
  This is a hypothetical example shows the use of PASSTHRU mode. This example uses the TCUSTMER 
  table, which can be created and manipulated using the demo*.sql scripts included with the ER 
  release.


  To compile this exit code, use the appropriate make or project file in this same directory.  The compiles are 32bit,
  for 64bit, changes will be necessary in the make files as well as possibly #includes in the .c file.


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