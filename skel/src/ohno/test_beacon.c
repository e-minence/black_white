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
 * @brief		�������[�N�\����
 */
//------------------------------------------------------------------

struct _SKEL_TEST_BEACON_WORK{
    HEAPID heapID;
    BOOL bParent;   ///< �e�@���[�h���ǂ���
    GFL_TEXT_PRINTPARAM* textParam;
    GFL_BMPWIN*				bmpwin[32];
    int testNo;
};

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
    OS_TPrintf("��r���Ă܂�%d\n",beaconNo);
    return TRUE;
}





//----------------------------------------------------------------
enum{
    _TEST_CONNECT,
    _TEST_CONNECT2,
    _TEST_1,
    _TEST_4,
    _TEST_2,
    _TEST_3,
    _TEST_END,
    _TEST_EXIT,

};


static int _testNo = 0;
static GFL_NETHANDLE* _pHandle=NULL;
static GFL_NETHANDLE* _pHandleServer=NULL;
#define _TEST_TIMING (12)


static void _testChild(SKEL_TEST_BEACON_WORK* pWork)
{
    u8 mac[6] = {0x00,0x09,0xbf,0x08,0x2e,0x6e};
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

    // �N���C�A���g���̃e�X�g
    switch(pWork->testNo){
      case _TEST_CONNECT:
        {
            _pHandle = GFL_NET_CreateHandle();
            GFL_NET_StartBeaconScan(_pHandle );    // �r�[�R����҂�
            //                GFL_NET_InitClientAndConnectToParent(_pHandle, mac);  //mac�A�h���X�֐ڑ�
            //                GFL_NET_ChangeoverConnect(_pHandle); // �����ڑ�
        }
            pWork->testNo++;
            break;
          case _TEST_CONNECT2:
            {
                u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
                if(pData){
                    GFL_NET_ConnectToParent(_pHandle, pData);
                }
            }
            pWork->testNo++;
            break;
          case _TEST_1:
            GFL_NET_RequestNegotiation( _pHandle );
            pWork->testNo++;
            break;

          case _TEST_4:
            {
                const u8 buff[10]={1,2,3,4,5,6,7,8,9,10};
                int i;
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_VARIABLE, 10, buff, FALSE, FALSE);
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_GETSIZE, 0, buff, FALSE, FALSE);
//                for(i=0;i<_TEST_HUGE_SIZE;i++){
//                    _dataSend[i] = (u8)i;
//                }
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_HUGE, 0, _dataSend, FALSE, FALSE);
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_VARIABLE_HUGE, 10, _dataSend, FALSE, FALSE);


                GFL_NET_ChangeMpMode(_pHandle);

            }
            pWork->testNo++;
            break;
            


          case _TEST_2:
            {
                u8 send = _TEST_TIMING;
//                BOOL ret = GFL_NET_SendData(_pHandle, GFL_NET_CMD_TIMING_SYNC,&send);
//                OS_TPrintf("send %d\n",ret);
                GFL_NET_TimingSyncStart(_pHandle, send);
            }
            pWork->testNo++;
            break;
          case _TEST_3:
            if(GFL_NET_IsTimingSync(_pHandle,_TEST_TIMING)){
                NET_PRINT("�^�C�~���O��ꂽ\n");
                pWork->testNo++;
            }
            else{
                NET_PRINT("�^�C�~���O�͎��ĂȂ�\n");
            }
            break;
          case _TEST_END:
            // ���̏�Őؒf
         //   GFL_NET_Disconnect();
            // �ʐM�őS����ؒf
            GFL_NET_SendData(_pHandle, GFL_NET_CMD_EXIT_REQ, NULL);
            pWork->testNo++;
            break;
        }
}


static void _testParent(SKEL_TEST_BEACON_WORK* pWork)
{

    // �T�[�o�[���̃e�X�g  �r�[�R���o���Ă��邾��
    switch(_testNo){
      case _TEST_CONNECT:
        {
            _pHandleServer = GFL_NET_CreateHandle();
            GFL_NET_InitServer(_pHandleServer);   // �T�[�o
            _pHandle = GFL_NET_CreateHandle();  // �N���C�A���g
            //                GFL_NET_ChangeoverConnect(_pHandle); // �����ڑ�
        }
        pWork->testNo++;
        break;
      case _TEST_CONNECT2:
        GFL_NET_RequestNegotiation( _pHandle );
        pWork->testNo++;
        break;
      case _TEST_1:
        break;
    }
}

void TEST_BEACON_Main(SKEL_TEST_BEACON_WORK* pWork)
{
    if(pWork->bParent){
        _testParent(pWork);
    }
    else{
        _testChild(pWork);
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

    GX_DisableBankForLCDC(); // LCDC��VRAMD�����蓖�Ă��Ă�悤�Ȃ̂őł�����
    GX_DisableBankForBG();
    GX_DisableBankForTex();
    GX_DisableBankForClearImage();

    GFL_NET_Init(&aGFLNetInit);

    if(PAD_BUTTON_R == GFL_UI_KEY_GetCont()){
        pWork->bParent = TRUE;
    }
    else if(PAD_BUTTON_L == GFL_UI_KEY_GetCont()){
        pWork->bParent = FALSE;
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
