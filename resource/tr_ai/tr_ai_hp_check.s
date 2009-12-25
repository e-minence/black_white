//============================================================================
/**
 *@file		tr_ai_hp_check.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	HP�Ŕ��f
//========================================================
HpCheckAISeq:
	IF_MIKATA_ATTACK	HpCheckAI_end			//�Ώۂ������Ȃ�I��

	IF_HP_OVER	CHECK_ATTACK,70,HpCheckAI_my70
	IF_HP_OVER	CHECK_ATTACK,30,HpCheckAI_my30

	CHECK_WAZASEQNO
	IF_TABLE_JUMP	HpCheckAI_my00_ng_tbl,HpCheckAI_my_ng
	JUMP		HpCheckAI_ene

HpCheckAI_my70:
	CHECK_WAZASEQNO
	IF_TABLE_JUMP	HpCheckAI_my70_ng_tbl,HpCheckAI_my_ng
	JUMP		HpCheckAI_ene

HpCheckAI_my30:
	CHECK_WAZASEQNO
	IF_TABLE_JUMP	HpCheckAI_my30_ng_tbl,HpCheckAI_my_ng
	JUMP		HpCheckAI_ene

HpCheckAI_my_ng:
	IF_RND_UNDER	50,HpCheckAI_ene
	INCDEC		-2

HpCheckAI_ene:
	IF_HP_OVER	CHECK_DEFENCE,70,HpCheckAI_ene70
	IF_HP_OVER	CHECK_DEFENCE,30,HpCheckAI_ene30

	//30����
	CHECK_WAZASEQNO
	IF_TABLE_JUMP	HpCheckAI_ene00_ng_tbl,HpCheckAI_ene_ng
	JUMP		HpCheckAI_end

HpCheckAI_ene70://70�ȏ�
	CHECK_WAZASEQNO
	IF_TABLE_JUMP	HpCheckAI_ene70_ng_tbl,HpCheckAI_ene_ng
	JUMP		HpCheckAI_end

HpCheckAI_ene30://30�ȏ�
	CHECK_WAZASEQNO
	IF_TABLE_JUMP	HpCheckAI_ene30_ng_tbl,HpCheckAI_ene_ng
	JUMP		HpCheckAI_end

HpCheckAI_ene_ng:
	IF_RND_UNDER	50,HpCheckAI_end
	INCDEC		-2


HpCheckAI_end:
	AIEND



HpCheckAI_my70_ng_tbl://71�ȏ゠��Ƃ���
	.long	7,32,37,98,99,116,132
	.long	168,194
	.long	214,220,270
	.long	0xffffffff
//------------------------------------
//�e�[�u������2006.6.16
//133,134,157���g�p�Ȃ̂ō폜
//204�g�p���Ă����Ȃ��̂ō폜
//214,220,270�c�o���ǉ�
//------------------------------------


HpCheckAI_my30_ng_tbl://31�ȏ�70����
	.long	7,10,11,12,13,14,15,16,18,19
	.long	20,21,22,23,24,26,30,35,46,47
	.long	50,51,52,53,54,55,56,58,59,60
	.long	61,62,63,64,93,124,142,205,206,208
	.long	211,212
	.long	240,243,244,265
	.long	0xffffffff
//------------------------------------
//�e�[�u������2006.6.16
//240,243,244,265�c�o���ǉ�
//------------------------------------


HpCheckAI_my00_ng_tbl://30�ȉ�
	.long	10,11,12,13,14,15,16,18,19,20
	.long	21,22,23,24,26,30,35,46,47,50
	.long	51,52,53,54,55,56,58,59,60,61
	.long	62,63,64,81,93,94,124,142,143,144
	.long	190,205,206,208,211,212
	.long	201,210			//�ǉ�2006.6.16
	.long	226,227,265		//�ǉ�2006.6.16
	.long	0xffffffff

//------------------------------------
//�e�[�u������2006.6.16
//151�\�[���[�r�[���g�p���Ă����Ȃ��̂ō폜
//201,210		�ǉ�2006.6.16
//226,227,265	DP�ǉ�2006.6.16
//------------------------------------

HpCheckAI_ene70_ng_tbl://71�ȏ�
	.long	0xffffffff

HpCheckAI_ene30_ng_tbl://31�ȏ�71����
	.long	10,11,12,13,14,15,16,18,19,20
	.long	21,22,23,24,46,47,50,51,52,53
	.long	54,55,56,58,59,60,61,62,63,64
	.long	66,91,114,124,205,206,208,211,212
	.long	226,237,265	
	.long	0xffffffff
//------------------------------------
//�e�[�u������2006.6.16
//226,227,265	DP�ǉ�2006.6.16
//------------------------------------


HpCheckAI_ene00_ng_tbl://31����
	.long	1,7,10,11,12,13,14,15,16,18
	.long	19,20,21,22,23,24,26,30,33,35
	.long	38,40,40,46,47,49,50,51,52,53
	.long	54,55,56,58,59,60,61,62,63,64
	.long	66,67,91,93,94,100,114,118,119,120
	.long	124,143,144,167,205,206,208,211,212
	.long	226,237,265	
	.long	0xffffffff

//------------------------------------
//�e�[�u������2006.6.16
//226,227,265	DP�ǉ�2006.6.16
//------------------------------------

