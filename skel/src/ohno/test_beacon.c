//============================================================================================
/**
 * @file	test_net.c
 * @brief	�ʐM����e�X�g�p�֐�
 * @author	ohno
 * @date	2006.12.19
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "gf_standard.h"
#include "test_beacon.h"
#include "main.h"
#include "fatal_error.h"
#include "net_icondata.h"
#include "textprint.h"


#define _BCON_GET_NUM  (6)

//------------------------------------------------------------------
// NET�̃e�X�g
//------------------------------------------------------------------




// ���[�J���ʐM�R�}���h�̒�`
enum _testCommand_e {
    _TEST_VARIABLE = GFL_NET_CMD_COMMAND_MAX,
    _TEST_GETSIZE,
    _TEST_HUGE,
    _TEST_VARIABLE_HUGE,
};

typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { 5 };

//-------------------------------------------------�f�o�b�O�\��

typedef struct {
	const char*	msg;
	u8 posx;
	u8 posy;
	u8 sizx;
	u8 sizy;
}TESTMODE_PRINTLIST;

static const char	test_index1[] = {"beacon catch"};
static const char	test_index2[] = {"scan mode"};
static const char	test_index3[] = {"point mode"};


enum {
	NUM_TITLE = 0,
	NUM_URL,
};

static const TESTMODE_PRINTLIST _childIndexList[] = {
	{ test_index1, 2, 1, 30, 2 },
	{ test_index2, 2, 22, 30, 1 },
};

static const TESTMODE_PRINTLIST _parentIndexList[] = {
	{ test_index1, 2, 1, 30, 2 },
	{ test_index3, 2, 22, 30, 1 },
};

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static const char font_path[] = {"src/gfl_graphic/gfl_font.dat"};

#define G2D_BACKGROUND_COL	(0x0000)
#define G2D_FONT_COL		(0x7fff)
#define G2D_FONTSELECT_COL	(0x001f)

static const GFL_TEXT_PRINTPARAM default_param = 
{ NULL, 0, 0, 1, 1, 1, 0, GFL_TEXT_WRITE_16 };

#define TEXT_FRM			(GFL_BG_FRAME3_M)
#define TEXT_FRM_PRI		(0)

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ�
 */
//------------------------------------------------------------------
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static const GFL_BG_BGCNT_HEADER bgCont3 = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

static const GFL_BG_BGCNT_HEADER bgCont0 = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};


#define _BACKUP_NUM  (5)   //RSSI�̕��ς��o�����߂Ƀo�b�N�A�b�v
//------------------------------------------------------------------
/**
 * @brief		�������[�N�\����
 */
//------------------------------------------------------------------
/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*_PTRFunc)(SKEL_TEST_BEACON_WORK* pNetHandle);

struct _SKEL_TEST_BEACON_WORK{
    HEAPID heapID;
    BOOL bParent;   ///< �e�@���[�h���ǂ���
    GFL_TEXT_PRINTPARAM* textParam;
    GFL_BMPWIN*	bmpwin[5];
    GFL_BMPWIN* pBmpwinBeacon[_BCON_GET_NUM];
    u16 rssiBackup[_BCON_GET_NUM][_BACKUP_NUM];
    u16 rssiDir[8];
    int dir;
    int testNo;
    _PTRFunc state;
    int timer;
    GFL_NETHANDLE* _pHandle;
    GFL_NETHANDLE* _pHandleServer;
};

#if defined(DEBUG_ONLY_FOR_ohno)
#define   _CHANGE_STATE(state, time)  _changeStateDebug(pWork ,state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(pWork ,state, time)
#endif

//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================

static void _changeState(SKEL_TEST_BEACON_WORK* pWork,_PTRFunc state, int time)
{
    pWork->state = state;
    pWork->timer = time;
}

//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(SKEL_TEST_BEACON_WORK* pWork,_PTRFunc state, int time, int line)
{
    NET_PRINT("net_state: %d\n",line);
    _changeState(pWork, state, time);
}
#endif

//==============================================================================
/**
 * @brief   �Ȃɂ����Ȃ�
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================
static void _noneFunc(SKEL_TEST_BEACON_WORK* pWork)
{
}

//------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�r�b�g�}�b�v�E�C���h�E�R���g���[��
 */
//------------------------------------------------------------------
static void msg_bmpwin_make
			(SKEL_TEST_BEACON_WORK * testmode, u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy )
{
	//�r�b�g�}�b�v�쐬
	testmode->bmpwin[bmpwinNum] = GFL_BMPWIN_Create( TEXT_FRM, px, py, sx, sy, 0, 
														GFL_BG_CHRAREA_GET_B );

	//�e�L�X�g���r�b�g�}�b�v�ɕ\��
	testmode->textParam->bmp = GFL_BMPWIN_GetBmp( testmode->bmpwin[ bmpwinNum ] );
	GFL_TEXT_PrintSjisCode( msg, testmode->textParam );

	//�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
	GFL_BMPWIN_TransVramCharacter( testmode->bmpwin[bmpwinNum] );
	//�r�b�g�}�b�v�X�N���[���쐬
	//GFL_BMPWIN_MakeScreen( testmode->bmpwin[bmpwinNum] );
}

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static void	g2d_load( SKEL_TEST_BEACON_WORK* testmode )
{
	//�t�H���g�ǂݍ���
	GFL_TEXT_CreateSystem( font_path );
	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( testmode->heapID, 16*2 );
		plt[0] = G2D_BACKGROUND_COL;
		plt[1] = G2D_FONT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 0 );
		plt[1] = G2D_FONTSELECT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 16*2 );

		GFL_HEAP_FreeMemory( plt );
	}
	//�����\���p�����[�^���[�N�쐬
	{
		GFL_TEXT_PRINTPARAM* param = GFL_HEAP_AllocClearMemoryLo
										( testmode->heapID,sizeof(GFL_TEXT_PRINTPARAM));
		*param = default_param;
		testmode->textParam = param;
	}
	//�����\���r�b�g�}�b�v�̍쐬
	{
		int i,num;
        const TESTMODE_PRINTLIST* pList;

        if(testmode->bParent){
            pList = &_parentIndexList[0];
            num = NELEMS(_parentIndexList);
        }
        else{
            pList = &_childIndexList[0];
            num = NELEMS(_childIndexList);
        }
        
		//�\��r�b�g�}�b�v�̍쐬
		for(i = 0 ; i < num ; i++,pList++){
			msg_bmpwin_make( testmode, i, pList->msg,
							pList->posx, pList->posy, 
							pList->sizx, pList->sizy );
		}
	}
	//�a�f�X�N���[���]�����N�G�X�g���s
	//GFL_BG_LoadScreenReq( TEXT_FRM );
}

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void	bg_init( HEAPID heapID )
{
	//�a�f�V�X�e���N��
	GFL_BG_Init( heapID );

	//�u�q�`�l�ݒ�
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForBG(GX_VRAM_BG_64_E);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//�a�f���[�h�ݒ�
	GFL_BG_InitBG( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl( TEXT_FRM, &bgCont3, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( TEXT_FRM, TEXT_FRM_PRI );
	GFL_BG_SetVisible( TEXT_FRM, VISIBLE_ON );

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_Init( heapID );

	//ARC�V�X�e��������
//	GFL_ARC_Init(&GraphicFileTable[0],1);

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
}


static void* _netBeaconGetFunc(void)    ///< �r�[�R���f�[�^�擾�֐�
{
    return &_testBeacon;
}

static int _netBeaconGetSizeFunc(void)    ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
{
    return sizeof(_testBeacon);
}

static BOOL _netBeaconCompFunc(int myNo,int beaconNo)    ///< �r�[�R���f�[�^�擾�֐�
{
    return TRUE;
}


//----------------------------------------------------------------


static u16 _backupRssi(SKEL_TEST_BEACON_WORK* pWork,int index, u16 rssi)
{
    u16 sum = 0;
    int i;

    if((pWork->rssiBackup[index][_BACKUP_NUM-1]==0) ||(pWork->rssiBackup[index][0] != rssi)){
        for(i = _BACKUP_NUM-1; (i-1) >= 0; i--){
            pWork->rssiBackup[index][i] = pWork->rssiBackup[index][i-1];
        }
        pWork->rssiBackup[index][0] = rssi;
    }
    for(i = 0; i < _BACKUP_NUM ; i++){
        sum  += pWork->rssiBackup[index][i];
    }
    if(sum!=0){
        sum /= _BACKUP_NUM;
    }
    return sum;
}


static void _stockRssi(SKEL_TEST_BEACON_WORK* pWork, u16 rssi)
{

    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){
        GFL_STD_MemClear(pWork->rssiDir,sizeof(u16)*8);
        pWork->dir=0;
    }
    if(PAD_BUTTON_Y == GFL_UI_KEY_GetTrg()){


    }
    if(PAD_BUTTON_B == GFL_UI_KEY_GetTrg()){
        pWork->rssiDir[pWork->dir] = rssi;
        pWork->dir++;
        if(pWork->dir >= 8){
            pWork->dir=0;
        }
    }

}


//----------------------------------------------------------------


static void _dispBeacon(SKEL_TEST_BEACON_WORK* pWork)
{
    int i;
    
    for(i = 0;i < _BCON_GET_NUM; i++){
        u8* pData = GFL_NET_GetBeaconMacAddress(i);
        
        if(pData){
//            OS_TPrintf("%d: mac %x\n",i,);
            char msg[100];
            u16 rssi = GFL_NET_WL_GetRssi(i);

            rssi = _backupRssi(pWork,i,rssi);

            _stockRssi(pWork,rssi);
            
            GFL_STD_MemClear(msg,100);
            OS_SPrintf(msg,"%d: %3d %3d %3d %3d %3d %3d %3d %3d",rssi,
                       pWork->rssiDir[0],
                       pWork->rssiDir[1],
                       pWork->rssiDir[2],
                       pWork->rssiDir[3],
                       pWork->rssiDir[4],
                       pWork->rssiDir[5],
                       pWork->rssiDir[6],
                       pWork->rssiDir[7]
                       );

            GFL_BMPWIN_ClearScreen(pWork->pBmpwinBeacon[ i ]);
            
            pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->pBmpwinBeacon[ i ] );
            GFL_BMP_Clear(pWork->textParam->bmp,0);
            GFL_TEXT_PrintSjisCode( msg, pWork->textParam );

            //�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
            GFL_BMPWIN_TransVramCharacter( pWork->pBmpwinBeacon[i] );
        }
        else{
            pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->pBmpwinBeacon[ i ] );
            GFL_BMP_Clear(pWork->textParam->bmp,0);
            GFL_BMPWIN_TransVramCharacter( pWork->pBmpwinBeacon[i] );
        }
    }
}


static void rssiWindowInit(SKEL_TEST_BEACON_WORK* pWork)
{
    int i;
    
    for(i = 0;i < _BCON_GET_NUM; i++){
        pWork->pBmpwinBeacon[i] = GFL_BMPWIN_Create( TEXT_FRM,
                                                       4, 5+i*2, 24, 1, 0, 
                                                       GFL_BG_CHRAREA_GET_B );
    }
}


static void _testChild(SKEL_TEST_BEACON_WORK* pWork)
{
    u8 beacon;
    int i;

    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){
        for(i = 0;i < _BCON_GET_NUM; i++){
            u8* pData = GFL_NET_GetBeaconMacAddress(i);
            if(pData){
                OS_TPrintf("%d: mac %x%x%x%x%x%x\n",i,pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
            }
        }
    }

    bg_init( pWork->heapID );
    //��ʍ쐬
    g2d_load(pWork);	//�Q�c�f�[�^�쐬


    rssiWindowInit(pWork);
        
    pWork->_pHandle = GFL_NET_CreateHandle();
    GFL_NET_StartBeaconScan(pWork->_pHandle );    // �r�[�R����҂�
    _CHANGE_STATE(_dispBeacon, 0);
}

//            u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�


static void _testParent(SKEL_TEST_BEACON_WORK* pWork)
{

    bg_init( pWork->heapID );    //��ʍ쐬
    g2d_load(pWork);	//�Q�c�f�[�^�쐬
    // �T�[�o�[���̃e�X�g  �r�[�R���o���Ă��邾��
    pWork->_pHandleServer = GFL_NET_CreateHandle();
    GFL_NET_InitServer(pWork->_pHandleServer);   // �T�[�o
    pWork->_pHandle = GFL_NET_CreateHandle();  // �N���C�A���g
    _CHANGE_STATE(_noneFunc, 0);

}

void TEST_BEACON_Main(SKEL_TEST_BEACON_WORK* pWork)
{

    if(pWork){
        _PTRFunc state = pWork->state;
        if(state != NULL){
            state(pWork);
        }
    }
}



#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}


// �σT�C�Y��M
static void _testRecvVariable(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvVariable %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}


static void _testRecvGetSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvGetSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}

static int _getTestCommandSize(void)
{
    return 12;
}


// ���[�J���ʐM�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
    // �σT�C�Y�e�X�g
    {_testRecvVariable,          GFL_NET_COMMAND_VARIABLE, NULL},
    // �T�C�Y�擾�֐��e�X�g
    {_testRecvGetSize,           _getTestCommandSize, NULL},
};

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (120)  // ���M�ő�T�C�Y

// �ʐM�������\����  wifi�p
static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,   // ���[�N�|�C���^
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NET_ICONDATA_GetTableData,   // �ʐM�A�C�R���̃t�@�C��ARC�e�[�u����Ԃ��֐�
    NET_ICONDATA_GetNoBuff,      // �ʐM�A�C�R���̃t�@�C��ARC�̔ԍ���Ԃ��֐�
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    FALSE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �ʐM���J�n���邩�ǂ���
};

//----------------------------------------------------------------------------
/**
 *	@brief	����������
 */
//-----------------------------------------------------------------------------

SKEL_TEST_BEACON_WORK* TEST_BEACON_Init(HEAPID heapID)
{

    SKEL_TEST_BEACON_WORK* pWork = GFL_HEAP_AllocMemory(heapID, sizeof(SKEL_TEST_BEACON_WORK));
    GFL_STD_MemClear(pWork, sizeof(SKEL_TEST_BEACON_WORK));
    pWork->heapID = heapID;

    //�C�N�j���[�������g�p����O�� VRAMD��disable�ɂ���K�v������̂���
    //VRAMD�����Ɏg���Ă����̂����킩��Ȃ��ƁA�������Ƃ��ł��Ȃ�
    
    if(GX_VRAM_LCDC_D == GX_GetBankForLCDC()){
        GX_DisableBankForLCDC(); // LCDC��VRAMD�����蓖�Ă��Ă�悤�Ȃ̂őł�����
    }
    else if(GX_VRAM_D & GX_GetBankForBG()){
        GX_DisableBankForBG();
    }
    else if(GX_VRAM_D & GX_GetBankForTex()){
        GX_DisableBankForTex();
    }
    else if(GX_VRAM_D & GX_GetBankForClearImage()){
        GX_DisableBankForClearImage();
    }
    else if(GX_VRAM_D & GX_GetBankForSubOBJ()){
        GX_DisableBankForSubOBJ();
    }

    GX_DisableBankForLCDC(); // LCDC��VRAMD�����蓖�Ă��Ă�悤�Ȃ̂őł�����
    GX_DisableBankForBG();
    GX_DisableBankForTex();
    GX_DisableBankForClearImage();
    GX_DisableBankForSubOBJ();

    GFL_NET_Init(&aGFLNetInit);

    if(PAD_BUTTON_R & GFL_UI_KEY_GetCont()){
        OS_TPrintf("�e�ɂȂ���\n");
        _CHANGE_STATE(_testParent,0);
    }
    else{
        OS_TPrintf("�q�ɂȂ���\n");
        _CHANGE_STATE(_testChild,0);
    }

    return pWork;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I������
 */
//-----------------------------------------------------------------------------

void TEST_BEACON_End(SKEL_TEST_BEACON_WORK* pWork)
{
    GFL_HEAP_FreeMemory(pWork);
}
