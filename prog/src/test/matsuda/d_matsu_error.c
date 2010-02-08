//==============================================================================
/**
 * @file	d_matsu_error.c
 * @brief	�ʐM�G���[��ʃe�X�g
 * @author	matsuda
 * @date	2008.11.14(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include <backup_system.h>
//#include "savedata/contest_savedata.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"

#include "net\network_define.h"

//#include "test/goto/comm_error.h"


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct {

	u16		seq;
	HEAPID	heapID;
	int debug_mode;
	
	//�Z�[�u�֘A
	GFL_SAVEDATA *sv_normal;	///<�m�[�}���Z�[�u�f�[�^�ւ̃|�C���^
	
	//�ʐM
	BOOL connect_ok;
	int send_key;
	
	u8 huge_data[0x1000];
	u8 receive_huge_data[2][0x1000];
	volatile u16 cs;
}D_MATSU_WORK;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static BOOL DebugMatsuda_WiressTest(D_MATSU_WORK *wk);
static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
static void _initCallBack(void* pWork);
static void _connectCallBack(void* pWork);
static void _endCallBack(void* pWork);
static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvHugeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvKeyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);



//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	D_MATSU_WORK* wk;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MATSUDA_DEBUG, 0x70000 );
	wk = GFL_PROC_AllocWork( proc, sizeof(D_MATSU_WORK), HEAPID_MATSUDA_DEBUG );
	MI_CpuClear8(wk, sizeof(D_MATSU_WORK));
	wk->heapID = HEAPID_MATSUDA_DEBUG;
	{
		int i;
		volatile u16 cs = 0;
		u8 maca[6];
		OS_GetMacAddress(maca);
		for(i = 0; i < 6; i++){
			OS_TPrintf("mac addrs = %x %x %x %x %x %x\n", maca[0], maca[1], maca[2], maca[3], maca[4], maca[5]);
		}
		for(i = 0; i < 0x1000; i++){
			if(i < 0x5000){
				wk->huge_data[i] = maca[1];
			}
			else{
				wk->huge_data[i] = maca[2];
			}
			cs += wk->huge_data[i];
			if(i % 0x100 == 0 || i == 0x1000-1 || i == 0x1000-2){
				OS_TPrintf("cs = %x, huge %x %x\n", cs, i, wk->huge_data[i]);
			}
		}
		wk->cs = cs;
		OS_TPrintf("����f�[�^�`�F�b�N�T�� = 0x%x\n", wk->cs);
	}
	
//	GFL_ARC_Init( arcFiles, NELEMS(arcFiles) );	gfl_use.c��1�񂾂��������ɕύX

	GFL_DISP_SetBank( &vramBank );

	// �e����ʃ��W�X�^������
	G2_BlendNone();

	// BGsystem������
	GFL_BG_Init( wk->heapID );
	GFL_BMPWIN_Init( wk->heapID );

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// �ʃt���[���ݒ�
	{
		static const GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );

//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0x00, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// �㉺��ʐݒ�
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

	return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	D_MATSU_WORK* wk = mywk;
	BOOL ret = 0;
	
	wk->debug_mode = 0;
	switch(wk->debug_mode){
	case 0:		//���C�����X�e�X�g
		ret = DebugMatsuda_WiressTest(wk);
		break;
	case 1:		//�ԊO���e�X�g
		ret = DebugMatsuda_WiressTest(wk);
		break;
	default:
		ret = TRUE;
	}
	
	if(ret == TRUE){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_MATSUDA_DEBUG);
	
	return GFL_PROC_RES_FINISH;
}



//==============================================================================
//	
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
//    {_RecvMoveData,        GFL_NET_COMMAND_SIZE( 0 ), NULL},    ///NET_CMD_MOVE
//    {_RecvHugeData,        GFL_NET_COMMAND_SIZE( 0 ), NULL},    ///NET_CMD_HUGE
//    {_RecvKeyData,         GFL_NET_COMMAND_SIZE( 4 ), NULL},    ///NET_CMD_KEY
    {_RecvMoveData,         NULL},    ///NET_CMD_MOVE
    {_RecvHugeData,         _RecvHugeBuffer},    ///NET_CMD_HUGE
    {_RecvKeyData,          NULL},    ///NET_CMD_KEY
};

enum{
	NET_CMD_MOVE = GFL_NET_CMD_DEBUG_MATSUDA,
	NET_CMD_HUGE,
	NET_CMD_KEY,
};

#define _MAXNUM   2//(4)         // �ő�ڑ��l��
#define _MAXSIZE  (GFL_NET_IRC_SEND_MAX)	//(32)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��


static const GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_IRC_WIRELESS,//GFL_NET_TYPE_WIRELESS,//GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_DEBUG_MATSUDA_SERVICEID,  //GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};

//--------------------------------------------------------------
/**
 * @brief   ���C�����X�ʐM�e�X�g
 *
 * @param   wk		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_WiressTest(D_MATSU_WORK *wk)
{
	BOOL ret;
	
	GF_ASSERT(wk);

	if(aGFLNetInit.bNetType == GFL_NET_TYPE_IRC && wk->seq > 3){
		static u8 shut_print = 0;
//		if(IRC_IsConnect() == FALSE && shut_print == 0){
//			OS_TPrintf("�ԊO���ؒf\n");
//			shut_print++;
//		}
	}
	
	switch( wk->seq ){
	case 0:
		CommErrorSys_Setup();	//�Q�[�����C���̍ŏ��Ɉ�x�����Ă΂��
		CommErrorSys_Init(GFL_BG_FRAME0_M);	//�e�A�v����Init�ŌĂ΂��
		wk->seq++;
		break;
	case 1:
		if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
			CommErrorSys_Call();
		}
		break;
	
	case 100:
		{
			GFLNetInitializeStruct net_ini_data;
			
			net_ini_data = aGFLNetInit;
			GFL_NET_Init(&net_ini_data, _initCallBack, wk);	//�ʐM������
		}
		wk->seq++;
		break;
	case 101:
		if(GFL_NET_IsInit() == TRUE){	//�������I���҂�
			wk->seq++;
		}
		break;
	case 102:
		GFL_NET_ChangeoverConnect(_connectCallBack); // �����ڑ�
		wk->seq++;
		break;
	case 103:
		//�����ڑ��҂�
		if(wk->connect_ok == TRUE){
			OS_TPrintf("�ڑ�����\n");
			wk->seq++;
		}
		break;
	case 104:		//�^�C�~���O�R�}���h���s
//		wk->seq = 6;
//		break;
		GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15, WB_NET_DEBUG_MATSUDA_SERVICEID);
		wk->seq++;
		break;
	case 105:
		if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),15, WB_NET_DEBUG_MATSUDA_SERVICEID) == TRUE){
			OS_TPrintf("�^�C�~���O��萬��\n");
			OS_TPrintf("�ڑ��l�� = %d\n", GFL_NET_GetConnectNum());
			wk->seq++;
		}
		break;
	
	case 106:	//�L�[�𑗐M������
		ret = -1;
		wk->send_key = GFL_UI_KEY_GetTrg();
		if(wk->send_key & PAD_KEY_UP){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		if(wk->send_key & PAD_KEY_DOWN){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		if(wk->send_key & PAD_KEY_LEFT){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		if(wk->send_key & PAD_KEY_RIGHT){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		
		if(wk->send_key & PAD_BUTTON_B){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_HUGE, 0x1000, &wk->huge_data);
		}
		
		if(ret == FALSE){
			OS_TPrintf("���M���s\n");
		}
		
		if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){
			wk->seq++;
		}
		break;
	case 107:	//�ʐM�I��
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL)){
			//GFL_NET_Exit(_endCallBack);	//�ʐM�I��
			wk->seq++;
		}
		break;
	case 108:	//�ʐM�I���҂�
		if(wk->connect_ok == FALSE){
			wk->seq++;
		}
		break;
	default:
		return TRUE;
	}
	
	return FALSE;
}



typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_DEBUG_MATSUDA_SERVICEID };

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void* _netBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _initCallBack(void* pWork)
{
	OS_TPrintf("�����������R�[���o�b�N���Ăяo���ꂽ\n");
	return;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWork)
{
	D_MATSU_WORK *wk = pWork;
	
    OS_TPrintf("�l�S�V�G�[�V��������\n");
    wk->connect_ok = TRUE;
}

static void _endCallBack(void* pWork)
{
	D_MATSU_WORK *wk = pWork;

    NET_PRINT("endCallBack�I��\n");
    wk->connect_ok = FALSE;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ړ��R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
}

//--------------------------------------------------------------
/**
 * @brief   ����f�[�^�R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvHugeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	D_MATSU_WORK *wk = pWork;
	int i;
	const u8 *data = pData;
	u16 cs = 0;
	
	OS_TPrintf("����f�[�^��M netid = %d, size = 0x%x\n", netID, size);
	for(i = 0; i < size; i++){
		cs += data[i];
		//OS_TPrintf("%d, ", data[i]);
	//	if(i % 32 == 0){
	//		OS_TPrintf("\n");
	//	}
	}
	OS_TPrintf("����f�[�^��M�`�F�b�N�T��=0x%x\n", cs);
}

static u8 * _RecvHugeBuffer(int netID, void* pWork, int size)
{
	D_MATSU_WORK *wk = pWork;
	return wk->receive_huge_data[netID - 1];
}


//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvKeyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	int key_data;
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	
	key_data = *((int *)pData);
	OS_TPrintf("�f�[�^��M�FnetID=%d, size=%d, data=%d\n", netID, size, key_data);
	if(key_data & PAD_KEY_UP){
		OS_TPrintf("��M�F��\n");
	}
	if(key_data & PAD_KEY_DOWN){
		OS_TPrintf("��M�F��\n");
	}
	if(key_data & PAD_KEY_LEFT){
		OS_TPrintf("��M�F��\n");
	}
	if(key_data & PAD_KEY_RIGHT){
		OS_TPrintf("��M�F�E\n");
	}
}
















//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA DebugMatsudaErrorProcData = {
	DebugMatsudaMainProcInit,
	DebugMatsudaMainProcMain,
	DebugMatsudaMainProcEnd,
};


