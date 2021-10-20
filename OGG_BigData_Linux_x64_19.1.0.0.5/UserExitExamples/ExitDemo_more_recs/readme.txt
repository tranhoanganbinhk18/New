/ Copyright (C) 1995, 2010, Oracle and/or its affiliates. All rights reserved./

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

  To compile this exit code, use the appropriate make or project file in this same directory.  The compiles are 32bit,
  for 64bit, changes will be necessary in the make files as well as possibly #includes in the .c file.

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