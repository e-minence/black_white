//////////////////////////////////////////////////////////////////////////////
/**
 * @brief  1�̃p�[�Z���g�\���������\����
 * @file   percentage.h
 * @author obata
 * @date   2010.03.01
 */
//////////////////////////////////////////////////////////////////////////////
#include <gflib.h>


//============================================================================
// ���p�[�Z���e�[�W�̕s���S�^
//============================================================================
typedef struct _PERCENTAGE PERCENTAGE;


//============================================================================
// ���\���p�����[�^
//============================================================================
typedef struct {
  u32 cgrIndex;      // OBJ�L�����f�[�^�̓o�^�C���f�b�N�X
  u32 plttIndex;     // OBJ�p���b�g�f�[�^�̓o�^�C���f�b�N�X
  u32 cellAnimIndex; // OBJ�Z���A�j���f�[�^�̓o�^�C���f�b�N�X
  u16 setSerface;    // �T�[�t�F�C�X�w�� ( CLSYS_DEFREND_MAIN/ CLSYS_DEFREND_SUB )
  u16 anmseqNumber;  // �A�j���[�V�����V�[�P���X ( ���l )
  u16 anmseqBase;    // �A�j���[�V�����V�[�P���X ( �y�� )
} PERCENTAGE_DISP_PARAM;


//============================================================================
// �������E�j��
//============================================================================
extern PERCENTAGE* PERCENTAGE_Create( 
    HEAPID heapID, const PERCENTAGE_DISP_PARAM* dispParam, GFL_CLUNIT* clactUnit ); // ����
extern void PERCENTAGE_Delete( PERCENTAGE* percentage ); // �j��

//============================================================================
// ������
//============================================================================
extern void PERCENTAGE_SetPos( PERCENTAGE* percentage, int x, int y ); // ���W��ݒ肷��
extern void PERCENTAGE_SetValue( PERCENTAGE* percentage, int value ); // ���l��ݒ肷��
extern void PERCENTAGE_SetDrawEnable( PERCENTAGE* percentage, BOOL enable ); // �`���Ԃ̕ύX
