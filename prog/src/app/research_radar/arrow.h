///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ���
 * @file   arrow.h
 * @author obata
 * @date   2010.02.22
 */
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>


//=====================================================================================
// �����̕s���S�^
//=====================================================================================
typedef struct _ARROW ARROW;


//=====================================================================================
// ���\���p�����[�^
//=====================================================================================
typedef struct {
  u32 cgrIndex;      // OBJ�L�����f�[�^�̓o�^�C���f�b�N�X
  u32 plttIndex;     // OBJ�p���b�g�f�[�^�̓o�^�C���f�b�N�X
  u32 cellAnimIndex; // OBJ�Z���A�j���f�[�^�̓o�^�C���f�b�N�X
  u16 setSerface;    // �T�[�t�F�C�X�w�� ( CLSYS_DEFREND_MAIN/ CLSYS_DEFREND_SUB )
  u16 anmseqH;       // �A�j���[�V�����V�[�P���X ( ���ɐL�т� )
  u16 anmseqV;       // �A�j���[�V�����V�[�P���X ( �c�ɐL�т� )
  u16 anmseqCorner;  // �A�j���[�V�����V�[�P���X ( �R�[�i�[ )
  u16 anmseqEnd;     // �A�j���[�V�����V�[�P���X ( �I�_ )
} ARROW_DISP_PARAM;



//=====================================================================================
// �������E�j��
//=====================================================================================
extern ARROW* ARROW_Create( HEAPID heapID, const ARROW_DISP_PARAM* dispParam );
extern void ARROW_Delete( ARROW* arrow ); 

//=====================================================================================
// ������
//=====================================================================================
extern void ARROW_Main( ARROW* arrow );
extern void ARROW_Setup( ARROW* arrow, int startX, int startY, int endX, int endY );
extern void ARROW_StartAnime( ARROW* arrow );
extern void ARROW_Vanish( ARROW* arrow );
