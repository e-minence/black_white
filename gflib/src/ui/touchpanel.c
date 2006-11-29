//****************************************************************************
/**
 *@file		touchpanel.c
 *@brief	��`�ƃ^�b�`�p�l���V�X�e�����Ƃ̓����蔻�菈��
 *@author	taya  >  katsumi ohno
 *@date		2005.07.29
 */
//****************************************************************************

#include "gflib.h"

#include "ui.h"
#include "ui_def.h"


//------------------------------------------------------------------
/**
 * @struct	_UI_TPSYS
 * @brief �^�b�`�p�l���̏��̕ێ�
  */
//------------------------------------------------------------------
struct _UI_TPSYS {
	u16		tp_x;			///< �^�b�`�p�l��X���W
	u16		tp_y;			///< �^�b�`�p�l��Y���W
	u16		tp_trg;			///< �^�b�`�p�l���ڐG����g���K
	u16		tp_cont;		///< �^�b�`�p�l���ڐG������
    u8 tp_auto_samp;        ///< AUTO�T���v�����O���s�����ǂ���
};


//==============================================================
// Prototype
//==============================================================
static BOOL _circle_hitcheck( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );
static BOOL _rect_hitcheck( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );
static int _tblHitCheck( const GFL_UI_TP_HITTBL *tbl, const u16 x, const u16 y );


//==============================================================================
/**
 * @brief  �^�b�`�p�l��������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  UI_TPSYS  �^�b�`�p�l���V�X�e�����[�N
 */
//==============================================================================

UI_TPSYS* GFL_UI_TP_sysInit(const int heapID)
{
	TPCalibrateParam calibrate;
    UI_TPSYS* pTP = GFL_HEAP_AllocMemory(heapID, sizeof(UI_TPSYS));

    MI_CpuClear8(pTP, sizeof(UI_TPSYS));
	// �^�b�`�p�l���̏������ƃL�����u���[�V�������Z�b�g
	TP_Init();

	// �}�V���̃L�����u���[�V�����l���擾
	if( TP_GetUserInfo( &calibrate ) == TRUE ){
		// �L�����u���[�V�����l�̐ݒ�
		TP_SetCalibrateParam( &calibrate );
		OS_Printf("Get Calibration Parameter from NVRAM\n");
	}
	else{
		// �擾�Ɏ��s�����̂Ńf�t�H���g�̃L�����u���[�V�����̐ݒ�
		calibrate.x0 = 0x02ae;
		calibrate.y0 = 0x058c;
		calibrate.xDotSize = 0x0e25;
		calibrate.yDotSize = 0x1208;
		TP_SetCalibrateParam( &calibrate );
		OS_Printf( "Warrning : TauchPanelInit( not found valid calibration data )\n" );
	}
    return pTP;
}

//==============================================================================
/**
 * @brief �^�b�`�p�l���ǂݎ�菈��
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_TP_sysMain(UISYS* pUI)
{
	TPData	tpTemp;
	TPData	tpDisp;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

	// �ӂ����܂��Ă���ꍇ�͑S�Ă̓��͂��Ȃ��ɂ���
	if(PAD_DetectFold()){
        MI_CpuClear8(pTP, sizeof(UI_TPSYS));
    }

	// �^�b�`�p�l���f�[�^���擾
	if(pTP->tp_auto_samp == 0){
//		while( TP_RequestRawSampling( &tpTemp ) != 0 ){};	//�T���v�����O�ɐ�������܂ő҂�
	}else{
		TP_GetLatestRawPointInAuto( &tpTemp );	// �I�[�g�T���v�����O���̃f�[�^���擾
	}

	TP_GetCalibratedPoint( &tpDisp, &tpTemp );	// ���W����ʍ��W�i�O�`�Q�T�T�j�ɂ���

	
	if( tpDisp.validity == TP_VALIDITY_VALID  ){		// ���W�̗L�������`�F�b�N
		// �^�b�`�p�l�����W�L��
		pTP->tp_x = tpDisp.x;
		pTP->tp_y = tpDisp.y;
	}else{
		// �^�b�`�p�l�����W����
		// 1�V���N�O�̍��W���i�[����Ă���Ƃ��̂ݍ��W�����̂܂܎󂯌p��
		if( pTP->tp_cont ){
			switch(tpDisp.validity){
			case TP_VALIDITY_INVALID_X:
				pTP->tp_y = tpDisp.y;
				break;
			case TP_VALIDITY_INVALID_Y:
				pTP->tp_x = tpDisp.x;
				break;
			case TP_VALIDITY_INVALID_XY:
				break;
			default:	// ����
				break;
			}
		}else{
			// �g���K�̃^�C�~���O�Ȃ�A
			// �^�b�`�p�l���������Ă��Ȃ����Ƃɂ���
			tpDisp.touch = 0;
		}
	}
	pTP->tp_trg	= (u16)(tpDisp.touch & (tpDisp.touch ^ pTP->tp_cont));	// �g���K ����
	pTP->tp_cont	= tpDisp.touch;										// �x�^ ����
}


//==============================================================================
/**
 * @brief �^�b�`�p�l���I������
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_TP_sysEnd(UISYS* pUI)
{
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
    GFL_HEAP_FreeMemory(pTP);
}

//------------------------------------------------------------------
/**
 * @brief �~�`�Ƃ��Ă̓����蔻��i�P���j
 * @param[in]   tbl		�����蔻��e�[�u��
 * @param[in]   x		X���W
 * @param[in]   y		Y���W
 * @retval  TRUE	������
 * @retval  FALSE	�������Ă��Ȃ�
 */
//------------------------------------------------------------------
static BOOL _circle_hitcheck( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y )
{
	x = (tbl->circle.x - x) * (tbl->circle.x - x);
	y = (tbl->circle.y - y) * (tbl->circle.y - y);

	if( x + y < (tbl->circle.r * tbl->circle.r) )
	{
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief ��`�Ƃ��Ă̓����蔻��i�P���j
 * @param[in]   tbl		�����蔻��e�[�u��
 * @param[in]   x		�w���W
 * @param[in]   y		�x���W
 * @retval  TRUE	������
 * @retval  FALSE	�������Ă��Ȃ�
 */
//------------------------------------------------------------------
static BOOL _rect_hitcheck( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y )
{
	if( ((u32)( x - tbl->rect.left) < (u32)(tbl->rect.right - tbl->rect.left))
	&	((u32)( y - tbl->rect.top) < (u32)(tbl->rect.bottom - tbl->rect.top))
	){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief ��`�A�~�̂����蔻����e�[�u�����s��
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   x		�w���W
 * @param[in]   y		�x���W
 * @return  �e�[�u��index  �Ȃ����TP_HIT_NONE
 */
//------------------------------------------------------------------
static int _tblHitCheck( const GFL_UI_TP_HITTBL *tbl, const u16 x, const u16 y )
{
    int i;

    for(i = 0; i < tbl[i].circle.code != GFL_UI_TP_HIT_END; i++)
    {
        if( tbl[i].circle.code == GFL_UI_TP_USE_CIRCLE )
        {
            if( _circle_hitcheck( &tbl[i], x, y ) ){
                return i;
            }
        }
        else
        {
            if( _rect_hitcheck( &tbl[i], x, y ) ){
                return i;
            }
        }
    }
    return GFL_UI_TP_HIT_NONE;
}

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�x�^���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @return  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
int GFL_UI_TouchPanelHitCont( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
    
	if( pTP->tp_cont ){
		return _tblHitCheck(tbl, pTP->tp_x, pTP->tp_y);
	}
	return GFL_UI_TP_HIT_NONE;
}
//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
int GFL_UI_TouchPanelHitTrg( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if( pTP->tp_trg ){
		return _tblHitCheck(tbl, pTP->tp_x, pTP->tp_y);
	}
	return GFL_UI_TP_HIT_NONE;
}

//------------------------------------------------------------------
/**
 * @brief  �^�b�`�p�l���ɐG��Ă��邩
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
BOOL GFL_UI_TouchPanelGetCont( const UISYS* pUI )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
    return pTP->tp_cont;
}
//------------------------------------------------------------------
/**
 * @fn �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
BOOL GFL_UI_TouchPanelGetTrg( const UISYS* pUI )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	return pTP->tp_trg;
}

//------------------------------------------------------------------
/**
 * @fn �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�x�^���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
BOOL GFL_UI_TouchPanelGetPointCont( const UISYS* pUI, u32* x, u32* y )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if( pTP->tp_cont )
	{
		*x = pTP->tp_x;
		*y = pTP->tp_y;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------
/**
 * @fn GFL_UI_TouchPanelGetPointTrg
 * @brief   �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
BOOL GFL_UI_TouchPanelGetPointTrg( const UISYS* pUI, u32* x, u32* y )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if( pTP->tp_trg )
	{
		*x = pTP->tp_x;
		*y = pTP->tp_y;
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�w�肵�����W�ŁA�����蔻����s���܂��B	����
 *	@param	tbl		�����蔻��e�[�u���i�z��j
 *	@param	x		���肘���W
 *	@param	y		���肙���W
 *	@retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
int GFL_UI_TouchPanelHitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y )
{
    return _tblHitCheck(tbl, x, y);
}

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O���s���Ă��邩�ǂ����𓾂�
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @retval  TRUE  �I�[�g�T���v�����O�ł���
 * @retval  FALSE  ���Ă��Ȃ�
 */
//==============================================================================
int GFL_UI_TPGetAutoSamplingFlg(const UISYS* pUI)
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	return pTP->tp_auto_samp;
}

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O�ݒ�
 * @param[out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]    bAuto   �I�[�g�T���v�����O����Ȃ�TRUE
 * @return  none
 */
//==============================================================================
void GFL_UI_TPSetAutoSamplingFlg(UISYS* pUI, const BOOL bAuto)
{
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	pTP->tp_auto_samp = bAuto;
}





