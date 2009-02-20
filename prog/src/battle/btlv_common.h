//=============================================================================================
/**
 * @file	btlv_common.h
 * @brief	�|�P����WB �o�g�� �`�揈�����L�w�b�_
 * @author	taya
 *
 * @date	2008.11.18	�쐬
 */
//=============================================================================================
#ifndef __BTLV_COMMON_H__
#define __BTLV_COMMON_H__

/**
 *	��ʃT�C�Y�^���C�A�E�g�֘A�萔
 */
enum {
	BTLV_LCD_HEIGHT = 192,
	BTLV_LCD_WIDTH  = 256,
};



/**
 *	�`�揈���p�^�X�N�v���C�I���e�B
 */
typedef enum {

	BTLV_TASKPRI_MAIN_WINDOW = 0,
	BTLV_TASKPRI_DAMAGE_EFFECT,


}BtlvTaskPri;


#endif
