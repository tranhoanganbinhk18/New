10  HEADER
    20  Profile_Name    PIC X(8)
    20  Source_System   PIC X(8)
    20  Source_Database PIC X(8)
    20  Dest_File       PIC X(8)
    20  Template_Name   PIC X(8)
    20  Function        PIC X
*  DATEFORMAT YYYY-MM-DD-HH.MM.SS.THH
    20  Timestamp       PIC X(23)
10  TABLE1
*   KEY
    20  Key                     PIC X(19)
*   KEY
    20  PAN_Seq_Num             PIC 9(3)
    20  Card_Type               PIC XX
    20  Account_Type            PIC XX
    20  Issuer_FIID             PIC X(4)
*   DATEFORMAT                  YYMM
    20  Start_Date              PIC 9(4)
*   DATEFORMAT                  YYMM
    20  Expiry_Date             PIC 9(4)
    20  Account_Number          PIC X(19)
    20  Encrypted_PIN           PIC X(16)
    20  Encryption_Key_Version  PIC X
    20  Daily_Limit             PIC 9(5).99
    20  Card_Status             PIC X
    20  Chip_Tran_Counter       PIC 9(4)
    20  Travel_Data             PIC X(256)
    20  CRC_or_MAC              PIC X(8)
10  PKLR1-VALUATION-CODE.                                
    20  PKLR1-MORTALITY-TABLE    PIC X(2).               
    20  PKLR1-LIVES-CODE         PIC X(1).               
    20  PKLR1-FUNCTION           PIC X(1). 
    20  PKLR1-MODIFICATION       PIC X(1).               
    20  PKLR1-INSURANCE-CLASS    PIC X(1).               
    20  PKLR1-SERIES             PIC X(5).               