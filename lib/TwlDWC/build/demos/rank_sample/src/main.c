#include <nitro.h>
#include <dwc.h>
#include  <cstring>


// �w�b�_�t�@�C���C���N���[�h
//-------------------------------------
#include "main.h"
#include "init.h"
#include "utility.h"

// �萔�錾
//---------------------------------------------------------

#define RANKING_INITDATA "hxBfMOkOvlOHpUGfQlOb0001ed350000685d0040000037012e6bdwctest"
#define CATEGORY		10

//#define USE_AUTHSERVER_RELEASE   // �{�ԃT�[�o�֐ڑ�


// �\���̌^�錾
//---------------------------------------------------------

// ���[�U�[��`�f�[�^
typedef struct _tagMYUSERDEFINEDDATA{
	u32		checksum;		// checksum
	u32		size;		// �T�C�Y
	u8		data[32];	// �f�[�^(32bytes)
}MYUSERDEFINEDDATA;


// �O���[�o���ϐ��錾
//---------------------------------------------------------
DWCInetControl stConnCtrl;


// �֐��v���g�^�C�v
//---------------------------------------------------------
static BOOL StartIPMain(void);
static void VBlankIntr(void);

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
 *	@brief	IP�̎擾
 *
 *	@retval	TRUE		����
 *	@retval	FALSE   	���s
 */
//=============================================================================
static BOOL StartIPMain(void)
{
	DWC_InitInet( &stConnCtrl );

#if defined( USE_AUTHSERVER_RELEASE )
    DWC_SetAuthServer( DWC_CONNECTINET_AUTH_RELEASE );
#else
    DWC_SetAuthServer( DWC_CONNECTINET_AUTH_TEST );
#endif

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

//=============================================================================
/*!
 *	@brief	���C��
 */
//=============================================================================
static u8 s_Work[ DWC_INIT_WORK_SIZE ] ATTRIBUTE_ALIGN( 32 );

void NitroMain()
{

	DWCUserData	    userdata;	// DWC�̃��[�U�[�f�[�^
	DWCRnkError		res;		// �����L���O���C�u�����̃G���[�擾�p
	DWCRnkGetParam	param;		// �����L���O�擾���̃p�����[�^

	MYUSERDEFINEDDATA	userdefineddata;	// ���[�U�[��`�f�[�^

	u32 order;
	u32 count;
	int i;
	int errorCode;

	// initialize
	//-------------------------------------------
	OS_Init();

	// �X�^�b�N���`�F�b�N�����ݒ�
	OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x100);
	
	Heap_Init();

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
	OS_WaitVBlankIntr();			  // Waiting the end of VBlank interrupt
	GX_DispOn();
	GXS_DispOn();

	// DWC���C�u����������
	DWC_Init( s_Work );
	
	// �f�o�b�O�\�����x���w��
	DWC_SetReportLevel(DWC_REPORTFLAG_ALL);
	
	DWC_SetMemFunc( AllocFunc, FreeFunc );

	//IP�̎擾
	while(!StartIPMain())
	{
		OS_Printf("error at StartIPMain()\n");	
	}

	//---------------------------------
	// ���O�C������
	//---------------------------------
	//
	// �����Ń��[�U�[�f�[�^�̎擾�������s���Ă��������B�����L���O���C�u������
	// GameSpy�Ɉˑ����Ă��邽�߁A���Ȃ��Ƃ���x�̓��O�C���������L����
	// GameSpyProfileID���擾���Ă��郆�[�U�[�f�[�^�łȂ���Ώ������Ɏ��s����
	// ���B��ID�͖����ł��B
	//
	// ���ɉߋ��Ƀ��O�C�����Ď擾�������[�U�[�f�[�^���o�b�N�A�b�v�ɕۑ���
	// ��Ă���ꍇ�͂�����g�p���邱�Ƃ��ł��܂��B�������A�K�C�h���C���ɒ��
	// ���Ă���Ƃ���A�C���^�[�l�b�g�ɐڑ�����ۂɂ͂��̓s�x�K�����O�C����
	// ��K�v�����邱�Ƃɒ��ӂ��Ă�������
	//
	// �O���F�؃T�[�o�[�Ȃǂ��g�p���Ă���ꍇ�͗L����GameSpyProfileID���擾��
	// �邱�Ƃ͂ł��܂���̂ŁA�����L���O���C�u�����͎g�p�ł��܂���B��������
	// ���̏ꍇ��GameSpy�̃T�[�o�[�������g�p�ł��܂���B
	//
	// <����>
	//
	// ���̃T���v���ł͊ȒP�̂��߂Ƀ��[�U�[�f�[�^�𖳗���萶�����Ă��܂����A
	// ���ۂɂ̓��O�C�����ē���ꂽ�����ȃ��[�U�[�f�[�^���g�p����悤�ɂ��Ă�
	// �������B����ȊO�̃f�[�^��n�����ꍇ�̓����L���O���W���[���̏�������
	// ��������\���͂���܂����A����͖���`�ł��B
	//
	{
		// ���[�U�[�f�[�^�̐ݒ�
		//
		//
		u32 dummyuserdata[DWC_ACC_USERDATA_BUFSIZE] = {
								0x00000040,0xa1b2c3d4,0x5e6f7a8b,0xc9d0e1f2,
								0x00000996,0x3a4b5c6d,0xe7f8a9b0,0x0001e240,
								0x1c2d3e4f,0xa5b6c7d8,0x9e0f1a2b,0xc3d4e5f6,
								0x7a8b9c0d,0xe1f2a3b4,0x5c6d7e8f,0xcfa4db0e };

		memcpy( &userdata, dummyuserdata, DWC_ACC_USERDATA_BUFSIZE );
	}

	//---------------------------------
	// �����L���O���W���[���̏���
	//---------------------------------

	// �����L���O���W���[���̏�����
	res = DWC_RnkInitialize(	RANKING_INITDATA,
								&userdata );

	switch( res ){
	case DWC_RNK_SUCCESS:
		OS_Printf("DWC_RnkInitialize success\n");
		break;
	case DWC_RNK_ERROR_INIT_ALREADYINITIALIZED:
		OS_Printf("[err]DWC_RnkInitialize already initialized\n");
		goto exit;
	case DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE:
		OS_Printf("[err]DWC_RnkInitialize invalid init data size\n");
		goto exit;
	case DWC_RNK_ERROR_INIT_INVALID_INITDATA:
		OS_Printf("[err]DWC_RnkInitialize invalid init data\n");
		goto exit;
	case DWC_RNK_ERROR_INIT_INVALID_USERDATA:
		OS_Printf("[err]DWC_RnkInitialize invaild user data\n");
		goto exit;
	}

	//
	// �A�b�v���[�h
	//

	// ���[�U�[��`�f�[�^�̐���
	//
	// ���[�U�[��`�f�[�^�ɂ͔C�ӂ̃o�C�i���f�[�^��764�o�C�g�w�肷�邱�Ƃ��ł�
	// �܂��B���̃f�[�^�̓����L���O���X�g�̎擾���Ɉꏏ�Ɏ擾���邱�Ƃ��ł���
	// ���B
	//
	// ���̃f�[�^�́A���ӂ̂��郆�[�U�[�ɂ��U���T�[�o�[�Ȃǂɂ��A�s����
	// ������������\��������܂��B�s���ȏ����������s��ꂽ�f�[�^�ɂ��A
	// �v���O�������\��������A���ӂ̂���R�[�h�����s����邱�Ƃ̂Ȃ��悤�ɁA
	// �N���C�A���g�ł͓��͂����f�[�^�͐M���������̂ł͂Ȃ��Ƃ����O��̏��
	// �R�[�f�B���O���s���Ă��������B
	//
	// �{�T���v���ł́A���[�U�[��`�f�[�^�ɃT�C�Y��Checksum���L�^���A�f�[�^��
    // ���������`�F�b�N���Ă��܂��B���ۂɂ́ANULL�I�[�ł���Ɖ��肵���f�[�^��
    // �T�[�`�ɂ��I�[�o�[�t���[���ɂ����ӂ��A���Ȃ��Ƃ����[�U�[��`�f�[�^��
    // �T�C�Y�ȏ�̃������̈�ɃA�N�Z�X���Ȃ��悤�ɃR�[�f�B���O���Ă��������B
	//
	userdefineddata.size = 32;
	strcpy((char *)userdefineddata.data, "test data");
	userdefineddata.checksum = (u32)MATH_CalcChecksum8(
										(const void*)userdefineddata.data,
										userdefineddata.size );

	// �X�R�A�A�b�v���[�h���N�G�X�g�J�n
	res = DWC_RnkPutScoreAsync(	CATEGORY,				// category
								DWC_RNK_REGION_JP,		// region
								1234,					// score
								(void*)&userdefineddata,// user defined data
								sizeof( userdefineddata ) );

	switch( res ){
	case DWC_RNK_SUCCESS:
		OS_Printf("DWC_RnkPutScoreAsync success\n");
		break;
	case DWC_RNK_IN_ERROR:
		OS_Printf("[err]DWC_RnkPutScoreAsync in error\n");
		goto exit;
	case DWC_RNK_ERROR_PUT_NOTREADY:
		OS_Printf("[err]DWC_RnkPutScoreAsync not ready\n");
		goto exit;
	case DWC_RNK_ERROR_INVALID_PARAMETER:
		OS_Printf("[err]DWC_RnkPutScoreAsync invalid parameter\n");
		goto exit;
	case DWC_RNK_ERROR_PUT_INVALID_KEY:
		OS_Printf("[err]DWC_RnkPutScoreAsync invalid key\n");
		goto exit;
	case DWC_RNK_ERROR_PUT_NOMEMORY:
		OS_Printf("[err]DWC_RnkPutScoreAsync nomemory\n");
		goto exit;
	}

	// �񓯊�����

	while( (res = DWC_RnkProcess()) == DWC_RNK_SUCCESS ){
	
		OS_Sleep(10);

	}

	switch( res ){
    case DWC_RNK_PROCESS_TIMEOUT:
        OS_Printf("DWC_RnkProcess timeout!!\n");
        goto exit;
	case DWC_RNK_IN_ERROR:
		DWC_GetLastError(&errorCode);
		OS_Printf("[err]DWC_RnkProcess in error [code:%d]\n", -errorCode);
		goto exit;
	case DWC_RNK_PROCESS_NOTASK:
		OS_Printf("DWC_RnkProcess done\n");
		break;
	}

	// �ʐM���ʎ擾
	switch( DWC_RnkGetState() ){
	case DWC_RNK_STATE_COMPLETED:
		OS_Printf("DWC_RnkGetState completed\n");
		break;
	case DWC_RNK_STATE_ERROR:
        DWC_GetLastError(&errorCode);
        OS_Printf("[err]DWC_RnkGetState error [code:%d]\n", -errorCode);
		goto exit;
	}



	//
	// �����̏��ʂ��擾
	//

	// ���ʎ擾���N�G�X�g�J�n

	param.size = sizeof( param.order );
	param.order.since = 0;
	param.order.sort = DWC_RNK_ORDER_DES;

	res = DWC_RnkGetScoreAsync(	DWC_RNK_GET_MODE_ORDER,		// mode
								CATEGORY,					// category
								DWC_RNK_REGION_JP,			// region
								&param );					// parameter

	switch( res ){
	case DWC_RNK_SUCCESS:
		OS_Printf("DWC_RnkGetScoreAsync success\n");
		break;
	case DWC_RNK_IN_ERROR:
		OS_Printf("[err]DWC_RnkGetScoreAsync in error\n");
		goto exit;
	case DWC_RNK_ERROR_GET_NOTREADY:
		OS_Printf("[err]DWC_RnkGetScoreAsync not ready\n");
		goto exit;
	case DWC_RNK_ERROR_INVALID_PARAMETER:
		OS_Printf("[err]DWC_RnkGetScoreAsync invalid parameter\n");
		goto exit;
	case DWC_RNK_ERROR_GET_INVALID_KEY:
		OS_Printf("[err]DWC_RnkGetScoreAsync invalid key\n");
		goto exit;
	case DWC_RNK_ERROR_GET_NOMEMORY:
		OS_Printf("[err]DWC_RnkGetScoreAsync nomemory\n");
		goto exit;
	}

	// �񓯊�����

	while( (res = DWC_RnkProcess()) == DWC_RNK_SUCCESS ){
	
		OS_Sleep(10);

	}

	switch( res ){
    case DWC_RNK_PROCESS_TIMEOUT:
        OS_Printf("DWC_RnkProcess timeout!!\n");
        goto exit;
	case DWC_RNK_IN_ERROR:
		DWC_GetLastError(&errorCode);
		OS_Printf("[err]DWC_RnkProcess in error [code:%d]\n", -errorCode);
		goto exit;
	case DWC_RNK_PROCESS_NOTASK:
		OS_Printf("DWC_RnkProcess done\n");
		break;
	}

	// �ʐM���ʎ擾
	switch( DWC_RnkGetState() ){
	case DWC_RNK_STATE_COMPLETED:
		OS_Printf("DWC_RnkGetState completed\n");
		break;
	case DWC_RNK_STATE_ERROR:
        DWC_GetLastError(&errorCode);
        OS_Printf("[err]DWC_RnkGetState error [code:%d]\n", -errorCode);
		goto exit;
	}

	// �\��
	res = DWC_RnkResGetOrder( &order );
	switch( res ){
	case DWC_RNK_SUCCESS:
		break;
	case DWC_RNK_IN_ERROR:
		OS_Printf("[err]DWC_RnkResGetOrder in error [code:%d]\n", DWC_GetLastError( NULL ) );
		goto exit;
	case DWC_RNK_ERROR_INVALID_MODE:
		OS_Printf("[err]DWC_RnkResGetOrder invalid mode\n");
		goto exit;
	case DWC_RNK_ERROR_INVALID_PARAMETER:
		OS_Printf("[err]DWC_RnkResGetOrder invalid parameter\n");
		goto exit;
	case DWC_RNK_ERROR_NOTCOMPLETED:
		OS_Printf("[err]DWC_RnkResGetOrder not completed\n");
		goto exit;
	case DWC_RNK_ERROR_EMPTY_RESPONSE:
		OS_Printf("[err]DWC_RnkResGetOrder empty response\n");
		goto exit;
	}

	OS_Printf("order=%d\n", order);


	//
	// �����L���O���擾
	//

	// �g�b�v�����L���O�擾���N�G�X�g�J�n
	param.size = sizeof( param.toplist );
	param.toplist.sort = DWC_RNK_ORDER_DES;
	param.toplist.limit = 10;
	param.toplist.since = 0;

	res = DWC_RnkGetScoreAsync(	DWC_RNK_GET_MODE_TOPLIST,	// mode
									CATEGORY,				// category
									DWC_RNK_REGION_JP,		// region
									&param );				// parameter

	switch( res ){
	case DWC_RNK_SUCCESS:
		OS_Printf("DWC_RnkGetScoreAsync success\n");
		break;
	case DWC_RNK_IN_ERROR:
		OS_Printf("[err]DWC_RnkGetScoreAsync in error\n");
		goto exit;
	case DWC_RNK_ERROR_GET_NOTREADY:
		OS_Printf("[err]DWC_RnkGetScoreAsync not ready\n");
		goto exit;
	case DWC_RNK_ERROR_INVALID_PARAMETER:
		OS_Printf("[err]DWC_RnkGetScoreAsync invalid parameter\n");
		goto exit;
	case DWC_RNK_ERROR_GET_INVALID_KEY:
		OS_Printf("[err]DWC_RnkGetScoreAsync invalid key\n");
		goto exit;
	case DWC_RNK_ERROR_GET_NOMEMORY:
		OS_Printf("[err]DWC_RnkGetScoreAsync nomemory\n");
		goto exit;
	}

	// �񓯊�����

	while( (res = DWC_RnkProcess()) == DWC_RNK_SUCCESS ){
	
		OS_Sleep(10);

	}

	switch( res ){
    case DWC_RNK_PROCESS_TIMEOUT:
        OS_Printf("DWC_RnkProcess timeout!!\n");
        goto exit;
	case DWC_RNK_IN_ERROR:
		DWC_GetLastError(&errorCode);
		OS_Printf("[err]DWC_RnkProcess in error [code:%d]\n", -errorCode);
		goto exit;
	case DWC_RNK_PROCESS_NOTASK:
		OS_Printf("DWC_RnkProcess done\n");
		break;
	}

	// �ʐM���ʎ擾
	switch( DWC_RnkGetState() ){
	case DWC_RNK_STATE_COMPLETED:
		OS_Printf("DWC_RnkGetState completed\n");
		break;
	case DWC_RNK_STATE_ERROR:
        DWC_GetLastError(&errorCode);
        OS_Printf("[err]DWC_RnkGetState error [code:%d]\n", -errorCode);
		goto exit;
	}


	// �ꗗ���擾����

	res = DWC_RnkResGetRowCount( &count );

	switch( res ){
	case DWC_RNK_SUCCESS:
		break;
	case DWC_RNK_IN_ERROR:
		OS_Printf("[err]DWC_RnkResGetRowCount in error [code:%d]\n", DWC_GetLastError( NULL ) );
		goto exit;
	case DWC_RNK_ERROR_INVALID_MODE:
		OS_Printf("[err]DWC_RnkResGetRowCount invalid mode\n");
		goto exit;
	case DWC_RNK_ERROR_INVALID_PARAMETER:
		OS_Printf("[err]DWC_RnkResGetRowCount invalid parameter\n");
		goto exit;
	case DWC_RNK_ERROR_NOTCOMPLETED:
		OS_Printf("[err]DWC_RnkResGetRowCount not completed\n");
		goto exit;
	case DWC_RNK_ERROR_EMPTY_RESPONSE:
		OS_Printf("[err]DWC_RnkResGetRowCount empty response\n");
		goto exit;
	}
	
	for( i=0; i<count; i++ ){

		DWCRnkData data;

		if( DWC_RnkResGetRow( &data, (u32)i ) != DWC_RNK_SUCCESS ){
			OS_Printf("[err]DWC_RnkResGetRow()\n");
			break;
		}

		// �o��
		OS_TPrintf("%dth score=%d pid=%d region=%d update=%dmin before ",
						data.order,
						data.score,
						data.pid,
						data.region,
						data.lastupdate );

		// ���[�U�[��`�f�[�^�̐������`�F�b�N
		{
			MYUSERDEFINEDDATA*	mydata;
			
			mydata = (MYUSERDEFINEDDATA*)data.userdata;

			if(mydata->size != 32){

				// �f�[�^�T�C�Y���s��
				OS_TPrintf("userdata=[invalid data size]\n");

			}else if(mydata->checksum != (u32)MATH_CalcChecksum8(
										(const void*)mydata->data,
										mydata->size )){

				// Checksum���s��
				OS_TPrintf("userdata=[invalid checksum]\n");
			
			}else{

				// �������f�[�^
				OS_TPrintf("userdata=%s\n", mydata->data);

			}

		}

	}

exit:
	// �����L���O���W���[���̏I��
	DWC_RnkShutdown();
	
	// �I��
	OS_Printf("Terminated.\n");	

	OS_Terminate();

}
