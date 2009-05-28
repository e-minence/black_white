//============================================================================================
/**
 * @file	el_scoreboard.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include <wchar.h>
#include "gflib.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include "font/font.naix"

#include "print/printsys.h"
#include "print/str_tool.h"

#include "system/el_scoreboard.h"

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
 * @brief		�e�N�X�`���쐬�f�[�^
 */
//------------------------------------------------------------------
typedef struct {
	u16						texSizIdxS;
	u16						texSizIdxT;

	NNSGfdTexKey	texVramKey;		//�e�N�X�`���u�q�`�l�L�[
	u32						texOffset;

	GFL_FONT*			fontHandle;
}EL_SCOREBOARD_SETDATA;

//------------------------------------------------------------------
typedef struct {
	u16					siz;
	GXTexSizeS	s;
	GXTexSizeT	t;
}GX_TEXSIZ_TBL;

static const GX_TEXSIZ_TBL GX_texSizTbl[] = {
	{ 16,		GX_TEXSIZE_S16,		GX_TEXSIZE_T16 },
	{ 32,		GX_TEXSIZE_S32,		GX_TEXSIZE_T32 },
	{ 64,		GX_TEXSIZE_S64 ,	GX_TEXSIZE_T64 },
	{ 128,	GX_TEXSIZE_S128,	GX_TEXSIZE_T128 },
	{ 256,	GX_TEXSIZE_S256,	GX_TEXSIZE_T256 },
	{ 512,	GX_TEXSIZE_S512,	GX_TEXSIZE_T512 },
};

static void makeElboard(EL_SCOREBOARD_SETDATA* setData, const STRBUF* str, HEAPID heapID);
//------------------------------------------------------------------
/**
 * @brief		�p���b�g�A�j���f�[�^
 */
//------------------------------------------------------------------
typedef struct {
	NNSGfdTexKey	plttVramKey;		//�e�N�X�`���u�q�`�l�L�[
	u32						plttOffset;
}EL_SCOREBOARD_ANMDATA;

#define COL_SIZ				(2)
#define PLTT_SIZ			(4*COL_SIZ)

#define COL_B1	(0x18C6)
#define COL_B2	(0x2108)
#define COL_F1	(0x021F)
#define COL_F2	(0x031F)

static const u16 plttData0[PLTT_SIZ/COL_SIZ] = { 0x0000, COL_B1, COL_B2, COL_F1 };
static const u16 plttData1[PLTT_SIZ/COL_SIZ] = { 0x0000, COL_B2, COL_B1, COL_F1 };
static const u16 plttData2[PLTT_SIZ/COL_SIZ] = { 0x0000, COL_B1, COL_B2, COL_F2 };
static const u16 plttData3[PLTT_SIZ/COL_SIZ] = { 0x0000, COL_B2, COL_B1, COL_F2 };

static const u16* plttData[] = { 
	plttData0, plttData1, plttData2, plttData3, plttData0, plttData1, plttData2, plttData3,
};

#define BCOL1 (1)
#define BCOL2 (2)
#define LCOL	(3)

static void anmElboard(EL_SCOREBOARD_ANMDATA* anmData, int timer);

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�d���f���I�u�W�F�N�g
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 *
 * @brief	�^�錾
 *
 */
//------------------------------------------------------------------
struct _EL_SCOREBOARD {
	ELB_MODE			mode;
	u16						texSizIdxS;
	u16						texSizIdxT;

	NNSGfdTexKey	texVramKey;		//�e�N�X�`���u�q�`�l�L�[
	NNSGfdPlttKey	plttVramKey;	//�p���b�g�u�q�`�l�L�[

	int						timer;
};

//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g�ǉ�
 */
//------------------------------------------------------------------
EL_SCOREBOARD* ELBOARD_Add( const STRBUF* str, const ELB_MODE mode, HEAPID heapID )
{
	EL_SCOREBOARD* elb = GFL_HEAP_AllocClearMemory(heapID, sizeof(EL_SCOREBOARD));
	GFL_FONT*			fontHandle;

	elb->mode = mode;

	//�t�H���g�n���h���쐬
	fontHandle = GFL_FONT_Create
						(ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID);

	//�e�N�X�`���̈�h�m�c�d�w�擾
	{
		u32 strLen;
		int i;

		elb->texSizIdxS = NELEMS(GX_texSizTbl)-1;	//�����ݒ�
		elb->texSizIdxT = NELEMS(GX_texSizTbl)-1;	//�����ݒ�

		strLen = PRINTSYS_GetStrWidth(str, fontHandle, 0);
		for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
			if(strLen <= GX_texSizTbl[i].siz){
					elb->texSizIdxS = i;
					break;
			}
		}
		strLen = PRINTSYS_GetStrHeight(str, fontHandle);
		for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
			if(strLen <= GX_texSizTbl[i].siz){
					elb->texSizIdxT = i;
					break;
			}
		}
	}
	//�e�N�X�`���u�q�`�l�m��
	{
		u32 texSizS = GX_texSizTbl[elb->texSizIdxS].siz;
		u32 texSizT = GX_texSizTbl[elb->texSizIdxT].siz;
		u32 texVramSiz = texSizS/8 * texSizT/8 * 0x20;
		elb->texVramKey = NNS_GfdAllocTexVram(texVramSiz, FALSE, 0);
		elb->plttVramKey = NNS_GfdAllocPlttVram(16, FALSE, 0);
	}

	//�e�N�X�`���쐬
	{
		EL_SCOREBOARD_SETDATA setData;
		setData.texSizIdxS = elb->texSizIdxS;
		setData.texSizIdxT = elb->texSizIdxT;
		setData.texVramKey = elb->texVramKey;	
		setData.texOffset = 0;
		setData.fontHandle = fontHandle;

		makeElboard(&setData, str, heapID);
	}

	GFL_FONT_Delete(fontHandle);

	elb->timer = 0;

	return elb;
}	

//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g�j��
 */
//------------------------------------------------------------------
void	ELBOARD_Delete( EL_SCOREBOARD* elb )
{
	NNS_GfdFreePlttVram(elb->plttVramKey);
	NNS_GfdFreeTexVram(elb->texVramKey);

	GFL_HEAP_FreeMemory(elb);
}	

//------------------------------------------------------------------
/**
 *
 * @brief	����
 *
 */
//------------------------------------------------------------------
void	ELBOARD_Main( EL_SCOREBOARD* elb )
{
	if( elb->mode == ELB_MODE_NONE ){ return; }

	elb->timer++;
	{
		EL_SCOREBOARD_ANMDATA anmData;

		anmData.plttVramKey = elb->plttVramKey;
		anmData.plttOffset = 0;

		anmElboard(&anmData, elb->timer);
	}
}

//------------------------------------------------------------------
/**
 *
 * @brief	�`��
 *
 */
//------------------------------------------------------------------
#define ELB_DIF			(GX_RGB(31, 31, 31))
#define ELB_AMB			(GX_RGB(31, 31, 31))
#define ELB_SPE			(GX_RGB(31, 31, 31))
#define ELB_EMI			(GX_RGB(31, 31, 31))
#define ELB_POLID 	(63)

void	ELBOARD_Draw( EL_SCOREBOARD* elb, 
										VecFx32* trans, fx32 scale, u16 width, u16 height, 
										GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset )
{
	VecFx16			vecN;
	u16 texSizS = GX_texSizTbl[elb->texSizIdxS].siz;
	u16 texSizT = GX_texSizTbl[elb->texSizIdxT].siz;

	if(width >= texSizS){ width = texSizS; }
	if(height >= texSizT){ height = texSizT; }

	GFL_G3D_CAMERA_Switching(g3Dcamera);

	G3X_Reset();

	GFL_G3D_DRAW_SetLookAt();

	//�J�����ݒ�擾
	{
		VecFx32		camPos, target, vecNtmp;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

		VEC_Subtract( &camPos, &target, &vecNtmp );
		VEC_Normalize( &vecNtmp, &vecNtmp );
		VEC_Fx16Set( &vecN, vecNtmp.x, vecNtmp.y, vecNtmp.z );
	}
	if( g3Dlightset ){ GFL_G3D_LIGHT_Switching( g3Dlightset ); }

	//�O���[�o���X�P�[���ݒ�
	//�����_���W�ݒ�l�ɐ���������̂ŁA�e�N�X�`���T�C�Y�ɍ��킹�邽�߃X�P�[���̕��ŋz������
	scale *= FX16_ONE;
	G3_Scale( scale, scale, scale );

	G3_PushMtx();

	//���s�ړ��p�����[�^�ݒ�
	G3_Translate(FX_Div(trans->x,scale), FX_Div(trans->y,scale), FX_Div(trans->z,scale));

	G3_TexImageParam(	GX_TEXFMT_PLTT16, GX_TEXGEN_TEXCOORD,
										GX_texSizTbl[elb->texSizIdxS].s, GX_texSizTbl[elb->texSizIdxT].t,
										GX_TEXREPEAT_ST, GX_TEXFLIP_NONE,
										GX_TEXPLTTCOLOR0_TRNS, NNS_GfdGetTexKeyAddr(elb->texVramKey) );
	G3_TexPlttBase( NNS_GfdGetPlttKeyAddr(elb->plttVramKey), GX_TEXFMT_PLTT16 );

	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb( ELB_DIF, ELB_AMB, TRUE );
	G3_MaterialColorSpecEmi( ELB_SPE, ELB_EMI, FALSE );
	G3_PolygonAttr(	0x0f, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
									ELB_POLID, 31, GX_POLYGON_ATTR_MISC_NONE );
	
	G3_Begin( GX_BEGIN_QUADS );

	if( g3Dlightset )	{ G3_Normal(vecN.x, vecN.y, vecN.z); } 	//���ʃ|���S���Ȃ̂Ŗ@���x�N�g���͋��p
	else							{ G3_Color(GX_RGB(31, 31, 31)); }

	{
		fx16	vtxX = width / 2;
		fx16	vtxY = height / 2;
		fx32	s0, s1, t0, t1;

		if(elb->mode == ELB_MODE_S){
			s0 = elb->timer * FX32_ONE;	//�\���ʒu�ړ�
			t0 = 0;
		} else {
			s0 = 0;
			t0 = elb->timer * FX32_ONE;	//�\���ʒu�ړ�
		}
		s0 &= 0x003fffff;						//smax = 0x007fff00�̂��ߎ��O�␳
		t0 &= 0x003fffff;						//tmax = 0x007fff00�̂��ߎ��O�␳
		s1 = s0 + width * FX32_ONE;
		t1 = t0 + height * FX32_ONE;

		G3_TexCoord(s0, t0);
		G3_Vtx(-vtxX, vtxY, 0);

		G3_TexCoord(s0, t1);
		G3_Vtx(-vtxX, -vtxY, 0);

		G3_TexCoord(s1, t1);
		G3_Vtx(vtxX, -vtxY, 0);

		G3_TexCoord(s1, t0);
		G3_Vtx(vtxX, vtxY, 0);
	}
	G3_End();
	G3_PopMtx(1);
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�d���f���e�N�X�`���i�����e�N�X�`���ɓ\��t���Ďg�p�j
 *
 *
 *
 *
 *
 */
//============================================================================================
#define INVALID_DATA	(0xffffffff)

struct _EL_SCOREBOARD_TEX {
	ELB_MODE			mode;
	u16						texSizIdxS;
	u16						texSizIdxT;

	NNSGfdTexKey	texVramKey;		//�e�N�X�`���u�q�`�l�L�[
	NNSGfdPlttKey	plttVramKey;	//�p���b�g�u�q�`�l�L�[
	u32						texOffset;
	u32						plttOffset;

	int						timer;
};

//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g�ǉ�(�����e�N�X�`���ɓ\��t��)
 */
//------------------------------------------------------------------
EL_SCOREBOARD_TEX*	ELBOARD_TEX_Add( const GFL_G3D_RES* g3Dtex, const STRBUF* str, HEAPID heapID )
{
	EL_SCOREBOARD_TEX* elb_tex = GFL_HEAP_AllocClearMemory(heapID, sizeof(EL_SCOREBOARD_TEX));

	elb_tex->mode = ELB_MODE_NONE;

	elb_tex->texSizIdxS = NELEMS(GX_texSizTbl)-1;	//�����ݒ�
	elb_tex->texSizIdxT = NELEMS(GX_texSizTbl)-1;	//�����ݒ�
	elb_tex->texOffset = INVALID_DATA;
	elb_tex->plttOffset = INVALID_DATA;

	//binary���������炩��]����f�[�^�擾
	{
		const NNSG3dResTex*		NNSresTex = GFL_G3D_GetResTex(g3Dtex);
		elb_tex->texVramKey = NNSresTex->texInfo.vramKey;
		elb_tex->plttVramKey = NNSresTex->plttInfo.vramKey;

		//tex�f�[�^�擾
		{
			u16							offsDict = NNSresTex->texInfo.ofsDict;
			NNSG3dResDict*	NNSresDict = (NNSG3dResDict*)((u32)NNSresTex + offsDict);
			NNSG3dResName name[] = {"elboard_core"};
			NNSG3dResDictTexData* entryTex;
			GXTexSizeS	s;
			GXTexSizeT	t;
			int i;

			entryTex = NNS_G3dGetResDataByName(NNSresDict, name);
			if(entryTex == NULL){ return elb_tex; }		// find name Error

			elb_tex->texOffset = (entryTex->texImageParam & 0x0000ffff) << 3;
			s = (entryTex->texImageParam & 0x00700000) >> 20;
			t = (entryTex->texImageParam & 0x03800000) >> 23;

			for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
				if(s == GX_texSizTbl[i].s){
						elb_tex->texSizIdxS = i;
						break;
				}
			}
			for( i=0; i<NELEMS(GX_texSizTbl); i++ ){
				if(t == GX_texSizTbl[i].t){
						elb_tex->texSizIdxT = i;
						break;
				}
			}
			//OS_Printf("tex offset = 0x%x\n", elb_tex->texOffset); 
			//OS_Printf("tex sizIdxS	= %d\n", elb_tex->texSizIdxS);
			//OS_Printf("tex sizIdxT	= %d\n", elb_tex->texSizIdxT);
		}
		//pltt�f�[�^�擾
		{
			u16							offsDict = NNSresTex->plttInfo.ofsDict;
			NNSG3dResDict*	NNSresDict = (NNSG3dResDict*)((u32)NNSresTex + offsDict);
			NNSG3dResName name[] = {"elboard_core_pl"};
			NNSG3dResDictPlttData* entryPltt;

			entryPltt = NNS_G3dGetResDataByName(NNSresDict, name);
			if(entryPltt == NULL){ return elb_tex; }		// find name Error
			if(entryPltt->flag == 1){ return elb_tex; }	// format Error

			elb_tex->plttOffset = (entryPltt->offset & 0x0000ffff) << 3;
			//OS_Printf("pltt offset = 0x%x\n", elb_tex->plttOffset); 
		}
	}
	{
		//�t�H���g�n���h���쐬
		GFL_FONT*	fontHandle = GFL_FONT_Create
						(ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID);

		//�e�N�X�`���쐬
		{
			EL_SCOREBOARD_SETDATA setData;
			setData.texSizIdxS = elb_tex->texSizIdxS;
			setData.texSizIdxT = elb_tex->texSizIdxT;
			setData.texVramKey = elb_tex->texVramKey;	
			setData.texOffset = elb_tex->texOffset;	
			setData.fontHandle = fontHandle;
	
			makeElboard(&setData, str, heapID);
		}
		GFL_FONT_Delete(fontHandle);
	}

	elb_tex->timer = 0;

	return elb_tex;
}	

//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g�j��
 */
//------------------------------------------------------------------
void	ELBOARD_TEX_Delete( EL_SCOREBOARD_TEX* elb_tex )
{
	GFL_HEAP_FreeMemory(elb_tex);
}	

//------------------------------------------------------------------
/**
 *
 * @brief	����
 *
 */
//------------------------------------------------------------------
void	ELBOARD_TEX_Main( EL_SCOREBOARD_TEX* elb_tex )
{
	elb_tex->timer++;
	if(elb_tex->plttOffset != INVALID_DATA)
	{
		EL_SCOREBOARD_ANMDATA anmData;

		anmData.plttVramKey = elb_tex->plttVramKey;
		anmData.plttOffset = elb_tex->plttOffset;

		anmElboard(&anmData, elb_tex->timer);
	}
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�d���f���e�N�X�`���쐬���]��
 *
 *
 *
 *
 *
 */
//============================================================================================
// �E�C���h�E�w�i�N���A
static void clearBitmap(GFL_BMP_DATA* bmp)
{
	u8*	dataAdrs = GFL_BMP_GetCharacterAdrs(bmp);
	u32	writeSize = GFL_BMP_GetBmpDataSize(bmp);
	int	i;

	for( i= 0; i<writeSize; i++ ){ dataAdrs[i] = ((BCOL2 << 4) | BCOL1); }

}
// ������`��
static void printStr
			(const STRBUF* str, GFL_BMP_DATA* bmp, PRINT_QUE* printQue, GFL_FONT* fontHandle)
{
	PRINTSYS_LSB	lsb = PRINTSYS_LSB_Make(LCOL,0,0);
	clearBitmap(bmp);

	PRINTSYS_PrintQueColor(printQue, bmp, 0, 2, str, fontHandle, lsb);
}
// �r�b�g�}�b�v�ϊ�
static void convBitmap(GFL_BMP_DATA* src, GFL_BMP_DATA* dst)
{
	u16	size_x = GFL_BMP_GetSizeX(src)/8;						//�摜�f�[�^��Xdot�T�C�Y
	u16	size_y = GFL_BMP_GetSizeY(src)/8;						//�摜�f�[�^��Ydot�T�C�Y
	u16	col = (u16)GFL_BMP_GetColorFormat(src);			//�J���[���[�h���f�[�^�T�C�Y
	u16	size_cx = col/8;														//�L�����N�^�f�[�^X�T�C�Y
	u8*	srcAdrs = GFL_BMP_GetCharacterAdrs(src);
	u8*	dstAdrs = GFL_BMP_GetCharacterAdrs(dst);
	int	i, cx, cy, x, y;

	for( i=0; i<size_x * size_y * col; i++ ){
		y = i/(size_x * size_cx);
		x = i%(size_x * size_cx);
		dstAdrs[i] = srcAdrs[ ((y/8)*size_x + (x/size_cx))*col + ((y%8)*size_cx + (x%size_cx)) ];
	}
}

//------------------------------------------------------------------
static void makeElboard(EL_SCOREBOARD_SETDATA* setData, const STRBUF* str, HEAPID heapID)
{
	//�e�N�X�`���쐬
	u16						texSizS, texSizT;
	PRINT_QUE*		printQue;
	GFL_BMP_DATA*	bmp;	
	GFL_BMP_DATA*	bmpTmp;	

	//�v�����g�L���[�n���h���쐬
	printQue = PRINTSYS_QUE_Create(heapID);

	texSizS = GX_texSizTbl[setData->texSizIdxS].siz;
	texSizT = GX_texSizTbl[setData->texSizIdxT].siz;

	//�e�N�X�`���p�r�b�g�}�b�v�쐬
	bmp = GFL_BMP_Create(texSizS/8, texSizT/8, GFL_BMP_16_COLOR, heapID);
	bmpTmp = GFL_BMP_Create(texSizS/8, texSizT/8, GFL_BMP_16_COLOR, heapID);

	printStr(str, bmpTmp, printQue, setData->fontHandle);
	convBitmap(bmpTmp, bmp);
	{
		//�e�N�X�`���]��
		void* src = GFL_BMP_GetCharacterAdrs(bmp);
		u32		dst = NNS_GfdGetTexKeyAddr(setData->texVramKey) + setData->texOffset;
		u32		siz = texSizS/8 * texSizT/8 * 0x20;

		GX_BeginLoadTex(); 
		DC_FlushRange(src, siz);
		GX_LoadTex(src, dst, siz); 
		GX_EndLoadTex(); 
	}

	GFL_BMP_Delete(bmpTmp);
	GFL_BMP_Delete(bmp);

	PRINTSYS_QUE_Clear(printQue);
	PRINTSYS_QUE_Delete(printQue);
}	





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�d���f���p���b�g�A�j��
 *
 *
 *
 *
 *
 */
//============================================================================================
static void anmElboard(EL_SCOREBOARD_ANMDATA* anmData, int timer)
{
	void*	src = (void*)plttData[ timer & 7 ];
	u32		dst = NNS_GfdGetPlttKeyAddr(anmData->plttVramKey) + anmData->plttOffset;
	u32		siz = PLTT_SIZ;

	NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, dst, src, siz);
}

