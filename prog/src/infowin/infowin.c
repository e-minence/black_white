//======================================================================
/**
 * @file	���o�[
 * @brief	��ʏ�ɏ풓������\�����
 * @author	ariizumi
 * @data	09/02/23
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"
#include "net/network_define.h"

#include "arc_def.h"
#include "infowin.naix"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
static const u8 INFOWIN_WIDTH = 32;
static const u8 INFOWIN_HEIGHT = 2;

static const u16 INFOWIN_CLEAR_CHR = 0x00;
static const u16 INFOWIN_BLACK_CHR = 0x10;
static const u16 INFOWIN_LINE_CHRNUM = 0x10;	//1��̐�

static const u8 INFOWIN_TIMECHR_START = 0x01;
static const u8 INFOWIN_COLONCHR = 0x0B;
static const u8 INFOWIN_GPFSYNC_CHR_X = 0x00;
static const u8 INFOWIN_GPFSYNC_CHR_Y = INFOWIN_LINE_CHRNUM*2;

static const u8 INFOWIN_TIME_DRAW_X = 1;
static const u8 INFOWIN_TIME_DRAW_WIDTH = 5;
static const u8 INFOWIN_AMPM_DRAW_X = 7;
static const u8 INFOWIN_AMPM_DRAW_WIDTH = 2;

static const u8 INFOWIN_GPFSYNC_DRAW_X = 10;
static const u8 INFOWIN_GPFSYNC_DRAW_WIDTH = 6;

static const u8 INFOWIN_IR_DRAW_X = 16;
static const u8 INFOWIN_IR_DRAW_WIDTH = 4;

static const u8 INFOWIN_WIFI_DRAW_X = 20;
static const u8 INFOWIN_WIFI_DRAW_WIDTH = 4;

static const u8 INFOWIN_BEACON_DRAW_X = 24;
static const u8 INFOWIN_BEACON_DRAW_WIDTH = 4;

static const u8 INFOWIN_WMI_DRAW_X = 30;
static const u8 INFOWIN_WMI_DRAW_WIDTH = 2;

static const u8 INFOWIN_BATTERY_DRAW_X = 28;
static const u8 INFOWIN_BATTERY_DRAW_WIDTH = 2;

static const u16 INFOWIN_CHR_FLIP_X = 0x400;
static const u16 INFOWIN_CHR_FLIP_Y = 0x800;
#define INFOWIN_CALC_NUM_SCR(num,pal) (num+INFOWIN_TIMECHR_START+(pal<<12))
#define INFOWIN_CHECK_BIT(value,bit) (value&bit?TRUE:FALSE)

static const u16 INFOWIN_COLOR_BLACK = 0x0000;
static const u16 INFOWIN_COLOR_WHITE = 0x7FFF;
static const u16 INFOWIN_COLOR_GRAY = 0x318c;
static const u16 INFOWIN_COLOR_RED = 0x187b;
static const u16 INFOWIN_COLOR_YERROW = 0x1e7f;
static const u16 INFOWIN_COLOR_GREEN = 0x0667;
//======================================================================
//	enum
//======================================================================
enum
{
	INFOWIN_REFRESH_TIME		= 0x0001,
	INFOWIN_REFRESH_GPF_SYNC	= 0x0002,
	INFOWIN_REFRESH_IR			= 0x0004,
	INFOWIN_REFRESH_WIFI		= 0x0008,
	INFOWIN_REFRESH_BEACON		= 0x0010,
	INFOWIN_REFRESH_BATTERY		= 0x0020,

	INFOWIN_DISP_GPF_SYNC	= 0x0100,
	INFOWIN_DISP_IR			= 0x0200,
	INFOWIN_DISP_BEACON		= 0x0400,
	
	INFOWIN_REFRESH_MASK = 0x003F,
	INFOWIN_DISP_MASK = 0x0700,
	INFOWIN_FLG_NULL = 0x0000,
};

//�p���b�g�ԍ���`
enum
{
	INFOWIN_PLT_NULL,
	INFOWIN_PLT_COMMON_BLACK,
	INFOWIN_PLT_COMMON_WHITE,
	INFOWIN_PLT_TIME_STR,
	INFOWIN_PLT_GPF_SYNC_BACK,
	INFOWIN_PLT_GPF_SYNC_STR,
	INFOWIN_PLT_IR_BACK,
	INFOWIN_PLT_IR_STR,
	INFOWIN_PLT_WIFI_BACK,
	INFOWIN_PLT_WIFI_STR,
	INFOWIN_PLT_BEACON_BACK,
	INFOWIN_PLT_BEACON_STR,
	INFOWIN_PLT_BATTERY_BACK,
	INFOWIN_PLT_BATTERY_STR,
	INFOWIN_PLT_NOTUSE1,
	INFOWIN_PLT_NOTUSE2,
};


typedef enum
{
	IWS_DISABLE,	//Wifi����
	IWS_ENABLE,		//Wifi�L��(�I����
	IWS_LOCKED,		//�����T�[�o�[
	IWS_GENERAL,	//�������ʏ�T�[�o�[
	IWS_NINTENDO,	//NintendoSpot
	
	IWS_MAX,
}INFOWIN_WIFI_STATE;

typedef enum
{
	ICS_STOP,
	ICS_WAIT_INIT,
	ICS_FINISH_INIT,
	ICS_SCAN_BEACON,
	ICS_WAIT_FINISH,
	
	ICS_MAX,
}INFOWIN_COMM_STATE;
//======================================================================
//	typedef struct
//======================================================================
typedef struct
{
	u32	ncgPos;
	u16	isRefresh;
	u8	bgplane;
	u8	pltNo;
	
	//���v
	u8		min;
	u8		hour;
	BOOL	isDispColon;
	BOOL	isPm;
	u8		batteryCnt;
	
	//Wi-Fi
	INFOWIN_WIFI_STATE	wifiState;
	
	//BATTERY
	u8	batteryVol;		//0~3

	GFL_TCB *vblankFunc;
}INFOWIN_WORK;

//======================================================================
//	proto
//======================================================================

static	void	INFOWIN_VBlankFunc( GFL_TCB* tcb , void* work );
static	void	INFOWIN_SetScrFunc( const u16 topChar , const u8 pltNo , const u8 posX , const u8 posY , const u8 sizeX , const u8 sizeY );
static	void	INFOWIN_SetPltFunc( const u8 pltPos , const u16 *col , const u8 num );
static	void	INFOWIN_InitBg( u8 bgplane , u8 pltNo, HEAPID heapId );
static	void	INFOWIN_UpdateTime(void);

static	void	INFOWIN_WaitCommInitCallBack(void* pWork);
static	void	INFOWIN_WaitCommExitCallBack(void* pWork);
static	void	INFOWIN_UpdateSearchBeacon( void );

INFOWIN_WORK *infoWk = NULL;
u8	infoCommState = ICS_STOP;

//������
//	@param bgplane	BG��
//	@param bgOfs	NCG�̓ǂݍ���VRAM�A�h���X(0x1000�K�v)
//	@param pltNo	�p���b�g�ԍ�(1�{�K�v)
//	@param heapId	�q�[�vID
void	INFOWIN_Init( u8 bgplane , u8 pltNo ,HEAPID heapId )
{
	GF_ASSERT_MSG(infoWk == NULL ,"Infobar is initialized!!\n");
	
	infoWk = GFL_HEAP_AllocMemory( heapId , sizeof( INFOWIN_WORK ) );
	
	infoWk->bgplane = bgplane;
	infoWk->pltNo = pltNo;
	INFOWIN_InitBg( bgplane,pltNo,heapId );
	
	infoWk->vblankFunc = GFUser_VIntr_CreateTCB( INFOWIN_VBlankFunc , (void*)infoWk , 1 );

	infoWk->wifiState = IWS_DISABLE;
	infoWk->batteryVol = 3;
/*
	//�_�~�[�p�ʐM�A�C�R��
	INFOWIN_SetScrFunc( 0x6e,infoWk->pltNo,
						INFOWIN_WMI_DRAW_X,0,
						INFOWIN_WMI_DRAW_WIDTH,2 );
*/
	//��񋭐��I�ɍX�V
	INFOWIN_Update();
	infoWk->isRefresh = INFOWIN_REFRESH_MASK;
}

void	INFOWIN_Update( void )
{
	INFOWIN_UpdateTime();
	//�o�b�e���[
	infoWk->batteryCnt++;
	if(	infoWk->batteryCnt > 200 )

	{
		PMBattery buf;
		if( PM_GetBattery(&buf) == PM_RESULT_SUCCESS )
		{
			const u8 nowVol = (buf==PM_BATTERY_HIGH?3:0);
			if( infoWk->batteryVol != nowVol )
			{
				infoWk->batteryVol = nowVol;
				infoWk->batteryVol |= INFOWIN_REFRESH_BATTERY;
			}
			infoWk->batteryCnt = 0;
		}
	}
	if( INFOWIN_IsStartComm() == TRUE )
	{
		INFOWIN_UpdateSearchBeacon();
	}
}

void	INFOWIN_Exit( void )
{
	GF_ASSERT_MSG(infoWk != NULL ,"Infobar is not initialize!!\n");
	
	GFL_TCB_DeleteTask( infoWk->vblankFunc );
	
	GFL_HEAP_FreeMemory(infoWk);
	infoWk = NULL;
}

static	void	INFOWIN_VBlankFunc( GFL_TCB* tcb , void* work )
{
	BOOL	transReq = FALSE;
	if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_TIME) )
	{
		u8 i;
		u16	scrData[2][5];
		u16	scrDataAmPm[2][2];
		scrData[0][0] = INFOWIN_CALC_NUM_SCR( infoWk->hour/10,infoWk->pltNo)+infoWk->ncgPos;
		scrData[0][1] = INFOWIN_CALC_NUM_SCR( infoWk->hour%10,infoWk->pltNo)+infoWk->ncgPos;
		scrData[0][2] = (infoWk->isDispColon?INFOWIN_COLONCHR:INFOWIN_BLACK_CHR) + (infoWk->pltNo<<12)+infoWk->ncgPos;
		scrData[0][3] = INFOWIN_CALC_NUM_SCR( infoWk->min/10,infoWk->pltNo)+infoWk->ncgPos;
		scrData[0][4] = INFOWIN_CALC_NUM_SCR( infoWk->min%10,infoWk->pltNo)+infoWk->ncgPos;
		scrData[1][0] = scrData[0][0] + INFOWIN_LINE_CHRNUM;
		scrData[1][1] = scrData[0][1] + INFOWIN_LINE_CHRNUM;
		scrData[1][2] = (infoWk->isDispColon?INFOWIN_COLONCHR+INFOWIN_LINE_CHRNUM:INFOWIN_BLACK_CHR) + (infoWk->pltNo<<12)+infoWk->ncgPos;
		scrData[1][3] = scrData[0][3] + INFOWIN_LINE_CHRNUM;
		scrData[1][4] = scrData[0][4] + INFOWIN_LINE_CHRNUM;
		
		
		GFL_BG_WriteScreenExpand( infoWk->bgplane , 
							INFOWIN_TIME_DRAW_X , 0 , 
							INFOWIN_TIME_DRAW_WIDTH , INFOWIN_HEIGHT ,
							scrData,0,0,5,2);
		
		if( infoWk->isPm == TRUE )
		{
			scrDataAmPm[0][0] = 0x0D+(infoWk->pltNo<<12)+infoWk->ncgPos;
			scrDataAmPm[1][0] = 0x1D+(infoWk->pltNo<<12)+infoWk->ncgPos;
		}
		else
		{
			scrDataAmPm[0][0] = 0x0C+(infoWk->pltNo<<12)+infoWk->ncgPos;
			scrDataAmPm[1][0] = 0x1C+(infoWk->pltNo<<12)+infoWk->ncgPos;
		}
		scrDataAmPm[0][1] = (0x0E)+(infoWk->pltNo<<12)+infoWk->ncgPos;
		scrDataAmPm[1][1] = (0x1E)+(infoWk->pltNo<<12)+infoWk->ncgPos;
		GFL_BG_WriteScreenExpand( infoWk->bgplane , 
							INFOWIN_AMPM_DRAW_X , 0 , 
							INFOWIN_AMPM_DRAW_WIDTH , INFOWIN_HEIGHT ,
							scrDataAmPm,0,0,2,2);
		
		infoWk->isRefresh -= INFOWIN_REFRESH_TIME;
		transReq = TRUE;
	}
	
	if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_GPF_SYNC) )
	{
		static const u16 GPFSYNC_TOP_CHAR[2] = {0x20,0x20};
		const u8	flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_GPF_SYNC);
		INFOWIN_SetScrFunc( GPFSYNC_TOP_CHAR[flg],infoWk->pltNo,
							INFOWIN_GPFSYNC_DRAW_X,0,
							INFOWIN_GPFSYNC_DRAW_WIDTH,2 );
		{//Pallet
			static const u16 GPFSYNC_BACK_COLOR[2][2] = {
						{INFOWIN_COLOR_GRAY,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_WHITE,INFOWIN_COLOR_BLACK} };
						
			INFOWIN_SetPltFunc( INFOWIN_PLT_GPF_SYNC_BACK , GPFSYNC_BACK_COLOR[flg] , 2 );
		}
		
		infoWk->isRefresh -= INFOWIN_REFRESH_GPF_SYNC;
		transReq = TRUE;
		
	}
	
	if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_IR) )
	{
		static const u16 IR_TOP_CHAR[2] = {0x48,0x48};
		const u8	flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_IR);
		INFOWIN_SetScrFunc( IR_TOP_CHAR[flg],infoWk->pltNo,
							INFOWIN_IR_DRAW_X,0,
							INFOWIN_IR_DRAW_WIDTH,2 );
		
		{//Pallet
			static const u16 IR_BACK_COLOR[2][2] = {
						{INFOWIN_COLOR_GRAY,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_WHITE,INFOWIN_COLOR_BLACK} };
						
			INFOWIN_SetPltFunc( INFOWIN_PLT_IR_BACK , IR_BACK_COLOR[flg] , 2 );
		}
		infoWk->isRefresh -= INFOWIN_REFRESH_IR;
		transReq = TRUE;
		
	}
	
	if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_WIFI) )
	{
		static const u16 WIFI_TOP_CHAR[IWS_MAX] = {0x28,0x28,0x28,0x28,0x28};
		INFOWIN_SetScrFunc( WIFI_TOP_CHAR[infoWk->wifiState],infoWk->pltNo,
							INFOWIN_WIFI_DRAW_X,0,
							INFOWIN_WIFI_DRAW_WIDTH,2 );
		
		{//Pallet
			static const u16 WIFI_BACK_COLOR[IWS_MAX][2] = {
						{INFOWIN_COLOR_GRAY,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_WHITE,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_RED,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_YERROW,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_GREEN,INFOWIN_COLOR_BLACK} };
						
			INFOWIN_SetPltFunc( INFOWIN_PLT_WIFI_BACK , WIFI_BACK_COLOR[infoWk->wifiState] , 2 );
		}
		infoWk->isRefresh -= INFOWIN_REFRESH_WIFI;
		transReq = TRUE;
		
	}
	
	if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_BEACON) )
	{
		static const u16 BEACON_TOP_CHAR[2] = {0x2C,0x2C};
		const u8	flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON);
		INFOWIN_SetScrFunc( BEACON_TOP_CHAR[flg],infoWk->pltNo,
							INFOWIN_BEACON_DRAW_X,0,
							INFOWIN_BEACON_DRAW_WIDTH,2 );
		
		{//Pallet
			static const u16 BEACON_BACK_COLOR[2][2] = {
						{INFOWIN_COLOR_GRAY,INFOWIN_COLOR_BLACK},
						{INFOWIN_COLOR_WHITE,INFOWIN_COLOR_BLACK} };
						
			INFOWIN_SetPltFunc( INFOWIN_PLT_BEACON_BACK , BEACON_BACK_COLOR[flg] , 2 );
		}
		infoWk->isRefresh -= INFOWIN_REFRESH_BEACON;
		transReq = TRUE;
		
	}
	
	if( INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_REFRESH_BATTERY) )
	{
		static const u16 BATTERY_TOP_CHAR[4] = {0x40,0x41,0x42,0x43};
		const u8	flg = INFOWIN_CHECK_BIT(infoWk->isRefresh,INFOWIN_DISP_BEACON);
		u16	scrData[2][2];
		scrData[0][0] = BATTERY_TOP_CHAR[infoWk->batteryVol] + (infoWk->pltNo<<12) + infoWk->ncgPos;
		scrData[1][0] = scrData[0][0] + INFOWIN_LINE_CHRNUM;
		scrData[0][1] = scrData[0][0] + INFOWIN_CHR_FLIP_X;
		scrData[1][1] = scrData[1][0] + INFOWIN_CHR_FLIP_X;
		
		GFL_BG_WriteScreenExpand( infoWk->bgplane , 
							INFOWIN_BATTERY_DRAW_X , 0, 
							INFOWIN_BATTERY_DRAW_WIDTH , INFOWIN_HEIGHT ,
							scrData,0,0,INFOWIN_BATTERY_DRAW_WIDTH,INFOWIN_HEIGHT);
		{//Pallet
			static const u16 BEACON_BACK_COLOR[4][2] = {
						{INFOWIN_COLOR_RED,INFOWIN_COLOR_WHITE},
						{INFOWIN_COLOR_BLACK,INFOWIN_COLOR_RED},
						{INFOWIN_COLOR_BLACK,INFOWIN_COLOR_WHITE},
						{INFOWIN_COLOR_BLACK,INFOWIN_COLOR_WHITE}};
						
			INFOWIN_SetPltFunc( INFOWIN_PLT_BATTERY_BACK , BEACON_BACK_COLOR[infoWk->batteryVol] , 2 );
		}
		infoWk->isRefresh -= INFOWIN_REFRESH_BATTERY;
		transReq = TRUE;
		
	}
	if( transReq == TRUE )
	{
		GFL_BG_LoadScreenReq( infoWk->bgplane );
	}
}

static	void	INFOWIN_SetScrFunc( const u16 topChar , const u8 pltNo , const u8 posX , const u8 posY , const u8 sizeX , const u8 sizeY )
{
	u8 x,y;
	//���̂Ƃ���ő�T�C�Y�Ŋm�ۑ傫���Ȃ�����v�ύX
	u16 scrData[12];
	GF_ASSERT_MSG( sizeY<=6,"Ysize over! Plz fix source!!\n");
	for( x=0;x<sizeX;x++ )
	{
		for( y=0;y<sizeY;y++ )
		{
			scrData[y*sizeX+x] = topChar+(pltNo<<12)+x+(y*INFOWIN_LINE_CHRNUM)+infoWk->ncgPos;
		}
	}
	GFL_BG_WriteScreenExpand( infoWk->bgplane , 
						posX , posY , 
						sizeX , sizeY ,
						scrData,0,0,sizeX,sizeY);
}

static	void	INFOWIN_SetPltFunc( const u8 pltPos , const u16 *col , const u8 num )
{
	GFL_BG_LoadPalette( infoWk->bgplane , (void*)col , sizeof(u16)*num , sizeof(u16)*(infoWk->pltNo*16 + pltPos) );
}


static	void	INFOWIN_InitBg( u8 bgplane , u8 pltNo, HEAPID heapId )
{
	const PALTYPE palType = (bgplane<=GFL_BG_FRAME3_M ? PALTYPE_MAIN_BG : PALTYPE_SUB_BG );
	ARCHANDLE *arcHdl = GFL_ARC_OpenDataHandle(ARCID_INFOWIN,heapId);
	
	//�̈�̊m��
	infoWk->ncgPos = GFL_BG_AllocCharacterArea( bgplane , 0x1000 , GFL_BG_CHRAREA_GET_B );
	GF_ASSERT_MSG( infoWk->ncgPos != AREAMAN_POS_NOTFOUND ,"Infobar�����ɕK�v�ȃL�����u�q�`�l�̈悪����Ȃ�\n" );

	//�f�[�^�̓ǂݍ���
	GFL_ARCHDL_UTIL_TransVramPalette( arcHdl,NARC_infowin_infobar_NCLR,palType,pltNo*2*0x10,2*0x10,heapId );
	GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHdl,NARC_infowin_infobar_NCGR,bgplane,infoWk->ncgPos,0,FALSE,heapId );
	GFL_ARC_CloseDataHandle(arcHdl);
	
	//�������BG�̐ݒ�
	GFL_BG_FillScreen( bgplane,INFOWIN_BLACK_CHR+infoWk->ncgPos,0,0,INFOWIN_WIDTH,INFOWIN_HEIGHT,pltNo);
	GFL_BG_LoadScreenReq( bgplane );
}



//--------------------------------------------------------------
//	���v�̍X�V
//--------------------------------------------------------------
static	void	INFOWIN_UpdateTime(void)
{
	RTCTime	time;
	
	if( RTC_GetTime( &time ) == RTC_RESULT_SUCCESS )
	{
		const BOOL isDispColon = (BOOL)(time.second&1);
		if( time.hour != infoWk->hour )
		{
			infoWk->hour = time.hour;
			if( infoWk->hour >= 12 )
			{
				infoWk->hour -=12;
				infoWk->isPm = TRUE;
			}
			else
			{
				infoWk->isPm = FALSE;
			}
			infoWk->isRefresh |= INFOWIN_REFRESH_TIME;
		}
		if( time.minute != infoWk->min )
		{
			infoWk->min = time.minute;
			infoWk->isRefresh |= INFOWIN_REFRESH_TIME;
		}
		if( isDispColon != infoWk->isDispColon )
		{
			infoWk->isDispColon = isDispColon;
			infoWk->isRefresh |= INFOWIN_REFRESH_TIME;
		}
	}
}

#pragma mark [>comm func

const BOOL	INFOWIN_IsStartComm( void )
{
	if( infoCommState==ICS_STOP ||
		infoCommState==ICS_WAIT_FINISH )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	
}

const BOOL	INFOWIN_IsStopComm( void )
{
	if( infoCommState==ICS_STOP )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

void	INFOWIN_StartComm( void )
{
	if( infoCommState != ICS_STOP )
	{
		OS_TPrintf("Infobar comm is started!!\n");
		return;
	}
	{
	GFLNetInitializeStruct aGFLNetInit = {
		NULL,		//NetSamplePacketTbl,  // ��M�֐��e�[�u��
		0,			// ��M�e�[�u���v�f��
		NULL,		///< �n�[�h�Őڑ��������ɌĂ΂��
		NULL,		///< �l�S�V�G�[�V�����������ɃR�[��
		NULL,		// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
		NULL,		// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
		NULL,		// �r�[�R���f�[�^�擾�֐�  
		NULL,		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
		NULL,		// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
		NULL,		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
		NULL,		//FatalError
		NULL,		// �ʐM�ؒf���ɌĂ΂��֐�(�I����
		NULL,		// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
		NULL,		///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
		NULL,		///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
		NULL,		///< wifi�t�����h���X�g�폜�R�[���o�b�N
		NULL,		///< DWC�`���̗F�B���X�g	
		NULL,		///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
		0,			///< DWC�ւ�HEAP�T�C�Y
		TRUE,		///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
		0x444,		//ggid  DP=0x333,RANGER=0x178,WII=0x346
		GFL_HEAPID_APP,		//���ɂȂ�heapid
		HEAPID_NETWORK + HEAPDIR_MASK,		//�ʐM�p��create�����HEAPID
		HEAPID_WIFI + HEAPDIR_MASK,		//wifi�p��create�����HEAPID
		HEAPID_NETWORK + HEAPDIR_MASK,		//
		GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
		4,//_MAXNUM,	//�ő�ڑ��l��
		48,//_MAXSIZE,	//�ő呗�M�o�C�g��
		4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
		TRUE,		// CRC�v�Z
		FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
		GFL_NET_TYPE_WIRELESS_SCANONLY,		//�ʐM�^�C�v�̎w��
		TRUE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
		WB_NET_FIELDMOVE_SERVICEID,	//GameServiceID
#if GFL_NET_IRC
		IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
		};
		
		GFL_NET_Init( &aGFLNetInit , INFOWIN_WaitCommInitCallBack , (void*)infoWk );	
	}
	
	infoCommState = ICS_WAIT_INIT;
	
}
void	INFOWIN_StopComm( void )
{
	if( infoCommState == ICS_STOP )
	{
		OS_TPrintf("Infobar comm is stoped!!\n");
		return;
	}
	GFL_NET_Exit(INFOWIN_WaitCommExitCallBack);
	infoCommState = ICS_WAIT_FINISH;
}


static void	INFOWIN_WaitCommInitCallBack(void* pWork)
{
	infoCommState = ICS_FINISH_INIT;
}


static void	INFOWIN_WaitCommExitCallBack(void* pWork)
{
	infoCommState = ICS_STOP;
}

//--------------------------------------------------------------
//	�r�[�R���̑{��
//--------------------------------------------------------------
static	void INFOWIN_UpdateSearchBeacon( void )
{
	u8 bcnIdx = 0;
	int targetIdx = -1;
	void *bcnData;
	BOOL flg = FALSE;

	if( infoCommState == ICS_FINISH_INIT )
	{
		infoCommState = ICS_SCAN_BEACON;
		GFL_NET_StartBeaconScan();
	}

	while( GFL_NET_GetBeaconData(bcnIdx) != NULL )
	{
		bcnData = GFL_NET_GetBeaconData( bcnIdx );
		flg = TRUE;
		bcnIdx++;
	}
	
	if( flg != INFOWIN_CHECK_BIT( infoWk->isRefresh , INFOWIN_DISP_BEACON ) )
	{
		if( flg == TRUE )
		{
			infoWk->isRefresh |= INFOWIN_DISP_BEACON;
			ARI_TPrintf("INFO:beacon is find!\n");
		}
		else
		{
			infoWk->isRefresh -= INFOWIN_DISP_BEACON;
			ARI_TPrintf("INFO:beacon is lost!\n");
		}
		infoWk->isRefresh |= INFOWIN_REFRESH_BEACON;
	}
}