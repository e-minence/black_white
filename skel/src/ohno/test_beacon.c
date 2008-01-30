//============================================================================================
/**
 * @file	test_beacon.c
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
#include "sample_graphic/radar.naix"


#define _BCON_GET_NUM  (6)
#define _BACKUP_NUM  (5)    // RSSI�̕��ς��o�����߂Ƀo�b�N�A�b�v
#define _DIR_SAMPL_NUM (8)  // �����̎擾�T���v��
#define _GAME_BMPWIN (5)
#define _SAMP_TIMER  (60*3)  // �T�b�T���v�����O

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
static const char	test_index4[] = {"xxxxxxxx"};
static const char	test_index5[] = {"start"};
static const char	test_index6[] = {"end"};


enum {
	NUM_TITLE = 0,
	NUM_URL,
};

static const TESTMODE_PRINTLIST _childIndexList[] = {
	{ test_index1, 2, 1, 30, 1 },
	{ test_index2, 2, 2, 30, 1 },
};

static const TESTMODE_PRINTLIST _parentIndexList[] = {
	{ test_index1, 2, 1, 30, 1 },
	{ test_index3, 2, 2, 30, 1 },
};

static const TESTMODE_PRINTLIST _gameIndexList[] = {
	{ test_index5, 2, 3, 30, 1 },
	{ test_index4, 2, 4, 30, 1 },
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

#define TEXT_FRM			(GFL_BG_FRAME3_S)
#define TEXT_FRM_PRI		(0)

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ�
 */
//------------------------------------------------------------------
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
};	


//------------------------------------------------------------------
/**
 * @brief		�������[�N�\����
 */
//------------------------------------------------------------------

typedef struct {
	GFL_CLWK		*clwk;
	GFL_CLACTPOS	now_pos;
	GFL_CLACTPOS	offset_pos;
	u8			clact_no;		//�Z���A�N�^�[�i���o�[
} _ACCESS_POINT_PARAM;

//------------------------------------------------------------------
/**
 * @brief		�������[�N�\����
 */
//------------------------------------------------------------------

typedef struct {
	void* p_cellbuff;
	void* p_cellanmbuff;
	NNSG2dCellDataBank*		p_cell;
	NNSG2dCellAnimBankData*	p_cellanm;
	NNSG2dImageProxy		imageproxy;
	NNSG2dImagePaletteProxy	plttproxy;
} _CLACT_RES;


/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*_PTRFunc)(SKEL_TEST_BEACON_WORK* pNetHandle);

struct _SKEL_TEST_BEACON_WORK{
    HEAPID heapID;
    BOOL bParent;   ///< �e�@���[�h���ǂ���
    GFL_TEXT_PRINTPARAM* textParam;
    GFL_BMPWIN*	bmpwin[7];
    GFL_BMPWIN* pBmpwinBeacon[_BCON_GET_NUM];
    u16 rssiBackup[_BCON_GET_NUM][_BACKUP_NUM];
    u16 rssiDir[_BCON_GET_NUM][_DIR_SAMPL_NUM];
    _ACCESS_POINT_PARAM pAP[_BCON_GET_NUM];
    int dir;
    int countDir;
    int testNo;
    _PTRFunc state;
    int timer;
    GFL_NETHANDLE* _pHandle;
    GFL_NETHANDLE* _pHandleServer;
	GFL_CLUNIT		*p_unit;
	GFL_CLWK		*clact_work[_BCON_GET_NUM];
    ACCESS_POINT_PARAM* Ap[_BCON_GET_NUM];
	_CLACT_RES res;
    
    GFL_AREAMAN			*clact_area;
};

#if defined(DEBUG_ONLY_FOR_ohno)
#define   _CHANGE_STATE(state, time)  _changeStateDebug(pWork ,state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(pWork ,state, time)
#endif

static _ACCESS_POINT_PARAM	*_InitAPChr(SKEL_TEST_BEACON_WORK *pWork, u16 xpos, u16 ypos, u16 rssi);
static void _DeleteFallChr(SKEL_TEST_BEACON_WORK* pWork,int clact_no);


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
	GFL_BMPWIN_MakeScreen( testmode->bmpwin[bmpwinNum] );
}

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�\��
 */
//------------------------------------------------------------------

static void clact_draw(SKEL_TEST_BEACON_WORK* testmode )
{
    if(testmode->p_unit){
        // �Z���A�N�^�[���j�b�g�`�揈��
        GFL_CLACT_UNIT_Draw( testmode->p_unit );
        // �Z���A�N�^�[�V�X�e�����C������
        // �S���j�b�g�`�悪�������Ă���s���K�v������܂��B
        GFL_CLACT_Main();
    }
}

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�\��
 */
//------------------------------------------------------------------

static void text_draw(SKEL_TEST_BEACON_WORK* testmode )
{
    int i;
    
	GFL_BG_ClearScreen( TEXT_FRM );

    //�I�����ڃr�b�g�}�b�v�̕\��
    for(i=0;i<NELEMS(_parentIndexList);i++){
        GFL_BMPWIN_MakeScreen( testmode->bmpwin[i] );
    }
	GFL_BG_LoadScreenReq( TEXT_FRM );
}

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static void	g2d_load( SKEL_TEST_BEACON_WORK* pWork )
{
	//�t�H���g�ǂݍ���
	GFL_TEXT_CreateSystem( font_path );
	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( pWork->heapID, 16*2 );
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
										( pWork->heapID,sizeof(GFL_TEXT_PRINTPARAM));
		*param = default_param;
		pWork->textParam = param;
	}
	//�����\���r�b�g�}�b�v�̍쐬
	{
		int i,num;
        const TESTMODE_PRINTLIST* pList;

        if(pWork->bParent){
            pList = &_parentIndexList[0];
            num = NELEMS(_parentIndexList);
        }
        else{
            pList = &_childIndexList[0];
            num = NELEMS(_childIndexList);
        }
        
		//�\��r�b�g�}�b�v�̍쐬
		for(i = 0 ; i < num ; i++,pList++){
			msg_bmpwin_make( pWork, i, pList->msg,
							pList->posx, pList->posy, 
							pList->sizx, pList->sizy );
		}

        for(i=0; i < NELEMS(_gameIndexList);i++, pList++){
            pList = &_gameIndexList[0];
            msg_bmpwin_make( pWork, _GAME_BMPWIN+i, pList->msg,
                             pList->posx, pList->posy, 
                             pList->sizx, pList->sizy );
        }
    }
}

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void	bg_init( HEAPID heapID )
{
	//�a�f�R���g���[���ݒ�
	GFL_BG_SetPriority( TEXT_FRM, TEXT_FRM_PRI );
	GFL_BG_SetVisible( TEXT_FRM, VISIBLE_ON );
}

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
    text_draw(pWork);
    clact_draw(pWork);
}


static void* _netBeaconGetFunc(void)    ///< �r�[�R���f�[�^�擾�֐�
{
    return &_testBeacon;
}

static int _netBeaconGetSizeFunc(void)    ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
{
    return sizeof(_testBeacon);
}

static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)    ///< �r�[�R���f�[�^�擾�֐�
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

//==============================================================================
/**
 * @brief   �L�[���Ď����ăQ�[���X�^�[�g������
 * @param   pWork  �Q�[���̃��[�N
 * @retval  none
 */
//==============================================================================

static void _gameStartCheck(SKEL_TEST_BEACON_WORK* pWork)
{
    int i;

    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){  // �o�b�t�@������
        GFL_STD_MemClear(pWork->rssiDir,sizeof(u16)*_DIR_SAMPL_NUM*_BCON_GET_NUM);
        pWork->dir = 0;

        for(i = 0;i < _BCON_GET_NUM;i++){
            _DeleteFallChr(pWork,i);
        }
    }


    if(PAD_BUTTON_B == GFL_UI_KEY_GetTrg()){
        if((pWork->dir == 0) && (pWork->countDir==0)){
            pWork->countDir = _SAMP_TIMER;

        }
    }
}

//==============================================================================
/**
 * @brief   �Q�[���̎��s�A���Ԋu�ŃT���v�����O������i�߂�
 * @param   pWork  �Q�[���̃��[�N
 * @retval  none
 */
//==============================================================================

static BOOL _gameStartFunc(SKEL_TEST_BEACON_WORK* pWork)
{
    int i,msgindex=0;
    char msg[100];

//    pWork->bmpwin[_GAME_BMPWIN+i];

    GFL_STD_MemClear(msg,100);

    for(i = 0;i < _DIR_SAMPL_NUM; i++){
        if(pWork->dir > i){
            msg[i]='o';
        }
        else{
            msg[i]='x';
        }
    }
    
    GFL_BMPWIN_ClearScreen(pWork->bmpwin[ _GAME_BMPWIN+1 ]);
    pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->bmpwin[ _GAME_BMPWIN+1  ] );
    GFL_BMP_Clear(pWork->textParam->bmp,0);
    GFL_TEXT_PrintSjisCode( msg, pWork->textParam );
    GFL_BMPWIN_TransVramCharacter( pWork->bmpwin[_GAME_BMPWIN+1 ] );
    GFL_BMPWIN_MakeScreen( pWork->bmpwin[_GAME_BMPWIN+1 ] );

    if(pWork->countDir){
        pWork->countDir--;
        if(pWork->countDir==0){
            pWork->countDir = _SAMP_TIMER;
            pWork->dir++;
        }
        if(pWork->dir == _DIR_SAMPL_NUM){ // ���W����
            pWork->countDir = 0;
            pWork->dir=0;
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �A�N�Z�X�|�C���g���ʕ\��
 * @param   pWork  �Q�[���̃��[�N
 * @retval  none
 */
//==============================================================================

static void _APDisp(SKEL_TEST_BEACON_WORK* pWork,int index)
{
    u16 max=0,dir=0,j;
    fx16 posx,posy;
    u16 x,y;
    s16 sx,sy;

    for(j =0; j < _DIR_SAMPL_NUM;j++){
        if(pWork->rssiDir[index][j] > max){
            dir = j;
            max = pWork->rssiDir[index][j];
        }
    }

    if(max==0){
        return;
    }
    if(max >= 140){
        max = 140;
    }
    
    
    posx = FX_SinIdx((0xffff/_DIR_SAMPL_NUM)*dir);
    posx = (posx >> (FX16_SHIFT/2));
    posx = posx * (((140-max)));
    sx = (s16)(posx >> (FX16_SHIFT/2));
    
    posy = FX_CosIdx((0xffff/_DIR_SAMPL_NUM)*dir);
    posy = posy >> (FX16_SHIFT/2);
    posy = posy * (((140-max)));
    sy = (s16)(posy >> (FX16_SHIFT/2));
    x = (u16)(sx + 128);
    y = (u16)(96 - sy);

    _InitAPChr(pWork, x, y, 144);
}


//----------------------------------------------------------------


static void _dispBeacon(SKEL_TEST_BEACON_WORK* pWork)
{
    int i;
    BOOL bEndScan;


	GFL_BG_ClearScreen( TEXT_FRM );

    
    for(i=0;i<NELEMS(_parentIndexList);i++){
        GFL_BMPWIN_MakeScreen( pWork->bmpwin[i] );
    }

    _gameStartCheck(pWork);
    if( _gameStartFunc(pWork) ){
        for(i = 0;i < _BCON_GET_NUM; i++){
            _APDisp(pWork,i);
        }
    }
    for(i = 0;i < _BCON_GET_NUM; i++){
        u8* pData = GFL_NET_GetBeaconMacAddress(i);
        
        if(pData){
//            OS_TPrintf("%d: mac %x\n",i,);
            char msg[100];
            u16 rssi = GFL_NET_WL_GetRssi(i);

            rssi = _backupRssi(pWork,i,rssi);
            
            if(pWork->countDir){
                pWork->rssiDir[i][pWork->dir] = rssi;
            }
            
            GFL_STD_MemClear(msg,100);
            OS_SPrintf(msg,"%d : %d %d %d %d %d %d %d %d",rssi,
                       pWork->rssiDir[i][0],
                       pWork->rssiDir[i][1],
                       pWork->rssiDir[i][2],
                       pWork->rssiDir[i][3],
                       pWork->rssiDir[i][4],
                       pWork->rssiDir[i][5],
                       pWork->rssiDir[i][6],
                       pWork->rssiDir[i][7]  );

            GFL_BMPWIN_ClearScreen(pWork->pBmpwinBeacon[ i ]);
            
            pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->pBmpwinBeacon[ i ] );
            GFL_BMP_Clear(pWork->textParam->bmp,0);
            GFL_TEXT_PrintSjisCode( msg, pWork->textParam );

            //�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
            GFL_BMPWIN_TransVramCharacter( pWork->pBmpwinBeacon[i] );

            GFL_BMPWIN_MakeScreen( pWork->pBmpwinBeacon[i] );

        }
        else{
            pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->pBmpwinBeacon[ i ] );
            GFL_BMP_Clear(pWork->textParam->bmp,0);
            GFL_BMPWIN_TransVramCharacter( pWork->pBmpwinBeacon[i] );
            GFL_BMPWIN_MakeScreen( pWork->pBmpwinBeacon[i] );
        }

    }

	GFL_BG_LoadScreenReq( TEXT_FRM );
    clact_draw(pWork);

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


//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���\�[�X�ǂݍ���
 *
 *	@param	clact_res	���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void _ClactResourceLoad( SKEL_TEST_BEACON_WORK *pWork, u32 heapID )
{
	BOOL result;
	void* p_buff;
	NNSG2dCharacterData* p_char;
	NNSG2dPaletteData* p_pltt;
	
	// �L�����N�^�f�[�^�ǂݍ��݁��]��
	{
		p_buff = GFL_ARC_LoadDataAlloc( 0,NARC_radar_ug_radar_obj_NCGR, heapID );
		result = NNS_G2dGetUnpackedCharacterData( p_buff, &p_char );
		GF_ASSERT( result );
		NNS_G2dInitImageProxy( &pWork->res.imageproxy );
		NNS_G2dLoadImage1DMapping( 
				p_char, 
				0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 
				&pWork->res.imageproxy );
		NNS_G2dLoadImage1DMapping( 
				p_char, 
				0, 
				NNS_G2D_VRAM_TYPE_2DSUB, 
				&pWork->res.imageproxy );
		GFL_HEAP_FreeMemory( p_buff );
	}

	// �p���b�g�f�[�^�ǂݍ��݁��]��
	{
		p_buff = GFL_ARC_LoadDataAlloc( 0,NARC_radar_ug_radar_obj_NCLR, heapID );
		result = NNS_G2dGetUnpackedPaletteData( p_buff, &p_pltt );
		GF_ASSERT( result );
		NNS_G2dInitImagePaletteProxy( &pWork->res.plttproxy );
		NNS_G2dLoadPalette( 
				p_pltt, 
				0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 
				&pWork->res.plttproxy );
		NNS_G2dLoadPalette( 
				p_pltt, 
				0, 
				NNS_G2D_VRAM_TYPE_2DSUB, 
				&pWork->res.plttproxy );
		GFL_HEAP_FreeMemory( p_buff );
	}

	// �Z���f�[�^�ǂݍ���
	{
		pWork->res.p_cellbuff = GFL_ARC_LoadDataAlloc( 0,NARC_radar_ug_radar_obj_NCER, heapID );
		result = NNS_G2dGetUnpackedCellBank( pWork->res.p_cellbuff, &pWork->res.p_cell );
		GF_ASSERT( result );
	}

	// �Z���A�j���f�[�^�ǂݍ���
	{
		pWork->res.p_cellanmbuff = GFL_ARC_LoadDataAlloc( 0,NARC_radar_ug_radar_obj_NANR, heapID );
		result = NNS_G2dGetUnpackedAnimBank( pWork->res.p_cellanmbuff, &pWork->res.p_cellanm );
		GF_ASSERT( result );
	}
}

//------------------------------------------------------------------
/**
 * @brief		��ʂ̏�����
 */
//------------------------------------------------------------------

static void _grapInit(SKEL_TEST_BEACON_WORK* pWork)
{
    // �Z���A�N�^�[���j�b�g�쐬
	pWork->p_unit = GFL_CLACT_UNIT_Create( _BCON_GET_NUM, pWork->heapID );
	//�G���A�}�l�[�W��������
	pWork->clact_area=GFL_AREAMAN_Create(_BCON_GET_NUM, pWork->heapID);
	
	//BG�V�X�e��������
	GFL_BG_Init(pWork->heapID);

	//VRAM�ݒ�
	{
		GFL_BG_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_0_B,				// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_0_F				// �e�N�X�`���p���b�g�X���b�g
		};
		GFL_DISP_SetBank( &vramSetTable );

		//VRAM�N���A
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
    GFL_BG_SetBGMode( &bgsysHeader );

	//���C����ʃt���[���ݒ�
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
			},
			///<FRAME2_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
		};
		GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME1_M);
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M);
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
		GFL_BG_SetBGControl(GFL_BG_FRAME2_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_S);
		GFL_BG_SetBGControl(GFL_BG_FRAME3_S, &TextBgCntDat[4], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_S);

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//��ʐ���
	GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_radar_ug_radar_NCGR,GFL_BG_FRAME2_M,0,0,0,pWork->heapID);
	GFL_ARC_UTIL_TransVramScreen(0,NARC_radar_ug_radar_NSCR,GFL_BG_FRAME2_M,0,0,0,pWork->heapID);
	GFL_ARC_UTIL_TransVramPalette(0,NARC_radar_ug_radar_NCLR,PALTYPE_MAIN_BG,0,0x100,pWork->heapID);

	GFL_DISP_SetDispOn();
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,16,0,2);

	//�Z���A�N�^�[���\�[�X�ǂݍ���
	_ClactResourceLoad(pWork, pWork->heapID);


	//BMP�֘A������
	GFL_BMPWIN_Init(pWork->heapID);

    // �ʐM�A�C�R���ĕ`��
    GFL_NET_ReloadIcon();


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
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
#endif  //GFL_NET_WIFI
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    FALSE,  //CRC�������v�Z���邩�ǂ��� TRUE�̏ꍇ���ׂČv�Z����
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

    _grapInit(pWork);
    
    GFL_NET_Init(&aGFLNetInit);

    if(PAD_BUTTON_R & GFL_UI_KEY_GetCont()){
        OS_TPrintf("�e�ɂȂ���\n");
        pWork->bParent = TRUE;
        _CHANGE_STATE(_testParent,0);
    }
    else{
        OS_TPrintf("�q�ɂȂ���\n");
        pWork->bParent = FALSE;
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


//----------------------------------------------------------------------------
/**
 *	@brief	CLACT�o�^
 *  @param  ���[�N�|�C���^
 *	@return	�����������[�N
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* _ClactWorkAdd(SKEL_TEST_BEACON_WORK *pWork, _ACCESS_POINT_PARAM* pAP,
                               s16 posx, s16 posy)
{
    GFL_CLWK_RES	resdat;
	GFL_CLWK		*p_wk;
	GFL_CLWK_DATA	data={
        0,0,
        NARC_radar_ug_radar_obj_NANR,		//�A�j���[�V�����V�[�P���X
        0,		//�D�揇��
        0		//bg�D�揇��
    };

    data.pos_x = posx;
    data.pos_y = posy;

	// ���\�[�X�f�[�^���
	GFL_CLACT_WK_SetCellResData( &resdat, 
                                 &pWork->res.imageproxy, &pWork->res.plttproxy,
                                 pWork->res.p_cell, pWork->res.p_cellanm );
	// �o�^
	//data.anmseq+=fcp->type;
//	data.softpri=soft_pri[fcp->type];
//	data.bgpri=bg_pri[fcp->type];
	p_wk = GFL_CLACT_WK_Add( pWork->p_unit, 
			&data, &resdat,
			CLWK_SETSF_NONE,
			pWork->heapID );
    GF_ASSERT(p_wk);

	// �I�[�g�A�j���[�V�����ݒ�
	GFL_CLACT_WK_SetAutoAnmFlag( p_wk, TRUE );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	AP CLACT����
 *  @param  ���[�N�|�C���^
 *	@return	�����������[�N
 */
//-----------------------------------------------------------------------------
static _ACCESS_POINT_PARAM	*_InitAPChr(SKEL_TEST_BEACON_WORK *pWork, u16 xpos, u16 ypos, u16 rssi)
{
	int clact_no=GFL_AREAMAN_ReserveAuto(pWork->clact_area,1);

    GF_ASSERT(clact_no < _BCON_GET_NUM);
    
    pWork->pAP[clact_no].clact_no = clact_no;
	pWork->clact_work[clact_no] = _ClactWorkAdd(pWork, &pWork->pAP[clact_no],xpos,ypos);
    pWork->pAP[clact_no].clwk = pWork->clact_work[clact_no];
	GFL_CLACT_WK_GetWldPos(pWork->pAP[clact_no].clwk, &pWork->pAP[clact_no].now_pos);

    
	return &pWork->pAP[clact_no];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����폜
 *	@param	clact_no		�������C���i���o�[
 *	@retval void
 */
//-----------------------------------------------------------------------------
static void _DeleteFallChr(SKEL_TEST_BEACON_WORK* pWork,int clact_no)
{

    if(pWork->pAP[clact_no].clwk){
        GFL_CLACT_WK_Remove(pWork->pAP[clact_no].clwk);
    }
    pWork->pAP[clact_no].clwk=NULL;
}
