//============================================================================
/**
 *@file		tr_ai_double.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�_�u���o�g��
//========================================================
DoubleAISeq:
	IF_MIKATA_ATTACK	DoubleAI_MineSeq

//--------------------------------------
//	�G���Ώ�
//--------------------------------------
	COMP_POWER	LOSS_CALC_OFF
	IF_EQUAL	COMP_POWER_NONE, DoubleAI_StrongAI_end	//�З͂Ȃ��̋Z
	
	//�G�Q�̂���̂ő����̈������͔�����
	//�����������O�{�ȊO�֌W�Ȃ��Œ�Z�͑����`�F�b�N���X���[����
	IF_WAZA_SEQNO_JUMP	38,DoubleAI_StrongAI_START	//	���������Ђ�����
	IF_WAZA_SEQNO_JUMP	41,DoubleAI_StrongAI_START	//�@��イ�̂�����
	IF_WAZA_SEQNO_JUMP	87,DoubleAI_StrongAI_START	//�@�i�C�g�w�b�h
	IF_WAZA_SEQNO_JUMP	88,DoubleAI_StrongAI_START	//�@�T�C�R�E�G�[�u
	IF_WAZA_SEQNO_JUMP	130,DoubleAI_StrongAI_START	//�@�\�j�b�N�u�[��
	
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,DoubleAI_Aisyou_Dec1	//1/2
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,DoubleAI_Aisyou_Dec2	//1/4
	JUMP	DoubleAI_StrongAI_START
	
DoubleAI_Aisyou_Dec1://1/2������������ɂ͏o���Â炢�i1/4�̊m���j
	IF_WAZA_HINSHI	LOSS_CALC_OFF,DoubleAI_StrongAI_START	//�m���ɂł���Ȃ�X���[
	IF_HP_EQUAL	CHECK_DEFENCE_FRIEND,0,DoubleAI_StrongAI_START	//���肪�P�̂Ȃ�X���[
	IF_RND_UNDER	64, DoubleAI_StrongAI_START
	INCDEC	-1
	JUMP	DoubleAI_StrongAI_START
	
DoubleAI_Aisyou_Dec2://1/4������������ɂ͏o���Â炢�i1/4�̊m���j
	IF_WAZA_HINSHI	LOSS_CALC_OFF,DoubleAI_StrongAI_START	//�m���ɂł���Ȃ�X���[
	IF_HP_EQUAL	CHECK_DEFENCE_FRIEND,0,DoubleAI_StrongAI_START	//���肪�P�̂Ȃ�X���[
	IF_RND_UNDER	64, DoubleAI_StrongAI_START
	INCDEC	-2
	JUMP	DoubleAI_StrongAI_START

DoubleAI_StrongAI_START:

//�����̋Z���p�[�g�i�[�Ɣ�ׂĈ�ԍ����Z----------------------------------------
	COMP_POWER_WITH_PARTNER	LOSS_CALC_OFF
	IFN_EQUAL	COMP_POWER_TOP, DoubleAI_StrongAICheck_Aisyou	//�����`�F�b�N

DoubleAI_StrongCheck:
	IF_WAZA_SEQNO_JUMP	7,DoubleAI_StrongAI_end		// ����
	IF_WAZA_SEQNO_JUMP	103,DoubleAI_StrongAICheck2	// �搧
	IF_RND_UNDER	128,DoubleAI_StrongAICheck_Aisyou// �����`�F�b�N

	INCDEC		1
	JUMP	DoubleAI_StrongAI_end	//�I��
	
DoubleAI_StrongAICheck2:
	IF_RND_UNDER	50, DoubleAI_StrongAICheck_Aisyou	//�����`�F�b�N
	INCDEC		1
	JUMP	DoubleAI_StrongAI_end	//�I��


//��ԍ����Ȃ��Ă������`�F�b�N���ĂQ�{�ȏ�Ȃ�o���₷������
DoubleAI_StrongAICheck_Aisyou:
	IF_WAZA_SEQNO_JUMP	38,DoubleAI_StrongAI_end	//	���������Ђ�����
	IF_WAZA_SEQNO_JUMP	41,DoubleAI_StrongAI_end	//�@��イ�̂�����
	IF_WAZA_SEQNO_JUMP	87,DoubleAI_StrongAI_end	//�@�i�C�g�w�b�h
	IF_WAZA_SEQNO_JUMP	88,DoubleAI_StrongAI_end	//�@�T�C�R�E�G�[�u
	IF_WAZA_SEQNO_JUMP	130,DoubleAI_StrongAI_end	//�@�\�j�b�N�u�[��
	
	CHECK_WAZA_AISYOU	AISYOU_2BAI,DoubleAI_StrongAICheck_Aisyou2	//2
	CHECK_WAZA_AISYOU	AISYOU_4BAI,DoubleAI_StrongAICheck_Aisyou4	//4
	JUMP	DoubleAI_StrongAI_end	//�I��

DoubleAI_StrongAICheck_Aisyou2://��60%
	IF_RND_UNDER	100, DoubleAI_StrongAI_end
	INCDEC		1
	JUMP	DoubleAI_StrongAI_end

DoubleAI_StrongAICheck_Aisyou4://75%
	IF_RND_UNDER	64, DoubleAI_StrongAI_end
	INCDEC		1
	JUMP	DoubleAI_StrongAI_end



DoubleAI_StrongAI_end:


//--------------------------------------------------------------------------------

	IF_WAZANO	WAZANO_SUKIRUSUWAPPU,DoubleEnemyAI_SkillSwap	// ��ٽܯ��2006.6.14ok
	CHECK_TYPE	CHECK_WAZA
	IF_WAZANO	WAZANO_ZISIN, DoubleEnemyAI_Zisin				// �n�k2006.6.14ok
	IF_WAZANO	WAZANO_MAGUNITYUUDO, DoubleEnemyAI_Zisin		// �}�O�j�`���[�h2006.6.14ok

	IF_WAZANO	WAZANO_MIRAIYOTI, DoubleEnemyAI_Miraiyoti		// �݂炢�悿2006.6.14ok
	IF_WAZANO	WAZANO_HAMETUNONEGAI, DoubleEnemyAI_Miraiyoti	// �͂߂̂˂���2006.6.14ok


	IF_WAZANO	WAZANO_AMAGOI, DoubleEnemyAI_Amagoi				// ���܂���2006.6.14ok
	IF_WAZANO	WAZANO_NIHONBARE, DoubleEnemyAI_Nihonbare		// �ɂق�΂�2006.6.14ok
	IF_WAZANO	WAZANO_ARARE, DoubleEnemyAI_Arare				// �����2006.6.14ok
	IF_WAZANO	WAZANO_SUNAARASI, DoubleEnemyAI_Sunaarasi		// ���Ȃ��炵2006.6.14ok
	
	IF_WAZANO	WAZANO_ZYUURYOKU, DoubleEnemyAI_Zyuuryoku		// ���イ��傭2006.6.23ok
	IF_WAZANO	WAZANO_TORIKKURUUMU, DoubleEnemyAI_Torikkuruumu	// �g���b�N���[��2006.6.23ok
	IF_WAZANO	WAZANO_KONOYUBITOMARE, DoubleEnemyAI_Konoyubitomare	// ���̂�тƂ܂�2006.6.14ok
	
	CHECK_TYPE	CHECK_WAZA
	IF_EQUAL	POKETYPE_DENKI,DoubleEnemyAI_ElectricType		// �d�C�Z2006.6.14ok
	IF_EQUAL	POKETYPE_HONOO,DoubleEnemyAI_FireType				// ���Z2006.6.14ok
	IF_EQUAL	POKETYPE_MIZU,DoubleEnemyAI_WaterType				// ���Z2006.6.14

	//CHECK_TOKUSEI	CHECK_ATTACK
	//IFN_EQUAL	TOKUSYU_KONZYOU, DoubleEnemyAI_Konzyou	//�ǂ�
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_TEDASUKE, DoubleEnemyAI_Tedasuke	//NG
	
	AIEND

//2006.6.14
//���܂���
DoubleEnemyAI_Amagoi:				
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_URUOIBODHI, DoubleEnemyAI_Amagoi_Mine1
	IF_EQUAL	TOKUSYU_KANSOUHADA, DoubleEnemyAI_Amagoi_Mine2
	JUMP	DoubleEnemyAI_Amagoi_Friend

DoubleEnemyAI_Amagoi_Mine1:
	IFN_COND	CHECK_ATTACK,0x000000ff,DoubleEnemyAI_Amagoi_Friend

DoubleEnemyAI_Amagoi_Mine2:
	INCDEC	2
	JUMP	DoubleEnemyAI_Amagoi_Friend

DoubleEnemyAI_Amagoi_Friend:
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_URUOIBODHI, DoubleEnemyAI_Amagoi_Friend1
	//IF_EQUAL	TOKUSYU_KANSOUHADA, DoubleEnemyAI_Amagoi_Friend2
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_URUOIBODHI
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Amagoi_Friend1
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_KANSOUHADA
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Amagoi_Friend2
	JUMP	DoubleEnemyAI_Amagoi_End

DoubleEnemyAI_Amagoi_Friend1:
	IFN_COND	CHECK_ATTACK_FRIEND,0x000000ff,DoubleEnemyAI_Amagoi_End

DoubleEnemyAI_Amagoi_Friend2:
	INCDEC	2
	JUMP	DoubleEnemyAI_Amagoi_End

DoubleEnemyAI_Amagoi_End:
	AIEND


//�ɂق�΂�	
DoubleEnemyAI_Nihonbare:			
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_RIIHUGAADO, DoubleEnemyAI_Nihonbare_Mine1	//ok
	IF_EQUAL	TOKUSYU_HURAWAAGIHUTO, DoubleEnemyAI_Nihonbare_Mine2//ok
	IF_EQUAL	TOKUSYU_KANSOUHADA, DoubleEnemyAI_Nihonbare_Mine3	//ok
	IF_EQUAL	TOKUSYU_SANPAWAA, DoubleEnemyAI_Nihonbare_Mine4		//ok
	JUMP	DoubleEnemyAI_Nihonbare_Friend

DoubleEnemyAI_Nihonbare_Mine1:
	IF_COND	CHECK_ATTACK,0x000000ff,DoubleEnemyAI_Nihonbare_Friend
	IF_HP_UNDER	CHECK_ATTACK,30,DoubleEnemyAI_Nihonbare_Friend

DoubleEnemyAI_Nihonbare_Mine2:
	INCDEC	2
	JUMP	DoubleEnemyAI_Nihonbare_Friend

DoubleEnemyAI_Nihonbare_Mine3:
	INCDEC	-2
	JUMP	DoubleEnemyAI_Nihonbare_Friend

DoubleEnemyAI_Nihonbare_Mine4:
	IF_HP_UNDER	CHECK_ATTACK,50,DoubleEnemyAI_Nihonbare_Mine4_1
	INCDEC	1

DoubleEnemyAI_Nihonbare_Mine4_1:
	IF_RND_UNDER	128, DoubleEnemyAI_Nihonbare_Friend
	INCDEC	-2

DoubleEnemyAI_Nihonbare_Friend:
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_RIIHUGAADO, DoubleEnemyAI_Nihonbare_Friend1
	//IF_EQUAL	TOKUSYU_HURAWAAGIHUTO, DoubleEnemyAI_Nihonbare_Friend2
	//IF_EQUAL	TOKUSYU_KANSOUHADA, DoubleEnemyAI_Nihonbare_Friend3
	//IF_EQUAL	TOKUSYU_SANPAWAA, DoubleEnemyAI_Nihonbare_Friend4
	
	//�������ڌ�
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_RIIHUGAADO
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Nihonbare_Friend1
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_HURAWAAGIHUTO
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Nihonbare_Friend2
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_KANSOUHADA
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Nihonbare_Friend3
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_SANPAWAA
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Nihonbare_Friend4
	
	JUMP	DoubleEnemyAI_Nihonbare_End

DoubleEnemyAI_Nihonbare_Friend1:
	IF_COND	CHECK_ATTACK_FRIEND,0x000000ff,DoubleEnemyAI_Nihonbare_End
	IF_HP_UNDER	CHECK_ATTACK_FRIEND,30,DoubleEnemyAI_Nihonbare_End

DoubleEnemyAI_Nihonbare_Friend2:
	INCDEC	2
	JUMP	DoubleEnemyAI_Nihonbare_End

DoubleEnemyAI_Nihonbare_Friend3:
	INCDEC	-2
	JUMP	DoubleEnemyAI_Nihonbare_End

DoubleEnemyAI_Nihonbare_Friend4:
	IF_HP_UNDER	CHECK_ATTACK_FRIEND,50,DoubleEnemyAI_Nihonbare_Friend4_1
	INCDEC	1

DoubleEnemyAI_Nihonbare_Friend4_1:
	IF_RND_UNDER	128, DoubleEnemyAI_Nihonbare_End
	INCDEC	-2

DoubleEnemyAI_Nihonbare_End:
	AIEND

//�����
DoubleEnemyAI_Arare:			
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_AISUBODHI, DoubleEnemyAI_Arare_Mine1	//ok
	IF_EQUAL	TOKUSYU_YUKIGAKURE, DoubleEnemyAI_Arare_Mine1	//ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUBUKI, DoubleEnemyAI_Arare_Mine1	//ok
	JUMP	DoubleEnemyAI_Arare_Friend

DoubleEnemyAI_Arare_Mine1:
	INCDEC	+2

DoubleEnemyAI_Arare_Friend:	
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_AISUBODHI, DoubleEnemyAI_Arare_Friend1
	//IF_EQUAL	TOKUSYU_YUKIGAKURE, DoubleEnemyAI_Arare_Friend1
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_AISUBODHI
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Arare_Friend1
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_YUKIGAKURE
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Arare_Friend1
	
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_HUBUKI, DoubleEnemyAI_Arare_Friend1
	
	JUMP	DoubleEnemyAI_Arare_End

DoubleEnemyAI_Arare_Friend1:
	INCDEC	+2

DoubleEnemyAI_Arare_End:	
	AIEND

//���Ȃ��炵
DoubleEnemyAI_Sunaarasi:			
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_SUNAGAKURE, DoubleEnemyAI_Sunaarasi_Mine1
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_IWA, DoubleEnemyAI_Sunaarasi_Mine1
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_IWA, DoubleEnemyAI_Sunaarasi_Mine1
	JUMP	DoubleEnemyAI_Sunaarasi_Friend

	
DoubleEnemyAI_Sunaarasi_Mine1:
	INCDEC	+2
	JUMP	DoubleEnemyAI_Sunaarasi_Friend


DoubleEnemyAI_Sunaarasi_Friend:	
	//CHECK_TOKUSEI	CHECK_ATTACK
	//IF_EQUAL	TOKUSYU_SUNAGAKURE, DoubleEnemyAI_Sunaarasi_Friend1
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_SUNAGAKURE
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Sunaarasi_Friend1
	
	//�����̃^�C�v���`�F�b�N���K�v
	CHECK_TYPE	CHECK_ATTACK_FRIEND_TYPE1
	IF_EQUAL	POKETYPE_IWA, DoubleEnemyAI_Sunaarasi_Friend1
	CHECK_TYPE	CHECK_ATTACK_FRIEND_TYPE2
	IF_EQUAL	POKETYPE_IWA, DoubleEnemyAI_Sunaarasi_Friend1
	JUMP	DoubleEnemyAI_Sunaarasi_End

DoubleEnemyAI_Sunaarasi_Friend1:
	INCDEC	+2

DoubleEnemyAI_Sunaarasi_End:	
	AIEND


//���イ��傭
DoubleEnemyAI_Zyuuryoku:
	FIELD_CONDITION_CHECK	FIELD_CONDITION_JUURYOKU,DoubleMineAI_end	//���イ��傭���͂��Ȃ�	

DoubleEnemyAI_Zyuuryoku_mine://���������V����s
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK,TOKUSYU_HUYUU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_mine_ng
	CHECK_HAVE_TYPE		CHECK_ATTACK,POKETYPE_HIKOU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_mine_ng
	IF_WAZAKOUKA	CHECK_ATTACK,WAZAKOUKA_DENZIHUYUU,DoubleEnemyAI_Zyuuryoku_mine_ng
	JUMP	DoubleEnemyAI_Zyuuryoku_mine_friend

DoubleEnemyAI_Zyuuryoku_mine_ng:
	INCDEC	-5
	JUMP	DoubleEnemyAI_Zyuuryoku_mine_friend

DoubleEnemyAI_Zyuuryoku_mine_friend://�p�[�g�i�[�����V����s
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_HUYUU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_mine_friend_ng
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_HIKOU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_mine_friend_ng
	IF_WAZAKOUKA	CHECK_ATTACK_FRIEND,WAZAKOUKA_DENZIHUYUU,DoubleEnemyAI_Zyuuryoku_mine_friend_ng
	JUMP	DoubleEnemyAI_Zyuuryoku_enemy

DoubleEnemyAI_Zyuuryoku_mine_friend_ng:
	INCDEC	-5
	JUMP	DoubleEnemyAI_Zyuuryoku_enemy

DoubleEnemyAI_Zyuuryoku_enemy://�G�����V����s
	CHECK_HAVE_TOKUSEI	CHECK_DEFENCE,TOKUSYU_HUYUU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_enemy_ok
	CHECK_HAVE_TYPE		CHECK_DEFENCE,POKETYPE_HIKOU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_enemy_ok
	IF_WAZAKOUKA	CHECK_DEFENCE,WAZAKOUKA_DENZIHUYUU,DoubleEnemyAI_Zyuuryoku_enemy_ok
	JUMP	DoubleEnemyAI_Zyuuryoku_enemy_friend

DoubleEnemyAI_Zyuuryoku_enemy_ok:
	IF_RND_UNDER	64, DoubleEnemyAI_Zyuuryoku_enemy_friend
	INCDEC	+3
	JUMP	DoubleEnemyAI_Zyuuryoku_enemy_friend

DoubleEnemyAI_Zyuuryoku_enemy_friend://�G�p�[�g�i�[�����V����s
	CHECK_HAVE_TOKUSEI	CHECK_DEFENCE_FRIEND,TOKUSYU_HUYUU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_enemy_friend_ok
	CHECK_HAVE_TYPE		CHECK_DEFENCE_FRIEND,POKETYPE_HIKOU
	IF_EQUAL	HAVE_YES,	DoubleEnemyAI_Zyuuryoku_enemy_friend_ok
	IF_WAZAKOUKA	CHECK_DEFENCE_FRIEND,WAZAKOUKA_DENZIHUYUU,DoubleEnemyAI_Zyuuryoku_enemy_friend_ok
	JUMP	DoubleEnemyAI_Zyuuryoku_End

DoubleEnemyAI_Zyuuryoku_enemy_friend_ok:
	IF_RND_UNDER	64, DoubleEnemyAI_Zyuuryoku_End
	INCDEC	+3
	JUMP	DoubleEnemyAI_Zyuuryoku_End

DoubleEnemyAI_Zyuuryoku_End:
AIEND


//�g���b�N���[��
DoubleEnemyAI_Torikkuruumu:
	//�S�����Ȃ��ꍇ�͂��Ȃ�
	IF_HP_EQUAL	CHECK_ATTACK_FRIEND,0,AI_DEC30	//���������Ȃ�
	IF_HP_EQUAL	CHECK_DEFENCE_FRIEND,0,AI_DEC30	//�G�̃p�[�g�i�[�����Ȃ�
	IF_HP_EQUAL	CHECK_DEFENCE,0,AI_DEC30		//�G�����Ȃ�
	
	//�����ƃp�[�g�i�[�̌J��o�����ʂ��R�ʁA�S�ʂȂ���Z
	//�����ƃp�[�g�i�[�̌J��o�����ʂ��P�ʁA�Q�ʂȂ猸�Z
	CHECK_AGI_RANK	CHECK_ATTACK	//�����̏���
	IF_EQUAL	0,DoubleEnemyAI_Torikkuruumu_mine1
	IF_EQUAL	1,DoubleEnemyAI_Torikkuruumu_mine2
	IF_EQUAL	2,DoubleEnemyAI_Torikkuruumu_mine3
	IF_EQUAL	3,DoubleEnemyAI_Torikkuruumu_mine4
	JUMP	DoubleEnemyAI_Torikkuruumu_End


DoubleEnemyAI_Torikkuruumu_mine1:
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND		//�����̏���
	IF_EQUAL	1,AI_DEC30
	IF_EQUAL	0,AI_DEC30	//�P�ʃ^�C
	JUMP	DoubleEnemyAI_Torikkuruumu_End1
	
DoubleEnemyAI_Torikkuruumu_mine2:
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND		//�����̏���
	IF_EQUAL	0,AI_DEC30
	JUMP	DoubleEnemyAI_Torikkuruumu_End1

DoubleEnemyAI_Torikkuruumu_mine3:
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND		//�����̏���
	IFN_EQUAL	3,DoubleEnemyAI_Torikkuruumu_End1
	IF_RND_UNDER	64, DoubleEnemyAI_Torikkuruumu_End1
	INCDEC	+5
	JUMP	DoubleEnemyAI_Torikkuruumu_End

DoubleEnemyAI_Torikkuruumu_mine4:
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND		//�����̏���
	IFN_EQUAL	2,DoubleEnemyAI_Torikkuruumu_End1
	IF_RND_UNDER	64, DoubleEnemyAI_Torikkuruumu_End1
	INCDEC	+5
	JUMP	DoubleEnemyAI_Torikkuruumu_End

DoubleEnemyAI_Torikkuruumu_End1:
	INCDEC	-5
DoubleEnemyAI_Torikkuruumu_End:
	AIEND


//���̂�тƂ܂�
DoubleEnemyAI_Konoyubitomare:
	IF_HP_OVER	CHECK_ATTACK,90,DoubleEnemyAI_Konoyubitomare1
	IF_HP_OVER	CHECK_ATTACK,50,DoubleEnemyAI_Konoyubitomare2
	IF_HP_OVER	CHECK_ATTACK,30,DoubleEnemyAI_Konoyubitomare3
	IF_RND_UNDER	64, DoubleEnemyAI_Konoyubitomare_End
	JUMP	AI_DEC5


DoubleEnemyAI_Konoyubitomare1://�����c��90�ȏ�
	IF_HP_OVER	CHECK_ATTACK_FRIEND,90,DoubleEnemyAI_Konoyubitomare_ng1
	IF_HP_OVER	CHECK_ATTACK_FRIEND,50,DoubleEnemyAI_Konoyubitomare_ok1
	IF_HP_OVER	CHECK_ATTACK_FRIEND,30,DoubleEnemyAI_Konoyubitomare_ok2
	JUMP	DoubleEnemyAI_Konoyubitomare_ok3


DoubleEnemyAI_Konoyubitomare2://�����c��50�ȏ�90����
	IF_HP_OVER	CHECK_ATTACK_FRIEND,90,DoubleEnemyAI_Konoyubitomare_ng2
	IF_HP_OVER	CHECK_ATTACK_FRIEND,50,DoubleEnemyAI_Konoyubitomare_ng1
	IF_HP_OVER	CHECK_ATTACK_FRIEND,30,DoubleEnemyAI_Konoyubitomare_ok1
	JUMP	DoubleEnemyAI_Konoyubitomare_ok2


DoubleEnemyAI_Konoyubitomare3://�����c��30�ȏ�50����
	IF_HP_OVER	CHECK_ATTACK_FRIEND,90,DoubleEnemyAI_Konoyubitomare_ng2
	IF_HP_OVER	CHECK_ATTACK_FRIEND,50,DoubleEnemyAI_Konoyubitomare_ng2
	IF_HP_OVER	CHECK_ATTACK_FRIEND,30,DoubleEnemyAI_Konoyubitomare_ok1
	JUMP	DoubleEnemyAI_Konoyubitomare_ok2


DoubleEnemyAI_Konoyubitomare_ng1:
	IF_RND_UNDER	64, DoubleEnemyAI_Konoyubitomare_End
	INCDEC	-1
	JUMP	DoubleEnemyAI_Konoyubitomare_End

DoubleEnemyAI_Konoyubitomare_ng2:
	IF_RND_UNDER	64, DoubleEnemyAI_Konoyubitomare_End
	INCDEC	-2
	JUMP	DoubleEnemyAI_Konoyubitomare_End

DoubleEnemyAI_Konoyubitomare_ok1:
	IF_RND_UNDER	64, DoubleEnemyAI_Konoyubitomare_End
	INCDEC	+1
	JUMP	DoubleEnemyAI_Konoyubitomare_End

DoubleEnemyAI_Konoyubitomare_ok2:
	IF_RND_UNDER	64, DoubleEnemyAI_Konoyubitomare_End
	INCDEC	+2
	JUMP	DoubleEnemyAI_Konoyubitomare_End

DoubleEnemyAI_Konoyubitomare_ok3:
	IF_RND_UNDER	64, DoubleEnemyAI_Konoyubitomare_End
	INCDEC	+3
	JUMP	DoubleEnemyAI_Konoyubitomare_End

DoubleEnemyAI_Konoyubitomare_End:
	AIEND


//�������u�Ă������v
DoubleEnemyAI_Tedasuke:
	IF_WAZA_SEQNO_JUMP	38,DoubleEnemyAI_Tedasuke_End		//	���������Ђ�����
	IF_WAZA_SEQNO_JUMP	41,DoubleEnemyAI_Tedasuke_End		//�@��イ�̂�����
	IF_WAZA_SEQNO_JUMP	87,DoubleEnemyAI_Tedasuke_End		//�@�i�C�g�w�b�h
	IF_WAZA_SEQNO_JUMP	88,DoubleEnemyAI_Tedasuke_End		//�@�T�C�R�E�G�[�u
	IF_WAZA_SEQNO_JUMP	130,DoubleEnemyAI_Tedasuke_End	//�@�\�j�b�N�u�[��
	COMP_POWER	LOSS_CALC_OFF
	IFN_EQUAL	COMP_POWER_NONE, AI_INC1
DoubleEnemyAI_Tedasuke_End:
	AIEND


// �������u���񂶂傤�v
DoubleEnemyAI_Konzyou:
	IF_COND	CHECK_ATTACK,0x000000ff,DoubleEnemyAI_Konzyou1
	AIEND
DoubleEnemyAI_Konzyou1:
	COMP_POWER	LOSS_CALC_OFF
	IF_EQUAL	COMP_POWER_NONE, AI_DEC5
	INCDEC		1
	IF_EQUAL	COMP_POWER_TOP, AI_INC2
	AIEND


// �n�k�Eϸ�������ok
DoubleEnemyAI_Zisin:
	//�������ł񂶂ӂ������Ă��邩�ǂ����H2006.6.14
	IF_WAZAKOUKA	CHECK_ATTACK_FRIEND,WAZAKOUKA_DENZIHUYUU,AI_INC2	// �ł񂶂ӂ䂤��
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND,TOKUSYU_HUYUU
	IF_EQUAL	HAVE_YES,	AI_INC2
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_HIKOU
	IF_EQUAL	HAVE_YES,	AI_INC2
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_HONOO
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_DENKI
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_DOKU
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_IWA
	IF_EQUAL	HAVE_YES,	AI_DEC10
	JUMP	AI_DEC3

//�@�݂炢�悿
DoubleEnemyAI_Miraiyoti:
	IF_HP_EQUAL	CHECK_ATTACK_FRIEND,0,DoubleEnemyAI_Miraiyoti_End	//���肪�P�̂Ȃ�X���[
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_MIRAIYOTI, DoubleEnemyAI_Miraiyoti_Dec
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_HAMETUNONEGAI, DoubleEnemyAI_Miraiyoti_Dec
	JUMP	DoubleEnemyAI_Miraiyoti_End

DoubleEnemyAI_Miraiyoti_Dec://�x���������͍T����
	CHECK_AGI_RANK	CHECK_ATTACK	
	IF_EQUAL	3,AI_DEC3						//4
	IF_EQUAL	2,DoubleEnemyAI_Miraiyoti_Rank3	//3
	IF_EQUAL	1,DoubleEnemyAI_Miraiyoti_Rank2	//2
	IF_EQUAL	0,DoubleEnemyAI_Miraiyoti_Rank1	//1	
	JUMP	DoubleEnemyAI_Miraiyoti_End

DoubleEnemyAI_Miraiyoti_Rank3:
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND	
	IF_EQUAL	0,AI_DEC3	//1
	IF_EQUAL	1,AI_DEC3	//2
	IF_RND_UNDER	128, DoubleEnemyAI_Miraiyoti_End
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND	
	IF_EQUAL	2,AI_DEC3	//3�ʃ^�C
	JUMP	DoubleEnemyAI_Miraiyoti_End

DoubleEnemyAI_Miraiyoti_Rank2:
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND	
	IF_EQUAL	0,AI_DEC3	//1
	IF_RND_UNDER	128, DoubleEnemyAI_Miraiyoti_End
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND	
	IF_EQUAL	1,AI_DEC3	//2�ʃ^�C
	JUMP	DoubleEnemyAI_Miraiyoti_End

DoubleEnemyAI_Miraiyoti_Rank1:
	IF_RND_UNDER	128, DoubleEnemyAI_Miraiyoti_End
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND	
	IF_EQUAL	0,AI_DEC3	//1�ʃ^�C
	JUMP	DoubleEnemyAI_Miraiyoti_End

DoubleEnemyAI_Miraiyoti_End:
	AIEND


// ��ٽܯ��2006.6.16ok
DoubleEnemyAI_SkillSwap:
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_NAMAKE, AI_INC5
	IF_EQUAL	TOKUSYU_SUROOSUTAATO, AI_INC5	//2006.6.14
	IF_EQUAL	TOKUSYU_ATODASI, AI_INC5		//2006.6.14
	IF_EQUAL	TOKUSYU_BUKIYOU, AI_INC5		//2006.6.14
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KAGEHUMI, AI_INC2
	IF_EQUAL	TOKUSYU_YOGAPAWAA, AI_INC2
	IF_EQUAL	TOKUSYU_TIKARAMOTI, AI_INC2		//2006.6.14
	IF_EQUAL	TOKUSYU_KATAYABURI, AI_INC2		//2006.6.14
	IF_EQUAL	TOKUSYU_HAADOROKKU, AI_INC2		//2006.6.14
	IF_EQUAL	TOKUSYU_FIRUTAA, AI_INC2		//2006.6.14
	IF_EQUAL	TOKUSYU_HURAWAAGIHUTO, AI_INC2	//2006.6.14
	AIEND

// �d�Cܻ�
DoubleEnemyAI_ElectricType:
	IF_WAZANO	WAZANO_HOUDEN,DoubleEnemyAI_ElectricType_FriendCheck_Houden	//�ق��ł�
	
	CHECK_HAVE_TOKUSEI	CHECK_DEFENCE_FRIEND,TOKUSYU_HIRAISIN
	IF_EQUAL	HAVE_YES, DoubleEnemyAI_ElectricType_Enemy_Hiraisin
	JUMP	DoubleEnemyAI_ElectricType_FriendCheck

DoubleEnemyAI_ElectricType_Enemy_Hiraisin:
	INCDEC	-1
	
	CHECK_HAVE_TYPE		CHECK_DEFENCE_FRIEND,POKETYPE_JIMEN
	IF_EQUAL	HAVE_NO, DoubleEnemyAI_ElectricType_FriendCheck
	INCDEC	-8

DoubleEnemyAI_ElectricType_FriendCheck:
	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_HIRAISIN
	IF_EQUAL	HAVE_YES,	AI_DEC10

	//�ق��ł�Ńp�[�g�i�[�d�C�ŃA�b�v����2006.6.16
	IF_WAZANO	WAZANO_HOUDEN,DoubleEnemyAI_ElectricType_FriendCheck_Houden	//�ق��ł�
	JUMP	DoubleEnemyAI_ElectricTypeEnd
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_DENKIENZIN, AI_INC3		// �������u�ł񂫃G���W���v2006.6.16
	//IF_EQUAL	TOKUSYU_TIKUDEN, AI_INC3		// �������u�����ł�v2006.6.16

DoubleEnemyAI_ElectricType_FriendCheck_Houden:
	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_DENKIENZIN
	IF_EQUAL	HAVE_YES,	AI_INC3
	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_TIKUDEN
	IF_EQUAL	HAVE_YES,	AI_INC3

	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_MIZU
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_HIKOU
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_JIMEN
	IF_EQUAL	HAVE_YES,	AI_INC3
	INCDEC	-3

DoubleEnemyAI_ElectricTypeEnd:
	AIEND


//	��ܻ�2006.6.16
DoubleEnemyAI_WaterType:
	IF_WAZANO	WAZANO_NAMINORI,DoubleEnemyAI_WaterType_FriendCheck_Naminori	//�Ȃ݂̂�
	CHECK_HAVE_TOKUSEI	CHECK_DEFENCE_FRIEND,TOKUSYU_YOBIMIZU
	IF_EQUAL	HAVE_NO, DoubleEnemyAI_WaterType_FriendCheck
	INCDEC	-1

DoubleEnemyAI_WaterType_FriendCheck:
	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_YOBIMIZU
	IF_EQUAL	HAVE_YES,	AI_DEC10
	
	//IFN_WAZANO	WAZANO_NAMINORI,DoubleEnemyAI_WaterTypeEnd	//�Ȃ݂̂�
	IF_WAZANO	WAZANO_NAMINORI,DoubleEnemyAI_WaterType_FriendCheck_Naminori	//�Ȃ݂̂�
	JUMP	DoubleEnemyAI_WaterTypeEnd	//�Ȃ݂̂�łȂ�


DoubleEnemyAI_WaterType_FriendCheck_Naminori:
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_KANSOUHADA, AI_INC3		// �������u���񂻂��͂��v2006.6.16

	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_KANSOUHADA
	IF_EQUAL	HAVE_YES,	AI_INC3

	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_TYOSUI
	IF_EQUAL	HAVE_YES,	AI_INC3


	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_JIMEN
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_HONOO
	IF_EQUAL	HAVE_YES,	AI_DEC10
	INCDEC	-3

DoubleEnemyAI_WaterTypeEnd:
	AIEND


// �����U
//DoubleEnemyAI_FireType:
//	IF_ALREADY_MORAIBI	CHECK_ATTACK, DoubleEnemyAI_FireType1//���炢�тŃp���[�A�b�v��Ԃ��H
//	AIEND
//DoubleEnemyAI_FireType1:
//	JUMP	AI_INC1

DoubleEnemyAI_FireType:
	IF_ALREADY_MORAIBI	CHECK_ATTACK, DoubleEnemyAI_FireType1//���炢�тŃp���[�A�b�v��Ԃ��H
	JUMP	DoubleEnemyAI_FireType_Check
DoubleEnemyAI_FireType1:
	INCDEC	+1

//dp���ǉ�
DoubleEnemyAI_FireType_Check:
	//IFN_WAZANO	WAZANO_HUNEN,DoubleEnemyAI_FireTypeEnd	//�ӂ񂦂�
	IF_WAZANO	WAZANO_HUNEN,DoubleEnemyAI_FireType_Check_Hunen	//�ӂ񂦂�
	JUMP	DoubleEnemyAI_FireTypeEnd	//�ӂ񂦂łȂ�

DoubleEnemyAI_FireType_Check_Hunen:
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_KANSOUHADA, AI_DEC3		// �������u���񂻂��͂��v2006.6.16
	//IF_EQUAL	TOKUSYU_MORAIBI,	AI_INC3		// �������u���炢�сv2006.6.16

	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_KANSOUHADA
	IF_EQUAL	HAVE_YES,	AI_DEC3
	CHECK_HAVE_TOKUSEI		CHECK_ATTACK_FRIEND,TOKUSYU_MORAIBI
	IF_EQUAL	HAVE_YES,	AI_INC3

	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_KUSA
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_HAGANE
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_KOORI
	IF_EQUAL	HAVE_YES,	AI_DEC10
	CHECK_HAVE_TYPE		CHECK_ATTACK_FRIEND,POKETYPE_MUSHI
	IF_EQUAL	HAVE_YES,	AI_DEC10
	INCDEC	-3

DoubleEnemyAI_FireTypeEnd:
	AIEND


//--------------------------------------
//	�������Ώ�
//--------------------------------------
DoubleAI_MineSeq:
	IF_HINSHI	CHECK_ATTACK_FRIEND,DoubleMineAI_end	//���������Ȃ��ꍇ�͂��Ȃ�	
	COMP_POWER	LOSS_CALC_OFF
	IF_EQUAL	COMP_POWER_NONE, DoubleMineAI_1

// �_���[�W���U------------------
	CHECK_TYPE	CHECK_WAZA
	IF_EQUAL	POKETYPE_HONOO, DoubleMineAI_FireType
	IF_EQUAL	POKETYPE_DENKI, DoubleMineAI_ElectricType
	IF_EQUAL	POKETYPE_MIZU, DoubleMineAI_WaterType
	IF_WAZANO	WAZANO_NAGETUKERU, DoubleMineAI_Nagetukeru	//�Ȃ�����2006.6.23
	


DoubleMineDamageEnd:
	JUMP	AI_DEC30	// ��{�I�Ƀ_���[�W���U�͏o���Ȃ�

//���^�C�v
DoubleMineAI_FireType:
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_MORAIBI
	IF_EQUAL	HAVE_YES, DoubleMineAI_FireType1
	JUMP	DoubleMineDamageEnd
DoubleMineAI_FireType1:
	IF_ALREADY_MORAIBI	CHECK_ATTACK_FRIEND, DoubleMineDamageEnd
	JUMP	AI_INC3

//�d�C�^�C�v2006.6.16
DoubleMineAI_ElectricType:
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_DENKIENZIN
	IF_EQUAL	HAVE_YES, DoubleMineAI_ElectricType1
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_TIKUDEN
	IF_EQUAL	HAVE_YES, DoubleMineAI_ElectricType2
	JUMP	DoubleMineDamageEnd

DoubleMineAI_ElectricType1:
	//�����_��������
	IF_RND_UNDER	160, DoubleMineAI_ElectricType_End
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_AGI,12,DoubleMineDamageEnd
	JUMP	AI_INC3

DoubleMineAI_ElectricType2:
	IF_HP_EQUAL	CHECK_ATTACK_FRIEND,100,AI_DEC10
	IF_HP_OVER	CHECK_ATTACK_FRIEND,90,DoubleMineAI_ElectricType_End
	IF_HP_OVER	CHECK_ATTACK_FRIEND,75,DoubleMineAI_ElectricType2_1
	IF_HP_OVER	CHECK_ATTACK_FRIEND,50,DoubleMineAI_ElectricType2_2
	JUMP	DoubleMineAI_ElectricType2_3

DoubleMineAI_ElectricType2_1://25%
	IF_RND_UNDER	64, AI_INC3
	JUMP	DoubleMineAI_ElectricType_End	

DoubleMineAI_ElectricType2_2://50%
	IF_RND_UNDER	128, AI_INC3
	JUMP	DoubleMineAI_ElectricType_End	
	
DoubleMineAI_ElectricType2_3://75%
	IF_RND_UNDER	192, AI_INC3
	JUMP	DoubleMineAI_ElectricType_End	

DoubleMineAI_ElectricType_End:
	AIEND



//���^�C�v2006.6.16
DoubleMineAI_WaterType:
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_TYOSUI
	IF_EQUAL	HAVE_YES, DoubleMineAI_WaterType1
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_KANSOUHADA
	IF_EQUAL	HAVE_YES, DoubleMineAI_WaterType1
	JUMP	DoubleMineDamageEnd

DoubleMineAI_WaterType1:
	IF_HP_EQUAL	CHECK_ATTACK_FRIEND,100,AI_DEC10
	IF_HP_OVER	CHECK_ATTACK_FRIEND,90,DoubleMineAI_WaterType_End
	IF_HP_OVER	CHECK_ATTACK_FRIEND,75,DoubleMineAI_WaterType1_1
	IF_HP_OVER	CHECK_ATTACK_FRIEND,50,DoubleMineAI_WaterType1_2
	JUMP	DoubleMineAI_WaterType1_3


DoubleMineAI_WaterType1_1://25%
	IF_RND_UNDER	64, AI_INC3
	JUMP	DoubleMineAI_WaterType_End	

DoubleMineAI_WaterType1_2://50%
	IF_RND_UNDER	128, AI_INC3
	JUMP	DoubleMineAI_WaterType_End	
	
DoubleMineAI_WaterType1_3://75%
	IF_RND_UNDER	192, AI_INC3
	JUMP	DoubleMineAI_WaterType_End	

DoubleMineAI_WaterType_End:
	AIEND




// ��_���[�W���U----------------
DoubleMineAI_1:
	IF_WAZANO	WAZANO_SUKIRUSUWAPPU,DoubleMineAI_SkillSwap	// ��ٽܯ��
	IF_WAZANO	WAZANO_ONIBI, DoubleMineAI_Onibi			// ���ɂ�
	IF_WAZANO	WAZANO_DENZIHA, DoubleMineAI_Denziha		// �ł񂶂�2006.6.23
	
	//IF_WAZANO	WAZANO_DOKUDOKU, DoubleMineAI_Onibi			// �ǂ��ǂ�	2006.6.23
	IF_WAZA_SEQNO_JUMP	33,DoubleMineAI_Doku				// �����ǂ�2006.6.23
	IF_WAZA_SEQNO_JUMP	66,DoubleMineAI_Doku				// �ǂ�	2006.6.23
	
	IF_WAZANO	WAZANO_TEDASUKE, DoubleMineAI_Tedasuke		// �Ă�����
	IF_WAZANO	WAZANO_IBARU, DoubleMineAI_Ibaru			// ���΂�
	IF_WAZANO	WAZANO_TORIKKU, DoubleMineAI_Torikku		// �g���b�N2006.6.23
	IF_WAZANO	WAZANO_SURIKAE, DoubleMineAI_Torikku		// ���ꂩ��2006.6.23
	
	IF_WAZANO	WAZANO_IEKI, DoubleMineAI_Ieki				// ������2006.6.23
	
	IF_WAZANO	WAZANO_TUBOWOTUKU, DoubleMineAI_Tubowotuku	// �ڂ���2006.6.23

	JUMP	DoubleMineAI_end

//��ٽܯ��---------
DoubleMineAI_SkillSwap:
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NAMAKE, AI_INC10		// �������u�Ȃ܂��v
	IF_EQUAL	TOKUSYU_SUROOSUTAATO, AI_INC10	// �������u�X���[�X�^�[�g�v2006.6.14
	
	// �������u�ӂ䂤�v
	CHECK_TOKUSEI	CHECK_ATTACK
	IFN_EQUAL	TOKUSYU_HUYUU, DoubleMineAI_SkillSwapFukugan
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HUYUU, DoubleMineAI_end		// �������u�ӂ䂤�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IFN_EQUAL	POKETYPE_DENKI, DoubleMineAI_SkillSwapFukugan
	INCDEC		1
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IFN_EQUAL	POKETYPE_DENKI, DoubleMineAI_SkillSwapFukugan
	INCDEC		1
	AIEND

DoubleMineAI_SkillSwapFukugan:
	CHECK_TOKUSEI	CHECK_ATTACK
	//IFN_EQUAL	TOKUSYU_HUKUGAN, DoubleMineAI_end
	IF_EQUAL	TOKUSYU_HUKUGAN,DoubleMineAI_SkillSwapFukugan_Check
	IF_EQUAL	TOKUSYU_NOOGAADO, DoubleMineAI_SkillSwapFukugan_Check
	JUMP	DoubleMineAI_end
	
	// �������オ�����
DoubleMineAI_SkillSwapFukugan_Check:
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_DAIMONZI, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_KAMINARI, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_KUROSUTYOPPU, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_HAIDOROPONPU, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_BAKURETUPANTI, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_HUBUKI, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_DENZIHOU, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_MEGAHOON, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_KIAIDAMA, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_DASUTOSYUUTO, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_MAGUMASUTOOMU, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_PAWAAWHIPPU, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_SIIDOHUREA, DoubleMineAI_SkillSwapFukugan1
	IF_HAVE_WAZA	CHECK_ATTACK_FRIEND, WAZANO_MOROHANOZUTUKI, DoubleMineAI_SkillSwapFukugan1
	JUMP		DoubleMineAI_end

DoubleMineAI_SkillSwapFukugan1:
	JUMP	AI_INC3


//���ɂ�
DoubleMineAI_Onibi:
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_MORAIBI
	IF_EQUAL	HAVE_YES, DoubleMineAI_FireType	//�_���[�W�O�ŃX���[�����̂ł����ōă`�F�b�N
	
	//����ɍ����ւ�	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_KONZYOU
	IFN_EQUAL	HAVE_YES, DoubleMineAI_end	
	
	//CHECK_TOKUSEI	CHECK_DEFENCE
	//IF_EQUAL	TOKUSYU_MORAIBI,DoubleMineAI_FireType	//�_���[�W�O�ŃX���[�����̂ł�����
	//IFN_EQUAL	TOKUSYU_KONZYOU, DoubleMineAI_end
	IF_COND	CHECK_ATTACK_FRIEND,0x000000ff,DoubleMineAI_end
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO, DoubleMineAI_end			// �ق̂��^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO, DoubleMineAI_end			// �ق̂��g�^�C�v	
	
	IF_HAVE_ITEM	CHECK_ATTACK_FRIEND, ITEM_KAENDAMA, DoubleMineAI_end//2006.6.23
	IF_HAVE_ITEM	CHECK_ATTACK_FRIEND, ITEM_DOKUDOKUDAMA, DoubleMineAI_end//2006.6.23
	IF_HP_UNDER	CHECK_ATTACK_FRIEND,81,DoubleMineAI_end
	JUMP AI_INC5


DoubleMineAI_Denziha://�ł񂶂�
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_JIMEN, DoubleMineAI_end			// ���߂�^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_JIMEN, DoubleMineAI_end			// ���߂�g�^�C�v		
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_DENKIENZIN
	IF_EQUAL	HAVE_YES, DoubleMineAI_ElectricType
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_TIKUDEN
	IF_EQUAL	HAVE_YES, DoubleMineAI_ElectricType

	//CHECK_TOKUSEI	CHECK_DEFENCE
	//IF_EQUAL	TOKUSYU_DENKIENZIN,DoubleMineAI_ElectricType	//�_���[�W�O�ŃX���[�����̂ł�����
	//IF_EQUAL	TOKUSYU_TIKUDEN,DoubleMineAI_ElectricType		//�_���[�W�O�ŃX���[�����̂ł�����
	JUMP	DoubleMineAI_end


//�ǂ��E�ǂ��ǂ�2006.6.23
DoubleMineAI_Doku:
	//CHECK_TOKUSEI	CHECK_DEFENCE
	//IFN_EQUAL	TOKUSYU_POIZUNHIIRU, DoubleMineAI_Onibi//���񂶂傤�`�F�b�N��
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_POIZUNHIIRU
	//IFN_EQUAL	HAVE_YES, DoubleMineAI_Onibi//���X�N�������̂œ���Ȃ�
	IFN_EQUAL	HAVE_YES, DoubleMineAI_end//���X�N�������̂œ���Ȃ�
	
	IF_COND	CHECK_DEFENCE,0x000000ff,DoubleMineAI_end
	IF_HAVE_ITEM	CHECK_ATTACK_FRIEND, ITEM_DOKUDOKUDAMA, DoubleMineAI_end
	IF_HP_OVER	CHECK_ATTACK_FRIEND,91,DoubleMineAI_end
	JUMP AI_INC5



//�Ă�����
DoubleMineAI_Tedasuke:
	IF_HP_EQUAL	CHECK_ATTACK_FRIEND,0,AI_DEC30	//���������Ȃ�
	IF_HP_OVER	CHECK_ATTACK_FRIEND,50,DoubleMineAI_Tedasuke1
	CHECK_AGI_RANK	CHECK_ATTACK_FRIEND		//�����̏���
	IF_UNDER	1,DoubleMineAI_Tedasuke1
	JUMP	DoubleMineAI_Tedasuke_end

DoubleMineAI_Tedasuke1:
	IF_RND_UNDER	64, AI_DEC1
	INCDEC	+2

DoubleMineAI_Tedasuke_end:
	AIEND


//���΂�
DoubleMineAI_Ibaru:
	IF_HAVE_ITEM	CHECK_DEFENCE, ITEM_KIINOMI, DoubleMineAI_Ibaru1
	IF_HAVE_ITEM	CHECK_DEFENCE, ITEM_RAMUNOMI, DoubleMineAI_Ibaru1
	JUMP	DoubleMineAI_end
DoubleMineAI_Ibaru1:
	IF_PARA_OVER	CHECK_DEFENCE, PARA_POW, 7, DoubleMineAI_IbaruEnd
	INCDEC	3
DoubleMineAI_IbaruEnd:
	AIEND

//�_�u��
DoubleMineAI_Nagetukeru:
DoubleMineAI_Torikku:
	AIEND

DoubleMineAI_Ieki:
	IF_WAZAKOUKA	CHECK_ATTACK_FRIEND,WAZAKOUKA_IEKI,DoubleMineAI_end		// ��������
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND
	//IF_EQUAL	TOKUSYU_NAMAKE,DoubleMineAI_Ieki_ok				// �Ȃ܂�
	//IF_EQUAL	TOKUSYU_SUROOSUTAATO,DoubleMineAI_Ieki_ok		// �X���[�X�^�[�g
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_NAMAKE
	IF_EQUAL	HAVE_YES, DoubleMineAI_Ieki_ok
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_SUROOSUTAATO
	IF_EQUAL	HAVE_YES, DoubleMineAI_Ieki_ok
	JUMP	DoubleMineAI_Ieki_end

DoubleMineAI_Ieki_ok:
	INCDEC	+5
DoubleMineAI_Ieki_end:
	AIEND


DoubleMineAI_Tubowotuku:	//�ڂ���:��l�p�Ɗ�{�͓���
	
	//�p�����[�^�[�l�`�w���͂��Ȃ�
	//CHECK_TOKUSEI	CHECK_ATTACK_FRIEND				//���񂶂��Ȃ�i�K���X�ł��2006.6.6
	//IF_EQUAL	TOKUSYU_TANZYUN,DoubleMineAI_Tubowotuku_tanzyun
	
	CHECK_HAVE_TOKUSEI	CHECK_ATTACK_FRIEND, TOKUSYU_TANZYUN
	IF_EQUAL	HAVE_YES, DoubleMineAI_Tubowotuku_tanzyun

	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_POW,12,DoubleMineAI_end
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_DEF,12,DoubleMineAI_end
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_AGI,12,DoubleMineAI_end
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_SPEPOW,12,DoubleMineAI_end
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_SPEDEF,12,DoubleMineAI_end
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_AVOID,12,DoubleMineAI_end
	IF_PARA_EQUAL	CHECK_ATTACK_FRIEND,PARA_HIT,12,DoubleMineAI_end
	JUMP	DoubleMineAI_Tubowotuku_check	

DoubleMineAI_Tubowotuku_tanzyun:
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_POW,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_DEF,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_AGI,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_SPEPOW,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_SPEDEF,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_AVOID,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK_FRIEND,PARA_HIT,8,AI_DEC10

DoubleMineAI_Tubowotuku_check:
	IF_HP_UNDER	CHECK_ATTACK_FRIEND,51,DoubleMineAI_Tubowotuku_ng
	IF_HP_OVER	CHECK_ATTACK_FRIEND,90,DoubleMineAI_Tubowotuku_ok
	IF_RND_UNDER	128,DoubleMineAI_Tubowotuku_end

DoubleMineAI_Tubowotuku_ok:	// �ڂ���
	IF_RND_UNDER	80,DoubleMineAI_Tubowotuku_end
	INCDEC	2
	JUMP	DoubleMineAI_Tubowotuku_end

DoubleMineAI_Tubowotuku_ng:
	INCDEC	-1
DoubleMineAI_Tubowotuku_end:
	AIEND

DoubleMineAI_end:
	INCDEC	-30		//��{�I�ɖ����ɂ͏o���Ȃ�
	AIEND

AI_DEC1:
	INCDEC		-1	
	AIEND
AI_DEC2:
	INCDEC		-2	
	AIEND
AI_DEC3:
	INCDEC		-3	
	AIEND
AI_DEC5:
	INCDEC		-5	
	AIEND
AI_DEC6:
	INCDEC		-6	
	AIEND
AI_DEC8:
	INCDEC		-8	
	AIEND
AI_DEC10:
	INCDEC		-10
	AIEND
AI_DEC12:
	INCDEC		-12
	AIEND
AI_DEC30:
	INCDEC		-30
	AIEND
AI_INC1:
	INCDEC		1	
	AIEND
AI_INC2:
	INCDEC		2	
	AIEND
AI_INC3:
	INCDEC		3	
	AIEND
AI_INC5:
	INCDEC		5	
	AIEND
AI_INC10:
	INCDEC		10	
	AIEND

//========================================================
//	HP�Ŕ��f
//========================================================

HpCheckAISeq:
	IF_MIKATA_ATTACK	DoubleAI_MineSeq			//�Ώۂ������Ȃ�I��

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

