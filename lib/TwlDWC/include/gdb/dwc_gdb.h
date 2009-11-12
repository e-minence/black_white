/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_GDB_H_
#define DWC_GDB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <dwc.h>
#include <nonport/dwc_nonport.h>

    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */



    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */
    
    /// SSL�ʐM�̏�Ԃ������񋓌^�ł��B
    typedef enum
    {
        DWC_GDB_SSL_TYPE_NONE,                  ///< SSL���g�p���܂���B
        DWC_GDB_SSL_TYPE_SERVER_AUTH,           ///< �T�[�o�F�؂��s��SSL�ŒʐM���܂��B
        DWC_GDB_SSL_TYPE_SERVER_CLIENT_AUTH,    ///< �T�[�o�F�؂ƃN���C�A���g�F�؂��s��SSL�ŒʐM���܂��B��Nitro,Twl�ł͎g���܂���B
        DWC_GDB_SSL_TYPE_NUM
    } DWCGdbSSLType;

    
    /// �t�B�[���h�̌^�������񋓌^�ł��B
    typedef enum
    {
        DWC_GDB_FIELD_TYPE_BYTE,            ///< 1�o�C�g�̕����Ȃ������ł��B
        DWC_GDB_FIELD_TYPE_SHORT,           ///< 2�o�C�g�̕����t�������ł��B
        DWC_GDB_FIELD_TYPE_INT,             ///< 4�o�C�g�̕����t�������ł��B
        DWC_GDB_FIELD_TYPE_FLOAT,           ///< 8�o�C�g�̕��������_���ł��B
        DWC_GDB_FIELD_TYPE_ASCII_STRING,    ///< �ꕶ��1�o�C�g�̕�����ł��B
        DWC_GDB_FIELD_TYPE_UNICODE_STRING,  ///< �ꕶ��2�o�C�g�̕�����ł��B
        DWC_GDB_FIELD_TYPE_BOOLEAN,         ///< �^�U�l�ł��B
        DWC_GDB_FIELD_TYPE_DATE_AND_TIME,   ///< UnixTime�ŕ\����鎞�Ԃł��B
        DWC_GDB_FIELD_TYPE_BINARY_DATA,     ///< �o�C�i���f�[�^�ł��B
        DWC_GDB_FIELD_TYPE_INT64,           ///< 8�o�C�g�̕����t�������ł��B
        DWC_GDB_FIELD_TYPE_NUM_FIELD_TYPES
    } DWCGdbFieldType;

    /// �G���[�������񋓌^�ł��B
    typedef enum
    {
        DWC_GDB_ERROR_NONE = 0,                 ///< ����ɏI�����܂����B
        DWC_GDB_ERROR_OUT_OF_MEMORY = 1,        ///< DWC_Init�ŗ^�����A���P�[�^���烁�������m�ۂł��܂���ł����B
        DWC_GDB_ERROR_NOT_AVAILABLE = 2,        ///<
        DWC_GDB_ERROR_CORE_SHUTDOWN = 3,        ///<
        DWC_GDB_ERROR_NOT_LOGIN = 4,            ///< WiFiConnection�ɐ��������O�C�����Ă��܂���B
        DWC_GDB_ERROR_NOT_AUTHENTICATED = 5,    ///< WiFiConnection�ɐ��������O�C�����Ă��܂���B
        DWC_GDB_ERROR_BAD_INPUT = 6,            ///<
        DWC_GDB_ERROR_BAD_TABLENAME = 7,        ///<
        DWC_GDB_ERROR_BAD_FIELDS = 8,           ///<
        DWC_GDB_ERROR_BAD_NUM_FIELDS = 9,       ///<
        DWC_GDB_ERROR_BAD_FIELD_NAME = 10,      ///<
        DWC_GDB_ERROR_BAD_FIELD_TYPE = 11,      ///<
        DWC_GDB_ERROR_BAD_FIELD_VALUE = 12,     ///<
        DWC_GDB_ERROR_BAD_OFFSET = 13,          ///<
        DWC_GDB_ERROR_BAD_MAX = 14,             ///<
        DWC_GDB_ERROR_BAD_RECORDIDS = 15,       ///<
        DWC_GDB_ERROR_BAD_NUM_RECORDIDS = 16,   ///<
        DWC_GDB_ERROR_UNKNOWN = 17,             ///<
        DWC_GDB_ERROR_ALREADY_INITIALIZED = 18, ///< ���ɏ���������Ă��܂��B
        DWC_GDB_ERROR_NOT_INITIALIZED = 19,     ///< ���������s���Ă��܂���B
        DWC_GDB_ERROR_IN_ASYNC_PROCESS = 20,    ///< �񓯊��������ł��B
        DWC_GDB_ERROR_GHTTP = 21,               ///< GHTTP�̃G���[���������܂����B
        DWC_GDB_ERROR_IN_DWC_ERROR = 22         ///< DWC�̃G���[���N�����Ă��܂��B�G���[�̑Ώ����s���Ă��������B
    } DWCGdbError;

    /// �񓯊������̌��ʂ������񋓌^�ł��B
    typedef enum
    {
        DWC_GDB_ASYNC_RESULT_NONE = 0,                  ///< �񓯊��������܂��I�����Ă��܂���B
        DWC_GDB_ASYNC_RESULT_SUCCESS = 1,               ///< �������܂����B
        DWC_GDB_ASYNC_RESULT_SECRET_KEY_INVALID = 2,
        DWC_GDB_ASYNC_RESULT_SERVICE_DISABLED = 3,
        DWC_GDB_ASYNC_RESULT_CONNECTION_TIMEOUT = 4,
        DWC_GDB_ASYNC_RESULT_CONNECTION_ERROR = 5,
        DWC_GDB_ASYNC_RESULT_MALFORMED_RESPONSE = 6,
        DWC_GDB_ASYNC_RESULT_OUT_OF_MEMORY = 7,
        DWC_GDB_ASYNC_RESULT_DATABASE_UNAVAILABLE = 8,
        DWC_GDB_ASYNC_RESULT_LOGIN_TICKET_INVALID = 9,
        DWC_GDB_ASYNC_RESULT_LOGIN_TICKET_EXPIRED = 10,
        DWC_GDB_ASYNC_RESULT_TABLE_NOT_FOUND = 11,
        DWC_GDB_ASYNC_RESULT_RECORD_NOT_FOUND = 12,
        DWC_GDB_ASYNC_RESULT_FIELD_NOT_FOUND = 13,
        DWC_GDB_ASYNC_RESULT_FIELD_TYPE_INVALID = 14,
        DWC_GDB_ASYNC_RESULT_NO_PERMISSION = 15,
        DWC_GDB_ASYNC_RESULT_RECORD_LIMIT_REACHED = 16,
        DWC_GDB_ASYNC_RESULT_ALREADY_RATED = 17,
        DWC_GDB_ASYNC_RESULT_NOT_RATEABLE = 18,
        DWC_GDB_ASYNC_RESULT_NOT_OWNED = 19,
        DWC_GDB_ASYNC_RESULT_FILTER_INVALID = 20,
        DWC_GDB_ASYNC_RESULT_SORT_INVALID = 21,
        DWC_GDB_ASYNC_RESULT_UNKNOWN_ERROR = 22,
        DWC_GDB_ASYNC_RESULT_BAD_HTTP_METHOD = 23,
        DWC_GDB_ASYNC_RESULT_BAD_FILE_COUNT = 24,
        DWC_GDB_ASYNC_RESULT_MISSING_PARAMETER = 25,
        DWC_GDB_ASYNC_RESULT_FILE_NOT_FOUND = 26,
        DWC_GDB_ASYNC_RESULT_FILE_TOO_LARGE = 27,
        DWC_GDB_ASYNC_RESULT_FILE_UNKNOWN_ERROR = 28,
        DWC_GDB_ASYNC_RESULT_GHTTP_ERROR = 29,          ///< GHTTP�̃G���[���������܂����B
        DWC_GDB_ASYNC_RESULT_TARGET_FILTER_INVALID = 30,
        DWC_GDB_ASYNC_RESULT_SERVER_ERROR = 31
    } DWCGdbAsyncResult;

    /// DWCGdb���C�u�����̏�Ԃ������񋓌^�ł��B
    typedef enum
    {
        DWC_GDB_STATE_UNINITIALIZED,    ///< �����������
        DWC_GDB_STATE_IDLE,             ///< ���������I�����A�񓯊��������s���Ă��Ȃ����
        DWC_GDB_STATE_IN_ASYNC_PROCESS, ///< �񓯊��������s���Ă�����
        DWC_GDB_STATE_ERROR_OCCURED     ///< �G���[�������������
    } DWCGdbState;


    /* -------------------------------------------------------------------------
            struct
       ------------------------------------------------------------------------- */

    /// �o�C�i���f�[�^�^�̒l�ł��B
    typedef struct
    {
        u8*     data;       ///< �f�[�^�̓��e�ł��B���̗̈�̓R�[���o�b�N�𔲂���Ɖ������܂��B
        s32     size;       ///< �f�[�^�̃T�C�Y�ł��B
    }
    DWCGdbBinaryData;

    /// �t�B�[���h�̒l�������܂��B
    typedef union
    {
        u8      int_u8;         ///< 1�o�C�g�̕����Ȃ������ł��B
        s16     int_s16;        ///< 2�o�C�g�̕����t�������ł��B
        s32     int_s32;        ///< 4�o�C�g�̕����t�������ł��B
        f32     float_f64;      ///< 8�o�C�g�̕��������_���ł��B
        char*   ascii_string;   ///< �ꕶ��1�o�C�g�̕�����ł��B
        u16*    wide_string;    ///< �ꕶ��2�o�C�g�̕�����ł��B
        int     boolean;        ///< �^�U�l�ł��B
        DWCTick datetime;       ///< UnixTime�ŕ\����鎞�Ԃł��B
        DWCGdbBinaryData    binary_data;	///< �o�C�i���f�[�^�ł��B
        s64     int_s64;        ///< 8�o�C�g�̕����t�������ł��B
    } DWCGdbValue;

    /// �t�B�[���h�������܂��B
    typedef struct
    {
        char*           name;   ///< �t�B�[���h�̖��O�ł��B
        DWCGdbFieldType type;   ///< �t�B�[���h�̌^�ł��B
        DWCGdbValue     value;  ///< �t�B�[���h�̒l�ł��B
    }
    DWCGdbField;

    /// �����̏����������܂��B
    ///
    /// target_filter�Asurrounding_num��ݒ肵���ꍇ�Aoffset��max�͖�������܂��B
    ///
    /// ownerids�Aownerid_num��ݒ肵���ꍇ�Aoffset�Atarget_filter�Asurrounding_num �͖�������܂��B
    /// �܂��A�ݒ肵���l�͍l������܂��񂪁Asort�ɂ͗L���ȕ����� (�Ⴆ�� recordid )���Amax�ɂ� 0 �ȊO�̐��̒l(�Ⴆ�� 1 )���A�ݒ肵�ĉ��� ���B
    ///
    /// unicode�t�B�[���h�̌����ŁAsort,target_filter���w�肷��ꍇ�ADWC_GdbUCS2ToUTF8Str�֐����g���Ȃǂ��āA�}���`�o�C�g��UTF-8������Ŏw�肷��K�v������܂��B
    typedef struct
    {
        char*   filter;         ///< SQL��WHERE��Ɠ������������܂��BNULL���w�肵���ꍇ�͎w�肵�Ȃ����ƂɂȂ�܂��B
        char*   sort;           ///< SQL��ORDER BY��Ɠ������������܂��BNULL���w�肵���ꍇ�͎w�肵�Ȃ����ƂɂȂ�܂��B
        int     offset;         ///< �������ʂ̉��Ԗڂ���擾���邩�w�肵�܂��B
        int     max;            ///< �������ʂ��ő傢������Ă��邩�w�肵�܂��B
        char*   target_filter;  ///< surrounding_num���g�p���Ď��ӂ̃��R�[�h���擾����ɂ́A���̃t�B���^�[�����R�[�h����������������悤�ɐݒ肷��K�v������܂��B
        ///< �g�p���Ȃ��ꍇ��NULL��^���邱�Ƃ��ł��܂��B
        int     surrounding_num; ///< target_filter�Ƒg�ݍ��킹�Ďg�p���܂��B1�ȏ�̐����w�肷��ƁA�������ʂ̃��R�[�h�̏㉺�w����Ԃ�̃��R�[�h���ꏏ�Ɏ擾���܂��B
        int*    ownerids;       ///< �������郌�R�[�h�̍쐬�҂̃v���t�@�C��ID��z��Ƃ��Ďw�肵�܂��Bownerid_num��0���w�肵���ꍇ��NULL��^���邱�Ƃ��ł��܂��B
        int     ownerid_num;    ///< ownerids�Ŏw�肵���z��̒������w�肵�܂��B�g�p���Ȃ��ꍇ��0�����Ă��������B
        BOOL    cache_flag;     ///< �������ʂ��T�[�o�ŃL���b�V�����邩�ۂ��������܂��B�L���b�V�����Ԃ�5���ł��B�T�[�o�̕��ׂ��y�����邽�߂ɂ��A�f�[�^�̍X�V���Ԃ�Z�����Ȃ���΂����Ȃ��ꍇ�������āA���̃t���O�͗L���ɂ���悤�ɂ��Ă��������B
    }
    DWCGdbSearchCond;

    /* -------------------------------------------------------------------------
            typedef - callback
       ------------------------------------------------------------------------- */

    /**
     *  �񓯊������̌��ʁA���R�[�h���擾����ۂ̃R�[���o�b�N�ł��B
     *
     *  �����ŗ^������t�B�[���h�̓R�[���o�b�N�𔲂���Ɖ������Ă��܂��܂��̂ŁA
     *  �K�v�ȃf�[�^�̓R�[���o�b�N���ŃR�s�[����悤�ɂ��Ă��������B
     *
     *  Param:  record_num   ���R�[�h�̐��ł��B
     *  Param:  records      ���R�[�h�̔z��ł��B
     *  Param:  user_param   �񓯊������̎��s���ɗ^�����p�����[�^�ł��B
    */
    typedef void (*DWCGdbGetRecordsCallback)(int record_num, DWCGdbField** records, void* user_param);

    /**
     *  �񓯊������̌��ʁA�t�@�C�����擾����ۂ̃R�[���o�b�N�ł��B
     *
     *  DWC_GdbDownloadFileAsync�ň���clear_buf��TRUE��^���Ă����ꍇ�́A����data�ŗ^������̈�̓R�[���o�b�N�𔲂���Ɖ������Ă��܂��܂��B
     *  FALSE��^���Ă����ꍇ�͉������܂���̂ŁA������Ă��悢�^�C�~���O�ɂȂ�����ADWC_Free�ŉ�����Ă��������B
     *
     *  Param:  data         �擾�����f�[�^�ł��B
     *  Param:  size         �擾�����f�[�^�T�C�Y�ł��B
     *  Param:  user_param   �񓯊������̎��s���ɗ^�����p�����[�^�ł��B
    */
    typedef void (*DWCGdbDownloadFileCallback)(const void* data, int size, void* user_param);


    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    //--- Auto Function Prototype --- Don't comment here.
    extern DWCGdbError DWC_GdbInitialize(int game_id, const DWCUserData* userdata, DWCGdbSSLType ssl_type);
    extern DWCGdbError DWC_GdbShutdown();
    extern void DWC_GdbProcess();
    extern DWCGdbState DWC_GdbGetState();
    extern DWCGdbAsyncResult DWC_GdbGetAsyncResult();
    extern DWCGdbField* DWC_GdbGetFieldByName(const char* field_name, DWCGdbField* fields, int field_num);
    extern DWCGdbError DWC_GdbCreateRecordAsync(const char* table_name, DWCGdbField* fields, int field_num, int* record_id);
    extern DWCGdbError DWC_GdbDeleteRecordAsync(const char* table_name, int record_id);
    extern DWCGdbError DWC_GdbUpdateRecordAsync(const char* table_name, int record_id, DWCGdbField* fields, int field_num);
    extern DWCGdbError DWC_GdbDownloadFileAsync(int file_id, int buf_size, BOOL clear_buf, DWCGdbDownloadFileCallback callback, void* user_param);
    extern DWCGdbError DWC_GdbUploadFileAsync(const void* data, int size, const char* file_name, int* file_id);
    extern DWCGdbError DWC_GdbGetMyRecordsAsync(const char* table_name, const char** field_names, int field_num, DWCGdbGetRecordsCallback callback, void* user_param);
    extern DWCGdbError DWC_GdbGetRecordLimitAsync(const char* table_name, int* limit_num, int* owned_num);
    extern DWCGdbError DWC_GdbGetRecordsAsync(const char* table_name, const int* record_ids, int record_num, const char** field_names, int field_num, DWCGdbGetRecordsCallback callback, void* user_param);
    extern DWCGdbError DWC_GdbRateRecordAsync(const char* table_name, int record_id, u8 rating);
    extern DWCGdbError DWC_GdbSearchRecordsAsync(const char* table_name, const char** field_names, int field_num, const DWCGdbSearchCond* search_cond, DWCGdbGetRecordsCallback callback, void* user_param);
    extern DWCGdbError DWC_GdbGetRandomRecordAsync(const char* table_name, const char** field_names, int field_num, const char* filter, DWCGdbGetRecordsCallback callback, void* user_param);
    extern DWCGdbError DWC_GdbGetRecordCountAsync(const char* table_name, const char* filter, BOOL cache_flag, int* count);
    extern int DWC_GdbUCS2ToUTF8Str(const u16* src, char* dst, const unsigned int dstlen);
    //--- End of Auto Function Prototype



#ifdef __cplusplus
}
#endif

#endif // DWC_GDB_H_
