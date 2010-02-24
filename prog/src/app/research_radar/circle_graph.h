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
extern void CIRCLE_GRAPH_SetupComponents(
    CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data, u8 dataNum ); // �\���v�f���Z�b�g����

extern void CIRCLE_GRAPH_AddComponent( 
    CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data ); // �\���v�f��ǉ�����


//=========================================================================================
// ������
//=========================================================================================
extern void CIRCLE_GRAPH_Main( CIRCLE_GRAPH* graph ); // ���C������

extern void CIRCLE_GRAPH_StopGraph( CIRCLE_GRAPH* graph, u32 frames ); // ������~������

extern void CIRCLE_GRAPH_AnalyzeReq( CIRCLE_GRAPH* graph );   // ��̓��N�G�X�g
extern void CIRCLE_GRAPH_AppearReq( CIRCLE_GRAPH* graph );    // �\�����N�G�X�g
extern void CIRCLE_GRAPH_DisappearReq( CIRCLE_GRAPH* graph ); // �������N�G�X�g
extern void CIRCLE_GRAPH_UpdateReq( CIRCLE_GRAPH* graph );    // �X�V���N�G�X�g

extern void CIRCLE_GRAPH_SetDrawEnable( CIRCLE_GRAPH* graph, BOOL enable ); // �\���E��\����ύX����
extern void CIRCLE_GRAPH_SetCenterPos( CIRCLE_GRAPH* graph, const VecFx16* pos ); // ���S�_�̍��W��ݒ肷�� 

extern u32 CIRCLE_GRAPH_GetComponentValue( const CIRCLE_GRAPH* graph, u8 componentID ); // �\���v�f�̒l���擾����
extern u8 CIRCLE_GRAPH_GetComponentRank( const CIRCLE_GRAPH* graph, u8 componentID ); // �\���v�f����ʉ��ʂȂ̂����擾����
extern u8 CIRCLE_GRAPH_GetComponentPercentage( const CIRCLE_GRAPH* graph, u8 componentID ); // �\���v�f����߂�, �O���t���̊������擾����
