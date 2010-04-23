//============================================================================
/**
 *@file		tr_ai_basic.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	��{AI
//========================================================

BasicAISeq:
	IF_MIKATA_ATTACK	BasicAISeq_end				//�Ώۂ������Ȃ�I��

	// �����A�̃h�����̓_���[�W�O�ł��_���[�W���U���[�`����ʂ�
	IF_WAZANO	WAZANO_ZIWARE, BasicAIDamageStart
	IF_WAZANO	WAZANO_TUNODORIRU, BasicAIDamageStart
	
	COMP_POWER	LOSS_CALC_OFF
	IF_EQUAL	COMP_POWER_NONE,BasicAI_DmgEnd

//�_���[�W���U�̏ꍇ
BasicAIDamageStart:
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�(�ӂ䂤�Ȃǂ��R�R2006.8.9)	


	CHECK_TOKUSEI	CHECK_ATTACK//������Ԃ莝���Ȃ瑊��������X���[���čU��
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_DmgEnd		// ������Ԃ�2006.6.7
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_TIKUDEN,BasicAI_00_1		// �����ł�
	IF_EQUAL	TOKUSYU_DENKIENZIN,BasicAI_00_1		// �ł񂫃G���W��2006.6.7
	IF_EQUAL	TOKUSYU_HIRAISIN,BasicAI_00_1			// �Ђ炢����2010.2.9
	IF_EQUAL	TOKUSYU_TYOSUI,BasicAI_00_2			// ���傷��
	IF_EQUAL	TOKUSYU_MORAIBI,BasicAI_00_3		// ���炢��
	IF_EQUAL	TOKUSYU_HUSIGINAMAMORI,BasicAI_00_4	// �ӂ����Ȃ܂���
	IF_EQUAL	TOKUSYU_HUYUU,BasicAI_00_5			// �ӂ䂤
	IF_EQUAL	TOKUSYU_HUYUU,BasicAI_00_6			// ���񂻂��͂�2006.6.7
	IF_EQUAL	TOKUSYU_SOUSYOKU,BasicAI_00_7			// �������傭2010.2.9
	JUMP	BasicAI_DmgEnd

BasicAI_00_1:		// �����ł񁕂ł񂫃G���W�����Ђ炢����
	CHECK_TYPE	CHECK_WAZA
	IF_WAZA_TYPE	POKETYPE_DENKI,AI_DEC12		//���ʂȂ��Z�͂����Ȃ�	
	JUMP	BasicAI_DmgEnd

BasicAI_00_2:		// ���傷��
	CHECK_TYPE	CHECK_WAZA
	IF_WAZA_TYPE	POKETYPE_MIZU,AI_DEC12		//���ʂȂ��Z�͂����Ȃ�	
	JUMP	BasicAI_DmgEnd

BasicAI_00_3:		// ���炢��
	CHECK_TYPE	CHECK_WAZA
	IF_WAZA_TYPE	POKETYPE_HONOO,AI_DEC12		//���ʂȂ��Z�͂����Ȃ�	
	JUMP	BasicAI_DmgEnd

BasicAI_00_4:		// �ӂ����Ȃ܂���
	CHECK_WAZA_AISYOU	AISYOU_2BAI,BasicAI_DmgEnd	// ���ʂ΂��񂵂�������Ȃ�	
	CHECK_WAZA_AISYOU	AISYOU_4BAI,BasicAI_DmgEnd	// ���ʂ΂��񂵂�������Ȃ�	
	JUMP	AI_DEC12

BasicAI_00_5:		// �ӂ䂤
	CHECK_TYPE	CHECK_WAZA
	IF_WAZA_TYPE	POKETYPE_JIMEN,AI_DEC12		//���ʂȂ��Z�͂����Ȃ�	
	JUMP	BasicAI_DmgEnd

BasicAI_00_6:		// ���񂻂��͂�
	CHECK_TYPE	CHECK_WAZA
	IF_WAZA_TYPE	POKETYPE_MIZU,AI_DEC12		//���ʂȂ��Z�͂����Ȃ�	
	JUMP	BasicAI_DmgEnd

BasicAI_00_7:		// �������傭2010.2.9
	CHECK_TYPE	CHECK_WAZA
	IF_WAZA_TYPE	POKETYPE_KUSA,AI_DEC12		//���ʂȂ��Z�͂����Ȃ�	
	JUMP	BasicAI_DmgEnd

BasicAI_DmgEnd:

	CHECK_TOKUSEI	CHECK_DEFENCE
	IFN_EQUAL	TOKUSYU_BOUON,BasicAI_01		// �ڂ�����
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_01	// ������Ԃ�

	IF_WAZANO	WAZANO_NAKIGOE,AI_DEC10
	IF_WAZANO	WAZANO_HOERU,AI_DEC10
	IF_WAZANO	WAZANO_UTAU,AI_DEC10
	IF_WAZANO	WAZANO_TYOUONPA,AI_DEC10
	IF_WAZANO	WAZANO_IYANAOTO,AI_DEC10
	IF_WAZANO	WAZANO_IBIKI,AI_DEC10
	IF_WAZANO	WAZANO_SAWAGU,AI_DEC10
	IF_WAZANO	WAZANO_KINZOKUON,AI_DEC10
	IF_WAZANO	WAZANO_KUSABUE,AI_DEC10
	IF_WAZANO	WAZANO_MUSINOSAZAMEKI,AI_DEC10	//2006.6.6
	IF_WAZANO	WAZANO_OSYABERI,AI_DEC10		//2006.6.6
	IF_WAZANO	WAZANO_RINSYOU,AI_DEC10	//2010.2.9
	IF_WAZANO	WAZANO_EKOOBOISU,AI_DEC10		//2010.2.9
	IF_WAZANO	WAZANO_INISIENOUTA,AI_DEC10		//2010.2.9
	IF_WAZANO	WAZANO_BAAKUAUTO,AI_DEC10		//2010.2.9

BasicAI_01:
  TABLE_JUMP TABLE_JUMP_WAZASEQNO,WAZASEQ_TABLE
  AIEND

  TABLE_ADRS  WAZASEQ_TABLE
	ADRS  BasicAI_Dummy	//	  0�_�~�[
	ADRS  BasicAI_1			//	�˂ނ�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_7			//	���΂�
	ADRS  BasicAI_8			//	��߂���
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_10		//	�U���P�A�b�v

	ADRS  BasicAI_11		//	�h��P�A�b�v
	ADRS  BasicAI_12		//	�f�����P�A�b�v
	ADRS  BasicAI_13		//	���U�P�A�b�v
	ADRS  BasicAI_14		//	���h�P�A�b�v
	ADRS  BasicAI_15		//	�����P�A�b�v
	ADRS  BasicAI_16		//	����P�A�b�v
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_18		//	�U���P�_�E��
	ADRS  BasicAI_19		//	�h��P�_�E��
	ADRS  BasicAI_20		//	�f�����P�_�E��

	ADRS  BasicAI_21		//	���U�P�_�E��
	ADRS  BasicAI_22		//	���h�P�_�E��
	ADRS  BasicAI_23		//	�����P�_�E��
	ADRS  BasicAI_24		//	����P�_�E��
	ADRS  BasicAI_25		//	���낢����
	ADRS  BasicAI_26		//	���܂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_28		//	�ӂ��Ƃ΂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_32		//	������������2006.6.14
	ADRS  BasicAI_33		//	�ǂ��ǂ�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_35		//	�Ђ���̂���
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_38		//	���������Ђ�����
	ADRS  BasicAI_39		//	���܂�����	
	ADRS  BasicAI_40		//	������̂܂���

	ADRS  BasicAI_41		//�@��イ�̂�����2006.6.14
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_46		//	���낢����
	ADRS  BasicAI_47		//	����������
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_49		//	������
	ADRS  BasicAI_50		//	�U���Q�A�b�v

	ADRS  BasicAI_51		//	�h��Q�A�b�v
	ADRS  BasicAI_52		//	�f�����Q�A�b�v
	ADRS  BasicAI_53		//	���U�Q�A�b�v
	ADRS  BasicAI_54		//	���h�Q�A�b�v
	ADRS  BasicAI_55		//	�����Q�A�b�v
	ADRS  BasicAI_56		//	����Q�A�b�v
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_58		//	�U���Q�_�E��
	ADRS  BasicAI_59		//	�h��Q�_�E��
	ADRS  BasicAI_60		//	�f�����Q�_�E��

	ADRS  BasicAI_61		//	���U�Q�_�E��
	ADRS  BasicAI_62		//	���h�Q�_�E��
	ADRS  BasicAI_63		//	�����Q�_�E��
	ADRS  BasicAI_64		//	����Q�_�E��
	ADRS  BasicAI_65		//	���t���N�^�[
	ADRS  BasicAI_66		//	�ǂ�33
	ADRS  BasicAI_67		//	�܂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_79		//	�݂����
	ADRS  BasicAI_80		//	�͂�����������

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_84		//	��ǂ肬
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_86		//�@���Ȃ��΂�
	ADRS  BasicAI_87		//�@�����イ�Ȃ�
	ADRS  BasicAI_88		//�@�T�C�R�E�F�[�u
	ADRS  BasicAI_89		//�@�J�E���^�[
	ADRS  BasicAI_90		//�@�A���R�[��

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_92		//�@���т�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_94		//�@���b�N�I��		2006.6.14
	ADRS  BasicAI_92		//�@�˂���
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_99		//�@�����΂�
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_106		//	���낢�܂Ȃ���
	ADRS  BasicAI_107		//�@������
	ADRS  BasicAI_108		// 	���������Ȃ�
	ADRS  BasicAI_109		// 	�̂낢
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_112		// 	�܂��т�
	ADRS  BasicAI_113		// 	�݂�Ԃ�
	ADRS  BasicAI_114		// 	�ق��
	ADRS  BasicAI_115		// 	���Ȃ��炵
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_118		// 	���΂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_120		// 	��������

	ADRS  BasicAI_121		// 	���񂪂���
	ADRS  BasicAI_122		// 	�v���[���g
	ADRS  BasicAI_123		// 	�������
	ADRS  BasicAI_124		// 	����҂̂܂���
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_126		// 	�}�O�j�`���[�h
	ADRS  BasicAI_127		//	�o�g���^�b�`
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_130		// 	�\�j�b�N�u�[��

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_132		// 	�����̂Ђ���2006.6.14
	ADRS  BasicAI_133		// 	�������������i�V�[�P���X�폜�j2006.6.14
	ADRS  BasicAI_134		// 	���̂Ђ���i�V�[�P���X�폜�j2006.6.14
	ADRS  BasicAI_135		//�@�߂��߂�p���[2006.6.14
	ADRS  BasicAI_136		// 	���܂���
	ADRS  BasicAI_137		// 	�ɂق�΂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_142		// 	�͂炾����
	ADRS  BasicAI_143		//	��������25	
	ADRS  BasicAI_144		// 	�~���[�R�[�g
	ADRS  BasicAI_145		// 	���P�b�g����
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_148		// 	�݂炢�悿
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  AI_DEC10		// 	�e���|�[�g �i��{�I�Ɏg��Ȃ��j
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_156		// 	�܂邭�Ȃ�
	ADRS  BasicAI_157		// 	���܂�����2006.6.14
	ADRS  BasicAI_158		// 	�˂����܂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_160		// 	�����킦��

	ADRS  BasicAI_161		// 	�͂�����
	ADRS  BasicAI_162		// 	�݂̂���
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_164		// 	�����
	ADRS  BasicAI_165		// 	���������
	ADRS  BasicAI_166		// 	�����Ă�
	ADRS  BasicAI_167		//	�₯��
	ADRS  BasicAI_168		// 	�����݂₰
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_170		//	�������p���`

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_172		// 	���̂�тƂ܂�
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_176		// 	�Ă�����
	ADRS  BasicAI_177		// 	�g���b�N
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_181		// 	�˂��͂�
	ADRS  BasicAI_182		// 	�΂�������
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_184		// 	���T�C�N��
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_187		// 	������
	ADRS  BasicAI_188		// 	�͂������Ƃ�
	ADRS  BasicAI_189		// 	���ނ����
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_192		// 	�ӂ�����
	ADRS  BasicAI_193		// 	���t���b�V��
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_196		// 	��������
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_201		// 	�ǂ날����
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_205		// 	��������
	ADRS  BasicAI_206		// 	�R�X���p���[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_208		// 	�r���h�A�b�v
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_210		// 	�݂�������

	ADRS  BasicAI_211		// 	�߂�����
	ADRS  BasicAI_212		// 	��イ�̂܂�
//DP���ǉ��V�[�P���X
	ADRS  BasicAI_213			// 	�ق����傭�i�G�������h�܂łȂ��j
	ADRS  BasicAI_214			// 	�͂˂₷��	
	ADRS  BasicAI_215			// 	���イ��傭
	ADRS  BasicAI_216			// 	�~���N���A�C
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_219			// 	�W���C���{�[��
	ADRS  BasicAI_220			// 	���₵�̂˂���

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_222			// 	������̂߂���
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_225			// 	��������
	ADRS  BasicAI_226			// 	�ڂ���
	ADRS  BasicAI_227			//	���^���o�[�X�g 
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_232			// 	����������
	ADRS  BasicAI_233			// 	�Ȃ�����
	ADRS  BasicAI_234			// 	�T�C�R�V�t�g
	ADRS  BasicAI_235			// 	����ӂ�
	ADRS  BasicAI_236			// 	�����ӂ��ӂ���
	ADRS  BasicAI_237			// 	���ڂ�Ƃ�
	ADRS  BasicAI_238			// 	�p���[�g���b�N
	ADRS  BasicAI_239			// 	������
	ADRS  BasicAI_240			// 	���܂��Ȃ�

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_242			// 	�܂˂���
	ADRS  BasicAI_243			// 	�p���[�X���b�v
	ADRS  BasicAI_244			// 	�K�[�h�X���b�v
	ADRS  BasicAI_245			// 	��������
	ADRS  BasicAI_246			// 	�Ƃ��Ă���
	ADRS  BasicAI_247			// 	�Ȃ�݂̃^�l
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_249			// 	�ǂ��т�
	ADRS  BasicAI_250			// 	�n�[�g�X���b�v

	ADRS  BasicAI_251			// 	�A�N�A�����O
	ADRS  BasicAI_252			// 	�ł񂶂ӂ䂤
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_258			// 	����΂炢
	ADRS  BasicAI_259			// 	�g���b�N���[��
	ADRS  BasicAI_Dummy	//	  �_�~�[

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_265			// 	�䂤�킭
	ADRS  BasicAI_266			// 	�X�e���X���b�N
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_270			// 	�݂��Â��̂܂�

	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
	ADRS  BasicAI_Dummy	//	  �_�~�[
//WB���ǉ��V�[�P���X
	ADRS  BasicAI_277	// 	�߂Ƃ�
	ADRS  BasicAI_278	// 	���C�h�K�[�h
	ADRS  BasicAI_279	// 	�K�[�h�V�F�A
	ADRS  BasicAI_280	// 	�p���[�V�F�A

	ADRS  BasicAI_281	// 	�����_�[���[��
	ADRS  BasicAI_282	// 	�T�C�R�V���b�N
	ADRS  BasicAI_283	// 	�x�m���V���b�N
	ADRS  BasicAI_284	//	�{�f�B�p�[�W 	
	ADRS  BasicAI_285	// 	�e���L�l�V�X
	ADRS  BasicAI_286	// 	�}�W�b�N���[��
	ADRS  BasicAI_287	// 	�������Ƃ�
	ADRS  BasicAI_288	// 	��܂��炵
	ADRS  BasicAI_289	// 	�͂�����ق̂�
	ADRS  BasicAI_290	// 	���傤�̂܂�

	ADRS  BasicAI_291	// 	�w�r�[�{���o�[
	ADRS  BasicAI_292	// 	�V���N���m�C�Y
	ADRS  BasicAI_293	// 	�G���N�g�{�[��
	ADRS  BasicAI_294	// 	�݂��т���
	ADRS  BasicAI_295	// 	�j�g���`���[�W
	ADRS  BasicAI_296	// 	�A�V�b�h�{��
	ADRS  BasicAI_297	// 	�C�J�T�}
	ADRS  BasicAI_298	// 	�V���v���r�[��
	ADRS  BasicAI_299	// 	�Ȃ��܂Â���
	ADRS  BasicAI_300	// 	�������ɂǂ���

	ADRS  BasicAI_301	// 	��񂵂傤
	ADRS  BasicAI_302	// 	�G�R�[�{�C�X
	ADRS  BasicAI_303	// 	�Ȃ�������
	ADRS  BasicAI_304	// 	�N���A�X���b�O
	ADRS  BasicAI_305	// 	�A�V�X�g�p���[
	ADRS  BasicAI_306	// 	�t�@�X�g�K�[�h
	ADRS  BasicAI_307	// 	�T�C�h�`�F���W
	ADRS  BasicAI_308	// 	�������Ԃ�
	ADRS  BasicAI_309	// 	���₵�̂͂ǂ�
	ADRS  BasicAI_310	// 	�������

	ADRS  BasicAI_311	// 	�t���[�t�H�[��
	ADRS  BasicAI_312	// 	�M�A�`�F���W
	ADRS  BasicAI_313	// 	�Ƃ����Ȃ�
	ADRS  BasicAI_314	// 	�₫����
	ADRS  BasicAI_315	// 	����������
	ADRS  BasicAI_316	// 	�������傤
	ADRS  BasicAI_317	// 	�A�N���o�b�g
	ADRS  BasicAI_318	// 	�~���[�^�C�v
	ADRS  BasicAI_319	// 	����������
	ADRS  BasicAI_320	// 	���̂�����

	ADRS  BasicAI_321	// 	�ق����
	ADRS  BasicAI_322	// 	�Ƃ�����܂�
	ADRS  BasicAI_323	// 	�M�t�g�p�X
	ADRS  BasicAI_324	// 	�݂��̂�����
	ADRS  BasicAI_325	// 	�ق̂��̂�����
	ADRS  BasicAI_326	// 	�����̂�����
	ADRS  BasicAI_327	// 	�ӂ邢���Ă�
	ADRS  BasicAI_328	// 	�R�b�g���K�[�h
	ADRS  BasicAI_329	// 	���ɂ����̂���
	ADRS  BasicAI_330	// 	�������邹����

	ADRS  BasicAI_331	// 	�t���[�Y�{���g
	ADRS  BasicAI_332	// 	�R�[���h�t���A
	ADRS  BasicAI_333	// 	�ǂȂ����

BasicAI_Dummy:	//	  �_�~�[
BasicAISeq_end:
	AIEND


//--------------------------------------------------

BasicAI_187:// 	������
BasicAI_1://���点��
	
	IF_POKESICK	CHECK_DEFENCE,AI_DEC10	// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�

	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI, AI_DEC10	// ����҂̂܂���
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HUMIN,AI_DEC10		//2006.6.14�ǉ�
	IF_EQUAL	TOKUSYU_YARUKI,AI_DEC10		//2006.6.14�ǉ�
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC10		//2010.2.13�ǉ�

	AIEND

BasicAI_7://����
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	

	CHECK_TOKUSEI	CHECK_ATTACK				//������Ԃ�Ȃ�A���߂肯�𖳎�
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_7_katayaburi	

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_SIMERIKE,AI_DEC10		// ���߂肯

BasicAI_7_katayaburi://������Ԃ�
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IFN_EQUAL	0,BasicAI_7_end					// �����̍T��������

	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IFN_EQUAL	0,AI_DEC10
	JUMP		AI_DEC1

BasicAI_7_end:
	AIEND

BasicAI_107://������
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_AKUMU,AI_DEC10		// �����ޒ�
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_NEMURI,AI_DEC8		//�Q�Ă��Ȃ�����ɂ͂����Ȃ�
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10				// �}�W�b�N�K�[�h	2006.6.6
	AIEND

BasicAI_8://��߂���
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_NEMURI,AI_DEC8		//�Q�Ă��Ȃ�����ɂ͂����Ȃ�
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10				//���ʂȂ��Z�͂����Ȃ�	
	AIEND

BasicAI_142://�͂炾����
	IF_HP_UNDER	CHECK_ATTACK,51,AI_DEC10		// HP��1/2�ȉ�

BasicAI_10://���������P�i�K�A�b�v         
BasicAI_50://��������2�i�K�A�b�v         
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	AIEND

BasicAI_156:		// 	�܂邭�Ȃ�
BasicAI_11://�ڂ�����P�i�K�A�b�v
BasicAI_51://�ڂ�����2�i�K�A�b�v
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC10
	AIEND

BasicAI_12://���΂₳�P�i�K�A�b�v
BasicAI_52://���΂₳2�i�K�A�b�v
	FLDEFF_CHECK	BTL_FLDEFF_TRICKROOM,AI_DEC10		//�g���b�N���[����	

	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC10
	AIEND
	
BasicAI_13://�Ƃ������P�i�K�A�b�v
BasicAI_53://�Ƃ�����2�i�K�A�b�v
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEPOW,12,AI_DEC10
	AIEND

BasicAI_14://�Ƃ��ڂ��P�i�K�A�b�v
BasicAI_54://�Ƃ��ڂ�2�i�K�A�b�v
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEDEF,12,AI_DEC10
	AIEND

BasicAI_15://�������P�i�K�A�b�v
BasicAI_55://������2�i�K�A�b�v
	CHECK_TOKUSEI	CHECK_DEFENCE				
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10	//�G�m�[�K�[�h�Ȃ疽�����グ�Ȃ�2006.6.8
	CHECK_TOKUSEI	CHECK_ATTACK				
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10	//�����m�[�K�[�h�Ȃ疽�����グ�Ȃ�2006.6.8

	IF_PARA_EQUAL	CHECK_ATTACK,PARA_HIT,12,AI_DEC10
	AIEND

BasicAI_108:		// 	���������Ȃ�
BasicAI_16://��𗦂P�i�K�A�b�v
BasicAI_56://���2�i�K�A�b�v
	CHECK_TOKUSEI	CHECK_DEFENCE				
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10	//�G�m�[�K�[�h�Ȃ��𗦏グ�Ȃ�2006.6.8
	CHECK_TOKUSEI	CHECK_ATTACK				
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10	//�����m�[�K�[�h�Ȃ��𗦏グ�Ȃ�2006.6.8

	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AVOID,12,AI_DEC10
	AIEND

BasicAI_18://���������P�i�K������
BasicAI_58://��������2�i�K������
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_POW,0,AI_DEC10

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAKENKI,AI_DEC12		// �܂���2010.2.13

	CHECK_TOKUSEI	CHECK_ATTACK				//������Ԃ�Ȃ�A�����肫�o�T�~�𖳎�  //2010.2.9
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_KURIABODHI	

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KAIRIKIBASAMI,AI_DEC10		// �����肫�o�T�~
	JUMP	BasicAI_KURIABODHI

BasicAI_19://�ڂ�����P�i�K������
BasicAI_59://�ڂ�����2�i�K������
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_DEF,0,AI_DEC10

	CHECK_TOKUSEI	CHECK_ATTACK				//������Ԃ�Ȃ�A�͂Ƃނ˂𖳎�  //2010.2.13
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_KURIABODHI	

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HATOMUNE,AI_DEC10		// �͂Ƃނ�2010.2.13

	JUMP	BasicAI_KURIABODHI

BasicAI_20://���΂₳�P�i�K������
BasicAI_60://���΂₳2�i�K������
	FLDEFF_CHECK	BTL_FLDEFF_TRICKROOM,AI_DEC10		//�g���b�N���[����	
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_AGI,0,AI_DEC10

	CHECK_HAVE_TOKUSEI	CHECK_DEFENCE, TOKUSYU_KASOKU
	IF_EQUAL	HAVE_YES,	AI_DEC10

	JUMP	BasicAI_KURIABODHI

BasicAI_21://�Ƃ������P�i�K������
BasicAI_61://�Ƃ�����2�i�K������
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEPOW,0,AI_DEC10
	JUMP	BasicAI_KURIABODHI
BasicAI_22://�Ƃ��ڂ��P�i�K������
BasicAI_62://�Ƃ��ڂ�2�i�K������
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEDEF,0,AI_DEC10
	JUMP	BasicAI_KURIABODHI
BasicAI_23://�������P�i�K������
BasicAI_63://������2�i�K������
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_HIT,0,AI_DEC10
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10		// �m�[�K�[�h2006.6.8
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10		// �m�[�K�[�h2006.6.8

	CHECK_TOKUSEI	CHECK_ATTACK				//������Ԃ�Ȃ�A����ǂ��߂𖳎�  //2010.2.9
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_KURIABODHI	

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_SURUDOIME,AI_DEC10		// ����ǂ���

	JUMP	BasicAI_KURIABODHI

BasicAI_24://��𗦂P�i�K������
BasicAI_64://���2�i�K������
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_AVOID,0,AI_DEC10

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10		// �m�[�K�[�h2006.6.8

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10		// �m�[�K�[�h2006.6.8

BasicAI_KURIABODHI://�N���A�{�f�B�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_AMANOZYAKU,AI_DEC12		// ���܂̂��Ⴍ2010.2.13

	CHECK_TOKUSEI	CHECK_ATTACK				//������Ԃ�Ȃ�A�N���A�{�f�B�A���낢���ނ�𖳎�  //2010.2.9
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_KURIABODHI_end	

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KURIABODHI,AI_DEC10		// �N���A�{�f�B
	IF_EQUAL	TOKUSYU_SIROIKEMURI,AI_DEC10	// ���낢���ނ�
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_KURIABODHI_end:
	AIEND

BasicAI_250://	�n�[�g�X���b�v
BasicAI_143://	��������
BasicAI_25://���낢���ނ�F�U�������U����ŁA��������U��艺�Ȃ猸�Z
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,6,BasicAI_25_END
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,6,BasicAI_25_END
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AGI,6,BasicAI_25_END
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,6,BasicAI_25_END
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,6,BasicAI_25_END
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,6,BasicAI_25_END
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,6,BasicAI_25_END

	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,6,BasicAI_25_END
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,6,BasicAI_25_END
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AGI,6,BasicAI_25_END
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,6,BasicAI_25_END
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,6,BasicAI_25_END
	IF_PARA_OVER	CHECK_DEFENCE,PARA_HIT,6,BasicAI_25_END
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,6,BasicAI_25_END
	JUMP		AI_DEC10

BasicAI_25_END:
	AIEND

BasicAI_28://�ӂ��Ƃ΂�
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,AI_DEC10			//	�T�������Ȃ�
	
	CHECK_TOKUSEI	CHECK_ATTACK				//2006.6.6
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_28_katayaburi	// ������Ԃ�`�F�b�N
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KYUUBAN,AI_DEC10	// ���イ�΂�
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_28_katayaburi:
	AIEND


//�݂̂��ނ�162�A���߂�J�E���g���Ń`�F�b�N��������ł���
BasicAI_132://�����̂Ђ���
BasicAI_133://������������
BasicAI_134://���̂Ђ���
BasicAI_157:// ���܂�����
BasicAI_214://�͂˂₷��2006.6.14
BasicAI_32://������������2006.6.14
	IFN_HP_EQUAL	CHECK_ATTACK,100,BasicAI_32_END
	INCDEC		-8
BasicAI_32_END:
	AIEND





BasicAI_66:		//	�ǂ�33
BasicAI_33: //�ǂ��ǂ�
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HAGANE,AI_DEC10				// ���肪�͂��˃^�C�v
	IF_EQUAL	POKETYPE_DOKU,AI_DEC10			// ���肪�ǂ��^�C�v 
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HAGANE,AI_DEC10				// ���肪�͂��˃^�C�v
	IF_EQUAL	POKETYPE_DOKU,AI_DEC10			// ���肪�ǂ��^�C�v

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MENEKI,AI_DEC10			// �߂񂦂�
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10	// �}�W�b�N�K�[�h	2006.6.6
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,AI_DEC10	// �|�C�Y���q�[��	2006.6.6

	CHECK_TOKUSEI	CHECK_ATTACK				//2010.2.9
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_33_NO_RIIHUGAADO	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
 	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�
	IFN_EQUAL	TOKUSYU_RIIHUGAADO,BasicAI_33_NO_RIIHUGAADO	// 	2006.6.7
	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,AI_DEC10			//�@���[�t�K�[�h�ł͂ꒆ

BasicAI_33_NO_RIIHUGAADO:
	CHECK_TOKUSEI	CHECK_DEFENCE
	IFN_EQUAL	TOKUSYU_URUOIBODHI,BasicAI_33_NO_URUOIBODHI	// 	2006.6.7
	CHECK_WEATHER
	IF_EQUAL	WEATHER_AME,AI_DEC10			//�@���邨���{�f�B�ŉJ
	
BasicAI_33_NO_URUOIBODHI:
	IF_POKESICK	CHECK_DEFENCE,AI_DEC10	// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI, AI_DEC10	// ����҂̂܂���
	AIEND

BasicAI_35://�Ђ���̂���
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_HIKARINOKABE,AI_DEC8	// �Ђ���̂��ג�
	AIEND

BasicAI_38://�ꌂ�K�E
	
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	
	
	CHECK_TOKUSEI	CHECK_ATTACK				//2006.6.6
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_38_katayaburi	// ������Ԃ�`�F�b�N
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_GANZYOU,AI_DEC10		// ���񂶂傤

BasicAI_38_katayaburi:
	IF_LEVEL	LEVEL_DEFENCE,AI_DEC10			// ����̕������x��������
	AIEND

BasicAI_126:	// �}�O�j�`���[�h
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_126_katayaburi	// ������Ԃ�`�F�b�N
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HUYUU,AI_DEC10			// �ӂ䂤

BasicAI_126_katayaburi:                   //�@��COMP_POWER���ł���܂ŕۗ�

BasicAI_26:	// ���܂�
BasicAI_39:	// ���܂�����
BasicAI_40:	// ������̂܂���
BasicAI_80:	// �͂�����������
BasicAI_87:	// �����イ�Ȃ�
BasicAI_88:	// �T�C�R�E�F�[�u
BasicAI_89:	// �J�E���^�[
BasicAI_99:	// �����΂�
BasicAI_121:// ���񂪂���
BasicAI_122:// �v���[���g
BasicAI_123:// �������
BasicAI_130:// �\�j�b�N�u�[��
BasicAI_144:// �~���[�R�[�g
BasicAI_145:// ���P�b�g����
BasicAI_170:// �������p���`
BasicAI_182:// �΂�������
BasicAI_189:// ���ނ����
BasicAI_196:// ��������

BasicAI_219:// �W���C���{�[��
BasicAI_235:// ����ӂ�
BasicAI_237:// ���ڂ�Ƃ�
BasicAI_245:// ��������
BasicAI_135:// �߂��߂�p���[
BasicAI_41:// ��イ�̂�����
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	
//���󂾂Ɣ{���O���ƌ^�j��ŕs�v�c�Ȏ����т���̂ɂł��Ȃ�

	CHECK_TOKUSEI	CHECK_DEFENCE
	IFN_EQUAL	TOKUSYU_HUSIGINAMAMORI,BasicAI_196_end	// �ӂ����Ȃ܂���

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_196_end	// ������Ԃ�2006.6.7
	CHECK_WAZA_AISYOU	AISYOU_2BAI,BasicAI_196_end	// ���ʂ΂��񂵂�������Ȃ�	
	CHECK_WAZA_AISYOU	AISYOU_4BAI,BasicAI_196_end	// ���ʂ΂��񂵂�������Ȃ�	
	JUMP	AI_DEC10

BasicAI_196_end:
	AIEND


BasicAI_46://���낢����
	IF_SIDEEFF	CHECK_ATTACK,BTL_SIDEEFF_SIROIKIRI,AI_DEC8		// ���낢���蒆
	AIEND

BasicAI_47://����������
  IF_CONTFLG	CHECK_ATTACK,BPP_CONTFLG_KIAIDAME,AI_DEC10		// ���������ߒ�
	AIEND

BasicAI_166:// 	�����Ă�
BasicAI_118://���΂�
BasicAI_49://����������
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KONRAN,AI_DEC5		//�������͍����U�������Ȃ�
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI, AI_DEC10	// ����҂̂܂���

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAIPEESU,AI_DEC10		// �}�C�y�[�X

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_49_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_49_end:
	AIEND

BasicAI_65://���t���N�^�[
	IF_SIDEEFF	CHECK_ATTACK,BTL_SIDEEFF_REFRECTOR,AI_DEC8	// ���t���N�^�[��
	AIEND

BasicAI_67://�܂Ђ�����
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_ZYUUNAN,AI_DEC10		// ���イ�Ȃ�
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10	// �}�W�b�N�K�[�h
	
	CHECK_TOKUSEI	CHECK_ATTACK				//2006.6.6
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_67_katayaburi	// ������Ԃ�`�F�b�N
		
	IF_WAZANO	WAZANO_DENZIHA,BasicAI_67_1	// �ł񂶂�
	JUMP	BasicAI_67_katayaburi

BasicAI_67_1:
	CHECK_TOKUSEI	CHECK_DEFENCE				//2006.6.6
	IF_EQUAL	TOKUSYU_DENKIENZIN,AI_DEC10	// �ł񂫃G���W��
	IF_EQUAL	TOKUSYU_TIKUDEN,AI_DEC10	// �����ł�
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�
	
BasicAI_67_katayaburi://������Ԃ�X���[
	IF_POKESICK	CHECK_DEFENCE,AI_DEC10	// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�

	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI, AI_DEC10	// ����҂̂܂���
	AIEND

BasicAI_79://�݂����
	IF_MIGAWARI	CHECK_ATTACK,AI_DEC8		// �݂���蒆
	IF_HP_UNDER	CHECK_ATTACK,26,AI_DEC10				// HP��1/4�ȉ�
	AIEND

BasicAI_84://��ǂ肬�̃^�l
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,AI_DEC10	// ��ǂ�ꒆ
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_KUSA,AI_DEC10				// ���肪�����^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_KUSA,AI_DEC10				// ���肪�����^�C�v
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10		// �}�W�b�N�K�[�h	2006.6.6
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�
	AIEND
	
BasicAI_86://���Ȃ��΂�
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KANASIBARI,AI_DEC8

	CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_86_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�
BasicAI_86_end:
	AIEND

BasicAI_90://�A���R�[��
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_ENCORE,AI_DEC8

	CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_90_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�
BasicAI_90_end:
	AIEND

BasicAI_92://���т��A�˂���
	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_NEMURI,AI_DEC8		//�Q�Ă��Ȃ����͂����Ȃ�
	AIEND

BasicAI_94://���b�N�I��2006.6.14
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MUSTHIT_TARGET,AI_DEC10			//���b�N�I����
	CHECK_TOKUSEI	CHECK_ATTACK				
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10
	CHECK_TOKUSEI	CHECK_DEFENCE				
	IF_EQUAL	TOKUSYU_NOOGAADO,AI_DEC10
	AIEND

BasicAI_106://�N���̂��A���낢�܂Ȃ����A�Ƃ�����ڂ�
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_TOOSENBOU,AI_DEC10	// ���낢�܂Ȃ�����
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_106_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_106_end:
	AIEND

BasicAI_109://�̂낢
	//2006.6.7------------------------------------------------------
	//�������S�[�X�g�^�C�v���`�F�b�N
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_GHOST,BasicAI_109_ghost	// �S�[�X�g�^�C�v
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_GHOST,BasicAI_109_ghost	// �S�[�X�g�g�^�C�v
	//--------------------------------------------------------------
	
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC8
	AIEND
	
	
BasicAI_109_ghost://�S�[�X�g�^�̂낢2006.6.7
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,AI_DEC10	// �̂낢��
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10		// �}�W�b�N�K�[�h	2006.6.6
	AIEND

BasicAI_112://�܂��т�
	//IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,AI_DEC10
	//2006.6.14�܂��т����R��T���Ă�����
	CHECK_SIDEEFF_COUNT	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI
	IF_EQUAL	3,AI_DEC10
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,AI_DEC10			//	�G�̍T�������Ȃ�

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_112_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_112_end:
	AIEND

BasicAI_113://�݂�Ԃ�
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MIYABURU,AI_DEC10	// �݂�Ԃ钆
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_113_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_113_end:
	AIEND

BasicAI_114://�ق�т̂���
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_HOROBINOUTA,AI_DEC10	// �ق�ђ�
	AIEND

BasicAI_115://���Ȃ��炵
	CHECK_WEATHER
	IF_EQUAL	WEATHER_SUNAARASHI,AI_DEC8	// ���Ȃ��炵��
	AIEND

BasicAI_120://��������
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MEROMERO,AI_DEC10		//����������
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_DONKAN,AI_DEC10				// �ǂ񂩂�

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_120_katayaburi	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_120_katayaburi:
	CHECK_POKESEX	CHECK_ATTACK
	IF_EQUAL	PTL_SEX_MALE,BasicAI_120_MALE
	IF_EQUAL	PTL_SEX_FEMALE,BasicAI_120_FEMALE
	JUMP		AI_DEC10
BasicAI_120_MALE:
	CHECK_POKESEX	CHECK_DEFENCE
	IF_EQUAL	PTL_SEX_FEMALE,BasicAI_120_END
	JUMP		AI_DEC10
BasicAI_120_FEMALE:
	CHECK_POKESEX	CHECK_DEFENCE
	IF_EQUAL	PTL_SEX_MALE,BasicAI_120_END
	JUMP		AI_DEC10
BasicAI_120_END:
	AIEND

BasicAI_124://����҂̂܂���
	IF_SIDEEFF	CHECK_ATTACK,BTL_SIDEEFF_SINPINOMAMORI,AI_DEC8	// ����҂̂܂��蒆
	AIEND

BasicAI_168://�����݂₰
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_AMANOZYAKU,AI_DEC12		// ���܂̂��Ⴍ2010.2.13

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_168_check

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KURIABODHI,AI_DEC10		// �N���A�{�f�B
	IF_EQUAL	TOKUSYU_SIROIKEMURI,AI_DEC10	// ���낢���ނ�

BasicAI_168_check://�����݂₰
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_POW,0,AI_DEC10
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEPOW,0,AI_DEC8
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,AI_DEC10			//	�����̍T�������Ȃ�
	AIEND


BasicAI_127://�o�g���^�b�`
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,AI_DEC10			//	�����̍T�������Ȃ�
	AIEND

BasicAI_136://���܂���

	CHECK_TOKUSEI	CHECK_ATTACK	//2006.06.06
	IF_EQUAL	TOKUSYU_SUISUI,BasicAI_136_2		// ��������
	IF_EQUAL	TOKUSYU_URUOIBODHI,BasicAI_136_2		// ���邨���{�f�B
	
	CHECK_TOKUSEI	CHECK_DEFENCE	//2006.06.06
	IFN_EQUAL	TOKUSYU_URUOIBODHI,BasicAI_136_2		// ���邨���{�f�B�łȂ�
	IF_POKESICK	CHECK_DEFENCE,AI_DEC8	// ��Ԉُ펞
	
BasicAI_136_2:
	CHECK_WEATHER
	IF_EQUAL	WEATHER_AME,AI_DEC8		// ���ߒ�
	AIEND

BasicAI_137://�ɂق�΂�

//	CHECK_TOKUSEI	CHECK_ATTACK	//2006.06.06           // �͂�ɂȂ��Ă����Ȃ� 2010.2.9
//	IF_EQUAL	TOKUSYU_HURAWAAGIHUTO,BasicAI_137_2		// �t�����[�M�t�g
//	IF_EQUAL	TOKUSYU_RIIHUGAADO,BasicAI_137_2		// ���[�t�K�[�h
//	IF_EQUAL	TOKUSYU_SANPAWAA,BasicAI_137_2			// �T���p���[
//	CHECK_TOKUSEI	CHECK_DEFENCE	//2006.06.06
//	IFN_EQUAL	TOKUSYU_URUOIBODHI,BasicAI_137_2		// ���[�t�K�[�h�łȂ�
//	IF_POKESICK	CHECK_DEFENCE,AI_DEC10	// ��Ԉُ펞
//BasicAI_137_2:

	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,AI_DEC8			//�@�͂ꒆ
	AIEND

BasicAI_148://�݂炢�悿
	IF_MIRAIYOCHI	CHECK_DEFENCE,  AI_DEC12 // �݂炢�悿��
	//INCDEC 5	//�݂炢�悿���łȂ��ꍇ���Z�������폜2006.8.10
	AIEND

BasicAI_158://�˂����܂�
	CHECK_NEKODAMASI	CHECK_ATTACK//�P���Ƃł���
	IF_EQUAL	0,AI_DEC10
	AIEND

BasicAI_160://�����킦��
	CHECK_TAKUWAERU		CHECK_ATTACK
	IF_EQUAL	3,AI_DEC10
	AIEND

BasicAI_161://�͂�����                    //���������\��i�S�[�X�g�A�ӂ����Ȃ܂���j
BasicAI_162://�݂̂���                    //���������\��i�S�[�X�g�A�ӂ����Ȃ܂���j
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	

	CHECK_TAKUWAERU		CHECK_ATTACK
	IF_EQUAL	0,AI_DEC10
	
	IF_WAZA_SEQNO_JUMP	162,BasicAI_32	// �݂̂��ނ�������
	AIEND


BasicAI_164://�����
	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,AI_DEC8		//�@����ꒆ

//                                              ����Ȃ�Basic����Ȃ��@2010.2.9
//	CHECK_TOKUSEI	CHECK_DEFENCE	//2006.6.5
//	IFN_EQUAL	TOKUSYU_AISUBODHI,BasicAI_164_1	// �A�C�X�{�f�B
//	INCDEC		-8
//
//	CHECK_TOKUSEI	CHECK_ATTACK	//2006.6.5
//	IFN_EQUAL	TOKUSYU_AISUBODHI,BasicAI_164_1	// �A�C�X�{�f�B
//	INCDEC		8

BasicAI_164_1:
	AIEND


BasicAI_165://���������
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_ICHAMON,AI_DEC10	//���������

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_165_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_165_end:
	AIEND

BasicAI_167://�₯�ǂ����킹��
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,AI_DEC10	// �݂��̃x�[��
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10	// �}�W�b�N�K�[�h
	IF_POKESICK	CHECK_DEFENCE,AI_DEC10	// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,AI_DEC10			// �ق̂��^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,AI_DEC10			// �ق̂��^�C�v	
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI, AI_DEC10	// ����҂̂܂���

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_167_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_167_end:
	AIEND

BasicAI_172:			// 	���̂�тƂ܂�
  CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE,AI_DEC10
  AIEND

BasicAI_176://�Ă�����
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_188://�͂������Ƃ�
BasicAI_177://�g���b�N
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NENTYAKU,AI_DEC10		// �˂񂿂Ⴍ
	
	CHECK_SOUBI_ITEM	CHECK_DEFENCE
	IF_EQUAL	0,AI_DEC10
	
	AIEND

BasicAI_181://�˂��͂�
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_NEWOHARU,AI_DEC10	// �˂��͂钆
	AIEND

BasicAI_184://���T�C�N��
	CHECK_RECYCLE_ITEM	CHECK_ATTACK
	IF_EQUAL	0,AI_DEC10
	AIEND

BasicAI_192://�ӂ�����
	FLDEFF_CHECK	BTL_FLDEFF_FUIN,AI_DEC10		// �ӂ�����
	AIEND

BasicAI_193://���t���b�V��
	//�ǂ��܂Ђ₯�ǂłȂ����͂����Ȃ�
  IF_WAZASICK CHECK_ATTACK, WAZASICK_DOKU,      BasicAI_193_end
  IF_DOKUDOKU CHECK_ATTACK,                     BasicAI_193_end
  IF_WAZASICK CHECK_ATTACK, WAZASICK_MAHI,      BasicAI_193_end
  IF_WAZASICK CHECK_ATTACK, WAZASICK_YAKEDO,    BasicAI_193_end
  JUMP  AI_DEC10
BasicAI_193_end://���t���b�V��
	AIEND

BasicAI_201://�ǂ날����
	FLDEFF_CHECK	BTL_FLDEFF_DOROASOBI,AI_DEC10	// �ǂ날���ђ�
	AIEND

BasicAI_205://��������
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_AMANOZYAKU,AI_DEC12		// ���܂̂��Ⴍ2010.2.13

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_205_check	// ������Ԃ�
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KURIABODHI,AI_DEC10				// �N���A�{�f�B
	IF_EQUAL	TOKUSYU_SIROIKEMURI,AI_DEC10			// ���낢���ނ�
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_205_check:
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_POW,0,AI_DEC10
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_DEF,0,AI_DEC8
	AIEND

BasicAI_206://�R�X���p���[
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEDEF,12,AI_DEC8
	AIEND

BasicAI_208://�r���h�A�b�v
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC8
	AIEND

BasicAI_210://�݂�������
	FLDEFF_CHECK	BTL_FLDEFF_MIZUASOBI,AI_DEC10	// �݂������ђ�
	AIEND

BasicAI_211://�߂�����
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEPOW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEDEF,12,AI_DEC8
	AIEND

BasicAI_212://��イ�̂܂�
	FLDEFF_CHECK	BTL_FLDEFF_TRICKROOM,AI_DEC10		//�g���b�N���[����	

	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC8
	AIEND

BasicAI_213://�ق����傭2006.6.5:ok
//@todo	IF_CONTFLG	CHECK_ATTACK,BPP_CONTFLG_HOGOSYOKU,AI_DEC10	// �ق����傭��
	AIEND

BasicAI_215://���イ��傭2006.6.5�Fok
	FLDEFF_CHECK	BTL_FLDEFF_JURYOKU,AI_DEC10	//���イ��傭���͂��Ȃ�	
	AIEND

BasicAI_216://�~���N���A�C2006.6.5:ok
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MIYABURU,AI_DEC10	// �~���N���A�C��

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_216_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_216_end:
	AIEND

BasicAI_220://���₵�̂˂���2006.6.5
	//�J��o��AI�����Ή��̂���AI�ł͎g�p�s�ɂ���2006.8.10         ���������\��
	INCDEC	-20
	
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,AI_DEC10			//	�����̍T�������Ȃ�
	IF_BENCH_COND	CHECK_ATTACK,BasicAI_220_end//�T���ɏ�Ԉُ킪���Ȃ�
	//�T���|�P�����ɂg�o�񕜂ł��邩
	IF_BENCH_HPDEC	CHECK_ATTACK,BasicAI_220_end
	JUMP		AI_DEC10
BasicAI_220_end:
	AIEND


BasicAI_222://������̂߂���2006.6.5ok
	CHECK_SOUBI_ITEM	CHECK_ATTACK
	IFN_TABLE_JUMP	BasicAI_222_Table,AI_DEC10	//�؂̎����Ȃ�
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	
	AIEND

BasicAI_222_Table://�؂̎��i���o�[
	.long	149,150,151,152,153,154,155,156,157,158,
	.long	159,160,161,162,163,164,165,166,167,168,
	.long	169,170,171,172,173,174,175,176,177,178,
	.long	179,180,181,182,183,184,185,186,187,188,
	.long	189,190,191,192,193,194,195,196,197,198,
	.long	199,200,201,202,203,204,205,206,207,208,
	.long	209,210,211,212
	.long	TR_AI_TABLE_END


BasicAI_225://��������2006.6.5ok
	FLDEFF_CHECK	BTL_FLDEFF_TRICKROOM,AI_DEC8		//�g���b�N���[����	
	IF_SIDEEFF	CHECK_ATTACK,BTL_SIDEEFF_OIKAZE,AI_DEC10	// ����������
	AIEND

BasicAI_226://�ڂ���2006.6.5ok
	//�p�����[�^�[�l�`�w���͂��Ȃ�

	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEPOW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEDEF,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AVOID,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_HIT,12,AI_DEC10
	AIEND


BasicAI_227://���^���o�[�X�g2006.6.5ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_ATODASI,AI_DEC10		//���Ƃ���
  CHECK_SOUBI_EQUIP CHECK_DEFENCE
	IF_EQUAL	SOUBI_KOUKOUNINARU,AI_DEC10	//���������̂�����
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_ATODASI,BasicAI_227_end		//���������Ƃ���
  CHECK_SOUBI_EQUIP CHECK_ATTACK
	IF_EQUAL	SOUBI_KOUKOUNINARU,BasicAI_227_end	//���������̂�����
	
	IF_FIRST	IF_FIRST_ATTACK,AI_DEC10//�����������ꍇ�͂��Ȃ�

BasicAI_227_end:
	AIEND


BasicAI_232://����������2006.6.5ok
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_SASIOSAE,AI_DEC10	//������������

	CHECK_RECYCLE_ITEM	CHECK_DEFENCE//���T�C�N���ł��Ȃ����A�C�e������Ă��Ȃ�
	IF_EQUAL	0,BasicAI_232_end
	CHECK_BTL_COMPETITOR
	IF_EQUAL	BTL_COMPETITOR_SUBWAY, AI_DEC10

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_232_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_232_end:
	AIEND

BasicAI_233://�Ȃ�����2006.6.5ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,AI_DEC10	//���ʂȂ��Z�͂����Ȃ�	
	CHECK_NAGETSUKERU_IRYOKU	CHECK_ATTACK
	IF_UNDER	10,AI_DEC10
	
	//�}���`�^�C�v�̏ꍇ���s����̂ł��Ȃ�
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MARUTITAIPU,AI_DEC10	

	CHECK_SOUBI_EQUIP	CHECK_ATTACK
	IF_TABLE_JUMP	BasicAI_233_Table1,BasicAI_233_doku
	IF_TABLE_JUMP	BasicAI_233_Table2,BasicAI_233_yakedo
	IF_TABLE_JUMP	BasicAI_233_Table3,BasicAI_233_mahi
	AIEND

BasicAI_233_doku:
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,BasicAI_233_doku_check		// ����҂̂܂���ok
	IF_POKESICK	CHECK_DEFENCE,BasicAI_233_doku_check			// ���肪��Ԉُ�̏ꍇ���Ȃ�ok
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,BasicAI_233_doku_check					// �������|�C�Y���q�[���Ȃ疳��ok
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_DOKU,BasicAI_233_doku_check							// �ǂ��^�C�vok
	IF_EQUAL	POKETYPE_HAGANE,BasicAI_233_doku_check							// �͂��˃^�C�vok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_DOKU,BasicAI_233_doku_check							// �ǂ��^�C�vok
	IF_EQUAL	POKETYPE_HAGANE,BasicAI_233_doku_check							// �͂��˃^�C�vok
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MENEKI,BasicAI_233_doku_check						// �߂񂦂��͖����Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,BasicAI_233_doku_check					// �|�C�Y���q�[���͖����Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,BasicAI_233_doku_check			// ���肪�}�W�b�N�K�[�h�Ȃ��Ȃ�
	AIEND

BasicAI_233_doku_check:
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_SINPINOMAMORI,AI_DEC5	// ����҂̂܂���ok
	IF_POKESICK	CHECK_ATTACK,AI_DEC5				// ��Ԉُ�̏ꍇ���Ȃ�ok
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_DOKU,AI_DEC5						// �ǂ��^�C�vok
	IF_EQUAL	POKETYPE_HAGANE,AI_DEC5						// �͂��˃^�C�vok
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_DOKU,AI_DEC5						// �ǂ��^�C�vok
	IF_EQUAL	POKETYPE_HAGANE,AI_DEC5						// �͂��˃^�C�vok
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_BUKIYOU,AI_DEC5					// �Ԃ��悤
	IF_EQUAL	TOKUSYU_MENEKI,AI_DEC5					// �߂񂦂�
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,AI_DEC5				// �|�C�Y���q�[��
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC5			// �}�W�b�N�K�[�h
	IF_EQUAL	TOKUSYU_KONZYOU,AI_DEC5					// ���񂶂傤
//	INCDEC	+3	//�댯�Ȃ̂œ�����                              Basic�ł͈���Ȃ��B  2010.2.10

	AIEND


BasicAI_233_yakedo:
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,BasicAI_233_yakedo_check	// ����҂̂܂���
	IF_POKESICK	CHECK_DEFENCE,BasicAI_233_yakedo_check			// ���肪��Ԉُ�̏ꍇ���Ȃ�ok
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,BasicAI_233_yakedo_check						// �ق̂��^�C�vok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,BasicAI_233_yakedo_check						// �ق̂��g�^�C�vok
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,BasicAI_233_yakedo_check			// ���肪�}�W�b�N�K�[�h�Ȃ��Ȃ�
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,BasicAI_233_yakedo_check			// �݂��̃x�[��
	AIEND

BasicAI_233_yakedo_check:
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_SINPINOMAMORI,AI_DEC5	// ����҂̂܂���
	IF_POKESICK	CHECK_ATTACK,AI_DEC5				// ��Ԉُ�̏ꍇ���Ȃ�ok
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_HONOO,AI_DEC5						// �ق̂��^�C�vok
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_HONOO,AI_DEC5						// �ق̂��g�^�C�vok
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_BUKIYOU,AI_DEC5					// �Ԃ��悤
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC5			// �}�W�b�N�K�[�h
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,AI_DEC5				// �݂��̃x�[��
	IF_EQUAL	TOKUSYU_KONZYOU,AI_DEC5					// ���񂶂傤
//	INCDEC	+3	//�댯�Ȃ̂œ�����                              Basic�ł͈���Ȃ��B  2010.2.10

	AIEND

BasicAI_233_mahi:
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,AI_DEC5	// ����҂̂܂���
	IF_POKESICK	CHECK_DEFENCE,AI_DEC5			// ���肪��Ԉُ�̏ꍇ���Ȃ�
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_ZYUUNAN,AI_DEC5					// ���イ�Ȃ�͖����Ȃ̂ł��Ȃ�
	AIEND


BasicAI_233_Table1://�ǂ�
	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	SOUBI_DOKUBARIUP
	.long	TR_AI_TABLE_END

BasicAI_233_Table2://�₯��
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	TR_AI_TABLE_END

BasicAI_233_Table3://�܂�	
	.long	SOUBI_PIKATYUUTOKUKOUNIBAI
	.long	TR_AI_TABLE_END
	

BasicAI_234://�T�C�R�V�t�g2006.6.5
	
	IFN_POKESICK	CHECK_ATTACK,AI_DEC10		// ��������Ԉُ�łȂ��ꍇ���Ȃ�ok
	IF_POKESICK	CHECK_DEFENCE,AI_DEC10			// ���肪��Ԉُ�̏ꍇ���Ȃ�ok

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_234_katayaburi	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC10		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_234_katayaburi:
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI, AI_DEC10	// ����҂̂܂���
	IF_WAZASICK	CHECK_ATTACK, WAZASICK_DOKU,    BasicAI_234_doku	  // ��
	IF_DOKUDOKU	CHECK_ATTACK,                   BasicAI_234_doku	  // �ǂ��ǂ�
	IF_WAZASICK	CHECK_ATTACK, WAZASICK_YAKEDO,  BasicAI_234_yakedo	// �₯��
	IF_WAZASICK	CHECK_ATTACK, WAZASICK_MAHI,    BasicAI_234_mahi	  // �܂�
	JUMP	BasicAI_234_end
	
BasicAI_234_doku:
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,AI_DEC10		// �������|�C�Y���q�[���Ȃ疳��ok
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_DOKU,AI_DEC10		// �ǂ��^�C�vok
	IF_EQUAL	POKETYPE_HAGANE,AI_DEC10			// �͂��˃^�C�vok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_DOKU,AI_DEC10		// �ǂ��^�C�vok
	IF_EQUAL	POKETYPE_HAGANE,AI_DEC10			// �͂��˃^�C�vok
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MENEKI,AI_DEC10		// �߂񂦂��͖����Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,AI_DEC10// �|�C�Y���q�[���͖����Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10// ���肪�}�W�b�N�K�[�h�Ȃ��Ȃ�
	JUMP	BasicAI_234_end
	
	//�₯�ǂ̂Ƃ��A�΃^�C�v�ɂ͂��Ȃ�
BasicAI_234_yakedo:
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,AI_DEC10			// �ق̂��^�C�vok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,AI_DEC10			// �ق̂��^�C�vok
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,AI_DEC10			// ���肪�}�W�b�N�K�[�h�Ȃ��Ȃ�
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,AI_DEC10			// �݂��̃x�[��
	JUMP	BasicAI_234_end

BasicAI_234_mahi:
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_ZYUUNAN,AI_DEC10		// ���イ�Ȃ�͖����Ȃ̂ł��Ȃ�

BasicAI_234_end:
	AIEND


BasicAI_236://�����ӂ��ӂ���2006.6.5ok
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KAIHUKUHUUJI,AI_DEC10	//�����ӂ��ӂ�����

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_236_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_236_end:
	AIEND

BasicAI_238://�p���[�g���b�N2006.6.14ok
	IF_CONTFLG	CHECK_ATTACK,BPP_CONTFLG_POWERTRICK,AI_DEC10	//�p���[�g���b�N��
	AIEND

BasicAI_239://������2006.6.5ok

	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_IEKI,AI_DEC10	// ��������
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MARUTITAIPU,AI_DEC10		// �}���`�^�C�v�͖����Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_NAMAKE,AI_DEC10				// �Ȃ܂��͑��肪�L���Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_YOWAKI,AI_DEC10				// ��킫�͑��肪�L���Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_SUROOSUTAATO,AI_DEC10		// �X���[�X�^�[�g�͑��肪�L���Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_AKUSYUU,AI_DEC10			// �퓬�ňӖ��Ȃ��̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_NIGEASI,AI_DEC10			// �퓬�ňӖ��Ȃ��̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_MONOHIROI,AI_DEC10			// �퓬�ňӖ��Ȃ��̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_MITUATUME,AI_DEC10			// �퓬�ňӖ��Ȃ��̂ł��Ȃ�ok
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_239_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_239_end:
	AIEND

BasicAI_240://���܂��Ȃ�2006.6.5ok
	IF_SIDEEFF	CHECK_ATTACK,BTL_SIDEEFF_OMAJINAI,AI_DEC10	//���܂��Ȃ���
	AIEND

BasicAI_242://�܂˂���2006.6.5ok
	CHECK_TURN
	IFN_EQUAL	0,BasicAI_242_end	//�ŏ��̃^�[��

  IF_FIRST	IF_FIRST_DEFENCE,BasicAI_242_end          //�@����̕����f����  2010.2.11

	IF_FIRST	IF_FIRST_ATTACK,AI_DEC10
BasicAI_242_end:
	AIEND

BasicAI_243://�p���[�X���b�v2006.6.5ok
	//�����莩���̕����i�K���������ꍇ�͂��Ȃ�
	CHECK_STATUS_DIFF	CHECK_DEFENCE,BPP_ATTACK_RANK
	IF_UNDER		1,BasicAI_243_1
	JUMP	BasicAI_243_End

BasicAI_243_1:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,BPP_SP_ATTACK_RANK
	IF_UNDER		1,AI_DEC10

BasicAI_243_End:
	AIEND

BasicAI_244://�K�[�h�X���b�v2006.6.5ok
	//�����莩���̕����i�K���������ꍇ�͂��Ȃ�
	CHECK_STATUS_DIFF	CHECK_DEFENCE,BPP_DEFENCE_RANK
	IF_UNDER		1,BasicAI_244_1
	JUMP	BasicAI_244_End

BasicAI_244_1:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,BPP_SP_DEFENCE_RANK
	IF_UNDER		1,AI_DEC10

BasicAI_244_End:
	AIEND

BasicAI_246://�Ƃ��Ă���2006.6.5ok
	IF_TOTTEOKI	CHECK_ATTACK,BasicAI_246_end		// �Ƃ��Ă����o����
	INCDEC	-10
BasicAI_246_end:
	AIEND


BasicAI_247://�Ȃ�݂̃^�l2006.6.5ok

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NAMAKE,AI_DEC10			// �Ȃ܂��͖����Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_YOWAKI,AI_DEC10				// ��킫�͑��肪�L���Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_HUMIN,AI_DEC10			// �ӂ݂�͖��Ӗ��Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_YARUKI,AI_DEC10			// ��邫�͖��Ӗ��Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_MARUTITAIPU,AI_DEC10	// �}���`�^�C�v�͖��Ӗ��Ȃ̂ł��Ȃ�ok
	
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_NEMURI,BasicAI_247_end	//�˂ނ��Ă��Ȃ�ok

	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_NEGOTO, BasicAI_247_end
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_IBIKI, BasicAI_247_end
	INCDEC	-10

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_247_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_247_end:
	AIEND

BasicAI_249://�ǂ��т�2006.6.5ok
	CHECK_SIDEEFF_COUNT	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI
	IF_EQUAL	2,AI_DEC10
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,AI_DEC10			//	�G�̍T�������Ȃ�
	//�T���ɏ�Ԉُ킪��C�����Ȃ����Ł��|�^�C�v�����Ȃ��i��߂Ă����j
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_249_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_249_end:
	AIEND

BasicAI_251://�A�N�A�����O2006.6.5ok
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_AQUARING,AI_DEC10	// �A�N�A�����O��
	AIEND

BasicAI_252://�ł񂶂ӂ䂤2006.6.5ok
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_FLYING,AI_DEC10	// �ł񂶂ӂ䂤��
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_HUYUU,AI_DEC10		// �ӂ䂤�͖��Ӗ��Ȃ̂ł��Ȃ�
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_HIKOU,AI_DEC10	// �Ђ����^�C�v
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_HIKOU,AI_DEC10	// �Ђ����g�^�C�v

	AIEND

BasicAI_258://����΂炢2006.6.5
	//����̉�𗦂O���A��΂�����̂��Ȃ��ꍇ�͂��Ȃ�	
	IFN_PARA_EQUAL	CHECK_DEFENCE,PARA_AVOID,0,BasicAI_258_END
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_HIKARINOKABE,BasicAI_258_END	// �Ђ���̂���
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_REFRECTOR,BasicAI_258_END	// ���t���N�^�[

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_258_katayaburi	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_258_katayaburi:
	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_HUKAIKIRI,BasicAI_258_END	//�@�ӂ�������ok

//	                                                               ���݂́u����΂炢�v�Łu�܂��т��v���������ĂȂ��B2010.2.11
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,AI_DEC10			//	�G�̍T�������Ȃ�

	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,BasicAI_258_END		// �܂��т���
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,BasicAI_258_END	// �X�e���X���b�N��
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,BasicAI_258_END		// �ǂ��т���
	
	JUMP		AI_DEC10

BasicAI_258_END:
	AIEND


BasicAI_259://�g���b�N���[��2006.6.14ok
	IF_FIRST	IF_FIRST_ATTACK,AI_DEC10	//�A�^�b�N�̕�������
	IF_FIRST	IF_FIRST_EQUAL,AI_DEC10		//����
	AIEND


BasicAI_265://�䂤�킭2006.6.5ok
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_AMANOZYAKU,AI_DEC12		// ���܂̂��Ⴍ2010.2.13

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_265_check	// ������Ԃ�
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_DONKAN,AI_DEC10					// �ǂ񂩂�
	IF_EQUAL	TOKUSYU_KURIABODHI,AI_DEC10				// �N���A�{�f�B
	IF_EQUAL	TOKUSYU_SIROIKEMURI,AI_DEC10			// ���낢���ނ�
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_265_check:	
	//���ʂ������A�����ʂ̏ꍇ���Ȃ�
	CHECK_POKESEX	CHECK_ATTACK
	IF_EQUAL	PTL_SEX_MALE,BasicAI_265_MALE
	IF_EQUAL	PTL_SEX_FEMALE,BasicAI_265_FEMALE
	JUMP		AI_DEC10
BasicAI_265_MALE:
	CHECK_POKESEX	CHECK_DEFENCE
	IF_EQUAL	PTL_SEX_FEMALE,BasicAI_265_1
	JUMP		AI_DEC10
BasicAI_265_FEMALE:
	CHECK_POKESEX	CHECK_DEFENCE
	IF_EQUAL	PTL_SEX_MALE,BasicAI_265_1
	JUMP		AI_DEC10

BasicAI_265_1:
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEPOW,1,AI_DEC10
	AIEND

BasicAI_266://�X�e���X���b�N2006.6.5ok
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,AI_DEC10 // �X�e���X���b�N��
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,AI_DEC10			//	�G�̍T�������Ȃ�
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_266_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_266_end:
	AIEND

BasicAI_270://�݂��Â��̂܂�2006.6.5
	//�J��o��AI�����Ή��̂���AI�ł͎g�p�s�ɂ���2006.8.10
	INCDEC	-20
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,AI_DEC10			//	�����̍T�������Ȃ�ok
	//�T���|�P�����ɂg�o�񕜂ł��邩ok
	IF_BENCH_HPDEC	CHECK_ATTACK,BasicAI_270_end
	//�T���|�P�����ɏ�Ԉُ�񕜂ł��邩�ǂ���ok
	IF_BENCH_COND	CHECK_ATTACK,BasicAI_270_end
	//�T���|�P�����ɂo�o�񕜂ł��邩�ǂ���ok
	IF_BENCH_PPDEC	CHECK_ATTACK,BasicAI_270_end
	JUMP		AI_DEC10
BasicAI_270_end:
	AIEND

BasicAI_277:			// 	�߂Ƃ�
	CHECK_TOKUSEI	CHECK_ATTACK				//���񂶂��Ȃ�i�K���X�ł��2006.6.6  ���������\��
	IFN_EQUAL	TOKUSYU_TANZYUN,BasicAI_277_tanzyun_no

	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,8,AI_DEC10
	IF_PARA_OVER	CHECK_ATTACK,PARA_HIT,8,AI_DEC8

BasicAI_277_tanzyun_no:
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_HIT,12,AI_DEC8
	AIEND

BasicAI_278:			// 	���C�h�K�[�h
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_279:			// 	�K�[�h�V�F�A
	AIEND
BasicAI_280:			// 	�p���[�V�F�A
	AIEND

BasicAI_281:			// 	�����_�[���[��
	FLDEFF_CHECK	BTL_FLDEFF_WONDERROOM,AI_DEC10		//�����_�[���[����	
	AIEND

BasicAI_282:			// 	�T�C�R�V���b�N
	AIEND
BasicAI_283:			// 	�x�m���V���b�N
	AIEND

BasicAI_284:			//	�{�f�B�p�[�W 	
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC10
	AIEND

BasicAI_285:			// 	�e���L�l�V�X
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_TELEKINESIS,AI_DEC10	// �e���L�l�V�X��
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_285_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_285_end:

	AIEND

BasicAI_286:			// 	�}�W�b�N���[��
	FLDEFF_CHECK	BTL_FLDEFF_MAGICROOM,AI_DEC10		//�}�W�b�N���[����	
	AIEND

BasicAI_287:			// 	�������Ƃ�
	AIEND
BasicAI_288:			// 	��܂��炵
	AIEND
BasicAI_289:			// 	�͂�����ق̂�
	AIEND
BasicAI_290:			// 	���傤�̂܂�
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC8
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC6
	AIEND

BasicAI_291:			// 	�w�r�[�{���o�[
	AIEND

BasicAI_292:			// 	�V���N���m�C�Y          //����肩�����v���O���}�ɕ���
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	CHECK_TYPE	CHECK_ATTACK_TYPE2

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	CHECK_TYPE	CHECK_DEFENCE_TYPE2

	AIEND

BasicAI_293:			// 	�G���N�g�{�[��
	AIEND

BasicAI_294:			// 	�݂��т���
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IFN_WAZA_TYPE	POKETYPE_MIZU,BasicAI_294_end		//����̃^�C�v�P��������Ȃ�

	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_WAZA_TYPE	POKETYPE_MIZU,AI_DEC10       		//����̃^�C�v�Q����

BasicAI_294_end:
	AIEND

BasicAI_295:			// 	�j�g���`���[�W
	AIEND
BasicAI_296:			// 	�A�V�b�h�{��
	AIEND
BasicAI_297:			// 	�C�J�T�}
	AIEND

BasicAI_298:			// 	�V���v���r�[��
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MARUTITAIPU,AI_DEC10		// �}���`�^�C�v�͖����Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_NAMAKE,AI_DEC10				// �Ȃ܂��͑��肪�L���Ȃ̂ł��Ȃ�ok
	IF_EQUAL	TOKUSYU_YOWAKI,AI_DEC10				// ��킫�͑��肪�L���Ȃ̂ł��Ȃ�
	IF_EQUAL	TOKUSYU_SUROOSUTAATO,AI_DEC10		// �X���[�X�^�[�g�͑��肪�L���Ȃ̂ł��Ȃ�ok
  
  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_298_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_298_end:

	AIEND

BasicAI_299:			// 	�Ȃ��܂Â���             //����肩�����v���O���}�ɕ���

  CHECK_TOKUSEI	CHECK_ATTACK		
	IF_EQUAL	TOKUSYU_KATAYABURI,BasicAI_299_end	// ������Ԃ�`�F�b�N

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUMIRAA,AI_DEC12		//�}�W�b�N�~���[2010.2.13�ǉ�

BasicAI_299_end:

	AIEND

BasicAI_300:			// 	�������ɂǂ���
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_301:			// 	��񂵂傤
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_302:			// 	�G�R�[�{�C�X
	AIEND

BasicAI_303:			// 	�Ȃ�������
	AIEND

BasicAI_304:			// 	�N���A�X���b�O
	AIEND

BasicAI_305:			// 	�A�V�X�g�p���[
	AIEND

BasicAI_306:			// 	�t�@�X�g�K�[�h
	AIEND

BasicAI_307:			// 	�T�C�h�`�F���W
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_308:			// 	�������Ԃ�
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEPOW,12,AI_DEC8
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC6
	AIEND

BasicAI_309:			// 	���₵�̂͂ǂ�
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_310:			// 	�������
	AIEND

BasicAI_311:			// 	�t���[�t�H�[��
	AIEND

BasicAI_312:			// 	�M�A�`�F���W
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AGI,12,AI_DEC8
	AIEND

BasicAI_313:			// 	�Ƃ����Ȃ�
	AIEND

BasicAI_314:			// 	�₫����
	AIEND

BasicAI_315:			// 	����������
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_316:			// 	�������傤
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEPOW,12,AI_DEC8
	AIEND

BasicAI_317:			// 	�A�N���o�b�g
	AIEND

BasicAI_318:			// 	�~���[�^�C�v             //����肩�����v���O���}�ɕ���
	AIEND

BasicAI_319:			// 	����������
	AIEND

BasicAI_320:			// 	���̂�����
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IFN_EQUAL	0,BasicAI_320_end					// �����̍T��������

	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IFN_EQUAL	0,AI_DEC10
	JUMP		AI_DEC1

BasicAI_320_end:
	AIEND

BasicAI_321:			// 	�ق����
	AIEND

BasicAI_322:			// 	�Ƃ�����܂�
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC8
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_HIT,12,AI_DEC6
	AIEND

BasicAI_323:			// 	�M�t�g�p�X
	CHECK_SOUBI_ITEM	CHECK_DEFENCE
	IFN_EQUAL	0,AI_DEC10

	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	AIEND

BasicAI_324:			// 	�݂��̂�����
	AIEND
BasicAI_325:			// 	�ق̂��̂�����
	AIEND
BasicAI_326:			// 	�����̂�����
	AIEND
BasicAI_327:			// 	�ӂ邢���Ă�
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_POW,12,AI_DEC10
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_SPEPOW,12,AI_DEC8
	AIEND

BasicAI_328:			// 	�R�b�g���K�[�h
	IF_PARA_EQUAL	CHECK_ATTACK,PARA_DEF,12,AI_DEC10
	AIEND

BasicAI_329:	// 	���ɂ����̂���
	AIEND
BasicAI_330:	// 	�������邹����
	AIEND
BasicAI_331:	// 	�t���[�Y�{���g
	AIEND
BasicAI_332:	// 	�R�[���h�t���A
	AIEND
BasicAI_333:	// 	�ǂȂ����
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

