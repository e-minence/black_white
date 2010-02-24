//============================================================================================
/**
 * @file	bgwinfrm.c
 * @brief	�a�f�X�N���[���z�u����
 * @author	Hiroyuki Nakamura
 * @date	08.08.19
 */
//============================================================================================
#include <gflib.h>

#include "system/bgwinfrm.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �t���[���f�[�^
typedef struct {
	u16 * scr;			// �X�N���[���f�[�^
	u16	sx:6;			// �X�N���[���̂w�T�C�Y
	u16	sy:6;			// �X�N���[���̂x�T�C�Y
	u16	vanish:1;		// �\���t���O�@0 = ON, 1 = OFF
	u16	auto_flg:3;		// �����ړ��t���O�@0 = ON, 1 = OFF
	// 2bit
	s8	px;				// �\���w���W
	s8	py;				// �\���x���W

	s8	mvx;			// �P�t���[���̂w�ړ���
	s8	mvy;			// �P�t���[���̂x�ړ���
	u8	frm;			// �a�f�t���[��
	u8	cnt;			// ����J�E���^

	s8	area_lx;		// �`��͈�
	s8	area_rx;
	s8	area_uy;
	s8	area_dy;

}BGWINFRM;

// �t���[�����[�N
struct _BGWINFRM_WORK {
	BGWINFRM * win;		// �t���[���f�[�^
	u16 max;			// �t���[����
	u16	trans_mode:15;	// �]�����[�h
	u16	auto_master:1;	// �����ړ��}�X�^�[�t���O�@0 = ON, 1 = OFF
	u32	heapID;			// �q�[�v�h�c
};

typedef void (*pTransFunc)(u8);		// �X�N���[���]���֐��^


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void LoadScreenDummy( u8 frm );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================

// �]���֐�
static const pTransFunc LoadScreenFunc[] = {
	LoadScreenDummy,
	GFL_BG_LoadScreenReq,
	GFL_BG_LoadScreenV_Req,
};



//--------------------------------------------------------------------------------------------
/**
 * �t���[�����[���쐬
 *
 * @param	bgl		BGL
 * @param	mode	�]�����[�h
 * @param	max		�o�^�ő吔
 * @param	heapID	�q�[�v�h�c
 *
 * @return	�t���[�����[��
 */
//--------------------------------------------------------------------------------------------
BGWINFRM_WORK * BGWINFRM_Create( u32 mode, u32 max, HEAPID heapID )
{
	BGWINFRM_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(BGWINFRM_WORK) );

	wk->max = max;
	wk->heapID = heapID;
	wk->trans_mode = mode;
	wk->auto_master = 0;

	wk->win = GFL_HEAP_AllocClearMemory( heapID, sizeof(BGWINFRM)*max );

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�����[�����
 *
 * @param	wk		�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_Exit( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<wk->max; i++ ){
		if( wk->win[i].scr == NULL ){ continue; }
		GFL_HEAP_FreeMemory( wk->win[i].scr );
	}
	GFL_HEAP_FreeMemory( wk->win );
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���ǉ�
 *
 * @param	wk		�t���[�����[�N
 * @param	index	�C���f�b�N�X
 * @param	frm		�a�f�t���[��
 * @param	sx		�w�T�C�Y
 * @param	sy		�x�T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_Add( BGWINFRM_WORK * wk, u32 index, u32 frm, u32 sx, u32 sy )
{
	BGWINFRM * win;

	win = &wk->win[index];

	win->scr = GFL_HEAP_AllocMemory( wk->heapID, sx * sy * 2 );
	win->sx  = sx;
	win->sy  = sy;

	win->frm = frm;

	win->px  = 0;
	win->py  = 0;

	win->auto_flg = 0;

	win->vanish = BGWINFRM_OFF;

	win->area_lx = 0;
	win->area_rx = 32;
	win->area_uy = 0;
	win->area_dy = 24;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���ɃX�N���[�����Z�b�g
 *
 * @param	wk		�t���[�����[�N
 * @param	index	�t���[���C���f�b�N�X
 * @param	scr		�X�N���[���f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameSet( BGWINFRM_WORK * wk, u32 index, u16 * scr )
{
  BGWINFRM * win;

  GF_ASSERT_MSG( index<wk->max , "index�̎w�肪���������̍ő吔�𒴂��Ă��܂�\n");

  win = &wk->win[index];

  GFL_STD_MemCopy16( scr, win->scr, win->sx*win->sy*2 );
/*
	BGWINFRM * win = &wk->win[index];

	GFL_STD_MemCopy16( scr, win->scr, win->sx*win->sy*2 );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���ɃA�[�J�C�u�X�N���[���f�[�^���Z�b�g
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	fileIdx		�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param	dataIdx		�f�[�^�C���f�b�N�X
 * @param	comp		���k�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameSetArc( BGWINFRM_WORK * wk, u32 index, u32 fileIdx, u32 dataIdx, BOOL comp )
{
	NNSG2dScreenData * scrn;
	void * buf;

	buf = GFL_ARC_UTIL_LoadScreen( fileIdx, dataIdx, comp, &scrn, wk->heapID );

	BGWINFRM_FrameSet( wk, index, (void *)scrn->rawData );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���ɃA�[�J�C�u�X�N���[���f�[�^���Z�b�g
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	ah			�A�[�J�C�u�n���h��
 * @param	dataIdx		�f�[�^�C���f�b�N�X
 * @param	comp		���k�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameSetArcHandle( BGWINFRM_WORK * wk, u32 index, ARCHANDLE * ah, u32 dataIdx, BOOL comp )
{
	NNSG2dScreenData * scrn;
	void * buf;

	buf = GFL_ARCHDL_UTIL_LoadScreen( ah, dataIdx, comp, &scrn, wk->heapID );

	BGWINFRM_FrameSet( wk, index, (void *)scrn->rawData );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�����a�f�ɔz�u�i�ʒu�w��j
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	px			�w���W
 * @param	py			�x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FramePut( BGWINFRM_WORK * wk, u32 index, s8 px, s8 py )
{
	BGWINFRM * win;
	u8	wpx, wpy, wsx, wsy, bpx, bpy;

	win = &wk->win[index];

	win->px = px;
	win->py = py;

	if( px >= win->area_rx ||
		py >= win->area_dy ||
		px + win->sx < win->area_lx ||
		py + win->sy < win->area_uy ){
		return;
	}

	wpx = px;
	wsx = win->sx;
	bpx = 0;

	if( px < win->area_lx ){
		wpx = win->area_lx;
		wsx -= ( win->area_lx - px );
		bpx = win->area_lx - px;
	}
	if( px + win->sx >= win->area_rx ){
		wsx -= ( px + win->sx - win->area_rx );
	}

	wpy = py;
	wsy = win->sy;
	bpy = 0;

	if( py < win->area_uy ){
		wpy = win->area_uy;
		wsy -= ( win->area_uy - py );
		bpy = win->area_uy - py;
	}
	if( py + win->sy >= win->area_dy ){
		wsy -= ( py + win->sy - win->area_dy );
	}

	GFL_BG_WriteScreenExpand(
		win->frm, wpx, wpy, wsx, wsy, win->scr, bpx, bpy, win->sx, win->sy );
	LoadScreenFunc[wk->trans_mode]( win->frm );

	win->vanish = BGWINFRM_ON;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�����a�f�ɔz�u�i�L���ʒu�j
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameOn( BGWINFRM_WORK * wk, u32 index )
{
	BGWINFRM_FramePut( wk, index, wk->win[index].px, wk->win[index].py );
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�����\��
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameOff( BGWINFRM_WORK * wk, u32 index )
{
	BGWINFRM * win;
	s8	px, py, sx, sy;

	win = &wk->win[index];

	win->vanish = BGWINFRM_OFF;

	if( win->px >= win->area_rx ||
		win->py >= win->area_dy ||
		win->px + win->sx < win->area_lx ||
		win->py + win->sy < win->area_uy ){
		return;
	}

	px = win->px;
	sx = win->sx;

	if( win->px < win->area_lx ){
		px = win->area_lx;
		sx -= ( win->area_lx - win->px );
	}
	if( win->px + win->sx >= win->area_rx ){
		sx -= ( win->px + win->sx - win->area_rx );
	}

	py = win->py;
	sy = win->sy;

	if( win->py < win->area_uy ){
		py = win->area_uy;
		sy -= ( win->area_uy - win->py );
	}
	if( win->py + win->sy >= win->area_dy ){
		sy -= ( win->py + win->sy - win->area_dy );
	}

	GFL_BG_FillScreen( win->frm, 0, px, py, sx, sy, GFL_BG_SCRWRT_PALNL );
	LoadScreenFunc[wk->trans_mode]( win->frm );
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ��ݒ�
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	mvx			�w�ړ��ʁi�P�t���[���j
 * @param	mvy			�x�ړ��ʁi�P�t���[���j
 * @param	cnt			�ړ��t���[����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_MoveInit( BGWINFRM_WORK * wk, u32 index, s8 mvx, s8 mvy, u8 cnt )
{
	BGWINFRM * win = &wk->win[index];

	win->mvx = mvx;
	win->mvy = mvy;
	win->cnt = cnt;

	win->auto_flg = 1;

	wk->auto_master = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ����C���i�S�́j
 *
 * @param	wk			�t���[�����[�N
 *
 * @return	none
 *
 * @li	BGWINFRM_MoveOne() �Ɠ����Ɏg�p����� 2move/1frame �ɂȂ�̂Œ���
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_MoveMain( BGWINFRM_WORK * wk )
{
	u32	i;

	if( wk->auto_master == 0 ){ return; }

	wk->auto_master = 0;

	for( i=0; i<wk->max; i++ ){
		if( BGWINFRM_MoveOne( wk, i ) == 1 ){
			wk->auto_master = 1;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ����C���i�ʁj
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @retval	"0 = ��~��"
 * @retval	"1 = �ړ���"
 *
 * @li	BGWINFRM_MoveMain() �Ɠ����Ɏg�p����� 2move/1frame �ɂȂ�̂Œ���
 */
//--------------------------------------------------------------------------------------------
u32 BGWINFRM_MoveOne( BGWINFRM_WORK * wk, u32 index )
{
	if( wk->win[index].auto_flg == 0 ){
		return 0;
	}

//	wk->auto_master = 0;

	BGWINFRM_FrameOff( wk, index );
	wk->win[index].px += wk->win[index].mvx;
	wk->win[index].py += wk->win[index].mvy;
	BGWINFRM_FrameOn( wk, index );
	wk->win[index].cnt--;
//	if( wk->win[index].cnt == 0 || wk->win[index].vanish == BGWINFRM_OFF ){
	if( wk->win[index].cnt == 0 ){
		wk->win[index].auto_flg = 0;
		return 0;
	}

//	wk->auto_master = 1;

	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ��`�F�b�N
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @retval	"0 = ��~��"
 * @retval	"1 = �ړ���"
 */
//--------------------------------------------------------------------------------------------
u32 BGWINFRM_MoveCheck( BGWINFRM_WORK * wk, u32 index )
{
	return wk->win[index].auto_flg;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���\���`�F�b�N
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @retval	"BGWINFRM_ON = �\��"
 * @retval	"BGWINFRM_OFF = ��\��"
 */
//--------------------------------------------------------------------------------------------
u32 BGWINFRM_VanishCheck( BGWINFRM_WORK * wk, u32 index )
{
	return wk->win[index].vanish;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[����\������a�f��ύX
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	frm			�ύX��̂a�f�t���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_BGFrameChange( BGWINFRM_WORK * wk, u32 index, u32 frm )
{
	wk->win[index].frm = frm;
}

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�̃p���b�g��ύX
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	px			�w���W
 * @param	py			�x���W
 * @param	sx			�w�T�C�Y
 * @param	sy			�x�T�C�Y
 * @param	pal			�p���b�g
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_PaletteChange( BGWINFRM_WORK * wk, u32 index, u8 px, u8 py, u8 sx, u8 sy, u8 pal )
{
	u16 * scr;
	u32	scr_sx;
	u16	i, j;

	scr    = wk->win[index].scr;
	scr_sx = wk->win[index].sx;

	for( i=py; i<py+sy; i++ ){
		for( j=px; j<px+sx; j++ ){
			scr[ i * scr_sx + j ] = ( scr[ i * scr_sx + j ] & 0xfff ) | ( pal << 12 );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN����������
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	win			BMPWIN
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_BmpWinOn( BGWINFRM_WORK * wk, u32 index, GFL_BMPWIN * win )
{
	u16 * scr;
	u32	sx;
	u16	i, j;
	u16	chr, pal;
	u8	bw_sx, bw_sy, bw_px, bw_py;

	scr = wk->win[index].scr;
	sx  = wk->win[index].sx;

	chr = GFL_BMPWIN_GetChrNum( win );
	pal = ( (u16)GFL_BMPWIN_GetPalette( win ) << 12 );

	bw_sx = GFL_BMPWIN_GetSizeX( win );
	bw_sy = GFL_BMPWIN_GetSizeY( win );
	bw_px = GFL_BMPWIN_GetPosX( win );
	bw_py = GFL_BMPWIN_GetPosY( win );

	for( i=0; i<bw_sy; i++ ){
		if( ( bw_py + i ) >= wk->win[index].sy ){
			break;
		}
		for( j=0; j<bw_sx; j++ ){
			if( ( bw_px + j ) >= wk->win[index].sx ){
				break;
			}
			scr[ ( bw_py + i ) * sx + bw_px + j ] = chr + j + pal;
		}
		chr += bw_sx;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���̃X�N���[�����擾
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @return	�X�N���[���̃|�C���^
 */
//--------------------------------------------------------------------------------------------
u16 * BGWINFRM_FrameBufGet( BGWINFRM_WORK * wk, u32 index )
{
	return wk->win[index].scr;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���̕\���a�f�t���[�����擾
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @return	�a�f�t���[��
 */
//--------------------------------------------------------------------------------------------
u8 BGWINFRM_BGFrameGet( BGWINFRM_WORK * wk, u32 index )
{
	return wk->win[index].frm;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���̕\���ʒu���擾
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	px			�w���W�i�[�ꏊ
 * @param	py			�x���W�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_PosGet( BGWINFRM_WORK * wk, u32 index, s8 * px, s8 * py )
{
	if( px != NULL ){ *px = wk->win[index].px; }
	if( py != NULL ){ *py = wk->win[index].py; }
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���̃T�C�Y���擾
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	sx			�w�T�C�Y�i�[�ꏊ
 * @param	sy			�x�T�C�Y�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_SizeGet( BGWINFRM_WORK * wk, u32 index, u16 * sx, u16 * sy )
{
	if( sx != NULL ){ *sx = wk->win[index].sx; }
	if( sy != NULL ){ *sy = wk->win[index].sy; }
}

//--------------------------------------------------------------------------------------------
/**
 * �\���G���A�ݒ�
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	lx			���w���W
 * @param	rx			�E�w���W
 * @param	uy			��x���W
 * @param	dy			���x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_PutAreaSet( BGWINFRM_WORK * wk, u32 index, s8 lx, s8 rx, s8 uy, s8 dy )
{
	wk->win[index].area_lx = lx;
	wk->win[index].area_rx = rx;
	wk->win[index].area_uy = uy;
	wk->win[index].area_dy = dy;
}

//--------------------------------------------------------------------------------------------
/**
 * �_�~�[�]���֐�
 *
 * @param	frm		�a�f�t���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadScreenDummy( u8 frm )
{
}
