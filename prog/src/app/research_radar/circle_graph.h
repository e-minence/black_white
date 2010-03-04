///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �~�O���t
 * @file   circle_graph.h
 * @author obata
 * @date   2010.2.21
 */
/////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once 
#include <gflib.h>


//=========================================================================================
// ���~�O���t�̕s���S�`
//=========================================================================================
typedef struct _CIRCLE_GRAPH CIRCLE_GRAPH;


//=========================================================================================
// ���~�O���t�̍\���v�f�ǉ��f�[�^
//=========================================================================================
typedef struct
{
  u8 ID;           // ID
  u8 value;        // �l
  u8 outerColorR;  // �O���̐F(R) [0, 31]
  u8 outerColorG;  // �O���̐F(G) [0, 31]
  u8 outerColorB;  // �O���̐F(B) [0, 31]
  u8 centerColorR; // ���S�_�̐F(R) [0, 31]
  u8 centerColorG; // ���S�_�̐F(G) [0, 31]
  u8 centerColorB; // ���S�_�̐F(B) [0, 31]

} GRAPH_COMPONENT_ADD_DATA;


//=========================================================================================
// ���O���t�̐����E�j��
//=========================================================================================
extern CIRCLE_GRAPH* CIRCLE_GRAPH_Create( HEAPID heapID ); // �C���X�^���X�𐶐�����
extern void CIRCLE_GRAPH_Delete( CIRCLE_GRAPH* graph );    // �C���X�^���X��j������


//=========================================================================================
// ���\���v�f�̐ݒ�
//=========================================================================================
// �\���v�f���Z�b�g����
extern void CIRCLE_GRAPH_SetupComponents( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data, u8 dataNum );
// �\���v�f��ǉ�����
extern void CIRCLE_GRAPH_AddComponent( CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data );


//=========================================================================================
// ������
//=========================================================================================
extern void CIRCLE_GRAPH_Main( CIRCLE_GRAPH* graph ); // ���C������
extern void CIRCLE_GRAPH_Draw( const CIRCLE_GRAPH* graph ); // �`��

extern void CIRCLE_GRAPH_AnalyzeReq( CIRCLE_GRAPH* graph );   // ��̓��N�G�X�g
extern void CIRCLE_GRAPH_AppearReq( CIRCLE_GRAPH* graph );    // �\�����N�G�X�g
extern void CIRCLE_GRAPH_DisappearReq( CIRCLE_GRAPH* graph ); // �������N�G�X�g
extern void CIRCLE_GRAPH_UpdateReq( CIRCLE_GRAPH* graph );    // �X�V���N�G�X�g

extern void CIRCLE_GRAPH_StopGraph( CIRCLE_GRAPH* graph, u32 frames ); // ������~������
extern void CIRCLE_GRAPH_SetDrawEnable( CIRCLE_GRAPH* graph, BOOL enable ); // �\���E��\����ύX����
extern void CIRCLE_GRAPH_SetCenterPos( CIRCLE_GRAPH* graph, const VecFx16* pos ); // ���S�_�̍��W��ݒ肷�� 


//=========================================================================================
// ���擾�E����
//=========================================================================================
// �\���v�f�̐����擾����
extern u8 CIRCLE_GRAPH_GetComponentNum( const CIRCLE_GRAPH* graph );
// �\���v�f�̒l���擾���� ( �\���v�fID���w�� )
extern u32 CIRCLE_GRAPH_GetComponentValue_byID( const CIRCLE_GRAPH* graph, u8 componentID );
// �\���v�f�̒l���擾���� ( �����N���w�� )
extern u32 CIRCLE_GRAPH_GetComponentValue_byRank( const CIRCLE_GRAPH* graph, u8 rank );
// �\���v�f����ʉ��ʂȂ̂����擾���� ( �\���v�fID���w�� )
extern u8 CIRCLE_GRAPH_GetComponentRank_byID( const CIRCLE_GRAPH* graph, u8 componentID );
// �w�肵�������N�̍\���v�f��ID���擾���� ( �����N���w�� )
extern u8 CIRCLE_GRAPH_GetComponentID_byRank( const CIRCLE_GRAPH* graph, u8 rank );
// �\���v�f����߂�, �O���t���̊������擾���� ( �\���v�fID���w�� )
extern u8 CIRCLE_GRAPH_GetComponentPercentage_byID( const CIRCLE_GRAPH* graph, u8 componentID );
// �\���v�f����߂�, �O���t���̊������擾���� ( �����N���w�� )
extern u8 CIRCLE_GRAPH_GetComponentPercentage_byRank( const CIRCLE_GRAPH* graph, u8 rank );
// ��󂪎w�������ׂ����W���擾���� ( �\���v�fID���w�� )
extern void CIRCLE_GRAPH_GetComponentPointPos_byID( const CIRCLE_GRAPH* graph, u8 componentID, int* destX, int* destY );
// ��󂪎w�������ׂ����W���擾���� ( �����N���w�� )
extern void CIRCLE_GRAPH_GetComponentPointPos_byRank( const CIRCLE_GRAPH* graph, u8 rank, int* destX, int* destY );
// �A�j���[�V���������ǂ����𔻒肷��
extern BOOL CIRCLE_GRAPH_IsAnime( const CIRCLE_GRAPH* graph );
