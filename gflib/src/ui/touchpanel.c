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
	TPData* pTouchPanelBuff;					// �o�b�t�@�|�C���^�i�[�p
	u32		TouchPanelBuffSize;						// �o�b�t�@�T�C�Y�i�[�p
	u32		TouchPanelSync;							// �P�t���[���ɉ���T���v�����O���邩
	TPData	TouchPanelOneSync[ TP_ONE_SYNC_BUFF ];	// �P�t���[���ɃT���v�����O���ꂽ�f�[�^�i�[�p
	u32		TouchPanelNowBuffPoint;					// ���o�b�t�@�Ƀf�[�^�����Ă���Ƃ���
	u16		TouchPanelSampFlag;						// �T���v�����O�t���O
	u16		TouchPanelExStop;						// �T���v�����O�������~�t���O

    u16		tp_x;			///< �^�b�`�p�l��X���W
	u16		tp_y;			///< �^�b�`�p�l��Y���W
	u16		tp_trg;			///< �^�b�`�p�l���ڐG����g���K
	u16		tp_cont;		///< �^�b�`�p�l���ڐG������
    u8 tp_auto_samp;        ///< AUTO�T���v�����O���s�����ǂ���
};

static UI_TPSYS* _pUITP = NULL;   ///< �������������Ȃ��̂ł����Ń|�C���^�Ǘ�

//==============================================================
// Prototype
//==============================================================
static BOOL _circle_hitcheck( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );
static BOOL _rect_hitcheck( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );
static int _tblHitCheck( const GFL_UI_TP_HITTBL *tbl, const u16 x, const u16 y );
static u32 startSampling( UI_TPSYS* pTP, u32 sync );


//==============================================================================
/**
 * @brief   �^�b�`�p�l�����[�N�𓾂�
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  UI_TPSYS  �^�b�`�p�l���V�X�e�����[�N
 */
//==============================================================================

static UI_TPSYS* _UI_GetTPSYS(const UISYS* pUI)
{
//    if(_pUITP==NULL){
//        OS_TPanic("no init");
//    }
    return _pUITP;
}

//==============================================================================
/**
 * @brief  �^�b�`�p�l��������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  none
 */
//==============================================================================

void GFL_UI_TP_Init(const HEAPID heapID)
{
	TPCalibrateParam calibrate;
    UI_TPSYS* pTP = GFL_HEAP_AllocMemory(heapID, sizeof(UI_TPSYS));

    MI_CpuClear8(pTP, sizeof(UI_TPSYS));
	// �T���v�����O�t���O��������
	pTP->TouchPanelSampFlag = TP_SAMP_NONE;
	pTP->TouchPanelExStop = FALSE;
	// �^�b�`�p�l���̏������ƃL�����u���[�V�������Z�b�g
	TP_Init();

	// �}�V���̃L�����u���[�V�����l���擾
	if( TP_GetUserInfo( &calibrate ) == TRUE ){
		// �L�����u���[�V�����l�̐ݒ�
		TP_SetCalibrateParam( &calibrate );
		GFL_UI_PRINT("Get Calibration Parameter from NVRAM\n");
	}
	else{
		// �擾�Ɏ��s�����̂Ńf�t�H���g�̃L�����u���[�V�����̐ݒ�
		calibrate.x0 = 0x02ae;
		calibrate.y0 = 0x058c;
		calibrate.xDotSize = 0x0e25;
		calibrate.yDotSize = 0x1208;
		TP_SetCalibrateParam( &calibrate );
		GFL_UI_PRINT( "Warrning : TauchPanelInit( not found valid calibration data )\n" );
	}
    _pUITP = pTP;
//    return pTP;
}

//==============================================================================
/**
 * @brief �^�b�`�p�l���ǂݎ�菈��
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

static void GFI_UI_TP_Main(UISYS* pUI)
{
	TPData	tpTemp;
	TPData	tpDisp;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if(pTP==NULL){
        return;
    }
    
	// �ӂ����܂��Ă���ꍇ�͑S�Ă̓��͂��Ȃ��ɂ���
	if(PAD_DetectFold()){
        MI_CpuClear8(pTP, sizeof(UI_TPSYS));
    }

	// �^�b�`�p�l���f�[�^���擾
	if(pTP->tp_auto_samp == 0){
		while( TP_RequestRawSampling( &tpTemp ) != 0 ){};	//�T���v�����O�ɐ�������܂ő҂�
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
	pTP->tp_cont = tpDisp.touch;										// �x�^ ����
}

//==============================================================================
/**
 * @brief �^�b�`�p�l���ǂݎ�菈��
 * @param   none
 * @return  none
 */
//==============================================================================

void GFL_UI_TP_Main(void)
{
    GFI_UI_TP_Main(_UI_GetUISYS());
}

//==============================================================================
/**
 * @brief �^�b�`�p�l���I������
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

static void GFI_UI_TP_Exit(UISYS* pUI)
{
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
    GFL_HEAP_FreeMemory(pTP);
}

//==============================================================================
/**
 * @brief �^�b�`�p�l���I������
 * @param   none
 * @return  none
 */
//==============================================================================

void GFL_UI_TP_Exit(void)
{
    GFI_UI_TP_Exit(_UI_GetUISYS());
    _pUITP = NULL;
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
	if( ((u32)( x - (u32)tbl->rect.left) <= (u32)((u32)tbl->rect.right - (u32)tbl->rect.left))
	&	((u32)( y - (u32)tbl->rect.top) <= (u32)((u32)tbl->rect.bottom - (u32)tbl->rect.top))
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

    for(i = 0; tbl[i].circle.code != GFL_UI_TP_HIT_END; i++)
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
static int GFI_UI_TP_HitCont( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
    
	if( pTP->tp_cont ){
		return _tblHitCheck(tbl, pTP->tp_x, pTP->tp_y);
	}
	return GFL_UI_TP_HIT_NONE;
}


//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�x�^���́j
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @return  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
int GFL_UI_TP_HitCont( const GFL_UI_TP_HITTBL *tbl )
{
    return GFI_UI_TP_HitCont(_UI_GetUISYS(), tbl);
}

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
static int GFI_UI_TP_HitTrg( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if( pTP->tp_trg ){
		return _tblHitCheck(tbl, pTP->tp_x, pTP->tp_y);
	}
	return GFL_UI_TP_HIT_NONE;
}

//------------------------------------------------------------------
/**
 * @brief ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 * @param[in]   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
int GFL_UI_TP_HitTrg( const GFL_UI_TP_HITTBL *tbl )
{
    return GFI_UI_TP_HitTrg(_UI_GetUISYS(), tbl);
}

//------------------------------------------------------------------
/**
 * @brief  �^�b�`�p�l���ɐG��Ă��邩
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
static BOOL GFI_UI_TP_GetCont( const UISYS* pUI )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
    return pTP->tp_cont;
}

//------------------------------------------------------------------
/**
 * @brief  �^�b�`�p�l���ɐG��Ă��邩
 * @param   none
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
BOOL GFL_UI_TP_GetCont( void )
{
    return GFI_UI_TP_GetCont(_UI_GetUISYS());
}

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
static BOOL GFI_UI_TP_GetTrg( const UISYS* pUI )
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	return pTP->tp_trg;
}

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 * @param   none
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
BOOL GFL_UI_TP_GetTrg( void )
{
    return GFI_UI_TP_GetTrg(_UI_GetUISYS());
}

//------------------------------------------------------------------
/**
 * @brief �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�x�^���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
static BOOL GFI_UI_TP_GetPointCont( const UISYS* pUI, u32* x, u32* y )
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
 * @brief �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�x�^���́j
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
BOOL GFL_UI_TP_GetPointCont( u32* x, u32* y )
{
    return GFI_UI_TP_GetPointCont( _UI_GetUISYS(), x, y );
}

//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�g���K���́j
 * @param[in]   pUI	    ���[�U�[�C���^�[�t�F�C�X�V�X�e��
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
static BOOL GFI_UI_TP_GetPointTrg( const UISYS* pUI, u32* x, u32* y )
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

//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�g���K���́j
 * @param[out]   x		�w���W�󂯎��ϐ��A�h���X
 * @param[out]   y		�x���W�󂯎��ϐ��A�h���X
 * @retval  TRUE  �G��Ă���
 * @retval  FALSE �G��Ă��Ȃ��B�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
BOOL GFL_UI_TP_GetPointTrg( u32* x, u32* y )
{
    return GFI_UI_TP_GetPointTrg( _UI_GetUISYS(), x, y );
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
int GFL_UI_TP_HitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y )
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
static int GFI_UI_TP_GetAutoSamplingFlg(const UISYS* pUI)
{
    const UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	return pTP->tp_auto_samp;
}

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O���s���Ă��邩�ǂ����𓾂�
 * @param   none
 * @retval  TRUE  �I�[�g�T���v�����O�ł���
 * @retval  FALSE  ���Ă��Ȃ�
 */
//==============================================================================
int GFL_UI_TP_GetAutoSamplingFlg(void)
{
    return GFI_UI_TP_GetAutoSamplingFlg(_UI_GetUISYS());
}

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O�ݒ�
 * @param[out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]    bAuto   �I�[�g�T���v�����O����Ȃ�TRUE
 * @return  none
 */
//==============================================================================
static void GFI_UI_TP_SetAutoSamplingFlg(UISYS* pUI, const BOOL bAuto)
{
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	pTP->tp_auto_samp = bAuto;
}

//==============================================================================
/**
 * @brief �I�[�g�T���v�����O�ݒ�
 * @param        none
 * @param[in]    bAuto   �I�[�g�T���v�����O����Ȃ�TRUE
 * @return  none
 */
//==============================================================================
void GFL_UI_TP_SetAutoSamplingFlg(const BOOL bAuto)
{
    GFI_UI_TP_SetAutoSamplingFlg(_UI_GetUISYS(), bAuto);
}

//----------------------------------------------------------------------------
/**
 * @brief	�T���v�����O�J�n���̃f�[�^�ݒ�
 * @param   pTP             ���[�N
 * @param	SampFlag		�T���v�����O�t���O
 * @param	auto_samp		AUTO�T���v�����O�t���O
 * @param	pBuff			�o�b�t�@�|�C���^
 * @param	buffSize		�o�b�t�@�T�C�Y
 * @param	nowBuffPoint	���T���v�����̈ʒu
 * @param	Sync			�V���N��
 * @return	none
 */
//-----------------------------------------------------------------------------
static void setStartBufferingParam( UI_TPSYS* pTP, u32 SampFlag, u32 auto_samp, void* pBuff, u32 buffSize, u32 nowBuffPoint, u32 Sync )
{
	int i;
    // �T���v�����O�t���O���Z�b�g
	pTP->TouchPanelSampFlag = SampFlag;
    // �V�X�e���̃t���O���I�[�g�T���v�����O���ɂ���
    pTP->tp_auto_samp = auto_samp;
    // �o�b�t�@�̃|�C���^�ƃT�C�Y���i�[
	pTP->pTouchPanelBuff = pBuff;
	pTP->TouchPanelBuffSize = buffSize;
	pTP->TouchPanelSync = Sync;
	pTP->TouchPanelNowBuffPoint = nowBuffPoint;
//	oneSyncBuffClean( pTP->TouchPanelOneSync, TP_ONE_SYNC_BUFF );
	for( i = 0; i < TP_ONE_SYNC_BUFF; i++ ){
		pTP->TouchPanelOneSync[ i ].touch = 0;
	}
}

//----------------------------------------------------------------------------
/**
 * @brief	�T���v�����O��~
 * @param  pTP             ���[�N
 * @retval TP_OK         ��~�����ꍇ
 * @retval TP_FIFO_ERR   ��~�Ɏ��s
 */
//-----------------------------------------------------------------------------
static u32 stopSampling( UI_TPSYS* pTP )
{
	u32 check_num = 0;			// ������
	u32 result;					// ����
	int	i;						// ���[�v�p

	// ���݂̃T���v�����O���I������
	// ����ɖ��߂̓]���������������Ǘ�����
	//
	// ���T���v�����O���Ă���̂�����
    if( pTP->TouchPanelSampFlag == TP_SAMP_NONE ){
		// �T���v�����O���Ă��Ȃ��̂łƂ܂�����Ԃł��B
		return TP_OK;
	}

    if( TP_RequestAutoSamplingStop()!=0 ){
        GFL_UI_PRINT( "�I�����ߓ]�����s\n" );
		return TP_FIFO_ERR;
	}
	return TP_OK;
}

//----------------------------------------------------------------------------
/**
 * @brief	���̍��W����ʍ��W�ɂ��ĕԂ�(TPData�z��p)
 * @param	pData�F�ύX����^�b�`�p�l���f�[�^�z��
 * @param	size�F�z��T�C�Y
 * @return	none
 */
//-----------------------------------------------------------------------------
static void ChangeTPDataBuff( TPData* pData, u32 size )
{	
	int i;		// ���[�v�p
	TPData sample;

	// ���̃^�b�`�p�l���l�����ʍ��W�ɕϊ����ĕԂ�
	for( i = 0; i < size; i++ ){
		TP_GetCalibratedPoint( &sample, &(pData[ i ]) );
		pData[ i ] = sample;
	}
}

//----------------------------------------------------------------------------
/**
 * @brief	���̃V���N�̃T���v�����O�����擾
 * @param	pTP�F �^�b�`�p�l��AUTO�T���v�����O���[�N
 * @param	pData�F���̃V���N�̃f�[�^�ۑ��p�̈�̃|�C���^
 * @param	last_idx�F�Ō�ɃT���v�����O�����ʒu
 * @return	none
 */
//-----------------------------------------------------------------------------
static void getThisSyncData( UI_TPSYS* pTP, TP_ONE_DATA* pData, u32 last_idx )
{
	int i;				// ���[�v�p
	s16	samp_work;		// �T���v�����O��Ɨp

	//
	// ������
	//
	pData->Size = 0;
	for( i = 0; i < TP_ONE_SYNC_DATAMAX; i++ ){
		pData->TPDataTbl[ i ].validity = TP_VALIDITY_VALID;
		pData->TPDataTbl[ i ].touch = TP_TOUCH_OFF;
		pData->TPDataTbl[ i ].x = 0;
		pData->TPDataTbl[ i ].y = 0;
	}
	
	//
	// �^�b�`�p�l���̃T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�
	//
	// �^�b�`�p�l���o�b�t�@�Ƃ��̃t���[���̃T���v�����O�f�[�^
	// �Ɋi�[����
	for( i = 0; i < pTP->TouchPanelSync; i++ ){
		samp_work = last_idx - pTP->TouchPanelSync + i + 1;

		// 0�ȉ��̂Ƃ��͍ŏI�v�f���ɂ���
		if( samp_work < 0 ){
			samp_work += TP_ONE_SYNC_BUFF;
		}

		//
		// �L�������f�[�^���`�F�b�N
		//
		if( pTP->TouchPanelOneSync[ samp_work ].validity == TP_VALIDITY_VALID ){	
			// ���̃t���[���̃T���v�����O�����i�[
			pData->TPDataTbl[ pData->Size ]  = pTP->TouchPanelOneSync[ samp_work ];
			pData->Size++;
		}
	}
}

//----------------------------------------------------------------------------
/**
 * @brief	�������߂� abs?
 * @param	n1  �ʒu
 * @param	n2  �ʒu
 * @return	����
 */
//-----------------------------------------------------------------------------
static inline int TouchPanel_GetDiff( int n1, int n2 )
{
	int calc = (n1 >= n2) ? (n1 - n2) : (n2 - n1);
	return calc;
}

//----------------------------------------------------------------------------
/**
 * @brief	�o�b�t�@�Ɉ��k���āA���W���i�[
 * @param	pTP�F �^�b�`�p�l��AUTO�T���v�����O���[�N
 * @param	type		�o�b�t�@�����O�^�C�v
 * @param	last_idx	�Ō�ɃT���v�����O�����ʒu
 * @param	comp_num	�o�b�t�@�Ɋi�[����Ƃ��ɁAcomp_num�ʂ̂�����������i�[����
 * @return	�o�b�t�@�Ɋi�[�����T�C�Y
 */
//-----------------------------------------------------------------------------
static u32 mainBuffComp( UI_TPSYS* pTP, u32 type, u32 last_idx, u32 comp_num )
{
	int i;				// ���[�v�p
	s32 dist_x;			// �����ǂ̂��炢���邩
	s32 dist_y;			// �����ǂ̂��炢���邩
	s16	samp_work;		// �T���v�����O��Ɨp

	//
	// �^�b�`�p�l���̃T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�
	//
	// �^�b�`�p�l���o�b�t�@�Ƃ��̃t���[���̃T���v�����O�f�[�^
	// �Ɋi�[����
	for( i = 0; i < pTP->TouchPanelSync; i++ ){
		samp_work = last_idx - pTP->TouchPanelSync + i + 1;

		// 0�ȉ��̂Ƃ��͍ŏI�v�f���ɂ���
		if( samp_work < 0 ){
			samp_work += TP_ONE_SYNC_BUFF;
		}

		// �L���ȃf�[�^�̎��o�b�t�@�Ɋi�[
		if( (pTP->TouchPanelOneSync[ samp_work ].touch == TP_TOUCH_ON) &&
			(pTP->TouchPanelOneSync[ samp_work ].validity == TP_VALIDITY_VALID) ){
			// �������߂�
			dist_x = TouchPanel_GetDiff(pTP->pTouchPanelBuff[ pTP->TouchPanelNowBuffPoint - 1 ].x, pTP->TouchPanelOneSync[ samp_work ].x);
			dist_y = TouchPanel_GetDiff(pTP->pTouchPanelBuff[ pTP->TouchPanelNowBuffPoint - 1 ].y, pTP->TouchPanelOneSync[ samp_work ].y);
			
			// ����comp_num�ȏォ�`�F�b�N
			if( (dist_x >= comp_num) ||
				(dist_y >= comp_num)){
					
				// �T���v�����O�f�[�^�i�[
				pTP->pTouchPanelBuff[ pTP->TouchPanelNowBuffPoint ] = pTP->TouchPanelOneSync[ samp_work ];				
				pTP->TouchPanelNowBuffPoint++;
				
				// �o�b�t�@�T�C�Y�I�[�o�[�`�F�b�N
				if(pTP->TouchPanelNowBuffPoint >= pTP->TouchPanelBuffSize){
					// �o�b�t�@�����[�v�����邩�`�F�b�N
					if(type == TP_BUFFERING){
						pTP->TouchPanelNowBuffPoint %= pTP->TouchPanelBuffSize;	// ���[�v������
					}else{
						return TP_END_BUFF;		// ���[�v�����Ȃ�
					}
				}
			}
		}
	}

	
	// �Ō�ɃT���v�����O���ꂽ�C���f�b�N�X��Ԃ�	
	return pTP->TouchPanelNowBuffPoint;
}

//----------------------------------------------------------------------------
/**
 * @brief	�o�b�t�@�ɗL���f�[�^�A�^�b�`�̔���������Ɋi�[
 * @param	pTP�F �^�b�`�p�l��AUTO�T���v�����O���[�N
 * @param	type		�o�b�t�@�����O�^�C�v
 * @param	last_idx	�Ō�ɃT���v�����O�����ʒu
 * @return	�o�b�t�@�Ɋi�[�����T�C�Y
 */
//-----------------------------------------------------------------------------
static u32 mainBuffJust( UI_TPSYS* pTP,u32 type, u32 last_idx )
{
	int i;				// ���[�v�p
	s16	samp_work;		// �T���v�����O��Ɨp

	//
	// �^�b�`�p�l���̃T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�
	//
	// �^�b�`�p�l���o�b�t�@�Ƃ��̃t���[���̃T���v�����O�f�[�^
	// �Ɋi�[����
	for( i = 0; i < pTP->TouchPanelSync; i++ ){
		samp_work = last_idx - pTP->TouchPanelSync + i + 1;

		// 0�ȉ��̂Ƃ��͍ŏI�v�f���ɂ���
		if( samp_work < 0 ){
			samp_work += TP_ONE_SYNC_BUFF;
		}

		// �T���v�����O�f�[�^�i�[
		pTP->pTouchPanelBuff[ pTP->TouchPanelNowBuffPoint ] = pTP->TouchPanelOneSync[ samp_work ];
		pTP->TouchPanelNowBuffPoint++;

		// �o�b�t�@�T�C�Y�I�[�o�[�`�F�b�N
		if(pTP->TouchPanelNowBuffPoint >= pTP->TouchPanelBuffSize){
			// �o�b�t�@�����[�v�����邩�`�F�b�N
			if(type == TP_BUFFERING_JUST){
				pTP->TouchPanelNowBuffPoint %= pTP->TouchPanelBuffSize;	// ���[�v������
			}else{
				return TP_END_BUFF;		// ���[�v�����Ȃ�
			}
		}
	}

	
	// �Ō�ɃT���v�����O���ꂽ�C���f�b�N�X��Ԃ�	
	return pTP->TouchPanelNowBuffPoint;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	�T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�
 *			�o�b�t�@�����O���[�h
 *@param	pTP�F �^�b�`�p�l��AUTO�T���v�����O���[�N
 *@param	type�F�T���v�����O��ʂ̔ԍ�
 *@param	last_idx�F���X�g�C���f�b�N�X
 *@param	comp_num�F�o�b�t�@�Ɋi�[����Ƃ��ɁAcomp_num�ʂ̂�����������i�[����(���k���[�h���̂�)
 *@return	�T���v�����O��ʂɂ��ω�
					TP_BUFFERING---�T���v�����O���ꂽ�o�b�t�@�T�C�Y
					TP_NO_LOOP---�T���v�����O���ꂽ�o�b�t�@�T�C�Y
								 �o�b�t�@����t�ɂȂ����Ƃ� TP_END_BUFF
					TP_NO_BUFF---TP_OK
 *
 */
//-----------------------------------------------------------------------------
static u32 modeBuff( UI_TPSYS* pTP, u32 type, u32 last_idx, u32 comp_num )
{		
	u32 ret;		// �߂�l
	// type�̃��[�h�Ńo�b�t�@�����O
	switch( type ){
	case TP_BUFFERING:
	case TP_NO_LOOP:
		ret = mainBuffComp( pTP, type, last_idx, comp_num );
		break;
		
	case TP_BUFFERING_JUST:	// ���̂܂܊i�[
	case TP_NO_LOOP_JUST:
		ret = mainBuffJust( pTP, type, last_idx );
		break;
	default:		// ���̑�
		ret = TP_OK;
		break;			// �Ȃɂ����Ȃ�
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *
 * @brief	�T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�	
 * @param   pUI  UI���[�N
 * @param	pData ���̃t���[���̏��(init�Ŏw�肵���T���v�����O�񐔕��̏��)
 * @param	type �T���v�����O��ʂ̔ԍ�
 * @param	comp_num �o�b�t�@�Ɋi�[����Ƃ��ɁAcomp_num�ʂ̂�����������i�[����
 * @retval  TP_BUFFERING	�T���v�����O���ꂽ�o�b�t�@�T�C�Y
 * @retval  TP_NO_LOOP		�T���v�����O���ꂽ�o�b�t�@�T�C�Y
 *							�o�b�t�@����t�ɂȂ����Ƃ� TP_END_BUFF
 * @retval  TP_NO_BUFF		TP_OK
 * @retval  TP_SAMP_NOT_START	�T���v�����O�J�n����Ă��܂���
 */
//-----------------------------------------------------------------------------
static u32 GFI_UI_TP_AutoSamplingMain( UISYS* pUI, TP_ONE_DATA* pData, u32 type, u32 comp_num )
{
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	u32 ret = TP_SAMP_NOT_START;	// �߂�l
	u32 last_idx;					// �Ō�ɃT���v�����O���ꂽ�ʒu

    if(pTP->tp_auto_samp!=TRUE){
        return ret;
    }
	// �T���v�����O����Ă��邩�`�F�b�N
	if( pTP->TouchPanelSampFlag != TP_SAMP_NONE ){

		// �Ō�ɃT���v�����O�����ʒu���擾
		last_idx  = TP_GetLatestIndexInAuto();
		
		// ��ʍ��W�ɕύX
		ChangeTPDataBuff( pTP->TouchPanelOneSync, TP_ONE_SYNC_BUFF );

		// ���̃t���[���̏����擾
		if( pData != NULL ){
			getThisSyncData( pTP, pData, last_idx );
		}
		
		// type�̃o�b�t�@�����O�܂��͍��̃t���[�������擾��
		// �f�[�^���Z�b�g���ĕԂ�
		// �܂��o�b�t�@�����O�����Init����Ă��邩�`�F�b�N
		if( pTP->TouchPanelSampFlag == TP_BUFFERING ){
			ret = modeBuff( pTP, type, last_idx, comp_num );	
		}else{
			// ���̑��̂Ƃ���TP_OK��Ԃ�
			ret = TP_OK;
		}
	}
    else{
        GFL_UI_PRINT("%s warning�F�T���v�����O�J�n����Ă��܂���\n",__FILE__);
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *
 * @brief	�T���v�����O�����Ǘ����A���̏�Ԃ�Ԃ�	
 * @param   pUI  UI���[�N
 * @param	pData ���̃t���[���̏��(init�Ŏw�肵���T���v�����O�񐔕��̏��)
 * @param	type �T���v�����O��ʂ̔ԍ�
 * @param	comp_num �o�b�t�@�Ɋi�[����Ƃ��ɁAcomp_num�ʂ̂�����������i�[����
 * @retval  TP_BUFFERING	�T���v�����O���ꂽ�o�b�t�@�T�C�Y
 * @retval  TP_NO_LOOP		�T���v�����O���ꂽ�o�b�t�@�T�C�Y
 *							�o�b�t�@����t�ɂȂ����Ƃ� TP_END_BUFF
 * @retval  TP_NO_BUFF		TP_OK
 * @retval  TP_SAMP_NOT_START	�T���v�����O�J�n����Ă��܂���
 */
//-----------------------------------------------------------------------------
u32 GFL_UI_TP_AutoSamplingMain( TP_ONE_DATA* pData, u32 type, u32 comp_num )
{
    return GFI_UI_TP_AutoSamplingMain( _UI_GetUISYS(), pData, type, comp_num );
}

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v������̍ĊJ����
 * @param   pUI             UI�̃��[�N
 * @return	none
 */
//-----------------------------------------------------------------------------
static void GFI_UI_TP_AutoSamplingReStart( UISYS* pUI )
{
	u32 result;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);
	
    if(pTP==NULL){
        return;
    }
	// ��~�����`�F�b�N
	if( pTP->TouchPanelExStop == FALSE ){
		return ;
	}
	// �T���v�����O�����`�F�b�N
	if( pTP->TouchPanelSampFlag == TP_SAMP_NONE ){
		return ;
	}

	// �T���v�����O���J�n������
	result = startSampling( pTP, pTP->TouchPanelSync / 2 );	// /2��30�V���N�̂���
	GF_ASSERT( result == TP_OK );

	pTP->TouchPanelExStop = FALSE;
}

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v������̍ĊJ����
 * @param   pUI             UI�̃��[�N
 * @return	none
 */
//-----------------------------------------------------------------------------
void GFL_UI_TP_AutoSamplingReStart( void )
{
    GFI_UI_TP_AutoSamplingReStart( _UI_GetUISYS() );
}

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v�����O�̒�~����
 * @param   pUI             UI�̃��[�N
 * @return  none
 */
//-----------------------------------------------------------------------------
static void GFI_UI_TP_AutoSamplingStop( UISYS* pUI )
{
	u32 result;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if(pTP==NULL){
        return;
    }
    // ���łɒ�~�����`�F�b�N
	if( pTP->TouchPanelExStop == TRUE ){
		return ;
	}
	// �T���v�����O�����`�F�b�N
	if( pTP->TouchPanelSampFlag == TP_SAMP_NONE ){
		return ;
	}

	// �T���v�����O���~������
	result = stopSampling( pTP );
	GF_ASSERT( result == TP_OK );

	pTP->TouchPanelExStop = TRUE;
}

//----------------------------------------------------------------------------
/**
 * @brief	�X���[�v�����O�̒�~����
 * @param   none
 * @return  none
 */
//-----------------------------------------------------------------------------
void GFL_UI_TP_AutoSamplingStop( void )
{
    GFI_UI_TP_AutoSamplingStop( _UI_GetUISYS() );
}

//----------------------------------------------------------------------------
/**
 * @brief	�I�[�g�T���v�����O�J�n���߂�]��
 * @param	pTP�F �^�b�`�p�l��AUTO�T���v�����O���[�N
 * @param	sync�F�P�t���[���̃T���v�����O��
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 */
//-----------------------------------------------------------------------------
static u32 startSampling( UI_TPSYS* pTP, u32 sync )
{
	u32	result;

    result = TP_RequestAutoSamplingStart(
        0,			// �x�[�XV�J�E���g
        sync,		// �P�t���[���ɉ���T���v�����O���邩
        pTP->TouchPanelOneSync,				// �T���v�����O�f�[�^�i�[�p�o�b�t�@
        TP_ONE_SYNC_BUFF );		// �o�b�t�@�T�C�Y

	if( result != 0 ) {
		GFL_UI_PRINT( "�X�^�[�g���ߓ]�����s\n" );
		return TP_FIFO_ERR;
	}
	return TP_OK;
}

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n
 * @param	pTP�F TP work�|�C���^
 * @param	sync�F�P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
static u32 _autoStart(UI_TPSYS* pTP, u32 sync)
{
	// �X���[�v��~���͋���
	if( pTP->TouchPanelExStop == TRUE ){
		return TP_ERR;
	}
	// �P�t���[���ɂT�ȏ�T���v�����O���悤�Ƃ������~
	if( sync > 4 ){
        OS_Panic("sync <= 4");
		return TP_ERR;
	}
	// �T���v�����O���Ȃ甲����
	if( pTP->TouchPanelSampFlag != TP_SAMP_NONE ){
		GFL_UI_PRINT("%s: �T���v�����O���ł�\n",__FILE__);
		return TP_ERR;
	}
	// �T���v�����O���ߓ]��
	return startSampling( pTP, sync );
}

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n
 * @param	pUI  UIwork�|�C���^
 * @param	p_buff �T���v�����O�f�[�^������o�b�t�@
 * @param	size �o�b�t�@�̃T�C�Y
 * @param	sync �P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK ����
 * @retval	TP_FIFO_ERR �]�����s
 * @retval	TP_ERR �]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
static u32 GFI_UI_TP_AutoStart(UISYS* pUI, TPData* p_buff, u32 size, u32 sync)
{
	u32	result;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if(pTP==NULL){
        return TP_ERR;
    }
    result = _autoStart(pTP, sync);
	// �]���Ɏ��s������G���[��Ԃ�
	if( result != TP_OK  ){
		return result;
	}
	// �T���v�����O�f�[�^�ݒ�
	setStartBufferingParam( pTP, TP_BUFFERING, TRUE,
                            p_buff, size, 0,
                            sync * 2 ); // 30�t���[������̂���

	return TP_OK;
}

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n
 * @param	p_buff �T���v�����O�f�[�^������o�b�t�@
 * @param	size �o�b�t�@�̃T�C�Y
 * @param	sync �P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK ����
 * @retval	TP_FIFO_ERR �]�����s
 * @retval	TP_ERR �]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
u32 GFL_UI_TP_AutoStart(TPData* p_buff, u32 size, u32 sync)
{
    return GFI_UI_TP_AutoStart(_UI_GetUISYS(), p_buff, size, sync);
}

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n	�o�b�t�@�����O�Ȃ�
 * @param	pUI  UIwork�|�C���^
 * @param	sync �P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK ����
 * @retval	TP_FIFO_ERR �]�����s
 * @retval	TP_ERR �]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
static u32 GFI_UI_TP_AutoStartNoBuff(UISYS* pUI, u32 sync)
{
	u32	result;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    result = _autoStart(pTP, sync);
    // �]���Ɏ��s������G���[��Ԃ�
	if( result != TP_OK ){
		return result;
	}
	// �T���v�����O�f�[�^�ݒ�
	setStartBufferingParam( pTP, TP_NO_BUFF, TRUE,
			NULL, 0, 0,
			sync * 2 ); // 30�t���[������̂���

	return TP_OK;
}

//----------------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l����AUTO�T���v�����O�J�n	�o�b�t�@�����O�Ȃ�
 * @param	pUI  UIwork�|�C���^
 * @param	sync �P�t���[���ɉ���T���v�����O����̂�(MAX4)
 * @retval	TP_OK ����
 * @retval	TP_FIFO_ERR �]�����s
 * @retval	TP_ERR �]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
u32 GFL_UI_TP_AutoStartNoBuff(u32 sync)
{
    return GFI_UI_TP_AutoStartNoBuff(_UI_GetUISYS(), sync);
}

//----------------------------------------------------------------------------
/**
 * @brief	�T���v�����O���I������
 * @param	pUI UIwork�|�C���^
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
static u32 GFI_UI_TP_AutoStop( UISYS* pUI )
{
	u32 result;
    UI_TPSYS* pTP = _UI_GetTPSYS(pUI);

    if(pTP==NULL){
        return TP_ERR;
    }
	// �X���[�v��~���͋���
	if( pTP->TouchPanelExStop == TRUE ){
		return TP_ERR;
	}
	
	result = stopSampling( pTP );
	if( result == TP_OK ){
		// �i�[���Ă������f�[�^�j��
		setStartBufferingParam( pTP, TP_SAMP_NONE, FALSE, NULL, 0, 0, 0 );
	}
	return result;
}

//----------------------------------------------------------------------------
/**
 * @brief	�T���v�����O���I������
 * @retval	TP_OK�F����
 * @retval	TP_FIFO_ERR�F�]�����s
 * @retval	TP_ERR�F�]���ȊO�̎��s
 */
//-----------------------------------------------------------------------------
u32 GFL_UI_TP_AutoStop( void )
{
    return GFI_UI_TP_AutoStop( _UI_GetUISYS() );
}

