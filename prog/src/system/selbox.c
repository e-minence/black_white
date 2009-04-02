/**
 *	@file	selwin.c
 *	@brief	�I�����X�g�E�B���h�E�`��E�R���g���[���V�X�e��
 *	@author	Miyuki Iwasawa
 *	@date	07.10.10
 */

#include "common.h"
#include "fntsys.h"
#include "system/fontproc.h"
#include "system/bmp_cursor.h"
#include "system/bmp_list.h"
#include "system/snd_tool.h"

#define __SELWIN_H_GLOBAL
#include "system/selbox.h"
#include "system/sbox_gra.naix"

/////////////////////////////////////////////////////////////////////////
///��`�G���A

#define SBOX_SE_DECIDE	(SEQ_SE_DP_SELECT)
#define SBOX_ENDWAIT	(8)

enum{
 SEQ_SELECT,
 SEQ_ENDWAIT,
};

#define BGRES_CHRID	(NARC_sbox_gra_bg_ncgr)
#define BGRES_PALID	(NARC_sbox_gra_bg_nclr)

#define FCOL_N_BASE	(3)
#define FCOL_S_BASE	(6)
#define FCOL_NORMAL (GF_PRINTCOLOR_MAKE(1,2,FCOL_N_BASE))
#define FCOL_SELECT (GF_PRINTCOLOR_MAKE(4,5,FCOL_S_BASE))
#define SBOX_FONT	(FONT_TOUCH)

typedef struct _SBOX_BGRES_TRANS{
	GF_BGL_INI* ini;
	u8 frame;
	u8 pal_no;
	u16 char_ofs;
	
	NNSG2dCharacterData* p_char;
	NNSG2dPaletteData* p_pltt;
}SBOX_BGRES_TRANS;

/////////////////////////////////////////////////////////////////////////////////////////
///�v���g�^�C�v
static void sbox_BGResTransReq( SELBOX_SYS* sp,SELBOX_HEADER* head,PALETTE_FADE_PTR palASys,u32 heapID );
static void sbox_vtcb_BGResTrans( TCB_PTR tcb, void* p_work );


static u8 sbox_GetBmpListParam(const BMP_MENULIST_DATA* list,u8 num,u8 font,u8 ofs_x);
static void sbox_BmpWinCreate(SELBOX_WORK* wk);
static void sbox_BmpWinDelete(SELBOX_WORK* wk);
static void sbox_DrawWinFrame(SELBOX_WORK* wk);
static void sbox_DrawList(SELBOX_WORK* wk);
static void sbox_DrawSelectTarget(SELBOX_WORK* wk,u8 idx,BOOL pat);
static void sbox_DrawOff(SELBOX_WORK* wk);
static void sbox_SetCursorPos(SELBOX_WORK* wk,u8 pos);
static u32 sbox_TouchInput(SELBOX_WORK* wk,BOOL* flag);
static u32 sbox_KeyInput(SELBOX_WORK* wk);
static void sbox_SePlay(SELBOX_WORK* wk,u16 se_no);
static void sbox_CallBackFuncExe(SELBOX_WORK* wk,SBOX_CB_MODE mode);

/**
 *	@brief	�Z���N�g�{�b�N�X�@�V�X�e�����[�N�m��
 *
 *	@param	heapID	���p�q�[�v
 *	@param	palASys	�p���b�g�A�j�����[�N�̎Q�ƃ|�C���^�BNULL�w���
 */
SELBOX_SYS*	SelectBoxSys_AllocWork( int heapID, PALETTE_FADE_PTR palASys )
{
	SELBOX_SYS*	wk;

	wk = sys_AllocMemory(heapID,sizeof(SELBOX_SYS));
	MI_CpuClear8(wk,sizeof(SELBOX_SYS));

	//�p�����[�^�ۑ�
	wk->heapID = heapID;

	// �L�����N�^�f�[�^�ǂݍ���
	wk->char_buf = ArcUtil_Load(ARC_SBOX_GRA,BGRES_CHRID, FALSE, heapID, ALLOC_TOP );
	NNS_G2dGetUnpackedBGCharacterData(wk->char_buf,&wk->p_char);

	//�p���b�g�f�[�^�ǂݍ���
	wk->pal_buf = ArcUtil_Load(ARC_SBOX_GRA,BGRES_PALID, FALSE, heapID, ALLOC_TOP);
	NNS_G2dGetUnpackedPaletteData(wk->pal_buf,&wk->p_pltt);

	wk->palASys = palASys;
	return wk;
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�V�X�e�����[�N���
 */
void SelectBoxSys_Free(SELBOX_SYS* wk)
{
	sys_FreeMemoryEz(wk->pal_buf);
	sys_FreeMemoryEz(wk->char_buf);

	MI_CpuClear8(wk,sizeof(SELBOX_SYS));
	sys_FreeMemoryEz(wk);
}

/*
 *	@brief	�Z���N�g�{�b�N�X�@�Z�b�g�@�R�A
 */
static SELBOX_WORK* sub_SelectBoxSetCore(SELBOX_SYS* sp,const SELBOX_HEADER* header,u8 key_mode,
	u8 x,u8 y,u8 width,u8 cursor,SELBOX_CB_FUNC cb_func,void* cb_work,BOOL se_manual_f)
{
	int i;
	SELBOX_WORK* wk = (SELBOX_WORK*)sys_AllocMemory(sp->heapID,sizeof(SELBOX_WORK));
	MI_CpuClear8(wk,sizeof(SELBOX_WORK));

	//�w�b�_�f�[�^���R�s�[
	MI_CpuCopy8(header,&(wk->hed),sizeof(SELBOX_HEADER));
	wk->sys_wk = sp;

	//�ݒ�
	if(cursor < wk->hed.count){
		wk->cp = cursor;
	}
	wk->seq = 0;
	wk->heapID = sp->heapID;

	wk->px = x;
	wk->py = y;
	wk->cp = cursor;
	wk->key_mode = key_mode;
	OS_Printf("cb_func = %x, cb_work = %x",cb_func,cb_work);
	wk->cb_func = cb_func;
	wk->cb_work = cb_work;
	wk->se_manual_f = se_manual_f;

	//�E�B���h�E���擾
	if(width == 0){
		wk->width = sbox_GetBmpListParam(header->list,
			wk->hed.count,SBOX_FONT,wk->hed.prm.ofs_x);
	}else{
		wk->width = width;
	}

	//�E�B���h�E�p�����[�^����
	sbox_BmpWinCreate(wk);
	
	//���\�[�X�]�����N�G�X�g
	sbox_BGResTransReq( sp,&wk->hed,wk->sys_wk->palASys,wk->heapID);

	//�`��
	sbox_DrawWinFrame(wk);
	sbox_DrawList(wk);
	sbox_SetCursorPos(wk,wk->cp);
	sbox_SePlay(wk,SBOX_SE_DECIDE);

	//���������R�[���o�b�N�Ăяo��
	sbox_CallBackFuncExe(wk,SBOX_CB_MODE_INI);
	return wk;

}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�Z�b�g
 *
 *	@param	header	�Z���N�g�{�b�N�X�w�b�_�|�C���^
 *	@param	key_mode	���샂�[�h(APP_END_KEY or APP_END_TOUCH)
 *	@param	x		�`��J�n�ʒux(char)
 *	@param	y		�`��J�n�ʒuy(char)
 *	@param	width	�E�B���h�E��(char) 0���w�肵���ꍇ�A������T�C�Y�ˑ�
 *	@param	cursor	�����J�[�\���|�W�V����
 
 *	@li	���[�N�������Alloc���邽�߁A�K��SelectBoxExit�ŉ�����Ă�������
 */
SELBOX_WORK* SelectBoxSet(SELBOX_SYS* sp,const SELBOX_HEADER* header,u8 key_mode,u8 x,u8 y,u8 width,u8 cursor)
{
	return sub_SelectBoxSetCore(sp,header,key_mode,x,y,width,cursor,NULL,NULL,FALSE);
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�Z�b�gEx(�R�[���o�b�N�����ASE�������Ŗ炷���ǂ����I�ׂ�)
 *
 *	@param	header	�Z���N�g�{�b�N�X�w�b�_�|�C���^
 *	@param	key_mode	���샂�[�h(APP_END_KEY or APP_END_TOUCH)
 *	@param	x		�`��J�n�ʒux(char)
 *	@param	y		�`��J�n�ʒuy(char)
 *	@param	width	�E�B���h�E��(char) 0���w�肵���ꍇ�A������T�C�Y�ˑ�
 *	@param	cursor	�����J�[�\���|�W�V����
 *	@param	cb_func	�R�[���o�b�N�֐��ւ̃|�C���^
 *	@param	cb_work	�R�[���o�b�N�֐��Ɉ����n���A�C�ӂ̃��[�N�|�C���^
 *	@param	se_manual_f	TRUE��n����SE�����O�Ŗ炷�K�v������
 
 *	@li	���[�N�������Alloc���邽�߁A�K��SelectBoxExit�ŉ�����Ă�������
 */
SELBOX_WORK* SelectBoxSetEx(SELBOX_SYS* sp,const SELBOX_HEADER* header,u8 key_mode,
	u8 x,u8 y,u8 width,u8 cursor,SELBOX_CB_FUNC cb_func,void* cb_work,BOOL se_manual_f)
{
	return sub_SelectBoxSetCore(sp,header,key_mode,x,y,width,cursor,cb_func,cb_work,se_manual_f);
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�I�����̃L�[�^�b�`�X�e�[�^�X���擾
 *
 *	@retval APP_END_KEY
 *	@retval APP_END_TOUCH
 *
 *	SelectBoxExit()�̃R�[���O�ɌĂяo���Ă�������
 */
int SelectBoxGetKTStatus(SELBOX_WORK* wk)
{
	return wk->key_mode;
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�I���E�������
 */
void SelectBoxExit(SELBOX_WORK* wk)
{
	MI_CpuClear8(wk,sizeof(SELBOX_WORK));
	sys_FreeMemoryEz(wk);
	wk = NULL;
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�R���g���[�����C��
 *
 * @retval	"param = �I���p�����[�^"
 * @retval	"SBOX_SELECT_NULL = �I��"
 * @retval	"SBOX_SELECT_CANCEL	= �L�����Z��(�a�{�^��)"
 */
u32 SelectBoxMain(SELBOX_WORK* wk)
{
	u32 ret;
		
	if(wk->seq == SEQ_ENDWAIT){
		if(wk->wait == 0){	//�I��
			sbox_DrawOff(wk);
			sbox_BmpWinDelete(wk);
			return wk->hed.list[wk->ret].param;
		}
		if(wk->wait % 2 == 0){
			if((wk->wait / 2) % 2 == 0){
				sbox_DrawSelectTarget(wk,wk->ret,TRUE);
			}else{
				sbox_DrawSelectTarget(wk,wk->ret,FALSE);
			}
		}
		--wk->wait;
		return SBOX_SELECT_NULL;
	}

	{
		BOOL touch;
		
		ret = sbox_TouchInput(wk,&touch);

		if(!touch){
			ret = sbox_KeyInput(wk);
		}
		
	}
	if(ret == SBOX_SELECT_CANCEL){
		sbox_DrawOff(wk);
		sbox_BmpWinDelete(wk);
		return ret;
	}
	return SBOX_SELECT_NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG�L�����N�^�f�[�^�]�����N�G�X�g
 *
 *	@param	fileIdx		�A�[�J�C�u�t�@�C���C���f�b�N�X
 *	@param	dataIdx		�f�[�^�C���f�b�N�X
 *	@param	bgl			BG�f�[�^
 *	@param	frm			�ڰуi���o�[
 *	@param	offs		�I�t�Z�b�g
 *	@param	heapID		�q�[�v
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void sbox_BGResTransReq( SELBOX_SYS* sp, SELBOX_HEADER* head,PALETTE_FADE_PTR palASys, u32 heapID )
{
	int i;
	SBOX_BGRES_TRANS* pw;
	SELBOX_HEAD_PRM *prm;

	pw = sys_AllocMemoryLo( heapID, sizeof(SBOX_BGRES_TRANS) );
	MI_CpuClear8(pw,sizeof(SBOX_BGRES_TRANS) );

	pw->p_char = sp->p_char;
	pw->p_pltt = sp->p_pltt;
	
	// �]���p�����[�^�i�[
	prm = &head->prm;
	pw->ini = head->bgl;
	pw->frame = prm->frm;
	pw->char_ofs = prm->fcgx;
	pw->pal_no = prm->pal;

	// �^�X�N�o�^
	VWaitTCB_Add( sbox_vtcb_BGResTrans, pw, 128);

	if(palASys == NULL){
		return;
	}
	//�p���b�g�A�j�����[�N�o�^
	if(prm->frm < GF_BGL_FRAME0_S){
		PaletteWorkSet(palASys,pw->p_pltt->pRawData,FADE_MAIN_BG,prm->pal*16,32);
	}else{
		PaletteWorkSet(palASys,pw->p_pltt->pRawData,FADE_SUB_BG,prm->pal*16,32);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�]���^�X�N
 *
 *	@param	tcb
 *	@param	p_work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void sbox_vtcb_BGResTrans( TCB_PTR tcb, void* p_work )
{
	int i;
	SBOX_BGRES_TRANS* pw = p_work;

	//�L�����N�^�f�[�^�]��
	DC_FlushRange( pw->p_char->pRawData, pw->p_char->szByte );
	GF_BGL_LoadCharacter( pw->ini, pw->frame, 
			pw->p_char->pRawData, pw->p_char->szByte, pw->char_ofs );
	
	//�p���b�g�f�[�^�]��
	DC_FlushRange( pw->p_pltt->pRawData, 32);

	if( pw->frame < GF_BGL_FRAME0_S){
		GX_LoadBGPltt( pw->p_pltt->pRawData, pw->pal_no*32, 32 );
	}else{
		GXS_LoadBGPltt( pw->p_pltt->pRawData, pw->pal_no*32, 32 );
	}

	// ���������
	TCB_Delete( tcb );
	sys_FreeMemoryEz( pw );
}

/**
 *	@brief	���X�g�ɓo�^���ꂽ������̍ő啶��������E�B���h�EX�T�C�Y���擾
 *
 *	@param	list	BMP���X�g
 *	@param	num		���ڐ�
 *	@param	font	�t�H���g�R�[�h
 *	@param	ofs_x	���ڕ\���I�t�Z�b�gX(dot)
 *	@return width ������ő啝�����܂�L�����N�^�T�C�Y��Ԃ�
 */
static u8 sbox_GetBmpListParam(const BMP_MENULIST_DATA* list,u8 num,u8 font,u8 ofs_x)
{
	int i;
	u8	w,width = 0;
	
	for(i = 0;i < num;i++){
		GF_ASSERT_MSG(list[i].str,"SelectBox BmpMenuList[%d] is NULL\n",i);
		GF_ASSERT_MSG(list[i].str != (void*)0xFFFFFFFF,"SelectBox BmpMenuList num is over\n");

		w = FontProc_GetPrintStrWidth(font,list[i].str,0);
		if(w > width){
			width = w;
		}
	}
	width += ofs_x*2;	//���E���[����������

	//�L�����N�^�T�C�Y�ɐ��`
	if((width%8) == 0){
		return (width/8);
	}
	return (width/8)+1;
}

/**
 *	@brief	BmpWin&Hit�e�[�u���f�[�^�쐬
 */
static void sbox_BmpWinCreate(SELBOX_WORK* wk)
{
	int i;
	u16	chr_siz;
	SELBOX_HEAD_PRM * prm;

	prm= &wk->hed.prm;

	wk->win = GF_BGL_BmpWinAllocGet(wk->heapID,wk->hed.count);
	wk->tbl = sys_AllocMemory(wk->heapID,sizeof(RECT_HIT_TBL)*(wk->hed.count+1));
	MI_CpuClear8(wk->tbl,sizeof(RECT_HIT_TBL)*(wk->hed.count+1));	//End�R�[�h�p�̈�����
	chr_siz = wk->width*2;
	
	for(i = 0;i < wk->hed.count;i++){
		GF_BGL_BmpWinAdd(
			wk->hed.bgl, &wk->win[i], prm->frm,
			(wk->px+1), (wk->py+1)+3*i, wk->width, 2, prm->pal,prm->scgx+chr_siz*i);
		GF_BGL_BmpWinDataFill(&(wk->win[i]),FCOL_N_BASE);

		//hit�e�[�u���p�����[�^�Z�b�g(�����΍�Ƃ��āA�e�[�u���̒��g�͐�΍��W�l�Ŏ���)
		wk->tbl[i].rect.top = (wk->py+1)*8+24*i;
		wk->tbl[i].rect.bottom = wk->tbl[i].rect.top+16;
		wk->tbl[i].rect.left = (wk->px+1)*8;
		wk->tbl[i].rect.right = wk->tbl[i].rect.left+(wk->width*8);
	}
	//hit�e�[�u���p�����[�^End�R�[�h����
	wk->tbl[i].rect.top = RECT_HIT_END; 
}

/**
 *	@brief	BmpWin&Hit�e�[�u���f�[�^���
 */
static void sbox_BmpWinDelete(SELBOX_WORK* wk)
{
	sys_FreeMemoryEz(wk->tbl);
	GF_BGL_BmpWinFree(wk->win,wk->hed.count);
}

/**
 *	@brief	�I�����X�g�g�`��
 */
static void sbox_DrawWinFrame(SELBOX_WORK* wk)
{
	int i,cgx,end;
	SELBOX_HEAD_PRM * prm;

	prm= &wk->hed.prm;
	
	end = wk->hed.count-1;

	cgx = prm->fcgx+0;
	if(wk->cp == 0){
		cgx += 12;
	}
	//����p
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+0, wk->px, wk->py, 1, 1,prm->pal);
	//��i�g
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+1, wk->px+1, wk->py, wk->width, 1,prm->pal);
	//�E��p
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+2, wk->px+wk->width+1, wk->py, 1, 1,prm->pal);

	cgx = prm->fcgx+9;
	if(wk->cp == end){
		cgx += 12;
	}
	//�����p
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+0, wk->px, wk->py+wk->hed.count*3, 1, 1,prm->pal);
	//���i�g
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+1, wk->px+1, wk->py+wk->hed.count*3, wk->width, 1,prm->pal);
	//�E���p
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+2, wk->px+wk->width+1, wk->py+wk->hed.count*3, 1, 1,prm->pal);

	//���i�d�؂�g
	for(i = 0;i < wk->hed.count-1;i++){
		cgx = prm->fcgx+6;
		if(	(wk->cp == 0 && i == 0)||
			(wk->cp == i)){
			cgx += 12;
		}else if(	(wk->cp == end && i == (end-1)) ||
					(wk->cp == (i+1))){
			cgx += 18;
		}
		GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+0, wk->px, wk->py+i*3+3, 1, 1,prm->pal);
		GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+1, wk->px+1, wk->py+i*3+3, wk->width, 1,prm->pal);
		GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+2, wk->px+wk->width+1, wk->py+i*3+3, 1, 1,prm->pal);
	}
	//���E�g
	for(i = 0;i < wk->hed.count;i++){
		cgx = prm->fcgx+3;
		if(wk->cp == i){
			cgx += 12;
		}
		GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+0, wk->px, wk->py+i*3+1, 1, 2,prm->pal);
		GF_BGL_ScrFill( wk->hed.bgl,prm->frm, cgx+2, wk->px+wk->width+1, wk->py+i*3+1, 1, 2,prm->pal);
	}
}

/**
 *	@brief	�I�����X�g�`��
 */
static void sbox_DrawList(SELBOX_WORK* wk)
{
	int i;
	u32	siz,win_siz;
	GF_PRINTCOLOR col;
	SELBOX_HEAD_PRM * prm;

	prm= &wk->hed.prm;

	col = FCOL_NORMAL;
	win_siz = wk->width*8;	
	for(i = 0;i < wk->hed.count;i++){
		if(prm->ofs_type == SBOX_OFSTYPE_LEFT){
			siz = prm->ofs_x;
		}else{
			siz = (win_siz-FontProc_GetPrintStrWidth(SBOX_FONT,wk->hed.list[i].str,0))/2;
		}
		GF_STR_PrintColor(	&wk->win[i],SBOX_FONT,
							wk->hed.list[i].str,
							siz,0,MSG_NO_PUT,col,NULL );
		GF_BGL_BmpWinOnVReq(&wk->win[i]);
	}
	GF_BGL_LoadScreenV_Req(wk->hed.bgl,prm->frm);
}

/**
 *	@brief	�I�����ꂽ���ڂ�ʐF�ōĕ`��
 */
static void sbox_DrawSelectTarget(SELBOX_WORK* wk,u8 idx,BOOL pat)
{
	u32	siz,win_siz;
	GF_PRINTCOLOR col;
	SELBOX_HEAD_PRM * prm;
	u8 base;
	
	prm= &wk->hed.prm;

	if(pat){
		col = FCOL_SELECT;
		base = FCOL_S_BASE;
	}else{
		col = FCOL_NORMAL;
		base = FCOL_N_BASE;
	}
	win_siz = wk->width*8;	

	if(prm->ofs_type == SBOX_OFSTYPE_LEFT){
		siz = prm->ofs_x;
	}else{
		siz = (win_siz-FontProc_GetPrintStrWidth(SBOX_FONT,wk->hed.list[idx].str,0))/2;
	}
	GF_BGL_BmpWinDataFill(&(wk->win[idx]),base);
	GF_STR_PrintColor(	&wk->win[idx],SBOX_FONT,
						wk->hed.list[idx].str,
						siz,0,MSG_NO_PUT,col,NULL );
	GF_BGL_BmpWinOnVReq(&wk->win[idx]);
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�`��I�t
 */
static void sbox_DrawOff(SELBOX_WORK* wk)
{
	int i;
	SELBOX_HEAD_PRM * prm;
	
	prm= &wk->hed.prm;

	for(i = 0;i < wk->hed.count;i++){
		GF_BGL_BmpWinOffVReq(&wk->win[i]);
	}
	//�E�B���h�E�g�̈���N���A
	GF_BGL_ScrFill( wk->hed.bgl,prm->frm, 0,wk->px,wk->py,wk->width+2,wk->hed.count*3+1,prm->pal);
	
	GF_BGL_LoadScreenV_Req(wk->hed.bgl,prm->frm);
}

/**
 *	@brief	�I���J�[�\���@�|�W�V�����Z�b�g
 */
static void sbox_SetCursorPos(SELBOX_WORK* wk,u8 pos)
{
	sbox_DrawWinFrame(wk);
	GF_BGL_LoadScreenV_Req(wk->hed.bgl,wk->hed.prm.frm);
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�^�b�`���͌��o
 */
static u32 sbox_TouchInput(SELBOX_WORK* wk,BOOL* flag)
{
	u32 ret;
	
	ret = GF_TP_RectHitTrg(wk->tbl);

	if(ret == RECT_HIT_NONE){
		*flag = FALSE;
		return SBOX_SELECT_NULL;
	}
	*flag = TRUE;

	wk->cp = ret;
	sbox_DrawSelectTarget(wk,ret,TRUE);
	sbox_DrawWinFrame(wk);
	GF_BGL_LoadScreenV_Req(wk->hed.bgl,wk->hed.prm.frm);

	wk->seq = SEQ_ENDWAIT;
	wk->wait = SBOX_ENDWAIT;
	wk->ret = ret;
	wk->key_mode = APP_KTST_TOUCH;

	sbox_SePlay(wk,SBOX_SE_DECIDE);
	sbox_CallBackFuncExe(wk,SBOX_CB_MODE_DECIDE);
	
	//�_�ŃG�t�F�N�g�\���҂�������̂ŁA�����ł�NULL��Ԃ�
	return SBOX_SELECT_NULL;
}

/**
 *	@brief	�Z���N�g�{�b�N�X�@�L�[����
 */
static u32 sbox_KeyInput(SELBOX_WORK* wk)
{
	u32 ret;

	if(sys.trg & PAD_BUTTON_KTST_CHG){
		wk->key_mode = APP_KTST_KEY;
	}

	if(sys.trg & PAD_BUTTON_B){
		sbox_SePlay(wk,SBOX_SE_DECIDE);
		sbox_CallBackFuncExe(wk,SBOX_CB_MODE_CANCEL);
		return SBOX_SELECT_CANCEL;
	}
	if(sys.trg & PAD_BUTTON_A){
		sbox_DrawSelectTarget(wk,wk->cp,TRUE);
		wk->seq = SEQ_ENDWAIT;
		wk->wait = SBOX_ENDWAIT;
		wk->ret = wk->cp;
		//�_�ŃG�t�F�N�g�҂�������̂ŁA�����ł�NULL��Ԃ�
		sbox_SePlay(wk,SBOX_SE_DECIDE);
		sbox_CallBackFuncExe(wk,SBOX_CB_MODE_DECIDE);
		return SBOX_SELECT_NULL;
	}

	if(sys.trg & PAD_KEY_UP){
		if(wk->hed.prm.loop_f){
			wk->cp = (wk->cp+(wk->hed.count-1))%wk->hed.count;
		}else{
			if(wk->cp > 0){
				wk->cp -= 1;
			}
		}
		sbox_SePlay(wk,SBOX_SE_DECIDE);
		sbox_CallBackFuncExe(wk,SBOX_CB_MODE_MOVE);
		sbox_SetCursorPos(wk,wk->cp);
	}
	if(sys.trg & PAD_KEY_DOWN){
		if(wk->hed.prm.loop_f){
			wk->cp = (wk->cp+1)%wk->hed.count;
		}else{
			if(wk->cp < (wk->hed.count-1)){
				wk->cp += 1;
			}
		}
		sbox_SePlay(wk,SBOX_SE_DECIDE);
		sbox_CallBackFuncExe(wk,SBOX_CB_MODE_MOVE);
		sbox_SetCursorPos(wk,wk->cp);
	}
	return SBOX_SELECT_NULL;
}

/*
 *	@brief	SE�Đ�
 */
static void sbox_SePlay(SELBOX_WORK* wk,u16 se_no)
{
	if(wk->se_manual_f){
		return;
	}
	Snd_SePlay(se_no);
}

/**
 *	@brief	�R�[���o�b�N�Ăяo��
 */
static void sbox_CallBackFuncExe(SELBOX_WORK* wk,SBOX_CB_MODE mode)
{
	if(wk->cb_func != NULL){
		(wk->cb_func)(wk,wk->cp,wk->cb_work,mode);
	}
}
