//============================================================================================
/**
 * @file	dlplay_parent_sample.c
 * @brief	�_�E�����[�h�v���C�T���v��
 * @author	ariizumi
 * @date	2008.10.8
 */
//============================================================================================

#ifndef DLPLAY_PARENT_SAMPLE_H__
#define DLPLAY_PARENT_SAMPLE_H__

//BG�ʒ�`
#define DLPLAY_MSG_PLANE			(GFL_BG_FRAME3_S)
#define DLPLAY_MSG_PLANE_PRI		(0)
#define DLPLAY_FONT_MSG_PLANE		(GFL_BG_FRAME3_M)
#define DLPLAY_FONT_MSG_PLANE_PRI	(0)
#define DLPLAY_MSGWIN_PLANE			(GFL_BG_FRAME2_M)
#define DLPLAY_MSGWIN_PLANE_PRI		(3)

enum DLPLAY_PARENT_MODE
{
	DPM_SEND_IMAGE,	//�q�@�ɃC���[�W��z�z����
	DPM_SELECT_BOX,	//���{�b�N�X��I������
	DPM_ERROR,		//�Ƃ肠�����G���[
	DPM_END,		//�I���p
};

#endif //DLPLAY_PARENT_SAMPLE_H__


