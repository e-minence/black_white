/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N( ��ʃt�F�[�h )
 * @file   field_task_fade.h
 * @author obata
 * @date   2009.11.20
 */
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h�^�X�N���쐬����( ��ʃt�F�[�h���N�G�X�g���� )
 *
 * @param fieldmap  ����Ώۃt�B�[���h�}�b�v
 * @param mode	    �t�F�[�h���[�h�iGFL_FADE_MASTER_BRIGHT_BLACKOUT/_WHITEOUT)
 * @param	start_evy	�X�^�[�g�P�x�i0�`16�j
 * @param	end_evy		�G���h�P�x�i0�`16�j
 * @param	wait		  �t�F�[�h�X�s�[�h
 *
 * @return �쐬�����^�X�N
 */
//-----------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_Fade( FIELDMAP_WORK* fieldmap, 
                             int mode, int start_evy, int end_evy, int wait );
