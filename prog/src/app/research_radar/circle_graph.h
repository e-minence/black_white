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
  u8 value;        // �l
  u8 outerColorR;  // �O���̐F(R) [0, 31]
  u8 outerColorG;  // �O���̐F(G) [0, 31]
  u8 outerColorB;  // �O���̐F(B) [0, 31]
  u8 centerColorR; // ���S�_�̐F(R) [0, 31]
  u8 centerColorG; // ���S�_�̐F(G) [0, 31]
  u8 centerColorB; // ���S�_�̐F(B) [0, 31]

} GRAPH_COMPONENT_ADD_DATA;


//=========================================================================================
// �������E�j��
//=========================================================================================
extern CIRCLE_GRAPH* CIRCLE_GRAPH_Create( HEAPID heapID ); // ����
extern void CIRCLE_GRAPH_Delete( CIRCLE_GRAPH* graph );    // �j��

extern void CIRCLE_GRAPH_AddComponent( 
    CIRCLE_GRAPH* graph, const GRAPH_COMPONENT_ADD_DATA* data ); // �\���v�f�ǉ�

//=========================================================================================
// ������
//=========================================================================================
extern void CIRCLE_GRAPH_Main( CIRCLE_GRAPH* graph ); // ���C������

extern void CIRCLE_GRAPH_AnalyzeReq( CIRCLE_GRAPH* graph ); // ��̓��N�G�X�g
extern void CIRCLE_GRAPH_ShowReq( CIRCLE_GRAPH* graph );    // �\�����N�G�X�g
extern void CIRCLE_GRAPH_HideReq( CIRCLE_GRAPH* graph );    // �������N�G�X�g
extern void CIRCLE_GRAPH_UpdateReq( CIRCLE_GRAPH* graph );  // �X�V���N�G�X�g
