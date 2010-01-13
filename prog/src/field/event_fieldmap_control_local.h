//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_fieldmap_control_local.h
 *  @brief	�C�x���g�F�t�B�[���h�}�b�v����c�[��  event_mapchange�p�֐�
 *	@author	tomoya takahashi
 *	@date		2010.01.12
 *
 *	���W���[�����F
 *
 *	event_mapchange��
 *	field_proc�����삷��O�Ɏg�p����t�B�[���h�p�̃\�[�X��
 *	�풓����͂������߂ɁA�wFIELD_INIT�x�I�[�o�[���C���쐬�����B
 *
 *	gamesystem �w�풓�x
 *	  event_mapchange �wFIELD_INIT�x
 *	    field_proc      �wFIELDMAP�x
 *
 *	�ʏ��EVENT_FieldOpen�AEVENT_FieldClose��FIELD_INIT��FIELDMAP������
 *	�ǂݍ��݁A�j�����s�����A
 *  event_mapchange�ł́A�wFIELDMAP�x�݂̂̓ǂݍ��ݔj���ł悢�̂ŁA
 *  ��p�̊֐���p�ӂ���B
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//============================================================================================
// event_mapchange�������FieldOpen Close
//============================================================================================
extern GMEVENT * EVENT_FieldOpen_FieldProcOnly(GAMESYS_WORK *gsys);
extern GMEVENT * EVENT_FieldClose_FieldProcOnly(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap);


#ifdef _cplusplus
}	// extern "C"{
#endif



