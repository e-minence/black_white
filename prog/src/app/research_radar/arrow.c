///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ���
 * @file   arrow.c
 * @author obata
 * @date   2010.02.22
 */
///////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "arrow.h"


//=====================================================================================
// ���萔
//=====================================================================================
#define MAX_CELL_NUM   (26) // �ő�Z����
#define ANIME_SPEED    (18.0f) // �A�j���[�V�����Đ����x
#define ANIME_FRAMES   (36) // �A�j���[�V�����t���[����
#define STRETCH_FRAMES (ANIME_FRAMES/ANIME_SPEED) // �L���A�j���̃t���[����
#define CELL_WIDTH     (8)  // �Z���̕� ( �h�b�g�� )
#define CELL_HEIGHT    (8)  // �Z���̍��� ( �h�b�g�� )

// ���̏��
typedef enum { 
  ARROW_STATE_WAIT,     // �ҋ@ ( ��\�� )
  ARROW_STATE_STRETCH,  // �L��
  ARROW_STATE_STAY,     // �ҋ@ ( �\�� )
} ARROW_STATE;

// ���̃Z���^�C�v
typedef enum {
  ARROW_CELL_H,      // ���֐L�т�Z��
  ARROW_CELL_V,      // �c�ɐL�т�Z��
  ARROW_CELL_CORNER, // �p�̃Z��
  ARROW_CELL_END,    // �I�_�̃Z��
} ARROW_CELL_TYPE;


//=====================================================================================
// �������\������Z��
//=====================================================================================
typedef struct {
  BOOL            bootFlag;   // �N�����Ă��邩�ǂ���
  u16             bootFrame;  // �N������t���[����
  BOOL            pauseFlag;  // ��~���邩�ǂ���
  u16             pauseFrame; // ��~����t���[����
  u8              left;       // �����x���W
  u8              top;        // �����y���W
  ARROW_CELL_TYPE type;       // �Z���^�C�v
  GFL_CLWK*       clactWork;  // �Z���A�N�^�[���[�N
} ARROW_CELL;


//=====================================================================================
// �����
//=====================================================================================
struct _ARROW {

  HEAPID           heapID;
  GFL_CLUNIT*      clactUnit; // �Z���A�N�^�[���j�b�g
  ARROW_DISP_PARAM dispParam; // �\���p�����[�^

  ARROW_STATE state;      // ���݂̏��
  u32         stateCount; // ��ԃJ�E���^

 ARROW_CELL cell[ MAX_CELL_NUM ];// �����\������Z��
  u8         cellNum;             // �g�p����Z���̐�
};




//=====================================================================================
// ��
//=====================================================================================

//-------------------------------------------------------------------------------------
// ����
//-------------------------------------------------------------------------------------
static void ArrowMain( ARROW* arrow ); // ���C������

static void ArrowAct_WAIT( ARROW* arrow ); // ��ԃ��C������ ( ARROW_STATE_WAIT )
static void ArrowAct_STRETCH( ARROW* arrow ); // ��ԃ��C������ ( ARROW_STATE_STRETCH )
static void ArrowAct_STAY( ARROW* arrow ); // ��ԃ��C������ ( ARROW_STATE_STAY )

static void ChangeState( ARROW* arrow, ARROW_STATE nextState ); // ��Ԃ�ύX����

static BOOL CheckAllCellBoot( const ARROW* arrow ); // �S�ẴZ�����N�����Ă��邩�ǂ����𔻒肷��
static void BootCell( ARROW* arrow, u8 cellIdx ); // �Z�����N������
static void PauseCell( ARROW* arrow, u8 cellIdx ); // �Z�����~����
static void Vanish( ARROW* arrow ); // �S�ẴZ�����\���ɂ���

//-------------------------------------------------------------------------------------
// �ݒ�
//------------------------------------------------------------------------------------- 
// �Z���̓���p�����[�^
static void SetupCellAct( ARROW* arrow, int startX, int startY, int endX, int endY ); // �Z���̓�����Z�b�g�A�b�v����
static void SetCellParams( ARROW_CELL* cell, u8 left, u8 top, u16 frame, ARROW_CELL_TYPE type ); // �Z���̃p�����[�^��ݒ肷��

// �Z���̕\���p�����[�^
static void SetDispParams( ARROW* arrow, const ARROW_DISP_PARAM* param ); // �\���p�����[�^��ݒ肷��

//-------------------------------------------------------------------------------------
// �������E�����E�j���E����
//-------------------------------------------------------------------------------------
static void InitArrow( ARROW* arrow, HEAPID heapID ); // ��� ������
static ARROW* CreateArrow( HEAPID heapID );           // ��� ����
static void DeleteArrow( ARROW* arrow );              // ��� �j��
static void CreateClactUnit( ARROW* arrow ); // �Z���A�N�^�[���j�b�g ����
static void DeleteClactUnit( ARROW* arrow ); // �Z���A�N�^�[���j�b�g �j��
static void CreateClactWorks( ARROW* arrow ); // �Z���A�N�^�[���[�N ����
static void DeleteClactWorks( ARROW* arrow ); // �Z���A�N�^�[���[�N �j��




//=====================================================================================
// ���O�����J�֐�
//===================================================================================== 

//-------------------------------------------------------------------------------------
/**
 * @brief ���𐶐�����
 *
 * @param heapID    �g�p����q�[�vID
 * @param dispParam �\���p�����[�^
 *
 * @return �쐬�������I�u�W�F�N�g
 */
//-------------------------------------------------------------------------------------
ARROW* ARROW_Create( HEAPID heapID, const ARROW_DISP_PARAM* dispParam )
{
  ARROW* arrow;
  arrow = CreateArrow( heapID );
  InitArrow( arrow, heapID );
  SetDispParams( arrow, dispParam );
  CreateClactUnit( arrow );
  CreateClactWorks( arrow );
  return arrow;
}

//-------------------------------------------------------------------------------------
/**
 * @brief ����j������
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
void ARROW_Delete( ARROW* arrow )
{
  DeleteClactWorks( arrow );
  DeleteClactUnit( arrow );
  DeleteArrow( arrow );
}

//-------------------------------------------------------------------------------------
/**
 * @brief ���̃p�����[�^���Z�b�g�A�b�v����
 *
 * @param arrow
 * @param startX  �n�_x���W
 * @param startY  �n�_y���W
 * @param endX    �I�_x���W
 * @param endY    �I�_y���W
 */
//-------------------------------------------------------------------------------------
void ARROW_Setup( ARROW* arrow, int startX, int startY, int endX, int endY ) 
{
  SetupCellAct( arrow, startX, startY, endX, endY );
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��󃁃C�����쏈��
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
void ARROW_Main( ARROW* arrow )
{
  ArrowMain( arrow );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����J�n
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
void ARROW_StartAnime( ARROW* arrow )
{
  ChangeState( arrow, ARROW_STATE_STRETCH );
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��������
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
void ARROW_Vanish( ARROW* arrow )
{
  ChangeState( arrow, ARROW_STATE_WAIT );
  Vanish( arrow );
}


//=====================================================================================
// ������
//=====================================================================================

//-------------------------------------------------------------------------------------
/**
 * @brief ��󃁃C������
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void ArrowMain( ARROW* arrow )
{
  // ��Ԃ��Ƃ̏���
  switch( arrow->state ) {
  case ARROW_STATE_WAIT:    ArrowAct_WAIT( arrow ); break;
  case ARROW_STATE_STRETCH: ArrowAct_STRETCH( arrow ); break;
  case ARROW_STATE_STAY:    ArrowAct_STAY( arrow ); break;
  default: GF_ASSERT(0);
  }

  // ��ԃJ�E���^�X�V
  arrow->stateCount++;
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( ARROW_STATE_WAIT )
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void ArrowAct_WAIT( ARROW* arrow )
{
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( ARROW_STATE_STRETCH )
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void ArrowAct_STRETCH( ARROW* arrow )
{
  int cellIdx;

  for( cellIdx=0; cellIdx < arrow->cellNum; cellIdx++ )
  {
    if( (arrow->cell[ cellIdx ].bootFlag == FALSE) &&
        (arrow->cell[ cellIdx ].bootFrame <= arrow->stateCount) ) {
      BootCell( arrow, cellIdx );
    }
    if( (arrow->cell[ cellIdx ].bootFlag == TRUE) &&
        (arrow->cell[ cellIdx ].pauseFlag == TRUE) &&
        (arrow->cell[ cellIdx ].pauseFrame <= arrow->stateCount) ) {
      PauseCell( arrow, cellIdx );
    }
  }

  if( CheckAllCellBoot( arrow ) ) {
    ChangeState( arrow, ARROW_STATE_STAY );
  }
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( ARROW_STATE_STAY )
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void ArrowAct_STAY( ARROW* arrow )
{
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void ChangeState( ARROW* arrow, ARROW_STATE nextState )
{
  // ��Ԃ��X�V
  arrow->state = nextState;
  arrow->stateCount = 0;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �S�ẴZ�����N�����Ă��邩�ǂ����𔻒肷��
 *
 * @param arrow
 *
 * @return ���ׂẴZ�����N�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------------
static BOOL CheckAllCellBoot( const ARROW* arrow )
{
  int cellIdx;
  int cellNum;

  cellNum = arrow->cellNum;

  for( cellIdx=0; cellIdx < cellNum; cellIdx++ )
  {
    // �N�����Ă��Ȃ��Z���𔭌�
    if( arrow->cell[ cellIdx ].bootFlag == FALSE ) {
      return FALSE;
    }
  }
  return TRUE;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z�����N������
 *
 * @param arrow
 * @param cellIdx �N������Z���̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------------
static void BootCell( ARROW* arrow, u8 cellIdx )
{
  ARROW_CELL* cell;
  GFL_CLWK* clwk;
  GFL_CLACTPOS pos;
  u16 anmseq;

  // �C���f�b�N�X�G���[
  GF_ASSERT( cellIdx < MAX_CELL_NUM );

  cell  = &( arrow->cell[ cellIdx ] );
  clwk  = cell->clactWork;
  pos.x = cell->left;
  pos.y = cell->top;

  // �N��
  cell->bootFlag = TRUE;

  // �A�j���[�V�����J�n
  switch( cell->type ) {
  case ARROW_CELL_H:      anmseq = arrow->dispParam.anmseqH; break;
  case ARROW_CELL_V:      anmseq = arrow->dispParam.anmseqV; break;
  case ARROW_CELL_CORNER: anmseq = arrow->dispParam.anmseqCorner; break;
  case ARROW_CELL_END:    anmseq = arrow->dispParam.anmseqEnd; break;
  default: GF_ASSERT(0);
  }
  GFL_CLACT_WK_SetPos( clwk, &pos, arrow->dispParam.setSerface );
  GFL_CLACT_WK_SetAnmSeq( clwk, anmseq );
  GFL_CLACT_WK_SetAnmFrame( clwk, 0 );
  GFL_CLACT_WK_SetAnmMode( clwk, CLSYS_ANMPM_FORWARD );
  GFL_CLACT_WK_SetAutoAnmFlag( clwk, TRUE );
  GFL_CLACT_WK_SetAutoAnmSpeed( clwk, FX32_CONST(ANIME_SPEED) );
  GFL_CLACT_WK_StartAnm( clwk );
  GFL_CLACT_WK_SetDrawEnable( clwk, TRUE );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z�����̓������~�߂�
 *
 * @param arrow
 * @param cellIdx
 */
//-------------------------------------------------------------------------------------
static void PauseCell( ARROW* arrow, u8 cellIdx )
{
  ARROW_CELL* cell;

  GF_ASSERT( cellIdx < MAX_CELL_NUM );

  cell = &( arrow->cell[ cellIdx ] ); 
  GFL_CLACT_WK_StopAnm( cell->clactWork );
}

//-------------------------------------------------------------------------------------
/**
 * @breif ���ׂẴZ�����\���ɂ���
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void Vanish( ARROW* arrow )
{
  int idx;
  int cellNum;

  cellNum = arrow->cellNum;

  for(idx=0; idx < cellNum; idx++ )
  {
    GFL_CLACT_WK_SetDrawEnable( arrow->cell[ idx ].clactWork, FALSE );
  }
}


//-------------------------------------------------------------------------------------
/**
 * @brief �Z���̃p�����[�^��ݒ肷��
 *
 * @param arrow
 * @param startX �n�_x���W
 * @param startY �n�_y���W
 * @param endX   �I�_x���W
 * @param endY   �I�_y���W
 */
//-------------------------------------------------------------------------------------
static void SetupCellAct( ARROW* arrow, int startX, int startY, int endX, int endY )
{
  int cellIdx, count;
  float frame;
  int left, top;
  int length;
  int hCellNum, vCellNum;

  GF_ASSERT( endX <= startX ); // ���ɂ����L�тȂ�
  GF_ASSERT( startY <= endY ); // ���ɂ����L�тȂ�

  frame   = 0;
  cellIdx = 0;

  // ���ɐL�т�Z��
  length   = startX - endX;
  length  -= CELL_WIDTH/2; // �p�̕�������
  hCellNum = length / CELL_WIDTH;
  left     = startX - CELL_WIDTH;
  top      = startY - CELL_HEIGHT/2;

  for( count=0; count < hCellNum; count++ )
  {
    SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_H );
    left  -= CELL_WIDTH;
    frame += STRETCH_FRAMES - 1;
    cellIdx++;
  } 

  // ���ɐL�т�Z�� ( ���� )
  if( hCellNum == 0 ) {
    if( 0 < length ) {
      SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_H );
      if( length < CELL_WIDTH/2 ) {
        arrow->cell[ cellIdx ].pauseFlag = TRUE; // �r���Ŏ~�߂�
        arrow->cell[ cellIdx ].pauseFrame = 1;
      }
      left  -= length;
      frame += (STRETCH_FRAMES * length / CELL_WIDTH) - 1;
      cellIdx++;
    }
  }
  else if( (length % CELL_WIDTH) != 0 ) {
    int diff;
    left   = endX + CELL_WIDTH/2;
    diff   = length - (hCellNum * CELL_WIDTH);
    frame -= STRETCH_FRAMES - (STRETCH_FRAMES * diff / CELL_WIDTH);
    SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_H );
    frame += STRETCH_FRAMES - 1;
    cellIdx++;
  }

  // �R�[�i�[�̃Z��
  left = endX - CELL_WIDTH/2;
  top  = startY - CELL_HEIGHT/2;
  SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_CORNER );
  top   += CELL_HEIGHT;
  frame += STRETCH_FRAMES - 1;
  cellIdx++;

  // �c�ɐL�т�Z��
  length   = endY - startY;
  length  -= CELL_HEIGHT/2;
  vCellNum = length / CELL_HEIGHT;
  left     = endX - CELL_WIDTH/2;
  top      = startY + CELL_HEIGHT/2;
  for( count=0; count < vCellNum; count++ )
  {
    SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_V );
    top   += CELL_HEIGHT;
    frame += STRETCH_FRAMES - 1;
    cellIdx++;
  } 

  // �c�ɐL�т�Z�� ( ���� )
  if( (length % CELL_WIDTH) != 0 )
  {
    int diff;
    top    = endY - CELL_HEIGHT;
    diff   = length - (vCellNum * CELL_HEIGHT);
    frame -= STRETCH_FRAMES - (STRETCH_FRAMES * diff / CELL_HEIGHT);
    SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_V );
    frame += STRETCH_FRAMES - 1;
    cellIdx++;
  }

  // �I�_
  left = endX - CELL_WIDTH/2;
  top  = endY - CELL_HEIGHT/2;
  SetCellParams( &( arrow->cell[ cellIdx ] ), left, top, frame, ARROW_CELL_END );
  cellIdx++;

  // �Z����
  arrow->cellNum = cellIdx;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z���̃p�����[�^��ݒ肷��
 *
 * @param cell  �ݒ肷��Z��
 * @param left  ����x���W
 * @param top   ����y���W
 * @param frame �N���t���[����
 * @param type  �Z���^�C�v
 */
//-------------------------------------------------------------------------------------
static void SetCellParams( ARROW_CELL* cell, u8 left, u8 top, u16 frame, ARROW_CELL_TYPE type )
{
  cell->left       = left;
  cell->top        = top;
  cell->bootFrame  = frame;
  cell->bootFlag   = FALSE;
  cell->pauseFlag  = FALSE;
  cell->pauseFrame = 0;
  cell->type       = type;
}




//=====================================================================================
// ���������E�����E�j���E����
//=====================================================================================

//-------------------------------------------------------------------------------------
/**
 * @brief ���𐶐�����
 *
 * @param heapID �g�p����q�[�vID
 */
//-------------------------------------------------------------------------------------
static ARROW* CreateArrow( HEAPID heapID )
{
  ARROW* arrow; 

  // ���𐶐�
  arrow = GFL_HEAP_AllocMemory( heapID, sizeof(ARROW) ); 
  return arrow;
}

//-------------------------------------------------------------------------------------
/**
 * @brief ��������������
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void InitArrow( ARROW* arrow, HEAPID heapID )
{
  int i;

  // �{�̂̏�����
  arrow->heapID         = heapID;
  arrow->state          = ARROW_STATE_WAIT;
  arrow->stateCount     = 0;
  arrow->cellNum        = 0;
  arrow->clactUnit      = NULL;

  // �Z���̏�����
  for( i=0; i < MAX_CELL_NUM; i++ )
  {
    arrow->cell[i].bootFlag   = FALSE;
    arrow->cell[i].bootFrame  = 0;
    arrow->cell[i].pauseFlag  = FALSE;
    arrow->cell[i].pauseFrame = 0;
    arrow->cell[i].left       = 0;
    arrow->cell[i].top        = 0;
    arrow->cell[i].clactWork  = NULL;
  }
}

//-------------------------------------------------------------------------------------
/**
 * @brief ����j������
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void DeleteArrow( ARROW* arrow )
{
  GF_ASSERT( arrow ); // ���d�j��

  // ����j��
  GFL_HEAP_FreeMemory( arrow );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g�𐶐�����
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void CreateClactUnit( ARROW* arrow )
{
  GF_ASSERT( arrow->clactUnit == NULL ); // ���d����

  arrow->clactUnit = GFL_CLACT_UNIT_Create( MAX_CELL_NUM, 0, arrow->heapID );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g��j������
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void DeleteClactUnit( ARROW* arrow )
{
  GF_ASSERT( arrow->clactUnit );

  GFL_CLACT_UNIT_Delete( arrow->clactUnit );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N�𐶐�����
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void CreateClactWorks( ARROW* arrow )
{
  int idx;
  const ARROW_DISP_PARAM* dispParam;
  GFL_CLWK_DATA data;

  dispParam = &( arrow->dispParam );
  data.pos_x   = 0;
  data.pos_y   = 0;
  data.anmseq  = 0;
  data.softpri = 0;
  data.bgpri   = 0;

  // �Z���A�N�^�[���j�b�g����������Ă��Ȃ�
  GF_ASSERT( arrow->clactUnit );

  // �Z���A�N�^�[���[�N�𐶐�
  for( idx=0; idx < MAX_CELL_NUM; idx++ )
  {
    arrow->cell[ idx ].clactWork = 
      GFL_CLACT_WK_Create( arrow->clactUnit, 
                           dispParam->cgrIndex, dispParam->plttIndex, 
                           dispParam->cellAnimIndex, &data, 
                           dispParam->setSerface, arrow->heapID ); 
  }
}

//-------------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N�𐶐�����
 *
 * @param arrow
 */
//-------------------------------------------------------------------------------------
static void DeleteClactWorks( ARROW* arrow )
{
  int idx;

  // �Z���A�N�^�[���[�N��j��
  for( idx=0; idx < MAX_CELL_NUM; idx++ )
  {
    GFL_CLACT_WK_Remove( arrow->cell[ idx ].clactWork );
  }
}

//-------------------------------------------------------------------------------------
/**
 * @param �\���p�����[�^��ݒ肷��
 *
 * @param arrow
 * @param param �\���p�����[�^
 */
//-------------------------------------------------------------------------------------
static void SetDispParams( ARROW* arrow, const ARROW_DISP_PARAM* param )
{ 
  arrow->dispParam = *param;
}
