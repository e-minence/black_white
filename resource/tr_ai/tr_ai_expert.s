//============================================================================
/**
 *@file		tr_ai_expert.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�G�L�X�p�[�gAI
//========================================================

ExpertAISeq:
	IF_MIKATA_ATTACK	ExpertAISeq_end   //�Ώۂ������Ȃ�I��

	IF_WAZA_SEQNO_JUMP	1,ExpertAI_1		// ���点��			2006.6.14
	IF_WAZA_SEQNO_JUMP	3,ExpertAI_3		// �����Ƃ�			2006.6.14
	IF_WAZA_SEQNO_JUMP	7,ExpertAI_7		// ���΂�			2006.6.14
	IF_WAZA_SEQNO_JUMP	8,ExpertAI_8		// ��߂���			2006.6.14�o�O�C��
	IF_WAZA_SEQNO_JUMP	9,ExpertAI_9		// �I�E��			2006.6.14�e�[�u���ǉ�

	IF_WAZA_SEQNO_JUMP	10,ExpertAI_10		// �U���̓A�b�v		2006.6.14���񂶂�񌩂邩��
	IF_WAZA_SEQNO_JUMP	11,ExpertAI_11		// �h��̓A�b�v		2006.6.14*����
	IF_WAZA_SEQNO_JUMP	12,ExpertAI_12		// �f�����A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	13,ExpertAI_13		// ���U�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	14,ExpertAI_14		// ���h�A�b�v		2006.6.14*����
	IF_WAZA_SEQNO_JUMP	15,ExpertAI_15		// �������A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	16,ExpertAI_16		// ��𗦃A�b�v		2006.6.14�A�N�A�����O�ǉ�

	IF_WAZA_SEQNO_JUMP	17,ExpertAI_17		// ���Ȃ炸������	2006.6.14

	IF_WAZA_SEQNO_JUMP	18,ExpertAI_18		// �U���̓_�E��	0	2006.6.14*����
	IF_WAZA_SEQNO_JUMP	19,ExpertAI_19		// �h��̓_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	20,ExpertAI_20		// �f�����_�E��		2006.6.14	
	IF_WAZA_SEQNO_JUMP	21,ExpertAI_21		// ���U�_�E��		2006.6.14*����
	IF_WAZA_SEQNO_JUMP	22,ExpertAI_22		// ���h�_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	23,ExpertAI_23		// �������_�E��		2006.6.14�A�N�A�����O�ǉ�
	IF_WAZA_SEQNO_JUMP	24,ExpertAI_24		// ��𗦃_�E��		2006.6.14

	IF_WAZA_SEQNO_JUMP	25,ExpertAI_25		// ���낢����	*���񂶂��`�F�b�N
	IF_WAZA_SEQNO_JUMP	26,ExpertAI_26		// ���܂�			2006.6.14
	IF_WAZA_SEQNO_JUMP	28,ExpertAI_28		// �ӂ��Ƃ΂�	�܂��т��`�F�b�N�]�͂������
	IF_WAZA_SEQNO_JUMP	30,ExpertAI_30		// �e�N�X�`���[		2006.6.14
	IF_WAZA_SEQNO_JUMP	32,ExpertAI_32		// ������������		2006.6.14
	IF_WAZA_SEQNO_JUMP	33,ExpertAI_33		// �ǂ��ǂ�			2006.6.14
	IF_WAZA_SEQNO_JUMP	35,ExpertAI_35		// �Ђ���̂���		2006.6.14�v�`�F�b�N
	IF_WAZA_SEQNO_JUMP	37,ExpertAI_37		// �˂ނ�			2006.6.14
	IF_WAZA_SEQNO_JUMP	38,ExpertAI_38		// ���������Ђ�����	2006.6.14
	IF_WAZA_SEQNO_JUMP	39,ExpertAI_39		// ����				2006.6.14
	IF_WAZA_SEQNO_JUMP	40,ExpertAI_40		// ������̂܂���	2006.6.14
	IF_WAZA_SEQNO_JUMP	42,ExpertAI_42		// ���߂�			2006.6.14
	IF_WAZA_SEQNO_JUMP	43,ExpertAI_43		// �N���e�B�J���ł₷��		*�����Ƒ�������
	IF_WAZA_SEQNO_JUMP	48,ExpertAI_48		// �Ƃ�����			2006.6.14�c�o�ǉ�
	IF_WAZA_SEQNO_JUMP	49,ExpertAI_49		// ������				

	IF_WAZA_SEQNO_JUMP	50,ExpertAI_10		// �U���Q�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	51,ExpertAI_11		// �h��Q�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	52,ExpertAI_12		// �f�����Q�A�b�v	2006.6.14
	IF_WAZA_SEQNO_JUMP	53,ExpertAI_13		// ���U�Q�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	54,ExpertAI_14		// ���h�Q�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	55,ExpertAI_15		// �����Q�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	56,ExpertAI_16		// ����Q�A�b�v		2006.6.14

	IF_WAZA_SEQNO_JUMP	58,ExpertAI_18		// �U���Q�_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	59,ExpertAI_19		// �h��Q�_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	60,ExpertAI_20		// �f�����Q�_�E��	2006.6.14
	IF_WAZA_SEQNO_JUMP	61,ExpertAI_21		// ���U�Q�_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	62,ExpertAI_22		// ���h�Q�_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	63,ExpertAI_23		// �����Q�_�E��		2006.6.14
	IF_WAZA_SEQNO_JUMP	64,ExpertAI_24		// ����Q�_�E��		2006.6.14

	IF_WAZA_SEQNO_JUMP	65,ExpertAI_65		// ���t���N�^�[		2006.6.14�v�`�F�b�N
	IF_WAZA_SEQNO_JUMP	66,ExpertAI_66		// �ǂ�				2006.6.14
	IF_WAZA_SEQNO_JUMP	67,ExpertAI_67		// �܂�				2006.6.14

	IF_WAZA_SEQNO_JUMP	118,ExpertAI_118	// ���΂�

	IF_WAZA_SEQNO_JUMP	70,ExpertAI_70		// �ǉ��f�����_�E��	2006.6.14
	IF_WAZA_SEQNO_JUMP	75,ExpertAI_75		// �S�b�h�o�[�h		2006.6.14
	IF_WAZA_SEQNO_JUMP	78,ExpertAI_78		// ���Ă݂Ȃ�		2006.6.14
	IF_WAZA_SEQNO_JUMP	79,ExpertAI_79		// �݂����			2006.6.14
	IF_WAZA_SEQNO_JUMP	80,ExpertAI_80		// �͂�����������	2006.6.14
	IF_WAZA_SEQNO_JUMP	84,ExpertAI_84		// ��ǂ肬			2006.6.14
	IF_WAZA_SEQNO_JUMP	86,ExpertAI_86		// ���Ȃ��΂�		2006.6.14
	IF_WAZA_SEQNO_JUMP	89,ExpertAI_89		// �J�E���^�[
	IF_WAZA_SEQNO_JUMP	90,ExpertAI_90		// �A���R�[�����e�[�u���쐬
	IF_WAZA_SEQNO_JUMP	91,ExpertAI_91		// �����݂킯		2006.6.14
	IF_WAZA_SEQNO_JUMP	92,ExpertAI_92		// ���т�			2006.6.14

	IF_WAZA_SEQNO_JUMP	94,ExpertAI_94		// ���b�N�I��		2006.6.14
	IF_WAZA_SEQNO_JUMP	97,ExpertAI_97		// �˂���			2006.6.14
	IF_WAZA_SEQNO_JUMP	98,ExpertAI_98		// �݂�����			2006.6.14
	IF_WAZA_SEQNO_JUMP	99,ExpertAI_99		// ������������		2006.6.14
	IF_WAZA_SEQNO_JUMP	102,ExpertAI_102	// ���₵�̂���		2006.6.14�C��
	IF_WAZA_SEQNO_JUMP	105,ExpertAI_105	// �ǂ�ڂ�			2006.6.14�e�[�u���ǉ�
	IF_WAZA_SEQNO_JUMP	106,ExpertAI_106	// ���낢�܂Ȃ���	2006.6.14
	IF_WAZA_SEQNO_JUMP	108,ExpertAI_16		// ��𗦃A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	109,ExpertAI_109	// �̂낢			2006.6.14
	IF_WAZA_SEQNO_JUMP	111,ExpertAI_111	// �܂���			2006.6.14�t�F�C���g�`�F�b�N�ǉ�
	IF_WAZA_SEQNO_JUMP	112,ExpertAI_112	// �܂��т�			2006.6.14�V�K�ǉ�
	IF_WAZA_SEQNO_JUMP	113,ExpertAI_113	// �݂�Ԃ�			2006.6.14
	IF_WAZA_SEQNO_JUMP	116,ExpertAI_116	// ���炦��			2006.6.14
	IF_WAZA_SEQNO_JUMP	127,ExpertAI_127	// �o�g���^�b�`		2006.6.14
	IF_WAZA_SEQNO_JUMP	128,ExpertAI_128	// ��������			2006.6.14�G�Ƃ�ڂ�����`�F�b�N�ǉ�
	IF_WAZA_SEQNO_JUMP	132,ExpertAI_132	// �����̂Ђ���		2006.6.14
	IF_WAZA_SEQNO_JUMP	133,ExpertAI_133	// ������������		2006.6.14�V�[�P���X�폜
	IF_WAZA_SEQNO_JUMP	134,ExpertAI_134	// ���̂Ђ���		2006.6.14�V�[�P���X�폜
	IF_WAZA_SEQNO_JUMP	136,ExpertAI_136	// ���܂���			2006.6.14
	IF_WAZA_SEQNO_JUMP	137,ExpertAI_137	// �ɂق�΂�		2006.6.14
	IF_WAZA_SEQNO_JUMP	142,ExpertAI_142	// �͂炾����		2006.6.14
	IF_WAZA_SEQNO_JUMP	143,ExpertAI_143	// ��������		2006.6.14
	IF_WAZA_SEQNO_JUMP	144,ExpertAI_144	// �~���[�R�[�g		���ރ`�F�b�N
	IF_WAZA_SEQNO_JUMP	145,ExpertAI_145	// ���P�b�g����	2006.6.14
	IF_WAZA_SEQNO_JUMP	151,ExpertAI_151	// �\�[���[�r�[��	2006.6.14�p���t���n�[�u
	IF_WAZA_SEQNO_JUMP	151,ExpertAI_152	// ���݂Ȃ�			2006.6.14�ǉ�
	IF_WAZA_SEQNO_JUMP	155,ExpertAI_155	// ������Ƃ�		2006.6.14
	IF_WAZA_SEQNO_JUMP	157,ExpertAI_157	// ���܂�����		2006.6.14
	IF_WAZA_SEQNO_JUMP	158,ExpertAI_158	// �˂����܂�		2006.6.14
	IF_WAZA_SEQNO_JUMP	161,ExpertAI_161	// �͂�����			2006.6.14
	IF_WAZA_SEQNO_JUMP	162,ExpertAI_162	// �݂̂���			2006.6.14
	IF_WAZA_SEQNO_JUMP	164,ExpertAI_164	// �����			2006.6.14
	IF_WAZA_SEQNO_JUMP	166,ExpertAI_166	// �����Ă�			2006.6.14
	IF_WAZA_SEQNO_JUMP	168,ExpertAI_168	// �����݂₰		2006.6.14
	IF_WAZA_SEQNO_JUMP	169,ExpertAI_169	// ���炰��		2006.6.14
	IF_WAZA_SEQNO_JUMP	170,ExpertAI_170	// �������p���`		2006.6.14
	IF_WAZA_SEQNO_JUMP	171,ExpertAI_171	// ����			2006.6.14
	IF_WAZA_SEQNO_JUMP	177,ExpertAI_177	// �g���b�N			����ɍ��킹�ăe�[�u���쐬�K�v����
	IF_WAZA_SEQNO_JUMP	178,ExpertAI_178	// �Ȃ肫��			�e�[�u���v����
	IF_WAZA_SEQNO_JUMP	181,ExpertAI_181	// �˂��͂�			2006.6.14
	IF_WAZA_SEQNO_JUMP	182,ExpertAI_182	// �΂�������		���낢�n�[�u�`�F�b�N����邩��
	IF_WAZA_SEQNO_JUMP	183,ExpertAI_183	// �}�W�b�N�R�[�g	2006.6.14
	IF_WAZA_SEQNO_JUMP	184,ExpertAI_184	// ���T�C�N��		2006.6.14
	IF_WAZA_SEQNO_JUMP	185,ExpertAI_185	// ���x���W			2006.6.14
	IF_WAZA_SEQNO_JUMP	186,ExpertAI_186	// �������		2006.6.14�Ђ���̂��ׂ��ǉ�
	IF_WAZA_SEQNO_JUMP	188,ExpertAI_188	// �͂������Ƃ�		2006.6.14
	IF_WAZA_SEQNO_JUMP	189,ExpertAI_189	// ���ނ����		2006.6.14
	IF_WAZA_SEQNO_JUMP	190,ExpertAI_190	// �ӂ�			2006.6.14
	IF_WAZA_SEQNO_JUMP	191,ExpertAI_191	// �X�L���X���b�v	2006.6.14
	IF_WAZA_SEQNO_JUMP	192,ExpertAI_192	// �ӂ�����			2006.6.14�����l���邩��
	IF_WAZA_SEQNO_JUMP	193,ExpertAI_193	// ���t���b�V��		2006.6.14
	IF_WAZA_SEQNO_JUMP	195,ExpertAI_195	// �悱�ǂ�			2006.6.14�v���
	IF_WAZA_SEQNO_JUMP	198,ExpertAI_198	// ���Ă݃^�b�N��	2006.6.14�c�o�ǉ�
	IF_WAZA_SEQNO_JUMP	200,ExpertAI_200	// �u���C�Y�L�b�N	�}�������Ƒ�������
	IF_WAZA_SEQNO_JUMP	201,ExpertAI_201	// �ǂ날����		2006.6.14
	IF_WAZA_SEQNO_JUMP	204,ExpertAI_204	// �I�[�o�[�q�[�g	2006.6.14���낢�n�[�u�`�F�b�N����H
	IF_WAZA_SEQNO_JUMP	205,ExpertAI_205	// ��������			2006.6.14
	IF_WAZA_SEQNO_JUMP	206,ExpertAI_206	// �R�X���p���[		2006.6.14
	IF_WAZA_SEQNO_JUMP	208,ExpertAI_208	// �r���h�A�b�v		2006.6.14
	IF_WAZA_SEQNO_JUMP	209,ExpertAI_209	// �|�C�Y���e�[��	�}�������Ƒ�������
	IF_WAZA_SEQNO_JUMP	210,ExpertAI_210	// �݂�������		2006.6.14
	IF_WAZA_SEQNO_JUMP	211,ExpertAI_211	// �߂�����			2006.6.14
	IF_WAZA_SEQNO_JUMP	212,ExpertAI_212	// ��イ�̂܂�		2006.6.14

	//�ǉ�2006.6.14

	IF_WAZA_SEQNO_JUMP	214,ExpertAI_214	// �͂˂₷��		2006.6.14 
	IF_WAZA_SEQNO_JUMP	215,ExpertAI_215	// ���イ��傭		2006.6.14		 
	IF_WAZA_SEQNO_JUMP	216,ExpertAI_216	// �~���N���A�C		2006.6.14 
	IF_WAZA_SEQNO_JUMP	217,ExpertAI_217	// �߂��܂��r���^	2006.6.14 
	IF_WAZA_SEQNO_JUMP	218,ExpertAI_218	// �A�[���n���}�[	2006.6.14 
	IF_WAZA_SEQNO_JUMP	219,ExpertAI_219	// �W���C���{�[��	2006.6.14 
	IF_WAZA_SEQNO_JUMP	220,ExpertAI_220	// ���₵�̂˂���	2006.6.14*�T���������Ƃ��̃`�F�b�N 
	IF_WAZA_SEQNO_JUMP	221,ExpertAI_221	// �����݂�		2006.6.14 
	//IF_WAZA_SEQNO_JUMP	222,ExpertAI_222// ������̂߂���* �_���[�W�`�F�b�N������̂łȂ�
	IF_WAZA_SEQNO_JUMP	223,ExpertAI_223	// �t�F�C���g		2006.6.14 
	IF_WAZA_SEQNO_JUMP	224,ExpertAI_224	// ���΂�			2006.6.14 
	IF_WAZA_SEQNO_JUMP	225,ExpertAI_225	// ��������			2006.6.14
	IF_WAZA_SEQNO_JUMP	226,ExpertAI_226	// �ڂ���		2006.6.14 
	IF_WAZA_SEQNO_JUMP	227,ExpertAI_227	// ���^���o�[�X�g	2006.6.14 
	IF_WAZA_SEQNO_JUMP	228,ExpertAI_228	// �Ƃ�ڂ�����		2006.6.14
	IF_WAZA_SEQNO_JUMP	229,ExpertAI_229	// �C���t�@�C�g		2006.6.14���낢�n�[�u�`�F�b�N 
	IF_WAZA_SEQNO_JUMP	230,ExpertAI_230	// �����؂�����		2006.6.14 
	IF_WAZA_SEQNO_JUMP	231,ExpertAI_231	// �_������			2006.6.14 
	IF_WAZA_SEQNO_JUMP	232,ExpertAI_232	// ����������		2006.6.14 
	IF_WAZA_SEQNO_JUMP	233,ExpertAI_233	// �Ȃ�����		2006.6.14 
	IF_WAZA_SEQNO_JUMP	234,ExpertAI_234	// �T�C�R�V�t�g		2006.6.14 
	IF_WAZA_SEQNO_JUMP	235,ExpertAI_235	// ����ӂ�			2006.6.14 
	IF_WAZA_SEQNO_JUMP	236,ExpertAI_236	// �����ӂ��ӂ���	2006.6.14 
	IF_WAZA_SEQNO_JUMP	237,ExpertAI_237	// ���ڂ�Ƃ�		2006.6.14
	IF_WAZA_SEQNO_JUMP	238,ExpertAI_238	// �p���[�g���b�N	2006.6.14
	IF_WAZA_SEQNO_JUMP	239,ExpertAI_239	// ������			2006.6.14
	IF_WAZA_SEQNO_JUMP	240,ExpertAI_240	// ���܂��Ȃ�		2006.6.14 
	IF_WAZA_SEQNO_JUMP	241,ExpertAI_241	// �����ǂ�			2006.6.14	
	IF_WAZA_SEQNO_JUMP	242,ExpertAI_242	// �܂˂����F�e�[�u��2006.6.14 
	IF_WAZA_SEQNO_JUMP	243,ExpertAI_243	// �p���[�X���b�v	2006.6.14
	IF_WAZA_SEQNO_JUMP	244,ExpertAI_244	// �K�[�h�X���b�v	2006.6.14
	IF_WAZA_SEQNO_JUMP	245,ExpertAI_245	// ��������			2006.6.14 
	IF_WAZA_SEQNO_JUMP	246,ExpertAI_246	// �Ƃ��Ă���		2006.6.14 
	IF_WAZA_SEQNO_JUMP	247,ExpertAI_247	// �Ȃ�݂̃^�l		2006.6.14
	IF_WAZA_SEQNO_JUMP	248,ExpertAI_248	// �ӂ�����			2006.6.14	 
	IF_WAZA_SEQNO_JUMP	249,ExpertAI_249	// �ǂ��т�			2006.6.14 
	IF_WAZA_SEQNO_JUMP	250,ExpertAI_250	// �n�[�g�X���b�v	2006.6.14 
	IF_WAZA_SEQNO_JUMP	251,ExpertAI_251	// �A�N�A�����O		2006.6.14 
	IF_WAZA_SEQNO_JUMP	252,ExpertAI_252	// �ł񂶂ӂ䂤		2006.6.14 
	IF_WAZA_SEQNO_JUMP	253,ExpertAI_253	// �t���A�h���C�u	2006.6.14 
	IF_WAZA_SEQNO_JUMP	255,ExpertAI_255	// �_�C�r���O		2006.6.14 
	IF_WAZA_SEQNO_JUMP	256,ExpertAI_256	// ���Ȃ��ق�		2006.6.14 
	IF_WAZA_SEQNO_JUMP	258,ExpertAI_258	// ����΂炢		2006.6.14	�悭�݂�K�v����
	IF_WAZA_SEQNO_JUMP	259,ExpertAI_259	// �g���b�N���[��	2006.6.14 
	IF_WAZA_SEQNO_JUMP	260,ExpertAI_260	// �ӂԂ�			2006.6.14
	IF_WAZA_SEQNO_JUMP	262,ExpertAI_262	// �{���e�b�J�[		2006.6.14�c�o�ǉ�
	IF_WAZA_SEQNO_JUMP	263,ExpertAI_263	// �Ƃт͂˂�		2006.6.14 
	IF_WAZA_SEQNO_JUMP	265,ExpertAI_265	// �䂤�킭			2006.6.14		 
	IF_WAZA_SEQNO_JUMP	266,ExpertAI_266	// �X�e���X���b�N	2006.6.14 
	//IF_WAZA_SEQNO_JUMP	268,ExpertAI_268 	//���΂��̂Ԃ�*�_���[�W�`�F�b�N������̂Ŗ��Ȃ� 
	IF_WAZA_SEQNO_JUMP	269,ExpertAI_269	// ����͂̂���	2006.6.14 
	IF_WAZA_SEQNO_JUMP	270,ExpertAI_270	// �݂��Â��̂܂�	2006.6.14 
	IF_WAZA_SEQNO_JUMP	272,ExpertAI_272	// �V���h�[�_�C�u	2006.6.14 
ExpertAISeq_end:
	AIEND

//---------------------------------------------------------------------------


ExpertAI_1:	//���点��
	//�A�^�b�N���̌��Ǝv����̂ŕύX�FDEFENCE��ATTACK��2006.6.6
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,8,ExpertAI_1_2	//��߂���
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,107,ExpertAI_1_2	//������
	JUMP		ExpertAI_1_3
	
ExpertAI_1_2:
	IF_RND_UNDER	128,ExpertAI_1_3
	INCDEC		+1

ExpertAI_1_3:
	AIEND
//---------------------------------------------------------------------------

ExpertAI_3:	//�����Ƃ�
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_3_1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_3_1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_3_1
	JUMP		ExpertAI_3_2
	
ExpertAI_3_1:
	IF_RND_UNDER	50,ExpertAI_3_2
	INCDEC		-3

ExpertAI_3_2:
	AIEND

//---------------------------------------------------------------------------
	
ExpertAI_168:	//�����݂₰
ExpertAI_7:	//���΂�

	//�P�j������̉�𗦂��V�ȏ�Ȃ��-1���ĂQ��
	//�P�f�j����Ɏ�����̉�𗦂�10�ȏ�`�Ȃ��128/255��-1
	//�Q�j�U������80�ȏ�ő�����U���������ꍇ��50/255�̊m����-3�ŏI��
	//�R�j�U�����̂g�o��50�ȉ��Ȃ�128/255��-1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_AVOID,7,ExpertAI_7_1
	INCDEC		-1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_AVOID,10,ExpertAI_7_1
	IF_RND_UNDER	128,ExpertAI_7_1
	INCDEC		-1

ExpertAI_7_1:
	IF_HP_UNDER	CHECK_ATTACK,80,ExpertAI_7_2
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_7_2

	IF_RND_UNDER	50,ExpertAI_7_5
	JUMP		AI_DEC3

ExpertAI_7_2:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_7_4
	IF_RND_UNDER	128,ExpertAI_7_3

	INCDEC		+1
ExpertAI_7_3:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_7_5
	IF_RND_UNDER	50,ExpertAI_7_5
	INCDEC		+1
	JUMP		ExpertAI_7_5

ExpertAI_7_4:
	IF_RND_UNDER	50,ExpertAI_7_5
	INCDEC		-1
ExpertAI_7_5:
	AIEND

//---------------------------------------------------------------------------

ExpertAI_8:	//��߂���
	//�Z������1/4�A1/2�Ȃ�-1
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_8_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_8_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_8_ng
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_8_ok
	JUMP		ExpertAI_8_end

ExpertAI_8_ok:
	IF_RND_UNDER	51,ExpertAI_8_end
	INCDEC		+3
	JUMP		ExpertAI_8_end

ExpertAI_8_ng:
	INCDEC		-1
ExpertAI_8_end:
	AIEND

//---------------------------------------------------------------------------

ExpertAI_9:	//�I�E��
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_9_1

	CHECK_LAST_WAZA	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_9_Table,ExpertAI_9_1
	
	IF_RND_UNDER	128,ExpertAI_9_3
	INCDEC		2	//�����Z��128/255�I�E������������
	JUMP		ExpertAI_9_3

ExpertAI_9_1://����̕��������ꍇ�����Z�łȂ���Ό��Z����
	CHECK_LAST_WAZA	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_9_Table,ExpertAI_9_3

	IF_RND_UNDER	80,ExpertAI_9_3
	INCDEC		-1
ExpertAI_9_3:
	AIEND

//	.ALIGN
ExpertAI_9_Table://�ω����ċ������ȋZ�Q
	.long	79,142,147,95,47,320,325,28,108,92
	.long	12,32,90,329,238,177,109,186,103,178
	.long	184,313,319,86,137,77,247,223,63,245
	.long	168,343,213,207,259,260,271,276,285	
	//�ȍ~�ǉ�2006.6.14
	//2006.6.14
	.long	375,384,385,389,391,415,445,464	
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_10:	// �U���̓A�b�v
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,9,ExpertAI_10_1
	IF_RND_UNDER	100,ExpertAI_10_2
	INCDEC		-1
	JUMP		ExpertAI_10_2
ExpertAI_10_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_10_2

	IF_RND_UNDER	128,ExpertAI_10_2
	INCDEC		2
ExpertAI_10_2:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_10_end
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_10_3

	IF_RND_UNDER	40,ExpertAI_10_end
ExpertAI_10_3:
	INCDEC		-2
ExpertAI_10_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_208:	// �r���h�A�b�v
ExpertAI_11:	// �h��̓A�b�v
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,9,ExpertAI_11_1
	IF_RND_UNDER	100,ExpertAI_11_2
	INCDEC		-1
	JUMP		ExpertAI_11_2
ExpertAI_11_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_11_2

	IF_RND_UNDER	128,ExpertAI_11_2
	INCDEC		2
ExpertAI_11_2:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_11_3

	IF_RND_UNDER	200,ExpertAI_11_end

ExpertAI_11_3:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_11_5

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_11_4

	//�^�C�v�łȂ��Z�ʂɕ��ނ����肷��̂Ŏg�p��߂�2006.6.14
	//--------------------------------------------------
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_TYPE
	//IFN_TABLE_JUMP	ExpertAI_11_Table,ExpertAI_11_5
	//--------------------------------------------------
	
	//�ǉ��F���肪�Ō�ɏo�����Z������Ȃ�Ώo���ɂ�������
	//2006.6.14
	CHECK_LAST_WAZA_KIND
	IF_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_11_5//����
	

	IF_RND_UNDER	60,ExpertAI_11_end
ExpertAI_11_4:
	IF_RND_UNDER	60,ExpertAI_11_end
ExpertAI_11_5:
	INCDEC		-2
ExpertAI_11_end:
	AIEND

ExpertAI_11_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_DOKU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_HIKOU
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_GHOST
	.long	POKETYPE_HAGANE
	.long	0xffffffffff

//---------------------------------------------------------------------------
ExpertAI_12:	// �f�����A�b�v
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_12_1
	INCDEC		-3
	JUMP		ExpertAI_12_end

ExpertAI_12_1:
	IF_RND_UNDER	70,ExpertAI_12_end

	INCDEC		3
ExpertAI_12_end:
	AIEND
//��������x���Ƃ��A185/255�̊m����+3
	
//---------------------------------------------------------------------------
ExpertAI_13:	// ���U�A�b�v
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,9,ExpertAI_13_1
	IF_RND_UNDER	100,ExpertAI_13_2
	INCDEC		-1
	JUMP		ExpertAI_13_2
ExpertAI_13_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_13_2

	IF_RND_UNDER	128,ExpertAI_13_2
	INCDEC		2
ExpertAI_13_2:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_13_end
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_13_3

	IF_RND_UNDER	70,ExpertAI_13_end
ExpertAI_13_3:
	INCDEC		-2
ExpertAI_13_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_211:	// �߂�����
ExpertAI_206:	// �R�X���p���[
ExpertAI_14:	// ���h�A�b�v
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,9,ExpertAI_14_1
	IF_RND_UNDER	100,ExpertAI_14_2
	INCDEC		-1
	JUMP		ExpertAI_14_2
ExpertAI_14_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_14_2

	IF_RND_UNDER	128,ExpertAI_14_2
	INCDEC		2
ExpertAI_14_2:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_14_3

	IF_RND_UNDER	200,ExpertAI_14_end

ExpertAI_14_3:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_14_5

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_14_4

	//�^�C�v�łȂ��Z�ʂɕ��ނ����肷��̂Ŏg�p��߂�2006.6.14
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_TYPE
	//IF_TABLE_JUMP	ExpertAI_14_Table,ExpertAI_14_5
	//--------------------------------------------------

	//�ǉ��F���肪�Ō�ɏo�����Z�������Ȃ�Ώo���ɂ�������
	//2006.6.14
	CHECK_LAST_WAZA_KIND
	IF_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_14_5//����
	
	IF_RND_UNDER	60,ExpertAI_14_end


ExpertAI_14_4:
	IF_RND_UNDER	60,ExpertAI_14_end
ExpertAI_14_5:
	INCDEC		-2
ExpertAI_14_end:
	AIEND

ExpertAI_14_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_DOKU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_HIKOU
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_GHOST
	.long	POKETYPE_HAGANE
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_15:	// �������A�b�v	�i���g�p�j
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,9,ExpertAI_15_1
	IF_RND_UNDER	50,ExpertAI_15_1
	INCDEC		-2
ExpertAI_15_1:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_15_end

	INCDEC		-2
ExpertAI_15_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_16:	// ��𗦃A�b�v
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_16_1

	IF_RND_UNDER	100,ExpertAI_16_1
	INCDEC		3
ExpertAI_16_1:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,9,ExpertAI_16_2

	IF_RND_UNDER	128,ExpertAI_16_2
	INCDEC		-1

ExpertAI_16_2:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_DOKUDOKU,ExpertAI_16_5

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_16_4

	IF_RND_UNDER	80,ExpertAI_16_5
ExpertAI_16_4:
	IF_RND_UNDER	50,ExpertAI_16_5
	INCDEC		3

ExpertAI_16_5:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_16_6	//��ǂ肬
	IF_RND_UNDER	70,ExpertAI_16_6
	INCDEC		3

ExpertAI_16_6:
	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_NEWOHARU,ExpertAI_16_6_1
	IF_RND_UNDER	128,ExpertAI_16_7
	INCDEC		2
	JUMP	ExpertAI_16_7

//2006.6.14�A�N�A�����O�ǉ�
ExpertAI_16_6_1:
//@todo	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_AQUARING,ExpertAI_16_7
	IF_RND_UNDER	128,ExpertAI_16_7
	INCDEC		2
	JUMP	ExpertAI_16_7


ExpertAI_16_7:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_16_8	// �̂낢
	IF_RND_UNDER	70,ExpertAI_16_8
	INCDEC		3
ExpertAI_16_8:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_16_end

	IF_PARA_EQUAL	CHECK_ATTACK,PARA_AVOID,6,ExpertAI_16_end

	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_16_9
	IF_HP_UNDER	CHECK_DEFENCE,40,ExpertAI_16_9
	
	IF_RND_UNDER	70,ExpertAI_16_end

ExpertAI_16_9:
	INCDEC		-2
ExpertAI_16_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_17:	//���Ȃ炸������ok
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,10,ExpertAI_17_1
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,2,ExpertAI_17_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_17_2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_17_2
	JUMP		ExpertAI_17_end

ExpertAI_17_1:
	INCDEC		1
ExpertAI_17_2:
	IF_RND_UNDER	100,ExpertAI_17_end
	INCDEC		1

ExpertAI_17_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_18:	// �U���̓_�E��
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_POW,6,ExpertAI_18_2

	INCDEC		-1

	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_18_1

	INCDEC		-1

ExpertAI_18_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,3,ExpertAI_18_2
	IF_RND_UNDER	50,ExpertAI_18_2
	INCDEC		-2
ExpertAI_18_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_18_3

	INCDEC		-2

ExpertAI_18_3:

	//�^�C�v�`�F�b�N���Ȃ�2006.6.14
	//-----------------------------------------------
	//CHECK_TYPE	CHECK_DEFENCE_TYPE1
	//IF_TABLE_JUMP	ExpertAI_18_Table,ExpertAI_18_end
	//CHECK_TYPE	CHECK_DEFENCE_TYPE2
	//IF_TABLE_JUMP	ExpertAI_18_Table,ExpertAI_18_end
	//-----------------------------------------------
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_18_end//����łȂ�
	IF_RND_UNDER	128,ExpertAI_18_end
	INCDEC		-2

ExpertAI_18_end:
	AIEND

//����̃|�P�����̃^�C�v�ŕ����A����̔��ʂ͌�����
ExpertAI_18_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_HAGANE
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_205:	// ��������
ExpertAI_19:	// �h��̓_�E��
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_19_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,3,ExpertAI_19_2
ExpertAI_19_1:
	IF_RND_UNDER	50,ExpertAI_19_2
	INCDEC		-2
ExpertAI_19_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_19_end

	INCDEC		-2
ExpertAI_19_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_70:	// �ǉ��f�����_�E��
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_70_end		//2008.6.21�ǉ�
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_70_end	//2008.6.21�ǉ�
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_70_end	//2008.6.21�ǉ�
	IF_WAZANO	WAZANO_KOGOERUKAZE,ExpertAI_20	// 100%�ǉ�
	IF_WAZANO	WAZANO_GANSEKIHUUZI,ExpertAI_20	// 100%�ǉ�
	IF_WAZANO	WAZANO_MADDOSYOTTO,ExpertAI_20	// 100%�ǉ�
	AIEND

ExpertAI_70_end:
	AIEND

	
//---------------------------------------------------------------------------

ExpertAI_20:	// �f�����_�E��
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_20_1
	INCDEC		-3
	JUMP		ExpertAI_20_end

ExpertAI_20_1:
	IF_RND_UNDER	70,ExpertAI_20_end

	INCDEC		2
ExpertAI_20_end:
	AIEND
//---------------------------------------------------------------------------

ExpertAI_21:	// ���U�_�E��
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEPOW,6,ExpertAI_21_2

	INCDEC		-1
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_21_1

	INCDEC		-1
ExpertAI_21_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,3,ExpertAI_21_2
	IF_RND_UNDER	50,ExpertAI_21_2
	INCDEC		-2
ExpertAI_21_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_21_3

	INCDEC		-2
ExpertAI_21_3:
	
	//�^�C�v�`�F�b�N�����Ȃ�2006.6.14
	//-----------------------------------------------
	//CHECK_TYPE	CHECK_DEFENCE_TYPE1
	//IF_TABLE_JUMP	ExpertAI_21_Table,ExpertAI_21_end
	//CHECK_TYPE	CHECK_DEFENCE_TYPE2
	//IF_TABLE_JUMP	ExpertAI_21_Table,ExpertAI_21_end
	//-----------------------------------------------
	
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_21_end//�����łȂ�
	IF_RND_UNDER	128,ExpertAI_21_end
	INCDEC		-2

ExpertAI_21_end:
	AIEND

ExpertAI_21_Table:
	.long	POKETYPE_HONOO
	.long	POKETYPE_MIZU
	.long	POKETYPE_KUSA
	.long	POKETYPE_DENKI
	.long	POKETYPE_ESPER
	.long	POKETYPE_KOORI
	.long	POKETYPE_DRAGON
	.long	POKETYPE_AKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_22:	// ���h�_�E��
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_22_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,3,ExpertAI_22_2
ExpertAI_22_1:
	IF_RND_UNDER	50,ExpertAI_22_2
	INCDEC		-2
ExpertAI_22_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_22_end

	INCDEC		-2
ExpertAI_22_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_23:	//����������
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_23_2
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_23_3

ExpertAI_23_2:
	IF_RND_UNDER	100,ExpertAI_23_3
	INCDEC		-1
ExpertAI_23_3:
	IF_PARA_OVER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_23_4

	IF_RND_UNDER	80,ExpertAI_23_4
	INCDEC		-2

ExpertAI_23_4:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_DOKUDOKU,ExpertAI_23_5
	IF_RND_UNDER	70,ExpertAI_23_5
	INCDEC		2

ExpertAI_23_5:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_23_6
	IF_RND_UNDER	70,ExpertAI_23_6
	INCDEC		2

ExpertAI_23_6:
	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_NEWOHARU,ExpertAI_23_6_1
	IF_RND_UNDER	128,ExpertAI_23_7
	INCDEC		1
	JUMP	ExpertAI_23_7

//�A�N�A�����O�ǉ�2006.6.14
ExpertAI_23_6_1:
//@todo	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_AQUARING,ExpertAI_23_7
	IF_RND_UNDER	128,ExpertAI_23_7
	INCDEC		1
	
ExpertAI_23_7:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_23_8	// �̂낢
	IF_RND_UNDER	70,ExpertAI_23_8
	INCDEC		2
ExpertAI_23_8:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_23_end

	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_HIT,6,ExpertAI_23_end

	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_23_9
	IF_HP_UNDER	CHECK_DEFENCE,40,ExpertAI_23_9
	
	IF_RND_UNDER	70,ExpertAI_23_end

ExpertAI_23_9:
	INCDEC		-2
ExpertAI_23_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_24:	// ��𗦃_�E��
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_24_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,3,ExpertAI_24_2
ExpertAI_24_1:
	IF_RND_UNDER	50,ExpertAI_24_2
	INCDEC		-2
ExpertAI_24_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_24_end

	INCDEC		-2
ExpertAI_24_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_25:	//���낢����
	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,8,ExpertAI_25_1

	IF_PARA_UNDER	CHECK_DEFENCE,PARA_POW,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_DEF,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEPOW,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEDEF,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_HIT,4,ExpertAI_25_1
	JUMP		ExpertAI_25_2	

ExpertAI_25_1:	
	IF_RND_UNDER	50,ExpertAI_25_2

	INCDEC		-3

ExpertAI_25_2:	
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_25_3

	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_25_3

	IF_RND_UNDER	50,ExpertAI_25_4
	INCDEC		-1
	JUMP		ExpertAI_25_4	

ExpertAI_25_3:	
	IF_RND_UNDER	50,ExpertAI_25_4

	INCDEC		3
ExpertAI_25_4:	

	AIEND

//---------------------------------------------------------------------------

ExpertAI_26:	//���܂�
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_26_1

	INCDEC		-2
ExpertAI_26_1:	
	AIEND

//---------------------------------------------------------------------------
ExpertAI_28:	//�ӂ��Ƃ΂�

	CHECK_SLOWSTART_TURN	CHECK_DEFENCE	//�X���[�X�^�[�g
	IF_OVER	3,	ExpertAI_28_ok1
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,ExpertAI_28_1	// �܂��т���
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,ExpertAI_28_1	// �X�e���X���b�N��
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,ExpertAI_28_1	// �ǂ��т���

	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_28_1

	INCDEC		-3
	JUMP		ExpertAI_28_end

ExpertAI_28_ok1:
	IF_RND_UNDER	64,ExpertAI_28_1
	INCDEC		2

ExpertAI_28_1:	
	IF_RND_UNDER	128,ExpertAI_28_end
	
	INCDEC		2
ExpertAI_28_end:	

	AIEND
//---------------------------------------------------------------------------
ExpertAI_30:	//�e�N�X�`���[
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_30_1

	INCDEC		-2
ExpertAI_30_1:	
	CHECK_TURN
	IF_EQUAL	0,ExpertAI_30_end
	
	IF_RND_UNDER	200,AI_DEC2
ExpertAI_30_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_132:	//�����̂Ђ���
ExpertAI_133:	//������������
ExpertAI_134:	//���̂Ђ���
	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,ExpertAI_134_1		//�@����ꒆ
	IF_EQUAL	WEATHER_AME,ExpertAI_134_1		//�@���ߒ�
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_134_1	//�@���Ȃ��炵��
	JUMP		ExpertAI_32

ExpertAI_134_1:
	INCDEC		-2

ExpertAI_214:	// �͂˂₷��
ExpertAI_157:	// ���܂�����
ExpertAI_162:	// �݂̂���
ExpertAI_32:	//������������
	IF_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_32_2

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_32_3

	INCDEC		-8
	JUMP		ExpertAI_32_end

ExpertAI_32_1:	
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_32_4
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_32_2

	IF_RND_UNDER	70,ExpertAI_32_4
ExpertAI_32_2:	
	INCDEC		-3
	JUMP		ExpertAI_32_end

ExpertAI_32_3:	
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_32_4
	IF_RND_UNDER	30,ExpertAI_32_4
	INCDEC		-3
	JUMP		ExpertAI_32_end

ExpertAI_32_4:	
	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,195,ExpertAI_32_5		// �悱�ǂ�

	IF_RND_UNDER	100,ExpertAI_32_end
ExpertAI_32_5:	
	IF_RND_UNDER	20,ExpertAI_32_end

	INCDEC		2

ExpertAI_32_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_84:	// ��ǂ肬
ExpertAI_33:	//�ǂ��ǂ�
	IFN_HAVE_DAMAGE_WAZA	ExpertAI_33_2

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_33_1		// HP��1/2�ȏ�

	IF_RND_UNDER	50,ExpertAI_33_1
	INCDEC		-3
ExpertAI_33_1:	
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_33_2		// HP��1/2�ȏ�

	IF_RND_UNDER	50,ExpertAI_33_2
	INCDEC		-3
ExpertAI_33_2:	
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,14,ExpertAI_33_3	// ��𗦃A�b�v
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,111,ExpertAI_33_3	// �܂���
	JUMP		ExpertAI_33_end

ExpertAI_33_3:	
	IF_RND_UNDER	60,ExpertAI_33_end
	INCDEC		2

ExpertAI_33_end:	

	AIEND
//---------------------------------------------------------------------------
ExpertAI_35:	//�Ђ���̂���
	
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_35_2		// HP��1/2�ȉ�
	
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_35_1		// HP90����
	IF_RND_UNDER	128,ExpertAI_35_1
	INCDEC		1

ExpertAI_35_1:
	CHECK_LAST_WAZA_KIND	//����̍Ō�̋Z�̕��ރ`�F�b�N
	IFN_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_35_end//����łȂ�
	IF_RND_UNDER	64,ExpertAI_35_end
	INCDEC		1
	JUMP	ExpertAI_35_end
	
ExpertAI_35_2:
	INCDEC		-2

ExpertAI_35_end:
	AIEND
	
	
//�c�o���番�ނ��^�C�v�łȂ��Ȃ����̂ňȉ��̃`�F�b�N�͂Ƃ߂�2006.6.14-------------------------------
//	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_35_2		// HP��1/2�ȉ�
//	CHECK_TYPE	CHECK_DEFENCE_TYPE1
//	IF_TABLE_JUMP	ExpertAI_35_Table,ExpertAI_35_end
//	CHECK_TYPE	CHECK_DEFENCE_TYPE2
//	IF_TABLE_JUMP	ExpertAI_35_Table,ExpertAI_35_end
//	IF_RND_UNDER	50,ExpertAI_35_end
//ExpertAI_35_2:
//	INCDEC		-2

//ExpertAI_35_end:
//	AIEND

ExpertAI_35_Table:
	.long	POKETYPE_HONOO
	.long	POKETYPE_MIZU
	.long	POKETYPE_KUSA
	.long	POKETYPE_DENKI
	.long	POKETYPE_ESPER
	.long	POKETYPE_KOORI
	.long	POKETYPE_DRAGON
	.long	POKETYPE_AKU
	.long	0xffffffff
//-----------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_37:	//�˂ނ�
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_37_3
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_37_1

	INCDEC		-8
	JUMP		ExpertAI_37_end

ExpertAI_37_1:	
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_37_5
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_37_2

	IF_RND_UNDER	70,ExpertAI_37_5
ExpertAI_37_2:	
	INCDEC		-3
	JUMP		ExpertAI_37_end

ExpertAI_37_3:	
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_37_5
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_37_4
	IF_RND_UNDER	50,ExpertAI_37_5
ExpertAI_37_4:	
	INCDEC		-3
	JUMP		ExpertAI_37_end

ExpertAI_37_5:	
	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,195,ExpertAI_37_6		// �悱�ǂ�

	IF_RND_UNDER	50,ExpertAI_37_end
ExpertAI_37_6:	
	IF_RND_UNDER	10,ExpertAI_37_end

	INCDEC		3

ExpertAI_37_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_38:	//���������Ђ�����
	IF_RND_UNDER	192,ExpertAI_38_end
	INCDEC		+1
ExpertAI_38_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_40:	//������̂܂���
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_40_end

	INCDEC		-1
ExpertAI_40_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_106:	// ���낢�܂Ȃ���
ExpertAI_42:	//���߂�
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_DOKUDOKU,ExpertAI_42_inc
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_42_inc	// �̂낢
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_HOROBINOUTA,ExpertAI_42_inc	// �ق��
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_42_inc	//��������
	JUMP		ExpertAI_42_end
ExpertAI_42_inc:	

	IF_RND_UNDER	128,ExpertAI_42_end
	INCDEC		1
ExpertAI_42_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_43:	//�N���e�B�J���ł₷��
ExpertAI_200:	//�N���e�B�J���ł₷��
ExpertAI_209:	//�N���e�B�J���ł₷��
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_43_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_43_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_43_end

	CHECK_WAZA_AISYOU	AISYOU_2BAI,ExpertAI_43_1
	CHECK_WAZA_AISYOU	AISYOU_4BAI,ExpertAI_43_1

	IF_RND_UNDER		128,ExpertAI_43_end
ExpertAI_43_1:	
	IF_RND_UNDER		128,ExpertAI_43_end
	INCDEC		1
ExpertAI_43_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_118:	//���΂�
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_ZIKOANZI, ExpertAI_IbaruAnziCombo

ExpertAI_120:	//��������
ExpertAI_166:	// �����Ă�
	IF_RND_UNDER		128,ExpertAI_49
	INCDEC		1
ExpertAI_49:	//������
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_49_end

	IF_RND_UNDER		128,ExpertAI_49_1
	INCDEC		-1
ExpertAI_49_1:
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_49_end
	INCDEC		-1
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_49_end
	INCDEC		-1
ExpertAI_49_end:	
	AIEND

ExpertAI_IbaruAnziCombo:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,3,ExpertAI_IbaruAnziComboNg
	INCDEC			3
	CHECK_TURN
	IFN_EQUAL	0,ExpertAI_IbaruAnziComboEnd
	INCDEC			2
	JUMP		ExpertAI_IbaruAnziComboEnd

ExpertAI_IbaruAnziComboNg:
	INCDEC	-5
ExpertAI_IbaruAnziComboEnd:
	AIEND

//---------------------------------------------------------------------------

ExpertAI_65:	// ���t���N�^�[
		
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_65_2		// HP��1/2�ȉ�
	
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_65_1		// HP90����
	IF_RND_UNDER	128,ExpertAI_65_1
	INCDEC		1

ExpertAI_65_1:
	CHECK_LAST_WAZA_KIND	//����̍Ō�̋Z�̕��ރ`�F�b�N
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_65_end//����łȂ�
	IF_RND_UNDER	64,ExpertAI_65_end
	INCDEC		1
	JUMP	ExpertAI_65_end
	
ExpertAI_65_2:
	INCDEC		-2

ExpertAI_65_end:
	AIEND

	
//�c�o���番�ނ��^�C�v�łȂ��Ȃ����̂ňȉ��̃`�F�b�N�͂Ƃ߂�2006.6.14-------------------------------
	
	//IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_65_2		// HP��1/2�ȉ�

	//CHECK_TYPE	CHECK_DEFENCE_TYPE1
	//IF_TABLE_JUMP	ExpertAI_65_Table,ExpertAI_65_end
	//CHECK_TYPE	CHECK_DEFENCE_TYPE2
	//IF_TABLE_JUMP	ExpertAI_65_Table,ExpertAI_65_end

	//IF_RND_UNDER	50,ExpertAI_65_end
//ExpertAI_65_2:
	//INCDEC		-2

//ExpertAI_65_end:
	//AIEND

ExpertAI_65_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_HIKOU
	.long	POKETYPE_DOKU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_GHOST
	.long	POKETYPE_HAGANE
	.long	0xffffffff
//--------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_66:	// �ǂ�
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_66_1		// HP��1/2�ȉ�
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_66_end	// HP��1/2�ȏ�
ExpertAI_66_1:
	INCDEC	-1
ExpertAI_66_end:
	AIEND
//---------------------------------------------------------------------------

ExpertAI_67:	// �܂�
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_67_1

	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_67_end	

	INCDEC	-1
	JUMP		ExpertAI_67_end	

ExpertAI_67_1:
	IF_RND_UNDER	20,ExpertAI_67_end
	INCDEC	3

ExpertAI_67_end:
	AIEND
//������x���ꍇ��20/255�̊m����+3
//������������g�o70���ȉ��Ȃ�-1

//---------------------------------------------------------------------------

ExpertAI_78:	// ���Ă݂Ȃ�
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_78_end
	
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_78_end	
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_78_2	

	IF_RND_UNDER	180,ExpertAI_78_end

ExpertAI_78_2:
	IF_RND_UNDER	50,ExpertAI_78_end
	INCDEC	-1
ExpertAI_78_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_79:	// �݂����
	IFN_HAVE_WAZA	CHECK_ATTACK, WAZANO_KIAIPANTI, ExpertAI_79_hp		//�݂����
	IF_RND_UNDER	96,ExpertAI_79_hp
	INCDEC	+1

ExpertAI_79_hp:
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_79_3	
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_79_2	
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_79_1	

	IF_RND_UNDER	100,ExpertAI_79_1
	INCDEC	-1
ExpertAI_79_1:
	IF_RND_UNDER	100,ExpertAI_79_2
	INCDEC	-1
ExpertAI_79_2:
	IF_RND_UNDER	100,ExpertAI_79_3
	INCDEC	-1
ExpertAI_79_3:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_79_end

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IF_EQUAL	1,ExpertAI_79_6		// �˂ނ�
	IF_EQUAL	33,ExpertAI_79_6	// �ǂ��ǂ�
	IF_EQUAL	66,ExpertAI_79_6	// �ǂ�
	IF_EQUAL	67,ExpertAI_79_6	// �܂�
	IF_EQUAL	167,ExpertAI_79_6	// �₯��
	IF_EQUAL	49,ExpertAI_79_7	// ������
	IF_EQUAL	84,ExpertAI_79_8	// ��ǂ肬
	JUMP		ExpertAI_79_end

ExpertAI_79_6:
	IFN_POKESICK	CHECK_DEFENCE,ExpertAI_79_ok	
	JUMP		ExpertAI_79_end

ExpertAI_79_7:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_79_ok	
	JUMP		ExpertAI_79_end

ExpertAI_79_8:
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_79_end

ExpertAI_79_ok:
	IF_RND_UNDER	100,ExpertAI_79_end
	INCDEC		1

ExpertAI_79_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_80:	// �͂�����������
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_80_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_80_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_80_ng
	
	CHECK_TOKUSEI	CHECK_ATTACK	//2006.8.7�����ǉ�
	IF_EQUAL	TOKUSYU_NAMAKE,ExpertAI_80_ok			// �Ȃ܂�

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_80_1

	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_80_ng	
	JUMP		ExpertAI_80_end	

ExpertAI_80_ok:
	IF_RND_UNDER	80,ExpertAI_80_end
	INCDEC	+1
	JUMP		ExpertAI_80_end	

ExpertAI_80_1:
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_80_end	

ExpertAI_80_ng:
	INCDEC	-1

ExpertAI_80_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_86:	// ���Ȃ��΂�
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_86_end

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_86_1

	INCDEC	1
	JUMP		ExpertAI_86_end	

ExpertAI_86_1:
	IF_RND_UNDER	100,ExpertAI_86_end

	INCDEC	-1

ExpertAI_86_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_89:	// �J�E���^�[
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_89_ng
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_89_ng	// ��������
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_89_ng		// ������	

	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_89_1	

	IF_RND_UNDER	10,ExpertAI_89_1
	INCDEC	-1
ExpertAI_89_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_89_2	

	IF_RND_UNDER	100,ExpertAI_89_2
	INCDEC	-1

ExpertAI_89_2:
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_MIRAAKOOTO, ExpertAI_89_6

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_89_4

	IFN_CHOUHATSU	ExpertAI_89_3	

	IF_RND_UNDER	100,ExpertAI_89_3
	INCDEC	1

ExpertAI_89_3:
	//�Ō�ɏo�����Z�������łȂ����ɕύX2006.6.14
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_89_ng

	//�G�������h�܂ł͍Ō�̋Z�^�C�v�Ŕ��f2006.6.14
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_TYPE
	//IFN_TABLE_JUMP	ExpertAI_89_Table,ExpertAI_89_ng

ExpertAI_89_3_2:
	IF_RND_UNDER	100,ExpertAI_89_end
	INCDEC	1
	JUMP	ExpertAI_89_end

ExpertAI_89_4://�З͂Ȃ�
	IFN_CHOUHATSU	ExpertAI_89_5	

	IF_RND_UNDER	100,ExpertAI_89_5
	INCDEC	1

ExpertAI_89_5://����̃^�C�v���݂Ĕ���
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_TABLE_JUMP	ExpertAI_89_Table,ExpertAI_89_end
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_TABLE_JUMP	ExpertAI_89_Table,ExpertAI_89_end

	IF_RND_UNDER	50,ExpertAI_89_end

ExpertAI_89_6:	// �~���[�R�[�g�Ɨ��������Ă�
	IF_RND_UNDER	100,ExpertAI_89_6_end
	INCDEC			4
ExpertAI_89_6_end:
	AIEND


ExpertAI_89_ng:
	INCDEC	-1
ExpertAI_89_end:
	AIEND

ExpertAI_89_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_HIKOU
	.long	POKETYPE_DOKU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_GHOST
	.long	POKETYPE_HAGANE
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_90:	// �A���R�[��
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KANASIBARI,ExpertAI_90_1

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_90_6

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IFN_TABLE_JUMP	ExpertAI_90_Table,ExpertAI_90_6

ExpertAI_90_1:
	IF_RND_UNDER		30,ExpertAI_90_end
	INCDEC	3
	JUMP			ExpertAI_90_end

ExpertAI_90_6:
	INCDEC		-2
ExpertAI_90_end:
	AIEND

ExpertAI_90_Table:
	.long	8,10,11,12,13,25,28,30,33,35
	.long	37,40,54,49,66,67,84,85,50,90
	.long	93,94,102,106,107,111,191,113,114,115
	.long	116,118,120,124,136,137,142,143,148,158
	.long	160,161,162,164,165,167,172,174,177,178
	.long	181,184,188,191,192,193,194,199,201,210
	.long	212,213
	//�c�o�ǉ�
	.long	215,216,220,222,223,225,226,233,234,236
	.long	238,239,240,243,244,247,250,251,252,259
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_91:	// �����݂킯
	IF_HP_UNDER	CHECK_DEFENCE,80,ExpertAI_91_2	

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_91_1

	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_91_2	
	INCDEC		1
	JUMP		ExpertAI_91_end	

ExpertAI_91_1:
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_91_2	
	INCDEC		1
	JUMP		ExpertAI_91_end	
	
ExpertAI_91_2:
	INCDEC		-1
ExpertAI_91_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_92:	// ���т�
	INCDEC	2
	AIEND
//---------------------------------------------------------------------------
ExpertAI_94:	// ���b�N�I��
	IF_RND_UNDER	128,ExpertAI_94_end

	INCDEC		2
ExpertAI_94_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_97:	// �˂���
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_NEMURI,AI_INC10
	INCDEC	-5
	AIEND
//---------------------------------------------------------------------------
ExpertAI_98:	// �݂�����
	INCDEC		-1
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_98_end

	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_98_end	

	IF_RND_UNDER	128,ExpertAI_98_1
	INCDEC		1
ExpertAI_98_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_98_end	

	IF_RND_UNDER	128,ExpertAI_98_2
	INCDEC		1
ExpertAI_98_2:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_98_end	

	IF_RND_UNDER	100,ExpertAI_98_end
	INCDEC		2

ExpertAI_98_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_99:	// ������������
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_99_2

	IF_HP_OVER	CHECK_ATTACK,33,ExpertAI_99_ng	
	IF_HP_OVER	CHECK_ATTACK,20,ExpertAI_99_end	
	IF_HP_UNDER	CHECK_ATTACK,8,ExpertAI_99_ok1	
	JUMP		ExpertAI_99_ok2	
	
ExpertAI_99_2:
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_99_ng	
	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_99_end	
	JUMP		ExpertAI_99_ok2	
	
ExpertAI_99_ok1:
	INCDEC		1
ExpertAI_99_ok2:
	IF_RND_UNDER	100,ExpertAI_99_end
	INCDEC		1
	JUMP		ExpertAI_99_end	

ExpertAI_99_ng:
	INCDEC		-1

ExpertAI_99_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_102:	// ���₵�̂���
//�t���Ƃ�����ꂤ�̂ŏC��2006.6.14	
	//IF_POKESICK		CHECK_DEFENCE,ExpertAI_102_end
	//IF_BENCH_COND	CHECK_DEFENCE,ExpertAI_102_end
	IF_POKESICK		CHECK_ATTACK,ExpertAI_102_end
	IF_BENCH_COND	CHECK_ATTACK,ExpertAI_102_end
	INCDEC		-5

ExpertAI_102_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_105:	// �ǂ�ڂ�
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_105_Table,ExpertAI_105_ng

	IF_RND_UNDER	50,ExpertAI_105_end
	INCDEC		1
	JUMP		ExpertAI_105_end	

ExpertAI_105_ng:
	INCDEC		-2
ExpertAI_105_end:
	AIEND

ExpertAI_105_Table:
	.long	SOUBI_NEMUKEZAMASI
	.long	SOUBI_ZYOUTAIIZYOUNAOSI
	.long	SOUBI_HPKAIHUKU
	.long	SOUBI_MEITYUURITUDOWN
	.long	SOUBI_HPKAIHUKUMAITURN
	.long	SOUBI_PIKATYUUTOKUKOUNIBAI
	.long	SOUBI_KARAGARADATOKOUGEKINIBAI
	//�ȍ~�ǉ�2006.6.14
	.long	SOUBI_HONOWOBATUGUNGUARD
	.long	SOUBI_MIZUBATUGUNGUARD
	.long	SOUBI_DENKIBATUGUNGUARD
	.long	SOUBI_KUSABATUGUNGUARD
	.long	SOUBI_KOORIBATUGUNGUARD
	.long	SOUBI_KAKUTOUBATUGUNGUARD
	.long	SOUBI_DOKUBATUGUNGUARD
	.long	SOUBI_ZIMENBATUGUNGUARD
	.long	SOUBI_HIKOUBATUGUNGUARD
	.long	SOUBI_ESUPAABATUGUNGUARD
	.long	SOUBI_MUSIGUARD
	.long	SOUBI_IWAGUARD
	.long	SOUBI_GOOSUTOGUARD
	.long	SOUBI_DORAGONGUARD
	.long	SOUBI_AKUGUARD
	.long	SOUBI_HAGANEGUARD
	.long	SOUBI_NOOMARUHANGEN
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_109:	// �̂낢
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_107_4	// �S�[�X�g�^�C�v
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_107_4	// �S�[�X�g�^�C�v

	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,9,ExpertAI_107_end

	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_ZYAIROBOORU, ExpertAI_107_zyairo		//�W���C���{�[��
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_TORIKKURUUMU, ExpertAI_107_zyairo		//�g���b�N���[��
	JUMP	ExpertAI_107_nozyairo

ExpertAI_107_zyairo:
	IF_RND_UNDER	32,ExpertAI_107_1
	INCDEC		1
ExpertAI_107_nozyairo:
	IF_RND_UNDER	128,ExpertAI_107_1
	INCDEC		1
ExpertAI_107_1:
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_107_end
	
	IF_RND_UNDER	128,ExpertAI_107_2
	INCDEC		1
ExpertAI_107_2:
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,6,ExpertAI_107_end

	IF_RND_UNDER	128,ExpertAI_107_end
	INCDEC		1
	JUMP		ExpertAI_107_end

ExpertAI_107_4:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_107_end	

	INCDEC		-1
ExpertAI_107_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_111:	// �܂���
	//�t�F�C���g���V���h�[�_�C�u
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_FEINTO, ExpertAI_111_mamoriyaburi
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_SYADOODAIBU, ExpertAI_111_mamoriyaburi
	JUMP	ExpertAI_111_mamoriyaburanai

ExpertAI_111_mamoriyaburi:
	IF_RND_UNDER	128,ExpertAI_111_mamoriyaburanai
	INCDEC		-2

ExpertAI_111_mamoriyaburanai:
	CHECK_MAMORU_COUNT	CHECK_ATTACK
	IF_OVER		1,ExpertAI_111_ng2
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_DOKUDOKU,ExpertAI_111_ng1	// �ǂ��ǂ�
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_NOROI,ExpertAI_111_ng1		// �̂낢
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_HOROBINOUTA,ExpertAI_111_ng1	// �ق��
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_MEROMERO,ExpertAI_111_ng1	//��������
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_YADORIGI,ExpertAI_111_ng1	// ��ǂ肬
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_AKUBI,ExpertAI_111_ng1		// ������
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_111_ng1		// ������������
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,156,ExpertAI_111_ng1		// �˂ނ�

	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_DOKUDOKU,ExpertAI_111_ok	// �ǂ��ǂ�
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_111_ok		// �̂낢
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_HOROBINOUTA,ExpertAI_111_ok	// �ق��
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_111_ok	//��������
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_111_ok	// ��ǂ肬
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_AKUBI,ExpertAI_111_ok		// ������
	
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_DOUBLE,ExpertAI_111_ok							//2VS2
	
	//���肪���b�N�I�������ĂȂ���������F�`�f�a����Ԉ���Ă���Ǝv����̂ŏC��2006.8.9
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_SEQNO
	//IFN_EQUAL	94,ExpertAI_111_ok					
	
	//���������b�N�I������Ă�������
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_MUSTHIT_TARGET,ExpertAI_111_ok	//���b�N�I�����ꒆ
	IF_RND_UNDER	85,ExpertAI_111_ok	//33%
	JUMP		ExpertAI_111_1

ExpertAI_111_ok:
	INCDEC		2

ExpertAI_111_1:
	IF_RND_UNDER	128,ExpertAI_111_1_2
	INCDEC		-1
ExpertAI_111_1_2:
	CHECK_MAMORU_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_111_end
	INCDEC		-1
	IF_RND_UNDER	128,ExpertAI_111_end
	INCDEC		-1
	JUMP		ExpertAI_111_end

ExpertAI_111_ng1:
	
	//���肪���b�N�I�������ĂȂ���������F�`�f�a����Ԉ���Ă���Ǝv����̂ŏC��2006.8.9
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_SEQNO
	//IFN_EQUAL	94,ExpertAI_111_end		
	
	//���������b�N�I��������Ă�����
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_MUSTHIT_TARGET,ExpertAI_111_end	//���b�N�I�����ꒆ

ExpertAI_111_ng2:
	INCDEC		-2
ExpertAI_111_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_112:	// �܂��т�
	IF_RND_UNDER	128,ExpertAI_112_end
	INCDEC	+1
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HOERU, ExpertAI_112_ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUKITOBASI, ExpertAI_112_ok
	JUMP	ExpertAI_112_end
	
ExpertAI_112_ok:
	IF_RND_UNDER	64,ExpertAI_112_end
	INCDEC	+1

ExpertAI_112_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_113:	// �݂�Ԃ�:��𗦂��U�����ɂȂ��Ă���̂�������ɏC��2006.6.14
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_113_ok1	// �S�[�X�g�^�C�v
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_113_ok1	// �S�[�X�g�^�C�v

	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_113_ok2

	INCDEC		-2
	JUMP		ExpertAI_113_end


ExpertAI_113_ok1:
	IF_RND_UNDER	80,ExpertAI_113_end
ExpertAI_113_ok2:
	IF_RND_UNDER	80,ExpertAI_113_end

	INCDEC		2
ExpertAI_113_end:

	AIEND
//---------------------------------------------------------------------------
ExpertAI_116:	// ���炦��
	IF_HP_UNDER	CHECK_ATTACK,4,ExpertAI_116_ng	
	IF_HP_UNDER	CHECK_ATTACK,35,ExpertAI_116_ok	

ExpertAI_116_ng:
	INCDEC		-1
	JUMP		ExpertAI_116_end

ExpertAI_116_ok:
	IF_RND_UNDER	70,ExpertAI_116_end
	INCDEC		1
ExpertAI_116_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_127:	// �o�g���^�b�`
	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,8,ExpertAI_127_1
	JUMP		ExpertAI_127_5

ExpertAI_127_1:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_127_2
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_127_end	
	JUMP		ExpertAI_127_3

ExpertAI_127_2:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_127_end	

ExpertAI_127_3:
	IF_RND_UNDER	80,ExpertAI_127_end
	INCDEC		2
	JUMP		ExpertAI_127_end

ExpertAI_127_5:
	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,7,ExpertAI_127_6
	JUMP		ExpertAI_127_ng

ExpertAI_127_6:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_127_7
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_127_ng	
	JUMP		ExpertAI_127_end

ExpertAI_127_7:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_127_end	

ExpertAI_127_ng:
	INCDEC		-2

ExpertAI_127_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_128:	// ��������
	CHECK_NEKODAMASI	CHECK_ATTACK
	//IFN_EQUAL	0,ExpertAI_128_ok2
	IFN_EQUAL	0,ExpertAI_128_ok1

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_128_ok1
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_ESPER,ExpertAI_128_ok1

	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_128_ok1
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_ESPER,ExpertAI_128_ok1
	
	JUMP		ExpertAI_128_ok2

ExpertAI_128_ok1:
	IF_RND_UNDER	128,ExpertAI_128_ok2
	INCDEC		1

ExpertAI_128_ok2:
	IFN_HAVE_WAZA	CHECK_DEFENCE, WAZANO_TONBOGAERI, ExpertAI_128_end
	IF_RND_UNDER	128,ExpertAI_128_end
	INCDEC		1

ExpertAI_128_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_136:	// ���܂���
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_136_1

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_SUISUI,ExpertAI_136_ok			// ��������

ExpertAI_136_1:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_136_ng	

	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,ExpertAI_136_ok		//�@����ꒆ
	IF_EQUAL	WEATHER_HARE,ExpertAI_136_ok		//�@�͂ꒆ
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_136_ok	//�@���Ȃ��炵��

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_AMEUKEZARA,ExpertAI_136_ok		// ���߂�������
	IFN_EQUAL	TOKUSYU_URUOIBODHI,ExpertAI_136_end		// ���邨���{�f�B�łȂ�
	IF_POKESICK	CHECK_ATTACK,ExpertAI_136_ok		// ��Ԉُ펞
	JUMP		ExpertAI_136_end

ExpertAI_136_ok:
	INCDEC		1
	JUMP		ExpertAI_136_end

ExpertAI_136_ng:
	INCDEC		-1

ExpertAI_136_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_137:	// �ɂق�΂�
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_137_ng	

	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,ExpertAI_137_ok		//�@����ꒆ
	IF_EQUAL	WEATHER_AME,ExpertAI_137_ok		//�@���ߒ�
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_137_ok	//�@���Ȃ��炵��
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_HURAWAAGIHUTO,ExpertAI_137_ok		// �t�����[�M�t�g
	IFN_EQUAL	TOKUSYU_RIIHUGAADO,ExpertAI_137_end		// ���[�t�K�[�h�łȂ�
	IF_POKESICK	CHECK_ATTACK,ExpertAI_137_ok		// ��Ԉُ펞
	
	JUMP		ExpertAI_137_end

ExpertAI_137_ok:
	INCDEC		1
	JUMP		ExpertAI_137_end

ExpertAI_137_ng:
	INCDEC		-1

ExpertAI_137_end:
	AIEND



//---------------------------------------------------------------------------
ExpertAI_142:	// �͂炾����
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_142_ng	
	JUMP	ExpertAI_142_end

ExpertAI_142_ng:
	INCDEC		-2
ExpertAI_142_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_143:	// ��������
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_143_1
	JUMP	ExpertAI_143_ng

ExpertAI_143_1:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,7,ExpertAI_143_ok1

	IF_RND_UNDER	50,ExpertAI_143_end
	JUMP	ExpertAI_143_ng

ExpertAI_143_ok1:
	INCDEC		1
ExpertAI_143_ok2:
	INCDEC		1
	AIEND
ExpertAI_143_ng:
	INCDEC		-2
ExpertAI_143_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_144:	// �~���[�R�[�g
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_144_ng
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_144_ng	// ��������
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_144_ng		// ������	

	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_144_1	

	IF_RND_UNDER	10,ExpertAI_144_1
	INCDEC	-1
ExpertAI_144_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_144_2	

	IF_RND_UNDER	100,ExpertAI_144_2
	INCDEC	-1
ExpertAI_144_2:
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_KAUNTAA, ExpertAI_144_6

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_144_4

	IFN_CHOUHATSU	ExpertAI_144_3	
	IF_RND_UNDER	100,ExpertAI_144_3
	INCDEC	1
ExpertAI_144_3:
	//�Ō�ɏo�����Z������łȂ����ɕύX2006.6.14
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_144_ng

	//�G�������h�܂ł͍Ō�̋Z�^�C�v�Ŕ��f2006.6.14
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_TYPE
	//IFN_TABLE_JUMP	ExpertAI_144_Table,ExpertAI_144_ng

ExpertAI_144_3_2:
	IF_RND_UNDER	100,ExpertAI_144_end
	INCDEC	1
	JUMP	ExpertAI_144_end

ExpertAI_144_4:
	IFN_CHOUHATSU	ExpertAI_144_5	
	IF_RND_UNDER	100,ExpertAI_144_5
	INCDEC	1
ExpertAI_144_5:
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_TABLE_JUMP	ExpertAI_144_Table,ExpertAI_144_end
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_TABLE_JUMP	ExpertAI_144_Table,ExpertAI_144_end

	IF_RND_UNDER	50,ExpertAI_144_end

ExpertAI_144_6:	// �J�E���^�[�Ɨ��������Ă�
	IF_RND_UNDER	100,ExpertAI_144_6_end
	INCDEC			4
ExpertAI_144_6_end:
	AIEND


ExpertAI_144_ng:
	INCDEC	-1
ExpertAI_144_end:
	AIEND

ExpertAI_144_Table:
	.long	POKETYPE_HONOO
	.long	POKETYPE_MIZU
	.long	POKETYPE_KUSA
	.long	POKETYPE_DENKI
	.long	POKETYPE_ESPER
	.long	POKETYPE_KOORI
	.long	POKETYPE_DRAGON
	.long	POKETYPE_AKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_39:	// ���܂�����
ExpertAI_75:	// �S�b�h�o�[�h
ExpertAI_145:	// ���P�b�g����
ExpertAI_151:	// �\�[���[�r�[��
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_151_ng1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_151_ng1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_151_ng1

	//�ɂق�΂�`�F�b�N�F�\�[���[�r�[��
	IF_WAZA_SEQNO_JUMP	151,ExpertAI_151_hare		// �\�[���[�r�[���̂�	2006.7.20
	JUMP	ExpertAI_151_itemcheck

ExpertAI_151_hare:	
	CHECK_WEATHER
	IFN_EQUAL	WEATHER_HARE,ExpertAI_151_itemcheck		// ���{���ꒆ
	INCDEC	+2
	JUMP		ExpertAI_151_end

ExpertAI_151_itemcheck:
	//�p���t���n�[�u�`�F�b�N
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_PAWAHURUHAABU,ExpertAI_151_1_item//2006.6.23
	JUMP	ExpertAI_151_1_noitem
ExpertAI_151_1_item:
	INCDEC	+2
	JUMP		ExpertAI_151_end
	
ExpertAI_151_1_noitem:
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,111,ExpertAI_151_ng1		// �܂���

	IF_HP_OVER	CHECK_ATTACK,38,ExpertAI_151_end	
	INCDEC	-1
	JUMP		ExpertAI_151_end

ExpertAI_151_ng1:
	INCDEC	-2
ExpertAI_151_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ExpertAI_152:	//���݂Ȃ�
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_152_1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_152_1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_152_1
	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,ExpertAI_152_1
	IFN_EQUAL	WEATHER_AME,ExpertAI_152_2
	INCDEC		+1
	JUMP		ExpertAI_152_2
	
ExpertAI_152_1:
	IF_RND_UNDER	50,ExpertAI_152_2
	INCDEC		-3

ExpertAI_152_2:
	AIEND

//---------------------------------------------------------------------------


//�p���t���n�[�u�`�F�b�N������\��
ExpertAI_263:	//�Ƃт͂˂�2006.6.14
ExpertAI_255:	//�_�C�r���O2006.6.14
ExpertAI_256:	//���Ȃ��ق�2006.6.14
ExpertAI_155:	// ������Ƃԁ@
	
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_PAWAHURUHAABU,ExpertAI_151_1_item//2006.6.23
	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,111,ExpertAI_155_1		// �܂���

	INCDEC	-1
	JUMP		ExpertAI_155_end

ExpertAI_272:	//�V���h�[�_�C�u�i�܂���`�F�b�N�����j2006.6.14
ExpertAI_155_1:
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_155_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_155_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_155_ng
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_PAWAHURUHAABU,ExpertAI_155_1_item//2006.6.23
	JUMP	ExpertAI_155_1_noitem
ExpertAI_155_1_item:
	INCDEC	+1
	JUMP		ExpertAI_155_end

ExpertAI_155_1_noitem:
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_DOKUDOKU,ExpertAI_155_ok	// �ǂ��ǂ�
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_155_ok		// �̂낢
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_155_ok	// ��ǂ肬
	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_155_2	//�@���Ȃ��炵��
	IF_EQUAL	WEATHER_ARARE,ExpertAI_155_3		//�@����ꒆ
	JUMP		ExpertAI_155_4

ExpertAI_155_2:
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_TABLE_JUMP	ExpertAI_155_Table,ExpertAI_155_ok
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_TABLE_JUMP	ExpertAI_155_Table,ExpertAI_155_ok
	JUMP		ExpertAI_155_4

ExpertAI_155_3:
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_KOORI,ExpertAI_155_ok
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_KOORI,ExpertAI_155_ok
	JUMP		ExpertAI_155_4

ExpertAI_155_4:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_155_end

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IFN_EQUAL	94,ExpertAI_155_ok					// ���b�N�I��

	JUMP		ExpertAI_155_end

ExpertAI_155_ok:
	IF_RND_UNDER	80,ExpertAI_155_end
	INCDEC		1
ExpertAI_155_end:
	AIEND

ExpertAI_155_ng:
	INCDEC		1
	AIEND

ExpertAI_155_Table:
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_HAGANE
	.long	0xffffffff


//---------------------------------------------------------------------------
ExpertAI_158:	// �˂����܂�ok
	INCDEC		2
ExpertAI_158_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_161:	// �͂�����
	CHECK_TAKUWAERU		CHECK_ATTACK
	IF_UNDER	2,ExpertAI_161_end

	IF_RND_UNDER	80,ExpertAI_161_end
	INCDEC		2
ExpertAI_161_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_164:	// �����
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_164_ng	

	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,ExpertAI_164_ok		//�@�͂ꒆ
	IF_EQUAL	WEATHER_AME,ExpertAI_164_ok			//�@���ߒ�
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_164_ok	//�@���Ȃ��炵��
	JUMP		ExpertAI_164_end

ExpertAI_164_ok:
	INCDEC		1

//�ӂԂ�
ExpertAI_164_hubuki:
	IFN_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUBUKI, ExpertAI_164_aisubodhi
	INCDEC		2

//�A�C�X�{�f�B
ExpertAI_164_aisubodhi:
	CHECK_TOKUSEI	CHECK_ATTACK
	IFN_EQUAL	TOKUSYU_AISUBODHI,ExpertAI_164_end
	INCDEC		2
	JUMP		ExpertAI_164_end

ExpertAI_164_ng:
	INCDEC		-1

ExpertAI_164_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_169:	// ���炰�� �ǂ��A�₯�ǁA�܂ЁA�ǂ��ǂ�
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_DOKU,ExpertAI_169_ok	
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_YAKEDO,ExpertAI_169_ok	
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MAHI,ExpertAI_169_ok	
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_DOKUDOKU,ExpertAI_169_end	

ExpertAI_169_ok:
	INCDEC		1
ExpertAI_169_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_170:	// �������p���`ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_170_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_170_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_170_ng

	IF_MIGAWARI	  CHECK_ATTACK,AI_INC5		// �݂���蒆
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_170_ok2
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_170_ok1	// ��������
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_170_ok1	// ������	

	CHECK_NEKODAMASI	CHECK_ATTACK
	IFN_EQUAL	0,ExpertAI_170_end	//�łĂ��ĂP�^�[���ڂłȂ��Ȃ�I��

	IF_RND_UNDER	200,ExpertAI_170_end
	INCDEC		1
	JUMP		ExpertAI_170_end

ExpertAI_170_ng:
	INCDEC		-1
	JUMP		ExpertAI_170_end

ExpertAI_170_ok1:
	IF_RND_UNDER	100,ExpertAI_170_end
ExpertAI_170_ok2:
	INCDEC		1
ExpertAI_170_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_171:	// ����
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_MAHI,ExpertAI_171_ok
	JUMP		ExpertAI_171_end

ExpertAI_171_ok:
	INCDEC		1
ExpertAI_171_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_177:	// �g���b�N
//�e�[�u���\�����ׂ����ݒ肷��K�v����

	CHECK_SOUBI_EQUIP	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_177_Table2,ExpertAI_177_hachimaki
	IF_TABLE_JUMP	ExpertAI_177_Table3,ExpertAI_177_doku
	IF_TABLE_JUMP	ExpertAI_177_Table4,ExpertAI_177_yakedo
	IF_TABLE_JUMP	ExpertAI_177_Table5,ExpertAI_177_hedoro
	IF_TABLE_JUMP	ExpertAI_177_Table,ExpertAI_177_ok

ExpertAI_177_ng:
	INCDEC		-3
	JUMP		ExpertAI_177_end

ExpertAI_177_hachimaki:
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	INCDEC		5
	JUMP		ExpertAI_177_end

ExpertAI_177_doku://�ǂ��ǂ�����

	CHECK_SOUBI_EQUIP	CHECK_DEFENCE	//�������̂������Ă�����Ӗ����Ȃ��̂ł��Ȃ�
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	
	IF_POKESICK	CHECK_DEFENCE,ExpertAI_177_doku_check// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_doku_check	// ����҂̂܂���
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_doku_check		// ���肪�͂��˃^�C�v
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_doku_check		// ���肪�ǂ��^�C�v 
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_doku_check		// ���肪�͂��˃^�C�v
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_doku_check	// ���肪�ǂ��^�C�v

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MENEKI,ExpertAI_177_doku_check		// �߂񂦂�
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_doku_check	// �}�W�b�N�K�[�h	2006.6.6
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,ExpertAI_177_doku_check		// �|�C�Y���q�[��	2006.6.6
	
	INCDEC		5
	JUMP		ExpertAI_177_end

	
ExpertAI_177_doku_check://���肪�łɑϐ�������Ƃ��������łɑϐ������邩�H
	
	IF_POKESICK	CHECK_ATTACK,ExpertAI_177_ng// ��Ԉُ펞�͑҂�
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_ng	// ����҂̂܂���
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_ng		// �͂��˃^�C�v
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// �ǂ��^�C�v 
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_ng		// �͂��˃^�C�v
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// �ǂ��^�C�v

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MENEKI,ExpertAI_177_ng			// �߂񂦂�
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_ng	// �}�W�b�N�K�[�h	2006.6.6
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,ExpertAI_177_ng		// �|�C�Y���q�[��	2006.6.6
	IF_EQUAL	TOKUSYU_BUKIYOU,ExpertAI_177_ng			// �Ԃ��悤

	INCDEC		5	//�ϐ����Ȃ��̂Ńg���b�N����
	JUMP		ExpertAI_177_end




ExpertAI_177_yakedo://�����񂾂�
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE	//���肪�����Ă�����Ӗ����Ȃ��̂ł��Ȃ�
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,ExpertAI_177_yakedo_check		// �݂��̃x�[��
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_yakedo_check		// �}�W�b�N�K�[�h
	IF_POKESICK	CHECK_DEFENCE,ExpertAI_177_yakedo_check		// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_yakedo_check	// ����҂̂܂���
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_yakedo_check			// �ق̂��^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_yakedo_check			// �ق̂��g�^�C�v
	
	INCDEC		5
	JUMP		ExpertAI_177_end


ExpertAI_177_yakedo_check:

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,ExpertAI_177_ng		// �݂��̃x�[��
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_ng	// �}�W�b�N�K�[�h
	IF_EQUAL	TOKUSYU_BUKIYOU,AI_DEC5					// �Ԃ��悤
	IF_POKESICK	CHECK_ATTACK,ExpertAI_177_ng	// ��Ԉُ펞�͏�Ԉُ�U�������Ȃ�
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_ng	// ����҂̂܂���
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_ng			// �ق̂��^�C�v
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_ng			// �ق̂��g�^�C�v
	
	INCDEC		5	//�ϐ����Ȃ��̂Ńg���b�N����
	JUMP		ExpertAI_177_end


ExpertAI_177_hedoro://���낢�փh��

	CHECK_SOUBI_EQUIP	CHECK_DEFENCE	//�������̂������Ă�����Ӗ����Ȃ��̂ł��Ȃ�
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_hedoro_check		// ���肪�ǂ��^�C�v 
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_hedoro_check	// ���肪�ǂ��^�C�v

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_doku_check	// �}�W�b�N�K�[�h	2006.6.6
	INCDEC		5
	JUMP		ExpertAI_177_end

	
ExpertAI_177_hedoro_check://���肪�łɑϐ�������Ƃ��������łɑϐ������邩�H
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// �ǂ��^�C�v 
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// �ǂ��^�C�v

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_ng	// �}�W�b�N�K�[�h	2006.6.6
	IF_EQUAL	TOKUSYU_BUKIYOU,ExpertAI_177_ng			// �Ԃ��悤

	INCDEC		5	//�ϐ����Ȃ��̂Ńg���b�N����
	JUMP		ExpertAI_177_end


ExpertAI_177_ok:
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_177_TableNG1,ExpertAI_177_ng

	IF_RND_UNDER	50,ExpertAI_177_end
	INCDEC		2
ExpertAI_177_end:
	AIEND

ExpertAI_177_Table://��
	.long	SOUBI_KARAIKONRAN
	.long	SOUBI_SIBUIKONRAN
	.long	SOUBI_AMAIKONRAN
	.long	SOUBI_NIGAIKONRAN
	.long	SOUBI_SUPPAIKONRAN
	.long	0xffffffff

ExpertAI_177_Table2://���ȓ���
	.long	SOUBI_ONAZIWAZAONLY
	.long	SOUBI_ONAZIAWZAONLYTOKUSYUUP
	.long	SOUBI_ONAZIWAZAONLYSUBAYASAUP
	.long	SOUBI_SUBAYASADOWN
	.long	SOUBI_KOUKOUNINARU
	.long	SOUBI_TEKINIMOTASETEDAMEEZI
	.long	SOUBI_KOUGEKIDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUKOUDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUBOUDORYOKUTIUP
	.long	SOUBI_SUBAYASADORYOKUTIUP
	.long	SOUBI_HPDORYOKUTIUP
	.long	0xffffffff

ExpertAI_177_Table3://�ǂ�
	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	0xffffffff

ExpertAI_177_Table4://�₯��
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	0xffffffff

ExpertAI_177_Table5://�w�h��
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff


ExpertAI_177_TableNG1://�G������������Ă�������Ȃ�
	.long	SOUBI_KARAIKONRAN
	.long	SOUBI_SIBUIKONRAN
	.long	SOUBI_AMAIKONRAN
	.long	SOUBI_NIGAIKONRAN
	.long	SOUBI_SUPPAIKONRAN
	
	.long	SOUBI_DORYOKUTINIBAI
	.long	SOUBI_ONAZIWAZAONLY
	.long	SOUBI_ONAZIAWZAONLYTOKUSYUUP
	.long	SOUBI_ONAZIWAZAONLYSUBAYASAUP
	.long	SOUBI_SUBAYASADOWN
	.long	SOUBI_KOUKOUNINARU
	
	.long	SOUBI_TEKINIMOTASETEDAMEEZI
	
	.long	SOUBI_KOUGEKIDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUKOUDORYOKUTIUP
	.long	SOUBI_TOKUBOUDORYOKUTIUP
	.long	SOUBI_SUBAYASADORYOKUTIUP
	.long	SOUBI_HPDORYOKUTIUP

	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff

ExpertAI_177_TableNG2://�G������������Ă�������Ȃ�
	.long	SOUBI_DORYOKUTINIBAI
	.long	SOUBI_ONAZIWAZAONLY
	.long	SOUBI_ONAZIAWZAONLYTOKUSYUUP
	.long	SOUBI_ONAZIWAZAONLYSUBAYASAUP
	.long	SOUBI_SUBAYASADOWN
	.long	SOUBI_KOUKOUNINARU
	
	.long	SOUBI_TEKINIMOTASETEDAMEEZI
	
	.long	SOUBI_KOUGEKIDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUKOUDORYOKUTIUP
	.long	SOUBI_TOKUBOUDORYOKUTIUP
	.long	SOUBI_SUBAYASADORYOKUTIUP
	.long	SOUBI_HPDORYOKUTIUP

	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_178:	// �Ȃ肫��
ExpertAI_191:	// �X�L���X���b�v
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_178_Table,ExpertAI_178_ng

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_178_Table,ExpertAI_178_ok

ExpertAI_178_ng:
	INCDEC		-1
	JUMP		ExpertAI_178_end

ExpertAI_178_ok:
	IF_RND_UNDER	50,ExpertAI_178_end
	INCDEC		2
ExpertAI_178_end:
	AIEND

ExpertAI_178_Table:
	.long	TOKUSYU_KASOKU
	.long	TOKUSYU_KABUTOAAMAA
	.long	TOKUSYU_SUNAGAKURE
	.long	TOKUSYU_SEIDENKI
	.long	TOKUSYU_MORAIBI
	.long	TOKUSYU_HUSIGINAMAMORI
	.long	TOKUSYU_HOUSI
	.long	TOKUSYU_SUISUI
	.long	TOKUSYU_TIKARAMOTI
	.long	TOKUSYU_AMEUKEZARA
	.long	TOKUSYU_MEROMEROBODHI
	.long	TOKUSYU_DAPPI
	.long	TOKUSYU_HUSIGINAUROKO
	.long	TOKUSYU_YOGAPAWAA
	.long	TOKUSYU_YOURYOKUSO
	.long	TOKUSYU_RINPUN
	//�c�o���ǉ�2006.6.14
	.long	TOKUSYU_TEKIOURYOKU
	.long	TOKUSYU_MAZIKKUGAADO
	.long	TOKUSYU_KATAYABURI
	.long	TOKUSYU_KYOUUN
	.long	TOKUSYU_TENNEN
	.long	TOKUSYU_IROMEGANE
	.long	TOKUSYU_FIRUTAA
	.long	TOKUSYU_HAADOROKKU
	.long	TOKUSYU_SUTEMI
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_181:	// �˂��͂�
	AIEND
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_182:	// �΂�������
	//���낢�n�[�u�`�F�b�N�Ȃǂ���邩��
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_182_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_182_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_182_ng

	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,6,ExpertAI_182_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_182_1
	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_182_ng
	JUMP		ExpertAI_182_end

ExpertAI_182_1:
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_182_end

ExpertAI_182_ng:
	INCDEC		-1
ExpertAI_182_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_183:	// �}�W�b�N�R�[�g
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_183_0

	IF_RND_UNDER	100,ExpertAI_183_0
	INCDEC		-1
ExpertAI_183_0:
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_183_ng

ExpertAI_183_1:
	IF_RND_UNDER	150,ExpertAI_183_end
	INCDEC		1
	JUMP		ExpertAI_183_end
ExpertAI_183_2:
	IF_RND_UNDER	50,ExpertAI_183_end

ExpertAI_183_ng:
	IF_RND_UNDER	30,ExpertAI_183_end
	INCDEC		-1
ExpertAI_183_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_184:	// ���T�C�N��
	CHECK_RECYCLE_ITEM	CHECK_ATTACK
	IFN_TABLE_JUMP	ExpertAI_184_Table,ExpertAI_184_ng

	IF_RND_UNDER	50,ExpertAI_184_end
	INCDEC		1
	JUMP		ExpertAI_184_end	

ExpertAI_184_ng:
	INCDEC		-2
ExpertAI_184_end:
	AIEND

//�c�o�p�̃A�C�e���i���o�[�ɕύX2006.6.14
ExpertAI_184_Table:
	.long	150,157,207
	//2006.6.14�ȉ��c�o���ǉ�
	//����Ȃ�
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_185:	// ���x���W
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_185_ng
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_185_ng	// ��������
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_185_ng		// ������	

	IF_RND_UNDER	180,ExpertAI_185_ng

	INCDEC		2
	JUMP		ExpertAI_185_end

ExpertAI_185_ng:
	INCDEC		-2
ExpertAI_185_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_186:	// �������
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_REFRECTOR,ExpertAI_186_ok // ���t���N�^�[��
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_HIKARINOKABE,ExpertAI_186_ok // �Ђ���̂���2006.6.14

	JUMP		ExpertAI_186_end

ExpertAI_186_ok:
	INCDEC		1
ExpertAI_186_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_188:	// �͂������Ƃ�
	IF_HP_UNDER	CHECK_DEFENCE,30,ExpertAI_188_end

	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_OVER		0,ExpertAI_188_end

	IF_RND_UNDER	180,ExpertAI_188_end
ExpertAI_188_ok:
	INCDEC		1
ExpertAI_188_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_189:	// ���ނ����
	IF_HP_UNDER	CHECK_DEFENCE,70,ExpertAI_189_2	

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_189_1

	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_189_2	
	INCDEC		1
	JUMP		ExpertAI_189_end	

ExpertAI_189_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_189_2	
	INCDEC		1
	JUMP		ExpertAI_189_end	
	
ExpertAI_189_2:
	INCDEC		-1
ExpertAI_189_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_190:	// �ӂ�
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_190_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_190_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_190_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_190_1

	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_190_end	
	JUMP		ExpertAI_190_ng	

ExpertAI_190_1:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_190_end	

ExpertAI_190_ng:
	INCDEC		-1
ExpertAI_190_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_192:	// �ӂ�����ok
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_OVER		0,ExpertAI_192_end

	IF_RND_UNDER	100,ExpertAI_192_end
	INCDEC		2
ExpertAI_192_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_193:	// ���t���b�V��
	IF_HP_UNDER	CHECK_DEFENCE,50,ExpertAI_193_ng

	JUMP		ExpertAI_193_end	

ExpertAI_193_ng:
	INCDEC		-1
ExpertAI_193_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_195:	// �悱�ǂ�
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	1,ExpertAI_195_ok1

	IF_RND_UNDER	30,ExpertAI_195_end

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_195_1

	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_195_ng

	IF_HP_UNDER	CHECK_DEFENCE,70,ExpertAI_195_ng
	IF_RND_UNDER	60,ExpertAI_195_end
	JUMP		ExpertAI_195_ng	

ExpertAI_195_1:
	IF_HP_OVER	CHECK_DEFENCE,25,ExpertAI_195_ng

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_195_ok1		// ������������
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,156,ExpertAI_195_ok1		// �˂ނ�

	JUMP		ExpertAI_195_ok2	

ExpertAI_195_ok1:
	IF_RND_UNDER	150,ExpertAI_195_end
	INCDEC		2
	JUMP		ExpertAI_195_end	

ExpertAI_195_ok2:
	IF_RND_UNDER	230,ExpertAI_195_ng
	INCDEC		1
	JUMP		ExpertAI_195_end	

ExpertAI_195_ng:
	IF_RND_UNDER	30,ExpertAI_195_end

	INCDEC		-2
ExpertAI_195_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_201:	// �ǂ날����
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_201_ng	

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_DENKI,ExpertAI_201_ok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_DENKI,ExpertAI_201_ok

	JUMP		ExpertAI_201_ng	

ExpertAI_201_ok:
	INCDEC		1
	JUMP		ExpertAI_201_end	

ExpertAI_201_ng:
	INCDEC		-1
ExpertAI_201_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_204:	// �I�[�o�[�q�[�g
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_204_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_204_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_204_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_204_1

	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_204_end
	JUMP		ExpertAI_204_ng	

ExpertAI_204_1:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_204_end

ExpertAI_204_ng:
	INCDEC		-1
ExpertAI_204_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_210:	// �݂�������
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_210_ng	

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_210_ok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_210_ok

	JUMP		ExpertAI_210_ng	

ExpertAI_210_ok:
	INCDEC		1
	JUMP		ExpertAI_210_end	

ExpertAI_210_ng:
	INCDEC		-1
ExpertAI_210_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_212:	// ��イ�̂܂�
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_212_ok

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_212_end	

	IF_RND_UNDER	70,ExpertAI_212_end
	INCDEC		-1
	JUMP		ExpertAI_212_end
	
ExpertAI_212_ok:
	IF_RND_UNDER	128,ExpertAI_212_end

	INCDEC		1
ExpertAI_212_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_215:	// ���イ��傭ok

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HUYUU,ExpertAI_215_1
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_FLYING,ExpertAI_215_1
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HIKOU,ExpertAI_215_1	// �Ђ����^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HIKOU,ExpertAI_215_1	// �Ђ����^�C�v
	
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_215_end	
	IF_RND_UNDER	128,ExpertAI_215_1
	JUMP	ExpertAI_215_end	
	
ExpertAI_215_1:	
	IF_RND_UNDER	64,ExpertAI_215_end
	INCDEC	+1
	
ExpertAI_215_end:	
	AIEND
//---------------------------------------------------------------------------


ExpertAI_216:	// �~���N���A�Cok
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_AKU,ExpertAI_216_ok1	// �����^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_AKU,ExpertAI_216_ok1	// �����^�C�v

	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_216_ok2

	INCDEC		-2
	AIEND

ExpertAI_216_ok1:
	IF_RND_UNDER	80,ExpertAI_216_end
ExpertAI_216_ok2:
	IF_RND_UNDER	80,ExpertAI_216_end
	INCDEC		2
ExpertAI_216_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_217:	// �߂��܂��r���^ok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_217_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_217_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_217_ng
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_217_ok
	JUMP		ExpertAI_217_end

ExpertAI_217_ng:
	INCDEC		-1
	JUMP	ExpertAI_217_end

ExpertAI_217_ok:
	INCDEC		1
ExpertAI_217_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_218:	// �A�[���n���}�[ok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_218_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_218_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_218_ng
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_218_ok
	
	JUMP		ExpertAI_218_end

ExpertAI_218_ng:
	INCDEC		-1
	AIEND

ExpertAI_218_ok:
	INCDEC		1
ExpertAI_218_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_219:	// �W���C���{�[��
//�_���[�W�v�Z�ɓ��ꂽ�̂ō폜

//	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_219_ng
//	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_219_ng
//	FIELD_CONDITION_CHECK	FIELD_CONDITION_TRICKROOM,ExpertAI_219_trickroom	//�g���b�N���[�������ۂ�	
//	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_219_ok
//	JUMP		ExpertAI_219_end
//ExpertAI_219_trickroom:
//	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_219_ok
//	JUMP		ExpertAI_219_end
//ExpertAI_219_ng:
//	INCDEC		-1
//	AIEND
//ExpertAI_219_ok:
//	IF_RND_UNDER	160,ExpertAI_219_end
//	INCDEC		1
//ExpertAI_219_end:
	AIEND
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
ExpertAI_221:	// �����݂�ok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_221_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_221_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_221_ng
	
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_221_end
	INCDEC	1
	IF_RND_UNDER	128,ExpertAI_221_end
	INCDEC	1
	JUMP	ExpertAI_221_end

ExpertAI_221_ng:
	INCDEC		-1

ExpertAI_221_end:
	AIEND
//---------------------------------------------------------------------------
	
//---------------------------------------------------------------------------
ExpertAI_223:	// �t�F�C���g
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,111,ExpertAI_223_mamoru		//�܂���
	IF_RND_UNDER	64,ExpertAI_223_mamoru
	JUMP	ExpertAI_223_end

ExpertAI_223_mamoru:

	IF_WAZASICK   CHECK_ATTACK,WAZASICK_DOKUDOKU,ExpertAI_223_ok1		// �ǂ��ǂ�
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_NOROI,ExpertAI_223_ok1			// �̂낢
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_HOROBINOUTA,ExpertAI_223_ok1		// �ق��
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_MEROMERO,ExpertAI_223_ok1		//��������
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_YADORIGI,ExpertAI_223_ok1		// ��ǂ肬
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_AKUBI,ExpertAI_223_ok1			// ������
	
	IF_HP_EQUAL		CHECK_DEFENCE,100,ExpertAI_223_count
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_223_Table,ExpertAI_223_count	//�����A�C�e��
	

ExpertAI_223_ok1:
	IF_RND_UNDER	128,ExpertAI_223_count
	INCDEC	1

ExpertAI_223_count:
	CHECK_MAMORU_COUNT	CHECK_DEFENCE
	IF_EQUAL		0,ExpertAI_223_1
	IF_EQUAL		1,ExpertAI_223_2
	IF_OVER			2,ExpertAI_223_ng

ExpertAI_223_1:
	IF_RND_UNDER	128,ExpertAI_223_end
	INCDEC	1
	JUMP	ExpertAI_223_end

ExpertAI_223_2:
	IF_RND_UNDER	192,ExpertAI_223_end
	INCDEC	1
	JUMP	ExpertAI_223_end

ExpertAI_223_ng:
	INCDEC		-2
ExpertAI_223_end:
	AIEND

ExpertAI_223_Table://�����A�C�e��
	.long	SOUBI_HPKAIHUKUMAITURN
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_224:	// ���΂�ok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_224_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_224_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_224_ng
	
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_224_1
	IF_RND_UNDER	64,ExpertAI_224_1
	INCDEC	1

ExpertAI_224_1:
	IF_RND_UNDER	128,ExpertAI_224_end
	INCDEC	1
	JUMP	ExpertAI_224_end

ExpertAI_224_ng:
	INCDEC	-1
ExpertAI_224_end:
	AIEND


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_225:	// ��������ok
	IF_RND_UNDER	64,ExpertAI_225_end
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_225_ng
	IF_HP_UNDER	CHECK_ATTACK,31,ExpertAI_225_ng
	IF_HP_OVER	CHECK_ATTACK,75,ExpertAI_225_ok
	IF_RND_UNDER	64,ExpertAI_225_end
ExpertAI_225_ok:
	INCDEC	1
	JUMP	ExpertAI_225_end
ExpertAI_225_ng:
	INCDEC	-1
ExpertAI_225_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_226:	// �ڂ���ok
	IF_HP_UNDER	CHECK_ATTACK,51,ExpertAI_226_ng
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_226_ok
	IF_RND_UNDER	128,ExpertAI_226_end

ExpertAI_226_ok:	// �ڂ���
	IF_RND_UNDER	64,ExpertAI_226_end
	INCDEC	1
	JUMP	ExpertAI_226_end

ExpertAI_226_ng:
	INCDEC	-1
ExpertAI_226_end:
	AIEND
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_227:	// ���^���o�[�X�g�F�J�E���^�[�𗬗p
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_227_ng
	IF_WAZASICK   CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_227_ng	// ��������
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_227_ng		// ������
	//���肪��U�Z�������Ă���ꍇ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,185,ExpertAI_227_ng		// ���x���W
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,170,ExpertAI_227_ng		// �������p���`
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,78,ExpertAI_227_ng		// ���Ă݂Ȃ�
	

	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_227_1	

	IF_RND_UNDER	10,ExpertAI_227_1
	INCDEC	-1

ExpertAI_227_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_227_koukou	

	IF_RND_UNDER	100,ExpertAI_227_koukou
	INCDEC	-1


ExpertAI_227_koukou:
	IF_RND_UNDER	192,ExpertAI_227_2
	INCDEC	1


ExpertAI_227_2:
	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_227_3

	IFN_CHOUHATSU	ExpertAI_227_3	

	IF_RND_UNDER	100,ExpertAI_227_3
	INCDEC	1

ExpertAI_227_3:
	IFN_CHOUHATSU	ExpertAI_227_end	

	IF_RND_UNDER	100,ExpertAI_227_end
	INCDEC	1
	JUMP	ExpertAI_227_end


ExpertAI_227_ng:
	INCDEC	-1
ExpertAI_227_end:
	AIEND

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
ExpertAI_228:	// �Ƃ�ڂ�����ok(�ق�)
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_228_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_228_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_228_ng
	
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_228_end			// �����̍T�������Ȃ�
	
	IF_HAVE_BATSUGUN	ExpertAI_228_ng1	// ���Q�������Ă���
	JUMP	ExpertAI_228_1


ExpertAI_228_ng:
	INCDEC	-1
	JUMP	ExpertAI_228_end	

ExpertAI_228_ng1:
	IF_RND_UNDER	64,ExpertAI_228_1
	INCDEC	-2
	
ExpertAI_228_1:
	IF_BENCH_DAMAGE_MAX		LOSS_CALC_OFF,ExpertAI_228_2
	//IF_BENCH_DAMAGE_MAX�͐���ɓ��삵�Ă��Ȃ��B�������̃v���O�����ύX�͑傫���̂ł��̂܂܂�2006.8.4
	IF_RND_UNDER	64,ExpertAI_228_2
	INCDEC	-2
	JUMP	ExpertAI_228_end	

ExpertAI_228_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_228_3
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_228_4
	IF_RND_UNDER	128,ExpertAI_228_5
	JUMP	ExpertAI_228_4

	
ExpertAI_228_3:
	IF_RND_UNDER	64,ExpertAI_228_4
	INCDEC	+1

ExpertAI_228_4:
	IF_RND_UNDER	128,ExpertAI_228_5
	INCDEC	+1
	
ExpertAI_228_5:
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_228_ok
	IF_RND_UNDER	128,ExpertAI_228_end

ExpertAI_228_ok:
	INCDEC	1
ExpertAI_228_end:
	AIEND

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_229:	// �C���t�@�C�g�F�I�[�o�[�q�[�g�𗬗pok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_229_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_229_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_229_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_229_1

	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_229_end
	JUMP		ExpertAI_229_ng	

ExpertAI_229_1:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_229_end

ExpertAI_229_ng:
	INCDEC		-1
ExpertAI_229_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_230:	// �����؂�����ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_230_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_230_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_230_ng
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_230_end
	IF_HP_UNDER	CHECK_ATTACK,30, ExpertAI_230_end
	IF_RND_UNDER	64,ExpertAI_230_end
	INCDEC	1
	AIEND

ExpertAI_230_ng:	
	INCDEC	-1

ExpertAI_230_end:	
	AIEND
	
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_231:	// �_������ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_231_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_231_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_231_ng
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_231_end
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_SAMEHADA, ExpertAI_231_1//���߂͂�

	CHECK_SOUBI_ITEM	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_231_Table,ExpertAI_231_1//�_���[�W��^����؂̎�
	
	IF_RND_UNDER	128,ExpertAI_231_1
	JUMP	ExpertAI_231_end
		

ExpertAI_231_ng:	
	INCDEC	-1
	JUMP	ExpertAI_231_end

ExpertAI_231_1:	
	IF_RND_UNDER	128,ExpertAI_231_end
	INCDEC	1

ExpertAI_231_end:	
	AIEND

ExpertAI_231_Table://�؂̎��i���o�[
	.long	ITEM_ZYAPONOMI,ITEM_RENBUNOMI
	.long	0xffffffff
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_232:	// ����������ok
	IF_RND_UNDER	128,ExpertAI_232_end
	INCDEC	1
ExpertAI_232_end:	// ����������
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_233:	// �Ȃ�����ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_233_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_233_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_233_ng
	
	CHECK_NAGETSUKERU_IRYOKU	CHECK_ATTACK
	IF_UNDER	30,ExpertAI_233_nagenai	//30�����͍U���n�łȂ��̂œ����Ȃ�
	IF_OVER		90,ExpertAI_233_nageru1	//91�ȏ�͒��U���n�Ȃ̂œ�����
	IF_OVER		60,ExpertAI_233_nageru2	//61�ȏ�͂��������U���n�Ȃ̂œ�����
	IF_RND_UNDER	128,ExpertAI_233_end
	INCDEC	-1
	JUMP	ExpertAI_233_end

ExpertAI_233_nagenai:
	INCDEC	-2
	JUMP	ExpertAI_233_end


ExpertAI_233_nageru1:
	CHECK_WAZA_AISYOU	AISYOU_2BAI,ExpertAI_233_nageru1_ok
	CHECK_WAZA_AISYOU	AISYOU_4BAI,ExpertAI_233_nageru1_ok
	IF_RND_UNDER	128,ExpertAI_233_nageru2
	INCDEC	1
	JUMP	ExpertAI_233_nageru2

ExpertAI_233_nageru1_ok:
	INCDEC	4

ExpertAI_233_nageru2:
	IF_RND_UNDER	64,ExpertAI_233_end
	INCDEC	1
	JUMP	ExpertAI_233_end

ExpertAI_233_ng:	
	CHECK_SOUBI_EQUIP	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_233_Table,ExpertAI_233_end
	INCDEC	-1

ExpertAI_233_end:	
	AIEND

ExpertAI_233_Table://�����������Ă���Ԉُ�n�Ȃ�Ό��Z���Ȃ�
	.long	SOUBI_HIRUMASERU				//�Ђ�܂���
	.long	SOUBI_DOKUBARIUP				//�ǂ��ɂ���	
	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU	//�����ǂ��ɂ���	
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO	//�₯�ǂɂɂ���
	.long	SOUBI_PIKATYUUTOKUKOUNIBAI		//�܂Ђɂ���
	.long	0xffffffff


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_234:	// �T�C�R�V�t�gok
	IFN_POKESICK	CHECK_ATTACK,AI_DEC10	// ��������Ԉُ�łȂ��ꍇ���Ȃ�
	IF_RND_UNDER	128,ExpertAI_234_end
	IF_HP_UNDER	CHECK_DEFENCE,30, ExpertAI_234_end
	INCDEC	1

ExpertAI_234_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_235:	// ����ӂ�ok
	
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_235_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_235_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_235_ng
	//IFN_WAZANO	WAZANO_KIRIHUDA,	ExpertAI_235_end//����ӂ��łȂ�:�F�����Ă��Ȃ�2006.7.20
	CHECK_PP_REMAIN
	IF_EQUAL	1,ExpertAI_235_ok	//�o����
	IF_EQUAL	2,ExpertAI_235_2	//���ƂP
	IF_EQUAL	3,ExpertAI_235_3	//���ƂQ

	//���肪�v���b�V���[�������Ă���Əo���₷��
	CHECK_TOKUSEI	CHECK_DEFENCE
	IFN_EQUAL	TOKUSYU_PURESSYAA, ExpertAI_235_1
	IF_RND_UNDER	30,ExpertAI_235_1
	INCDEC	1

//�K����������ڐA--------------------------------------
ExpertAI_235_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,10,ExpertAI_235_2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,2,ExpertAI_235_2
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_235_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_235_3
	JUMP		ExpertAI_235_end

ExpertAI_235_2:
	INCDEC		1
ExpertAI_235_3:
	IF_RND_UNDER	100,ExpertAI_235_end
	INCDEC		1

	JUMP		ExpertAI_235_end
//------------------------------------------------------------

ExpertAI_235_ok:
	//IF_RND_UNDER	25,ExpertAI_235_end
	INCDEC	3
	JUMP	ExpertAI_235_end


ExpertAI_235_ng:	
	INCDEC	-1
ExpertAI_235_end:	

	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_236:	// �����ӂ��ӂ���

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,8,ExpertAI_236_ok		//��߂���ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_236_ok	//������������ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,214,ExpertAI_236_ok	//�͂�˂���ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,157,ExpertAI_236_ok	//���܂�����ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,132,ExpertAI_236_ok	//���̂Ђ���ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,37,ExpertAI_236_ok	//�˂ނ�ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,162,ExpertAI_236_ok	//�݂̂���ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,3,ExpertAI_236_ok		//�����Ƃ�ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,181,ExpertAI_236_ok	//�˂��͂�
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,251,ExpertAI_236_ok	//�A�N�A�����Ook
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,84,ExpertAI_236_ok	//��ǂ肬�̃^�l
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,220,ExpertAI_236_ok	//���₵�̂˂���ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,270,ExpertAI_236_ok	//�݂��Â��̂܂�ok
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_YADORIGI,ExpertAI_236_ok// ������ǂ�ꒆok
//@todo	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_AQUARING,ExpertAI_236_ok// ����A�N�A�����O��ok
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_NEWOHARU,ExpertAI_236_ok//����˂��͂钆ok
	IF_RND_UNDER	96,ExpertAI_236_ok
	JUMP	ExpertAI_236_end

ExpertAI_236_ok:
	IF_RND_UNDER	25,ExpertAI_236_end
	INCDEC	1
ExpertAI_236_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_237:	// ���ڂ�Ƃ�ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_237_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_237_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_237_ng
	IF_HP_UNDER	CHECK_DEFENCE,50, ExpertAI_237_ng
	IF_HP_EQUAL	CHECK_DEFENCE,100, ExpertAI_237_ok1
	IF_HP_OVER	CHECK_DEFENCE,85, ExpertAI_237_ok2
	JUMP	ExpertAI_237_end
	

ExpertAI_237_ok1:	
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_237_ok1_1
	INCDEC	1
ExpertAI_237_ok1_1:	
	INCDEC	1
ExpertAI_237_ok2:	
	IF_RND_UNDER	25,ExpertAI_237_end
	INCDEC	1
	JUMP	ExpertAI_237_end

ExpertAI_237_ng:	
	INCDEC	-1
ExpertAI_237_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_238:	// �p���[�g���b�Nok
	//�g�o���݂ēK�x�ɌJ��o���B�P�x�s���Ƃ��Ȃ��BbasicAI�őΏ��B
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_238_1
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_238_2
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_238_3
	JUMP	AI_DEC2

ExpertAI_238_1:
	IF_RND_UNDER	96,ExpertAI_238_end
	INCDEC	+1
	JUMP	ExpertAI_238_end
	
ExpertAI_238_2:
	IF_RND_UNDER	128,ExpertAI_238_end
	INCDEC	+1
	JUMP	ExpertAI_238_end

ExpertAI_238_3:
	IF_RND_UNDER	164,ExpertAI_238_end
	INCDEC	+1
	JUMP	ExpertAI_238_end

ExpertAI_238_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_239:	// �������F�����Ă闬�p
	IF_RND_UNDER		64,ExpertAI_239_end
	INCDEC		1
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_239_end

	IF_RND_UNDER		128,ExpertAI_239_1
	INCDEC		-1
ExpertAI_239_1:
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_239_end
	INCDEC		-1
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_239_end
	INCDEC		-1
ExpertAI_239_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_240:	// ���܂��Ȃ�ok

	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_240_ng
	//�N���e�B�J���Z
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,43,ExpertAI_240_ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,200,ExpertAI_240_ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,209,ExpertAI_240_ok
	IF_RND_UNDER	64,ExpertAI_240_ok
	JUMP	ExpertAI_240_end

ExpertAI_240_ok:
	INCDEC	1
	JUMP	ExpertAI_240_end

ExpertAI_240_ng:
	INCDEC	-1
ExpertAI_240_end:
	AIEND
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_241:	// �����ǂ�
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_241_ng
	
	IF_LAST_WAZA_DAMAGE_CHECK	CHECK_DEFENCE,LOSS_CALC_OFF,ExpertAI_241_Ok
	JUMP	ExpertAI_241_1

ExpertAI_241_Ok:
	IF_RND_UNDER	32,ExpertAI_241_1
	INCDEC	1	

ExpertAI_241_1:
	
	//���肪�Ō�ɏo�����Z���ω��Z
	CHECK_LAST_WAZA_KIND
	IF_EQUAL	WAZADATA_DMG_NONE,ExpertAI_241_2
	IF_RND_UNDER	128,ExpertAI_241_end
	INCDEC	1	

ExpertAI_241_2:
	IF_RND_UNDER	64,ExpertAI_241_end
	INCDEC	1	
	JUMP	ExpertAI_241_end

ExpertAI_241_ng:
	INCDEC	-2	

ExpertAI_241_end:
	AIEND
	
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
ExpertAI_242:	// �܂˂���
//�܂˂����Z�������̎����Ă���Z�̂Ȃ��ň�ԋ����Z�̏ꍇ�_�������Z����ǉ�
//�����̕��������Ă�����̋Z�����͂łȂ���Ό��ʓI�Ɍ��Z����B

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_242_1
	
	IF_LAST_WAZA_DAMAGE_CHECK	CHECK_DEFENCE,LOSS_CALC_OFF,ExpertAI_242_ok

	CHECK_LAST_WAZA	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_242_Table,ExpertAI_242_1
		
	IF_RND_UNDER	128,ExpertAI_242_3
	INCDEC		2				//�����킴�͕Ԃ�
	JUMP		ExpertAI_242_3

ExpertAI_242_ok:
	IF_RND_UNDER	32,ExpertAI_242_3
	INCDEC		2	
	JUMP		ExpertAI_242_3


ExpertAI_242_1://����̕��������ꍇ�����Z�łȂ���Ό��Z����
	IF_LAST_WAZA_DAMAGE_CHECK	CHECK_DEFENCE,LOSS_CALC_OFF,ExpertAI_242_3
	CHECK_LAST_WAZA	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_242_Table,ExpertAI_242_3

	IF_RND_UNDER	80,ExpertAI_242_3
	INCDEC		-1
ExpertAI_242_3:
	AIEND

ExpertAI_242_Table://�ω����ċ������ȋZ�Q
	.long	79,142,147,95,47,320,325,28,108,92
	.long	12,32,90,329,238,177,109,186,103,178
	.long	184,313,319,86,137,77,247,223,63,245
	.long	168,343,213,207,259,260,271,276,285
	//2006.6.14
	.long	375,384,385,389,391,415,445,464	
	.long	0xffffffff


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_243:	// �p���[�X���b�vok
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_ATK//�i�Ώۑ���iDEFENCE�j�[ATTACK�j
	IF_OVER			3,ExpertAI_243_ok1	//4�ȏ�
	IF_OVER			1,ExpertAI_243_ok2	//2�ȏ�
	IF_OVER			0,ExpertAI_243_ok3	//1�ȏ�
	IF_EQUAL		0,ExpertAI_243_ok4	//0
	JUMP	ExpertAI_243_end

ExpertAI_243_ok1:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_05	//4�ȏ�
	IF_OVER			1,ExpertAI_243_04	//2�ȏ�
	IF_EQUAL		0,ExpertAI_243_03	//0
	JUMP	ExpertAI_243_end
	
ExpertAI_243_ok2:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_04	//4�ȏ�
	IF_OVER			1,ExpertAI_243_03	//2�ȏ�
	IF_EQUAL		0,ExpertAI_243_02	//0
	JUMP	ExpertAI_243_end

ExpertAI_243_ok3:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_03	//4�ȏ�
	IF_OVER			1,ExpertAI_243_02	//2�ȏ�
	IF_EQUAL		0,ExpertAI_243_01	//0
	JUMP	ExpertAI_243_end

ExpertAI_243_ok4:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_03	//4�ȏ�
	IF_OVER			1,ExpertAI_243_02	//2�ȏ�
	IF_OVER			0,ExpertAI_243_01	//0�ȏ�
	JUMP	ExpertAI_243_end

ExpertAI_243_05:
	IF_RND_UNDER	128,ExpertAI_243_04
	INCDEC	+5
	JUMP	ExpertAI_243_end

ExpertAI_243_04:
	IF_RND_UNDER	128,ExpertAI_243_03
	INCDEC	+4
	JUMP	ExpertAI_243_end
	
ExpertAI_243_03:
	IF_RND_UNDER	128,ExpertAI_243_02
	INCDEC	+3
	JUMP	ExpertAI_243_end
	
ExpertAI_243_02:
	IF_RND_UNDER	128,ExpertAI_243_01
	INCDEC	+2
	JUMP	ExpertAI_243_end
	
ExpertAI_243_01:
	IF_RND_UNDER	128,ExpertAI_243_end
	INCDEC	+1
	JUMP	ExpertAI_243_end

ExpertAI_243_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_244:	// �K�[�h�X���b�vok
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_DEF//�i�Ώۑ���iDEFENCE�j�[ATTACK�j
	IF_OVER			3,ExpertAI_244_ok1	//4�ȏ�
	IF_OVER			1,ExpertAI_244_ok2	//2�ȏ�
	IF_OVER			0,ExpertAI_244_ok3	//1�ȏ�
	IF_EQUAL		0,ExpertAI_244_ok4	//0
	JUMP	ExpertAI_244_end

ExpertAI_244_ok1:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_05	//4�ȏ�
	IF_OVER			1,ExpertAI_244_04	//2�ȏ�
	IF_EQUAL		0,ExpertAI_244_03	//0
	JUMP	ExpertAI_244_end
	
ExpertAI_244_ok2:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_04	//4�ȏ�
	IF_OVER			1,ExpertAI_244_03	//2�ȏ�
	IF_EQUAL		0,ExpertAI_244_02	//0
	JUMP	ExpertAI_244_end

ExpertAI_244_ok3:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_03	//4�ȏ�
	IF_OVER			1,ExpertAI_244_02	//2�ȏ�
	IF_EQUAL		0,ExpertAI_244_01	//0
	JUMP	ExpertAI_244_end

ExpertAI_244_ok4:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_03	//4�ȏ�
	IF_OVER			1,ExpertAI_244_02	//2�ȏ�
	IF_OVER			0,ExpertAI_244_01	//0�ȏ�
	JUMP	ExpertAI_244_end

ExpertAI_244_05:
	IF_RND_UNDER	128,ExpertAI_244_04
	INCDEC	+5
	JUMP	ExpertAI_244_end

ExpertAI_244_04:
	IF_RND_UNDER	128,ExpertAI_244_03
	INCDEC	+4
	JUMP	ExpertAI_244_end
	
ExpertAI_244_03:
	IF_RND_UNDER	128,ExpertAI_244_02
	INCDEC	+3
	JUMP	ExpertAI_244_end
	
ExpertAI_244_02:
	IF_RND_UNDER	128,ExpertAI_244_01
	INCDEC	+2
	JUMP	ExpertAI_244_end
	
ExpertAI_244_01:
	IF_RND_UNDER	128,ExpertAI_244_end
	INCDEC	+1
	JUMP	ExpertAI_244_end

ExpertAI_244_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_245:	// ��������ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_245_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_245_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_245_end
	CHECK_STATUS_UP	CHECK_DEFENCE
	IF_OVER		6,ExpertAI_245_ok1
	IF_OVER		5,ExpertAI_245_ok2
	IF_OVER		4,ExpertAI_245_ok3
	IF_OVER		3,ExpertAI_245_ok4
	IF_OVER		2,ExpertAI_245_ok4
	JUMP	ExpertAI_245_end

ExpertAI_245_ok1:
	IF_RND_UNDER	128,ExpertAI_245_ok2
	INCDEC	+4

ExpertAI_245_ok2:
	IF_RND_UNDER	128,ExpertAI_245_ok3
	INCDEC	+3

ExpertAI_245_ok3:
	IF_RND_UNDER	128,ExpertAI_245_ok4
	INCDEC	+2
	
ExpertAI_245_ok4:
	IF_RND_UNDER	128,ExpertAI_245_end
	INCDEC	+1

ExpertAI_245_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_246:	// �Ƃ��Ă���ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_246_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_246_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_246_ng
	IF_TOTTEOKI	CHECK_ATTACK,ExpertAI_246_ok// �Ƃ��Ă����o����
	JUMP	ExpertAI_246_end

ExpertAI_246_ng:
	INCDEC	-1
	JUMP	ExpertAI_246_end
ExpertAI_246_ok:
	INCDEC	1
ExpertAI_246_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_247:	// �Ȃ�݂̃^�l
	IFN_HAVE_WAZA	CHECK_DEFENCE, WAZANO_NEMURU, ExpertAI_247_1
	INCDEC	+1

ExpertAI_247_1:
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_247_2
	IF_RND_UNDER	128,ExpertAI_247_2
	INCDEC	+1
	
ExpertAI_247_2:
	IF_RND_UNDER	64,ExpertAI_247_end
	INCDEC	+1
	JUMP	ExpertAI_247_end

ExpertAI_247_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_248:	// �ӂ�����ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_248_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_248_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_248_ng

	IF_RND_UNDER	64,ExpertAI_248_end
	INCDEC	1
	JUMP	ExpertAI_248_end

ExpertAI_248_ng:
	INCDEC	-1
ExpertAI_248_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_249:	// �ǂ��т�ok
	IF_RND_UNDER	128,ExpertAI_249_end
	INCDEC	+1
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HOERU, ExpertAI_249_ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUKITOBASI, ExpertAI_249_ok
	JUMP	ExpertAI_249_end
	
ExpertAI_249_ok:
	IF_RND_UNDER	64,ExpertAI_249_end
	INCDEC	+1

ExpertAI_249_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_250:	// �n�[�g�X���b�v�F�������񂶂��ؗp
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,7,ExpertAI_250_1
	IF_CONTFLG	  CHECK_DEFENCE,BPP_CONTFLG_KIAIDAME,ExpertAI_250_1
	JUMP	ExpertAI_250_ng

ExpertAI_250_1:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,7,ExpertAI_250_ok1
	IFN_CONTFLG	  CHECK_ATTACK,BPP_CONTFLG_KIAIDAME,ExpertAI_250_ok2

	IF_RND_UNDER	50,ExpertAI_250_end
	JUMP	ExpertAI_250_ng

ExpertAI_250_ok1:
	INCDEC		1
ExpertAI_250_ok2:
	INCDEC		1
	AIEND
ExpertAI_250_ng:
	INCDEC		-2
ExpertAI_250_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_251:	// �A�N�A�����Ook
	IF_HP_UNDER	CHECK_ATTACK,30,ExpertAI_251_end
	IF_RND_UNDER	128,ExpertAI_251_end
	INCDEC	+1
ExpertAI_251_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_252:	// �ł񂶂ӂ䂤ok
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_252_end

	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_ZISIN, ExpertAI_252_ok1
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_DAITINOTIKARA, ExpertAI_252_ok1
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_ZIWARE, ExpertAI_252_ok1
	JUMP	ExpertAI_252_1

ExpertAI_252_ok1:
	INCDEC	1
ExpertAI_252_1:
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_JIMEN,ExpertAI_252_ok2	// ���߂�^�C�v
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_JIMEN,ExpertAI_252_ok2	// ���߂�^�C�v
	IF_RND_UNDER	128,ExpertAI_252_end

ExpertAI_252_ok2:
	INCDEC	1

ExpertAI_252_end:	// �ł񂶂ӂ䂤
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_254:	// ��邠����
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//ExpertAI_255:	// �_�C�r���O�F������ƂԂƈꏏ
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//ExpertAI_256:	// ���Ȃ��ق�F������ƂԂƈꏏ
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_257:	// �Ȃ݂̂�
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_258:	// ����΂炢
	//�P�j�ǂ܂��т����Ȃ��ꍇ�A��𗦃`�F�b�N�̂�
	//�Q�j�ǂ݂̂���ꍇ�A�ǂ������₷������
	//�R�j�܂��т��݂̂̏ꍇ�A���Ȃ�
	//�S�j�ǂ�����A�܂��т�������ꍇ�A�ǂ��������ǂ����̃`�F�b�N��ɉ�𗦃`�F�b�N���s��
	
	//�ǃ`�F�b�N
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_HIKARINOKABE,ExpertAI_258_ok1	// �Ђ���̂���
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_REFRECTOR,ExpertAI_258_ok1	// ���t���N�^�[

	//�܂��т��`�F�b�N
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,ExpertAI_258_ng1	// �܂��т���
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,ExpertAI_258_ng1 // �X�e���X���b�N��
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,ExpertAI_258_ng1 // �ǂ��т���
	JUMP	ExpertAI_258_avoid

ExpertAI_258_ok1:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_258_ok1_1
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_258_1			// �����̍T�������Ȃ�

ExpertAI_258_ok1_1:
	INCDEC		1
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,ExpertAI_258_end			// �G�̍T�������Ȃ�
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,ExpertAI_258_ng2	// �܂��т���
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,ExpertAI_258_ng2 // �X�e���X���b�N��
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,ExpertAI_258_ng2 // �ǂ��т���
	JUMP	ExpertAI_258_avoid


ExpertAI_258_ng1://�ǂ��Ȃ���Ԃł܂��т��n������ꍇ
	INCDEC	-2
	JUMP	ExpertAI_258_avoid

ExpertAI_258_ng2://�ǂ������Ԃł܂��т��n������ꍇ
	IF_RND_UNDER	128,ExpertAI_258_avoid
	INCDEC	-1
	JUMP	ExpertAI_258_avoid


ExpertAI_258_avoid:

	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_258_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,3,ExpertAI_258_2

ExpertAI_258_1:
	IF_RND_UNDER	50,ExpertAI_258_2
	INCDEC		-2
ExpertAI_258_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_258_end

	INCDEC		-2
ExpertAI_258_end:
	AIEND


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ExpertAI_259:	// �g���b�N���[��ok
	
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_DOUBLE,ExpertAI_259_end	//2VS2�Ȃ�X���[
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_259_1
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_259_end	// �����̍T�������Ȃ�

ExpertAI_259_1:	
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_259_ok
	INCDEC		-1	//basicAI�ł��ł�-10���Ă���
	JUMP		ExpertAI_259_end

ExpertAI_259_ok:
	IF_RND_UNDER	64,ExpertAI_259_end
	INCDEC		3

ExpertAI_259_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ExpertAI_260:	//�ӂԂ�ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_260_1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_260_1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_260_1
	CHECK_WEATHER
	IFN_EQUAL	WEATHER_ARARE,ExpertAI_260_end
	INCDEC		1
	JUMP		ExpertAI_260_end
	
ExpertAI_260_1:
	IF_RND_UNDER	50,ExpertAI_260_end
	INCDEC		-3

ExpertAI_260_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//ExpertAI_263:	// �Ƃт͂˂�F������ƂԂƈꏏ
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_265:	// �䂤�킭ok

	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEPOW,6,ExpertAI_265_2

	INCDEC		-1
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_265_1

	INCDEC		-1
ExpertAI_265_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,3,ExpertAI_265_2
	IF_RND_UNDER	50,ExpertAI_265_2
	INCDEC		-2
ExpertAI_265_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_265_3
	INCDEC		-2

ExpertAI_265_3:
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_265_end//�����łȂ�
	IF_RND_UNDER	64,ExpertAI_265_end
	INCDEC		-1

ExpertAI_265_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_266:	// �X�e���X���b�Nok
	IF_RND_UNDER	128,ExpertAI_266_end
	INCDEC	+1
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HOERU, ExpertAI_266_ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUKITOBASI, ExpertAI_266_ok
	JUMP	ExpertAI_266_end
	
ExpertAI_266_ok:
	IF_RND_UNDER	64,ExpertAI_266_end
	INCDEC	+1

ExpertAI_266_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_267:	// ������ׂ�
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_268:	// ���΂��̂Ԃ�
//�_���[�W�v�Z�ɓ��ꂽ
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_48:	// �Ƃ�����ok
ExpertAI_198:	// ���Ă݃^�b�N��ok
ExpertAI_253:	// �t���A�h���C�uok
ExpertAI_262:	// �{���e�b�J�[ok
ExpertAI_269:	// ����͂̂���ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_269_end	
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_269_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_269_end

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_ISIATAMA, ExpertAI_269_ok
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_269_ok
	JUMP	ExpertAI_269_end
ExpertAI_269_ok:
	INCDEC	+1
ExpertAI_269_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_220:	// ���₵�̂˂����F�����𗬗p
ExpertAI_270:	// �݂��Â��̂܂��F�������p

ExpertAI_270_1:
	IF_HP_UNDER	CHECK_ATTACK,80,ExpertAI_270_2
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_270_2

	IF_RND_UNDER	192,ExpertAI_270_5
	JUMP		AI_DEC5

ExpertAI_270_2:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_270_4
	IF_RND_UNDER	192,ExpertAI_270_3
	INCDEC		+1

	IF_HAVE_BATSUGUN	ExpertAI_270_hikae_batugun	//���Q�������Ă���
	IF_RND_UNDER	192,ExpertAI_270_hikae_batugun
	INCDEC		+1
	
ExpertAI_270_hikae_batugun:	

	IF_BENCH_DAMAGE_MAX		LOSS_CALC_OFF,ExpertAI_270_hikae_nostrong	//�T���̕��������Z�������Ă���
	JUMP	ExpertAI_270_3
	
ExpertAI_270_hikae_nostrong:	
	IF_RND_UNDER	128,ExpertAI_270_3
	INCDEC	+1

ExpertAI_270_3:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_270_5
	IF_RND_UNDER	128,ExpertAI_270_5
	INCDEC		+1
	JUMP		ExpertAI_270_5

ExpertAI_270_4:
	IF_RND_UNDER	50,ExpertAI_270_5
	INCDEC		-1
ExpertAI_270_5:
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

