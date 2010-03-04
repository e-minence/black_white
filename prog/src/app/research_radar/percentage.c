//////////////////////////////////////////////////////////////////////////////
/**
 * @brief  1�̃p�[�Z���g�\���������\����
 * @file   percentage.c
 * @author obata
 * @date   2010.03.01
 */
//////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "percentage.h"


//============================================================================
// ���萔
//============================================================================ 
#define PRINT_TARGET (2)  // �f�o�b�O���̏o�͐�
#define OFFSET_X_100 (10)  // �x�[�X�����x���W�I�t�Z�b�g ( 100�̈� )
#define OFFSET_Y_100 (-8) // �x�[�X�����y���W�I�t�Z�b�g ( 100�̈� )
#define OFFSET_X_10  (15) // �x�[�X�����x���W�I�t�Z�b�g ( 10�̈� )
#define OFFSET_Y_10  (-8) // �x�[�X�����y���W�I�t�Z�b�g ( 10�̈� )
#define OFFSET_X_1   (20) // �x�[�X�����x���W�I�t�Z�b�g ( 1�̈� )
#define OFFSET_Y_1   (-8) // �x�[�X�����y���W�I�t�Z�b�g ( 1�̈� )

// �Z���A�N�^�[���[�N�̃C���f�b�N�X
typedef enum {
  CLWK_BASE,      // �y��
  CLWK_VALUE_100, // 100�̈�
  CLWK_VALUE_10,  // 10�̈�
  CLWK_VALUE_1,   // 1�̈�
  CLWK_NUM,       // ����
} CLWK_INDEX;


//============================================================================
// ���p�[�Z���e�[�W
//============================================================================ 
struct _PERCENTAGE
{
  HEAPID heapID;

  u8 value; // ���l
  int x;    // x���W
  int y;    // y���W

  GFL_CLUNIT* clactUnit;             // �Z���A�N�^�[���j�b�g
  GFL_CLWK*   clactWork[ CLWK_NUM ]; // �Z���A�N�^�[���[�N
  PERCENTAGE_DISP_PARAM dispParam;   // �\���p�����[�^
};


//============================================================================
// ������J�֐�
//============================================================================ 

//----------------------------------------------------------------------------
// ���\������
//----------------------------------------------------------------------------
void SetDrawEnable( PERCENTAGE* percentage, BOOL enable ); // �`���Ԃ�ݒ肷��

//----------------------------------------------------------------------------
// ���ݒ�
//----------------------------------------------------------------------------
// layer 1
void SetupClactWorks( PERCENTAGE* percentage ); // �Z���A�N�^�[���[�N���Z�b�g�A�b�v����
// layer 0
void SetValue( PERCENTAGE* percentage, u8 value ); // ���l��ݒ肷��
void SetPos( PERCENTAGE* percentage, int x, int y ); // ���W��ݒ肷��

//----------------------------------------------------------------------------
// ���擾
//----------------------------------------------------------------------------
u8 GetValue_1( const PERCENTAGE* percentage ); // 1�̈ʂ̐������擾����
u8 GetValue_10( const PERCENTAGE* percentage ); // 10�̈ʂ̐������擾����
u8 GetValue_100( const PERCENTAGE* percentage ); // 100�̈ʂ̐������擾����
void GetPos_1( const PERCENTAGE* percentage, s16* destX, s16* destY ); // 1�̈ʂ̐����̍��W���擾����
void GetPos_10( const PERCENTAGE* percentage, s16* destX, s16* destY ); // 10�̈ʂ̐����̍��W���擾����
void GetPos_100( const PERCENTAGE* percentage, s16* destX, s16* destY ); // 100�̈ʂ̐����̍��W���擾����

//----------------------------------------------------------------------------
// �������E�������E�j��
//----------------------------------------------------------------------------
// layer 1
PERCENTAGE* CreatePercentage( HEAPID heapID ); // �p�[�Z���e�[�W�𐶐�����
void DeletePercentage( PERCENTAGE* percentage ); // �p�[�Z���e�[�W��j������
void InitPercentage( PERCENTAGE* percentage, HEAPID heapID ); // �p�[�Z���e�[�W������������
void SetupPercentage( PERCENTAGE* percentage,
    const PERCENTAGE_DISP_PARAM* dispParam, GFL_CLUNIT* clactUnit ); // �p�[�Z���e�[�W���Z�b�g�A�b�v����
void CleanUpPercentage( PERCENTAGE* percentage ); // �p�[�Z���e�[�W���N���[���A�b�v����
// layer 0
void SetClactUnit( PERCENTAGE* percentage, GFL_CLUNIT* clactUnit ); // �Z���A�N�^�[���j�b�g���Z�b�g����
void CreateClactWorks( PERCENTAGE* percentage ); // �Z���A�N�^�[���[�N�𐶐�����
void DeleteClactWorks( PERCENTAGE* percentage ); // �Z���A�N�^�[���[�N��j������
void SetDispParams( PERCENTAGE* percentage, 
    const PERCENTAGE_DISP_PARAM* dispParam ); // �\���p�����[�^���Z�b�g����



//============================================================================
// �������E�j��
//============================================================================

//----------------------------------------------------------------------------
/**
 * @brief ����
 *
 * @param percentage
 * @parma dispParam �\���p�����[�^
 * @parma clactUnit �Z���A�N�^�[���j�b�g
 *
 * @return ���������C���X�^���X
 */
//----------------------------------------------------------------------------
PERCENTAGE* PERCENTAGE_Create( 
    HEAPID heapID, const PERCENTAGE_DISP_PARAM* dispParam, GFL_CLUNIT* clactUnit )
{
  PERCENTAGE* percentage;
  percentage = CreatePercentage( heapID ); // ����
  InitPercentage( percentage, heapID ); // ������
  SetupPercentage( percentage, dispParam, clactUnit ); // �Z�b�g�A�b�v
  return percentage;
}

//----------------------------------------------------------------------------
/**
 * @brief �j��
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void PERCENTAGE_Delete( PERCENTAGE* percentage )
{
  CleanUpPercentage( percentage ); // �N���[���A�b�v
  DeletePercentage( percentage ); // �j��
}


//============================================================================
// ������
//============================================================================

//----------------------------------------------------------------------------
/**
 * @brief ���W��ݒ肷��
 *
 * @param percentage
 * @param x �x�[�X����x���W
 * @param y �x�[�X����y���W
 */
//----------------------------------------------------------------------------
void PERCENTAGE_SetPos( PERCENTAGE* percentage, int x, int y )
{
  SetPos( percentage, x, y );
  SetupClactWorks( percentage );
}

//----------------------------------------------------------------------------
/**
 * @brief ���l��ݒ肷��
 *
 * @param percentage
 * @param value ���l
 */
//----------------------------------------------------------------------------
void PERCENTAGE_SetValue( PERCENTAGE* percentage, int value )
{
  SetValue( percentage, value );
  SetupClactWorks( percentage );
}

//----------------------------------------------------------------------------
/**
 * @brief �`���Ԃ̕ύX
 *
 * @param percentage
 * @param enable �`�悷�邩�ǂ���
 */
//----------------------------------------------------------------------------
void PERCENTAGE_SetDrawEnable( PERCENTAGE* percentage, BOOL enable )
{
  SetDrawEnable( percentage, enable );
}



//============================================================================ 
// ���\������
//============================================================================ 

//----------------------------------------------------------------------------
/**
 * @brief �`���Ԃ�ݒ肷��
 *
 * @param percentage
 * @param enable �`�悷�邩�ǂ���
 */
//----------------------------------------------------------------------------
void SetDrawEnable( PERCENTAGE* percentage, BOOL enable )
{
  // �y��
  GFL_CLACT_WK_SetDrawEnable( percentage->clactWork[ CLWK_BASE ], enable );

  // 100�̈�
  if( GetValue_100(percentage) != 0 ) {
    GFL_CLACT_WK_SetDrawEnable( percentage->clactWork[ CLWK_VALUE_100 ], enable );
  }

  // 10�̈�
  if( (GetValue_100(percentage) != 0) || (GetValue_10(percentage) != 0) ) {
    GFL_CLACT_WK_SetDrawEnable( percentage->clactWork[ CLWK_VALUE_10 ], enable );
  }

  // 1�̈�
  GFL_CLACT_WK_SetDrawEnable( percentage->clactWork[ CLWK_VALUE_1 ], enable );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: set draw enable ==> %d\n", enable );
}




//============================================================================ 
// ���ݒ�
//============================================================================ 

//----------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N���Z�b�g�A�b�v����
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void SetupClactWorks( PERCENTAGE* percentage )
{ 
  GFL_CLWK* clwk;
  const PERCENTAGE_DISP_PARAM* dispParam;
  GFL_CLACTPOS pos;

  dispParam = &( percentage->dispParam );

  // �y��
  clwk = percentage->clactWork[ CLWK_BASE ];
  pos.x = percentage->x;
  pos.y = percentage->y;
  GFL_CLACT_WK_SetPos( clwk, &pos, dispParam->setSerface );
  GFL_CLACT_WK_SetAnmSeq( clwk, dispParam->anmseqBase );
  GFL_CLACT_WK_SetAnmFrame( clwk, 0 );
  GFL_CLACT_WK_StopAnm( clwk );
  GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );

  // 100�̈�
  clwk = percentage->clactWork[ CLWK_VALUE_100 ];
  GetPos_100( percentage, &pos.x, &pos.y );
  GFL_CLACT_WK_SetPos( clwk, &pos, dispParam->setSerface );
  GFL_CLACT_WK_SetAnmSeq( clwk, dispParam->anmseqNumber );
  GFL_CLACT_WK_SetAnmFrame( clwk, GetValue_100(percentage) << FX32_SHIFT );
  GFL_CLACT_WK_StopAnm( clwk );
  GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );

  // 10�̈�
  clwk = percentage->clactWork[ CLWK_VALUE_10 ];
  GetPos_10( percentage, &pos.x, &pos.y );
  GFL_CLACT_WK_SetPos( clwk, &pos, dispParam->setSerface );
  GFL_CLACT_WK_SetAnmSeq( clwk, dispParam->anmseqNumber );
  GFL_CLACT_WK_SetAnmFrame( clwk, GetValue_10(percentage) << FX32_SHIFT );
  GFL_CLACT_WK_StopAnm( clwk );
  GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );

  // 1�̈�
  clwk = percentage->clactWork[ CLWK_VALUE_1 ];
  GetPos_1( percentage, &pos.x, &pos.y );
  GFL_CLACT_WK_SetPos( clwk, &pos, dispParam->setSerface );
  GFL_CLACT_WK_SetAnmSeq( clwk, dispParam->anmseqNumber );
  GFL_CLACT_WK_SetAnmFrame( clwk, GetValue_1(percentage) << FX32_SHIFT );
  GFL_CLACT_WK_StopAnm( clwk );
  GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: setup clact works\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief ���l��ݒ肷��
 *
 * @param percentage
 * @param value      �ݒ肷��l
 */
//----------------------------------------------------------------------------
void SetValue( PERCENTAGE* percentage, u8 value )
{
  percentage->value = value;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: set value ==> %d\n", value );
}

//----------------------------------------------------------------------------
/**
 * @brief ���W��ݒ肷��
 *
 * @param percentage
 * @param x x���W
 * @param y y���W
 */
//----------------------------------------------------------------------------
void SetPos( PERCENTAGE* percentage, int x, int y )
{
  percentage->x = x;
  percentage->y = y;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: set pos (%d, %d)\n", x, y );
}



//============================================================================ 
// ���擾
//============================================================================ 

//----------------------------------------------------------------------------
/**
 * @brief 1�̈ʂ̐������擾����
 *
 * @param percentage
 *
 * @return 1�̈ʂ̐��l
 */
//----------------------------------------------------------------------------
u8 GetValue_1( const PERCENTAGE* percentage )
{
  return percentage->value % 10;
}

//----------------------------------------------------------------------------
/**
 * @brief 10�̈ʂ̐������擾����
 *
 * @param percentage
 *
 * @return 10�̈ʂ̐��l
 */
//----------------------------------------------------------------------------
u8 GetValue_10( const PERCENTAGE* percentage )
{
  return ( percentage->value % 100 ) / 10;
}

//----------------------------------------------------------------------------
/**
 * @brief 100�̈ʂ̐������擾����
 *
 * @param percentage
 *
 * @return 100�̈ʂ̐��l
 */
//----------------------------------------------------------------------------
u8 GetValue_100( const PERCENTAGE* percentage )
{
  return percentage->value / 100;
}

//----------------------------------------------------------------------------
/**
 * @brief 1�̈ʂ̐����̍��W���擾����
 *
 * @param percentage
 * @param destX ���߂�x���W�̊i�[�� 
 * @param destY ���߂�y���W�̊i�[��
 *
 * @return 1�̈ʂ̐��l�̍��W
 */
//----------------------------------------------------------------------------
void GetPos_1( const PERCENTAGE* percentage, s16* destX, s16* destY )
{
  *destX = percentage->x + OFFSET_X_1;
  *destY = percentage->y + OFFSET_Y_1;
}

//----------------------------------------------------------------------------
/**
 * @brief 10�̈ʂ̐����̍��W���擾����
 *
 * @param percentage
 * @param destX ���߂�x���W�̊i�[�� 
 * @param destY ���߂�y���W�̊i�[��
 *
 * @return 10�̈ʂ̐��l�̍��W
 */
//----------------------------------------------------------------------------
void GetPos_10( const PERCENTAGE* percentage, s16* destX, s16* destY )
{
  *destX = percentage->x + OFFSET_X_10;
  *destY = percentage->y + OFFSET_Y_10;
}

//----------------------------------------------------------------------------
/**
 * @brief 100�̈ʂ̐����̍��W���擾����
 *
 * @param percentage
 * @param destX ���߂�x���W�̊i�[�� 
 * @param destY ���߂�y���W�̊i�[��
 *
 * @return 100�̈ʂ̐��l�̍��W
 */
//----------------------------------------------------------------------------
void GetPos_100( const PERCENTAGE* percentage, s16* destX, s16* destY )
{
  *destX = percentage->x + OFFSET_X_100;
  *destY = percentage->y + OFFSET_Y_100;
}



//============================================================================ 
// �������E�������E�j��
//============================================================================ 

//----------------------------------------------------------------------------
/**
 * @brief �p�[�Z���e�[�W�𐶐�����
 *
 * @param heapID
 *
 * @return ���������I�u�W�F�N�g
 */
//----------------------------------------------------------------------------
PERCENTAGE* CreatePercentage( HEAPID heapID )
{
  PERCENTAGE* percentage;
  percentage = GFL_HEAP_AllocMemory( heapID, sizeof(PERCENTAGE) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: create percentage\n" );

  return percentage;
}

//----------------------------------------------------------------------------
/**
 * @brief �p�[�Z���e�[�W��j������
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void DeletePercentage( PERCENTAGE* percentage )
{
  GFL_HEAP_FreeMemory( percentage );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: delete percentage\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �p�[�Z���e�[�W������������
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void InitPercentage( PERCENTAGE* percentage, HEAPID heapID )
{
  int i;

  percentage->heapID = heapID;
  percentage->value = 0;
  percentage->clactUnit = NULL;

  for( i=0; i < CLWK_NUM; i++ )
  {
    percentage->clactWork[i] = NULL;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: init percentage\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �p�[�Z���e�[�W���Z�b�g�A�b�v����
 *
 * @param percentage
 * @param dispParam  �\���p�����[�^
 * @param clactUnit  �Z���A�N�^�[���j�b�g
 */
//----------------------------------------------------------------------------
void SetupPercentage( 
    PERCENTAGE* percentage, const PERCENTAGE_DISP_PARAM* dispParam, GFL_CLUNIT* clactUnit )
{
  SetClactUnit( percentage, clactUnit );  // �Z���A�N�^�[���j�b�g���Z�b�g
  SetDispParams( percentage, dispParam ); // �\���p�����[�^���Z�b�g
  CreateClactWorks( percentage );         // �Z���A�N�^�[���[�N�𐶐�

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: setup percentage\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �p�[�Z���e�[�W���N���[���A�b�v����
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void CleanUpPercentage( PERCENTAGE* percentage )
{
  DeleteClactWorks( percentage ); // �Z���A�N�^�[���[�N��j��

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: clean up percentage\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���j�b�g���Z�b�g����
 *
 * @param percentage
 * @param clactUnit
 */
//----------------------------------------------------------------------------
void SetClactUnit( PERCENTAGE* percentage, GFL_CLUNIT* clactUnit )
{
  GF_ASSERT( percentage->clactUnit == NULL );

  percentage->clactUnit = clactUnit;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: set clact unit\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N�𐶐�����
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void CreateClactWorks( PERCENTAGE* percentage )
{ 
  int idx;
  const PERCENTAGE_DISP_PARAM* dispParam;
  GFL_CLWK_DATA data;

  dispParam = &(percentage->dispParam);
  data.pos_x   = 0;
  data.pos_y   = 0;
  data.anmseq  = 0;
  data.softpri = 0;
  data.bgpri   = 0;

  for(idx=0; idx < CLWK_NUM; idx++ )
  {
    GF_ASSERT( percentage->clactWork[ idx ] == NULL );

    percentage->clactWork[ idx ] =
      GFL_CLACT_WK_Create( percentage->clactUnit, 
                           dispParam->cgrIndex, 
                           dispParam->plttIndex, 
                           dispParam->cellAnimIndex, 
                           &data, 
                           dispParam->setSerface, 
                           percentage->heapID );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: create clact works\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[���[�N��j������
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void DeleteClactWorks( PERCENTAGE* percentage )
{
  int idx;

  for(idx=0; idx < CLWK_NUM; idx++ )
  {
    GF_ASSERT( percentage->clactWork[ idx ] );

    GFL_CLACT_WK_Remove( percentage->clactWork[ idx ] );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: delete clact works\n" );
}

//----------------------------------------------------------------------------
/**
 * @brief �\���p�����[�^���Z�b�g����
 *
 * @param percentage
 */
//----------------------------------------------------------------------------
void SetDispParams( PERCENTAGE* percentage, const PERCENTAGE_DISP_PARAM* dispParam )
{
  percentage->dispParam = *dispParam;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "PERCENTAGE: set disp params\n" );
} 
