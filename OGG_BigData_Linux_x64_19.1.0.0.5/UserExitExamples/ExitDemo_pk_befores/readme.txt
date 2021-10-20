/ Copyright (C) 1995, 2010, Oracle and/or its affiliates. All rights reserved./

  This is a User exit example for access to "Primary Key Updates" and "before images" 
  from Updates.  This example uses the PK_BF_TIMESRC and PK_BF_TIMETRG tables,
  which can be created using the demo_*pk_befores_create.sql scripts included 
  with the ER release.  This same script will add a stored procedure, which when 
  executed will Insert and Update a row of data.  Please be sure to "ADD TRANDATA" for 
  the PK_BF_TIMESRC table for those databases which allow the GGSCI command, before executing.

  To compile this exit code, use the appropriate make or project file in this same directory.  The compiles are 32bit,
  for 64bit, changes will be necessary in the make files as well as possibly #includes in the .c file.

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