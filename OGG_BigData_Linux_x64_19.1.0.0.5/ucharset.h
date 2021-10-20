/**************************************************************************

 Copyright (c) 2008, 2016, Oracle and/or its affiliates. All rights reserved.

  Program description:

  Oracle GoldenGate character set/type definition.

***************************************************************************/

#ifndef _UCHARSET_H_
#define _UCHARSET_H_

/**----------------------------------------
 * ULibCharSet:
 *  Character Set / Encoding definition.
 */

/* for special character set type */
#define ULIB_CS_ESCAPE_BIT      0x80000000L
#define ULIB_CS_DEFAULT_BIT     0x40000000L
#define ULIB_CS_DEFESC_BITS     (int)0xc0000000L
#define ULIB_CS_UNKNOWN_BIT     0x20000000L
#define ULIB_IS_ESCAPED_CS(cs)  ((int32_t)(cs) & ULIB_CS_ESCAPE_BIT)
#define ULIB_CS_ESCAPE_MASK     ~(ULIB_CS_ESCAPE_BIT)
#define ULIB_CS_GETCS(cs)       (ULibCharSet)(((int32_t)(cs) & ULIB_CS_ESCAPE_MASK))

typedef enum
{
    /* special */
    ULIB_CS_UNKNOWN        = ULIB_CS_UNKNOWN_BIT,   /* unknown */
    ULIB_CS_DEFAULT        = ULIB_CS_DEFAULT_BIT,   /* operating system default */
    ULIB_CS_DBCHARSET      = -1,                    /* inherit DB character set */

    /* default character set and escaped UTF-16 code */
    ULIB_CS_DEFAULT_ESCAPE = ULIB_CS_DEFESC_BITS,

    ULIB_CS_BEGIN          = 0,
    ULIB_CS_UNICODE_BEGIN  = ULIB_CS_BEGIN, /* Start marker UNICODE */

    /* ISO/IEC-10646 UNICODE variants */
    ULIB_CS_UTF8 = ULIB_CS_UNICODE_BEGIN,   /* UTF-8 */
    ULIB_CS_UTF16,                          /* UTF-16 platform dependent endien */
    ULIB_CS_UTF16BE,                        /* UTF-16 Big Endian */
    ULIB_CS_UTF16LE,                        /* UTF-16 Little Endian */
    ULIB_CS_UTF32,                          /* UTF-32 Little Endian */
    ULIB_CS_UTF32BE,                        /* UTF-32 Big Endian */
    ULIB_CS_UTF32LE,                        /* UTF-32 Little Endian */
    ULIB_CS_UTF_EBCDIC,                     /* UTF-EBCDIC, reserverd and not supported */
    ULIB_CS_CESU8,                          /* CESU-8 compatibility encoding schema for UTF-16 */
    ULIB_CS_UTF7,                           /* UTF-7 */
    ULIB_CS_SCSU,                           /* Standard Compression Schema for UNICODE */
    ULIB_CS_BOCU1,                          /* Binary Ordered Compression UNICODE */
    ULIB_CS_UNICODE_END,

    /* SBCS - Single Byte Character Sets */
    ULIB_CS_SBCS_BEGIN = 1000,              /* Start marker SBCS */
    ULIB_CS_US_ASCII = ULIB_CS_SBCS_BEGIN,  /* US-ASCII, ANSI X34-1968 */

    /* Windows ANSI code pages */
    ULIB_CS_WCP1250,                        /* Windows Central Europe */
    ULIB_CS_WCP1251,                        /* Windows Cyrillic */
    ULIB_CS_WCP1252,                        /* Windows Western Europe, Latin-1 */
    ULIB_CS_WCP1253,                        /* Windows Greek */
    ULIB_CS_WCP1254,                        /* Windows Turkish */
    ULIB_CS_WCP1255,                        /* Windows Hebrew */
    ULIB_CS_WCP1256,                        /* Windows Arabic */
    ULIB_CS_WCP1257,                        /* Windows Baltic */
    ULIB_CS_WCP1258,                        /* Windows Vietnam */
    ULIB_CS_WCP874,                         /* Windows Thai */

    /* DOS OEM code page */
    ULIB_CS_CP437,                          /* DOS */
    ULIB_CS_CP720,                          /* DOS Arabic */
    ULIB_CS_CP737,                          /* DOS Greek */
    ULIB_CS_CP775,                          /* DOS Baltic */
    ULIB_CS_CP850,                          /* DOS Multilingual */
    ULIB_CS_CP851,                          /* DOS Greek-1 */
    ULIB_CS_CP852,                          /* DOS Latin-2 */ 
    ULIB_CS_CP855,                          /* DOS Cyrillic */
    ULIB_CS_CP856,                          /* DOS Cyrillic */
    ULIB_CS_CP857,                          /* DOS Turkish */
    ULIB_CS_CP858,                          /* DOS Multilingual with Euro */
    ULIB_CS_CP860,                          /* DOS Portuguese */
    ULIB_CS_CP861,                          /* DOS Icelandic */
    ULIB_CS_CP862,                          /* DOS Hebrew */
    ULIB_CS_CP863,                          /* DOS French */
    ULIB_CS_CP864,                          /* DOS Arabic */
    ULIB_CS_CP865,                          /* DOS Nordic */
    ULIB_CS_CP866,                          /* DOS Cyrillic */
    ULIB_CS_CP867,                          /* DOS Hebrew */
    ULIB_CS_CP868,                          /* DOS Urdu */
    ULIB_CS_CP869,                          /* DOS Greek-2 */

    /* ISO/IEC-8859 */
    ULIB_CS_ISO8859_1,                      /* ISO-8859-1 Latin-1, Western Europe */
    ULIB_CS_ISO8859_2,                      /* ISO-8859-2 Latin-2, Central Europe */
    ULIB_CS_ISO8859_3,                      /* ISO-8859-3 Latin-3, South Europe */
    ULIB_CS_ISO8859_4,                      /* ISO-8859-4 Latin-4, North Europe */
    ULIB_CS_ISO8859_5,                      /* ISO-8859-5 Latin/Cyrillic */
    ULIB_CS_ISO8859_6,                      /* ISO-8859-6 Latin/Arabic */
    ULIB_CS_ISO8859_7,                      /* ISO-8859-7 Latin/Greek */
    ULIB_CS_ISO8859_8,                      /* ISO-8859-8 Latin/Hebrew */
    ULIB_CS_ISO8859_9,                      /* ISO-8859-9 Latin-5, Turkish */
    ULIB_CS_ISO8859_10,                     /* ISO-8859-10 Latin-6, Nordic */
    ULIB_CS_ISO8859_11,                     /* ISO-8859-11 Latin/Thai */
    ULIB_CS_ISO8859_13,                     /* ISO-8859-13 Latin-7, Baltic Rim */
    ULIB_CS_ISO8859_14,                     /* ISO-8859-14 Latin-8, Celtic */
    ULIB_CS_ISO8859_15,                     /* ISO-8859-15 Latin-9, replace ISO-8859-1 */
    ULIB_CS_ISO8859_16,                     /* ISO-8859-16 Latin-10, South-Eastern Europe */

    /* EBCDIC */
    ULIB_CS_EBCDIC,                         /* IBM EBCDIC code page 037, US and Canada */
    ULIB_CS_EBCDIC_1140,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_273,                     /* IBM EBCDIC code page 273, Austria and Germany */
    ULIB_CS_EBCDIC_1141,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_277,                     /* IBM EBCDIC code page 277, Denmark and Norway */
    ULIB_CS_EBCDIC_1142,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_278,                     /* IBM EBCDIC code page 278, Finland and Sweden */
    ULIB_CS_EBCDIC_1143,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_280,                     /* IBM EBCDIC code page 280, Italy */
    ULIB_CS_EBCDIC_1144,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_284,                     /* IBM EBCDIC code page 284, Latin America and Spain */
    ULIB_CS_EBCDIC_1145,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_285,                     /* IBM EBCDIC code page 285, UK and Ireland */
    ULIB_CS_EBCDIC_1146,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_290,                     /* IBM EBCDIC code page 290, Japan Katakana */
    ULIB_CS_EBCDIC_297,                     /* IBM EBCDIC code page 297, France */
    ULIB_CS_EBCDIC_1147,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_420,                     /* IBM EBCDIC code page 420, Arabic 1 */
    ULIB_CS_EBCDIC_424,                     /* IBM EBCDIC code page 424, Hebrew 1 */
    ULIB_CS_EBCDIC_500,                     /* IBM EBCDIC code page 500, International */
    ULIB_CS_EBCDIC_1148,                    /* same as above with Euro update */
    ULIB_CS_EBCDIC_870,                     /* IBM EBCDIC code page 870, Latin-2 Multilingual */
    ULIB_CS_EBCDIC_871,                     /* IBM EBCDIC code page 871, Iceland */
    ULIB_CS_EBCDIC_918,                     /* IBM EBCDIC code page 918, Arabic 2 */
    ULIB_CS_EBCDIC_1149,                    /* IBM 1149-1/695-1, EBCDIC Iceland */
    ULIB_CS_EBCDIC_1047,                    /* IBM EBCDIC code page 1047, MVS C compiler */
    ULIB_CS_EBCDIC_803,                     /* IBM EBCDIC code page 803, Hebrew Old */
    ULIB_CS_EBCDIC_875,                     /* IBM EBCDIC code page 875, Greek */
    ULIB_CS_EBCDIC_924,                     /* IBM EBCDIC code page 924, Latin9 */
    ULIB_CS_EBCDIC_1153,                    /* IBM EBCDIC code page 1153, Latin2 */
    ULIB_CS_EBCDIC_1122,                    /* IBM EBCDIC code page 1122, Estonian */
    ULIB_CS_EBCDIC_1157,                    /* IBM EBCDIC code page 1157, Estonian with Euro */
    ULIB_CS_EBCDIC_1112,                    /* IBM EBCDIC code page 1112, Baltic */
    ULIB_CS_EBCDIC_1156,                    /* IBM EBCDIC code page 1156, Baltic with Euro */
    ULIB_CS_EBCDIC_4899,                    /* IBM EBCDIC code page 4899, Hebrew with Euro */
    ULIB_CS_EBCDIC_12712,                   /* IBM EBCDIC code page 12712, Hebrew */
    ULIB_CS_EBCDIC_1097,                    /* IBM EBCDIC code page 1097, Farsi */
    ULIB_CS_EBCDIC_1018,                    /* IBM EBCDIC code page 1018, Finland */
    ULIB_CS_EBCDIC_1132,                    /* IBM EBCDIC code page 1132, Lao */
    ULIB_CS_EBCDIC_1137,                    /* IBM EBCDIC code page 1137, Devanagari */
    ULIB_CS_EBCDIC_1025,                    /* IBM EBCDIC code page 1025, Cyrillic */
    ULIB_CS_EBCDIC_1154,                    /* IBM EBCDIC code page 1154, Cyrillic with Euro */
    ULIB_CS_EBCDIC_1026,                    /* IBM EBCDIC code page 1026, Turkish / Latin5 */
    ULIB_CS_EBCDIC_1155,                    /* IBM EBCDIC code page 1155, Turkish with Euro */
    ULIB_CS_EBCDIC_1123,                    /* IBM EBCDIC code page 1123, Ukranian */
    ULIB_CS_EBCDIC_1158,                    /* IBM EBCDIC code page 1158, Ukranian with Euro */
    ULIB_CS_EBCDIC_838,                     /* IBM EBCDIC code page 838, Thai */
    ULIB_CS_EBCDIC_1160,                    /* IBM EBCDIC code page 1160, Thai with Euro */
    ULIB_CS_EBCDIC_1130,                    /* IBM EBCDIC code page 1130, Vietnamese */
    ULIB_CS_EBCDIC_1164,                    /* IBM EBCDIC code page 1164, Vietnamese with Euro */
    ULIB_CS_EBCDIC_4517,                    /* IBM EBCDIC code page 4517, Arabic French */
    ULIB_CS_EBCDIC_4971,                    /* IBM EBCDIC code page 4971, Greek */
    ULIB_CS_EBCDIC_9067,                    /* IBM EBCDIC code page 9067, Greek 2005 */
    ULIB_CS_EBCDIC_16804,                   /* IBM EBCDIC code page 16804, Arabic */

    /* SBCS others */
    ULIB_CS_KOI8R,                          /* Russian and Cyrillic (KOI8-R) */
    ULIB_CS_KOI8U,                          /* Ukranian (KOI8-U) */
    ULIB_CS_EUC_TH,                         /* EUC Thailand */
    ULIB_CS_IBM1162,                        /* Windows Thai with Euro */
    ULIB_CS_DECMCS,                         /* DEC-MCS */
    ULIB_CS_HP_ROMAN8,                      /* HP Roman8 */
    ULIB_CS_IBM901,                         /* IBM Baltic ISO-8 CCSID 901 */
    ULIB_CS_IBM902,                         /* IBM Estonia ISO-8 with Euro CCSID 902 */
    ULIB_CS_IBM916,                         /* IBM ISO8859-8 CCSID 916 */
    ULIB_CS_IBM922,                         /* IBM Estonia ISO-8 CCSID 922 */
    ULIB_CS_IBM1006,                        /* IBM Urdu ISO-8 CCSID 1006 */
    ULIB_CS_IBM1098,                        /* IBM Farsi PC CCSID 1098 */
    ULIB_CS_IBM1124,                        /* IBM Ukranian ISO-8 CCSID 1124 */
    ULIB_CS_IBM1125,                        /* IBM Ukranian without Euro CCSID 1125 */ 
    ULIB_CS_IBM1129,                        /* IBM Vietnamese without Euro CCSID 1129 */
    ULIB_CS_IBM1131,                        /* IBM Belarusi CCSID 1131 */
    ULIB_CS_IBM1133,                        /* IBM Lao CCSID 1133 */
    ULIB_CS_IBM4909,                        /* IBM Greek Latin ASCII CCSID 4909 */
    ULIB_CS_JISX201,                        /* JIS X201 Japanese */
    ULIB_CS_KOI8RU,                         /* IBM KOI8-RU */
    ULIB_CS_IBM1046,                        /* IBM Arabic Generic CCSID 1046 */
    ULIB_CS_IBM274,                         /* IBM EBCDIC Belgium */
    ULIB_CS_IBM425,                         /* IBM EBCDIC Arabic */

    /* added from release 12.3 */
    ULIB_CS_IBM5233,                        /* IBM 5233 EBCDIC Devanagari including Indian Rupee */

    ULIB_CS_SBCS_END,                       /* End marker SBCS */
    ULIB_CS_MBCS_BEGIN = 5000,              /* Start marker MBCS */

    /* Windows MBCS */
    ULIB_CS_WCP932   = ULIB_CS_MBCS_BEGIN,  /* Windows Japanese */
    ULIB_CS_WCP936,                         /* Windows Simplified Chinese */
    ULIB_CS_IBM942,                         /* IBM Windows Japanese */
    ULIB_CS_WCP949,                         /* Windows Korean */
    ULIB_CS_WCP950,                         /* Windows Traditional Chinese */

    /* EUC variant */
    ULIB_CS_EUC_JP,                         /* EUC Japanese */
    ULIB_CS_IBM_EUC_JP,                     /* IBM-33722 EUC Japanese, Tilde difference */
    ULIB_CS_EUC_CN,                         /* EUC Simplified Chinese */
    ULIB_CS_EUC_KR,                         /* EUC Korean */
    ULIB_CS_EUC_TW,                         /* EUC Traditional Chinese */

    /* CJK EBCDIC */
    ULIB_CS_CCSID_930,                      /* IBM CCSID 930/5026 Japanese */
    ULIB_CS_CCSID_933,                      /* IBM CCSID 933 Korean */
    ULIB_CS_CCSID_935,                      /* IBM CCSID 935 Simplified Chinese */
    ULIB_CS_CCSID_937,                      /* IBM CCSID 937 Traditional Chinese */
    ULIB_CS_CCSID_939,                      /* IBM CCSID 939/5035 Japanese */
    ULIB_CS_CCSID_1364,                     /* IBM CCSID 1364 Korean */
    ULIB_CS_CCSID_1371,                     /* IBM CCSID 1371 Traditional Chinese */
    ULIB_CS_CCSID_1388,                     /* IBM CCSID 1388 Simplified Chinese */
    ULIB_CS_CCSID_1390,                     /* IBM CCSID 1390 Japanese */
    ULIB_CS_CCSID_1399,                     /* IBM CCSID 1399 Japanese */
    ULIB_CS_CCSID_5123,                     /* IBM CCSID 5123 Japanese (SBCS) */
    ULIB_CS_CCSID_8482,                     /* IBM CCSID 8482 Japanese (SBCS) */
    ULIB_CS_CCSID_13218,                    /* IBM CCSID 13218 Japanese */

    /* CJK others */
    ULIB_CS_CCSID_16684,                    /* IBM CCSID 16684 Japanese */
    ULIB_CS_SHIFTJIS,                       /* Japanese Shift-JIS, Tilde 0x8160 mapped to U+301C */
    ULIB_CS_GB18030,                        /* China's new national standard - similar to UNICODE */
    ULIB_CS_GB2312_80,                      /* GB-2312-80 */
    ULIB_CS_GBK,                            /* GBK, Windows-936-2000 */
    ULIB_CS_HZ_GB2312,                      /* HZ GB2312 */
    ULIB_CS_CCSID_1381,                     /* IBM CCSID 1381 Simplified Chinese */
    ULIB_CS_BIG5,                           /* Big5, Traditional Chinese */
    ULIB_CS_BIG5HKSCS,                      /* Big5, HongKong ext. */
    ULIB_CS_BIG5HKSCS2001,                  /* Big5, HongKong ext. HKSCS-2001 */
    ULIB_CS_IBM_BIG5,                       /* IBM Big5, CCSID 950 */
    ULIB_CS_CCSID_949,                      /* IBM CCSID 949 Korean */
    ULIB_CS_CCSID_949C,                     /* IBM CCSID 949 Korean, has backslash */
    ULIB_CS_IBM_KSC5601,                    /* IBM CCSID 971 Korean EUC, KSC5601 1989 */
    ULIB_CS_CCSID_1363,                     /* IBM CCSID 1363, Korean */
    ULIB_CS_CCSID_1350,                     /* IBM CCSID 1350, Japanese x-eucJP-Open */

    /* added from release 12.3 */
    ULIB_CS_IBM_837,                        /* Simplified Chinese (DBCS subset of ibm-5031) */
    ULIB_CS_EUC_JP_2007,                    /* updated EUC Japanese */
    ULIB_CS_EUC_TW_2014,                    /* updated EUC Tranditional Chinese */

    ULIB_CS_MBCS_END,                       /* End marker MBCS */
    ULIB_CS_ORACLECS_BEGIN = 10000,         /* Start marker Oracle cahracter sets */

    /* Oracle Character Sets, no ICU starndatd */
    ULIB_CS_AR8ADOS710T = ULIB_CS_ORACLECS_BEGIN,     /* Arabic MS-DOS 710 8-bit Latin/Arabic */
    ULIB_CS_AR8ADOS710,                     /* Arabic MS-DOS 710 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8ADOS720T,                    /* Arabic MS-DOS 720 8-bit Latin/Arabic */
    ULIB_CS_AR8ADOS720,                     /* Arabic MS-DOS 720 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8APTEC715T,                   /* APTEC 715 8-bit Latin/Arabic */
    ULIB_CS_AR8APTEC715,                    /* APTEC 715 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8ARABICMACS,                  /* Mac Server 8-bit Latin/Arabic */
    ULIB_CS_AR8ARABICMACT,                  /* Mac 8-bit Latin/Arabic */
    ULIB_CS_AR8ARABICMAC,                   /* Mac Client 8-bit Latin/Arabic */
    ULIB_CS_AR8ASMO708PLUS,                 /* ASMO 708 Plus 8-bit Latin/Arabic */
    ULIB_CS_AR8ASMO8X,                      /* ASMO Extended 708 8-bit Latin/Arabic */
    ULIB_CS_AR8EBCDIC420S,                  /* EBCDIC Code Page 420 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8EBCDICX,                     /* EBCDIC XBASIC Server 8-bit Latin/Arabic */
    ULIB_CS_AR8HPARABIC8T,                  /* HP 8-bit Latin/Arabic */
    ULIB_CS_AR8ISO8859P6,                   /* ISO 8859-6 Latin/Arabic */
    ULIB_CS_AR8MSWIN1256,                   /* MS Windows Code Page 1256 8-Bit Latin/Arabic */
    ULIB_CS_AR8MUSSAD768T,                  /* Mussa'd Alarabi/2 768 8-bit Latin/Arabic */
    ULIB_CS_AR8MUSSAD768,                   /* Mussa'd Alarabi/2 768 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8NAFITHA711T,                 /* Nafitha International 711 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8NAFITHA711,                  /* Nafitha Enhanced 711 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8NAFITHA721T,                 /* Nafitha International 721 8-bit Latin/Arabic */
    ULIB_CS_AR8NAFITHA721,                  /* Nafitha International 721 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8SAKHR706,                    /* SAKHR 706 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8SAKHR707T,                   /* SAKHR 707 8-bit Latin/Arabic */
    ULIB_CS_AR8SAKHR707,                    /* SAKHR 707 Server 8-bit Latin/Arabic */
    ULIB_CS_AR8XBASIC,                      /* XBASIC 8-bit Latin/Arabic */
    ULIB_CS_AZ8ISO8859P9E,                  /* ISO 8859-9 Azerbaijani */
    ULIB_CS_BG8MSWIN,                       /* MS Windows 8-bit Bulgarian Cyrillic */
    ULIB_CS_BG8PC437S,                      /* IBM-PC Code Page 437 8-bit (Bulgarian Modification) */
    ULIB_CS_BLT8CP921,                      /* Latvian Standard LVS8-92(1) Windows/Unix 8-bit Baltic */
    ULIB_CS_BLT8EBCDIC1112S,                /* EBCDIC Code Page 1112 8-bit Server Baltic Multilingual */
    ULIB_CS_BLT8EBCDIC1112,                 /* EBCDIC Code Page 1112 8-bit Baltic Multilingual */
    ULIB_CS_BLT8ISO8859P13,                 /* ISO 8859-13 Baltic */
    ULIB_CS_BLT8MSWIN1257,                  /* MS Windows Code Page 1257 8-bit Baltic */
    ULIB_CS_BLT8PC775,                      /* IBM-PC Code Page 775 8-bit Baltic */
    ULIB_CS_BN8BSCII,                       /* Bangladesh National Code 8-bit BSCII */
    ULIB_CS_CDN8PC863,                      /* IBM-PC Code Page 863 8-bit Canadian French */
    ULIB_CS_CE8BS2000,                      /* Siemens EBCDIC.DF.04-2 8-bit Central European */
    ULIB_CS_CEL8ISO8859P14,                 /* ISO 8859-14 Celtic */
    ULIB_CS_CH7DEC,                         /* DEC VT100 7-bit Swiss (German/French) */
    ULIB_CS_CL8BS2000,                      /* Siemens EBCDIC.EHC.LC 8-bit Latin/Cyrillic-1 */
    ULIB_CS_CL8EBCDIC1025C,                 /* EBCDIC Code Page 1025 Client 8-bit Cyrillic */
    ULIB_CS_CL8EBCDIC1025R,                 /* EBCDIC Code Page 1025 Server 8-bit Cyrillic */
    ULIB_CS_CL8EBCDIC1025S,                 /* EBCDIC Code Page 1025 Server 8-bit Cyrillic */
    ULIB_CS_CL8EBCDIC1025,                  /* EBCDIC Code Page 1025 8-bit Cyrillic */
    ULIB_CS_CL8EBCDIC1025X,                 /* EBCDIC Code Page 1025 (Modified)8-bit Cyrillic */
    ULIB_CS_CL8EBCDIC1158R,                 /* EBCDIC Code Page 1158 Server 8-bit Cyrillic */
    ULIB_CS_CL8EBCDIC1158,                  /* EBCDIC Code Page 1158 8-bit Cyrillic */
    ULIB_CS_CL8ISO8859P5,                   /* ISO 8859-5 Latin/Cyrillic */
    ULIB_CS_CL8ISOIR111,                    /* SOIR111 Cyrillic */
    ULIB_CS_CL8KOI8R,                       /* RELCOM Internet Standard 8-bit Latin/Cyrillic */
    ULIB_CS_CL8KOI8U,                       /* KOI8 Ukrainian Cyrillic */
    ULIB_CS_CL8MACCYRILLICS,                /* Mac Server 8-bit Latin/Cyrillic */
    ULIB_CS_CL8MACCYRILLIC,                 /* Mac Client 8-bit Latin/Cyrillic */
    ULIB_CS_CL8MSWIN1251,                   /* MS Windows Code Page 1251 8-bit Latin/Cyrillic */
    ULIB_CS_D7DEC,                          /* DEC VT100 7-bit German */
    ULIB_CS_D7SIEMENS9780X,                 /* Siemens 97801/97808 7-bit German */
    ULIB_CS_D8BS2000,                       /* Siemens 9750-62 EBCDIC 8-bit German */
    ULIB_CS_D8EBCDIC1141,                   /* EBCDIC Code Page 1141 8-bit Austrian German */
    ULIB_CS_D8EBCDIC273,                    /* EBCDIC Code Page 273/1 8-bit Austrian German */
    ULIB_CS_DK7SIEMENS9780X,                /* Siemens 97801/97808 7-bit Danish */
    ULIB_CS_DK8BS2000,                      /* Siemens 9750-62 EBCDIC 8-bit Danish */
    ULIB_CS_DK8EBCDIC1142,                  /* EBCDIC Code Page 1142 8-bit Danish */
    ULIB_CS_DK8EBCDIC277,                   /* EBCDIC Code Page 277/1 8-bit Danish */
    ULIB_CS_E7DEC,                          /* DEC VT100 7-bit Spanish */
    ULIB_CS_E7SIEMENS9780X,                 /* Siemens 97801/97808 7-bit Spanish */
    ULIB_CS_E8BS2000,                       /* Siemens 9750-62 EBCDIC 8-bit Spanish */
    ULIB_CS_EE8BS2000,                      /* Siemens EBCDIC.EHC.L2 8-bit East European */
    ULIB_CS_EE8EBCDIC870C,                  /* EBCDIC Code Page 870 Client 8-bit East European */
    ULIB_CS_EE8EBCDIC870S,                  /* EBCDIC Code Page 870 Server 8-bit East European */
    ULIB_CS_EE8EBCDIC870,                   /* EBCDIC Code Page 870 8-bit East European */
    ULIB_CS_EE8ISO8859P2,                   /* ISO 8859-2 East European */
    ULIB_CS_EE8MACCES,                      /* Mac Server 8-bit Central European */
    ULIB_CS_EE8MACCE,                       /* Mac Client 8-bit Central European */
    ULIB_CS_EE8MACCROATIANS,                /* Mac Server 8-bit Croatian */
    ULIB_CS_EE8MACCROATIAN,                 /* Mac Client 8-bit Croatian */
    ULIB_CS_EE8MSWIN1250,                   /* MS Windows Code Page 1250 8-bit East European */
    ULIB_CS_EE8PC852,                       /* IBM-PC Code Page 852 8-bit East European */
    ULIB_CS_EEC8EUROASCI,                   /* EEC Targon 35 ASCI West European/Greek */
    ULIB_CS_EEC8EUROPA3,                    /* EEC EUROPA3 8-bit West European/Greek */
    ULIB_CS_EL8DEC,                         /* DEC 8-bit Latin/Greek */
    ULIB_CS_EL8EBCDIC423R,                  /* IBM EBCDIC Code Page 423 for RDBMS server-side */
    ULIB_CS_EL8EBCDIC875R,                  /* EBCDIC Code Page 875 Server 8-bit Greek */
    ULIB_CS_EL8EBCDIC875S,                  /* EBCDIC Code Page 875 Server 8-bit Greek */
    ULIB_CS_EL8EBCDIC875,                   /* EBCDIC Code Page 875 8-bit Greek */
    ULIB_CS_EL8GCOS7,                       /* Bull EBCDIC GCOS7 8-bit Greek */
    ULIB_CS_EL8ISO8859P7,                   /* ISO 8859-7 Latin/Greek */
    ULIB_CS_EL8MACGREEKS,                   /* Mac Server 8-bit Greek */
    ULIB_CS_EL8MACGREEK,                    /* Mac Client 8-bit Greek */
    ULIB_CS_EL8MSWIN1253,                   /* MS Windows Code Page 1253 8-bit Latin/Greek */
    ULIB_CS_EL8PC437S,                      /* IBM-PC Code Page 437 8-bit (Greek modification) */
    ULIB_CS_EL8PC737,                       /* IBM-PC Code Page 737 8-bit Greek/Latin */
    ULIB_CS_EL8PC851,                       /* IBM-PC Code Page 851 8-bit Greek/Latin */
    ULIB_CS_EL8PC869,                       /* IBM-PC Code Page 869 8-bit Greek/Latin */
    ULIB_CS_ET8MSWIN923,                    /* MS Windows Code Page 923 8-bit Estonian */
    ULIB_CS_F7DEC,                          /* DEC VT100 7-bit French */
    ULIB_CS_F7SIEMENS9780X,                 /* Siemens 97801/97808 7-bit French */
    ULIB_CS_F8BS2000,                       /* Siemens 9750-62 EBCDIC 8-bit French */
    ULIB_CS_F8EBCDIC1147,                   /* EBCDIC Code Page 1147 8-bit French */
    ULIB_CS_F8EBCDIC297,                    /* EBCDIC Code Page 297 8-bit French */
    ULIB_CS_HU8ABMOD,                       /* Hungarian 8-bit Special AB Mod */
    ULIB_CS_HU8CWI2,                        /* Hungarian 8-bit CWI-2 */
    ULIB_CS_I7DEC,                          /* DEC VT100 7-bit Italian */
    ULIB_CS_I7SIEMENS9780X,                 /* Siemens 97801/97808 7-bit Italian */
    ULIB_CS_I8EBCDIC1144,                   /* EBCDIC Code Page 1144 8-bit Italian */
    ULIB_CS_I8EBCDIC280,                    /* EBCDIC Code Page 280/1 8-bit Italian */
    ULIB_CS_IN8ISCII,                       /* Multiple-Script Indian Standard 8-bit Latin/Indian Languages */
    ULIB_CS_IS8MACICELANDICS,               /* Mac Server 8-bit Icelandic */
    ULIB_CS_IS8MACICELANDIC,                /* Mac Client 8-bit Icelandic */
    ULIB_CS_IS8PC861,                       /* IBM-PC Code Page 861 8-bit Icelandic */
    ULIB_CS_IW7IS960,                       /* Israeli Standard 960 7-bit Latin/Hebrew */
    ULIB_CS_IW8EBCDIC1086,                  /* EBCDIC Code Page 1086 8-bit Hebrew */
    ULIB_CS_IW8EBCDIC424S,                  /* EBCDIC Code Page 424 Server 8-bit Latin/Hebrew */
    ULIB_CS_IW8EBCDIC424,                   /* EBCDIC Code Page 424 8-bit Latin/Hebrew */
    ULIB_CS_IW8ISO8859P8,                   /* ISO 8859-8 Latin/Hebrew */
    ULIB_CS_IW8MACHEBREWS,                  /* Mac Server 8-bit Hebrew */
    ULIB_CS_IW8MACHEBREW,                   /* Mac Client 8-bit Hebrew */
    ULIB_CS_IW8MSWIN1255,                   /* MS Windows Code Page 1255 8-bit Latin/Hebrew */
    ULIB_CS_IW8PC1507,                      /* IBM-PC Code Page 1507/862 8-bit Latin/Hebrew */
    ULIB_CS_JA16DBCS,                       /* IBM EBCDIC 16-bit Japanese */
    ULIB_CS_JA16EBCDIC930,                  /* IBM DBCS Code Page 290 16-bit Japanese */
    ULIB_CS_JA16EUCTILDE,                   /* The same as JA16EUC except for the way that the wave dash and the tilde are mapped to and from Unicode */
    ULIB_CS_JA16EUC,                        /* EUC 24-bit Japanese */
    ULIB_CS_JA16EUCYEN,                     /* EUC 24-bit Japanese with '\' mapped to the Japanese yen character */
    ULIB_CS_JA16MACSJIS,                    /* Mac client Shift-JIS 16-bit Japanese */
    ULIB_CS_JA16SJISTILDE,                  /* The same as JA16SJIS except for the way that the wave dash and the tilde are mapped to and from Unicode */
    ULIB_CS_JA16SJIS,                       /* Shift-JIS 16-bit Japanese */
    ULIB_CS_JA16SJISYEN,                    /* Shift-JIS 16-bit Japanese with '\' mapped to the Japanese yen character */
    ULIB_CS_JA16VMS,                        /* JVMS 16-bit Japanese */
    ULIB_CS_KO16DBCS,                       /* IBM EBCDIC 16-bit Korean */
    ULIB_CS_KO16KSC5601,                    /* KSC5601 16-bit Korean */
    ULIB_CS_KO16KSCCS,                      /* KSCCS 16-bit Korean */
    ULIB_CS_KO16MSWIN949,                   /* MS Windows Code Page 949 Korean */
    ULIB_CS_LA8ISO6937,                     /* ISO 6937 8-bit Coded Character Set for Text Communication */
    ULIB_CS_LA8PASSPORT,                    /* German Government Printer 8-bit All-European Latin */
    ULIB_CS_LT8MSWIN921,                    /* MS Windows Code Page 921 8-bit Lithuanian */
    ULIB_CS_LT8PC772,                       /* IBM-PC Code Page 772 8-bit Lithuanian (Latin/Cyrillic) */
    ULIB_CS_LT8PC774,                       /* IBM-PC Code Page 774 8-bit Lithuanian (Latin) */
    ULIB_CS_LV8PC1117,                      /* IBM-PC Code Page 1117 8-bit Latvian */
    ULIB_CS_LV8PC8LR,                       /* Latvian Version IBM-PC Code Page 866 8-bit Latin/Cyrillic */
    ULIB_CS_LV8RST104090,                   /* IBM-PC Alternative Code Page 8-bit Latvian (Latin/Cyrillic) */
    ULIB_CS_N7SIEMENS9780X,                 /* Siemens 97801/97808 7-bit Norwegian */
    ULIB_CS_N8PC865,                        /* IBM-PC Code Page 865 8-bit Norwegian */
    ULIB_CS_NDK7DEC,                        /* DEC VT100 7-bit Norwegian/Danish */
    ULIB_CS_NE8ISO8859P10,                  /* ISO 8859-10 North European */
    ULIB_CS_NEE8ISO8859P4,                  /* ISO 8859-4 North and North-East European */
    ULIB_CS_NL7DEC,                         /* DEC VT100 7-bit Dutch */
    ULIB_CS_RU8BESTA,                       /* BESTA 8-bit Latin/Cyrillic */
    ULIB_CS_RU8PC855,                       /* IBM-PC Code Page 855 8-bit Latin/Cyrillic */
    ULIB_CS_RU8PC866,                       /* IBM-PC Code Page 866 8-bit Latin/Cyrillic */
    ULIB_CS_S7DEC,                          /* DEC VT100 7-bit Swedish */
    ULIB_CS_S7SIEMENS9780X,                 /* Siemens 97801/97808 7-bit Swedish */
    ULIB_CS_S8BS2000,                       /* Siemens 9750-62 EBCDIC 8-bit Swedish */
    ULIB_CS_S8EBCDIC1143,                   /* EBCDIC Code Page 1143 8-bit Swedish */
    ULIB_CS_S8EBCDIC278,                    /* EBCDIC Code Page 278/1 8-bit Swedish */
    ULIB_CS_SE8ISO8859P3,                   /* ISO 8859-3 South European */
    ULIB_CS_SF7ASCII,                       /* ASCII 7-bit Finnish */
    ULIB_CS_SF7DEC,                         /* DEC VT100 7-bit Finnish */
    ULIB_CS_TH8MACTHAIS,                    /* Mac Server 8-bit Latin/Thai */
    ULIB_CS_TH8MACTHAI,                     /* Mac Client 8-bit Latin/Thai */
    ULIB_CS_TH8TISASCII,                    /* Thai Industrial Standard 620-2533 - ASCII 8-bit */
    ULIB_CS_TH8TISEBCDICS,                  /* Thai Industrial Standard 620-2533 - EBCDIC Server 8-bit */
    ULIB_CS_TH8TISEBCDIC,                   /* Thai Industrial Standard 620-2533 - EBCDIC 8-bit */
    ULIB_CS_TIMESTEN8,                      /* TimesTen Legacy */
    ULIB_CS_TR7DEC,                         /* DEC VT100 7-bit Turkish */
    ULIB_CS_TR8DEC,                         /* DEC 8-bit Turkish */
    ULIB_CS_TR8EBCDIC1026S,                 /* EBCDIC Code Page 1026 Server 8-bit Turkish */
    ULIB_CS_TR8EBCDIC1026,                  /* EBCDIC Code Page 1026 8-bit Turkish */
    ULIB_CS_TR8MACTURKISHS,                 /* Mac Server 8-bit Turkish */
    ULIB_CS_TR8MACTURKISH,                  /* Mac Client 8-bit Turkish */
    ULIB_CS_TR8MSWIN1254,                   /* MS Windows Code Page 1254 8-bit Turkish */
    ULIB_CS_TR8PC857,                       /* IBM-PC Code Page 857 8-bit Turkish */
    ULIB_CS_US7ASCII,                       /* ASCII 7-bit American */
    ULIB_CS_US8BS2000,                      /* Siemens 9750-62 EBCDIC 8-bit American */
    ULIB_CS_US8ICL,                         /* ICL EBCDIC 8-bit American */
    ULIB_CS_US8PC437,                       /* IBM-PC Code Page 437 8-bit American */
    ULIB_CS_VN8MSWIN1258,                   /* MS Windows Code Page 1258 8-bit Vietnamese */
    ULIB_CS_VN8VN3,                         /* VN3 8-bit Vietnamese */
    ULIB_CS_WE8BS2000E,                     /* Siemens EBCDIC.DF.04-F 8-bit West European with Euro symbol */
    ULIB_CS_WE8BS2000L5,                    /* Siemens EBCDIC.DF.04-9 8-bit WE & Turkish */
    ULIB_CS_WE8BS2000,                      /* Siemens EBCDIC.DF.04-1 8-bit West European */
    ULIB_CS_WE8DEC,                         /* DEC 8-bit West European */
    ULIB_CS_WE8DG,                          /* DG 8-bit West European */
    ULIB_CS_WE8EBCDIC1047E,                 /* Latin 1/Open Systems 1047 */
    ULIB_CS_WE8EBCDIC1047,                  /* EBCDIC Code Page 1047 8-bit West European */
    ULIB_CS_WE8EBCDIC1140C,                 /* EBCDIC Code Page 1140 Client 8-bit West European */
    ULIB_CS_WE8EBCDIC1140,                  /* EBCDIC Code Page 1140 8-bit West European */
    ULIB_CS_WE8EBCDIC1145,                  /* EBCDIC Code Page 1145 8-bit West European */
    ULIB_CS_WE8EBCDIC1146,                  /* EBCDIC Code Page 1146 8-bit West European */
    ULIB_CS_WE8EBCDIC1148C,                 /* EBCDIC Code Page 1148 Client 8-bit West European */
    ULIB_CS_WE8EBCDIC1148,                  /* EBCDIC Code Page 1148 8-bit West European */
    ULIB_CS_WE8EBCDIC284,                   /* EBCDIC Code Page 284 8-bit Latin American/Spanish */
    ULIB_CS_WE8EBCDIC285,                   /* EBCDIC Code Page 285 8-bit West European */
    ULIB_CS_WE8EBCDIC37C,                   /* EBCDIC Code Page 37 8-bit Oracle/c */
    ULIB_CS_WE8EBCDIC37,                    /* EBCDIC Code Page 37 8-bit West European */
    ULIB_CS_WE8EBCDIC500C,                  /* EBCDIC Code Page 500 8-bit Oracle/c */
    ULIB_CS_WE8EBCDIC500,                   /* EBCDIC Code Page 500 8-bit West European */
    ULIB_CS_WE8EBCDIC871,                   /* EBCDIC Code Page 871 8-bit Icelandic */
    ULIB_CS_WE8EBCDIC924,                   /* Latin 9 EBCDIC 924 */
    ULIB_CS_WE8GCOS7,                       /* Bull EBCDIC GCOS7 8-bit West European */
    ULIB_CS_WE8HP,                          /* HP LaserJet 8-bit West European */
    ULIB_CS_WE8ICL,                         /* ICL EBCDIC 8-bit West European */
    ULIB_CS_WE8ISO8859P15,                  /* ISO 8859-15 West European */
    ULIB_CS_WE8ISO8859P1,                   /* ISO 8859-1 West European */
    ULIB_CS_WE8ISO8859P9,                   /* ISO 8859-9 West European & Turkish */
    ULIB_CS_WE8ISOICLUK,                    /* ICL special version ISO8859-1 */
    ULIB_CS_WE8MACROMAN8S,                  /* Mac Server 8-bit Extended Roman8 West European */
    ULIB_CS_WE8MACROMAN8,                   /* Mac Client 8-bit Extended Roman8 West European */
    ULIB_CS_WE8MSWIN1252,                   /* MS Windows Code Page 1252 8-bit West European */
    ULIB_CS_WE8NCR4970,                     /* NCR 4970 8-bit West European */
    ULIB_CS_WE8NEXTSTEP,                    /* NeXTSTEP PostScript 8-bit West European */
    ULIB_CS_WE8PC850,                       /* IBM-PC Code Page 850 8-bit West European */
    ULIB_CS_WE8PC858,                       /* IBM-PC Code Page 858 8-bit West European */
    ULIB_CS_WE8PC860,                       /* IBM-PC Code Page 860 8-bit West European */
    ULIB_CS_WE8ROMAN8,                      /* HP Roman8 8-bit West European */
    ULIB_CS_YUG7ASCII,                      /* ASCII 7-bit Yugoslavian */
    ULIB_CS_ZHS16CGB231280,                 /* CGB2312-80 16-bit Simplified Chinese */
    ULIB_CS_ZHS16DBCS,                      /* IBM EBCDIC 16-bit Simplified Chinese */
    ULIB_CS_ZHS16GBK,                       /* GBK 16-bit Simplified Chinese */
    ULIB_CS_ZHS16MACCGB231280,              /* Mac client CGB2312-80 16-bit Simplified Chinese */
    ULIB_CS_ZHT16BIG5,                      /* BIG5 16-bit Traditional Chinese */
    ULIB_CS_ZHT16CCDC,                      /* HP CCDC 16-bit Traditional Chinese */
    ULIB_CS_ZHT16DBCS,                      /* IBM EBCDIC 16-bit Traditional Chinese */
    ULIB_CS_ZHT16DBT,                       /* Taiwan Taxation 16-bit Traditional Chinese */
    ULIB_CS_ZHT16HKSCS31,                   /* MS Windows Code Page 950 with Hong Kong Supplementary Character Set HKSCS-2001 
                                               (character set conversion to and from Unicode is based on Unicode 3.1) */
    ULIB_CS_ZHT16HKSCS,                     /* MS Windows Code Page 950 with Hong Kong Supplementary Character Set HKSCS-2001 
                                               (character set conversion to and from Unicode is based on Unicode 3.0) */
    ULIB_CS_ZHT16MSWIN950,                  /* MS Windows Code Page 950 Traditional Chinese */
    ULIB_CS_ZHT32EUC,                       /* EUC 32-bit Traditional Chinese */
    ULIB_CS_ZHT32SOPS,                      /* SOPS 32-bit Traditional Chinese */
    ULIB_CS_ZHT32TRIS,                      /* TRIS 32-bit Traditional Chinese */

    ULIB_CS_INVALID                         /* invalid value */

} ULibCharSet;

/* check if UNICODE character set */
#define ULIB_IS_UNICODE_CS(cs)   ((cs == ULIB_CS_UTF8) || (cs == ULIB_CS_UTF16) ||       \
                                  (cs == ULIB_CS_UTF16LE) || (cs == ULIB_CS_UTF16BE) ||  \
                                  (cs == ULIB_CS_UTF32) ||                               \
                                  (cs == ULIB_CS_UTF32LE) || (cs == ULIB_CS_UTF32BE) ||  \
                                  (cs == ULIB_CS_UTF_EBCDIC))

/* make escaped */
#define ULIB_CS_ESCAPE(cs)       (ULibCharSet)(((int32_t)(cs) | ULIB_CS_ESCAPE_BIT))
                                                            
/* check if UTF-16 */
#define ULIB_IS_UTF16(cs)  ((cs == ULIB_CS_UTF16) ||                               \
                            (cs == ULIB_CS_UTF16LE) || (cs == ULIB_CS_UTF16BE))

/* check if UTF-32 */
#define ULIB_IS_UTF32(cs)  ((cs == ULIB_CS_UTF32) ||                               \
                            (cs == ULIB_CS_UTF32LE) || (cs == ULIB_CS_UTF32BE))

#endif
