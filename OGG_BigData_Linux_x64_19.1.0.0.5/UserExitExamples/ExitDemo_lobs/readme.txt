/ Copyright (C) 1995, 2010, Oracle and/or its affiliates. All rights reserved./

  User exit example for read access to LOB data.  This example uses the
  TSRSLOB and TTRGVAR tables, which can be created  using the
  demo_*_lob_*.sql scripts included with the ER release.  This same script
  will add a stored procedure, which when executed will Insert and Update a
  row of data.  Please be sure to ADD TRANDATA TSRSLOB for those databases which
  allow the GGSCI command, before executing.


  To compile this exit code, use the appropriate make or project file in this same directory.  The compiles are 32bit,
  for 64bit, changes will be necessary in the make files as well as possibly #includes in the .c file.

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
