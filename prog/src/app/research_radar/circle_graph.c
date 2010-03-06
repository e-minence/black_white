//////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �~�O���t
 * @file   circle_graph.c
 * @author obata
 * @date   2010.2.21
 */
///////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <gflib.h>
#include "circle_graph.h"
#include "queue.h"


//==============================================================================
// ���萔
//==============================================================================
#define PI                   (3.1415926535f) // �~����
#define PRINT_TARGET         (2)   // �f�o�b�O���̏o�͐�
#define STATE_QUEUE_SIZE     (10)  // ��ԃL���[�̃T�C�Y
#define CIRCLE_DIV_COUNT     (100) // �~�O���t�̕����� ( �O�����_�� )
#define CIRCLE_POINT_COUNT   (CIRCLE_DIV_COUNT) // �~�̊O�����_��
#define CIRCLE_POLYGON_COUNT (CIRCLE_DIV_COUNT) // �~���\������O�p�`�|���S���̐�
#define CIRCLE_VERTEX_COUNT  (CIRCLE_POINT_COUNT * 3) // �~�̕`��Ɏg�p���钸�_�̐�
#define DIV_PERCENTAGE       (100.0f / CIRCLE_DIV_COUNT) // �|���S�������肪��߂銄��
#define MAX_COMPONENT_NUM    (20)  // �~�O���t�̍ő�\���v�f��
#define CIRCLE_CENTER_X      (FX16_CONST(-0.707f)) // �~�O���t���S�_�� x ���W
#define CIRCLE_CENTER_Y      (FX16_CONST(-0.041f)) // �~�O���t���S�_�� y ���W
#define CIRCLE_RADIUS        (FX32_CONST(0.491f)) // �~�O���t�̔��a
#define COMPONENT_POINT_RADIUS (0.42f) // �\���v�f���w�������ꏊ�����肷��~�̔��a
#define Z_STRIDE             (FX16_CONST(0.05f)) // �\���v�f���Ƃ� z �l�̊Ԋu
#define BOARDER_Z            (FX16_CONST(4.00f)) // ���E���� z �l
#define BOARDER_COLOR_R      (0) // ���E���̐F(R)[0, 31]
#define BOARDER_COLOR_B      (0) // ���E���̐F(G)[0, 31]
#define BOARDER_COLOR_G      (0) // ���E���̐F(B)[0, 31]

#define ANALYZE_FRAMES   (120) // ��͏�Ԃ̓���t���[����
#define APPEAR_FRAMES    (30)  // �o����Ԃ̓���t���[����
#define DISAPPEAR_FRAMES (20)  // ������Ԃ̓���t���[����
#define UPDATE_FRAMES    (60)  // ������Ԃ̓���t���[����

// �~�O���t�̏��
typedef enum {
  GRAPH_STATE_HIDE,      // ��\��
  GRAPH_STATE_ANALYZE,   // ���
  GRAPH_STATE_APPEAR,    // �o��
  GRAPH_STATE_DISAPPEAR, // ����
  GRAPH_STATE_STAY,      // �\��
  GRAPH_STATE_UPDATE,    // �X�V
} GRAPH_STATE;


//==============================================================================
// ���`��p���_�f�[�^
//==============================================================================
typedef struct {
  VecFx16 pos;       // ���W
  GXRgb   color;     // ���_�J���[
  u8      polygonID; // �|���S��ID 
} VERTEX;


//==============================================================================
// ���O���t�̍\���v�f�P�ʂ̃f�[�^
//==============================================================================
typedef struct {
  u8  ID;              // �\���v�fID
  u32 value;           // �l
  u8  percentage;      // �S�\���v�f���̊���[%]
  u8  startPercentage; // ��������߂�͈� ( �擪�ʒu[%] )
  u8  endPercentage;   // ��������߂�͈� ( �����ʒu[%] )
  u8  outerColorR;     // �O���̐F(R) [0, 31]
  u8  outerColorG;     // �O���̐F(G) [0, 31]
  u8  outerColorB;     // �O���̐F(B) [0, 31]
  u8  centerColorR;    // ���S�_�̐F(R) [0, 31]
  u8  centerColorG;    // ���S�_�̐F(G) [0, 31]
  u8  centerColorB;    // ���S�_�̐F(B) [0, 31] 
} GRAPH_COMPONENT;


//==============================================================================
// ���~�O���t
//==============================================================================
struct _CIRCLE_GRAPH
{
  HEAPID heapID;

  BOOL        drawFlag;   // �`�拖�t���O
  GRAPH_STATE state;      // ���
  QUEUE*      stateQueue; // ��ԃL���[
  u32         stateCount; // ��ԃJ�E���^
  BOOL        stopFlag;   // ��~�t���O
  u32         stopCount;  // ��~�J�E���^

  GRAPH_COMPONENT components[ MAX_COMPONENT_NUM ]; // �O���t�̍\���v�f
  u8              componentNum;                    // �L���ȍ\���v�f�̐�

  fx32    radius;    // �~�̔��a
  VecFx16 centerPos; // ���S�_�̍��W
  VecFx16 circlePoints[ CIRCLE_POINT_COUNT ]; // �O�����_�̍��W ( ���S�_����̃I�t�Z�b�g )

  VERTEX vertices[ CIRCLE_VERTEX_COUNT ]; // �`��Ɏg�p���钸�_���X�g ( �O�p�`���X�g ) 
};


//==============================================================================
// ���֐��C���f�b�N�X
//==============================================================================

//------------------------------------------------------------------------------
// ���`��
//------------------------------------------------------------------------------
static void DrawGraph( const CIRCLE_GRAPH* graph ); // �~�O���t��`�悷��
static void DrawBoarder( const CIRCLE_GRAPH* graph ); // ���E����`�悷��
static void SetMatrix( const CIRCLE_GRAPH* graph ); // �s���ݒ肷��
//------------------------------------------------------------------------------
// ������
//------------------------------------------------------------------------------
static void GraphMain( CIRCLE_GRAPH* graph ); // ���C���֐�
// ��ԊJ�n����
static void GraphStart_HIDE     ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_HIDE )
static void GraphStart_ANALYZE  ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_ANALYZE )
static void GraphStart_APPEAR   ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_APPEAR )
static void GraphStart_DISAPPEAR( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_DISAPPEAR )
static void GraphStart_STAY     ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_STAY )
static void GraphStart_UPDATE   ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_UPDATE )
// ��Ԃ��Ƃ̏���
static void GraphAct_HIDE     ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_HIDE )
static void GraphAct_ANALYZE  ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_ANALYZE )
static void GraphAct_APPEAR   ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_APPEAR )
static void GraphAct_DISAPPEAR( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_DISAPPEAR )
static void GraphAct_STAY     ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_STAY )
static void GraphAct_UPDATE   ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_UPDATE )
// ��I������
static void GraphFinish_HIDE     ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_HIDE )
static void GraphFinish_ANALYZE  ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_ANALYZE )
static void GraphFinish_APPEAR   ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_APPEAR )
static void GraphFinish_DISAPPEAR( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_DISAPPEAR )
static void GraphFinish_STAY     ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_STAY )
static void GraphFinish_UPDATE   ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_UPDATE )
// ���
static void SetNextState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState ); // ���̏�Ԃ��L���[�ɓo�^����
static void SwitchState( CIRCLE_GRAPH* graph ); // ��Ԃ�؂�ւ���
static void ChangeState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState ); // ��Ԃ�ύX����
// �J�E���^
static void CountUpStateCount ( CIRCLE_GRAPH* graph ); // ��ԃJ�E���^���X�V����
static void CountDownStopCount( CIRCLE_GRAPH* graph ); // ��~�J�E���^���X�V����
//------------------------------------------------------------------------------
// ���ʑ���
//------------------------------------------------------------------------------
// �\���v�f
static void ResetComponents( CIRCLE_GRAPH* graph ); // �\���v�f�����Z�b�g����
static void AddComponent( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* newComponent ); // �\���v�f��ǉ�����
static void LowerSortComponents( CIRCLE_GRAPH* graph ); // �\���v�f���~���\�[�g����
static void UpdateComponentsPercentage( CIRCLE_GRAPH* graph ); // �e�\���v�f�̊������X�V����
static void UpdateComponentsScope( CIRCLE_GRAPH* graph ); // �e�\���v�f����߂銄���͈̔͂��X�V����
// �`��p���_���X�g
static void UpdateDrawVertices( CIRCLE_GRAPH* graph ); // �`��Ɏg�p���钸�_���X�g���X�V����
// �`�拖�t���O
static void SetDrawEnable( CIRCLE_GRAPH* graph, BOOL enable ); // �`��̋���Ԃ�ݒ肷��
// �����~�t���O
static void StopGraph( CIRCLE_GRAPH* graph, u32 frames ); // ������~������
// ���W
static void SetCenterPos( CIRCLE_GRAPH* graph, const VecFx16* pos ); // ���S�_�̍��W��ݒ肷��
//------------------------------------------------------------------------------
// ���擾�E����
//------------------------------------------------------------------------------
const u8 GetComponentRank( const CIRCLE_GRAPH* graph, u8 componentID ); // �\���v�f�̃����N
const GRAPH_COMPONENT* GetComponentByID( const CIRCLE_GRAPH* graph, u8 componentID ); // �\���v�f
const GRAPH_COMPONENT* GetComponentByRank( const CIRCLE_GRAPH* graph, int compoentRank ); // �\���v�f
static u32 GetComponentsTotalValue( const CIRCLE_GRAPH* graph ); // �S�\���v�f�̍��v�l
static u32 GetComponentsTotalPercentage( const CIRCLE_GRAPH* graph ); // �S�\���v�f����߂銄���̍��v�l
static void GetComponentCirclePointIndex( const GRAPH_COMPONENT* component, u8* destHeadIdx, u8* destTailIdx ); // �\���v�f�ɊY������O�����_�̃C���f�b�N�X
static GXRgb GetComponentOuterColor( const GRAPH_COMPONENT* component ); // �\���v�f�̊O���̐F
static GXRgb GetComponentCenterColor( const GRAPH_COMPONENT* component ); // �\���v�f�̒��S�̐F
static BOOL GetDrawEnable( const CIRCLE_GRAPH* graph ); // �`�悪������Ă��邩�ǂ���
static BOOL CheckAnime( const CIRCLE_GRAPH* graph ); // �A�j���[�V���������ǂ���
static void GetComponentPointPos( const CIRCLE_GRAPH* graph, const GRAPH_COMPONENT* component, VecFx16* dest ); // ���̎w���ׂ����W

//------------------------------------------------------------------------------
// ���������E�����E�j���E����
//------------------------------------------------------------------------------
// layer 1
static void SetupGraph( CIRCLE_GRAPH* graph );   // �~�O���t �Z�b�g�A�b�v
static void CleanUpGraph( CIRCLE_GRAPH* graph ); // �~�O���t �N���[���A�b�v
// layer 0
static void InitGraph( CIRCLE_GRAPH* graph, HEAPID heapID ); // �~�O���t ������
static CIRCLE_GRAPH* CreateGraph( HEAPID heapID );           // �~�O���t ����
static void DeleteGraph( CIRCLE_GRAPH* graph );              // �~�O���t �j��
static void CreateStateQueue( CIRCLE_GRAPH* graph ); // ��ԃL���[ ����
static void DeleteStateQueue( CIRCLE_GRAPH* graph ); // ��ԃL���[ �j��
static void SetupCirclePoints( CIRCLE_GRAPH* graph ); // �O�����_�̍��W ����

//------------------------------------------------------------------------------
// �v�Z
//------------------------------------------------------------------------------
static void CalcScreenPos( const VecFx16* pos, int* destX, int* destY ); // �X�N���[�����W���v�Z����

//------------------------------------------------------------------------------
// �f�o�b�O
//------------------------------------------------------------------------------
static void DebugPrint_stateQueue( const CIRCLE_GRAPH* graph ); // ��ԃL���[
static void DebugPrint_components( const CIRCLE_GRAPH* graph ); // �\���v�f





//===============================================================================
// ���O�����J�֐�
//===============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t���쐬����
 *
 * @param heapID �g�p����q�[�vID
 *
 * @return �쐬�����~�O���t
 */
//------------------------------------------------------------------------------
CIRCLE_GRAPH* CIRCLE_GRAPH_Create( HEAPID heapID )
{
  CIRCLE_GRAPH* graph;

  graph = CreateGraph( heapID ); // ����
  SetupGraph( graph );           // �Z�b�g�A�b�v

  return graph;
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_Delete( CIRCLE_GRAPH* graph )
{
  CleanUpGraph( graph ); // �N���[���A�b�v
  DeleteGraph( graph );  // �j��
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f���Z�b�g����
 *
 * @param graph
 * @param data    �\���v�f�̃f�[�^�z��
 * @param dataNum �\���v�f�̃f�[�^��
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_SetupComponents(
    CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data, u8 componentNum )
{
  int idx;

  // �\���v�f�����Z�b�g
  ResetComponents( graph );

  // �\���v�f��ǉ�
  for( idx=0; idx < componentNum; idx++ )
  {
    AddComponent( graph, &(data[ idx ]) );
  }

  // �Z�b�g�A�b�v����
  LowerSortComponents( graph );        // �\�[�g
  UpdateComponentsPercentage( graph ); // �������X�V
  UpdateComponentsScope( graph );      // �����͈̔͂��X�V
  UpdateDrawVertices( graph );         // �`��Ɏg�p���钸�_

  // DEBUG:
  DebugPrint_components( graph );
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f��ǉ�����
 *
 * @param graph
 * @param data  �ǉ�����\���v�f�̃f�[�^
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_AddComponent( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data )
{
  AddComponent( graph, data );         // �ǉ�
  LowerSortComponents( graph );        // �\�[�g
  UpdateComponentsPercentage( graph ); // �������X�V
  UpdateComponentsScope( graph );      // �����͈̔͂��X�V
  UpdateDrawVertices( graph );         // �`��Ɏg�p���钸�_

  // DEBUG:
  DebugPrint_components( graph );
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t�̃��C������
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_Main( CIRCLE_GRAPH* graph )
{
  GraphMain( graph ); 
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t��`�悷��
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_Draw( const CIRCLE_GRAPH* graph )
{
  if( (GetDrawEnable(graph) == TRUE) && (graph->state != GRAPH_STATE_HIDE) )
  {
    SetMatrix( graph );   // �s���ݒ肷��
    DrawGraph( graph );   // �~�O���t��`�悷��
    DrawBoarder( graph ); // ���E����`�悷��
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��̓��N�G�X�g�𔭍s����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_AnalyzeReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_ANALYZE );
}

//------------------------------------------------------------------------------
/**
 * @brief �\�����N�G�X�g�𔭍s����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_AppearReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_APPEAR );
}

//------------------------------------------------------------------------------
/**
 * @brief �������N�G�X�g�𔭍s����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_DisappearReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_DISAPPEAR );
}

//------------------------------------------------------------------------------
/**
 * @brief �X�V���N�G�X�g�𔭍s����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_UpdateReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_UPDATE );
}


//------------------------------------------------------------------------------
/**
 * @brief �`��̋���Ԃ�ݒ肷��
 *
 * @param graph
 * @param enable �`���������Ȃ� TRUE
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_SetDrawEnable( CIRCLE_GRAPH* graph, BOOL enable )
{
  SetDrawEnable( graph, enable );
}

//------------------------------------------------------------------------------
/**
 * @brief �O���t�̓�����~������
 *
 * @param graph
 * @param frames ��~�t���[����
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_StopGraph( CIRCLE_GRAPH* graph, u32 frames )
{
  StopGraph( graph, frames );
}

//------------------------------------------------------------------------------
/**
 * @brief �O���t�̒��S�_�̍��W��ݒ肷��
 *
 * @param graph
 * @param pos    ���S�_�̍��W
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_SetCenterPos( CIRCLE_GRAPH* graph, const VecFx16* pos )
{
  SetCenterPos( graph, pos );  // ���S�_��ݒ�
  UpdateDrawVertices( graph ); // �`��Ɏg�p���钸�_���W���X�V
} 

//------------------------------------------------------------------------------
/**
 * @brief �O���t�̒��S�_�̍��W��ݒ肷��
 *
 * @param graph
 * @param z    ���S�_��z���W
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_SetCenterZ( CIRCLE_GRAPH* graph, fx16 z )
{
  VecFx16 pos;

  // z���W�݂̂�ύX�������S�_�̈ʒu�x�N�g�����쐬
  VEC_Fx16Set( &pos, graph->centerPos.x, graph->centerPos.y, z );

  // ���S�_��ݒ�
  CIRCLE_GRAPH_SetCenterPos( graph, &pos );
} 

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̐����擾����  
 *
 * @param graph
 *
 * @return �w�肵���~�O���t�̍\���v�f�̐�
 */
//------------------------------------------------------------------------------
u8 CIRCLE_GRAPH_GetComponentNum( const CIRCLE_GRAPH* graph )
{
  return graph->componentNum;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̒l���擾���� ( �\���v�fID���w�� )
 *
 * @param graph 
 * @param componentID �\���v�fID
 *
 * @return �w�肵���\���v�f�̒l
 */
//------------------------------------------------------------------------------
u32 CIRCLE_GRAPH_GetComponentValue_byID( const CIRCLE_GRAPH* graph, u8 componentID )
{
  const GRAPH_COMPONENT* component;

  component = GetComponentByID( graph, componentID );

  return component->value;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̒l���擾���� ( �����N���w�� )
 *
 * @param graph 
 * @param rank �\���v�f�̃����N
 *
 * @return �w�肵���\���v�f�̒l
 */
//------------------------------------------------------------------------------
u32 CIRCLE_GRAPH_GetComponentValue_byRank( const CIRCLE_GRAPH* graph, u8 rank )
{
  const GRAPH_COMPONENT* component;

  component = GetComponentByRank( graph, rank );

  return component->value;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f����ʉ��ʂȂ̂����擾���� ( �\���v�fID���w�� )
 *
 * @param graph
 * @param componentID �\���v�fID
 *
 * @return �w�肵���\���v�f����ʉ��ʂȂ̂�
 */
//------------------------------------------------------------------------------
u8 CIRCLE_GRAPH_GetComponentRank_byID( const CIRCLE_GRAPH* graph, u8 componentID )
{
  return GetComponentRank( graph, componentID );
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f����ʉ��ʂȂ̂����擾���� ( �����N���w�� )
 *
 * @param graph
 * @param rank �\���v�f�̃����N
 *
 * @return �w�肵�������N�̍\���v�f��ID
 */
//------------------------------------------------------------------------------
u8 CIRCLE_GRAPH_GetComponentID_byRank( const CIRCLE_GRAPH* graph, u8 rank )
{
  const GRAPH_COMPONENT* component;

  component = GetComponentByRank( graph, rank );

  return component->ID;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f����߂�, �O���t���̊������擾���� ( �\���v�fID���w�� )
 *
 * @param graph 
 * @param componentID �\���v�fID
 *
 * @return �\���v�f����߂�, �O���t���̊���
 */
//------------------------------------------------------------------------------
u8 CIRCLE_GRAPH_GetComponentPercentage_byID( const CIRCLE_GRAPH* graph, u8 componentID )
{
  const GRAPH_COMPONENT* component;

  component = GetComponentByID( graph, componentID );

  return component->percentage;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f����߂�, �O���t���̊������擾���� ( �����N���w�� )
 *
 * @param graph 
 * @param rank �\���v�f�̃����N
 *
 * @return �\���v�f����߂�, �O���t���̊���
 */
//------------------------------------------------------------------------------
u8 CIRCLE_GRAPH_GetComponentPercentage_byRank( const CIRCLE_GRAPH* graph, u8 rank )
{
  const GRAPH_COMPONENT* component;

  component = GetComponentByRank( graph, rank );

  return component->percentage;
}

//------------------------------------------------------------------------------
/**
 * @breif �A�j���[�V���������ǂ����𔻒肷��
 *
 * @param graph
 *
 * @return �A�j���[�V�������Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------
BOOL CIRCLE_GRAPH_IsAnime( const CIRCLE_GRAPH* graph )
{
  return CheckAnime( graph );
}

//------------------------------------------------------------------------------
/**
 * @brief ��󂪎w�������ׂ����W���擾���� ( �\���v�fID���w�� )
 *
 * @param graph
 * @param componentID �\���v�fID
 * @param destX       �X�N���[��x���W�̊i�[��
 * @param destY       �X�N���[��y���W�̊i�[��
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_GetComponentPointPos_byID( const CIRCLE_GRAPH* graph, u8 componentID, int* destX, int* destY )
{
  const GRAPH_COMPONENT* component;
  VecFx16 worldPos;

  component = GetComponentByID( graph, componentID );
  GetComponentPointPos( graph, component, &worldPos );
  CalcScreenPos( &worldPos, destX, destY );
}

//------------------------------------------------------------------------------
/**
 * @brief ��󂪎w�������ׂ����W���擾���� ( �����N���w�� )
 *
 * @param graph
 * @param rank  �\���v�f�̃����N
 * @param destX �X�N���[��x���W�̊i�[��
 * @param destY �X�N���[��y���W�̊i�[��
 */
//------------------------------------------------------------------------------
void CIRCLE_GRAPH_GetComponentPointPos_byRank( const CIRCLE_GRAPH* graph, u8 rank, int* destX, int* destY )
{
  const GRAPH_COMPONENT* component;
  VecFx16 worldPos;

  component = GetComponentByRank( graph, rank );
  GetComponentPointPos( graph, component, &worldPos );
  CalcScreenPos( &worldPos, destX, destY );
}


//==============================================================================
// �`��
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t��`�悷��
 *
 * @param
 */
//------------------------------------------------------------------------------
static void DrawGraph( const CIRCLE_GRAPH* graph )
{
  int drawPolygonNum;
  int drawPolygonCount;
  int firstPolygonIdx;
  int vertexIdx;
  int polygonID;

  // �`��|���S���͈͂�����
  switch( graph->state )
  {
  case GRAPH_STATE_HIDE:
    drawPolygonNum = 0;  // �`�悵�Ȃ�
    break;
  case GRAPH_STATE_ANALYZE: 
    drawPolygonNum  = CIRCLE_POLYGON_COUNT * graph->stateCount / ANALYZE_FRAMES;  // �|���S����
    firstPolygonIdx = CIRCLE_POLYGON_COUNT - drawPolygonNum;                      // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_APPEAR:    
    drawPolygonNum  = CIRCLE_POLYGON_COUNT * graph->stateCount / APPEAR_FRAMES;    // �|���S����
    firstPolygonIdx = CIRCLE_POLYGON_COUNT - drawPolygonNum;                     // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_DISAPPEAR:    
    drawPolygonNum  = CIRCLE_POLYGON_COUNT * (DISAPPEAR_FRAMES - graph->stateCount) / DISAPPEAR_FRAMES; // �|���S����
    firstPolygonIdx = CIRCLE_POLYGON_COUNT - drawPolygonNum;                                  // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_STAY:    
    drawPolygonNum  = CIRCLE_POLYGON_COUNT;                      // �|���S����
    firstPolygonIdx = CIRCLE_POLYGON_COUNT - drawPolygonNum;     // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_UPDATE:  
    drawPolygonNum  = CIRCLE_POLYGON_COUNT * graph->stateCount / UPDATE_FRAMES;  // �|���S����
    firstPolygonIdx = CIRCLE_POLYGON_COUNT - drawPolygonNum;                     // �擪�|���S���C���f�b�N�X
    break;
  default: 
    GF_ASSERT(0);
  }

  // �擪���_�C���f�b�N�X���Z�o
  vertexIdx = firstPolygonIdx * 3;

  // �`��
  for( drawPolygonCount=0; drawPolygonCount < drawPolygonNum; drawPolygonCount++ )
  { 
    // �|���S��ID�͒��_1�Ɉˑ�
    polygonID = graph->vertices[ vertexIdx ].polygonID;

    // �|���S���֘A�����l��ݒ�
    G3_PolygonAttr( 
        GX_LIGHTMASK_NONE,        // no lights
        GX_POLYGONMODE_MODULATE,  // modulation mode
        GX_CULL_NONE,             // cull none
        polygonID,                // polygon ID(0 - 63)
        31,                       // alpha(0 - 31)
        0                         // OR of GXPolygonAttrMisc's value
        );

    G3_Begin( GX_BEGIN_TRIANGLES );

    // ���_1
    G3_Color( graph->vertices[ vertexIdx ].color );
    G3_Vtx( graph->vertices[ vertexIdx ].pos.x,
            graph->vertices[ vertexIdx ].pos.y,
            graph->vertices[ vertexIdx ].pos.z );
    vertexIdx = ( vertexIdx + 1 ) % CIRCLE_VERTEX_COUNT;

    // ���_2
    G3_Color( graph->vertices[ vertexIdx ].color );
    G3_Vtx( graph->vertices[ vertexIdx ].pos.x,
            graph->vertices[ vertexIdx ].pos.y,
            graph->vertices[ vertexIdx ].pos.z );
    vertexIdx = ( vertexIdx + 1 ) % CIRCLE_VERTEX_COUNT;

    // ���_3
    G3_Color( graph->vertices[ vertexIdx ].color );
    G3_Vtx( graph->vertices[ vertexIdx ].pos.x,
            graph->vertices[ vertexIdx ].pos.y,
            graph->vertices[ vertexIdx ].pos.z );
    vertexIdx = ( vertexIdx + 1 ) % CIRCLE_VERTEX_COUNT;

    G3_End();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ���E����`�悷��
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void DrawBoarder( const CIRCLE_GRAPH* graph )
{
  int componentIdx;
  int componentNum; // �\���v�f�̐�
  GXRgb boarderColor; // ���E���̐F

  // �A�j���[�V�������͕`�悵�Ȃ�
  if( CheckAnime(graph) ) { return; }

  // �|���S���֘A�����l��ݒ�
  G3_PolygonAttr( 
      GX_LIGHTMASK_NONE,        // no lights
      GX_POLYGONMODE_MODULATE,  // modulation mode
      GX_CULL_NONE,             // cull none
      0,                        // polygon ID(0 - 63)
      31,                       // alpha(0 - 31)
      0                         // OR of GXPolygonAttrMisc's value
      );

  componentNum = graph->componentNum;
  boarderColor = GX_RGB( BOARDER_COLOR_R, BOARDER_COLOR_G, BOARDER_COLOR_B );

  // ���ׂĂ̍\���v�f�ɂ���, �`����s��
  for( componentIdx=0; componentIdx < componentNum; componentIdx++ )
  {
    const GRAPH_COMPONENT* component;
    u8 headPointIdx, tailPointIdx;
    VecFx16 lineStartPos, lineEndPos;

    // �\���v�f�̃f�[�^���擾
    component = GetComponentByRank( graph, componentIdx );
    GetComponentCirclePointIndex( component, &headPointIdx, &tailPointIdx ); // �\���v�f���܂ފO�����_�͈̔�

    // ��߂銄�������l�ȉ��̏ꍇ, �`����X�L�b�v����
    if( component->percentage < 3 ) { continue; } 

    // ���C���n�_�̍��W��ݒ�
    lineStartPos.x = graph->centerPos.x;
    lineStartPos.y = graph->centerPos.y;
    lineStartPos.z = BOARDER_Z;
    // ���C���I�_�̍��W���v�Z
    lineEndPos.x = graph->centerPos.x + graph->circlePoints[ headPointIdx ].x;
    lineEndPos.y = graph->centerPos.y + graph->circlePoints[ headPointIdx ].y;
    lineEndPos.z = BOARDER_Z;

    // �\���v�f�̐擪���Ƀ��C��������
    G3_Begin( GX_BEGIN_TRIANGLES ); 
    G3_Color( boarderColor );
    G3_Vtx( lineStartPos.x, lineStartPos.y, lineStartPos.z ); // ���_1
    G3_Vtx( lineStartPos.x, lineStartPos.y, lineStartPos.z ); // ���_2
    G3_Vtx( lineEndPos.x, lineEndPos.y, lineEndPos.z );       // ���_3 
    G3_End();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief �s����Z�b�g����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void SetMatrix( const CIRCLE_GRAPH* graph )
{
  VecFx32 camPos, camUp, target;

  VEC_Set( &camPos, 0, 0, FX32_ONE );
  VEC_Set( &camUp, 0, FX32_ONE, 0 );
  VEC_Set( &target, 0, 0, 0 );

  G3_LookAt( &camPos, &camUp, &target, NULL ); 
  G3_Ortho( FX_F32_TO_FX16(1.0f),
            FX_F32_TO_FX16(-1.0f),
            FX_F32_TO_FX16(-1.333f),
            FX_F32_TO_FX16(1.333f),
            FX_F32_TO_FX16(0.1f),
            FX_F32_TO_FX16(5.0f),
            NULL ); 

  NNS_G3dGlbInit(); 
  NNS_G3dGlbLookAt( &camPos, &camUp, &target ); 
  NNS_G3dGlbOrtho( FX32_CONST(1.0f),
                   FX32_CONST(-1.0f),
                   FX32_CONST(-1.333f),
                   FX32_CONST(1.333f),
                   FX32_CONST(0.1f),
                   FX32_CONST(5.0f) ); 
  NNS_G3dGlbFlushP();
}



//==============================================================================
// ������
//===============================================================================

//------------------------------------------------------------------------------
/**
 * @brief ���C���֐�
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphMain( CIRCLE_GRAPH* graph )
{
  if( graph->stopFlag == FALSE ) {
    // ��Ԃ��Ƃ̃��C������
    switch( graph->state ) {
    case GRAPH_STATE_HIDE:      GraphAct_HIDE( graph );      break;
    case GRAPH_STATE_ANALYZE:   GraphAct_ANALYZE( graph );   break;
    case GRAPH_STATE_APPEAR:    GraphAct_APPEAR( graph );    break;
    case GRAPH_STATE_DISAPPEAR: GraphAct_DISAPPEAR( graph ); break;
    case GRAPH_STATE_STAY:      GraphAct_STAY( graph );      break;
    case GRAPH_STATE_UPDATE:    GraphAct_UPDATE( graph );    break;
    default: GF_ASSERT(0);
    }
  }

  // ��ԃJ�E���^�X�V
  CountUpStateCount( graph );

  // ��~�J�E���^�X�V
  CountDownStopCount( graph );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_HIDE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphAct_HIDE( CIRCLE_GRAPH* graph )
{
  // �L���[����łȂ���Ώ�ԑJ�ڂ���
  if( QUEUE_IsEmpty( graph->stateQueue ) == FALSE )
  {
    SwitchState( graph );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_ANALYZE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphAct_ANALYZE( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( ANALYZE_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_STAY );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_APPEAR )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphAct_APPEAR( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( APPEAR_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_STAY );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_DISAPPEAR )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphAct_DISAPPEAR( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( DISAPPEAR_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_HIDE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_STAY )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphAct_STAY( CIRCLE_GRAPH* graph )
{
  // �L���[����łȂ���Ώ�ԑJ�ڂ���
  if( QUEUE_IsEmpty( graph->stateQueue ) == FALSE )
  {
    SwitchState( graph );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_UPDATE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphAct_UPDATE( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( UPDATE_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_STAY );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���X�V����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void CountUpStateCount( CIRCLE_GRAPH* graph )
{
  u32 maxCount;

  // �C���N�������g
  graph->stateCount++;

  // �ő�l������
  switch( graph->state ) {
  case GRAPH_STATE_HIDE:      maxCount = 0xffffffff;       break;
  case GRAPH_STATE_ANALYZE:   maxCount = ANALYZE_FRAMES;   break;
  case GRAPH_STATE_APPEAR:    maxCount = APPEAR_FRAMES;    break;
  case GRAPH_STATE_DISAPPEAR: maxCount = DISAPPEAR_FRAMES; break;
  case GRAPH_STATE_STAY:      maxCount = 0xffffffff;       break;
  case GRAPH_STATE_UPDATE:    maxCount = UPDATE_FRAMES;    break;
  default: GF_ASSERT(0);
  }

  // �ő�l�␳
  if( maxCount < graph->stateCount )
  { 
    graph->stateCount = maxCount;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief ��~��Ԃ̃J�E���g�_�E������
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void CountDownStopCount( CIRCLE_GRAPH* graph )
{
  // ��~���Ă��Ȃ�
  if( graph->stopFlag == FALSE ) { return; }

  if( 0 < graph->stopCount ) {
    // �J�E���g�_�E��
    graph->stopCount--; 

    // �J�E���g�_�E���I��
    if( graph->stopCount == 0 ) { graph->stopFlag = FALSE; }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ���̏�Ԃ��L���[�ɓo�^����
 *
 * @param graph
 * @param nextState �L���[�ɓo�^������
 */
//------------------------------------------------------------------------------
static void SetNextState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState )
{
  // �G���L���[
  QUEUE_EnQueue( graph->stateQueue, nextState );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: set next state\n" );
  DebugPrint_stateQueue( graph );  // �L���[�̏�Ԃ��o��
}

//------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�؂�ւ���
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void SwitchState( CIRCLE_GRAPH* graph )
{
  GRAPH_STATE nextState;

  // ��Ԃ��X�V
  nextState = QUEUE_DeQueue( graph->stateQueue );
  ChangeState( graph, nextState );

  // DEBUG:
  DebugPrint_stateQueue( graph );  // �L���[�̏�Ԃ��o��
}

//------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param graph
 * @param nextState
 */
//------------------------------------------------------------------------------
static void ChangeState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState )
{
  // ���݂̏�Ԃ̏I������
  switch( graph->state ) {
  case GRAPH_STATE_HIDE:      GraphFinish_HIDE( graph );      break;
  case GRAPH_STATE_ANALYZE:   GraphFinish_ANALYZE( graph );   break;
  case GRAPH_STATE_APPEAR:    GraphFinish_APPEAR( graph );    break;
  case GRAPH_STATE_DISAPPEAR: GraphFinish_DISAPPEAR( graph ); break;
  case GRAPH_STATE_STAY:      GraphFinish_STAY( graph );      break;
  case GRAPH_STATE_UPDATE:    GraphFinish_UPDATE( graph );    break;
  default: GF_ASSERT(0);
  }

  // ��Ԃ��X�V
  graph->state = nextState;
  graph->stateCount = 0;

  // ���̏�Ԃ̏�Ԃ̊J�n����
  switch( nextState ) {
  case GRAPH_STATE_HIDE:      GraphStart_HIDE( graph );      break;
  case GRAPH_STATE_ANALYZE:   GraphStart_ANALYZE( graph );   break;
  case GRAPH_STATE_APPEAR:    GraphStart_APPEAR( graph );    break;
  case GRAPH_STATE_DISAPPEAR: GraphStart_DISAPPEAR( graph ); break;
  case GRAPH_STATE_STAY:      GraphStart_STAY( graph );      break;
  case GRAPH_STATE_UPDATE:    GraphStart_UPDATE( graph );    break;
  default: GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: change state ==> " );
  switch( nextState ) {
  case GRAPH_STATE_HIDE:      OS_TFPrintf( PRINT_TARGET, "HIDE" );      break;
  case GRAPH_STATE_ANALYZE:   OS_TFPrintf( PRINT_TARGET, "ANALYZE" );   break;
  case GRAPH_STATE_APPEAR:    OS_TFPrintf( PRINT_TARGET, "APPEAR" );    break;
  case GRAPH_STATE_DISAPPEAR: OS_TFPrintf( PRINT_TARGET, "DISAPPEAR" ); break;
  case GRAPH_STATE_STAY:      OS_TFPrintf( PRINT_TARGET, "STAY" );      break;
  case GRAPH_STATE_UPDATE:    OS_TFPrintf( PRINT_TARGET, "UPDATE" );    break;
  default: GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_HIDE )
 *
 * @param graph
 */
//-------------------------------------------------------------------------------
static void GraphStart_HIDE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state HIDE\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_ANALYZE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphStart_ANALYZE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state ANALYZE\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_APPEAR )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphStart_APPEAR( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state APPEAR\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_DISAPPEAR )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphStart_DISAPPEAR( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state DISAPPEAR\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_STAY )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphStart_STAY( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state STAY\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_UPDATE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphStart_UPDATE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state UPDATE\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_HIDE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------

static void GraphFinish_HIDE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state HIDE\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_ANALYZE )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphFinish_ANALYZE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state ANALYZE\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_APPEAR )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphFinish_APPEAR( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state APPEAR\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_DISAPPEAR )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphFinish_DISAPPEAR( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state DISAPPEAR\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_STAY )
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphFinish_STAY( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state STAY\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_UPDATE ) 
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void GraphFinish_UPDATE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state UPDATE\n" );
} 




//==============================================================================
// ���ʑ���
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�����Z�b�g����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void ResetComponents( CIRCLE_GRAPH* graph )
{
  graph->componentNum = 0;                // �L���ȍ\���v�f�̐������Z�b�g
  QUEUE_Clear( graph->stateQueue );       // ��ԃL���[���N���A
  ChangeState( graph, GRAPH_STATE_HIDE ); // ��Ԃ�ύX����
}

//------------------------------------------------------------------------------
/**
 * @brief �O���t�̍\���v�f��ǉ�����
 *
 * @param graph
 * @param newComponent �ǉ�����\���v�f�̃f�[�^
 */
//------------------------------------------------------------------------------
static void AddComponent( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* newComponent )
{
  // �\���v�f���I�[�o�[�t���[
  GF_ASSERT( graph->componentNum < MAX_COMPONENT_NUM );

  // �ǉ�
  graph->components[ graph->componentNum ].ID              = newComponent->ID;
  graph->components[ graph->componentNum ].value           = newComponent->value;
  graph->components[ graph->componentNum ].percentage      = 0;
  graph->components[ graph->componentNum ].startPercentage = 0;
  graph->components[ graph->componentNum ].endPercentage   = 0;
  graph->components[ graph->componentNum ].outerColorR     = newComponent->outerColorR;
  graph->components[ graph->componentNum ].outerColorG     = newComponent->outerColorG;
  graph->components[ graph->componentNum ].outerColorB     = newComponent->outerColorB;
  graph->components[ graph->componentNum ].centerColorR    = newComponent->centerColorR;
  graph->components[ graph->componentNum ].centerColorG    = newComponent->centerColorG;
  graph->components[ graph->componentNum ].centerColorB    = newComponent->centerColorB;
  graph->componentNum++;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: add new component(%d)\n", graph->componentNum );
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f���~���\�[�g����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void LowerSortComponents( CIRCLE_GRAPH* graph )
{
  int idx;
  int sortedNum;
  int unSortedTailPos;
  int componentNum;

  componentNum = graph->componentNum;

  // �o�u���\�[�g
  for( sortedNum = 0; sortedNum < componentNum-1; sortedNum++ )
  {
    // ���\�[�g�̏I�[�v�f�C���f�b�N�X���v�Z
    unSortedTailPos = componentNum - 1 - sortedNum;

    for( idx=0; idx < unSortedTailPos; idx++ )
    {
      if( graph->components[ idx ].value < graph->components[ idx + 1 ].value )
      {
        // ����ւ���
        GRAPH_COMPONENT temp         = graph->components[ idx ];
        graph->components[ idx ]     = graph->components[ idx + 1 ];
        graph->components[ idx + 1 ] = temp;
      }
    }
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: lower sort components\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �e�\���v�f����߂銄�����X�V����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void UpdateComponentsPercentage( CIRCLE_GRAPH* graph )
{
  int totalValue;
  int idx;
  int componentNum;
  int loss;

  totalValue   = GetComponentsTotalValue( graph );
  componentNum = graph->componentNum;

  // �S�Ă̍\���v�f���X�V����
  for( idx=0; idx < componentNum; idx++ )
  {
    GRAPH_COMPONENT* component;

    component = &( graph->components[ idx ] );

    // �������v�Z
    component->percentage = component->value * 100 / totalValue;
  }

  // �v�Z�덷�ɂ��s���������߂�
  loss = 100 - GetComponentsTotalPercentage( graph );
  
  // �s������␳����
  idx = 0;
  while( 0 < loss )
  {
    graph->components[ idx ].percentage++;
    loss--;
    idx = ( idx + 1 ) % componentNum;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: update components percentage\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �e�\���v�f����߂銄���͈̔͂��X�V����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void UpdateComponentsScope( CIRCLE_GRAPH* graph )
{
  int idx;
  int componentNum;
  int percentageOffset;

  componentNum = graph->componentNum;
  percentageOffset = 0;

  // �S�Ă̍\���v�f���X�V����
  for( idx=0; idx < componentNum; idx++ )
  {
    GRAPH_COMPONENT* component;

    component = &( graph->components[ idx ] );

    // �͈͂�ݒ�
    component->startPercentage = percentageOffset;
    component->endPercentage   = percentageOffset + component->percentage - 1;

    // �I�t�Z�b�g���Z
    percentageOffset += component->percentage;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: update components scope\n" );
} 

//------------------------------------------------------------------------------
/**
 * @brief �`��Ɏg�p���钸�_���X�g���X�V����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void UpdateDrawVertices( CIRCLE_GRAPH* graph )
{
  int vertexIdx;
  int drawPolygonNum;
  int componentNum;
  int componentIdx;

  vertexIdx = 0;
  componentNum = graph->componentNum;

  // ���ׂĂ̍\���v�f�̒��_��ݒ肷��
  for( componentIdx=0; componentIdx < componentNum; componentIdx++ )
  {
    const GRAPH_COMPONENT* component;
    GXRgb centerColor, outerColor;
    u8 headPointIdx, tailPointIdx;
    int circlePointIdx;
    VecFx16 centerPos;

    // �\���v�f�̃f�[�^���擾
    component   = GetComponentByRank( graph, componentIdx );
    centerColor = GetComponentCenterColor( component ); // ���S�̐F
    outerColor  = GetComponentOuterColor( component );  // �O���̐F
    GetComponentCirclePointIndex( component, &headPointIdx, &tailPointIdx ); // �\���v�f���܂ފO�����_�͈̔�
    
    // ���S�_�̍��W������
    centerPos.x = graph->centerPos.x;
    centerPos.y = graph->centerPos.y;
    centerPos.z = graph->centerPos.z - Z_STRIDE * componentIdx; // �\���v�f���ƂɈقȂ� z�l �ŕ`�悷��

    // �\���v�f����߂銄���͈͓̔��ɂ���, ���ׂĂ̊O�����_���\������|���S����ǉ�����
    for( circlePointIdx=headPointIdx; circlePointIdx <= tailPointIdx; circlePointIdx++ )
    {
      // ���_�f�[�^�ǉ�
      // ���_1
      graph->vertices[ vertexIdx ].color     = centerColor;
      graph->vertices[ vertexIdx ].pos       = centerPos;
      graph->vertices[ vertexIdx ].polygonID = componentIdx; // �|���S��ID = �\���v�f�̃C���f�b�N�X
      vertexIdx++;

      // ���_2
      graph->vertices[ vertexIdx ].color = outerColor;
      VEC_Fx16Add( &centerPos, 
                   &graph->circlePoints[ circlePointIdx ], 
                   &graph->vertices[ vertexIdx ].pos );
      graph->vertices[ vertexIdx ].polygonID = componentIdx; // �|���S��ID = �\���v�f�̃C���f�b�N�X
      vertexIdx++;

      // ���_3
      graph->vertices[ vertexIdx ].color = outerColor;
      VEC_Fx16Add( &centerPos, 
                   &graph->circlePoints[ (circlePointIdx + 1) % CIRCLE_POINT_COUNT ], 
                   &graph->vertices[ vertexIdx ].pos );
      graph->vertices[ vertexIdx ].polygonID = componentIdx; // �|���S��ID = �\���v�f�̃C���f�b�N�X
      vertexIdx++;
    }
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: update draw vertices\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �`��̋���Ԃ�ݒ肷��
 *
 * @param graph
 * @param enable �`���������Ȃ� TRUE
 */
//------------------------------------------------------------------------------
static void SetDrawEnable( CIRCLE_GRAPH* graph, BOOL enable )
{
  graph->drawFlag = enable;
}

//------------------------------------------------------------------------------
/**
 * @brief �O���t�̓�����~������
 *
 * @param graph
 * @param frames ��~�t���[����
 */
//------------------------------------------------------------------------------
static void StopGraph( CIRCLE_GRAPH* graph, u32 frames )
{
  graph->stopFlag  = TRUE;
  graph->stopCount = frames;
}

//------------------------------------------------------------------------------
/**
 * @brief ���S�_�̍��W��ݒ肷��
 *
 * @param graph
 * @param pos
 */
//------------------------------------------------------------------------------
static void SetCenterPos( CIRCLE_GRAPH* graph, const VecFx16* pos )
{
  VEC_Fx16Set( &(graph->centerPos), pos->x, pos->y, pos->z );
}


//==============================================================================
// ���擾
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̃����N���擾����
 *
 * @param graph
 * @param componentID �\���v�fID
 *
 * @return �w�肵��ID�̍\���v�f����ʉ��ʂȂ̂� [0, �v�f��-1]
 */
//------------------------------------------------------------------------------
const u8 GetComponentRank( const CIRCLE_GRAPH* graph, u8 componentID )
{
  int rank;
  int componentNum;

  componentNum = graph->componentNum;

  for( rank=0; rank < componentNum; rank++ )
  {
    const GRAPH_COMPONENT* component = &( graph->components[ rank ] );

    // ����
    if( component->ID == componentID ) { return rank; }
  }

  // �w�肳�ꂽID�����\���v�f�����݂��Ȃ�
  GF_ASSERT(0);
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f���擾����
 *
 * @param graph
 * @param rank ���Ԗڂ̍\���v�f���擾���邩
 *
 * @return �w�肵���C���f�b�N�X�̍\���v�f
 */
//------------------------------------------------------------------------------
const GRAPH_COMPONENT* GetComponentByRank( const CIRCLE_GRAPH* graph, int rank )
{
  // �C���f�b�N�X �G���[
  GF_ASSERT( rank < graph->componentNum );

  return &( graph->components[ rank ] );
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f���擾����
 *
 * @param graph
 * @param componentID �\���v�fID
 *
 * @param �w�肵��ID�����\���v�f
 */
//------------------------------------------------------------------------------
const GRAPH_COMPONENT* GetComponentByID( const CIRCLE_GRAPH* graph, u8 componentID )
{ 
  int rank;

  rank = GetComponentRank( graph, componentID );
  return GetComponentByRank( graph, rank );
}

//------------------------------------------------------------------------------
/**
 * @brief �S�\���v�f�̍��v�l���v�Z����
 *
 * @param graph
 *
 * @return �S�\���v�f�����f�[�^�̍��v�l
 */
//------------------------------------------------------------------------------
static u32 GetComponentsTotalValue( const CIRCLE_GRAPH* graph )
{
  int idx;
  int num;
  int sum;

  num = graph->componentNum;
  sum = 0;

  // ���v���v�Z
  for( idx=0; idx < num; idx++ )
  {
    sum += graph->components[ idx ].value;
  } 

  return sum;
}

//------------------------------------------------------------------------------
/**
 * @brief �S�\���v�f����߂銄���̍��v�l���v�Z����
 *
 * @param graph
 *
 * @return �S�\���v�f����߂銄���̍��v�l
 */
//------------------------------------------------------------------------------
static u32 GetComponentsTotalPercentage( const CIRCLE_GRAPH* graph )
{
  int idx;
  int num;
  int sum;

  num = graph->componentNum;
  sum = 0;

  // ���v���v�Z
  for( idx=0; idx < num; idx++ )
  {
    sum += graph->components[ idx ].percentage;
  } 

  return sum;
}

//------------------------------------------------------------------------------
/**
 * @brief �w�肵���\���v�f�ɊY������, �O�����_�̃C���f�b�N�X���擾����
 *
 * @param component    �C���f�b�N�X���擾����\���v�f
 * @param destHeadIdx  �擾�����擪�C���f�b�N�X�̊i�[��
 * @param destTailIdx  �擾���������C���f�b�N�X�̊i�[��
 */
//------------------------------------------------------------------------------
static void GetComponentCirclePointIndex( const GRAPH_COMPONENT* component, u8* destHeadIdx, u8* destTailIdx )
{
  u8 startPercentage, endPercentage;
  int headIdx, tailIdx;

  // �擪�E�����̒��_�C���f�b�N�X���v�Z
  headIdx = component->startPercentage / DIV_PERCENTAGE;
  tailIdx = component->endPercentage / DIV_PERCENTAGE;

  *destHeadIdx = headIdx;
  *destTailIdx = tailIdx;
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̊O���̐F���擾����
 *
 * @param component �F���擾����\���v�f
 *
 * @return �w�肵���\���v�f�̕\���J���[
 */
//------------------------------------------------------------------------------
static GXRgb GetComponentOuterColor( const GRAPH_COMPONENT* component )
{
  return GX_RGB( component->outerColorR, component->outerColorG, component->outerColorB );
}

//------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̒��S�̐F���擾����
 *
 * @param component �F���擾����\���v�f
 *
 * @return �w�肵���\���v�f�̕\���J���[
 */
//------------------------------------------------------------------------------
static GXRgb GetComponentCenterColor( const GRAPH_COMPONENT* component )
{
  return GX_RGB( component->centerColorR, component->centerColorG, component->centerColorB );
}

//------------------------------------------------------------------------------
/**
 * @brief �w�肵���\���v�f�ɂ���, ��󂪎w���ׂ����W���擾����
 * 
 * @param graph
 * @param component �\���v�f
 * @param dest      �v�Z�������W�̊i�[��
 */
//------------------------------------------------------------------------------
static void GetComponentPointPos( const CIRCLE_GRAPH* graph, const GRAPH_COMPONENT* component, VecFx16* dest )
{
  u8 headIdx, tailIdx, centerIdx;
  VecFx16 outerPos, vecToOuterPos;
  float vx, vy, vz;
  float cx, cy, cz;

  // �ΏۂƂȂ�O���_�̍��W���擾
  GetComponentCirclePointIndex( component, &headIdx, &tailIdx ); // �\���v�f�̗��[�̊O�����_�̃C���f�b�N�X���擾
  centerIdx = (headIdx + tailIdx) / 2; // �O�������_�̒��Ԓn�_�ɂ��钸�_�C���f�b�N�X���Z�o
  outerPos = graph->circlePoints[ centerIdx ];

  // �w���������W������
  VEC_Fx16Normalize( &outerPos, &vecToOuterPos ); // ���S�_���O���_ �����x�N�g�� ( �P�ʃx�N�g�� )
  vx = FX_FX16_TO_F32(vecToOuterPos.x) * COMPONENT_POINT_RADIUS; // ���S�_����O���_�����x�N�g��
  vy = FX_FX16_TO_F32(vecToOuterPos.y) * COMPONENT_POINT_RADIUS; // 
  vz = FX_FX16_TO_F32(vecToOuterPos.z) * COMPONENT_POINT_RADIUS; // 
  cx = FX_FX16_TO_F32(graph->centerPos.x); // ���S�_�̈ʒu�x�N�g��
  cy = FX_FX16_TO_F32(graph->centerPos.y); //
  cz = FX_FX16_TO_F32(graph->centerPos.z); //
  dest->x = FX16_CONST(cx + vx);
  dest->y = FX16_CONST(cy + vy);
  dest->z = FX16_CONST(cz + vz);
}

//------------------------------------------------------------------------------
/**
 * @brief �`�悪������Ă��邩�ǂ���
 *
 * @param graph
 *
 * @return �`�悪������Ă���Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------
static BOOL GetDrawEnable( const CIRCLE_GRAPH* graph )
{
  return graph->drawFlag;
}

//------------------------------------------------------------------------------
/**
 * @breif �A�j���[�V���������ǂ���
 *
 * @param graph
 *
 * @return �A�j���[�V�������Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------
static BOOL CheckAnime( const CIRCLE_GRAPH* graph )
{
  switch( graph->state ) {
    case GRAPH_STATE_HIDE:      return FALSE;
    case GRAPH_STATE_ANALYZE:   return TRUE;
    case GRAPH_STATE_APPEAR:    return TRUE;
    case GRAPH_STATE_DISAPPEAR: return TRUE;
    case GRAPH_STATE_STAY:      return FALSE;
    case GRAPH_STATE_UPDATE:    return TRUE;
    default: GF_ASSERT(0);
  }
  return FALSE;
}



//==============================================================================
// ���������E�����E�j���E����
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t���Z�b�g�A�b�v����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void SetupGraph( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start setup graph\n" );

  // �Z�b�g�A�b�v����
  CreateStateQueue( graph );  // ��ԃL���[���쐬
  SetupCirclePoints( graph ); // �O���I�t�Z�b�g���W���Z�o

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: end setup graph\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t���N���[���A�b�v����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void CleanUpGraph( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start clean up graph\n" );

  // �N���[���A�b�v����
  DeleteStateQueue( graph ); // ��ԃL���[��j��

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: end clean up graph\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t������������
 *
 * @param graph
 * @param heapID �g�p����q�[�vID
 */
//------------------------------------------------------------------------------
static void InitGraph( CIRCLE_GRAPH* graph, HEAPID heapID )
{
  graph->heapID       = heapID;
  graph->drawFlag     = FALSE;
  graph->state        = GRAPH_STATE_HIDE;
  graph->stateQueue   = NULL;
  graph->stopFlag     = FALSE;
  graph->stopCount    = 0;
  graph->radius       = CIRCLE_RADIUS;
  graph->componentNum = 0;

  VEC_Fx16Set( &(graph->centerPos), CIRCLE_CENTER_X, CIRCLE_CENTER_Y, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: init graph\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t�𐶐�����
 *
 * @param graph
 * @param heapID �g�p����q�[�vID
 */
//------------------------------------------------------------------------------
static CIRCLE_GRAPH* CreateGraph( HEAPID heapID )
{
  CIRCLE_GRAPH* graph;

  // ����
  graph = GFL_HEAP_AllocMemory( heapID, sizeof(CIRCLE_GRAPH) );

  // ������
  InitGraph( graph, heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: create graph\n" );

  return graph;
}

//------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void DeleteGraph( CIRCLE_GRAPH* graph )
{
  GFL_HEAP_FreeMemory( graph );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: delete graph\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[�𐶐�����
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void CreateStateQueue( CIRCLE_GRAPH* graph )
{
  // ���d����
  GF_ASSERT( graph->stateQueue == NULL );

  graph->stateQueue = QUEUE_Create( STATE_QUEUE_SIZE, graph->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: create state queueh\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[��j������
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void DeleteStateQueue( CIRCLE_GRAPH* graph )
{
  // ������
  GF_ASSERT( graph->stateQueue );

  QUEUE_Delete( graph->stateQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: delete state queue\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief �O�����_�̍��W����������
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void SetupCirclePoints( CIRCLE_GRAPH* graph )
{
  int pointIdx;
  float radius;

  radius = FX_FX32_TO_F32( graph->radius );

  for( pointIdx=0; pointIdx < CIRCLE_POINT_COUNT; pointIdx++ )
  {
    float radian;
    float x, y, z;

    radian = 2.0f * PI * pointIdx / (float)CIRCLE_POINT_COUNT;
    radian = 0.5f * PI - radian; // �ʑ��𒲐�
    x = radius * cosf(radian);
    y = radius * sinf(radian);
    z = 0;

    VEC_Fx16Set( &(graph->circlePoints[ pointIdx ]), 
                 FX_F32_TO_FX16(x), FX_F32_TO_FX16(y), FX_F32_TO_FX16(z)  );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: setup circle points\n" );
}


//==============================================================================
// ���v�Z
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @breif �X�N���[�����W�����߂�
 *
 * @param worldPos �ϊ����郏�[���h���W
 * @param destX    ���߂��X�N���[��x���W�̊i�[��
 * @param destY    ���߂��X�N���[��y���W�̊i�[��Y
 */
//------------------------------------------------------------------------------
static void CalcScreenPos( const VecFx16* worldPos, int* destX, int* destY )
{
  VecFx32 posFx32;
  int ret;

  // �x�N�g����ϊ�
  posFx32.x = FX32_CONST( FX_FX16_TO_F32(worldPos->x) );
  posFx32.y = FX32_CONST( FX_FX16_TO_F32(worldPos->y) );
  posFx32.z = FX32_CONST( FX_FX16_TO_F32(worldPos->z) );

  ret = NNS_G3dWorldPosToScrPos( &posFx32, destX, destY );
}


//==============================================================================
// ���f�o�b�O
//===============================================================================

//------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[�̓��e���o�͂���
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void DebugPrint_stateQueue( const CIRCLE_GRAPH* graph )
{
  int i;
  int data;
  int dataNum;
  const QUEUE* queue;

  queue   = graph->stateQueue;
  dataNum = QUEUE_GetDataNum( queue );

  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: state queue = " );
  for( i=0; i < dataNum; i++ )
  {
    data = QUEUE_PeekData( graph->stateQueue, i );

    switch( data ) {
    case GRAPH_STATE_HIDE:      OS_TFPrintf( PRINT_TARGET, "HIDE ");      break;
    case GRAPH_STATE_ANALYZE:   OS_TFPrintf( PRINT_TARGET, "ANALYZE ");   break;
    case GRAPH_STATE_APPEAR:    OS_TFPrintf( PRINT_TARGET, "APPEAR ");    break;
    case GRAPH_STATE_DISAPPEAR: OS_TFPrintf( PRINT_TARGET, "DISAPPEAR "); break;
    case GRAPH_STATE_STAY:      OS_TFPrintf( PRINT_TARGET, "STAY ");      break;
    case GRAPH_STATE_UPDATE:    OS_TFPrintf( PRINT_TARGET, "UPDATE ");    break;
    default:                    OS_TFPrintf( PRINT_TARGET, "UNKNOWN ");   break;
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief ���ׂĂ̍\���v�f���o�͂���
 *
 * @param graph
 */
//------------------------------------------------------------------------------
static void DebugPrint_components( const CIRCLE_GRAPH* graph )
{
  int idx;
  int componentNum;
  int totalValue;
  int totalPercentage;

  totalValue      = 0;
  totalPercentage = 0;
  componentNum    = graph->componentNum;

  // �ʃf�[�^�o��
  for( idx=0; idx < componentNum; idx++ )
  {
    const GRAPH_COMPONENT* component;

    component = GetComponentByRank( graph, idx );

    // �f�[�^�o��
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].ID              = %d\n", idx, component->ID );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].value           = %d\n", idx, component->value );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].percentage      = %d\n", idx, component->percentage );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].startPercentage = %d\n", idx, component->startPercentage );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].endPercentage   = %d\n", idx, component->endPercentage );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].outerColorR     = %d\n", idx, component->outerColorR );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].outerColorG     = %d\n", idx, component->outerColorG );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].outerColorB     = %d\n", idx, component->outerColorB );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].centerColorR    = %d\n", idx, component->centerColorR );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].centerColorG    = %d\n", idx, component->centerColorG );
    OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: component[%d].centerColorB    = %d\n", idx, component->centerColorB );

    // �����f�[�^�W�v
    totalValue      += component->value;
    totalPercentage += component->percentage;
  }

  // �����f�[�^�o��
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: components num   = %d\n", componentNum );
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: total value      = %d\n", totalValue );
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: total percentage = %d\n", totalPercentage );
} 
