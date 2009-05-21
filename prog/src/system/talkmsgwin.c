//============================================================================================
/**
 * @file	talkmsgwin.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"

#include "font/font.naix"
#include "print/printsys.h"
#include "print/str_tool.h"
#include "gamesystem\msgspeed.h"

#include "system/talkmsgwin.h"

#include "talkwin_test.naix"
//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�萔
 */
//------------------------------------------------------------------
#define COL_SIZ				(2)
#define PLTT_SIZ			(16*COL_SIZ)

#define BACKGROUND_COLOR	(GX_RGB(31,31,31))
#define BACKGROUND_COLIDX (15)

typedef enum {
	TALKWIN_SETPAT_FLOAT = 0,
	TALKWIN_SETPAT_FIX_U,
	TALKWIN_SETPAT_FIX_D,
}TALKWIN_SETPAT;

#define TALKMSGWIN_FLOAT_MAX	(TALKMSGWIN_NUM - 2)

#define TWIN_FIX_SIZX		(28)
#define TWIN_FIX_SIZY		(5)
#define TWIN_FIX_POSX		(2)
#define TWIN_FIX_POSY_U (1)
#define TWIN_FIX_POSY_D (24 - (TWIN_FIX_SIZY+2))
#define TWIN_FIX_TAIL_X	(7)

typedef enum {
	TAIL_SETPAT_NONE = 0,
	TAIL_SETPAT_U,
	TAIL_SETPAT_D,
	TAIL_SETPAT_L,
	TAIL_SETPAT_R,
	TAIL_SETPAT_FIX_UL,
	TAIL_SETPAT_FIX_UR,
	TAIL_SETPAT_FIX_DL,
	TAIL_SETPAT_FIX_DR,
}TAIL_SETPAT;

//============================================================================================
/**
 *
 * @brief	�\���̒�`
 *
 */
//============================================================================================
typedef struct {
	VecFx32				trans;
	fx32					scale;
	VecFx16				vtxTail0;
	VecFx16				vtxTail1;
	VecFx16				vtxTail2;
	VecFx16				vtxWin0;
	VecFx16				vtxWin1;
	VecFx16				vtxWin2;
	VecFx16				vtxWin3;
	VecFx16				vecN;
	TAIL_SETPAT		tailPat;
}TAIL_DATA;

typedef struct {
	int							seq;
	TALKWIN_SETPAT  winPat;

	PRINT_STREAM*		printStream;
	GFL_BMPWIN*			bmpwin;

	VecFx32*				pTarget;
	STRBUF*					msg;
	GXRgb						color;
	u16							refTarget;

	TAIL_DATA				tailData;

	u8							writex;
	u8							writey;

	u16							timer;
}TMSGWIN;

struct _TALKMSGWIN_SYS{
	TALKMSGWIN_SYS_SETUP	setup;
	TMSGWIN								tmsgwin[TALKMSGWIN_NUM];
  GFL_TCBLSYS*					tcbl;
  u16										chrNum;

	//����m�F�p�b��
	BOOL						debugOn;
};

typedef struct {
	TALKWIN_SETPAT	winPat;
	u8							winpx;			
	u8							winpy;			
	u8							winsx;			
	u8							winsy;			
	GXRgb						color;
}TALKMSGWIN_SETUP;

static u32 setupWindowBG( TALKMSGWIN_SYS_SETUP* setup );

static void initWindow( TMSGWIN* tmsgwin );
static BOOL checkEmptyWindow( TMSGWIN* tmsgwin );
static BOOL checkPrintOnWindow( TMSGWIN* tmsgwin );
static void setupWindow(	TALKMSGWIN_SYS*		tmsgwinSys,
													TMSGWIN*					tmsgwin,
													VecFx32*					pTarget,
													STRBUF*						msg,
													TALKMSGWIN_SETUP* setup );
static void deleteWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin );
static void openWindow( TMSGWIN* tmsgwin );
static void closeWindow( TMSGWIN* tmsgwin );
static void mainfuncWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin );
static void draw3Dwindow( TMSGWIN* tmsgwin);

static BOOL calcTail( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin );
static void	drawTail(TAIL_DATA* tailData, GXRgb color, u16 scaleWait, BOOL tailOnly);

static void writeWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin );
static void clearWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin );

#define TALKMSGWIN_OPENWAIT	(16)
//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�V�X�e���֐�
 *
 *
 *
 *
 *
 */
//============================================================================================
TALKMSGWIN_SYS* TALKMSGWIN_SystemCreate( TALKMSGWIN_SYS_SETUP* setup )
{
	int i;

	TALKMSGWIN_SYS* tmsgwinSys = GFL_HEAP_AllocClearMemory(setup->heapID, sizeof(TALKMSGWIN_SYS));
	tmsgwinSys->setup = *setup;
  tmsgwinSys->tcbl = GFL_TCBL_Init(setup->heapID, setup->heapID, 32, 32);

	for( i=0; i<TALKMSGWIN_NUM; i++ ){ initWindow(&tmsgwinSys->tmsgwin[i]); }

	{
		u32 siz = setupWindowBG(&tmsgwinSys->setup);
		tmsgwinSys->chrNum = siz/0x20;
	}
  tmsgwinSys->debugOn = FALSE;

	return tmsgwinSys;
}

//------------------------------------------------------------------
void TALKMSGWIN_SystemMain( TALKMSGWIN_SYS* tmsgwinSys )
{
	int i;

  GFL_TCBL_Main(tmsgwinSys->tcbl);

	for( i=0; i<TALKMSGWIN_NUM; i++ ){ 
		mainfuncWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[i]); 
	}
}

//------------------------------------------------------------------
void TALKMSGWIN_SystemDraw2D( TALKMSGWIN_SYS* tmsgwinSys )
{
}

//------------------------------------------------------------------
void TALKMSGWIN_SystemDraw3D( TALKMSGWIN_SYS* tmsgwinSys )
{
	int i;

//	GFL_G3D_DRAW_Start();			//�`��J�n

	for( i=0; i<TALKMSGWIN_NUM; i++ ){ draw3Dwindow(&tmsgwinSys->tmsgwin[i]); }

//	GFL_G3D_DRAW_End();				//�`��I���i�o�b�t�@�X���b�v�j
}

//------------------------------------------------------------------
void TALKMSGWIN_SystemDelete( TALKMSGWIN_SYS* tmsgwinSys )
{
	int i;

	for( i=0; i<TALKMSGWIN_NUM; i++ ){ 
		deleteWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[i]); 
	}
  GFL_TCBL_Exit(tmsgwinSys->tcbl);
	GFL_HEAP_FreeMemory(tmsgwinSys);
}

//------------------------------------------------------------------
u32 TALKMSGWIN_SystemGetUsingChrNumber( TALKMSGWIN_SYS* tmsgwinSys )
{
	return tmsgwinSys->chrNum;
}

//------------------------------------------------------------------
void TALKMSGWIN_SystemDebugOn( TALKMSGWIN_SYS* tmsgwinSys )
{
	tmsgwinSys->debugOn = TRUE;
}

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�E�C���h�E�֐�
 *
 *
 *
 *
 *
 */
//============================================================================================
void TALKMSGWIN_CreateFloatWindowIdx(	TALKMSGWIN_SYS*		tmsgwinSys, 
																			int								tmsgwinIdx,
																			VecFx32*					pTarget,
																			STRBUF*						msg,
																			u8								winpx,			
																			u8								winpy,			
																			u8								winsx,			
																			u8								winsy,			
																			u8								colIdx )
{
	TALKMSGWIN_SETUP setup;

	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	setup.winPat = TALKWIN_SETPAT_FLOAT;
	setup.winpx = winpx;
	setup.winpy = winpy;
	setup.winsx = winsx;
	setup.winsy = winsy;
	setup.color = BACKGROUND_COLOR;

	setupWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[tmsgwinIdx], pTarget, msg, &setup);
}

//------------------------------------------------------------------
void TALKMSGWIN_CreateFloatWindowIdxConnect(	TALKMSGWIN_SYS*		tmsgwinSys, 
																							int								tmsgwinIdx,
																							int								prev_tmsgwinIdx,
																							STRBUF*						msg,
																							u8								winpx,			
																							u8								winpy,			
																							u8								winsx,			
																							u8								winsy,			
																							u8								colIdx )
{
	TALKMSGWIN_SETUP setup;

	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );
	GF_ASSERT( (prev_tmsgwinIdx>=0)&&(prev_tmsgwinIdx<TALKMSGWIN_NUM) );

	setup.winPat = TALKWIN_SETPAT_FLOAT;
	setup.winpx = winpx;
	setup.winpy = winpy;
	setup.winsx = winsx;
	setup.winsy = winsy;
	setup.color = BACKGROUND_COLOR;

	setupWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[tmsgwinIdx], 
							tmsgwinSys->tmsgwin[prev_tmsgwinIdx].pTarget, msg, &setup);

	tmsgwinSys->tmsgwin[prev_tmsgwinIdx].pTarget = NULL;
	tmsgwinSys->tmsgwin[prev_tmsgwinIdx].refTarget = tmsgwinIdx;
}

//------------------------------------------------------------------
void TALKMSGWIN_CreateFixWindowUpper( TALKMSGWIN_SYS* tmsgwinSys,
																			int							tmsgwinIdx,
																			VecFx32*				pTarget,
																			STRBUF*					msg,
																			u8							colIdx )
{
	TALKMSGWIN_SETUP setup;

	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	setup.winPat = TALKWIN_SETPAT_FIX_U;
	setup.winpx = TWIN_FIX_POSX;
	setup.winpy = TWIN_FIX_POSY_U;
	setup.winsx = TWIN_FIX_SIZX;
	setup.winsy = TWIN_FIX_SIZY;
	setup.color = BACKGROUND_COLOR;

	setupWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[tmsgwinIdx], pTarget, msg, &setup);
}

void TALKMSGWIN_CreateFixWindowLower( TALKMSGWIN_SYS* tmsgwinSys,
																			int							tmsgwinIdx,
																			VecFx32*				pTarget,
																			STRBUF*					msg,
																			u8							colIdx )
{
	TALKMSGWIN_SETUP setup;

	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	setup.winPat = TALKWIN_SETPAT_FIX_D;
	setup.winpx = TWIN_FIX_POSX;
	setup.winpy = TWIN_FIX_POSY_D;
	setup.winsx = TWIN_FIX_SIZX;
	setup.winsy = TWIN_FIX_SIZY;
	setup.color = BACKGROUND_COLOR;

	setupWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[tmsgwinIdx], pTarget, msg, &setup);
}

void TALKMSGWIN_CreateFixWindowAuto(	TALKMSGWIN_SYS* tmsgwinSys,
																			int							tmsgwinIdx,
																			VecFx32*				pTarget,
																			STRBUF*					msg,
																			u8							colIdx )
{
	int targetx, targety;

	NNS_G3dWorldPosToScrPos(pTarget, &targetx, &targety);

	if( targety < 96 ){ 
		TALKMSGWIN_CreateFixWindowLower(tmsgwinSys, tmsgwinIdx, pTarget, msg, colIdx);
	} else {
		TALKMSGWIN_CreateFixWindowUpper(tmsgwinSys, tmsgwinIdx, pTarget, msg, colIdx);
	}
}

//------------------------------------------------------------------
void TALKMSGWIN_DeleteWindow( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx )
{
	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	deleteWindow(tmsgwinSys, &tmsgwinSys->tmsgwin[tmsgwinIdx]);
}

//------------------------------------------------------------------
void TALKMSGWIN_OpenWindow( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx )
{
	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	openWindow(&tmsgwinSys->tmsgwin[tmsgwinIdx]);
}

//------------------------------------------------------------------
void TALKMSGWIN_CloseWindow( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx )
{
	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	closeWindow(&tmsgwinSys->tmsgwin[tmsgwinIdx]);
}

//------------------------------------------------------------------
BOOL	TALKMSGWIN_CheckPrintOn( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx )	
{
	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	return checkPrintOnWindow(&tmsgwinSys->tmsgwin[tmsgwinIdx]);
}

//------------------------------------------------------------------
PRINT_STREAM*	TALKMSGWIN_GetPrintStream( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx )	
{
	GF_ASSERT( (tmsgwinIdx>=0)&&(tmsgwinIdx<TALKMSGWIN_NUM) );

	return tmsgwinSys->tmsgwin[tmsgwinIdx].printStream; 
}

//------------------------------------------------------------------





//============================================================================================
/**
 * @brief	�E�C���h�E����֐�
 */
//============================================================================================
enum {
	WINSEQ_EMPTY = 0,
	WINSEQ_IDLING,
	WINSEQ_OPEN,
	WINSEQ_HOLD,
	WINSEQ_CLOSE,
};

//------------------------------------------------------------------
static void initWindow( TMSGWIN* tmsgwin )
{
	tmsgwin->seq = WINSEQ_EMPTY;
	tmsgwin->printStream = NULL;
}

//------------------------------------------------------------------
static BOOL checkEmptyWindow( TMSGWIN* tmsgwin )
{
	if( tmsgwin->seq != WINSEQ_EMPTY ){ return FALSE; }

	return TRUE;
}

//------------------------------------------------------------------
static BOOL checkPrintOnWindow( TMSGWIN* tmsgwin )
{
	if( tmsgwin->seq != WINSEQ_HOLD ){ return FALSE; }

	return TRUE;
}

//------------------------------------------------------------------
static void setupWindow(	TALKMSGWIN_SYS*		tmsgwinSys,
													TMSGWIN*					tmsgwin,
													VecFx32*					pTarget,
													STRBUF*						msg,
													TALKMSGWIN_SETUP* setup )
{
	GF_ASSERT( (setup->winpx<32)&&(setup->winpy<24)&&(setup->winsx<=32)&&(setup->winsy<=24) );

	deleteWindow(tmsgwinSys, tmsgwin);

	tmsgwin->pTarget = pTarget;
	tmsgwin->msg = msg;
	tmsgwin->color = setup->color;
	tmsgwin->refTarget = 0;
	tmsgwin->winPat = setup->winPat;

	//�`��p�r�b�g�}�b�v�쐬
	{
		u8 px = ( setup->winpx + setup->winsx <= 32 )? setup->winpx : 32 - setup->winsx;
		u8 py = ( setup->winpy + setup->winsy <= 24 )? setup->winpy : 24 - setup->winsy;

		tmsgwin->bmpwin = GFL_BMPWIN_Create(tmsgwinSys->setup.ini.frameID,
																				px, py, setup->winsx, setup->winsy,
																				tmsgwinSys->setup.ini.fontPltID,
																				GFL_BG_CHRAREA_GET_B );
		//�E�C���h�E����
		GFL_BMP_Clear(GFL_BMPWIN_GetBmp(tmsgwin->bmpwin), BACKGROUND_COLIDX);
		GFL_BMPWIN_TransVramCharacter(tmsgwin->bmpwin);
	}
	//�����o���G�t�F�N�g�p�����[�^�v�Z
	tmsgwin->tailData.tailPat = TAIL_SETPAT_NONE;
	calcTail(tmsgwinSys, tmsgwin);

	//�`��ʒu�Z�o�i�Z���^�����O�j
	{
		u32 width = PRINTSYS_GetStrWidth(msg, tmsgwinSys->setup.fontHandle, 0);
		u32 height = PRINTSYS_GetStrHeight(msg, tmsgwinSys->setup.fontHandle);

		//height���������Ȃ��̂łƂ肠����
		height = setup->winsy*8;

		if(width > (setup->winsx * 8)){
			tmsgwin->writex = 0;
			GF_ASSERT(0);
		} else {
			tmsgwin->writex = (setup->winsx*8 - width)/2;
		}
		if(height > (setup->winsy * 8)){
			tmsgwin->writey = 0;
			GF_ASSERT(0);
		} else {
			tmsgwin->writey = (setup->winsy*8 - height)/2;
		}
	}

	tmsgwin->seq = WINSEQ_IDLING;
}

//------------------------------------------------------------------
static void deleteWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin )
{
	if(checkEmptyWindow(tmsgwin) == FALSE){
		if(tmsgwin->printStream != NULL){
			PRINTSYS_PrintStreamDelete(tmsgwin->printStream);
			tmsgwin->printStream = NULL;
		}
		clearWindow(tmsgwinSys, tmsgwin);

		GFL_BMPWIN_Delete(tmsgwin->bmpwin);
	
		tmsgwin->seq = WINSEQ_EMPTY;
	}
}

//------------------------------------------------------------------
static void openWindow( TMSGWIN* tmsgwin )
{
	tmsgwin->seq = WINSEQ_OPEN;
	tmsgwin->timer = 0;
}

//------------------------------------------------------------------
static void closeWindow( TMSGWIN* tmsgwin )
{
	tmsgwin->seq = WINSEQ_CLOSE;
}

//------------------------------------------------------------------
static void mainfuncWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin )
{
	switch(tmsgwin->seq){
	case WINSEQ_EMPTY:
	case WINSEQ_IDLING:
		break;
	case WINSEQ_OPEN:
		if(tmsgwin->timer < TALKMSGWIN_OPENWAIT){
			tmsgwin->timer++;
		}else{
			int wait;
			
			if( tmsgwinSys->debugOn == TRUE ){
				wait = 2;
			} else {
				wait = MSGSPEED_GetWait();
			}

			tmsgwin->seq = WINSEQ_HOLD;
			writeWindow(tmsgwinSys, tmsgwin);

			tmsgwin->printStream = PRINTSYS_PrintStream(	tmsgwin->bmpwin,							// GFL_BMPWIN
																										tmsgwin->writex,							// u16
																										tmsgwin->writey,							// u16
																										tmsgwin->msg,									// STRBUF*
																										tmsgwinSys->setup.fontHandle,	// GFL_FONT*
																										wait,													// int
																										tmsgwinSys->tcbl,							// GFL_TCBLSYS*
																										0,														// u32 tcbpri
																										tmsgwinSys->setup.heapID,			// HEAPID
																										BACKGROUND_COLIDX );					// u16 clrCol
		}
		break;
	case WINSEQ_HOLD:
		if(calcTail(tmsgwinSys, tmsgwin) == TRUE){ writeWindow(tmsgwinSys, tmsgwin); }
		break;
	case WINSEQ_CLOSE:
		deleteWindow(tmsgwinSys, tmsgwin);
		break;
	}
}

//------------------------------------------------------------------
static void draw3Dwindow( TMSGWIN* tmsgwin )
{
	switch(tmsgwin->seq){
	case WINSEQ_EMPTY:
	case WINSEQ_IDLING:
		break;
	case WINSEQ_OPEN:
	case WINSEQ_CLOSE:
		drawTail(&tmsgwin->tailData, tmsgwin->color, tmsgwin->timer, FALSE);
		break;
	case WINSEQ_HOLD:
		drawTail(&tmsgwin->tailData, tmsgwin->color, tmsgwin->timer, TRUE);
		break;
	}
}

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R�c�`��
 *
 *
 *
 *
 *
 */
//============================================================================================
static void	drawTail( TAIL_DATA* tailData, GXRgb color, u16 scaleWait, BOOL tailOnly )
{
	fx32 scale;

	G3_PushMtx();
	//���s�ړ��p�����[�^�ݒ�
	G3_Translate(tailData->trans.x, tailData->trans.y, tailData->trans.z);
	//�O���[�o���X�P�[���ݒ�
	scale = tailData->scale * scaleWait / TALKMSGWIN_OPENWAIT;
	G3_Scale(scale, scale, scale);

	G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, 0, 0, 0, 0, GX_TEXPLTTCOLOR0_USE, 0);

	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(16, 16, 16), TRUE);
	G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 63, 31, 0);
	
	if(tailData->tailPat != TAIL_SETPAT_NONE){
		G3_Begin(GX_BEGIN_TRIANGLES);

		G3_Color(color);

		G3_Vtx(tailData->vtxTail2.x, tailData->vtxTail2.y, tailData->vtxTail2.z); 
		G3_Vtx(tailData->vtxTail0.x, tailData->vtxTail0.y, tailData->vtxTail0.z); 
		G3_Vtx(tailData->vtxTail1.x, tailData->vtxTail1.y, tailData->vtxTail1.z); 

		G3_End();
	}
	if( tailOnly == FALSE ){
		G3_Begin( GX_BEGIN_QUADS );

		G3_Color(color);

		G3_Vtx(tailData->vtxWin2.x, tailData->vtxWin2.y, tailData->vtxWin2.z); 
		G3_Vtx(tailData->vtxWin0.x, tailData->vtxWin0.y, tailData->vtxWin0.z); 
		G3_Vtx(tailData->vtxWin1.x, tailData->vtxWin1.y, tailData->vtxWin1.z); 
		G3_Vtx(tailData->vtxWin3.x, tailData->vtxWin3.y, tailData->vtxWin3.z); 

		G3_End();
	}

	G3_PopMtx(1);
}


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R�c���W�v�Z
 *
 *
 *
 *
 *
 */
//============================================================================================
static void calcStart( GFL_G3D_CAMERA* g3Dcamera, fx32* backupData )
{
	//�ϊ����x���グ�邽��near�̋������Ƃ�
	fx32 near = 64* FX32_ONE;

	GFL_G3D_CAMERA_GetNear(g3Dcamera, backupData);
	GFL_G3D_CAMERA_SetNear(g3Dcamera, &near);
	GFL_G3D_CAMERA_Switching(g3Dcamera);
}

//------------------------------------------------------------------
static void calcEnd( GFL_G3D_CAMERA* g3Dcamera, fx32* backupData )
{
	//near���A
	GFL_G3D_CAMERA_SetNear(g3Dcamera, backupData);
	GFL_G3D_CAMERA_Switching(g3Dcamera);
}

//------------------------------------------------------------------
static void calcTailVtx0_target(	const VecFx32*	pTarget, 
																	int*						pTargetScrx,
																	int*						pTargetScry,
																	VecFx32*				pTailVtx0 )
{
	//�Ώۍ��W��near�N���b�v���ʏ�̍��W�ɕϊ����Atail���_�Z�o
	NNS_G3dWorldPosToScrPos(pTarget, pTargetScrx, pTargetScry);
	NNS_G3dScrPosToWorldLine(*pTargetScrx, *pTargetScry, pTailVtx0, NULL );
}

//------------------------------------------------------------------
static void calcTailVtx0_refwin(	const TMSGWIN*	win, 
																	const TMSGWIN*	refwin,
																	int*						pTargetScrx,
																	int*						pTargetScry,
																	VecFx32*				pTailVtx0 )
{
	//�w��Q�ƃE�C���h�E���W���Atail���_�Z�o���g�̃T�C�Y���l������
	int	px = (GFL_BMPWIN_GetPosX(win->bmpwin)-1) * 8;
	int	py = (GFL_BMPWIN_GetPosY(win->bmpwin)-1) * 8;
	int	sx = (GFL_BMPWIN_GetScreenSizeX(win->bmpwin)+2) * 8;
	int	sy = (GFL_BMPWIN_GetScreenSizeY(win->bmpwin)+2) * 8;
	int	ref_px = (GFL_BMPWIN_GetPosX(refwin->bmpwin)-1) * 8;
	int	ref_py = (GFL_BMPWIN_GetPosY(refwin->bmpwin)-1) * 8;
	int	ref_sx = (GFL_BMPWIN_GetScreenSizeX(refwin->bmpwin)+2) * 8;
	int	ref_sy = (GFL_BMPWIN_GetScreenSizeY(refwin->bmpwin)+2) * 8;

	if((px + sx) < ref_px){
		//�Q��win�̍��ւ̔z�u�ݒ�
		*pTargetScrx = ref_px;
	} else if(px > (ref_px + ref_sx)){
		//�Q��win�̉E�ւ̔z�u�ݒ�
		*pTargetScrx = ref_px + ref_sx;
	} else {
		*pTargetScrx = ref_px + ref_sx/2;
	}
	if((py + sy) < ref_py){
		//�Q��win�̏�ւ̔z�u�ݒ�
		*pTargetScry = ref_py;
	} else if(py > (ref_py + ref_sy)){
		//�Q��win�̉��ւ̔z�u�ݒ�
		*pTargetScry = ref_py + ref_sy;
	} else {
		*pTargetScry = ref_py + ref_sy/2;
	}
	NNS_G3dScrPosToWorldLine(*pTargetScrx, *pTargetScry, pTailVtx0, NULL );
}

//------------------------------------------------------------------
static void getFixWinVtxPosX( int px, int sx, int side, u16* ex1, u16* ex2 )
{
	if(side == 0){
		*ex1 = px + (TWIN_FIX_TAIL_X+1)*8;	//�ʒu���+�g
		*ex2 = *ex1 + 16;
	} else {
		*ex2 = px + sx - (TWIN_FIX_TAIL_X+1)*8;	//�ʒu���+�g
		*ex1 = *ex2 - 16;
	}
}

static u8 calcTailVtx1Vtx2( const TMSGWIN*	win,
														const int				targetScrx,
														const int				targetScry,
														VecFx32*				pTailVtx1,
														VecFx32*				pTailVtx2 )
{
	//tail�������Z�o���g�̃T�C�Y���l������
	int	px = (GFL_BMPWIN_GetPosX(win->bmpwin)-1) * 8;
	int	py = (GFL_BMPWIN_GetPosY(win->bmpwin)-1) * 8;
	int	sx = (GFL_BMPWIN_GetScreenSizeX(win->bmpwin)+2) * 8;
	int	sy = (GFL_BMPWIN_GetScreenSizeY(win->bmpwin)+2) * 8;
	int	tail_length;
	u8	tailPat;
	u16 ex1, ey1, ex2, ey2;

	switch(win->winPat){
	default:
	case TALKWIN_SETPAT_FLOAT:
		tail_length = ((sx>=16)&&(sy>=16))? 16 : sy;	//tail��default = 16

		if(targetScry < py){
			ey1 = py + 1;
			ey2 = py + 1;
			ex1 = px + sx/2 - tail_length/2;
			ex2 = px + sx/2 + tail_length/2;
			tailPat = TAIL_SETPAT_U;
		} else if(targetScry > (py + sy)){
			ey1 = py + sy - 1;
			ey2 = py + sy - 1;
			ex1 = px + sx/2 - tail_length/2;
			ex2 = px + sx/2 + tail_length/2;
			tailPat = TAIL_SETPAT_D;
		} else {
			ey1 = py + sy/2 - tail_length/2;
			ey2 = py + sy/2 + tail_length/2;
			if(targetScrx < px){
				ex1 = px + 1;
				ex2 = px + 1;
				tailPat = TAIL_SETPAT_L;
			} else if(targetScrx > (px + sx)){
				ex1 = px + sx - 1;
				ex2 = px + sx - 1;
				tailPat = TAIL_SETPAT_R;
			} else {
				ex1 = px + sx/2;
				ex2 = px + sx/2;
				tailPat = TAIL_SETPAT_NONE;
			}
		}
		break;
	case TALKWIN_SETPAT_FIX_U:
		{
			int side;
			ey1 = py + sy - 1;
			ey2 = py + sy - 1;

			if(win->tailData.tailPat == TAIL_SETPAT_NONE){
				if(targetScrx < 128){
					side = 0;
					tailPat = TAIL_SETPAT_FIX_DL;
				} else {
					side = 1;
					tailPat = TAIL_SETPAT_FIX_DR;
				}
			} else {
				if(win->tailData.tailPat == TAIL_SETPAT_FIX_DL){
					side = 0;
					tailPat = TAIL_SETPAT_FIX_DL;
				} else {
					side = 1;
					tailPat = TAIL_SETPAT_FIX_DR;
				}
			}
			getFixWinVtxPosX(px, sx, side, &ex1, &ex2);
		}
		break;
	case TALKWIN_SETPAT_FIX_D:
		{
			int side;
			ey1 = py + 1;
			ey2 = py + 1;

			if(win->tailData.tailPat == TAIL_SETPAT_NONE){
				if(targetScrx < 128){
					side = 0;
					tailPat = TAIL_SETPAT_FIX_UL;
				} else {
					side = 1;
					tailPat = TAIL_SETPAT_FIX_UR;
				}
			} else {
				if(win->tailData.tailPat == TAIL_SETPAT_FIX_UL){
					side = 0;
					tailPat = TAIL_SETPAT_FIX_UL;
				} else {
					side = 1;
					tailPat = TAIL_SETPAT_FIX_UR;
				}
			}
			getFixWinVtxPosX(px, sx, side, &ex1, &ex2);
		}
		break;
	}
	NNS_G3dScrPosToWorldLine(ex1, ey1, pTailVtx1, NULL );
	NNS_G3dScrPosToWorldLine(ex2, ey2, pTailVtx2, NULL );

	return tailPat;
}

//------------------------------------------------------------------
static void calcWinVtx( const TMSGWIN*	win, 
												VecFx32*				pWinVtx0,
												VecFx32*				pWinVtx1,
												VecFx32*				pWinVtx2,
												VecFx32*				pWinVtx3 )
{
	//win��near�N���b�v���ʏ�̍��W�ɕϊ����`��͈͂̂�
	int	px = GFL_BMPWIN_GetPosX(win->bmpwin) * 8;
	int	py = GFL_BMPWIN_GetPosY(win->bmpwin) * 8;
	int	sx = GFL_BMPWIN_GetScreenSizeX(win->bmpwin) * 8;
	int	sy = GFL_BMPWIN_GetScreenSizeY(win->bmpwin) * 8;

	NNS_G3dScrPosToWorldLine((px + 0),	(py + 0),		pWinVtx0, NULL );
	NNS_G3dScrPosToWorldLine((px + sx),	(py + 0),		pWinVtx1, NULL );
	NNS_G3dScrPosToWorldLine((px + 0),	(py + sy),	pWinVtx2, NULL );
	NNS_G3dScrPosToWorldLine((px + sx), (py + sy),	pWinVtx3, NULL );
}

//------------------------------------------------------------------
static void calcTailData( const TALKMSGWIN_SYS* tmsgwinSys,
													TMSGWIN*							tmsgwin,
													const VecFx32*				pTailVtx0,
													const VecFx32*				pTailVtx1,
													const VecFx32*				pTailVtx2,
													const VecFx32*				pWinVtx0,
													const VecFx32*				pWinVtx1,
													const VecFx32*				pWinVtx2,
													const VecFx32*				pWinVtx3 )
{
	VecFx32		vecTail1, vecTail2, vecWin0, vecWin1, vecWin2, vecWin3;
	fx32			scale;

	//pTailVtx0��Ƀx�N�g���ϊ�
	VEC_Subtract(pTailVtx1, pTailVtx0, &vecTail1);
	VEC_Subtract(pTailVtx2, pTailVtx0, &vecTail2);
	VEC_Subtract(pWinVtx0, pTailVtx0, &vecWin0);
	VEC_Subtract(pWinVtx1, pTailVtx0, &vecWin1);
	VEC_Subtract(pWinVtx2, pTailVtx0, &vecWin2);
	VEC_Subtract(pWinVtx3, pTailVtx0, &vecWin3);

	//�X�P�[���̌���
	{
		fx32 valTail1 = VEC_Mag(&vecTail1);
		fx32 valTail2 = VEC_Mag(&vecTail2);
		fx32 valWin0 = VEC_Mag(&vecWin0);
		fx32 valWin1 = VEC_Mag(&vecWin1);
		fx32 valWin2 = VEC_Mag(&vecWin2);
		fx32 valWin3 = VEC_Mag(&vecWin3);

		//��Ԓ����x�N�g���̒������X�P�[���ɐݒ�i���_�ݒ�l�̐����̂��߁j
		scale = (valTail1 >= valTail2)? valTail1 : valTail2;
		if( scale < valWin0 ){ scale = valWin0; }
		if( scale < valWin1 ){ scale = valWin1; }
		if( scale < valWin2 ){ scale = valWin2; }
		if( scale < valWin3 ){ scale = valWin3; }
	}
	//���_�f�[�^�ݒ�
	VEC_Set(&tmsgwin->tailData.trans, pTailVtx0->x, pTailVtx0->y, pTailVtx0->z);
	tmsgwin->tailData.scale = scale;

	VEC_Fx16Set(&tmsgwin->tailData.vtxTail0, 0, 0, 0);
	VEC_Fx16Set(&tmsgwin->tailData.vtxTail1, 
							FX_Div(vecTail1.x,scale), FX_Div(vecTail1.y,scale), FX_Div(vecTail1.z,scale));
	VEC_Fx16Set(&tmsgwin->tailData.vtxTail2,
							FX_Div(vecTail2.x,scale), FX_Div(vecTail2.y,scale), FX_Div(vecTail2.z,scale));

	VEC_Fx16Set(&tmsgwin->tailData.vtxWin0, 
							FX_Div(vecWin0.x,scale), FX_Div(vecWin0.y,scale), FX_Div(vecWin0.z,scale));
	VEC_Fx16Set(&tmsgwin->tailData.vtxWin1,
							FX_Div(vecWin1.x,scale), FX_Div(vecWin1.y,scale), FX_Div(vecWin1.z,scale));
	VEC_Fx16Set(&tmsgwin->tailData.vtxWin2,
							FX_Div(vecWin2.x,scale), FX_Div(vecWin2.y,scale), FX_Div(vecWin2.z,scale));
	VEC_Fx16Set(&tmsgwin->tailData.vtxWin3,
							FX_Div(vecWin3.x,scale), FX_Div(vecWin3.y,scale), FX_Div(vecWin3.z,scale));
}


//============================================================================================
static BOOL calcTail( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin )
{
	fx32	nearBackup;
	u8		tailPat;

	calcStart(tmsgwinSys->setup.g3Dcamera, &nearBackup);
	{
		int			targetScrx, targetScry;
		VecFx32	tailVtx0, tailVtx1, tailVtx2;
		VecFx32	winVtx0, winVtx1, winVtx2, winVtx3;

		if(tmsgwin->pTarget != NULL){
			calcTailVtx0_target(tmsgwin->pTarget, &targetScrx, &targetScry, &tailVtx0);
		} else {
			calcTailVtx0_refwin
				(tmsgwin, &tmsgwinSys->tmsgwin[tmsgwin->refTarget], &targetScrx, &targetScry, &tailVtx0);
		}
		tailPat = calcTailVtx1Vtx2(tmsgwin, targetScrx, targetScry, &tailVtx1, &tailVtx2);

		calcWinVtx(tmsgwin, &winVtx0, &winVtx1, &winVtx2, &winVtx3);

		calcTailData
		(tmsgwinSys, tmsgwin, &tailVtx0, &tailVtx1, &tailVtx2, &winVtx0, &winVtx1, &winVtx2, &winVtx3);
	}
	calcEnd(tmsgwinSys->setup.g3Dcamera, &nearBackup);

	if(tmsgwin->tailData.tailPat != tailPat){
		tmsgwin->tailData.tailPat = tailPat;
		return TRUE;
	}
	return FALSE;
}


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�a�f�E�C���h�E�L�����N�^�[
 *
 *
 *
 *
 *
 */
//============================================================================================
static u32 setupWindowBG( TALKMSGWIN_SYS_SETUP* setup )
{
	u32 chrSiz;
	//GFL_BG_FillCharacter(setup->ini.frameID, 0, 1, 0);	// �擪�ɃN���A�L�����z�u
	GFL_BG_ClearScreen(setup->ini.frameID);

	//�p���b�g�]��
	{
		PALTYPE paltype = PALTYPE_MAIN_BG;

		switch(setup->ini.frameID){
		case GFL_BG_FRAME0_M:
		case GFL_BG_FRAME1_M:
		case GFL_BG_FRAME2_M:
		case GFL_BG_FRAME3_M:
			break;
		case GFL_BG_FRAME0_S:
		case GFL_BG_FRAME1_S:
		case GFL_BG_FRAME2_S:
		case GFL_BG_FRAME3_S:
			paltype = PALTYPE_SUB_BG;
			break;
		}
		GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
																	NARC_font_default_nclr,
																	paltype,
																	setup->ini.fontPltID * PLTT_SIZ,
																	PLTT_SIZ,
																	setup->heapID);
		GFL_ARC_UTIL_TransVramPalette(ARCID_TALKWIN_TEST, 
																	NARC_talkwin_test_talkwin_NCLR,
																	paltype,
																	setup->ini.winPltID * PLTT_SIZ,
																	PLTT_SIZ,
																	setup->heapID);
	}
	//�L�����N�^�[�]��
	chrSiz = GFL_ARC_UTIL_TransVramBgCharacter(	ARCID_TALKWIN_TEST, 
																							NARC_talkwin_test_talkwin_NCGR,
																							setup->ini.frameID,
																							setup->chrNumOffs,
																							0,
																							FALSE,
																							setup->heapID);
	return chrSiz;
}

//------------------------------------------------------------------
#define FLIP_H (0x0400)
#define FLIP_V (0x0800)
#define UL_CHR (1)
#define UR_CHR (1 + FLIP_H)
#define DL_CHR (1 + FLIP_V)
#define DR_CHR (1 + FLIP_H + FLIP_V)
#define U_CHR (3)
#define D_CHR (3 + FLIP_V)
#define L_CHR (4)
#define R_CHR (4 + FLIP_H)
#define SPC_CHR (5)

static void writeWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin )
{
	u16	px = GFL_BMPWIN_GetPosX(tmsgwin->bmpwin);
	u16	py = GFL_BMPWIN_GetPosY(tmsgwin->bmpwin);
	u16	sx = GFL_BMPWIN_GetScreenSizeX(tmsgwin->bmpwin);
	u16	sy = GFL_BMPWIN_GetScreenSizeY(tmsgwin->bmpwin);
	BOOL overU = FALSE;
	BOOL overD = FALSE;
	BOOL overL = FALSE;
	BOOL overR = FALSE;
	u8	frameID = tmsgwinSys->setup.ini.frameID;
	u16	wplt = (tmsgwinSys->setup.ini.winPltID) << 12;
	u16	chrOffs = tmsgwinSys->setup.chrNumOffs;

	if(px == 0){ overL = TRUE; }
	if((px + sx) == 32-1){ overR = TRUE; }
	if(py == 0){ overU = TRUE; }
	if((py + sy) == 24-1){ overD = TRUE; }

	//�����`��̈�
	GFL_BMPWIN_MakeScreen(tmsgwin->bmpwin);

	//�g�̈�
	if(overU == FALSE){
		if(overL == FALSE){
			GFL_BG_FillScreen
				(frameID, (wplt | (UL_CHR+chrOffs)), px - 1, py - 1, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
		GFL_BG_FillScreen
			(frameID, (wplt | (U_CHR+chrOffs)), px, py - 1, sx, 1, GFL_BG_SCRWRT_PALIN);
		if(overR == FALSE){
			GFL_BG_FillScreen
				(frameID, (wplt | (UR_CHR+chrOffs)), px + sx, py - 1, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
	}
	if(overL == FALSE){
		GFL_BG_FillScreen
			(frameID, (wplt | (L_CHR+chrOffs)), px - 1, py, 1, sy, GFL_BG_SCRWRT_PALIN);
	}
	if(overR == FALSE){
		GFL_BG_FillScreen
			(frameID, (wplt | (R_CHR+chrOffs)), px + sx, py, 1, sy, GFL_BG_SCRWRT_PALIN);
	}
	if(overD == FALSE){
		if(overL == FALSE){
			GFL_BG_FillScreen
				(frameID, (wplt | (DL_CHR+chrOffs)), px - 1, py + sy, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
		GFL_BG_FillScreen
			(frameID, (wplt | (D_CHR+chrOffs)), px, py + sy, sx, 1, GFL_BG_SCRWRT_PALIN);
		if(overR == FALSE){
			GFL_BG_FillScreen
				(frameID, (wplt | (DR_CHR+chrOffs)), px + sx, py + sy, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
	}
	//tail�ڑ��̈�
	{
		u16	nullChr = (wplt | SPC_CHR+chrOffs);

		switch(tmsgwin->tailData.tailPat){
		case TAIL_SETPAT_U:
			if(overU == FALSE){
				GFL_BG_FillScreen(frameID, nullChr, px + sx/2 - 1, py - 1, 2, 1, GFL_BG_SCRWRT_PALIN);
			}
		break;
		case TAIL_SETPAT_D:
			if(overD == FALSE){
				GFL_BG_FillScreen(frameID, nullChr, px + sx/2 - 1, py + sy, 2, 1, GFL_BG_SCRWRT_PALIN);
			}
		break;
		case TAIL_SETPAT_L:
			if(overL == FALSE){
				GFL_BG_FillScreen(frameID, nullChr, px - 1, py + sy/2 - 1, 1, 2, GFL_BG_SCRWRT_PALIN);
			}
			break;
		case TAIL_SETPAT_R:
			if(overR == FALSE){
				GFL_BG_FillScreen(frameID, nullChr, px + sx, py + sy/2 - 1, 1, 2, GFL_BG_SCRWRT_PALIN);
			}
			break;
		case TAIL_SETPAT_FIX_UL:
			if(overU == FALSE){
				GFL_BG_FillScreen
					(frameID, nullChr, px + TWIN_FIX_TAIL_X, py - 1, 2, 1, GFL_BG_SCRWRT_PALIN);
			}
			break;
		case TAIL_SETPAT_FIX_DL:
			if(overU == FALSE){
				GFL_BG_FillScreen
					(frameID, nullChr, px + TWIN_FIX_TAIL_X, py + sy, 2, 1, GFL_BG_SCRWRT_PALIN);
			}
			break;
		case TAIL_SETPAT_FIX_UR:
			if(overD == FALSE){
				GFL_BG_FillScreen
					(frameID, nullChr, px + sx - TWIN_FIX_TAIL_X -2, py - 1, 2, 1, GFL_BG_SCRWRT_PALIN);
			}
			break;
		case TAIL_SETPAT_FIX_DR:
			if(overD == FALSE){
				GFL_BG_FillScreen
					(frameID, nullChr, px + sx - TWIN_FIX_TAIL_X -2, py + sy, 2, 1, GFL_BG_SCRWRT_PALIN);
			}
			break;
		}
	}
	GFL_BG_LoadScreenReq(frameID);
}

//------------------------------------------------------------------
static void clearWindow( TALKMSGWIN_SYS* tmsgwinSys, TMSGWIN* tmsgwin )
{
	u16	px = GFL_BMPWIN_GetPosX(tmsgwin->bmpwin);
	u16	py = GFL_BMPWIN_GetPosY(tmsgwin->bmpwin);
	u16	sx = GFL_BMPWIN_GetScreenSizeX(tmsgwin->bmpwin);
	u16	sy = GFL_BMPWIN_GetScreenSizeY(tmsgwin->bmpwin);
	BOOL overU = FALSE;
	BOOL overD = FALSE;
	BOOL overL = FALSE;
	BOOL overR = FALSE;
	u8	frameID = tmsgwinSys->setup.ini.frameID;

	GFL_BMPWIN_ClearScreen(tmsgwin->bmpwin);

	if(px == 0){ overL = TRUE; }
	if((px + sx) == 32-1){ overR = TRUE; }
	if(py == 0){ overU = TRUE; }
	if((py + sy) == 24-1){ overD = TRUE; }

	if(overU == FALSE){
		if(overL == FALSE){
			GFL_BG_FillScreen(frameID, 0, px - 1, py - 1, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
		GFL_BG_FillScreen(frameID, 0, px, py - 1, sx, 1, GFL_BG_SCRWRT_PALIN);
		if(overR == FALSE){
			GFL_BG_FillScreen(frameID, 0, px + sx, py - 1, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
	}
	if(overL == FALSE){
		GFL_BG_FillScreen(frameID, 0, px - 1, py, 1, sy, GFL_BG_SCRWRT_PALIN);
	}
	if(overR == FALSE){
		GFL_BG_FillScreen(frameID, 0, px + sx, py, 1, sy, GFL_BG_SCRWRT_PALIN);
	}
	if(overD == FALSE){
		if(overL == FALSE){
			GFL_BG_FillScreen(frameID, 0, px - 1, py + sy, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
		GFL_BG_FillScreen(frameID, 0, px, py + sy, sx, 1, GFL_BG_SCRWRT_PALIN);
		if(overR == FALSE){
			GFL_BG_FillScreen(frameID, 0, px + sx, py + sy, 1, 1, GFL_BG_SCRWRT_PALIN);
		}
	}
	GFL_BG_LoadScreenReq(frameID);
}







