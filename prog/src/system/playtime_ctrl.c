//============================================================================================
/**
 * @brief	playtime_ctrl.c
 * @brief	�v���C���ԏ��̑���
 * @date	2006.06.21
 * @author	taya
 * @todo  OS_InitTick��WiFi�ڑ����ɌĂ΂��̂ŁA�ʓr�v����i���l����
 *
 * 2010.02.18 tamada  ���V�X�e������ڍs��
 *
 *
 */
//============================================================================================

#include <gflib.h>
#include "savedata/playtime.h"

#include "system\playtime_ctrl.h"


static BOOL StartFlag = FALSE;
static u64  LastTick = 0;
static u64  PassedSec = 0;
static u64  StartTick = 0;

//------------------------------------------------------------------
/**
 * �������i�Q�[�����C�����[�v�̑O�ɂP�񂾂��Ăԁj
 */
//------------------------------------------------------------------
void PLAYTIMECTRL_Init( void )
{
	StartFlag = FALSE;
}

//------------------------------------------------------------------
/**
 * �v���C���Ԃ̃J�E���g�J�n�i�Q�[���J�n����ɂP�񂾂��Ăԁj
 *
 */
//------------------------------------------------------------------
void PLAYTIMECTRL_Start( void )
{
	StartFlag = TRUE;
	LastTick = 0;
	PassedSec = 0;
  StartTick = OS_GetTick();
	//StartTick = APTM_GetData();
}

//------------------------------------------------------------------
/**
 * �v���C���Ԃ̃J�E���g�i���C�����[�v���Ŗ���Ăԁj
 *
 * @param   time		�v���C���ԃZ�[�u�f�[�^�|�C���^
 *
 */
//------------------------------------------------------------------
void PLAYTIMECTRL_Countup( GAMEDATA* gamedata )
{
	if( StartFlag )
	{
		//u64  sec = APTM_CalcSec( APTM_GetData() - StartTick );
    u64 sec;
    u64 tick = OS_GetTick();

    // �`�b�N�^�C���̋t�_���ۃ`�F�b�N
    // Wifi�ڑ�����tick�͂O�N���A�����B
    if( LastTick > tick ){
      // 0�N���A�����O�̃`�b�N�����������ށB
      tick = tick + LastTick;
    }else{
      LastTick = tick;
    }

    sec = OS_TicksToSeconds( tick - StartTick );
		if( sec > PassedSec )
		{
			GAMEDATA_PlayTimeCountUp( gamedata, (sec - PassedSec) );
      
      if( LastTick == tick ){
        // �ʏ펞
			  PassedSec = sec;
      }else{

        // ��������̏�����Tick���N���A���ꂽ�ꍇ
        // �ď������B
        StartTick = OS_GetTick();
        PassedSec = OS_TicksToSeconds( StartTick );
        LastTick = StartTick;  // 
      }
		}
	}
}










