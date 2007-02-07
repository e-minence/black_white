
//============================================================================================
/**
 * @file	yt_common.h
 * @brief	DS�Ń��b�V�[�̂��܂�common�w�b�_�[
 * @date	2007.02.06
 */
//============================================================================================

#ifndef	__YT_COMMON_H__
#define	__YT_COMMON_H__

//�W���u�i���o�[��`
enum{
	YT_InitTitleNo=0,
	YT_MainTitleNo,
	YT_InitGameNo,
	YT_MainGameNo,
};

//�Q�[���p�����[�^�\���̐錾
typedef	struct
{
	int		job_no;			//�W���u�i���o�[
	TCBSYS	*tcbsys;
	void	*tcb_work;
	TCB		*tcb[3];
}GAME_PARAM;

#endif	__YT_COMMON_H__
