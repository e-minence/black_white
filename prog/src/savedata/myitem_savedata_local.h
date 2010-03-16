//=============================================================================
/**
 * @file	  myitem_savedata_local.h
 * @brief	  �莝���A�C�e���\���̂̒�`�w�b�_
 * @author	tamada
 * @author	hiroyuki nakamura
 * @author	ohno
 * @date	  2005.10.13
 */
//=============================================================================

//------------------------------------------------------------------
/**
 * @brief	�莝���A�C�e���\���̂̒�`
 */
//------------------------------------------------------------------
struct _MYITEM {
	ITEM_ST MyNormalItem[ BAG_NORMAL_ITEM_MAX ];	// �莝���̕��ʂ̓���
	ITEM_ST MyEventItem[ BAG_EVENT_ITEM_MAX ];		// �莝���̑�؂ȓ���
	ITEM_ST MySkillItem[ BAG_WAZA_ITEM_MAX ];		// �莝���̋Z�}�V��
	ITEM_ST MyDrugItem[ BAG_DRUG_ITEM_MAX ];		// �莝���̖�
	ITEM_ST MyNutsItem[ BAG_NUTS_ITEM_MAX ];		// �莝���̖؂̎�
	u32	cnv_button[DUMMY_SHORTCUT_MAX];									// �֗��{�^�� @@OO�폜�\��  �֗��E�C���h�E�͓����Ǘ������
};

