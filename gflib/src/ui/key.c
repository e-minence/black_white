//==============================================================================
/**
 * @file	key.c
 * @brief	���[�U�[�C���^�[�t�F�C�X �L�[�̊Ǘ�
            �f�o�C�X���A�N�Z�X�֐�
 * @author	k.ohno
 * @date	2006.11.17
 */
//==============================================================================

#include "gflib.h" 

#include "ui.h"
#include "ui_def.h"

//==============================================================================
//
//			��`�A�֐��錾
//
//==============================================================================

static void _keyConvert(UI_KEYSYS* pKey);

//------------------------------------------------------------------
/**
 * @struct	_UIKeySys
 * @brief �L�[���̕ێ�
  */
//------------------------------------------------------------------
struct _UI_KEYSYS {
    const GFL_UI_KEY_CUSTOM_TBL* pControl;  ///< �L�[���[�h��ݒ肷��e�[�u��
	int control_mode;	///< �L�[�R���g���[�����[�h
    int control_tblno;  ///< �R���g���[���e�[�u���̏ꏊ
	int cont_org;			///< �L�[���
	int trg_org;			///< �L�[�g���K
	int repeat_org;			///< �L�[���s�[�g
	int cont;				///< �L�[���(�Q�[���ݒ�ɂ��ϊ�����������)
	int trg;				///< �L�[�g���K(�Q�[���ݒ�ɂ��ϊ�����������)
	int repeat;				///< �L�[���s�[�g(�Q�[���ݒ�ɂ��ϊ�����������)
	int repeatWait_Count;	///< �L�[���s�[�g�J�E���^
	int repeatSpeed;		///< �L�[���s�[�g�X�s�[�h
	int repeatWait;			///< �L�[���s�[�g�E�F�C�g
};

//==============================================================================
//
//			�֐�
//
//==============================================================================

//==============================================================================
/**
 * @brief  �p�b�h������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @return  UI_KEYSYS  �L�[�V�X�e�����[�N
 */
//==============================================================================

UI_KEYSYS* GFL_UI_Key_sysInit(const int heapID)
{
    UI_KEYSYS* pKey = GFL_HEAP_AllocMemory(heapID, sizeof(UI_KEYSYS));

    MI_CpuClear8(pKey, sizeof(UI_KEYSYS));
	pKey->repeatSpeed = 8;
	pKey->repeatWait = 15;
    return pKey;
}

//==============================================================================
/**
 * @brief �p�b�h�ǂݎ�菈��
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

static void GFI_UI_Key_sysMain(UISYS* pUI)
{
	TPData	tpTemp;
	TPData	tpDisp;
	int	keyData;
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);

	// �ӂ����܂��Ă���ꍇ�͑S�Ă̓��͂��Ȃ��ɂ���
	if(PAD_DetectFold()){
	  pKey->trg	= 0;
	  pKey->cont	= 0;
	  pKey->repeat	= 0;
	  return;
	}

	keyData = PAD_Read();
	
	pKey->trg_org = keyData & (keyData ^ pKey->cont_org);	// �g���K ����
    pKey->repeat_org = keyData & (keyData ^ pKey->cont_org);        // ���s�[�g ����

    if((keyData!=0) && (pKey->cont_org==keyData)){
		if(--pKey->repeatWait_Count==0){
			pKey->repeat_org = keyData;
			pKey->repeatWait_Count = pKey->repeatSpeed;
		}
	}
	else{
		pKey->repeatWait_Count = pKey->repeatWait;
	}
	pKey->cont_org	= keyData;							// �x�^ ����

	pKey->trg = pKey->trg_org;
	pKey->cont = pKey->cont_org;
	pKey->repeat = pKey->repeat_org;

	_keyConvert(pKey);
}

//==============================================================================
/**
 * @brief �p�b�h�ǂݎ�菈��
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_Key_sysMain(void)
{
    GFI_UI_Key_sysMain(_UI_GetUISYS());
}

//==============================================================================
/**
 * @brief �p�b�h�I������
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

static void GFI_UI_Key_sysEnd(UISYS* pUI)
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
    GFL_HEAP_FreeMemory(pKey);
}

//==============================================================================
/**
 * @brief �p�b�h�I������
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================

void GFL_UI_Key_sysEnd(void)
{
    GFI_UI_Key_sysEnd(_UI_GetUISYS());
}

//==============================================================================
// �L�[���̕ϊ��}�N��
//==============================================================================
#define KEYCONV( key, check_pat, set_pat )	{ if( key & check_pat ) key |= set_pat; }
#define KEYCONV_RESET( key, check_pat, set_pat ) {	\
	if( key & check_pat ){							\
		key |= set_pat;								\
		key &= ( check_pat^0xffff );				\
	}												\
}
#define KEYCONV_EXCHANGE( key, pat1, pat2 ) {	\
	int	tmp = 0;								\
	if( key & pat1 ){							\
		tmp |= pat2;							\
	}											\
	if( key & pat2 ){							\
		tmp |= pat1;							\
	}											\
	key &= ( (pat1 | pat2 )^0xffff );			\
	key |= tmp;									\
}
#define KEY_RESET( key, reset_pat ) {			\
	key &= ( reset_pat^0xffff );				\
}

//==============================================================================
/**
 * @brief �L�[���̕ϊ�
 * @param[in,out]   pKey     ���[�U�[�C���^�[�t�F�C�X�L�[�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================
static void _keyConvert(UI_KEYSYS* pKey)
{
    const GFL_UI_KEY_CUSTOM_TBL* pTbl;
    if(pKey->pControl==NULL){
        return;
    }
    pTbl = &pKey->pControl[pKey->control_tblno];

    while(1){
        switch(pTbl->mode){
          case GFL_UI_KEY_END:
            return;
          case GFL_UI_KEY_COPY:
            KEYCONV( pKey->trg, pTbl->keySource, pTbl->keyDist );
            KEYCONV( pKey->cont, pTbl->keySource, pTbl->keyDist );
            KEYCONV( pKey->repeat, pTbl->keySource, pTbl->keyDist );
            break;
          case GFL_UI_KEY_CHANGE:
            KEYCONV_EXCHANGE( pKey->trg, pTbl->keySource, pTbl->keyDist );
            KEYCONV_EXCHANGE( pKey->cont, pTbl->keySource, pTbl->keyDist );
            KEYCONV_EXCHANGE( pKey->repeat, pTbl->keySource, pTbl->keyDist );
            break;
          case GFL_UI_KEY_RESET:
            KEY_RESET( pKey->trg, pTbl->keySource );
            KEY_RESET( pKey->cont, pTbl->keySource );
            KEY_RESET( pKey->repeat, pTbl->keySource );
            break;
        }
        pTbl++;
    }
    
#if 0
    switch( pKey->control_mode ){
	case KEYCONTROL_NORMAL:
		break;
	case KEYCONTROL_PATTERN1:

		KEYCONV( pKey->trg, PAD_BUTTON_START, PAD_BUTTON_X )
		KEYCONV( pKey->cont, PAD_BUTTON_START, PAD_BUTTON_X )
		KEYCONV( pKey->repeat, PAD_BUTTON_START, PAD_BUTTON_X )
		break;

	case KEYCONTROL_PATTERN2:

		KEYCONV_EXCHANGE( pKey->trg, PAD_BUTTON_X,PAD_BUTTON_Y )
		KEYCONV_EXCHANGE( pKey->cont, PAD_BUTTON_X,PAD_BUTTON_Y )
		KEYCONV_EXCHANGE( pKey->repeat, PAD_BUTTON_X,PAD_BUTTON_Y )
		break;

	case KEYCONTROL_PATTERN3:

		KEYCONV( pKey->trg, PAD_BUTTON_L, PAD_BUTTON_A )
		KEYCONV( pKey->cont, PAD_BUTTON_L, PAD_BUTTON_A )
		KEYCONV( pKey->repeat, PAD_BUTTON_L, PAD_BUTTON_A )

		KEY_RESET( pKey->trg, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		KEY_RESET( pKey->cont, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		KEY_RESET( pKey->repeat, ( PAD_BUTTON_L | PAD_BUTTON_R ) )
		break;
	}
#endif
}

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Z�b�g
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]	speed	���x
 * @param[in]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
void GFI_UI_KeySetRepeatSpeed(UISYS* pUI, const int speed, const int wait )
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
    pKey->repeatSpeed	= speed;
	pKey->repeatWait	= wait;
}

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Z�b�g
 * @param[in]	speed	���x
 * @param[in]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
void GFL_UI_KeySetRepeatSpeed(const int speed, const int wait )
{
    GFI_UI_KeySetRepeatSpeed(_UI_GetUISYS(),speed,wait );
}

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�e�[�u����ݒ�
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]	pTbl   �R���g���[���e�[�u���z��̃|�C���^
 * @return  none
 */
//==============================================================================
void GFI_UI_KeySetControlModeTbl(UISYS* pUI, const GFL_UI_KEY_CUSTOM_TBL* pTbl )
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
	pKey->pControl = pTbl;
}

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�e�[�u����ݒ�
 * @param[in]	pTbl   �R���g���[���e�[�u���z��̃|�C���^
 * @return  none
 */
//==============================================================================
void GFL_UI_KeySetControlModeTbl(const GFL_UI_KEY_CUSTOM_TBL* pTbl )
{
    GFI_UI_KeySetControlModeTbl(_UI_GetUISYS(),pTbl);
}

//==============================================================================
/**
 * @brief �L�[�g���K�Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �L�[�g���K
 */
//==============================================================================
int GFI_UI_KeyGetTrg( const UISYS* pUI )
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
	return pKey->trg;
}

//==============================================================================
/**
 * @brief �L�[�g���K�Q�b�g
 * @param  none
 * @return  �L�[�g���K
 */
//==============================================================================
int GFL_UI_KeyGetTrg( void )
{
    return GFI_UI_KeyGetTrg(_UI_GetUISYS());
}

//==============================================================================
/**
 * @brief �L�[�R���g�Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �L�[�R���g
 */
//==============================================================================
int GFI_UI_KeyGetCont( const UISYS* pUI )
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
	return pKey->cont;
}

//==============================================================================
/**
 * @brief �L�[�R���g�Q�b�g
 * @param   none
 * @return  �L�[�R���g
 */
//==============================================================================
int GFL_UI_KeyGetCont( void )
{
     return GFI_UI_KeyGetCont( _UI_GetUISYS() );
}

//==============================================================================
/**
 * @brief �L�[���s�[�g�Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �L�[�R���g
 */
//==============================================================================
int GFI_UI_KeyGetRepeat( const UISYS* pUI )
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
	return pKey->repeat;
}

//==============================================================================
/**
 * @brief �L�[���s�[�g�Q�b�g
 * @param none
 * @return  �L�[�R���g
 */
//==============================================================================
int GFL_UI_KeyGetRepeat(void )
{
    return GFI_UI_KeyGetRepeat( _UI_GetUISYS());
}

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Q�b�g
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[out]	speed	���x
 * @param[out]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
void GFI_UI_KeyGetRepeatSpeed(const UISYS* pUI, int* speed, int* wait )
{
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
	*speed = pKey->repeatSpeed;
	*wait = pKey->repeatWait;
}

//==============================================================================
/**
 * @brief �L�[���s�[�g�̑��x�ƃE�F�C�g���Q�b�g
 * @param[out]	speed	���x
 * @param[out]	wait	�E�F�C�g
 * @return  none
 */
//==============================================================================
void GFL_UI_KeyGetRepeatSpeed( int* speed, int* wait )
{
    GFI_UI_KeyGetRepeatSpeed(_UI_GetUISYS(), speed, wait);
}

//==============================================================================
/**
 * @brief   �L�[�R���t�B�O�̃��[�h�̒l��ݒ肷��
 * @param[in,out]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @param[in]   mode    �L�[�R���t�B�O���[�h
 * @return  none
 */
//==============================================================================
void GFI_UI_KeySetControlMode(UISYS* pUI,const int mode)
{
    const GFL_UI_KEY_CUSTOM_TBL* pTbl;
    UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
    int n = mode,tblno = 0;

    if(pKey->pControl==NULL){  // �e�[�u�����Ȃ��ꍇ�͐ݒ�ł��Ȃ�
        return;
    }
    pKey->control_mode = mode;
    pTbl = pKey->pControl;
    while(n){  // �e�[�u���̃R���t�B�O�̐擪������
        tblno++;
        if(pTbl->mode == GFL_UI_KEY_END){
            n--;
        }
        pTbl++;
    }
    pKey->control_tblno = tblno;
}

//==============================================================================
/**
 * @brief   �L�[�R���t�B�O�̃��[�h�̒l��ݒ肷��
 * @param[in]   mode    �L�[�R���t�B�O���[�h
 * @return  none
 */
//==============================================================================
void GFL_UI_KeySetControlMode(const int mode)
{
    GFI_UI_KeySetControlMode(_UI_GetUISYS(), mode);
}

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�̃��[�h�̒l�𓾂�
 * @param[in]   pUI     ���[�U�[�C���^�[�t�F�C�X�n���h���̃|�C���^
 * @return  �R���g���[�����[�h
 */
//==============================================================================
int GFI_UI_KeyGetControlMode(const UISYS* pUI)
{
    const UI_KEYSYS* pKey = _UI_GetKEYSYS(pUI);
	return pKey->control_mode;
}

//==============================================================================
/**
 * @brief �L�[�R���t�B�O�̃��[�h�̒l�𓾂�
 * @param   none
 * @return  �R���g���[�����[�h
 */
//==============================================================================
int GFL_UI_KeyGetControlMode(void)
{
    return GFI_UI_KeyGetControlMode(_UI_GetUISYS());
}
