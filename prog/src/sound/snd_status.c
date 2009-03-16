//============================================================================================
/**
 * @file	snd_status.c
 * @brief	サウンドステータスビューワー
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "arc_def.h"

#include "sound/pm_sndsys.h"
#include "sound/snd_status.h"
//============================================================================================
/**
 *
 *
 *
 * @brief	サウンドステータスビューワー
 *
 *
 *
 */
//============================================================================================
// グラフィックアーカイブ内容ＩＤ定義
enum {
	NARC_soundstatus_soundstatus_NCGR = 0,
	NARC_soundstatus_soundstatus_NCLR = 1,
	NARC_soundstatus_soundstatus_NSCR = 2,
	NARC_soundstatus_soundstatus_vsw_NSCR = 3,
};

static const char arc_path[] = {"soundstatus.narc"};
//============================================================================================
// コントロール定義
enum {
	EVENT_CHECK_MODE = 0,
	SWITCH_SLIDE_MODE,
};

typedef enum {
	SWITCH_TR1 = 0,
	SWITCH_TR2,
	SWITCH_TR3,
	SWITCH_TR4,
	SWITCH_TR5,
	SWITCH_TR6,
	SWITCH_TR7,
	SWITCH_TR8,
	SWITCH_TR9,
	SWITCH_TR10,
	SWITCH_TR11,
	SWITCH_TR12,
	SWITCH_TR13,
	SWITCH_TR14,
	SWITCH_TR15,
	SWITCH_TR16,

	SWITCH_BGM_VOL,
	SWITCH_BGM_PAN,
	SWITCH_BGM_TEMPO,
	SWITCH_BGM_PITCH,

	SWITCH_SE_VOL,
	SWITCH_SE_PAN,
	SWITCH_SE_TEMPO,
	SWITCH_SE_PITCH,

	SWITCH_MAX,
}SWITCH_ID;

//============================================================================================
#define CHANNEL_NUM (16)
#define TRACK_NUM	(16)
#define SWITCH_VAL_MAX	(32)
#define SWITCH_VAL_ZERO	(16)
#define SWITCH_VAL_MIN	(0)

typedef struct {
	s8		valOffs;
}SWITCH_STATUS;

typedef struct {
	SWITCH_ID	volume;
	SWITCH_ID	pan;
	SWITCH_ID	tempo;
	SWITCH_ID	pitch;
}MASTERTRACK_STATUS;

typedef struct {
	SWITCH_ID	volume;
	BOOL		active;
	BOOL		enable;
}TRACK_STATUS;

typedef struct {
	u8			seq;
	SWITCH_ID	swID;
	u8			tpy;
	u8			rectL;
}SWITCH_CONTROL;

struct _GFL_SNDSTATUS {
	HEAPID					heapID;
	GFL_SNDSTATUS_SETUP		setup;
	GFL_DISPUT_VRAMSV*		vramSv;

	SNDChannelInfo			channelInfo[CHANNEL_NUM];
	SNDTrackInfo			bgmTrackInfo[TRACK_NUM];
	TRACK_STATUS			bgmTrackStatus[TRACK_NUM];
	MASTERTRACK_STATUS		bgmMasterTrackStatus;
	MASTERTRACK_STATUS		seMasterTrackStatus;

	SWITCH_CONTROL			swControl;
	SWITCH_STATUS			switchStatus[SWITCH_MAX];
	GFL_UI_TP_HITTBL		eventSwitchTable[SWITCH_MAX+1];

	int						seq;

	void*					volumeSwScrnPattern;
	u16						scrnBuf[32*32];
};

#define PLT_SIZ			(0x20)
#define PUSH_CGX_SIZ	(0x2000)
#define PUSH_SCR_SIZ	(0x800)

static void initStatus( GFL_SNDSTATUS* gflSndStatus );
static void initSwitchControl( GFL_SNDSTATUS* gflSndStatus );

static void loadGraphicData( GFL_SNDSTATUS* gflSndStatus );
static void makeEventSwitchTable( GFL_SNDSTATUS* gflSndStatus );

static BOOL SNDSTATUS_SetInfo( GFL_SNDSTATUS* gflSndStatus );
static BOOL SNDSTATUS_Control( GFL_SNDSTATUS* gflSndStatus );

static void SNDSTATUS_SwitchParamSet( GFL_SNDSTATUS* gflSndStatus, SWITCH_ID swID );
//============================================================================================
/**
 *
 * @brief	システム起動 & 終了
 *
 */
//============================================================================================
extern GFL_SNDSTATUS*	GFL_SNDSTATUS_Create( const GFL_SNDSTATUS_SETUP* setup, HEAPID heapID )
{
	GFL_SNDSTATUS* gflSndStatus;
	int i;

	gflSndStatus = GFL_HEAP_AllocClearMemory( GetHeapLowID(heapID), sizeof(GFL_SNDSTATUS) );

	gflSndStatus->heapID = heapID;
	gflSndStatus->setup = *setup;

	gflSndStatus->seq = 0;
	{
		u16	bgPalMask = 0x0001 << gflSndStatus->setup.bgPalID;
		gflSndStatus->vramSv =  GFL_DISPUT_CreateVramSv(gflSndStatus->setup.bgID, 
														PUSH_CGX_SIZ, PUSH_SCR_SIZ, 
														bgPalMask, heapID );
	}
	gflSndStatus->volumeSwScrnPattern = GFL_ARC_LoadDataFilePathAlloc
					( arc_path, NARC_soundstatus_soundstatus_vsw_NSCR, heapID );

	initStatus( gflSndStatus );
	initSwitchControl( gflSndStatus );

	return gflSndStatus;
}

void	GFL_SNDSTATUS_Delete( GFL_SNDSTATUS* gflSndStatus )
{
	if( gflSndStatus == NULL ) return;

	GFL_HEAP_FreeMemory( gflSndStatus->volumeSwScrnPattern );

	GFL_DISPUT_DeleteVramSv( gflSndStatus->vramSv );
	GFL_HEAP_FreeMemory( gflSndStatus );
}

void	GFL_SNDSTATUS_InitControl( GFL_SNDSTATUS* gflSndStatus )
{
	initStatus( gflSndStatus );
	initSwitchControl( gflSndStatus );
}

void	GFL_SNDSTATUS_SetControlEnable( GFL_SNDSTATUS* gflSndStatus, u16 flag )
{
	if( gflSndStatus == NULL ) return;

	gflSndStatus->setup.controlFlag = flag;
	makeEventSwitchTable( gflSndStatus );
}

void	GFL_SNDSTATUS_ChangeSndHandle( GFL_SNDSTATUS* gflSndStatus, NNSSndHandle* pBgmHandle )
{
	if( gflSndStatus == NULL ) return;

	gflSndStatus->setup.pBgmHandle = pBgmHandle;
}

//============================================================================================
static void initStatus( GFL_SNDSTATUS* gflSndStatus )
{
	SWITCH_ID swID;
	int i;

	for( i=0; i<TRACK_NUM; i++ ){
		swID = SWITCH_TR1 + i;
		gflSndStatus->bgmTrackStatus[i].volume = swID;
		gflSndStatus->bgmTrackStatus[i].active = FALSE;
		gflSndStatus->bgmTrackStatus[i].enable = TRUE;
		gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_MAX;
	}
	swID = SWITCH_BGM_VOL;
	gflSndStatus->bgmMasterTrackStatus.volume = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_MAX;
	
	swID = SWITCH_BGM_PAN;
	gflSndStatus->bgmMasterTrackStatus.pan = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_ZERO;

	swID = SWITCH_BGM_TEMPO;
	gflSndStatus->bgmMasterTrackStatus.tempo = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_ZERO;

	swID = SWITCH_BGM_PITCH;
	gflSndStatus->bgmMasterTrackStatus.pitch = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_ZERO;

	swID = SWITCH_SE_VOL;
	gflSndStatus->seMasterTrackStatus.volume = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_MAX;
	
	swID = SWITCH_SE_PAN;
	gflSndStatus->seMasterTrackStatus.pan = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_ZERO;

	swID = SWITCH_SE_TEMPO;
	gflSndStatus->seMasterTrackStatus.tempo = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_ZERO;

	swID = SWITCH_SE_PITCH;
	gflSndStatus->seMasterTrackStatus.pitch = swID;
	gflSndStatus->switchStatus[swID].valOffs = SWITCH_VAL_ZERO;
}
	
static void initSwitchControl( GFL_SNDSTATUS* gflSndStatus )
{
	gflSndStatus->swControl.seq = EVENT_CHECK_MODE;
	gflSndStatus->swControl.swID = SWITCH_MAX;
	gflSndStatus->swControl.tpy = 0;
	gflSndStatus->swControl.rectL = 0;

	makeEventSwitchTable( gflSndStatus );
}

//============================================================================================
/**
 *
 * @brief	システムメイン
 *
 */
//============================================================================================
enum {
	SEQ_PUSHVRAM = 0,
	SEQ_MAKEDISP,
	SEQ_DISPON,
	SEQ_MAIN,
	SEQ_POPVRAM,
	SEQ_END,
};

BOOL	GFL_SNDSTATUS_Main( GFL_SNDSTATUS* gflSndStatus )
{
	if( gflSndStatus == NULL ){ return FALSE; }

	switch( gflSndStatus->seq ){

	case SEQ_PUSHVRAM:
		GFL_DISPUT_VisibleOff( gflSndStatus->setup.bgID );

		GFL_DISPUT_PushVram( gflSndStatus->vramSv );

		gflSndStatus->seq = SEQ_MAKEDISP;
		break;

	case SEQ_MAKEDISP:
		loadGraphicData( gflSndStatus );
		GFL_DISPUT_SetAlpha( gflSndStatus->setup.bgID, 31, 6 );

		gflSndStatus->seq = SEQ_DISPON;
		break;

	case SEQ_DISPON:
		GFL_DISPUT_VisibleOn( gflSndStatus->setup.bgID );
		gflSndStatus->seq = SEQ_MAIN;
		break;

	case SEQ_MAIN:
		SNDSTATUS_SetInfo( gflSndStatus );

		if( gflSndStatus->setup.cancelKey ){
			if( GFL_UI_KEY_GetTrg() == gflSndStatus->setup.cancelKey ){
				gflSndStatus->seq = SEQ_POPVRAM;
				break;
			}
		}
		if( gflSndStatus->setup.controlFlag != GFL_SNDSTATUS_CONTOROL_NONE ){
			if( SNDSTATUS_Control( gflSndStatus ) == FALSE ){
				gflSndStatus->seq = SEQ_POPVRAM;
			}
		}
		GFL_DISPUT_LoadScr( gflSndStatus->setup.bgID, gflSndStatus->scrnBuf, 0, PUSH_SCR_SIZ );
		break;

	case SEQ_POPVRAM:
		GFL_DISPUT_VisibleOff( gflSndStatus->setup.bgID );

		GFL_DISPUT_PopVram( gflSndStatus->vramSv );

		gflSndStatus->seq = SEQ_END;
		break;

	case SEQ_END:
		GFL_DISPUT_VisibleOn( gflSndStatus->setup.bgID );
		return FALSE;
	}
	return TRUE;
}

//============================================================================================
/**
 *
 * @brief	描画関数
 *
 */
//============================================================================================
static void resetScrAttr( u16* scr, GFL_DISPUT_PALID palID )
{
	int i;
	u16 palMask = palID <<12;

	for( i=0; i<32*32; i++ ){
		scr[i] &= 0x0fff;
		scr[i] |= palMask;
	}
}

//============================================================================================
static void loadGraphicData( GFL_SNDSTATUS* gflSndStatus )
{
	void* binCgx;
	void* binScr;
	void* binPlt;
	void* cgx;
	void* scr;
	void* plt;

	binCgx = GFL_ARC_LoadDataFilePathAlloc
					( arc_path, NARC_soundstatus_soundstatus_NCGR, gflSndStatus->heapID );
	binScr = GFL_ARC_LoadDataFilePathAlloc
					( arc_path, NARC_soundstatus_soundstatus_NSCR, gflSndStatus->heapID );
	binPlt = GFL_ARC_LoadDataFilePathAlloc
					( arc_path, NARC_soundstatus_soundstatus_NCLR, gflSndStatus->heapID );

	cgx = GFL_DISPUT_GetCgxDataNNSbin( binCgx );
	scr = GFL_DISPUT_GetScrDataNNSbin( binScr ); 
	plt = GFL_DISPUT_GetPltDataNNSbin( binPlt );

	GFL_STD_MemCopy32( scr, (void*)gflSndStatus->scrnBuf, PUSH_SCR_SIZ );
	resetScrAttr( gflSndStatus->scrnBuf, gflSndStatus->setup.bgPalID );

	GFL_DISPUT_LoadCgx( gflSndStatus->setup.bgID, cgx, 0, PUSH_CGX_SIZ );
	GFL_DISPUT_LoadScr( gflSndStatus->setup.bgID, gflSndStatus->scrnBuf, 0, PUSH_SCR_SIZ );
	GFL_DISPUT_LoadPlt( gflSndStatus->setup.bgID, plt, PLT_SIZ * gflSndStatus->setup.bgPalID );

	GFL_HEAP_FreeMemory( binPlt );
	GFL_HEAP_FreeMemory( binScr );
	GFL_HEAP_FreeMemory( binCgx );
}

//============================================================================================
static void writeScrnSwitch( GFL_SNDSTATUS* gflSndStatus, u16 param, u16 x, u16 y )
{
	u16*	scrnBuf = gflSndStatus->scrnBuf;
	u16		palMask = gflSndStatus->setup.bgPalID << 12;
	u16*	swScrPattern = (u16*)GFL_DISPUT_GetScrDataNNSbin( gflSndStatus->volumeSwScrnPattern );
	int		offsx, offsy;
	int		i;

	offsy = (param/16)*(32*6);
	offsx = (param%16)*2;

	for( i=0; i<6; i++ ){
		scrnBuf[ (y+i)*32 + x + 0 ] = swScrPattern[offsy + i*32 + offsx + 0] | palMask;
		scrnBuf[ (y+i)*32 + x + 1 ] = swScrPattern[offsy + i*32 + offsx + 1] | palMask;
	}
}

//--------------------------------------------------------------------------------------------
static void writeTrackStatus( GFL_SNDSTATUS* gflSndStatus, TRACK_STATUS* status, u16 x, u16 y )
{
	u16		chrNo;
	u16*	scrnBuf = gflSndStatus->scrnBuf;
	u16		palMask = gflSndStatus->setup.bgPalID << 12;

	writeScrnSwitch( gflSndStatus, gflSndStatus->switchStatus[status->volume].valOffs, x*2, y);
	if( status->active == TRUE ){
		if( status->enable == TRUE ){
			chrNo = 0x42;
		} else {
			chrNo = 0x44;
		}
	} else {
		chrNo = 0x40;
	}
	scrnBuf[ (y+6)*32 + x*2 + 0 ] = (chrNo + 0) | palMask;
	scrnBuf[ (y+6)*32 + x*2 + 1 ] = (chrNo + 1) | palMask;
}

//--------------------------------------------------------------------------------------------
static void writeMasterTrackStatus
	( GFL_SNDSTATUS* gflSndStatus, MASTERTRACK_STATUS* status, u16 x, u16 y )
{
	writeScrnSwitch( gflSndStatus, gflSndStatus->switchStatus[status->volume].valOffs, x+1, y);
	writeScrnSwitch( gflSndStatus, gflSndStatus->switchStatus[status->pan].valOffs, x+5, y);
	writeScrnSwitch( gflSndStatus, gflSndStatus->switchStatus[status->tempo].valOffs, x+9, y);
	writeScrnSwitch( gflSndStatus, gflSndStatus->switchStatus[status->pitch].valOffs, x+13, y);
}

//============================================================================================
static void SetScrnChannelStatus( GFL_SNDSTATUS* gflSndStatus )
{
	int		i, j;
	int		x = 0;
	int		y = 1;
	u16		volume;
	u16		gauge;
	u16*	scrnBuf = gflSndStatus->scrnBuf;
	u16		palMask = gflSndStatus->setup.bgPalID << 12;
	u16		volumeMeterPattern[4] = {0,0,0,0};

	for( i=0; i<CHANNEL_NUM; i++ ){
		volume = gflSndStatus->channelInfo[i].volume;
		gauge = volume * 32 / 2032;
		if( gauge > 32 ){ gauge = 32; }

		{
			int d = gauge / 8;
			int f = gauge % 8;
			for( j=3; j>-1; j-- ){
				if(d){
					volumeMeterPattern[j] = 8;
					d--;
				} else {
					if(f){
						volumeMeterPattern[j] = f;
						f = 0;
					} else {
						volumeMeterPattern[j] = 0;
					}
				}
			}
		}

		for( j=0; j<4; j++ ){
			scrnBuf[ (y+j)*32 + (x+i)*2 + 0 ] = (volumeMeterPattern[j] + 0x60) | palMask;
			scrnBuf[ (y+j)*32 + (x+i)*2 + 1 ] = (volumeMeterPattern[j] + 0x70) | palMask;
		}
	}
}

//--------------------------------------------------------------------------------------------
static void SetScrnTrackStatus( GFL_SNDSTATUS* gflSndStatus )
{
	int	x = 0;
	int	y = 7;
	int i;

	for( i=0; i<TRACK_NUM; i++ ){
		writeTrackStatus( gflSndStatus, &gflSndStatus->bgmTrackStatus[i], x + i, y );
	}
}

//--------------------------------------------------------------------------------------------
static void SetScrnMasterTrackStatus( GFL_SNDSTATUS* gflSndStatus )
{
	writeMasterTrackStatus( gflSndStatus, &gflSndStatus->bgmMasterTrackStatus, 0, 16);
	writeMasterTrackStatus( gflSndStatus, &gflSndStatus->seMasterTrackStatus, 16, 16);
}

//============================================================================================
/**
 *
 * @brief	情報の取得と表示
 *
 */
//============================================================================================
static BOOL SNDSTATUS_SetInfo( GFL_SNDSTATUS* gflSndStatus )
{
	NNSSndHandle*	pBgmHandle = gflSndStatus->setup.pBgmHandle;
	BOOL result;
	int i;

	// サウンドドライバ情報更新
	//NNS_SndUpdateDriverInfo();

	// 更新されたチャンネル情報取得
	for( i=0; i<CHANNEL_NUM; i++ ){
		NNS_SndReadDriverChannelInfo( i, &gflSndStatus->channelInfo[i]); 
	}
	// 更新されたトラック情報取得
	if( pBgmHandle != NULL ){
		for( i=0; i<TRACK_NUM; i++ ){
			gflSndStatus->bgmTrackStatus[i].active = 
				NNS_SndPlayerReadDriverTrackInfo( pBgmHandle, i, &gflSndStatus->bgmTrackInfo[i]);
		}
	} else {
		for( i=0; i<TRACK_NUM; i++ ){
			gflSndStatus->bgmTrackStatus[i].active = FALSE;
		}
	}
	SetScrnChannelStatus( gflSndStatus );
	SetScrnTrackStatus( gflSndStatus );
	SetScrnMasterTrackStatus( gflSndStatus );

	return TRUE;
}

//============================================================================================
/**
 *
 * @brief	タッチパネル判定テーブル
 *
 */
//============================================================================================
#define SWITCH_SX	(16-1)
#define BUTTON_SX	(16-1)
#define BUTTON_SY	(8-1)

#define TRACK_BUTTON_PY	(104)
#define TRACK_SWITCH_PY	(56)
#define TRACK1_PX		(0)
#define TRACK2_PX		(16)
#define TRACK3_PX		(32)
#define TRACK4_PX		(48)
#define TRACK5_PX		(64)
#define TRACK6_PX		(80)
#define TRACK7_PX		(96)
#define TRACK8_PX		(112)
#define TRACK9_PX		(128)
#define TRACK10_PX		(144)
#define TRACK11_PX		(160)
#define TRACK12_PX		(176)
#define TRACK13_PX		(192)
#define TRACK14_PX		(208)
#define TRACK15_PX		(224)
#define TRACK16_PX		(240)

#define MTRACK_SWITCH_PY	(128)
#define MTRACK_VOLUME_PX	(8)
#define MTRACK_PAN_PX		(40)
#define MTRACK_TEMPO_PX		(72)
#define MTRACK_PITCH_PX		(104)

#define BGM_MTRACK_PX	(0)
#define SE_MTRACK_PX	(128)

#define EXIT_PX			(232)
#define EXIT_PY			(0)
//------------------------------------------------------------------
enum {
	TOUCH_BUTTON_TR1 = 0,
	TOUCH_BUTTON_TR2,
	TOUCH_BUTTON_TR3,
	TOUCH_BUTTON_TR4,
	TOUCH_BUTTON_TR5,
	TOUCH_BUTTON_TR6,
	TOUCH_BUTTON_TR7,
	TOUCH_BUTTON_TR8,
	TOUCH_BUTTON_TR9,
	TOUCH_BUTTON_TR10,
	TOUCH_BUTTON_TR11,
	TOUCH_BUTTON_TR12,
	TOUCH_BUTTON_TR13,
	TOUCH_BUTTON_TR14,
	TOUCH_BUTTON_TR15,
	TOUCH_BUTTON_TR16,

	TOUCH_EXIT,
};

static const GFL_UI_TP_HITTBL eventTouchPanelTable[] = {
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK1_PX, TRACK1_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK2_PX, TRACK2_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK3_PX, TRACK3_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK4_PX, TRACK4_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK5_PX, TRACK5_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK6_PX, TRACK6_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK7_PX, TRACK7_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK8_PX, TRACK8_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK9_PX, TRACK9_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK10_PX, TRACK10_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK11_PX, TRACK11_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK12_PX, TRACK12_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK13_PX, TRACK13_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK14_PX, TRACK14_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK15_PX, TRACK15_PX + BUTTON_SX },
{ TRACK_BUTTON_PY, TRACK_BUTTON_PY + BUTTON_SY, TRACK16_PX, TRACK16_PX + BUTTON_SX },

{ EXIT_PY, EXIT_PY+7, EXIT_PX, EXIT_PX+23 },

{GFL_UI_TP_HIT_END,0,0,0},			//終了データ
};

//------------------------------------------------------------------
static void setEventSwitchTable( GFL_SNDSTATUS* gflSndStatus, SWITCH_ID swID, u8 x, u8 y )
{
	GFL_UI_TP_HITTBL* tbl = &gflSndStatus->eventSwitchTable[swID];
	u8	cy;

	cy = (y + 24) - (gflSndStatus->switchStatus[swID].valOffs - SWITCH_VAL_ZERO);// 中心算出;

	// switch y判定設定(幅8)
	tbl->rect.top = cy - 4;
	tbl->rect.bottom = cy + 3;
	// switch x判定設定
	tbl->rect.left = x;
	tbl->rect.right = x + SWITCH_SX;
}

//------------------------------------------------------------------
static void makeEventSwitchTable( GFL_SNDSTATUS* gflSndStatus )
{
	u8	offs, cy;
	int i;

	for( i=0; i<SWITCH_MAX+1; i++ ){
		gflSndStatus->eventSwitchTable[i].rect.top = 0;
		gflSndStatus->eventSwitchTable[i].rect.bottom = 0;
		gflSndStatus->eventSwitchTable[i].rect.left = 0;
		gflSndStatus->eventSwitchTable[i].rect.right = 0;
	}
	for( i=0; i<SWITCH_BGM_VOL; i++ ){
		setEventSwitchTable
			( gflSndStatus, SWITCH_TR1 + i, TRACK1_PX + 16*i, TRACK_SWITCH_PY );
	}
	if( gflSndStatus->setup.controlFlag & GFL_SNDSTATUS_CONTOROL_BGM ){ 
		setEventSwitchTable
			( gflSndStatus, SWITCH_BGM_VOL, BGM_MTRACK_PX + MTRACK_VOLUME_PX, MTRACK_SWITCH_PY );
		//setEventSwitchTable
		//	( gflSndStatus, SWITCH_BGM_PAN, BGM_MTRACK_PX + MTRACK_PAN_PX, MTRACK_SWITCH_PY );
		setEventSwitchTable
			( gflSndStatus, SWITCH_BGM_TEMPO, BGM_MTRACK_PX + MTRACK_TEMPO_PX, MTRACK_SWITCH_PY );
		setEventSwitchTable
			( gflSndStatus, SWITCH_BGM_PITCH, BGM_MTRACK_PX + MTRACK_PITCH_PX, MTRACK_SWITCH_PY );
	}
	if( gflSndStatus->setup.controlFlag & GFL_SNDSTATUS_CONTOROL_SE ){ 
		setEventSwitchTable
			( gflSndStatus, SWITCH_SE_VOL, SE_MTRACK_PX + MTRACK_VOLUME_PX, MTRACK_SWITCH_PY );
		//setEventSwitchTable
		//	( gflSndStatus, SWITCH_SE_PAN, SE_MTRACK_PX + MTRACK_PAN_PX, MTRACK_SWITCH_PY );
		setEventSwitchTable
			( gflSndStatus, SWITCH_SE_TEMPO, SE_MTRACK_PX + MTRACK_TEMPO_PX, MTRACK_SWITCH_PY );
		setEventSwitchTable
			( gflSndStatus, SWITCH_SE_PITCH, SE_MTRACK_PX + MTRACK_PITCH_PX, MTRACK_SWITCH_PY );
	}
	gflSndStatus->eventSwitchTable[SWITCH_MAX].rect.top = GFL_UI_TP_HIT_END;//終了データ埋め込み
	gflSndStatus->eventSwitchTable[SWITCH_MAX].rect.bottom = 0;
	gflSndStatus->eventSwitchTable[SWITCH_MAX].rect.left = 0;
	gflSndStatus->eventSwitchTable[SWITCH_MAX].rect.right = 0;
}

//============================================================================================
/**
 *
 * @brief	タッチパネルイベント判定
 *
 */
//============================================================================================
static BOOL checkTouchPanelEvent( GFL_SNDSTATUS* gflSndStatus )
{
	int tblPos;
	u32 tpx, tpy;

	if( GFL_UI_TP_GetPointTrg( &tpx, &tpy ) == FALSE ){
		return TRUE;
	} 
	tblPos = GFL_UI_TP_HitTrg(eventTouchPanelTable);
	if( tblPos != GFL_UI_TP_HIT_NONE ){
		if( tblPos == TOUCH_EXIT ){
			if( gflSndStatus->setup.controlFlag & GFL_SNDSTATUS_CONTOROL_EXIT ){
				return FALSE;	// 終了
			}
		}
		if((tblPos >= TOUCH_BUTTON_TR1)&&(tblPos <= TOUCH_BUTTON_TR16)){
			int trNo = tblPos - TOUCH_BUTTON_TR1;
			u16	bitMask = 0x0001 << trNo;

			if( gflSndStatus->bgmTrackStatus[trNo].active == TRUE ){
				if( gflSndStatus->bgmTrackStatus[trNo].enable == TRUE ){
					NNS_SndPlayerSetTrackMute( gflSndStatus->setup.pBgmHandle, bitMask, TRUE );
					gflSndStatus->bgmTrackStatus[trNo].enable = FALSE;
				} else {
					NNS_SndPlayerSetTrackMute( gflSndStatus->setup.pBgmHandle, bitMask, FALSE );
					gflSndStatus->bgmTrackStatus[trNo].enable = TRUE;
				}
			}
		}
		return TRUE;
	}
	tblPos = GFL_UI_TP_HitTrg(gflSndStatus->eventSwitchTable);
	if( tblPos != GFL_UI_TP_HIT_NONE ){
		gflSndStatus->swControl.seq = SWITCH_SLIDE_MODE;
		gflSndStatus->swControl.swID = tblPos;
		gflSndStatus->swControl.tpy = tpy;
		gflSndStatus->swControl.rectL = gflSndStatus->eventSwitchTable[tblPos].rect.left;
	}
	return TRUE;
}

//============================================================================================
/**
 *
 * @brief	タッチパネルスライド判定
 *
 */
//============================================================================================
static BOOL checkTouchPanelSlide( GFL_SNDSTATUS* gflSndStatus )
{
	u32 tpx, tpy;
	u8	wxL, wxR;
	s8	valOffs, diff;

	if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){ return FALSE; } 

	wxL = gflSndStatus->swControl.rectL;
	wxR = wxL + SWITCH_SX;
	if( (tpy < wxL)&&(tpy > wxR) ){ return FALSE; }	// x座標範囲外

	diff = -(tpy - gflSndStatus->swControl.tpy);
	if( diff != 0 ){
		gflSndStatus->swControl.tpy = tpy;

		valOffs = gflSndStatus->switchStatus[gflSndStatus->swControl.swID].valOffs;
		valOffs += diff;
		if(valOffs < -SWITCH_VAL_MIN){
			valOffs = -SWITCH_VAL_MIN;
		} else if( valOffs > SWITCH_VAL_MAX ){
			valOffs = SWITCH_VAL_MAX;
		}
		gflSndStatus->switchStatus[gflSndStatus->swControl.swID].valOffs = valOffs;

		SNDSTATUS_SwitchParamSet( gflSndStatus, gflSndStatus->swControl.swID );
	}
	return TRUE;
}

//============================================================================================
/**
 *
 * @brief	メインコントロール	
 *
 */
//============================================================================================
static BOOL SNDSTATUS_Control( GFL_SNDSTATUS* gflSndStatus )
{
	switch( gflSndStatus->swControl.seq ){
	
		case EVENT_CHECK_MODE:
			return checkTouchPanelEvent( gflSndStatus );

		case SWITCH_SLIDE_MODE:
			if( checkTouchPanelSlide( gflSndStatus ) == FALSE ){
				initSwitchControl( gflSndStatus );
			}
			break;
	}
	return TRUE;
}

//============================================================================================
/**
 *
 * @brief	ドライバ設定
 *
 */
//============================================================================================
static const int tempoRatioTable[32+1] = {
	64, 72, 80, 88, 96, 104, 112, 120, 128, 144, 160, 176, 192, 208, 224, 240,
	256,
	288, 320, 352, 384, 416, 448, 480, 512, 576, 640, 704, 768, 832, 896, 960, 1024,
};

//------------------------------------------------------------------
static void SNDSTATUS_SwitchParamSet( GFL_SNDSTATUS* gflSndStatus, SWITCH_ID swID )
{
	int	value;

	switch( swID ){

	case SWITCH_TR1:
	case SWITCH_TR2:
	case SWITCH_TR3:
	case SWITCH_TR4:
	case SWITCH_TR5:
	case SWITCH_TR6:
	case SWITCH_TR7:
	case SWITCH_TR8:
	case SWITCH_TR9:
	case SWITCH_TR10:
	case SWITCH_TR11:
	case SWITCH_TR12:
	case SWITCH_TR13:
	case SWITCH_TR14:
	case SWITCH_TR15:
	case SWITCH_TR16:
		{
			// volume幅 0〜127
			u32	bitMask;
			value = gflSndStatus->switchStatus[gflSndStatus->swControl.swID].valOffs * 4;
			if(value > 127){ value = 127; }

			bitMask = 0x00000001 << swID;
			NNS_SndPlayerSetTrackVolume( gflSndStatus->setup.pBgmHandle, bitMask, value );
		}
		break;

	case SWITCH_BGM_VOL:
		// volume幅 0〜127
		value = gflSndStatus->switchStatus[SWITCH_BGM_VOL].valOffs * 4;
		if(value > 127){ value = 127; }
		NNS_SndPlayerSetVolume( gflSndStatus->setup.pBgmHandle, value );
		break;
	case SWITCH_BGM_PAN:
		break;
	case SWITCH_BGM_TEMPO:
		value = tempoRatioTable[gflSndStatus->switchStatus[SWITCH_BGM_TEMPO].valOffs];
		NNS_SndPlayerSetTempoRatio( gflSndStatus->setup.pBgmHandle, value );
		break;
	case SWITCH_BGM_PITCH:
		// pitch幅 -32768〜32767
		value = (gflSndStatus->switchStatus[SWITCH_BGM_PITCH].valOffs - SWITCH_VAL_ZERO) * 64;
		NNS_SndPlayerSetTrackPitch( gflSndStatus->setup.pBgmHandle, 0xffff, value );
		break;

	case SWITCH_SE_VOL:
		// volume幅 0〜127
		//value = gflSndStatus->switchStatus[SWITCH_SE_VOL].valOffs * 4;
		//if(value > 127){ value = 127; }
		//NNS_SndPlayerSetVolume( gflSndStatus->setup.pSeHandle, value );
		break;
	case SWITCH_SE_PAN:
		break;
	case SWITCH_SE_TEMPO:
		//value = tempoRatioTable[gflSndStatus->switchStatus[SWITCH_SE_TEMPO].valOffs];
		//NNS_SndPlayerSetTempoRatio( gflSndStatus->setup.pSeHandle, value );
		break;
	case SWITCH_SE_PITCH:
		// pitch幅 -32768〜32767
		//value = (gflSndStatus->switchStatus[SWITCH_SE_PITCH].valOffs - SWITCH_VAL_ZERO) * 64;
		//NNS_SndPlayerSetTrackPitch( gflSndStatus->setup.pSeHandle, 0xffff, value );
		break;
	}
}












