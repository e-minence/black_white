//===================================================================
/**
 * @file	main.h
 * @brief	�A�v���P�[�V�����@�T���v���X�P���g��
 * @author	GAME FREAK Inc.
 * @date	06.11.28
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================

//�e�q�[�v�p�錾
enum{
	GFL_HEAPID_APP = GFL_HEAPID_SYSTEM + 1,
	GFL_HEAPID_MAX
};

//�q�q�[�v�p�錾
enum{
	HEAPID_BG=GFL_HEAPID_MAX,

	HEAPID_TITLE,
	HEAPID_ARIIZUMI_DEBUG,
    HEAPID_NETWORK,
    HEAPID_WIFI,

	HEAPID_CHILD_MAX,
};

#define	HEAPSIZE_SYSTEM	(0x008000)
#define	HEAPSIZE_APP	(0x280000)

