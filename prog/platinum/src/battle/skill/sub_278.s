
//============================================================================
/**
 *
 *@file		sub_278.s
 *@brief	�퓬�V�[�P���X
 *			�搧�n�̃A�C�e�����ʔ����`�F�b�N
 *@author	HisashiSogabe
 *@data		2006.06.22
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_278:
//2vs2��̂Ƃ��ɁA��ɂS�̈ȓ��ꍇ�͂��̃`�F�b�N�ł͂��߂Ȃ̂ŁA�v���`�i�ŏC��
//	VALUE_WORK			VAL_GET,BUF_PARA_CLIENT_SET_MAX,BUF_PARA_TEMP_WORK
//	VALUE				VAL_SUB,BUF_PARA_TEMP_WORK,1

//�v���`�i�ł̏C���o�[�W����
	VALUE_WORK			VAL_SET,BUF_PARA_CALC_WORK,BUF_PARA_CLIENT_WORK		//CLIENT_WORK��ޔ�
	VALUE				VAL_SET,BUF_PARA_CLIENT_WORK,0
	VALUE				VAL_SET,BUF_PARA_TEMP_WORK,0

	//HP�̂���CLIENT���J�E���g
SUB_278_CHECK_LOOP:
	IF_PSP				IF_FLAG_EQ,SIDE_WORK,ID_PSP_hp,0,SUB_278_CHECK_NEXT
	VALUE				VAL_ADD,BUF_PARA_TEMP_WORK,1
SUB_278_CHECK_NEXT:
	VALUE				VAL_ADD,BUF_PARA_CLIENT_WORK,1
	CLIENT_SET_MAX_LOOP	BUF_PARA_CLIENT_WORK,SUB_278_CHECK_LOOP	

	VALUE				VAL_SUB,BUF_PARA_TEMP_WORK,1
	VALUE_WORK			VAL_SET,BUF_PARA_CLIENT_WORK,BUF_PARA_CALC_WORK		//CLIENT_WORK�𕜋A

	//�Ō��Client�̓`�F�b�N���Ȃ�
	IF_WORK				IF_FLAG_NC,BUF_PARA_TEMP_WORK,BUF_PARA_AGI_CNT,SUB_278_END

	IF_PSP				IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_wkw_sensei_flag,1,SUB_278_NEXT
	IF_PSP				IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_wkw_once_agi_up,0,SUB_278_END
SUB_278_NEXT:
	STATUS_EFFECT		SIDE_ATTACK,STATUS_ITEM_POKE
	SERVER_WAIT
	IF_PSP				IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_wkw_once_agi_up,1,SUB_278_ONCE_AGI_UP
	PSP_VALUE			VAL_SET,SIDE_ATTACK,ID_PSP_wkw_sensei_flag,0
	BRANCH				SUB_278_END
SUB_278_ONCE_AGI_UP:
	MESSAGE				ItemSenseiMineMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_ATTACK
	SERVER_WAIT
	WAIT				MSG_WAIT
	PSP_VALUE			VAL_SET,SIDE_ATTACK,ID_PSP_wkw_once_agi_up,0
	KILL_ITEM			SIDE_ATTACK
SUB_278_END:
	SEQ_END
