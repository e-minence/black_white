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
 * @brief		�p���b�g�f�[�^
 */
//------------------------------------------------------------------
#define COL_SIZ				(2)
#define PLTT_SIZ			(16*COL_SIZ)

static const u16 plttData[PLTT_SIZ/2] = { 
	0x0000, 0x18C6, 0x2108, 0x021F, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

static const u16 plttData2[PLTT_SIZ/2] = { 
	0x0000, 0x18C6, 0x2108, 0x031F, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

#define BCOL1 (1)
#define BCOL2 (2)
#define LCOL	(3)

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

	GFL_BMP_DATA*	bmp;	

	int						timer;
};

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

//------------------------------------------------------------------
/**
 * @brief	�E�C���h�E�w�i�N���A
 */
//------------------------------------------------------------------
static void clearBitmap(GFL_BMP_DATA* bmp)
{
	u8*	dataAdrs = GFL_BMP_GetCharacterAdrs(bmp);
	u32	writeSize = GFL_BMP_GetBmpDataSize(bmp);
	int	i;

	for( i= 0; i<writeSize; i++ ){ dataAdrs[i] = ((BCOL2 << 4) | BCOL1); }

}

//------------------------------------------------------------------
/**
 * @brief	������`��
 */
//------------------------------------------------------------------
static void printStr
			(const STRBUF* str, GFL_BMP_DATA* bmp, PRINT_QUE* printQue, GFL_FONT* fontHandle)
{
	PRINTSYS_LSB	lsb = PRINTSYS_LSB_Make(LCOL,0,0);
	clearBitmap(bmp);

	PRINTSYS_PrintQueColor(printQue, bmp, 0, 2, str, fontHandle, lsb);
}

//------------------------------------------------------------------
/**
 * @brief	�r�b�g�}�b�v�ϊ�
 */
//------------------------------------------------------------------
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
 * @brief	�I�u�W�F�N�g�ǉ�
 */
//------------------------------------------------------------------
EL_SCOREBOARD* ELBOARD_Add( const STRBUF* str, const ELB_MODE mode, HEAPID heapID )
{
	EL_SCOREBOARD* elb = GFL_HEAP_AllocClearMemory(heapID, sizeof(EL_SCOREBOARD));

	elb->mode = mode;
	{
		//�e�N�X�`���쐬
		int						sizMode;
		u16						texSizS, texSizT;
		PRINT_QUE*		printQue;
		GFL_FONT*			fontHandle;
		GFL_BMP_DATA*	bmpTmp;	

		//�t�H���g�n���h���쐬
		fontHandle = GFL_FONT_Create
							(ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID);
		//�v�����g�L���[�n���h���쐬
		printQue = PRINTSYS_QUE_Create(heapID);
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
		texSizS = GX_texSizTbl[elb->texSizIdxS].siz;
		texSizT = GX_texSizTbl[elb->texSizIdxT].siz;

		//�e�N�X�`���p�r�b�g�}�b�v�쐬
		elb->bmp = GFL_BMP_Create(texSizS/8, texSizT/8, GFL_BMP_16_COLOR, heapID);
		bmpTmp = GFL_BMP_Create(texSizS/8, texSizT/8, GFL_BMP_16_COLOR, heapID);

		printStr(str, bmpTmp, printQue, fontHandle);
		convBitmap(bmpTmp, elb->bmp);

		GFL_BMP_Delete(bmpTmp);
		PRINTSYS_QUE_Clear(printQue);
		PRINTSYS_QUE_Delete(printQue);
		GFL_FONT_Delete(fontHandle);
	}
	{
		//�e�N�X�`���u�q�`�l�m��
		u32 texVramSiz = GX_texSizTbl[elb->texSizIdxS].siz * GX_texSizTbl[elb->texSizIdxT].siz;
		elb->texVramKey = NNS_GfdAllocTexVram(texVramSiz, FALSE, 0);
		elb->plttVramKey = NNS_GfdAllocPlttVram(16, FALSE, 0);
	}
	{
		//�e�N�X�`���]��
		void* src;
		u32		dst, siz;

		src = GFL_BMP_GetCharacterAdrs(elb->bmp);
		dst = NNS_GfdGetTexKeyAddr(elb->texVramKey);
		siz = NNS_GfdGetTexKeySize(elb->texVramKey);
		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, dst, src, siz);
	}
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

	GFL_BMP_Delete(elb->bmp);
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
	elb->timer++;
	{
		//�p���b�g�A�j���[�V����
		void* src;
		u32 dst, siz;

		if(elb->timer & 0x0004){ src = (void*)plttData; } else { src = (void*)plttData2; }
		dst = NNS_GfdGetPlttKeyAddr(elb->plttVramKey);
		siz = PLTT_SIZ;
		NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, dst, src, siz);
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
#define ELB_AMB			(GX_RGB(16, 16, 16))
#define ELB_SPE			(GX_RGB(16, 16, 16))
#define ELB_EMI			(GX_RGB(0, 0, 0))
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
	G3_PolygonAttr(	GFL_BBD_LIGHTMASK_0123, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
									ELB_POLID, 31, GX_POLYGON_ATTR_MISC_FOG );
	
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






