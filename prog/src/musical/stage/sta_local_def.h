//======================================================================
/**
 * @file	sta_local_def.h
 * @brief	�X�e�[�W�p�@��`
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================
#ifndef STA_LOCAL_DEF_H__
#define STA_LOCAL_DEF_H__


//BBD�p���W�ϊ�(�J�����̕�����v�Z
#define ACT_POS_X(val)	FX32_CONST((val)/16.0f)
#define ACT_POS_Y(val)	FX32_CONST((192.0f-(val))/16.0f)
#define ACT_POS_X_FX(val)	((val)/16)
#define ACT_POS_Y_FX(val)	(FX32_CONST(192.0f)-(val))/16

//BBD�p�T�C�Y�ϊ�
//BBD��32��P�ʂƂ��ăT�C�Y�𑪂�B�������w�i�\���̂��߂ɑS�̂��S�΂����Ă���̂�
#define ACT_BBD_SIZE(val) FX16_CONST( val/32.0f/4.0f )

//�I�u�W�F�N�g�֌W
#define ACT_OBJECT_MAX (5)

//�G�t�F�N�g�֌W
#define ACT_EFFECT_MAX (3)

//�X�|�b�g���C�g�֌W
#define ACT_LIGHT_MAX	(4)

//3D�֌W(�|���S��ID
#define ACT_POLYID_LIGHT (4)
#define ACT_POLYID_SHADOW (3)
//�G�t�F�N�g��5�`63

//���֌W
#define ACT_CURTAIN_SCROLL_SPEED (2)
#define ACT_CURTAIN_SCROLL_MIN (0)
#define ACT_CURTAIN_SCROLL_MAX (224)

#endif STA_LOCAL_DEF_H__
