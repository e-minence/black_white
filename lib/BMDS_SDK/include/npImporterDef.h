
#ifndef NPIMPORTERDEF_H
#define NPIMPORTERDEF_H


/*
	file data contents �̕\�����[�h
	0	�\�����Ȃ�
	1	�\������
*/
#define _NP_DATA_MASSAGE 0
#ifdef _NP_DATA_MASSAGE
	#define dfprintf( arg ) ( fprintf arg )
#else
	#define dfprintf( arg )
#endif	/* _NP_DATA_MASSAGE */

// �t�@�C�����̍ő咷
#define DEFNP_BPH_FILE_NAME_LENGTH	256
// �e�N�X�`���̍ő喇��
#define	NP_BPC_TEXTURE_MAX			32

#define NP_BPC_KEYFRAME_MAX			32

#endif //NPIMPORTERDEF_H

