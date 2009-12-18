#ifndef DS_WEB_INTERFACE_DEFS_INCLUDE_H__
#define DS_WEB_INTERFACE_DEFS_INCLUDE_H__

/*
 * DS ���󂯎�� HTTP ���X�|���X�iHTTP BODY ���j�̋��ʃw�b�_
 *
 * �@�\�ɂ���ẮA�w�b�_�ȊO�̏�񂪑��݂��邪�A���̏���
 * ���ʃw�b�_���ɘA�����郁�����Ɋi�[����Ă��邽�߁A���L�̂悤��
 * ���@�ɂ��擾����B
 *
 * ��1�j
 *   struct gs_response* resp = (struct gs_response*)HTTP_BOD_ADDR;
 *   struct app_func1_info* info = (struct app_func1_info*)&resp[1];
 *
 * ��2�j
 *   struct gs_response* resp = (struct gs_response*)HTTP_BOD_ADDR;
 *   struct app_func1_info* info = (struct app_func1_info*)&resp->ext_info[1];
 *
 */
/*
 * [09-12-06]
 *		ret_cd �ɐݒ肷��l�� enum �ɕύX�������߁A�^�� "unsigned short" ���� "long"
 *		�ɕύX�����B
 *		����ɂƂ��Ȃ��A�����o�̈ʒu�𒲐��B
 */
typedef struct
{
	long ret_cd;				/* �������ʃR�[�h			=0:����, <>0:�ȏ�				*/
	unsigned long body_size;	/* �f�[�^���o�C�g�T�C�Y										*/
	unsigned long start_sec;	/* �T�[�o�����J�n����										*/
	char yobi1[8];				/* �\��														*/
	unsigned short desc_cd;		/* �ڍ׃G���[�R�[�h			ret_cd=0 �̏ꍇ�ɐݒ肳���		*/
	char desc_msg[80];			/* �ڍ׃G���[����			PHP �T�[�o�v���O�����������	*/
	char yobi2[25];				/* �\��														*/
	char ext_info[1];			/* �ʒu�m�F�p				��� 'Z'						*/
}  gs_response;

/*
 * DS ���󂯎�� HTTP ���X�|���X�iHTTP BODY ���j�̋@�\�ʃf�[�^��
 *
 * ��L�� gs_response �\���̂ɘA�����郁�����ɔz�u����邪�A���N�G�X�g���s����
 * �@�\���ƂɈقȂ�f�[�^�����ݒ肳���B
 * �܂��A�@�\�ɂ���Ă̓f�[�^���̑��݂��Ȃ��ꍇ������A�ǂ���̏ꍇ��
 * ���ʃw�b�_���� body_size �Ƀf�[�^���ɐݒ肳�ꂽ�o�C�g�T�C�Y���ݒ肳��Ă���B
 *
 */

/*
 * GET �C���^�[�t�F�[�X�e�X�g�p�\����
 *
 * URI: /gs?p=test.get&gsid=1234
 * ���N�G�X�g���\�b�h: GET
 *
 * ����: �N���C�A���g(DS)�ƃT�[�o�̃f�[�^�ʐM�p�̎������s�����߂̍\���́B
 *       �����p�ł��邽�߁A�T�[�o����ԋp�����f�[�^�͌Œ�l�ł���B
 *
 */
typedef struct 
{
	unsigned long u32_val1;		/* =4294967294 (-2)	*/
	unsigned short u16_val1;	/* =65534 (-2)		*/
	unsigned char u8_val1;		/* =254 (-2)		*/

	char s8_val1;				/* =-2				*/
	long s32_val1;				/* =-2				*/
	short s16_val1;				/* =-2				*/

	char s8_val2;				/* =66 ('B')		*/
	char str_val1[9];			/* ="abcd0123"		*/
}respbody_test_get;

/*
 * POST �C���^�[�t�F�[�X�e�X�g�p�\����
 *
 * URI: /gs?p=test.post&gsid=1234
 * ���N�G�X�g���\�b�h: POST
 *
 * ����: �N���C�A���g(DS)�ƃT�[�o�̃f�[�^�ʐM�p�̎������s�����߂̍\���́B
 *       ���N�G�X�g�ƃ��X�|���X�͓����`�ł���A���X�|���X�Ƃ��ĕԋp�����
 *       �f�[�^�́A���N�G�X�g���s��ꂽ�l���ȉ��̋K���ŕҏW�������̂ł���B
 *
 *       �ҏW�K��:
 *           u32_val1, u16_val1, u8_val1 ... ���̒l - 1
 *           s8_val1, s32_val1, s16_val1 ... ���̒l * 2
 *           s8_val2                     ... �������ɕϊ�
 *           str_val1                    ... ������𔽓]
 *
 */
typedef struct 
{
	unsigned long u32_val1;
	unsigned short u16_val1;
	unsigned char u8_val1;

	char s8_val1;
	long s32_val1;
	short s16_val1;

	char s8_val2;
	char str_val1[9];
}iobody_test_post;

#endif
