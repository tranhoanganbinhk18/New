       01 HEADER
             05 TABLE-NAME                      PIC X(32)
             05 OP-CODE                         PIC X
      *   DATEFORMAT YYYY-MM-DD-HH.MM.SS.TH
             05 TIMESTAMP                       PIC X(22)
       01 ALL_DT
      *   KEY
             05 KEY-COL                         PIC 9(11)
             05 CHAR-COL                        PIC X(10)
             05 VARCHAR-COL                     PIC X(20)
             05 SMALL-INT-COL                   PIC 9(6)
             05 BIG-INT-COL                     PIC 9(20)
             05 CHAR-COL-2                      PIC X(10)
             05 SMALL-DEC-COL                   PIC 99.99
             05 MED-DEC-COL                     PIC 9(8)V9(7)
             05 BIG-DEC-COL                     PIC 9(28).9(10)
             05 VARCHAR-COL-2                   PIC X(20)
      *   DATEFORMAT YYYY-MM-DD-HH.MM.SS
             05 DATE-COL                        PIC X(19)
      *   DATEFORMAT YYYY-MM-DD-HH.MM.SS.FFFFFFFFF
             05 TIMESTAMP-COL                   PIC X(29)
