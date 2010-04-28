//============================================================================================
/**
 * @file  palanm.c
 * @brief �p���b�g�t�F�[�h����
 * @author  Hiroyuki Nakamura
 * @date  2004.11.24
 */
//============================================================================================
#define PALANM_H_GLOBAL

#include <gflib.h>
#include "system/palanm.h"


//============================================================================================
//  �萔��`
//============================================================================================
#define DEF_FADE_VAL  ( 2 )     // ��{���x

///�t�F�[�h�v�Z���s�����[�J���^�X�N��TCB�v���C�I���e�B
/// ���K��TCB�̍Ō�Ɏ��s�����悤�Ƀv���C�I���e�B�����ɂ��Ă���
#define TCBPRI_CALC_FADETASK    (0xffffffff - 1)

#define NORMAL_COLOR_NUM  ( 16 )    // �ʏ�p���b�g�̃J���[��
#define EXTRA_COLOR_NUM   ( 256 )   // �g���p���b�g�̃J���[��


//==============================================================================
//  �\���̒�`
//==============================================================================
/// �p���b�g�t�F�[�h�f�[�^
typedef struct _PALETTE_FADE_DATA{
  FADE_REQ_EX2  dat[ ALL_PALETTE_SIZE ];

  u16 req_flg:2;    // 0=���o�^, 1=���쒆, 2=�ꎞ��~
  u16 req_bit:14;   // ���N�G�X�g�r�b�g�i�J���[�v�Z�p�j
  u16 trans_bit:14; // �g�����X�r�b�g�i�J���[�]���p�j
  u16 tcb_flg:1;    // TCB�o�^�t���O�@0=���o�^, 1=�o�^��
  u16 auto_trans:1;     // �g�����X�r�b�g�Ɋ֌W�Ȃ��A���t���[���S�p���b�g��]��

  u8 force_stop;    //TRUE:�p���b�g�t�F�[�h�����I��

  u8 dummy[3];
}PALETTE_FADE_DATA;

/// �J���[�p���b�g�\����
typedef struct {
    u16 Red:5;              // ��
    u16 Green:5;            // ��
    u16 Blue:5;             // ��
    u16 Dummy_15:1;
} PLTT_DATA;

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static u8 ReqBitCheck( u16 flg, u16 req );
static void TransBitSet( PALETTE_FADE_PTR pfd, u16 id );
static void FadeBitCheck( FADEREQ req, FADE_REQ_EX2 * wk, u16 * fade_bit );
static void FadeReqSet(
        FADE_REQ_PARAM * wk, u16 fade_bit,
        s8 wait, u8 start_evy, u8 end_evy, u16 next_rgb );
static void PaletteFadeTask( GFL_TCB *tcb, void * work );
static void NormalPaletteFade(PALETTE_FADE_PTR pfd);
static void ExtraPaletteFade(PALETTE_FADE_PTR pfd);
static void PaletteFade( PALETTE_FADE_PTR pfd, u16 id, u16 siz );
static void PaletteFadeMain( PALETTE_FADE_PTR pfd, u16 id, u16 siz );
static void FadeWorkSet( u16 * def, u16 * trans, FADE_REQ_PARAM * wk, u32 siz );
static void FadeParamCheck(PALETTE_FADE_PTR pfd, u8 id, FADE_REQ_PARAM * wk );



//--------------------------------------------------------------
/**
 * @brief   �p���b�g�t�F�[�h�V�X�e�����[�N�쐬
 *
 * @param   heap_id   �q�[�vID
 *
 * @retval  �쐬�����p���b�g�t�F�[�h�V�X�e�����[�N�̃|�C���^
 *
 * �V�X�e�����g��Ȃ��Ȃ�����PaletteFadeFree�ŉ���������s���Ă�������
 */
//--------------------------------------------------------------
PALETTE_FADE_PTR PaletteFadeInit(int heap_id)
{
  PALETTE_FADE_PTR pfd;

  pfd = GFL_HEAP_AllocMemory(heap_id, sizeof(PALETTE_FADE_DATA));
  MI_CpuClear8(pfd, sizeof(PALETTE_FADE_DATA));
  return pfd;
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g�t�F�[�h�V�X�e�����
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void PaletteFadeFree(PALETTE_FADE_PTR pfd)
{
  GFL_HEAP_FreeMemory(pfd);
}

//--------------------------------------------------------------------------------------------
/**
 * ���N�G�X�g�f�[�^�Z�b�g
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param req     ���N�G�X�g�f�[�^�ԍ�
 * @param def_wk    �����f�[�^
 * @param trans_wk  �]���p�f�[�^
 * @param siz     �T�C�Y�i�o�C�g�P�ʁj
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void PaletteFadeWorkAdrSet(PALETTE_FADE_PTR pfd, FADEREQ req, void * def_wk, void * trans_wk, u32 siz )
{
  pfd->dat[req].def_wk   = (u16 *)def_wk;
  pfd->dat[req].trans_wk = (u16 *)trans_wk;
  pfd->dat[req].siz      = siz;
}

//--------------------------------------------------------------------------------------------
/**
 * ���N�G�X�g�f�[�^��malloc���ăZ�b�g
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param req   ���N�G�X�g�f�[�^�ԍ�
 * @param siz   �f�[�^�T�C�Y�i�o�C�g�P�ʁj
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void PaletteFadeWorkAllocSet(PALETTE_FADE_PTR pfd, FADEREQ req, u32 siz, HEAPID heap )
{
  void * def_wk;
  void * trans_wk;

  def_wk   = GFL_HEAP_AllocMemory( heap, siz );
  trans_wk = GFL_HEAP_AllocMemory( heap, siz );
/*
#ifdef  OSP_ERR_PALANMWK_GET  // �p���b�g�t�F�[�h�̃��[�N�擾���s
  if( def_wk == NULL ){
    OS_Printf( "ERROR : PaletteFadeWorkAlloc ( %d ) - def_wk\n", req );
  }
  if( trans_wk == NULL ){
    OS_Printf( "ERROR : PaletteFadeWorkAlloc ( %d ) - trans_wk\n", req );
  }
#endif  // OSP_ERR_PALANMWK_GET
*/
  PaletteFadeWorkAdrSet(pfd, req, def_wk, trans_wk, siz );
}

//--------------------------------------------------------------------------------------------
/**
 * ���N�G�X�g�f�[�^�J��
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param req   ���N�G�X�g�f�[�^�ԍ�
 *
 * @return  none
 *
 * @li  PaletteFadeWorkAllocSet()�Ŏ擾�����ꍇ�Ɏg�p
 */
//--------------------------------------------------------------------------------------------
void PaletteFadeWorkAllocFree(PALETTE_FADE_PTR pfd, FADEREQ req)
{
  GFL_HEAP_FreeMemory(pfd->dat[req].def_wk );
  GFL_HEAP_FreeMemory(pfd->dat[req].trans_wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�̏����f�[�^���Z�b�g
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param dat   �p���b�g�f�[�^
 * @param req   ���N�G�X�g�f�[�^�ԍ�
 * @param pos   �J�n�ʒu
 * @param siz   �f�[�^�T�C�Y
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void PaletteWorkSet(PALETTE_FADE_PTR pfd, const void * dat, FADEREQ req, u16 pos, u16 siz )
{
/*
#ifdef  OSP_ERR_PALANM_SIZ_OVER   // �T�C�Y�I�[�o�[
  if( (pos*2+siz) > pfd->dat[req].siz ){
    OS_Printf( "ERROR : PaletteWorkSet - SizeOver ( %d )\n", req );
  }
#endif  // OSP_ERR_PALANM_SIZ_OVER
*/
  MI_CpuCopy16( dat, (void *)&pfd->dat[req].def_wk[pos], (u32)siz );
  MI_CpuCopy16( dat, (void *)&pfd->dat[req].trans_wk[pos], (u32)siz );
}

//--------------------------------------------------------------
/**
 * @brief   �A�[�J�C�u����Ă���p���b�g�f�[�^�����[�h���ă��[�N�ɓW�J���܂�(�g����)
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   fileIdx     �A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx     �A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heap      �f�[�^�ǂݍ��݃e���|�����Ƃ��Ďg���q�[�vID
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 * @param   pos       �p���b�g�]���J�n�ʒu(�J���[�P��)
 * @param   read_pos    ���[�h�����p���b�g�̓ǂݍ��݊J�n�ʒu(�J���[�P��)
 */
//--------------------------------------------------------------
void PaletteWorkSetEx_Arc(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap,
  FADEREQ req, u32 trans_size, u16 pos, u16 read_pos)
{
  NNSG2dPaletteData *pal_data;
  void *arc_data;

  arc_data = GFL_ARC_UTIL_LoadPalette(fileIdx, dataIdx, &pal_data, GFL_HEAP_LOWID(heap));
  GF_ASSERT(arc_data != NULL);

  if(trans_size == 0){
    trans_size = pal_data->szByte;
  }

  GF_ASSERT(pos * sizeof(pos) + trans_size <= pfd->dat[req].siz);
  PaletteWorkSet(pfd, &(((u16*)(pal_data->pRawData))[read_pos]), req, pos, trans_size);

  GFL_HEAP_FreeMemory(arc_data);
}

//--------------------------------------------------------------
/**
 * @brief   �A�[�J�C�u����Ă���p���b�g�f�[�^�����[�h���ă��[�N�ɓW�J���܂�
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   fileIdx     �A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx     �A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heap      �f�[�^�ǂݍ��݃e���|�����Ƃ��Ďg���q�[�vID
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 * @param   pos       �p���b�g�]���J�n�ʒu(�J���[�P��)
 */
//--------------------------------------------------------------
void PaletteWorkSet_Arc(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap,
  FADEREQ req, u32 trans_size, u16 pos)
{
  PaletteWorkSetEx_Arc(pfd, fileIdx, dataIdx, heap, req, trans_size, pos, 0);
}

//--------------------------------------------------------------
/**
 * @brief   �A�[�J�C�u����Ă���p���b�g�f�[�^�����[�h���ă��[�N�ɓW�J���܂�(�n���h���g����)
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   fileIdx     �A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx     �A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heap      �f�[�^�ǂݍ��݃e���|�����Ƃ��Ďg���q�[�vID
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 * @param   pos       �p���b�g�]���J�n�ʒu(�J���[�P��)
 * @param   read_pos    ���[�h�����p���b�g�̓ǂݍ��݊J�n�ʒu(�J���[�P��)
 */
//--------------------------------------------------------------
void PaletteWorkSetEx_ArcHandle(PALETTE_FADE_PTR pfd, ARCHANDLE* handle, ARCDATID dataIdx, HEAPID heap,
  FADEREQ req, u32 trans_size, u16 pos, u16 read_pos)
{
  NNSG2dPaletteData *pal_data;
  void *arc_data;

  arc_data = GFL_ARCHDL_UTIL_LoadPalette( handle, dataIdx, &pal_data, GFL_HEAP_LOWID(heap) );
  GF_ASSERT(arc_data != NULL);

  if(trans_size == 0){
    trans_size = pal_data->szByte;
  }

  GF_ASSERT(pos * sizeof(pos) + trans_size <= pfd->dat[req].siz);
  PaletteWorkSet(pfd, &(((u16*)(pal_data->pRawData))[read_pos]), req, pos, trans_size);

  GFL_HEAP_FreeMemory(arc_data);
}

//--------------------------------------------------------------
/**
 * @brief   �A�[�J�C�u����Ă���p���b�g�f�[�^�����[�h���ă��[�N�ɓW�J���܂��i�n���h���Łj
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   fileIdx     �A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx     �A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heap      �f�[�^�ǂݍ��݃e���|�����Ƃ��Ďg���q�[�vID
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 * @param   pos       �p���b�g�]���J�n�ʒu(�J���[�P��)
 */
//--------------------------------------------------------------
void PaletteWorkSet_ArcHandle(PALETTE_FADE_PTR pfd, ARCHANDLE* handle, ARCDATID dataIdx, HEAPID heap,
  FADEREQ req, u32 trans_size, u16 pos)
{
  PaletteWorkSetEx_ArcHandle(pfd, handle, dataIdx, heap, req, trans_size, pos, 0);
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�gVRAM���烏�[�N�փf�[�^�R�s�[�����܂�
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   pos       �p���b�g�]���J�n�ʒu(�J���[�P��)
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 *
 * �g���p���b�g�ɂ͖��Ή��ł�
 */
//--------------------------------------------------------------
void PaletteWorkSet_VramCopy(PALETTE_FADE_PTR pfd, FADEREQ req, u16 pos, u32 trans_size)
{
  u16 *pltt_vram;

  GF_ASSERT(pos * sizeof(pos) + trans_size <= pfd->dat[req].siz);

  switch(req){
  case FADE_MAIN_BG:
    pltt_vram = (u16 *)HW_BG_PLTT;
    break;
  case FADE_SUB_BG:
    pltt_vram = (u16 *)HW_DB_BG_PLTT;
    break;
  case FADE_MAIN_OBJ:
    pltt_vram = (u16 *)HW_OBJ_PLTT;
    break;
  case FADE_SUB_OBJ:
    pltt_vram = (u16 *)HW_DB_OBJ_PLTT;
    break;
  default:
    GF_ASSERT(0 && "���Ή��̃��N�G�X�g�ł�");
    return;
  }

  PaletteWorkSet(pfd, &pltt_vram[pos], req, pos, trans_size);
}

//--------------------------------------------------------------
/**
 * @brief   �g���p���b�gVRAM���烏�[�N�փf�[�^�R�s�[�����܂�
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 * @param   pos       �p���b�g�]���J�n�ʒu(�J���[�P��)
 *
 * ���C���p���b�g��PaletteWorkSet_VramCopy���g�p���Ă�������
 */
//--------------------------------------------------------------
void PaletteWorkSet_VramCopyEx(PALETTE_FADE_PTR pfd, FADEREQ req, u32 trans_size, u16 pos)
{
  GF_ASSERT(0 && "���ݖ��쐬�ł�");
}

//--------------------------------------------------------------
/**
 * @brief   �A�[�J�C�u����Ă���p���b�g�f�[�^���w�肵�����[�N�Ƀ��[�h���܂�(�g����)
 *
 * @param   fileIdx     �A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx     �A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heap      �f�[�^�ǂݍ��݃e���|�����Ƃ��Ďg���q�[�vID
 * @param   trans_size    �]���T�C�Y(�o�C�g�P�ʁ@��2�o�C�g�A���C�����g����Ă��邱��)
 * @param   read_pos    ���[�h�����p���b�g�̓ǂݍ��݊J�n�ʒu(�J���[�P��)
 * @param   dest      ���[�h�����p���b�g�̓W�J��ւ̃|�C���^
 */
//--------------------------------------------------------------
void PaletteWorkSetEx_ArcWork(u32 fileIdx, u32 dataIdx, HEAPID heap, u32 trans_size,
  u16 read_pos, void *dest)
{
  NNSG2dPaletteData *pal_data;
  void *arc_data;

  arc_data = GFL_ARC_UTIL_LoadPalette(fileIdx, dataIdx, &pal_data, heap);
  GF_ASSERT(arc_data != NULL);

  if(trans_size == 0){
    trans_size = pal_data->szByte;
  }

  MI_CpuCopy16(&(((u16*)(pal_data->pRawData))[read_pos]), dest, trans_size);

  GFL_HEAP_FreeMemory(arc_data);
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g���[�N�ԃR�s�[
 *
 * @param   pfd       �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param src_req     �R�s�[�����N�G�X�g�f�[�^�ԍ�
 * @param src_pos     �R�s�[���J�n�ʒu(�J���[�P��)
 * @param dest_req    �R�s�[�惊�N�G�X�g�f�[�^�ԍ�
 * @param dest_pos    �R�s�[��J�n�ʒu(�J���[�P��)
 * @param siz       �R�s�[����f�[�^�T�C�Y(�o�C�g�P��)
 *
 * �R�s�[���͌��f�[�^����ǂݎ��A�R�s�[��͌��f�[�^�A�]����A�̗����ɃR�s�[���s���܂�
 */
//--------------------------------------------------------------------------------------------
void PaletteWorkCopy(PALETTE_FADE_PTR pfd, FADEREQ src_req, u16 src_pos,
  FADEREQ dest_req, u16 dest_pos, u16 siz )
{
  MI_CpuCopy16((void *)&pfd->dat[src_req].def_wk[src_pos],
    (void *)&pfd->dat[dest_req].def_wk[dest_pos], siz);
  MI_CpuCopy16((void *)&pfd->dat[src_req].def_wk[src_pos],
    (void *)&pfd->dat[dest_req].trans_wk[dest_pos], siz);
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g���[�N�ɃZ�b�g����Ă���p���b�g�f�[�^���擾
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param tpye  �擾����p���b�g�̎��
 *
 * @return  �Z�b�g����Ă���p���b�g�f�[�^
 */
//--------------------------------------------------------------------------------------------
u16 * PaletteWorkDefaultWorkGet( PALETTE_FADE_PTR pfd, FADEREQ type )
{
  return pfd->dat[type].def_wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g���[�N�ɃZ�b�g����Ă���]���p�p���b�g�f�[�^���擾
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param tpye  �擾����p���b�g�̎��
 *
 * @return  �Z�b�g����Ă���p���b�g�f�[�^
 */
//--------------------------------------------------------------------------------------------
u16 * PaletteWorkTransWorkGet( PALETTE_FADE_PTR pfd, FADEREQ type )
{
  return pfd->dat[type].trans_wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h���N�G�X�g�i�㏑���s�j
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param req_bit   �p���b�g�̎�ށi�r�b�g�w��j
 * @param fade_bit  �p���b�g�w��i�r�b�g�w��jbit0 = pal0
 * @param wait    �v�Z�҂����ԁi�}�C�i�X����j
 * @param start_evy �����Z�x
 * @param end_evy   �ŏI�Z�x
 * @param next_rgb  �ύX��̐F
 *
 * @retval  "0 = ����"
 * @retval  "1 = �G���["
 *
 * @li  �Z�x�F0 �` 16 ( 0(���̐F) �` 2,3..(���ԐF) �` 16(�w�肵���F) )
 */
//--------------------------------------------------------------------------------------------
u8 PaletteFadeReq(PALETTE_FADE_PTR pfd, u16 req_bit, u16 fade_bit, s8 wait,
  u8 start_evy, u8 end_evy, u16 next_rgb, GFL_TCBSYS *tcbsys )
{
  u16 cpy_bit;
  u8  tmp;
  u8  i;

  cpy_bit = fade_bit;
  tmp = 0;
  for( i=0; i<ALL_PALETTE_SIZE; i++ ){
    if( ReqBitCheck( req_bit, i ) == TRUE &&
      ReqBitCheck( pfd->req_bit, i ) == FALSE ){

      FadeBitCheck( i, &pfd->dat[i], &fade_bit );
      FadeReqSet( &pfd->dat[i].prm, fade_bit, wait, start_evy, end_evy, next_rgb );

      TransBitSet( pfd, i );
      if( i >= FADE_MAIN_BG_EX0 ){
        PaletteFadeMain( pfd, i, EXTRA_COLOR_NUM );
      }else{
        PaletteFadeMain( pfd, i, NORMAL_COLOR_NUM );
      }

      fade_bit = cpy_bit;
      tmp = 1;
    }
  }

  if( tmp == 1 ){
    pfd->req_bit |= req_bit;
    if( pfd->tcb_flg == 0 ){
      pfd->tcb_flg = 1;
      pfd->req_flg = 1;
      pfd->force_stop = FALSE;
      GFL_TCB_AddTask( tcbsys, PaletteFadeTask, pfd, TCBPRI_CALC_FADETASK );
    }
  }

  return tmp;
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h���N�G�X�g�i�㏑���j
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param req_bit   �p���b�g�̎�ށi�r�b�g�w��j
 * @param fade_bit  �p���b�g�w��i�r�b�g�w��jbit0 = pal0
 * @param wait    �v�Z�҂����ԁi�}�C�i�X����j
 * @param start_evy �����Z�x
 * @param end_evy   �ŏI�Z�x
 * @param next_rgb  �ύX��̐F
 *
 * @retval  "0 = ����"
 * @retval  "1 = �G���["
 *
 * @li  �Z�x�F0 �` 16 ( 0(���̐F) �` 2,3..(���ԐF) �` 16(�w�肵���F) )
 */
//--------------------------------------------------------------------------------------------
u8 PaletteFadeReqWrite(PALETTE_FADE_PTR pfd, u16 req_bit, u16 fade_bit, s8 wait,
  u8 start_evy, u8 end_evy, u16 next_rgb, GFL_TCBSYS *tcbsys )
{
  u16 cpy_bit;
  u8  tmp;
  u8  i;

  cpy_bit = fade_bit;
  tmp = 0;
  for( i=0; i<ALL_PALETTE_SIZE; i++ ){
    if( ReqBitCheck( req_bit, i ) == TRUE ){
      FadeBitCheck( i, &pfd->dat[i], &fade_bit );
      FadeReqSet( &pfd->dat[i].prm, fade_bit, wait, start_evy, end_evy, next_rgb );

      TransBitSet( pfd, i );
      if( i >= FADE_MAIN_BG_EX0 ){
        PaletteFadeMain( pfd, i, EXTRA_COLOR_NUM );
      }else{
        PaletteFadeMain( pfd, i, NORMAL_COLOR_NUM );
      }

      fade_bit = cpy_bit;
      tmp = 1;
    }
  }

  if( tmp == 1 ){
//    pfd->req_bit |= req_bit;
    pfd->req_bit = req_bit;
    if( pfd->tcb_flg == 0 ){
      pfd->tcb_flg = 1;
      pfd->req_flg = 1;
      pfd->force_stop = FALSE;
      GFL_TCB_AddTask( tcbsys, PaletteFadeTask, pfd, TCBPRI_CALC_FADETASK );
    }
  }

  return tmp;
}

//--------------------------------------------------------------------------------------------
/**
 * ���N�G�X�g�̏�Ԃ𒲂ׂ�
 *
 * @param flg
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static u8 ReqBitCheck( u16 flg, u16 req )
{
  if( ( flg & ( 1 << req ) ) != 0 ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �]���t���O�Z�b�g
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param id    �p���b�gID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void TransBitSet( PALETTE_FADE_PTR pfd, u16 id )
{
  if( ReqBitCheck( pfd->trans_bit, id ) == TRUE ){
    return;
  }
  pfd->trans_bit |= ( 1 << id );
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�r�b�g�`�F�b�N
 *
 * @param req     ���N�G�X�g�f�[�^�ԍ�
 * @param wk      ���N�G�X�g�f�[�^
 * @param fade_bit  �p���b�g�w��i�r�b�g�w��jbit0 = pal0
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void FadeBitCheck( FADEREQ req, FADE_REQ_EX2 * wk, u16 * fade_bit )
{
  u16 siz_bit;
  u8  i, j;

  if( req < NORMAL_PALETTE_SIZE ){
    j = wk->siz / 32;
  }else{
    j = wk->siz / 512;
  }

  siz_bit = 0;
  for( i=0; i<j; i++ ){
    siz_bit += ( 1 << i );
  }

  *fade_bit &= siz_bit;
}


//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�Z�b�g
 *
 * @param wk      �f�[�^
 * @param fade_bit  �p���b�g�w��i�r�b�g�w��jbit0 = pal0
 * @param wait    �v�Z�҂����ԁi�}�C�i�X����j
 * @param start_evy �����Z�x
 * @param end_evy   �ŏI�Z�x
 * @param next_rgb  �ύX��̐F
 *
 * @return  none
 *
 * @li  �Z�x�F0 �` 16 ( 0(���̐F) �` 2,3..(���ԐF) �` 16(�w�肵���F) )
 */
//--------------------------------------------------------------------------------------------
static void FadeReqSet(
        FADE_REQ_PARAM * wk, u16 fade_bit,
        s8 wait, u8 start_evy, u8 end_evy, u16 next_rgb )
{
  // wait��ϲŽ�̎���value��傫������̪��ނ𑁂�����
  if( wait < 0 ){
		u8	val = DEF_FADE_VAL + MATH_ABS( wait );
		if( val > 16 ){
			val = 16;
		}
    wk->fade_value = val;
    wk->wait = 0;
  }else{
    wk->fade_value = DEF_FADE_VAL;
    wk->wait = wait;
  }

  wk->fade_bit = fade_bit;
  wk->now_evy  = start_evy;
  wk->end_evy  = end_evy;
  wk->next_rgb = next_rgb;
  wk->wait_cnt = wk->wait;

  if( start_evy < end_evy ){
    wk->direction = 0;
  }else{
    wk->direction = 1;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h���C���^�X�N
 *
 * @param tcb   TCB�̃|�C���^
 * @param work  TCB���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PaletteFadeTask( GFL_TCB *tcb, void * work )
{
  PALETTE_FADE_PTR pfd = work;

  if(pfd->force_stop == TRUE){
    pfd->force_stop = FALSE;
    pfd->trans_bit = 0;
    pfd->req_bit = 0;
    pfd->tcb_flg = 0;
    GFL_TCB_DeleteTask( tcb );
    return;
  }

  if( pfd->req_flg != 1 ){ return; }

  pfd->trans_bit = pfd->req_bit;

  NormalPaletteFade(pfd);
  ExtraPaletteFade(pfd);

  if( pfd->req_bit == 0 ){
    pfd->tcb_flg = 0;
    GFL_TCB_DeleteTask( tcb );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h������~����
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void PaletteFadeForceStop(PALETTE_FADE_PTR pfd)
{
  if(pfd->req_bit == 0){
    return;
  }

  pfd->force_stop = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �ʏ�p���b�g�̃t�F�[�h�v�Z
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void NormalPaletteFade(PALETTE_FADE_PTR pfd)
{
  u8  i, j;

  for( i=0; i<NORMAL_PALETTE_SIZE; i++ ){
    PaletteFade( pfd, i, NORMAL_COLOR_NUM );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �g���p���b�g�̃t�F�[�h�v�Z
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ExtraPaletteFade(PALETTE_FADE_PTR pfd)
{
  u8  i, j;

  for( i=NORMAL_PALETTE_SIZE; i<ALL_PALETTE_SIZE; i++ ){
    PaletteFade( pfd, i, EXTRA_COLOR_NUM );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�̃t�F�[�h�v�Z
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param id    �p���b�gID
 * @param siz   �J���[�� ( �ʏ� = 16, �g�� = 256 )
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PaletteFade( PALETTE_FADE_PTR pfd, u16 id, u16 siz )
{
  if( ReqBitCheck( pfd->req_bit, id ) == FALSE ){
    return;
  }

  if( pfd->dat[id].prm.wait_cnt < pfd->dat[id].prm.wait ){
    pfd->dat[id].prm.wait_cnt++;
    return;
  }else{
    pfd->dat[id].prm.wait_cnt = 0;
  }

  PaletteFadeMain( pfd, id, siz );
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�̃t�F�[�h�v�Z���C��
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param id    �p���b�gID
 * @param siz   �J���[�� ( �ʏ� = 16, �g�� = 256 )
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PaletteFadeMain( PALETTE_FADE_PTR pfd, u16 id, u16 siz )
{
  u32 i;

  for( i=0; i<16; i++ ){
    if( ReqBitCheck( pfd->dat[id].prm.fade_bit, i ) == FALSE ){
      continue;
    }
    FadeWorkSet(
      &pfd->dat[id].def_wk[i*siz],
      &pfd->dat[id].trans_wk[i*siz],
      &pfd->dat[id].prm, siz );
  }
  FadeParamCheck( pfd, id, &pfd->dat[id].prm );
}


//--------------------------------------------------------------------------------------------
/**
 * �J���[�v�Z
 *
 * @param def   ���p���b�g�f�[�^
 * @param trans �v�Z��̃p���b�g�f�[�^
 * @param wk    �t�F�[�h�p�����[�^
 * @param siz   �v�Z�T�C�Y
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void FadeWorkSet( u16 * def, u16 * trans, FADE_REQ_PARAM * wk, u32 siz )
{
  u32 i;
  u8  red, green, blue;

  for( i=0; i<siz; i++ ){
    red   =
      FADE_CHANGE( (def[i]&0x1f), (wk->next_rgb&0x1f), wk->now_evy );
    green =
      FADE_CHANGE( ((def[i]>>5)&0x1f), ((wk->next_rgb>>5)&0x1f), wk->now_evy );
    blue  =
      FADE_CHANGE( ((def[i]>>10)&0x1f), ((wk->next_rgb>>10)&0x1f), wk->now_evy );

    trans[i] = ( blue << 10 ) | ( green << 5 ) | red;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�̐i�s�󋵂��Ď�
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param id    �t�F�[�h�f�[�^�ԍ�
 * @param wk    �t�F�[�h�p�����[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void FadeParamCheck(PALETTE_FADE_PTR pfd, u8 id, FADE_REQ_PARAM * wk )
{
  s16 tmp;

  if( wk->now_evy == wk->end_evy ){
    if( ( pfd->req_bit & ( 1 << id ) ) != 0 ){
      pfd->req_bit ^= ( 1 << id );
    }
  }else if( wk->direction == 0 ){
    tmp = wk->now_evy;
    tmp += wk->fade_value;
    if( tmp > wk->end_evy ){
      tmp = wk->end_evy;
    }
    wk->now_evy = tmp;
  }else{
    tmp = wk->now_evy;
    tmp -= wk->fade_value;
    if( tmp < wk->end_evy ){
      tmp = wk->end_evy;
    }
    wk->now_evy = tmp;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�]��
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 *
 * @return  none
 *
 * @li  VBlank���ŌĂԂ���
 */
//--------------------------------------------------------------------------------------------
void PaletteFadeTrans(PALETTE_FADE_PTR pfd)
{
  if( pfd->auto_trans == FALSE && pfd->req_flg != 1 ){ return; }

  {
    int i;

    for( i=0; i<ALL_PALETTE_SIZE; i++ ){
      if(pfd->auto_trans == FALSE){
        if(pfd->dat[i].trans_wk == NULL || ReqBitCheck( pfd->trans_bit, i ) == FALSE){
          continue;
        }
      }

      DC_FlushRange( (void*)pfd->dat[i].trans_wk, pfd->dat[i].siz );
      switch( i ){
      case FADE_MAIN_BG:
        GX_LoadBGPltt(
          (const void *)pfd->dat[i].trans_wk, 0, pfd->dat[i].siz );
        break;
      case FADE_SUB_BG:
        GXS_LoadBGPltt(
          (const void *)pfd->dat[i].trans_wk, 0, pfd->dat[i].siz );
        break;
      case FADE_MAIN_OBJ:
        GX_LoadOBJPltt(
          (const void *)pfd->dat[i].trans_wk, 0, pfd->dat[i].siz );
        break;
      case FADE_SUB_OBJ:
        GXS_LoadOBJPltt(
          (const void *)pfd->dat[i].trans_wk, 0, pfd->dat[i].siz );
        break;
      case FADE_MAIN_BG_EX0:
        GX_BeginLoadBGExtPltt();
        GX_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT0_ADRS, pfd->dat[i].siz );
        GX_EndLoadBGExtPltt();
        break;
      case FADE_MAIN_BG_EX1:
        GX_BeginLoadBGExtPltt();
        GX_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT1_ADRS, pfd->dat[i].siz );
        GX_EndLoadBGExtPltt();
        break;
      case FADE_MAIN_BG_EX2:
        GX_BeginLoadBGExtPltt();
        GX_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT2_ADRS, pfd->dat[i].siz );
        GX_EndLoadBGExtPltt();
        break;
      case FADE_MAIN_BG_EX3:
        GX_BeginLoadBGExtPltt();
        GX_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT3_ADRS, pfd->dat[i].siz );
        GX_EndLoadBGExtPltt();
        break;
      case FADE_SUB_BG_EX0:
        GXS_BeginLoadBGExtPltt();
        GXS_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT0_ADRS, pfd->dat[i].siz );
        GXS_EndLoadBGExtPltt();
        break;
      case FADE_SUB_BG_EX1:
        GXS_BeginLoadBGExtPltt();
        GXS_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT1_ADRS, pfd->dat[i].siz );
        GXS_EndLoadBGExtPltt();
        break;
      case FADE_SUB_BG_EX2:
        GXS_BeginLoadBGExtPltt();
        GXS_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT2_ADRS, pfd->dat[i].siz );
        GXS_EndLoadBGExtPltt();
        break;
      case FADE_SUB_BG_EX3:
        GXS_BeginLoadBGExtPltt();
        GXS_LoadBGExtPltt(
          (const void *)pfd->dat[i].trans_wk,
          EX_PAL_SLOT3_ADRS, pfd->dat[i].siz );
        GXS_EndLoadBGExtPltt();
        break;
      case FADE_MAIN_OBJ_EX:
        GX_BeginLoadOBJExtPltt();
        GX_LoadOBJExtPltt(
          (const void *)pfd->dat[i].trans_wk, 0, pfd->dat[i].siz );
        GX_EndLoadOBJExtPltt();
        break;
      case FADE_SUB_OBJ_EX:
        GXS_BeginLoadOBJExtPltt();
        GXS_LoadOBJExtPltt(
          (const void *)pfd->dat[i].trans_wk, 0, pfd->dat[i].siz );
        GXS_EndLoadOBJExtPltt();
      }
    }
  }

  pfd->trans_bit = pfd->req_bit;
  if( pfd->trans_bit == 0 ){
    pfd->req_flg = 0;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �I���`�F�b�N
 *
 * @param   pfd   �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 *
 * @retval  "0 = �I��"
 * @retval  "0 != ������"
 */
//--------------------------------------------------------------------------------------------
u16 PaletteFadeCheck(PALETTE_FADE_PTR pfd)
{
  return pfd->req_bit;
}

//--------------------------------------------------------------
/**
 * @brief   �����]���t���O���Z�b�g����
 *
 * @param   pfd     �p���b�g�t�F�[�h�V�X�e�����[�N�ւ̃|�C���^
 * @param   on_off    TRUE:�����]��ON�B�@FALSE:�����]��OFF
 */
//--------------------------------------------------------------
void PaletteTrans_AutoSet(PALETTE_FADE_PTR pfd, int on_off)
{
  pfd->auto_trans = on_off;
}


void PaletteTransSwitch(PALETTE_FADE_PTR pfd, u8 flag)
{
  pfd->req_flg = flag & 0x01;
  pfd->req_bit = 0xFFFFFFFF;
}



//--------------------------------------------------------------------------------------------
/**
 * �w��p���b�g�S�̂��N���A���ē]��
 *
 * @param bit   �N���A����p���b�g
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void PaletteAreaClear( u16 bit, HEAPID heap )
{
  void * buf;

  buf = (u8 *)GFL_HEAP_AllocMemory( heap, FADE_PAL_ALL_SIZE );
  GFL_STD_MemClear( buf, FADE_PAL_ALL_SIZE );
  DC_FlushRange( (void*)buf, FADE_PAL_ALL_SIZE );

  if( bit & PF_BIT_MAIN_BG ){
    GX_LoadBGPltt( (const void *)buf, 0, FADE_PAL_ALL_SIZE );
  }
  if( bit & PF_BIT_SUB_BG ){
    GXS_LoadBGPltt( (const void *)buf, 0, FADE_PAL_ALL_SIZE );
  }
  if( bit & PF_BIT_MAIN_OBJ ){
    GX_LoadOBJPltt( (const void *)buf, 0, FADE_PAL_ALL_SIZE );
  }
  if( bit & PF_BIT_SUB_OBJ ){
    GXS_LoadOBJPltt( (const void *)buf, 0, FADE_PAL_ALL_SIZE );
  }

  GFL_HEAP_FreeMemory( buf );

  buf = (u8 *)GFL_HEAP_AllocMemory( heap, FADE_EXPAL_ALL_SIZE );
  GFL_STD_MemClear( buf, FADE_EXPAL_ALL_SIZE );
  DC_FlushRange( (void*)buf, FADE_EXPAL_ALL_SIZE );

  if( bit & PF_BIT_MAIN_BG_EX0 ){
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT0_ADRS, FADE_EXPAL_ALL_SIZE );
    GX_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_MAIN_BG_EX1 ){
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT1_ADRS, FADE_EXPAL_ALL_SIZE );
    GX_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_MAIN_BG_EX2 ){
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT2_ADRS, FADE_EXPAL_ALL_SIZE );
    GX_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_MAIN_BG_EX3 ){
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT3_ADRS, FADE_EXPAL_ALL_SIZE );
    GX_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_SUB_BG_EX0 ){
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT0_ADRS, FADE_EXPAL_ALL_SIZE );
    GXS_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_SUB_BG_EX1 ){
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT1_ADRS, FADE_EXPAL_ALL_SIZE );
    GXS_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_SUB_BG_EX2 ){
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT2_ADRS, FADE_EXPAL_ALL_SIZE );
    GXS_EndLoadBGExtPltt();
  }
  if( bit & PF_BIT_SUB_BG_EX3 ){
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt( (const void *)buf, EX_PAL_SLOT3_ADRS, FADE_EXPAL_ALL_SIZE );
    GXS_EndLoadBGExtPltt();
  }

  if( bit & PF_BIT_MAIN_OBJ_EX ){
    GX_BeginLoadOBJExtPltt();
    GX_LoadOBJExtPltt( (const void *)buf, 0, FADE_EXPAL_ALL_SIZE );
    GX_EndLoadOBJExtPltt();
  }

  if( bit & PF_BIT_SUB_OBJ_EX ){
    GXS_BeginLoadOBJExtPltt();
    GXS_LoadOBJExtPltt( (const void *)buf, 0, FADE_EXPAL_ALL_SIZE );
    GXS_EndLoadOBJExtPltt();
  }

  GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g���[�N���w��R�[�h�ŃN���A����
 *
 * @param   pfd       �p���b�g�t�F�[�h�V�X�e���ւ̃|�C���^
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   select      �o�b�t�@�w��
 * @param   clear_code    �N���A�R�[�h
 * @param   start     �J�n�ʒu(�J���[�ʒu)
 * @param   end       �I���ʒu(�J���[�ʒu�@���I���ʒu�̃J���[�͏��������܂���B)
 *
 * start=0, end=16 �Ƃ���ƃp���b�g1�{�܂�܂�N���A�ΏۂɂȂ�܂��B
 * start=0, end=15 �Ƃ���ƁA�p���b�g�̍Ō�̃J���[�͑ΏۂɊ܂܂�܂���B
 */
//--------------------------------------------------------------
void PaletteWork_Clear(PALETTE_FADE_PTR pfd, FADEREQ req, FADEBUF select,
  u16 clear_code, u16 start, u16 end)
{
  GF_ASSERT(end * sizeof(u16) <= pfd->dat[req].siz);

  if(select == FADEBUF_SRC || select == FADEBUF_ALL){
    MI_CpuFill16(&pfd->dat[req].def_wk[start], clear_code, (end - start) * 2);
  }
  if(select == FADEBUF_TRANS || select == FADEBUF_ALL){
    MI_CpuFill16(&pfd->dat[req].trans_wk[start], clear_code, (end - start) * 2);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �w��ʒu�̃J���[�f�[�^���p���b�g���[�N����擾����
 *
 * @param   pfd       �p���b�g�t�F�[�h�V�X�e���ւ̃|�C���^
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   select      �o�b�t�@�w��
 * @param   color_pos   �擾����J���[�̈ʒu
 *
 * @retval  �J���[�f�[�^
 */
//--------------------------------------------------------------
u16 PaletteWork_ColorGet(PALETTE_FADE_PTR pfd, FADEREQ req, FADEBUF select, u16 color_pos)
{
  if(select == FADEBUF_SRC){
    return pfd->dat[req].def_wk[color_pos];
  }
  if(select == FADEBUF_TRANS){
    return pfd->dat[req].trans_wk[color_pos];
  }

  GF_ASSERT(0 && "�o�b�t�@�w�肪�Ԉ���Ă��܂�\n");
  return 0;
}




//==============================================================================
//  �ʃc�[��
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �\�t�g�t�F�[�h�C���A�A�E�g
 *
 * @param   src     ���p���b�g�f�[�^�ւ̃|�C���^
 * @param   dest    �ύX�f�[�^�����
 * @param   col_num   �ύX����J���[��
 * @param   evy     �W��(0�`16) (�ύX�̓x�����A0(���̐F)�`2,3..(���ԐF)�`16(�w�肵���F)
 * @param   next_rgb  �ύX��̐F���w��
 */
//--------------------------------------------------------------
void SoftFade(const u16 *src, u16 *dest, u16 col_num, u8 evy, u16 next_rgb)
{
  u16 i;
  int red, green, blue;
  int next_red, next_green, next_blue;

  next_red = ((PLTT_DATA*)&next_rgb)->Red;
  next_green = ((PLTT_DATA*)&next_rgb)->Green;
  next_blue = ((PLTT_DATA*)&next_rgb)->Blue;

  for(i = 0; i < col_num; i++){
    red = ((PLTT_DATA*)&src[i])->Red;
    green = ((PLTT_DATA*)&src[i])->Green;
    blue = ((PLTT_DATA*)&src[i])->Blue;

    dest[i] =
      FADE_CHANGE(red, next_red, evy) |
      (FADE_CHANGE(green, next_green, evy) << 5) |
      (FADE_CHANGE(blue, next_blue, evy) << 10);
  }
}

//--------------------------------------------------------------
/**
 * @brief   PFD�������Ƃ����J���[�����Z
 *
 * @param   pfd       �p���b�g�t�F�[�h�V�X�e���ւ̃|�C���^
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   start_pos   �\�t�g�t�F�[�h�J�n�ʒu(�J���[�P��)
 * @param   col_num     start_pos���牽�̃J���[�������Z�ΏۂƂ��邩(�J���[�P��)
 * @param   evy       EVY�l
 * @param   next_rgb    �ύX��̐F
 */
//--------------------------------------------------------------
void SoftFadePfd(PALETTE_FADE_PTR pfd, FADEREQ req, u16 start_pos, u16 col_num,
  u8 evy, u16 next_rgb)
{
  GF_ASSERT(pfd->dat[req].def_wk != NULL && pfd->dat[req].trans_wk != NULL);

  SoftFade(&pfd->dat[req].def_wk[start_pos], &pfd->dat[req].trans_wk[start_pos],
    col_num, evy, next_rgb);
}

//--------------------------------------------------------------
/**
 * @brief   �J���[�����Z(�r�b�g�w��F�p���b�g�P��)
 *
 * @param   src       ���p���b�g�f�[�^�ւ̃|�C���^
 * @param   dest      �ύX�f�[�^�����
 * @param   fade_bit    �����Z�Ώۂ̃r�b�g
 * @param   evy       EVY�l
 * @param   next_rgb    �ύX��̐F
 */
//--------------------------------------------------------------
void ColorConceChange(const u16 *src, u16 *dest, u16 fade_bit, u8 evy, u16 next_rgb)
{
  int offset = 0;

  while(fade_bit){
    if(fade_bit & 1){
      SoftFade(&src[offset], &dest[offset], 16, evy, next_rgb);
    }
    fade_bit >>= 1;
    offset += 16;
  }
}

//--------------------------------------------------------------
/**
 * @brief   PFD�������Ƃ����J���[�����Z(�r�b�g�w��F�p���b�g�P��)
 *
 * @param   pfd       �p���b�g�t�F�[�h�V�X�e���ւ̃|�C���^
 * @param   req       ���N�G�X�g�f�[�^�ԍ�
 * @param   fade_bit    �����Z�Ώۂ̃r�b�g
 * @param   evy       EVY�l
 * @param   next_rgb    �ύX��̐F
 */
//--------------------------------------------------------------
void ColorConceChangePfd(PALETTE_FADE_PTR pfd, FADEREQ req, u16 fade_bit, u8 evy, u16 next_rgb)
{
  int offset = 0;

  GF_ASSERT(pfd->dat[req].def_wk != NULL && pfd->dat[req].trans_wk != NULL);

  while(fade_bit){
    if(fade_bit & 1){
      SoftFadePfd(pfd, req, offset, 16, evy, next_rgb);
    }
    fade_bit >>= 1;
    offset += 16;
  }
}

// =============================================================================
//
//
//  ���J���[����֐��S  add goto
//
//
// =============================================================================
#define RGBtoY(r,g,b) (((r)*76 + (g)*151 + (b)*29) >> 8)
#define COL_FIL(c, p) ((u16)((p)*(c))>>8)

//--------------------------------------------------------------
/**
 * @brief �O���[�X�P�[����
 *
 * @param pal       �ύX�Ώۃp���b�g�f�[�^
 * @param pal_size    �ύX�T�C�Y(���F�ύX���邩)
 *
 * @retval  none
 *
 */
//--------------------------------------------------------------
void PaletteGrayScale(u16* pal, int pal_size)
{
  int i, r, g, b;
  u32 c;

  for(i = 0; i < pal_size; i++)
  {
    r = (*pal) & 0x1f;
    g = ((*pal) >> 5) & 0x1f;
    b = ((*pal) >> 10) & 0x1f;

    c = RGBtoY(r,g,b);
    
    *pal = (u16)((c<<10)|(c<<5)|c);
    pal++;
  }
}

//--------------------------------------------------------------
/**
 * @brief �O���[�X�P�[����(���������])
 *
 * @param pal       �ύX�Ώۃp���b�g�f�[�^
 * @param pal_size    �ύX�T�C�Y(���F�ύX���邩)
 *
 * @retval  none
 *
 */
//--------------------------------------------------------------
void PaletteGrayScaleFlip(u16* pal, int pal_size)
{
  int i, r, g, b;
  u32 c;

  for(i = 0; i < pal_size; i++){
    r = 31 - ((*pal) & 0x1f);
    g = 31 - (((*pal) >> 5) & 0x1f);
    b = 31 - (((*pal) >> 10) & 0x1f);

    c = RGBtoY(r,g,b);

    *pal = (u16)((c<<10)|(c<<5)|c);
    pal++;
  }
}

//--------------------------------------------------------------
/**
 * @brief �O���[�X�P�[����(�p���X���p)
 *
 * @param pal       �ύX�Ώۃp���b�g�f�[�^
 * @param pal_size    �ύX�T�C�Y(���F�ύX���邩)
 *
 * @retval  none
 *
 */
//--------------------------------------------------------------
void PaletteGrayScalePalaceWhite(u16* pal, int pal_size)
{
  int i, r, g, b;
  u32 c;

  static u8 whiteRate[32] = 
  {
     27, 26, 27, 26, 27, 26, 27, 26,
     27, 26, 27, 26, 27, 26, 27, 26,
     27, 26, 27, 26, 27, 26, 27, 26,
     27, 26, 27, 26, 27, 26, 27, 26,
  };

  for(i = 0; i < pal_size; i++)
  {
    r = (*pal) & 0x1f;
    g = ((*pal) >> 5) & 0x1f;
    b = ((*pal) >> 10) & 0x1f;

    c = whiteRate[RGBtoY(r,g,b)];
    
    *pal = (u16)((c<<10)|(c<<5)|c);
    pal++;
  }
}
//--------------------------------------------------------------
/**
 * @brief �O���[�X�P�[����(�p���X���p)
 *
 * @param pal       �ύX�Ώۃp���b�g�f�[�^
 * @param pal_size    �ύX�T�C�Y(���F�ύX���邩)
 *
 * @retval  none
 *
 */
//--------------------------------------------------------------
void PaletteGrayScalePalaceBlack(u16* pal, int pal_size)
{
  int i, r, g, b;
  u32 c;

  static u8 blackRate[32] = 
  {
     9, 10, 9, 10, 9, 10, 9, 10,
     9, 10, 9, 10, 9, 10, 9, 10,
     9, 10, 9, 10, 9, 10, 9, 10,
     9, 10, 9, 10, 9, 10, 9, 10,
  };

  for(i = 0; i < pal_size; i++)
  {
    r = (*pal) & 0x1f;
    g = ((*pal) >> 5) & 0x1f;
    b = ((*pal) >> 10) & 0x1f;

    c = blackRate[RGBtoY(r,g,b)];
    
    *pal = (u16)((c<<10)|(c<<5)|c);
    pal++;
  }
}

//--------------------------------------------------------------
/**
 * @brief �O���[�X�P�[������ARGB�̔䗦��������
 *
 * @param pal
 * @param pal_size
 * @param rp
 * @param gp
 * @param bp
 *
 * @retval  none
 *
 *     r = g = b = 256 �ŃO���[�X�P�[���֐��Ɠ�������
 *
 */
//--------------------------------------------------------------
void PaletteColorChange(u16* pal, int pal_size, int rp, int gp, int bp)
{
  int i, r, g, b;
  u32 c;

  for(i = 0; i < pal_size; i++){
    r = ((*pal) & 0x1f);
    g = (((*pal) >> 5) & 0x1f);
    b = (((*pal) >> 10) & 0x1f);

    c = RGBtoY(r,g,b);

    r = COL_FIL(c, rp);
    g = COL_FIL(c, gp);
    b = COL_FIL(c, bp);

    if (r > 31){ r = 31; }
    if (g > 31){ g = 31; }
    if (b > 31){ b = 31; }

    *pal = (u16)((b<<10)|(g<<5)|r);
    pal++;
  }
}


//--------------------------------------------------------------
/**
 * @brief �|�P�����̃p���b�g��ϊ����ē]��
 *
 * @param pfd
 * @param fileIdx
 * @param dataIdx
 * @param heap
 *
 * @retval  none
 *
 */
//--------------------------------------------------------------
void PokeColorChange(PALETTE_FADE_PTR pfd, u32 fileIdx, u32 dataIdx, HEAPID heap,
           FADEREQ req, u32 trans_size, u16 pos, int r, int g, int b)
{
  NNSG2dPaletteData *pal_data;
  void *arc_data;

  arc_data = GFL_ARC_UTIL_LoadPalette(fileIdx, dataIdx, &pal_data, heap);
  GF_ASSERT(arc_data != NULL);

  if(trans_size == 0){
    trans_size = pal_data->szByte;
  }

  //PaletteGrayScale(pal_data->pRawData, 16);
  PaletteColorChange(pal_data->pRawData, 16, r,g,b);

  PaletteWorkSet(pfd, pal_data->pRawData, req, pos, trans_size);

  GFL_HEAP_FreeMemory(arc_data);
}


