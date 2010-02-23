//============================================================================
/**
 *
 *	@file		demo3d_engine.h
 *	@brief  3D�f���Đ��G���W��
 *	@author	hosaka genya
 *	@data		2009.12.08
 *
 */
//============================================================================
#pragma once

typedef struct _DEMO3D_ENGINE_WORK DEMO3D_ENGINE_WORK;

#ifdef PM_DEBUG

// @TODO �W�F�[���X�̐Ȃ����f�o�b�O����
#if DEBUG_ONLY_FOR_james_turner

#define DEBUG_USE_KEY // �f�o�b�O�p�̃L�[������g���t���O�B��{�I�Ɂ��̋@�\���g���ꍇ�͕K�{
#define DEBUG_CAMERA_DISP_OFFSET_CONTROL // 2��ʂ̃I�t�Z�b�g�𑀍�
//#define DEBUG_CAMERA_CONTROL // 2��ʎ��̃J��������f�o�b�O

#endif // DEBUG_ONLY_FOR_james_turner

#if DEBUG_ONLY_FOR_iwasawa

#define DEBUG_USE_KEY // �f�o�b�O�p�̃L�[������g���t���O�B��{�I�Ɂ��̋@�\���g���ꍇ�͕K�{
#define DEBUG_CAMERA_DISP_OFFSET_CONTROL // 2��ʂ̃I�t�Z�b�g�𑀍�
//#define DEBUG_CAMERA_CONTROL // 2��ʎ��̃J��������f�o�b�O

#endif // DEBUG_ONLY_FOR_iwasawa

#endif //PM_DEBUG

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N ������
 *
 *	@param	DEMO3D_GRAPHIC_WORK* graphic  �O���t�B�b�N���[�N
 *	@param	demo_id �f��ID
 *	@param	start_frame �����t���[���l(1SYNC=1)
 *	@param	heapID �q�[�vID
 *
 *	@retval DEMO3D_ENGINE_WORK* ���[�N
 */
//-----------------------------------------------------------------------------
extern DEMO3D_ENGINE_WORK* Demo3D_ENGINE_Init( DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ID demo_id, u32 start_frame, HEAPID heapID );
//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N �J��
 *
 *	@param	DEMO3D_ENGINE_WORK* wk  ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
extern void Demo3D_ENGINE_Exit( DEMO3D_ENGINE_WORK* wk );
//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N �又��
 *
 *	@param	DEMO3D_ENGINE_WORK* wk ���[�N
 *
 *	@retval TRUE : �I��
 */
//-----------------------------------------------------------------------------
extern BOOL Demo3D_ENGINE_Main( DEMO3D_ENGINE_WORK* wk );
//-----------------------------------------------------------------------------
/**
 *	@brief  ���݂̃t���[���l���擾
 *
 *	@param	DEMO3D_ENGINE_WORK* wk �@���[�N
 *
 *	@retval �t�[�����l
 */
//-----------------------------------------------------------------------------
extern fx32 DEMO3D_ENGINE_GetNowFrame( const DEMO3D_ENGINE_WORK* wk );
