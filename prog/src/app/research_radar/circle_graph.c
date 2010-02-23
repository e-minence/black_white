///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �~�O���t
 * @file   circle_graph.c
 * @author obata
 * @date   2010.2.21
 */
/////////////////////////////////////////////////////////////////////////////////////////// 
#include <math.h>
#include <gflib.h>
#include "circle_graph.h"
#include "queue.h"


//=========================================================================================
// ���萔
//=========================================================================================
#define PI                   (3.1415926535f) // �~����
#define PRINT_TARGET         (2)   // �f�o�b�O���̏o�͐�
#define STATE_QUEUE_SIZE     (10)  // ��ԃL���[�̃T�C�Y
#define CIRCLE_DIV_COUNT     (100) // �~�O���t�̕����� ( �O�����_�� )
#define CIRCLE_POINT_COUNT   (CIRCLE_DIV_COUNT) // �~�̊O�����_��
#define CIRCLE_VERTEX_COUNT  (CIRCLE_POINT_COUNT * 3) // �~�̕`��Ɏg�p���钸�_�̐�
#define DIV_PERCENTAGE       (100.0f / CIRCLE_DIV_COUNT) // �|���S�������肪��߂銄��
#define MAX_COMPONENT_NUM    (20)  // �~�O���t�̍ő�\���v�f��
#define CIRCLE_RADIUS        (FX32_CONST(0.491f)) // �~�O���t�̔��a
#define CIRCLE_CENTER_X      (FX16_CONST(-0.707f)) // �~�O���t���S�_�� x ���W
#define CIRCLE_CENTER_Y      (FX16_CONST(-0.041f)) // �~�O���t���S�_�� y ���W

#define ANALYZE_FRAMES (120) // ��͏�Ԃ̓���t���[����
#define SHOW_FRAMES    (30)  // �o����Ԃ̓���t���[����
#define HIDE_FRAMES    (30)  // ������Ԃ̓���t���[����
#define UPDATE_FRAMES  (60)  // ������Ԃ̓���t���[����


// �~�O���t�̏��
typedef enum {
  GRAPH_STATE_WAIT,    // �ҋ@ ( ��\�� )
  GRAPH_STATE_ANALYZE, // ���
  GRAPH_STATE_SHOW,    // �o��
  GRAPH_STATE_HIDE,    // ����
  GRAPH_STATE_STAY,    // �ҋ@ ( �\�� )
  GRAPH_STATE_UPDATE,  // �X�V
} GRAPH_STATE;


//=========================================================================================
// ���`��p���_�f�[�^
//=========================================================================================
typedef struct
{
  VecFx16 pos;   // ���W
  GXRgb   color; // ���_�J���[

} VERTEX;


//=========================================================================================
// ���O���t�̍\���v�f�P�ʂ̃f�[�^
//=========================================================================================
typedef struct
{
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


//=========================================================================================
// ���~�O���t
//=========================================================================================
struct _CIRCLE_GRAPH
{
  HEAPID heapID;

  GRAPH_STATE state;      // ���
  QUEUE*      stateQueue; // ��ԃL���[
  u32         stateCount; // ��ԃJ�E���^

  fx32    radius;    // �~�̔��a
  VecFx16 centerPos; // ���S�_�̍��W
  VecFx16 circlePoints[ CIRCLE_POINT_COUNT ]; // �O�����_�̍��W ( ���S�_����̃I�t�Z�b�g )

  GRAPH_COMPONENT components[ MAX_COMPONENT_NUM ]; // �\���v�f
  u8              componentNum;                    // �L���ȍ\���v�f�̐�

  VERTEX vertices[ CIRCLE_VERTEX_COUNT ]; // �`��Ɏg�p���钸�_���X�g ( �O�p�`���X�g ) 
};




//=========================================================================================
// ��
//========================================================================================= 

//-----------------------------------------------------------------------------------------
// �`��
//-----------------------------------------------------------------------------------------
static void DrawGraph( const CIRCLE_GRAPH* graph ); // �~�O���t��`�悷��
static void SetMatrix( const CIRCLE_GRAPH* graph ); // �s���ݒ肷��

//-----------------------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------------------
static void GraphMain( CIRCLE_GRAPH* graph );

static void GraphAct_WAIT   ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_WAIT )
static void GraphAct_ANALYZE( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_ANALYZE )
static void GraphAct_SHOW   ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_SHOW )
static void GraphAct_HIDE   ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_HIDE )
static void GraphAct_STAY   ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_STAY )
static void GraphAct_UPDATE ( CIRCLE_GRAPH* graph ); // ��ԃ��C������ ( GRAPH_STATE_UPDATE )

static void CountUpStateCount( CIRCLE_GRAPH* graph ); // ��ԃJ�E���^���X�V����

static void SetNextState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState ); // ���̏�Ԃ��L���[�ɓo�^����
static void SwitchState( CIRCLE_GRAPH* graph ); // ��Ԃ�؂�ւ���
static void ChangeState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState ); // ��Ԃ�ύX����

static void GraphStart_WAIT   ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_WAIT )
static void GraphStart_ANALYZE( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_ANALYZE )
static void GraphStart_SHOW   ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_SHOW )
static void GraphStart_HIDE   ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_HIDE )
static void GraphStart_STAY   ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_STAY )
static void GraphStart_UPDATE ( CIRCLE_GRAPH* graph ); // ��ԊJ�n���� ( GRAPH_STATE_UPDATE )

static void GraphFinish_WAIT   ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_WAIT )
static void GraphFinish_ANALYZE( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_ANALYZE )
static void GraphFinish_SHOW   ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_SHOW )
static void GraphFinish_HIDE   ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_HIDE )
static void GraphFinish_STAY   ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_STAY )
static void GraphFinish_UPDATE ( CIRCLE_GRAPH* graph ); // ��ԏI������ ( GRAPH_STATE_UPDATE )


//-----------------------------------------------------------------------------------------
// �ʑ���
//-----------------------------------------------------------------------------------------
static void AddComponent( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* newComponent ); // �\���v�f��ǉ�����
static void LowerSortComponents( CIRCLE_GRAPH* graph ); // �\���v�f���~���\�[�g����
static void UpdateComponentsPercentage( CIRCLE_GRAPH* graph ); // �e�\���v�f�̊������X�V����
static void UpdateComponentsScope( CIRCLE_GRAPH* graph ); // �e�\���v�f����߂銄���͈̔͂��X�V����
static void UpdateDrawVertices( CIRCLE_GRAPH* graph ); // �`��Ɏg�p���钸�_���X�g���X�V����

//-----------------------------------------------------------------------------------------
// �擾
//-----------------------------------------------------------------------------------------
const GRAPH_COMPONENT* GetComponent( const CIRCLE_GRAPH* graph, int idx ); // �\���v�f
static u32 GetComponentsTotalValue( const CIRCLE_GRAPH* graph ); // �S�\���v�f�̍��v�l
static u32 GetComponentsTotalPercentage( const CIRCLE_GRAPH* graph ); // �S�\���v�f����߂銄���̍��v�l
static void GetComponentCirclePointIndex( const GRAPH_COMPONENT* component, u8* destHeadIdx, u8* destTailIdx ); // �\���v�f�ɊY������O�����_�̃C���f�b�N�X
static GXRgb GetComponentOuterColor( const GRAPH_COMPONENT* component ); // �\���v�f�̊O���̐F
static GXRgb GetComponentCenterColor( const GRAPH_COMPONENT* component ); // �\���v�f�̒��S�̐F

//-----------------------------------------------------------------------------------------
// �������E�����E�j���E����
//-----------------------------------------------------------------------------------------
static CIRCLE_GRAPH* CreateGraph( HEAPID heapID );           // �~�O���t ����
static void InitGraph( CIRCLE_GRAPH* graph, HEAPID heapID ); // �~�O���t ������
static void DeleteGraph( CIRCLE_GRAPH* graph );              // �~�O���t �j��
static void CreateStateQueue( CIRCLE_GRAPH* graph ); // ��ԃL���[ ����
static void DeleteStateQueue( CIRCLE_GRAPH* graph ); // ��ԃL���[ �j��
static void SetupCirclePoints( CIRCLE_GRAPH* graph ); // �O�����_�̍��W ����

//-----------------------------------------------------------------------------------------
// �f�o�b�O
//-----------------------------------------------------------------------------------------
static void DebugPrint_stateQueue( const CIRCLE_GRAPH* graph ); // ��ԃL���[
static void DebugPrint_components( const CIRCLE_GRAPH* graph ); // �\���v�f





//========================================================================================= 
// ���O�����J�֐�
//========================================================================================= 

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t���쐬����
 *
 * @param heapID �g�p����q�[�vID
 *
 * @return �쐬�����~�O���t
 */
//-----------------------------------------------------------------------------------------
CIRCLE_GRAPH* CIRCLE_GRAPH_Create( HEAPID heapID )
{
  CIRCLE_GRAPH* graph;

  // ����
  graph = CreateGraph( heapID );

  // ������
  InitGraph( graph, heapID );
  CreateStateQueue( graph );
  SetupCirclePoints( graph );

  return graph;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
void CIRCLE_GRAPH_Delete( CIRCLE_GRAPH* graph )
{
  DeleteStateQueue( graph );
  DeleteGraph( graph );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\���v�f��ǉ�����
 *
 * @param graph
 * @param data  �ǉ�����\���v�f�̃f�[�^
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t�̃��C������
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
void CIRCLE_GRAPH_Main( CIRCLE_GRAPH* graph )
{
  // ����
  GraphMain( graph );

  // �`��
  SetMatrix( graph ); // �s���ݒ肷��
  DrawGraph( graph ); // �~�O���t��`�悷��
  G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��̓��N�G�X�g�𔭍s����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
void CIRCLE_GRAPH_AnalyzeReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_ANALYZE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\�����N�G�X�g�𔭍s����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
void CIRCLE_GRAPH_ShowReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_SHOW );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �������N�G�X�g�𔭍s����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
void CIRCLE_GRAPH_HideReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_HIDE );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�V���N�G�X�g�𔭍s����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
void CIRCLE_GRAPH_UpdateReq( CIRCLE_GRAPH* graph )
{
  SetNextState( graph, GRAPH_STATE_UPDATE );
}



//=========================================================================================
// �`��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t��`�悷��
 *
 * @param
 */
//-----------------------------------------------------------------------------------------
static void DrawGraph( const CIRCLE_GRAPH* graph )
{
  int drawPolygonNum;
  int drawPolygonCount;
  int firstPolygonIdx;
  int vertexIdx;

  // �|���S���֘A�����l��ݒ�
  G3_PolygonAttr( GX_LIGHTMASK_NONE,        // no lights
                  GX_POLYGONMODE_MODULATE,  // modulation mode
                  GX_CULL_NONE,             // cull none
                  0,                        // polygon ID(0 - 63)
                  31,                       // alpha(0 - 31)
                  0                         // OR of GXPolygonAttrMisc's value
                  );

  // �`��|���S���͈͂�����
  switch( graph->state )
  {
  case GRAPH_STATE_WAIT:
    drawPolygonNum = 0;  // �`�悵�Ȃ�
    break;
  case GRAPH_STATE_ANALYZE: 
    drawPolygonNum  = CIRCLE_DIV_COUNT * graph->stateCount / ANALYZE_FRAMES;  // �|���S����
    firstPolygonIdx = CIRCLE_DIV_COUNT - drawPolygonNum;                      // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_SHOW:    
    drawPolygonNum  = CIRCLE_DIV_COUNT * graph->stateCount / SHOW_FRAMES;    // �|���S����
    firstPolygonIdx = CIRCLE_DIV_COUNT - drawPolygonNum;                     // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_HIDE:    
    drawPolygonNum  = CIRCLE_DIV_COUNT * (HIDE_FRAMES - graph->stateCount) / HIDE_FRAMES; // �|���S����
    firstPolygonIdx = CIRCLE_DIV_COUNT - drawPolygonNum;                                  // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_STAY:    
    drawPolygonNum  = CIRCLE_DIV_COUNT;                      // �|���S����
    firstPolygonIdx = CIRCLE_DIV_COUNT - drawPolygonNum;     // �擪�|���S���C���f�b�N�X
    break;
  case GRAPH_STATE_UPDATE:  
    drawPolygonNum  = CIRCLE_DIV_COUNT * graph->stateCount / UPDATE_FRAMES;  // �|���S����
    firstPolygonIdx = CIRCLE_DIV_COUNT - drawPolygonNum;                     // �擪�|���S���C���f�b�N�X
    break;
  default: 
    GF_ASSERT(0);
  }

  // �擪���_�C���f�b�N�X���Z�o
  vertexIdx = firstPolygonIdx * 3;

  // �`��
  for( drawPolygonCount=0; drawPolygonCount < drawPolygonNum; drawPolygonCount++ )
  {
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �s����Z�b�g����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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
}



//=========================================================================================
// ������
//========================================================================================= 

//-----------------------------------------------------------------------------------------
/**
 * @brief ���C���֐�
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphMain( CIRCLE_GRAPH* graph )
{
  switch( graph->state )
  {
  case GRAPH_STATE_WAIT:    GraphAct_WAIT( graph );    break;
  case GRAPH_STATE_ANALYZE: GraphAct_ANALYZE( graph ); break;
  case GRAPH_STATE_SHOW:    GraphAct_SHOW( graph );    break;
  case GRAPH_STATE_HIDE:    GraphAct_HIDE( graph );    break;
  case GRAPH_STATE_STAY:    GraphAct_STAY( graph );    break;
  case GRAPH_STATE_UPDATE:  GraphAct_UPDATE( graph );  break;
  default: GF_ASSERT(0);
  }

  // ��ԃJ�E���^�X�V
  CountUpStateCount( graph );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_WAIT )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphAct_WAIT( CIRCLE_GRAPH* graph )
{
  // �L���[����łȂ���Ώ�ԑJ�ڂ���
  if( QUEUE_IsEmpty( graph->stateQueue ) == FALSE )
  {
    SwitchState( graph );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_ANALYZE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphAct_ANALYZE( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( ANALYZE_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_STAY );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_SHOW )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphAct_SHOW( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( SHOW_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_STAY );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_HIDE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphAct_HIDE( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( HIDE_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_WAIT );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_STAY )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphAct_STAY( CIRCLE_GRAPH* graph )
{
  // �L���[����łȂ���Ώ�ԑJ�ڂ���
  if( QUEUE_IsEmpty( graph->stateQueue ) == FALSE )
  {
    SwitchState( graph );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃ��C������ ( GRAPH_STATE_UPDATE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphAct_UPDATE( CIRCLE_GRAPH* graph )
{
  // ��莞�Ԍo�߂ŏ�ԑJ�ڂ���
  if( UPDATE_FRAMES <= graph->stateCount )
  {
    ChangeState( graph, GRAPH_STATE_STAY );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃJ�E���^���X�V����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void CountUpStateCount( CIRCLE_GRAPH* graph )
{
  u32 maxCount;

  // �C���N�������g
  graph->stateCount++;

  // �ő�l������
  switch( graph->state )
  {
  case GRAPH_STATE_WAIT:    maxCount = 0xffffffff;     break;
  case GRAPH_STATE_ANALYZE: maxCount = ANALYZE_FRAMES; break;
  case GRAPH_STATE_SHOW:    maxCount = SHOW_FRAMES;    break;
  case GRAPH_STATE_HIDE:    maxCount = HIDE_FRAMES;    break;
  case GRAPH_STATE_STAY:    maxCount = 0xffffffff;     break;
  case GRAPH_STATE_UPDATE:  maxCount = UPDATE_FRAMES;  break;
  default: GF_ASSERT(0);
  }

  // �ő�l�␳
  if( maxCount < graph->stateCount )
  { 
    graph->stateCount = maxCount;
  }
}


//-----------------------------------------------------------------------------------------
/**
 * @brief ���̏�Ԃ��L���[�ɓo�^����
 *
 * @param graph
 * @param nextState �L���[�ɓo�^������
 */
//-----------------------------------------------------------------------------------------
static void SetNextState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState )
{
  // �G���L���[
  QUEUE_EnQueue( graph->stateQueue, nextState );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: set next state\n" );
  DebugPrint_stateQueue( graph );  // �L���[�̏�Ԃ��o��
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�؂�ւ���
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void SwitchState( CIRCLE_GRAPH* graph )
{
  GRAPH_STATE nextState;

  // ��Ԃ��X�V
  nextState = QUEUE_DeQueue( graph->stateQueue );
  ChangeState( graph, nextState );

  // DEBUG:
  DebugPrint_stateQueue( graph );  // �L���[�̏�Ԃ��o��
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param graph
 * @param nextState
 */
//-----------------------------------------------------------------------------------------
static void ChangeState( CIRCLE_GRAPH* graph, GRAPH_STATE nextState )
{
  // ���݂̏�Ԃ̏I������
  switch( graph->state )
  {
  case GRAPH_STATE_WAIT:    GraphFinish_WAIT( graph );    break;
  case GRAPH_STATE_ANALYZE: GraphFinish_ANALYZE( graph ); break;
  case GRAPH_STATE_SHOW:    GraphFinish_SHOW( graph );    break;
  case GRAPH_STATE_HIDE:    GraphFinish_HIDE( graph );    break;
  case GRAPH_STATE_STAY:    GraphFinish_STAY( graph );    break;
  case GRAPH_STATE_UPDATE:  GraphFinish_UPDATE( graph );  break;
  default: GF_ASSERT(0);
  }

  // ��Ԃ��X�V
  graph->state = nextState;
  graph->stateCount = 0;

  // ���̏�Ԃ̏�Ԃ̊J�n����
  switch( nextState )
  {
  case GRAPH_STATE_WAIT:    GraphStart_WAIT( graph );    break;
  case GRAPH_STATE_ANALYZE: GraphStart_ANALYZE( graph ); break;
  case GRAPH_STATE_SHOW:    GraphStart_SHOW( graph );    break;
  case GRAPH_STATE_HIDE:    GraphStart_HIDE( graph );    break;
  case GRAPH_STATE_STAY:    GraphStart_STAY( graph );    break;
  case GRAPH_STATE_UPDATE:  GraphStart_UPDATE( graph );  break;
  default: GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: change state ==> " );
  switch( nextState )
  {
  case GRAPH_STATE_WAIT:    OS_TFPrintf( PRINT_TARGET, "WAIT" );    break;
  case GRAPH_STATE_ANALYZE: OS_TFPrintf( PRINT_TARGET, "ANALYZE" ); break;
  case GRAPH_STATE_SHOW:    OS_TFPrintf( PRINT_TARGET, "SHOW" );    break;
  case GRAPH_STATE_HIDE:    OS_TFPrintf( PRINT_TARGET, "HIDE" );    break;
  case GRAPH_STATE_STAY:    OS_TFPrintf( PRINT_TARGET, "STAY" );    break;
  case GRAPH_STATE_UPDATE:  OS_TFPrintf( PRINT_TARGET, "UPDATE" );  break;
  default: GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_WAIT )
 *
 * @param graph
 */
//----------------------------------------------------------------------------------------- 
static void GraphStart_WAIT( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_ANALYZE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphStart_ANALYZE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state ANALYZE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_SHOW )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphStart_SHOW( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state SHOW\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_HIDE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphStart_HIDE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state HIDE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_STAY )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphStart_STAY( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state STAY\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԊJ�n���� ( GRAPH_STATE_UPDATE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphStart_UPDATE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: start state UPDATE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_WAIT )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------

static void GraphFinish_WAIT( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state WAIT\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_ANALYZE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphFinish_ANALYZE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state ANALYZE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_SHOW )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphFinish_SHOW( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state SHOW\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_HIDE )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphFinish_HIDE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state HIDE\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_STAY )
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphFinish_STAY( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state STAY\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԏI������ ( GRAPH_STATE_UPDATE ) 
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void GraphFinish_UPDATE( CIRCLE_GRAPH* graph )
{
  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: finish state UPDATE\n" );
} 




//=========================================================================================
// ���ʑ���
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �O���t�̍\���v�f��ǉ�����
 *
 * @param graph
 * @param newComponent �ǉ�����\���v�f�̃f�[�^
 */
//-----------------------------------------------------------------------------------------
static void AddComponent( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* newComponent )
{
  // �\���v�f���I�[�o�[�t���[
  GF_ASSERT( graph->componentNum < MAX_COMPONENT_NUM );

  // �ǉ�
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �\���v�f���~���\�[�g����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �e�\���v�f����߂銄�����X�V����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �e�\���v�f����߂銄���͈̔͂��X�V����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �`��Ɏg�p���钸�_���X�g���X�V����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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

    // �\���v�f�̃f�[�^���擾
    component   = GetComponent( graph, componentIdx );
    centerColor = GetComponentCenterColor( component ); // ���S�̐F
    outerColor  = GetComponentOuterColor( component );  // �O���̐F
    GetComponentCirclePointIndex( component, &headPointIdx, &tailPointIdx ); // �\���v�f���܂ފO�����_�͈̔�

    // �\���v�f����߂銄���͈͓̔��ɂ���, ���ׂĂ̊O�����_���\������|���S����ǉ�����
    for( circlePointIdx=headPointIdx; circlePointIdx <= tailPointIdx; circlePointIdx++ )
    {
      // ���_�f�[�^�ǉ�
      graph->vertices[ vertexIdx ].color = centerColor;
      graph->vertices[ vertexIdx ].pos   = graph->centerPos;
      vertexIdx++;

      graph->vertices[ vertexIdx ].color = outerColor;
      VEC_Fx16Add( &graph->centerPos, &graph->circlePoints[ circlePointIdx ], &graph->vertices[ vertexIdx ].pos );
      vertexIdx++;

      graph->vertices[ vertexIdx ].color = outerColor;
      VEC_Fx16Add( &graph->centerPos, &graph->circlePoints[ (circlePointIdx + 1) % CIRCLE_POINT_COUNT ], &graph->vertices[ vertexIdx ].pos );
      vertexIdx++;
    }
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: update draw vertices\n" );
}


//=========================================================================================
// ���擾
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �\���v�f���擾����
 *
 * @param graph
 * @param idx   ���Ԗڂ̍\���v�f���擾���邩
 *
 * @return �w�肵���C���f�b�N�X�̍\���v�f
 */
//-----------------------------------------------------------------------------------------
const GRAPH_COMPONENT* GetComponent( const CIRCLE_GRAPH* graph, int idx )
{
  // �C���f�b�N�X �G���[
  GF_ASSERT( idx < graph->componentNum );

  return &( graph->components[ idx ] );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �S�\���v�f�̍��v�l���v�Z����
 *
 * @param graph
 *
 * @return �S�\���v�f�����f�[�^�̍��v�l
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �S�\���v�f����߂銄���̍��v�l���v�Z����
 *
 * @param graph
 *
 * @return �S�\���v�f����߂銄���̍��v�l
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���\���v�f�ɊY������, �O�����_�̃C���f�b�N�X���擾����
 *
 * @param component    �C���f�b�N�X���擾����\���v�f
 * @param destHeadIdx  �擾�����擪�C���f�b�N�X�̊i�[��
 * @param destTailIdx  �擾���������C���f�b�N�X�̊i�[��
 */
//-----------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̊O���̐F���擾����
 *
 * @param component �F���擾����\���v�f
 *
 * @return �w�肵���\���v�f�̕\���J���[
 */
//-----------------------------------------------------------------------------------------
static GXRgb GetComponentOuterColor( const GRAPH_COMPONENT* component )
{
  return GX_RGB( component->outerColorR, component->outerColorG, component->outerColorB );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �\���v�f�̒��S�̐F���擾����
 *
 * @param component �F���擾����\���v�f
 *
 * @return �w�肵���\���v�f�̕\���J���[
 */
//-----------------------------------------------------------------------------------------
static GXRgb GetComponentCenterColor( const GRAPH_COMPONENT* component )
{
  return GX_RGB( component->centerColorR, component->centerColorG, component->centerColorB );
}



//=========================================================================================
// ���������E�����E�j���E����
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t�𐶐�����
 *
 * @param graph
 * @param heapID �g�p����q�[�vID
 */
//-----------------------------------------------------------------------------------------
static CIRCLE_GRAPH* CreateGraph( HEAPID heapID )
{
  CIRCLE_GRAPH* graph;

  graph = GFL_HEAP_AllocMemory( heapID, sizeof(CIRCLE_GRAPH) );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: create graph\n" );

  return graph;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t������������
 *
 * @param graph
 * @param heapID �g�p����q�[�vID
 */
//-----------------------------------------------------------------------------------------
static void InitGraph( CIRCLE_GRAPH* graph, HEAPID heapID )
{
  graph->heapID       = heapID;
  graph->state        = GRAPH_STATE_WAIT;
  graph->stateQueue   = NULL;
  graph->radius       = CIRCLE_RADIUS;
  graph->componentNum = 0;

  VEC_Fx16Set( &(graph->centerPos), CIRCLE_CENTER_X, CIRCLE_CENTER_Y, 0 );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: init graph\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �~�O���t��j������
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void DeleteGraph( CIRCLE_GRAPH* graph )
{
  GFL_HEAP_FreeMemory( graph );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: delete graph\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[�𐶐�����
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void CreateStateQueue( CIRCLE_GRAPH* graph )
{
  // ���d����
  GF_ASSERT( graph->stateQueue == NULL );

  graph->stateQueue = QUEUE_Create( STATE_QUEUE_SIZE, graph->heapID );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: create state queueh\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[��j������
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void DeleteStateQueue( CIRCLE_GRAPH* graph )
{
  // ������
  GF_ASSERT( graph->stateQueue );

  QUEUE_Delete( graph->stateQueue );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: delete state queue\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �O�����_�̍��W����������
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
static void SetupCirclePoints( CIRCLE_GRAPH* graph )
{
  int pointIdx;
  int pointNum;
  float radius;

  radius   = FX_FX32_TO_F32( graph->radius );
  pointNum = CIRCLE_POINT_COUNT;

  for( pointIdx=0; pointIdx < pointNum; pointIdx++ )
  {
    float radian;
    float x, y, z;

    radian = 2.0f * PI * pointIdx / (float)pointNum;
    radian = 0.5f * PI - radian;
    x = radius * cosf(radian);
    y = radius * sinf(radian);
    z = 0;

    VEC_Fx16Set( &(graph->circlePoints[ pointIdx ]), 
                 FX_F32_TO_FX16(x), FX_F32_TO_FX16(y), FX_F32_TO_FX16(z)  );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "CIRCLE-GRAPH: setup circle points\n" );
}



//=========================================================================================
// ���f�o�b�O
//========================================================================================= 

//-----------------------------------------------------------------------------------------
/**
 * @brief ��ԃL���[�̓��e���o�͂���
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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

    switch( data )
    {
    case GRAPH_STATE_WAIT:    OS_TFPrintf( PRINT_TARGET, "WAIT ");    break;
    case GRAPH_STATE_ANALYZE: OS_TFPrintf( PRINT_TARGET, "ANALYZE "); break;
    case GRAPH_STATE_SHOW:    OS_TFPrintf( PRINT_TARGET, "SHOW ");    break;
    case GRAPH_STATE_HIDE:    OS_TFPrintf( PRINT_TARGET, "HIDE ");    break;
    case GRAPH_STATE_STAY:    OS_TFPrintf( PRINT_TARGET, "STAY ");    break;
    case GRAPH_STATE_UPDATE:  OS_TFPrintf( PRINT_TARGET, "UPDATE ");  break;
    default:                  OS_TFPrintf( PRINT_TARGET, "UNKNOWN "); break;
    }
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ׂĂ̍\���v�f���o�͂���
 *
 * @param graph
 */
//-----------------------------------------------------------------------------------------
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

    component = GetComponent( graph, idx );

    // �f�[�^�o��
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
