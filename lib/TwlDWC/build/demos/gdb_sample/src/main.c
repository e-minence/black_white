/*===========================================================================*


   This demo program uses following database settings.

   ��table setting
   �@�@table name�@�@�@�F�@demotable
   �@  table type    �@�F�@Profile type
   �@�@permission    �@�F�@all checked (all permitted)
   �@�@limit per owner �F  10

   ��field setting�F
   �@�@recordid     [Int]
       ownerid      [Int]
       demoscore    [Int]
       demostage    [Short]
       demodate     [DateAndTime]
       demofile     [FileID]


 *===========================================================================*/


#include <nitro.h>
#include <dwc.h>
#include <gdb/dwc_gdb.h>
#include  <cstring>


// �w�b�_�t�@�C���C���N���[�h
//-------------------------------------
#include "main.h"
#include "init.h"
#include "utility.h"

// �萔�錾
//---------------------------------------------------------
#define FRIEND_LIST_LEN  64
#define TABLE_NAME "demotable"
#define TIMEOUT_MSEC (20*1000)

// �ȈՃf�[�^�x�[�X���C�u�����e�X�g�p�A�J�E���g�i�{�Ԃł͕ς���K�v������܂��j
#define GAME_NAME        "dwctest"
#define SECRET_KEY       "d4q9GZ"	// ���̃T���v�����g�p����V�[�N���b�g�L�[
#define GAME_PRODUCTID   10824		// ���̃T���v�����g�p����v���_�N�gID
#define GAME_ID           1408       // ���̃T���v�����g�p����Q�[��ID
#define INITIAL_CODE     'NTRJ'     // �C�j�V�����R�[�h

#define CREATE_NUM  4

//#define USE_AUTHSERVER_PRODUCTION // ���i�����F�؃T�[�o���g�p����ꍇ�L���ɂ���


// �O���[�o���ϐ��錾
//---------------------------------------------------------
DWCInetControl stConnCtrl;
static int gprofile_id;                       // profileid = ownerid


// �֐��v���g�^�C�v
//---------------------------------------------------------
static BOOL StartIPMain(void);
static void VBlankIntr(void);
static void login_callback(DWCError error, int profileid, void *param);
static DWCError wait_async_end(void);
static void gdb_demo_exit(void);
static void get_records_callback(int record_num, DWCGdbField** records, void* user_param);
static void print_field(DWCGdbField* field);
static void download_file_callback(const void* data, int size, void* user_param);
static void file_progress_callback(int bytesDone, int totalBytes, void* user_param);


/*---------------------------------------------------------------------------*
  �������m�ۊ֐�
 *---------------------------------------------------------------------------*/
static void*
AllocFunc( DWCAllocType name, u32   size, int align )
{
    void * ptr;
    OSIntrMode old;
    (void)name;
    (void)align;

    old = OS_DisableInterrupts();

    ptr = OS_AllocFromMain( size );

    OS_RestoreInterrupts( old );

    return ptr;
}

/*---------------------------------------------------------------------------*
  �������J���֐�
 *---------------------------------------------------------------------------*/
static void
FreeFunc( DWCAllocType name, void* ptr,  u32 size  )
{
    OSIntrMode old;
    (void)name;
    (void)size;

    if ( !ptr ) return;

    old = OS_DisableInterrupts();

    OS_FreeToMain( ptr );

    OS_RestoreInterrupts( old );
}


//=============================================================================
/*!
 *    @brief    IP�̎擾
 *
 *    @retval    TRUE        ����
 *    @retval    FALSE       ���s
 */
//=============================================================================
static BOOL StartIPMain(void)
{
    DWC_InitInet( &stConnCtrl );

    DWC_SetRwinSize(65535);

    DWC_ConnectInetAsync();

    // ����ȃX�e�[�g�܂ő҂B
    while ( !DWC_CheckInet() )
    {
        DWC_ProcessInet();
        OS_WaitVBlankIntr();
    }

    switch ( DWC_GetInetStatus() )
    {
    case DWC_CONNECTINET_STATE_ERROR:
    {
        // �G���[�\��
        DWCError err;
        int errcode;
        err = DWC_GetLastError( &errcode );

        OS_TPrintf("   Error occurred %d %d.\n", err, errcode );
    }
    DWC_ClearError();
    DWC_CleanupInet();
    break;

    case DWC_CONNECTINET_STATE_FATAL_ERROR:
    {
        // �G���[�\��
        DWCError err;
        int errcode;
        err = DWC_GetLastError( &errcode );

        OS_Panic("   Error occurred %d %d.\n", err, errcode );
    }
    break;

    case DWC_CONNECTINET_STATE_CONNECTED:
        OS_TPrintf("   CONNECTED!!!.\n");
        return TRUE;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  V�u�����N���荞�݊֐�
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}




/*---------------------------------------------------------------------------*
  ���O�C�������������Ƃ��ɌĂяo�����R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void
login_callback(DWCError error, int profileid, void *param)
{
    OS_TPrintf("gdb_sample_DEBUG: Login callback : %d\n", error );

    if ( error == DWC_ERROR_NONE )
    {
        *((DWCError*)param) = error;
        OS_TPrintf("gdb_sample_DEBUG: Login Success. ProfileID=%d\n", profileid );
        gprofile_id = profileid;
    }
    else
    {
        *((DWCError*)param) = error;
        OS_TPrintf("gdb_sample_DEBUG: Login Failed\n" );
    }
}


/*---------------------------------------------------------------------------*
  �񓯊������҂��֐�
 *---------------------------------------------------------------------------*/
static DWCError
wait_async_end()
{
    OSTick start_time = OS_GetTick();
    DWCError err = DWC_ERROR_NONE;
    int cancelflag = 1;
    
    for (;;)
    {
        DWCGdbState state  =  DWC_GdbGetState();
        if(state != DWC_GDB_STATE_IN_ASYNC_PROCESS &&
           state != DWC_GDB_STATE_IN_CANCEL_PROCESS)
        {
            break;
        }
        
        DWC_ProcessFriendsMatch();
        if ((err = DWC_GetLastError(NULL)) != DWC_ERROR_NONE)
	    {
            return err;
	    }
	    
        DWC_GdbProcess();
        
        // �^�C���A�E�g�m�F
        if(OS_TicksToMilliSeconds(OS_GetTick() - start_time) > TIMEOUT_MSEC && cancelflag)
        {
            OS_TPrintf("gdb_sample_DEBUG: wait async timeout.\n");
            DWC_GdbCancelRequest();
            cancelflag = 0;
        }
        OS_WaitVBlankIntr();
    }
    return err;
}


/*---------------------------------------------------------------------------*
  �f���v���O�����I�������֐�
 *---------------------------------------------------------------------------*/
static void
gdb_demo_exit()
{
    int errorCode;
    DWCErrorType errorType;
    
    // �G���[������΃G���[�R�[�h�ƃG���[�^�C�v��\��
    if( DWC_ERROR_NONE != DWC_GetLastErrorEx( &errorCode, &errorType ) )
    {
        OS_TPrintf("gdb_sample_DEBUG: Error code %d   DWCErrorType[%d].\n", errorCode, errorType );
    }
    DWC_GdbShutdown();           // �ȈՃf�[�^�x�[�X���C�u�����̏I��
    DWC_ShutdownFriendsMatch();  // Friend�}�b�`���C�u�����̏I��
    DWC_CleanupInet();
    OS_TPrintf("gdb_sample_DEBUG: OS Terminated.\n");
    Heap_Dump();

}


/*---------------------------------------------------------------------------*
  ���R�[�h�擾���̃R�[���o�b�N�֐��@-�@�ȈՃf�[�^�x�[�X���C�u����
 *---------------------------------------------------------------------------*/
static void
get_records_callback(int record_num, DWCGdbField** records, void* user_param)
{
    int i,k;

    for (i = 0; i < record_num; i++)
    {
        OS_TPrintf("gdb_sample_DEBUG:�@");
        for (k = 0; k < *(int*)user_param; k++)   // user_param -> field_num
        {
            print_field(&records[i][k]);
            OS_TPrintf(" ");
        }
        OS_TPrintf("\n");
    }
}


static void
print_field(DWCGdbField* field) // ���R�[�h���f�o�b�O�o�͂���B
{
    OS_TPrintf("%s[", field->name);
    switch (field->type)
    {
    case DWC_GDB_FIELD_TYPE_BYTE:
        OS_TPrintf("BYTE]:%d", field->value.int_u8);
        break;
    case DWC_GDB_FIELD_TYPE_SHORT:
        OS_TPrintf("SHORT]:%d", field->value.int_s16);
        break;
    case DWC_GDB_FIELD_TYPE_INT:
        OS_TPrintf("INT]:%d", field->value.int_s32);
        break;
    case DWC_GDB_FIELD_TYPE_FLOAT:
        OS_Printf("FLOAT]:%f", field->value.float_f64);
        break;
    case DWC_GDB_FIELD_TYPE_ASCII_STRING:
        OS_TPrintf("ASCII]:%s", field->value.ascii_string);
        break;
    case DWC_GDB_FIELD_TYPE_UNICODE_STRING:
        OS_Printf("UNICODE]");
        break;
    case DWC_GDB_FIELD_TYPE_BOOLEAN:
        OS_TPrintf("BOOL]:%d", field->value.boolean);
        break;
    case DWC_GDB_FIELD_TYPE_DATE_AND_TIME:
        OS_TPrintf("TIME]:%lld", field->value.datetime);
        break;
    case DWC_GDB_FIELD_TYPE_BINARY_DATA:
        OS_TPrintf("BINARY]:%d", field->value.binary_data.size);
        break;
    }
}


/*---------------------------------------------------------------------------*
  �t�@�C���擾���̃R�[���o�b�N�֐��@-�@�ȈՃf�[�^�x�[�X���C�u����
 *---------------------------------------------------------------------------*/
static void
download_file_callback(const void* data, int size, void* user_param)
{
#pragma unused(user_param)
#ifdef SDK_FINALROM
#pragma unused(data)
#pragma unused(size)
#endif
    OS_TPrintf("gdb_sample_DEBUG: size: %d data: %lld \n", size, *(s64*)data);

}


/*---------------------------------------------------------------------------*
  �t�@�C������M���̐i�s�󋵎擾�R�[���o�b�N�֐��@-�@�ȈՃf�[�^�x�[�X���C�u����
 *---------------------------------------------------------------------------*/
static void
file_progress_callback(int bytesDone, int totalBytes, void* user_param)
{
#pragma unused(user_param)

    OS_TPrintf("gdb_sample_DEBUG: bytesDone: %d totalBytes: %d \n", bytesDone, totalBytes);
}


//=============================================================================
/*!
 *    @brief    ���C��
 */
//=============================================================================
void NitroMain()
{
    int i;
    int ret = 0;

    DWCUserData     userdata;                    // ���[�U�[�f�[�^
    DWCFriendData   friendlist[FRIEND_LIST_LEN]; // �t�����h���X�g

    DWCGdbError          res;   // �ȈՃf�[�^�x�[�X���C�u�����̃G���[�擾�p
    DWCGdbState        state;   // �ȈՃf�[�^�x�[�X���C�u�����̏�Ԏ擾�p
    DWCGdbAsyncResult  result;  // �ȈՃf�[�^�x�[�X���C�u�������ʎ擾�p
    
    DWCError    err = DWC_ERROR_NONE;   // �񓯊��ʐM�҂���Ԏ��̃G���[�擾�p

    int record_id[CREATE_NUM];      // ���R�[�h�̍쐬�f���ō쐬�������R�[�hID
    int record_id_file;             // �t�@�C���A�b�v���[�h�p�̃��R�[�hID
    
    DWCGdbField fields_create[2];   // ���R�[�h���쐬����ۂɐݒ肷�郌�R�[�h���
    DWCGdbField fields_update[2];   // ���R�[�h���X�V����ۂɐݒ肷�郌�R�[�h���


    // initialize
    //-----------------------------------------------------------------------
    OS_Init();

    Heap_Init();
    Heap_SetDebug(TRUE);

    GX_Init();

    GX_DispOff();
    GXS_DispOff();

    // V�u�����N���荞�݋���
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    FS_Init( MI_DMA_MAX_NUM );

    // �\���J�n
    OS_WaitVBlankIntr();  // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();

    // �f�o�b�O�\�����x���w��
    DWC_SetReportLevel(DWC_REPORTFLAG_ALL);

    // DWC���C�u����������
#if defined( USE_AUTHSERVER_PRODUCTION )
    ret = DWC_InitForProduction( GAME_NAME, INITIAL_CODE, AllocFunc, FreeFunc );
#else
    ret = DWC_InitForDevelopment( GAME_NAME, INITIAL_CODE, AllocFunc, FreeFunc );
#endif
    
    OS_TPrintf( "DWC_InitFor*() result = %d\n", ret );

    if ( ret == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
        OS_TPrintf( "Wi-Fi setting might be broken.\n" );
    }
    
    
    //IP�̎擾
    while (!StartIPMain())
    {
        OS_Printf("error at StartIPMain()\n");
    }

    //-----------------------------------------------------------------------
    // ���O�C������
    //-----------------------------------------------------------------------
    //
    // �����Ń��[�U�[�f�[�^�̎擾�������s���Ă��������B�ȈՃf�[�^�x�[�X
    // ���C�u������GameSpy�Ɉˑ����Ă��邽�߁A���Ȃ��Ƃ���x�̓��O�C��������
    // �L����GameSpyProfileID���擾���Ă��郆�[�U�[�f�[�^�łȂ���Ώ������Ɏ�
    // �s���܂��B��ID�͖����ł��B
    //
    // ���ɉߋ��Ƀ��O�C�����Ď擾�������[�U�[�f�[�^���o�b�N�A�b�v�ɕۑ���
    // ��Ă���ꍇ�͂�����g�p���邱�Ƃ��ł��܂��B�������A�K�C�h���C���ɒ��
    // ���Ă���Ƃ���A�C���^�[�l�b�g�ɐڑ�����ۂɂ͂��̓s�x�K�����O�C����
    // ��K�v�����邱�Ƃɒ��ӂ��Ă�������
    //
    // �O���F�؃T�[�o�[�Ȃǂ��g�p���Ă���ꍇ�͗L����GameSpyProfileID���擾��
    // �邱�Ƃ͂ł��܂���̂ŁA�ȈՃf�[�^�x�[�X���C�u�����͎g�p�ł��܂���B��
    // ���������̏ꍇ��GameSpy�̃T�[�o�[�������g�p�ł��܂���B
    //


    // ���[�U�f�[�^���쐬����
    OS_TPrintf("gdb_sample_DEBUG: Creating new UserData\n");
    DWC_CreateUserData( &userdata );

    // �F�B���X�g������
    memset( &friendlist, 0, sizeof( friendlist ) );

    // ���[�U�f�[�^��\��
    DWC_ReportUserData( &userdata );
    OS_TPrintf("gdb_sample_DEBUG: Initialized\n");

    // �t�����h�}�b�`���C�u�����̏�����
    DWC_InitFriendsMatch( &userdata,
                          GAME_PRODUCTID,
                          SECRET_KEY,
                          0,
                          0,
                          friendlist,
                          FRIEND_LIST_LEN );

    // �A�v���P�[�V������GameSpy�T�[�o�𗘗p�\�ł��邩���ׁA�����[�g�F�؂��s���AWi-Fi�R�l�N�V�����֐ڑ�
    {
        int login_result = -1;

        DWC_LoginAsync((u16*)L"name", NULL, login_callback, &login_result);
        while (login_result == -1)
        {
            OS_WaitVBlankIntr();
            DWC_ProcessFriendsMatch();
            if (DWC_GetLastError(NULL) != DWC_ERROR_NONE) OS_Terminate();
        }

    }


    // �ȈՃf�[�^�x�[�X���C�u������������
    //-----------------------------------------------------------------------
    res = DWC_GdbInitialize(GAME_ID, &userdata, DWC_SSL_TYPE_SERVER_AUTH);
    
    if (res == DWC_GDB_ERROR_NONE)
    {
        OS_Printf("gdb_sample_DEBUG: DWC_GdbInitialize() success\n");
    }
    else
    {
        OS_Printf("gdb_sample_DEBUG: DWCGdbError[%d] in DWC_GdbInitialize().  %s line[%d]\n",res,__FILE__,__LINE__);
        gdb_demo_exit();
        return;
    }



    // ���R�[�h��V�K�ɍ쐬����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_1  *********  create record\n");

    MI_CpuClear8(record_id,sizeof(record_id));

    fields_create[0].name = "demoscore";
    fields_create[0].type = DWC_GDB_FIELD_TYPE_INT;
    fields_create[0].value.int_s32 = 1000;
    fields_create[1].name = "demostage";
    fields_create[1].type = DWC_GDB_FIELD_TYPE_SHORT;
    fields_create[1].value.int_u8 = 5;


    for (i=0; i<CREATE_NUM; i++) // ��ɑ��������Ȃǂ̃f���̂��߁A���R�[�h��V�K�ɂS�쐬���܂��B
    {
        res = DWC_GdbCreateRecordAsync(TABLE_NAME, fields_create, 2, &record_id[i]);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbCreateRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]  %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_Printf("error!! DWCGdbAsyncResult[%d].  LINE[%d]\n",result,__LINE__);
            gdb_demo_exit();
            return;
        }
    }



    // ���R�[�h���폜����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_2  *********  delete record\n");
    
    if(record_id[0] != 0)
    {
        res = DWC_GdbDeleteRecordAsync(TABLE_NAME, record_id[0]);// �ŏ��ɍ쐬���ꂽ���R�[�h���폜
    
        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbDeleteRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }
    else
    {
        OS_TPrintf("gdb_sample_DEBUG: Target record is not found. skip.\n");
    }


    // ���R�[�h���X�V����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_3  *********  update record\n");
    
    if(record_id[1] != 0)
    {
        fields_update[0].name = "demoscore";
        fields_update[0].type = DWC_GDB_FIELD_TYPE_INT;
        fields_update[0].value.int_s32 = 1500;    // 1000 ���������̂��A1500 �ɍX�V����
        fields_update[1].name = "demostage";
        fields_update[1].type = DWC_GDB_FIELD_TYPE_SHORT;
        fields_update[1].value.int_u8 = 10;        // 5 ���������̂��A10 �ɍX�V����
    
        res = DWC_GdbUpdateRecordAsync(TABLE_NAME, record_id[1], fields_update, 2); // 2�Ԗڂɍ쐬�������R�[�h���X�V
    
        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbUpdateRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
        
        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }
    else
    {
        OS_TPrintf("gdb_sample_DEBUG: Target record is not found. skip.\n");
    }


    // �����̃��R�[�h��S�Ď擾����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_4  *********  get my all records\n");
    {
        const char* field_names[4] = {"recordid","ownerid","demostage","demoscore"};  // �����Ŏ擾����t�B�[���h��
        int field_num = sizeof(field_names)/sizeof(field_names[0]);  // ��L�Őݒ肵���t�B�[���h���̑���

        res = DWC_GdbGetMyRecordsAsync(TABLE_NAME, field_names, field_num, get_records_callback, &field_num);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbGetMyRecordsAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }



    // �w�肵�����R�[�hID�̃��R�[�h��_���t������񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_5  *********  rate record\n");
    
    if(record_id[2] != 0)
    {
        res = DWC_GdbRateRecordAsync(TABLE_NAME, record_id[2], 5); // 3�Ԗڂɍ쐬�������R�[�h��]������i5�_�j
    
        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbRateRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
        
        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }
    else
    {
        OS_TPrintf("gdb_sample_DEBUG: Target record is not found. skip.\n");
    }


    // �w�肵�����R�[�hID�̃��R�[�h���擾����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_6  *********  ged specified record\n");
    
    if(record_id[1] != 0 && record_id[2] != 0)
    {
        const char* field_names[3] = { "recordid", "num_ratings", "average_rating" }; //�@�擾�������t�B�[���h���̐ݒ�
        int field_num = sizeof(field_names)/sizeof(field_names[0]);    // ��L�Őݒ肵���t�B�[���h���̑���
        int record_ids[2];

        // 2�Ԗڂ�3�Ԗڂɍ쐬�������R�[�h
        record_ids[0] = record_id[1];
        record_ids[1] = record_id[2];

        DWC_GdbGetRecordsAsync( TABLE_NAME,
                                record_ids,
                                sizeof(record_ids)/sizeof(record_ids[0]),
                                field_names,
                                field_num,
                                get_records_callback,
                                &field_num );

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbGetRecordsAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }
    else
    {
        OS_TPrintf("gdb_sample_DEBUG: Target record is not found. skip.\n");
    }



    // �w�肵�������Ń��R�[�h�������_���ɂP�擾����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_7  *********  ged record randomly under the specified condition(filter)\n");
    {
        const char* field_names[2] = { "recordid", "demoscore" };    //�@�擾�������t�B�[���h���̐ݒ�
        const char* filter = "demoscore = 1000"; // �����w��@�����_���Ɏ擾����͈͂����肷��

        int field_num = sizeof(field_names)/sizeof(field_names[0]);    // ��L�Őݒ肵���t�B�[���h���̑���

        res = DWC_GdbGetRandomRecordAsync(TABLE_NAME, field_names, field_num, filter, get_records_callback, &field_num);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbGetRandomRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }



    //  �e�[�u���ɐݒ肳�ꂽ�A�P���[�U�[���쐬���邱�Ƃ̂ł���
    //  ���R�[�h�̍ő�l�ƁA���ݍ쐬���Ă��鐔���擾����񓯊�����
    //  ���R�[�h�̍ő�l�́A�폜���ꂽ���R�[�h�ɂ��Ă̓J�E���g����܂���
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_8  *********  ged the count of record limit and now you have\n");
    {
        int limit_num;
        int owned_num;

        res = DWC_GdbGetRecordLimitAsync(TABLE_NAME, &limit_num, &owned_num);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbGetRecordLimitAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        OS_Printf("gdb_sample_DEBUG: limit_num = %d ,  owned_num = %d\n",limit_num,owned_num);

    }



    //  �t�@�C���Ɋւ��鏈���F
    //  �t�@�C���A�b�v���[�h��A�f�[�^�x�[�X�փt�@�C��ID�̓o�^���A
    //  �o�^�����t�@�C��ID���g���āA�t�@�C�����^�f�[�^�Ɋւ�������擾��A
    //  �t�@�C�����_�E�����[�h���܂��B
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_9  *********  upload a file, register the file_id, get the metadata, download the file.\n");
    {
        s64 data = 83912736414123Ull;            // �A�b�v���[�h�f�[�^
        const char* filename = "sample_file";    // �A�b�v���[�h�����t�@�C���ɕt����t�@�C����
        int file_id;    // �t�@�C��ID�i�[�p�ϐ�

        DWCGdbField fields[1]; // �t�@�C��ID���A�f�[�^�x�[�X�ɓo�^����Ƃ��ɗp����t�B�[���h���


        // �t�@�C�����A�b�v���[�h����񓯊��������J�n
        //-----------------------------------------------------------------------
        res = DWC_GdbUploadFileAsync(&data, sizeof(s64), filename, &file_id, file_progress_callback);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbUploadFileAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }


        record_id_file = 0;
        if(file_id != 0)
        {
            // �t�@�C��ID���f�[�^�x�[�X�֓o�^����񓯊��������J�n
            // �i���R�[�h��V�K�ɍ쐬����񓯊������j
            //-----------------------------------------------------------------------
    
            fields[0].name = "demofile";                    // <=--------------------------------
            fields[0].type = DWC_GDB_FIELD_TYPE_INT;        // <=  �t�@�C��ID�o�^�p���R�[�h���
            fields[0].value.int_s32 = file_id;              // <=--------------------------------
            
            res = DWC_GdbCreateRecordAsync(TABLE_NAME, fields, 1, &record_id_file);
            OS_TPrintf("gdb_sample_DEBUG: creating a record for fileid = %d\n",file_id);
    
            if (res != DWC_GDB_ERROR_NONE)
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbCreateRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
            if ((err = wait_async_end()) != DWC_ERROR_NONE)
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
            if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
            result = DWC_GdbGetAsyncResult();
            if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
    
    
            // �t�@�C�����_�E�����[�h����񓯊�����
            //-----------------------------------------------------------------------
    
            res = DWC_GdbDownloadFileAsync(file_id, 2048, TRUE, file_progress_callback, download_file_callback, NULL);
    
            if (res != DWC_GDB_ERROR_NONE)
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbDownloadFileAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
            if ((err = wait_async_end()) != DWC_ERROR_NONE)
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
            if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
    
            result = DWC_GdbGetAsyncResult();
            if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
            {
                OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
                gdb_demo_exit();
                return;
            }
        }
        else
        {
            OS_TPrintf("gdb_sample_DEBUG: Target record is not found. skip.\n");
        }

    }



    // �w�肵�������ɓ��Ă͂܂郌�R�[�h�̐����擾����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_10  *********  get the count of records which can pass the filter\n");
    {
        char filter[30]; // �����w��
        int count; // �擾�������R�[�h�����i�[����ϐ�
        OS_SNPrintf(filter,sizeof(filter),"ownerid = %d",gprofile_id); // gprofile_id �̓O���[�o���ϐ���profileID���i�[���Ă���

        res = DWC_GdbGetRecordCountAsync(TABLE_NAME, filter, FALSE, &count);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbDeleteRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

         if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }



    // �w�肵�����������Ń��R�[�h���擾����񓯊�����
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_11  *********  get records under the specified conditions(filter etc.)\n");
    {
        const char* field_names[5] = {  "recordid",
                                        "demofile",
                                        "demofile.size",            //<=-----------------------------
                                        "demofile.name",            //<= �t�@�C�����^�f�[�^�t�B�[���h
                                        "demofile.downloads"        //<=-----------------------------
                                     };

        const DWCGdbSearchCond cond = { "demofile.downloads = 1 or demoscore = 1000",
                                        "- recordid", // �~��
                                        0,
                                        3,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        FALSE
                                      };

        int field_num = sizeof(field_names)/sizeof(field_names[0]);    // ��L�Őݒ肵���t�B�[���h���̑���

        res = DWC_GdbSearchRecordsAsync(TABLE_NAME, field_names, field_num, &cond, get_records_callback, &field_num);

        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbSearchRecordsAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }

    }



    // �t�B�[���h�̔z�񂩂�A�w�肵�����O�̃t�B�[���h���擾����B
    // �T�[�o�Ƃ̒ʐM�͍s���܂���B���[�J���ȃf�[�^�ɑ΂��čs�������ł��B
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_12  *********  get a specified field from any field array.\n");
    {
        DWCGdbField fields[5];
        DWCGdbField *field_get;
        int field_num = sizeof(fields)/sizeof(fields[0]);  // ��L�Őݒ肵���t�B�[���h���̑���

        fields[0].name = "field_1";
        fields[0].type = DWC_GDB_FIELD_TYPE_INT;
        fields[0].value.int_s32 = 1;
        fields[1].name = "field_2";
        fields[1].type = DWC_GDB_FIELD_TYPE_INT;
        fields[1].value.int_s32 = 2;
        fields[2].name = "field_3";
        fields[2].type = DWC_GDB_FIELD_TYPE_INT;
        fields[2].value.int_s32 = 3;
        fields[3].name = "field_4";
        fields[3].type = DWC_GDB_FIELD_TYPE_INT;
        fields[3].value.int_s32 = 4;
        fields[4].name = "field_5";
        fields[4].type = DWC_GDB_FIELD_TYPE_INT;
        fields[4].value.int_s32 = 5;

        field_get = DWC_GdbGetFieldByName(fields[2].name, fields, field_num);

        OS_TPrintf("gdb_sample_DEBUG:  %s = %d\n",fields[2].name,field_get->value);

    }



    // �f�[�^�x�[�X�ɓo�^�����t�@�C��ID���폜����񓯊��������J�n
    // �i���R�[�h���폜����񓯊������j
    //  �f�[�^�x�[�X����t�@�C��ID�������Ȃ�ƁA���Y�t�@�C���͖�24���Ԃ�
    //  �t�@�C���T�[�o�[����폜����܂��B
    //-----------------------------------------------------------------------
    OS_Printf("*********  DEMO_fin  *********  delete file id from the database to finalize this demo..\n");

    if(record_id_file != 0)
    {
        res = DWC_GdbDeleteRecordAsync(TABLE_NAME, record_id_file);
    
        if (res != DWC_GDB_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbError[%d] in DWC_GdbDeleteRecordAsync().  %s line[%d]\n",res,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        if ((err = wait_async_end()) != DWC_ERROR_NONE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCError[%d]   %s line[%d]\n",err,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        if ((state = DWC_GdbGetState()) != DWC_GDB_STATE_IDLE)
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbState[%d] is improper state here.  %s line[%d]\n",state,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    
        result = DWC_GdbGetAsyncResult();
        if (result != DWC_GDB_ASYNC_RESULT_SUCCESS && result != DWC_GDB_ASYNC_RESULT_REQUEST_CANCELLED )
        {
            OS_TPrintf("gdb_sample_DEBUG: error!! DWCGdbAsyncResult[%d].  %s line[%d]\n",result,__FILE__,__LINE__);
            gdb_demo_exit();
            return;
        }
    }
    else
    {
        OS_TPrintf("gdb_sample_DEBUG: Target record is not found. skip.\n");
    }


//�f���I��
    gdb_demo_exit();
    OS_Terminate();

}
