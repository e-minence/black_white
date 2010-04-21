//==============================================================================
/**
 * @file	codein_pv.c
 * @brief	�������̓C���^�[�t�F�[�X
 * @author	goto
 * @date	2007.07.11(��)
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"
#include "message.naix"

#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "system/wipe.h"

#include "sound/pm_sndsys.h"

#include "br_codein_pv.h"
#include "br_snd.h"


#include "test/ariizumi/ari_debug.h"

//�f�o�b�O�p�}�N��
#if DEB_ARI&0
#define CIPV_Printf(...)  (void)((OS_Printf(__VA_ARGS__)))
#else
#define CIPV_Printf(...)  ((void)0)
#endif //DEB_ARI

//--------------------------------------------------------------
/**
 * @brief	�t�H�[�J�X�Z�b�g���Ċg��k���Ώۂ�ݒ肷��
 *
 * @param	next_focus	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_FocusSet( BR_CODEIN_WORK* wk, int next_focus )
{
	int index;
	wk->focus_old = wk->focus_now;
	wk->focus_now = next_focus;
	
	wk->ls = 0;
	wk->le = 0;
	wk->ss = 0;
	wk->se = 0;
#if 0	
	if ( wk->focus_now != 0 ){		
		wk->ls = ( wk->focus_now - 1 ) * wk->param.block[ wk->focus_now - 0 ];
		wk->le = ( wk->focus_now - 0 ) * wk->param.block[ wk->focus_now - 1 ];
	}

	if ( wk->focus_old != 0 ){		
		wk->ss = ( wk->focus_old - 1 ) * wk->param.block[ wk->focus_old - 0 ];
		wk->se = ( wk->focus_old - 0 ) * wk->param.block[ wk->focus_old - 1 ];
	}
#endif

	if ( wk->focus_now != 0 ){
		wk->ls = wk->b_tbl[ wk->focus_now - 1 ][ 0 ];
		wk->le = wk->b_tbl[ wk->focus_now - 1 ][ 1 ];
	}
	if ( wk->focus_old != 0 ){
		wk->ss = wk->b_tbl[ wk->focus_old - 1 ][ 0 ];
		wk->se = wk->b_tbl[ wk->focus_old - 1 ][ 1 ];
	}

	CIPV_Printf( "fn = %2d | fo =  %2d\n", wk->focus_now, wk->focus_old );
	CIPV_Printf( "L = %2d ~ %2d\n", wk->ls, wk->le );
	CIPV_Printf( "S = %2d ~ %2d\n", wk->ss, wk->se );
}

//--------------------------------------------------------------
/**
 * @brief	�e��f�[�^������������
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_ParamInit( BR_CODEIN_WORK* wk )
{
	int i;
	
	///< Touch �p�l�����[�h
	wk->sys.touch = TRUE;
	
	///< block table �̍쐬
	{
		wk->b_tbl[ 0 ][ 0 ] = 0;
		wk->b_tbl[ 0 ][ 1 ] = wk->param.block[ 0 ];
		wk->b_tbl[ 1 ][ 0 ] = wk->param.block[ 0 ];
		wk->b_tbl[ 1 ][ 1 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ];
		wk->b_tbl[ 2 ][ 0 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ];
		wk->b_tbl[ 2 ][ 1 ] = wk->param.block[ 0 ] + wk->param.block[ 1 ] + wk->param.block[ 2 ];
	}		
	///< focus�̐ݒ�
	CI_pv_FocusSet( wk, 1 );

	///< block ��������code�̍ő吔�����߂�
	for ( i = 0; i < BR_CODE_BLOCK_MAX; i++ ){
		
		wk->code_max += wk->param.block[ i ];
	}
	
	///< table �쐬
	{
		int sw = 0;
		const x_tbl[][ BR_CODE_BLOCK_MAX ] = {
			{ POS_4_4_4_p1, POS_4_4_4_p2, POS_4_4_4_p3 },
			{ POS_2_5_5_p1, POS_2_5_5_p2, POS_2_5_5_p3 },
		};
		if ( wk->param.block[ 0 ] != 4 ){
			sw = 1;
		}
		for ( i = 0; i < BR_CODE_BLOCK_MAX; i++ ){
			
			wk->x_tbl[ i ] = x_tbl[ sw ][ i ];
		}
		wk->x_tbl[ i ] = x_tbl[ sw ][ i - 1 ];
	}
	
	///< code����ł�bar�̈ʒu
	{
		int pos = 0;			
		for ( i = 0; i < BAR_OAM_MAX; i++ ){
			
			pos += wk->param.block[ i ];
			wk->bar[ i ].state = pos - 1;
			CIPV_Printf( "bar pos = %2d\n", wk->bar[ i ].state );
		}
	}
	
	///< �O���[�v�̐ݒ�
	{
		int j;
		int bno = 0;
		i = 0;
		do {
			
			for ( j = 0; j < wk->param.block[ bno ]; j++ ){
				
				wk->code[ i ].group = bno + 1;
				CIPV_Printf( "group = %2d\n", wk->code[ i ].group );
				i++;
			}
			bno++;			
		} while ( i < wk->code_max );
	}
}

//--------------------------------------------------------------
/**
 * @brief	�V�[�P���X�؂�ւ�
 *
 * @param	wk	
 * @param	seq	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_SeqChange( BR_CODEIN_WORK* wk, int seq )
{
	wk->seq			= seq;
	wk->wait		= 0;
	wk->gene_seq	= 0;
	wk->gene_cnt	= 0;
}

//--------------------------------------------------------------
/**
 * @brief	������
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainInit( BR_CODEIN_WORK* wk )
{
	
	CI_pv_disp_CodeRes_Load( wk );	
	CI_pv_disp_SizeFlagSet( wk );	

	CI_pv_disp_CodeOAM_Create( wk );
	CI_pv_disp_MovePosSet( wk, eMPS_SET );
	
	CI_pv_disp_CurOAM_Create( wk );
  CI_pv_disp_BtnOAM_Create( wk );
	
	CI_pv_disp_HitTableSet( wk );
	
	
	CI_pv_FontOam_SysInit( wk );
	CI_pv_FontOam_Add( wk );
	
	
	CI_pv_SeqChange( wk, eSEQ_INPUT );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	�I��
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainEnd( BR_CODEIN_WORK* wk )
{
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	����
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainInput( BR_CODEIN_WORK* wk )
{
	switch ( wk->gene_seq ){
	case 0:
		wk->gene_seq++;
		break;

	default:	
		CI_pv_StateUpdate( wk );	
		
		GFL_BMN_Main( wk->sys.btn );
		break;
	}
	
	return FALSE;	
}

//--------------------------------------------------------------
/**
 * @brief	�t�H�[�J�X�̈ړ�
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainFocusMove( BR_CODEIN_WORK* wk )
{
	int i;
	static fx32 scale_l[] = {
		FX32_CONST(0.5f), FX32_CONST(0.2f), FX32_CONST(0.5f),
		FX32_CONST(1.0f), FX32_CONST(1.2f), FX32_CONST(1.0f), 
		FX32_CONST(1.0f)
	};
	static fx32 scale_s[] = {
		FX32_CONST(0.8f), FX32_CONST(0.6f), FX32_CONST(0.4f), 
		FX32_CONST(0.2f), FX32_CONST(0.8f), FX32_CONST(1.0f), 
		FX32_CONST(1.0f)
	};
	GFL_CLSCALE scale;

	switch( wk->gene_seq ){
	///< �g��k���O��
	case 0:
		CI_pv_disp_CurOAM_Visible( wk, 0, FALSE );
		{
			///< �R�[�h�����̏���
			for ( i = 0; i < wk->code_max; i++ ){
				GFL_CLACTPOS pos;
				if ( wk->code[ i ].move_wk.wait == 0 ){ continue; }
				GFL_CLACT_WK_GetPos( wk->code[ i ].clwk, &pos , CLSYS_DEFREND_SUB );
				pos.x += wk->code[ i ].move_wk.pos.x;
				pos.y += wk->code[ i ].move_wk.pos.y;
				GFL_CLACT_WK_SetPos( wk->code[ i ].clwk, &pos , CLSYS_DEFREND_SUB );
				wk->code[ i ].move_wk.wait--;				
				
				
				if ( i >= wk->ls && i < wk->le ){
					scale.x = scale_l[ wk->code[ i ].move_wk.scale ];
					scale.y = scale_l[ wk->code[ i ].move_wk.scale ];
					GFL_CLACT_WK_SetScale( wk->code[ i ].clwk, &scale );
					wk->code[ i ].move_wk.scale++;
				}
				
				if ( i >= wk->ss && i < wk->se ){
					scale.x = scale_s[ wk->code[ i ].move_wk.scale ];
					scale.y = scale_s[ wk->code[ i ].move_wk.scale ];
					GFL_CLACT_WK_SetScale( wk->code[ i ].clwk, &scale );
					wk->code[ i ].move_wk.scale++;
				}
			}
			
			///< �o�[�����̏���
			for ( i = 0; i < BAR_OAM_MAX; i++ ){
				GFL_CLACTPOS pos;
				if ( wk->bar[ i ].move_wk.wait == 0 ){ continue; }
				GFL_CLACT_WK_GetPos( wk->bar[ i ].clwk, &pos , CLSYS_DEFREND_SUB );
				pos.x += wk->bar[ i ].move_wk.pos.x;
				pos.y += wk->bar[ i ].move_wk.pos.y;
				GFL_CLACT_WK_SetPos( wk->bar[ i ].clwk, &pos , CLSYS_DEFREND_SUB );
				wk->bar[ i ].move_wk.wait--;
			}
			
			///< ����
			if ( wk->code[ 0 ].move_wk.wait == 0 ){
				for ( i = wk->ls; i < wk->le; i++ ){
					GFL_CLACT_WK_SetAnmSeq( wk->code[ i ].clwk, CI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );///< anime pat �� 3 �ɁB
				}
				for ( i = wk->ss; i < wk->se; i++ ){
					GFL_CLACT_WK_SetAnmSeq( wk->code[ i ].clwk, CI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );///< anime pat �� 3 �ɁB
				}
				wk->gene_seq++;
			}
			wk->gene_cnt++;
		}
		break;
		
	///< �g��k���㔼
	case 1:
		for ( i = wk->ls; i < wk->le; i++ ){
			if ( wk->code[ i ].move_wk.scale == SCALE_CNT_F ){ continue; }
			scale.x = scale_l[ wk->code[ i ].move_wk.scale ];
			scale.y = scale_l[ wk->code[ i ].move_wk.scale ];
			GFL_CLACT_WK_SetScale( wk->code[ i ].clwk, &scale );
			wk->code[ i ].move_wk.scale++;
		}
		for ( i = wk->ss; i < wk->se; i++ ){
			if ( wk->code[ i ].move_wk.scale == SCALE_CNT_F ){ continue; }
			scale.x = scale_s[ wk->code[ i ].move_wk.scale ];
			scale.y = scale_s[ wk->code[ i ].move_wk.scale ];
			GFL_CLACT_WK_SetScale( wk->code[ i ].clwk, &scale );
			wk->code[ i ].move_wk.scale++;
		}
		wk->gene_cnt++;
		if ( wk->gene_cnt == SCALE_CNT_F ){
			
			wk->gene_seq++;		
		}
		break;
	
	///< �I��
	default:
		CI_pv_disp_HitTableSet( wk );				///< �����蔻��̃e�[�u���X�V
		
		if ( wk->state.work == 0 ){
			CI_pv_disp_CurBar_PosSet( wk, CI_pv_FocusTopSerach( wk, wk->state.target ) );
		}
		else {
			CI_pv_disp_CurBar_PosSet( wk, CI_pv_FocusBottomSerach( wk, wk->state.target ) );
		}
		
		if ( wk->focus_now != 0 ){
			CI_pv_disp_CurOAM_Visible( wk, 0, TRUE );
		}
		
		CI_pv_StateInit( wk );		
		CI_pv_SeqChange( wk, eSEQ_INPUT );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	�֐��e�[�u��
 *
 * @param	CI_pv_MainTable[]	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL (* const CI_pv_MainTable[])( BR_CODEIN_WORK* wk ) = {
	CI_pv_MainInit,
	CI_pv_MainInput,
	CI_pv_MainFocusMove,
	CI_pv_MainEnd,
};


//--------------------------------------------------------------
/**
 * @brief	���C��
 *
 * @param	wk	
 *
 * @retval	BOOL	TRUE = �I��
 *
 */
//--------------------------------------------------------------
BOOL CI_pv_MainUpdate( BR_CODEIN_WORK* wk )
{
	BOOL bActive;
	
	bActive = CI_pv_MainTable[ wk->seq ]( wk );	
	
//	BMN_Main( wk->sys.btn );
	
	CI_pv_disp_CurUpdate( wk );
	
	return bActive;
}


//--------------------------------------------------------------
/**
 * @brief	�I��
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_Input_End( BR_CODEIN_WORK* wk )
{
	int i;
	u32 num = 0;
  int block_idx = 0;
  int block_keta = 0;

	
	STRBUF* str_dest = GFL_STR_CreateBuffer( 100, wk->heapID );
	
	wk->btn[ 1 ].state = 1;
	wk->btn[ 1 ].move_wk.wait = 0;
	
  wk->code_block[0] = 0;
  wk->code_block[1] = 0;
  wk->code_block[2] = 0;
  wk->code_number   = 0;
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].state == 0 )
		{
			wk->code[ i ].state = 1;
			GFL_CLACT_WK_SetAnmSeq( wk->code[ i ].clwk, CI_pv_disp_CodeAnimeGet( wk->code[ i ].state, wk->code[ i ].size ) );
		}

    //�o�b�t�@�ɐݒ�
		num = wk->code[ i ].state - 1;
		STRTOOL_SetNumber( str_dest, num, 1, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
		GFL_STR_AddString( wk->param.strbuf, str_dest );

    //�؂蕪���ɐݒ�
    wk->code_block[ block_idx ] *= 10;
    wk->code_block[ block_idx ] += num;
    block_keta++;
    if( block_keta >= wk->param.block[ block_idx ] )
    { 
      block_idx++;
      block_keta  = 0;
    }

    //�ꊇ�ɐݒ�
    wk->code_number  *= 10;
    wk->code_number  += num;
	}
	
	GFL_STR_DeleteBuffer( str_dest );
	
	CI_pv_SeqChange( wk, eSEQ_END );

  wk->select  = BR_CODEIN_SELECT_DECIDE;
}

//--------------------------------------------------------------
/**
 * @brief	�߂�
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_Input_back( BR_CODEIN_WORK* wk )
{	///< �߂�
					
	int cur_p;
	int now_g;
	int next_g;
	
	wk->btn[ 0 ].state = 1;
	wk->btn[ 0 ].move_wk.wait = 0;
	
	if ( wk->focus_now == 0 ){
		cur_p = wk->cur[ 0 ].state = wk->code_max - 1;				
		next_g = wk->code[ cur_p ].group;
		wk->state.param	 = eSTATE_FOCUS_MOVE;
		wk->state.target = next_g;
		wk->state.work	 = 1;

		return;
	}
	
	cur_p = wk->cur[ 0 ].state;				
	wk->code[ cur_p ].state = eANM_CODE_LN;
	
	GFL_CLACT_WK_SetAnmSeq( wk->code[ cur_p ].clwk, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
									
	now_g = wk->code[ cur_p ].group;
	if ( cur_p > 0 ){						 
		cur_p--;
		GFL_CLACT_WK_SetAnmSeq( wk->code[ cur_p ].clwk, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
		
		next_g = wk->code[ cur_p ].group;
		
		if ( now_g != next_g ){
			
			wk->state.param	 = eSTATE_FOCUS_MOVE;
			wk->state.target = next_g;
			wk->state.work	 = 1;
		}
		else {
			
			wk->state.param	 = eSTATE_CUR_MOVE;
			wk->state.target = cur_p;
		}
	}
  else
  { 
    wk->select =BR_CODEIN_SELECT_CANCEL;
  }
}


//--------------------------------------------------------------
/**
 * @brief	�{�^���p�}�l�[�W���[�ݒ�
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_ButtonManagerInit( BR_CODEIN_WORK* wk )
{
	int i;
	
	///< �R�[�h�̓����蔻��
	for ( i = 0; i < eHRT_CODE_11 + 1; i ++ ){
				
		wk->code[ i ].hit = &wk->sys.rht[ i ];
	}
	
	///< �{�^�������̓����蔻��
	{
		///< x, y, sx, sy,
		const s16 tbl[][ 4 ] = {
			// 0 - 4
			{  32,  80, 20, 20 },{  80,  80, 20, 20 },{ 128,  80, 20, 20 },{ 176,  80, 20, 20 },{ 224,  80, 20, 20 },
			
			// 5 - 9
			{  32, 128, 20, 20 },{  80, 128, 20, 20 },{ 128, 128, 20, 20 },{ 176, 128, 20, 20 },{ 224, 128, 20, 20 },
			
			// back - end
			{  80, 168, 48, 16 },{ 176, 168, 48, 16 },
		};

		for ( ; i < eHRT_MAX; i++ ){
			
			wk->sys.rht[ i ].rect.top		= tbl[ i - eHRT_NUM_0 ][ 1 ] - tbl[ i - eHRT_NUM_0 ][ 3 ];
			wk->sys.rht[ i ].rect.left		= tbl[ i - eHRT_NUM_0 ][ 0 ] - tbl[ i - eHRT_NUM_0 ][ 2 ];
			wk->sys.rht[ i ].rect.bottom	= tbl[ i - eHRT_NUM_0 ][ 1 ] + tbl[ i - eHRT_NUM_0 ][ 3 ];
			wk->sys.rht[ i ].rect.right		= tbl[ i - eHRT_NUM_0 ][ 0 ] + tbl[ i - eHRT_NUM_0 ][ 2 ];
		}	
	}

	wk->sys.rht[ eHRT_MAX ].circle.code = GFL_UI_TP_HIT_END;

	wk->sys.btn = GFL_BMN_Create( wk->sys.rht,
							  CI_pv_ButtonManagerCallBack, wk, wk->heapID );
}


//--------------------------------------------------------------
/**
 * @brief	�{�^���p�}�l�[�W���[�R�[���o�b�N
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_ButtonManagerCallBack( u32 button, u32 event, void* work )
{
	BR_CODEIN_WORK* wk = work;
	
	///< ���͂���Ȃ���Δ�΂�
	if ( wk->seq != eSEQ_INPUT ){ return; }
	
	///< Touch ���ᖳ����� Touch �ɂ��Ĕ�΂�
/*	if ( wk->sys.touch != TRUE ){
		 wk->sys.touch = TRUE;
	}
*/	
	if ( event == GFL_BMN_EVENT_TOUCH ){
		
		///< �������͕����������ꂽ
		if ( button >= eHRT_CODE_0 && button <= eHRT_CODE_11 ){

			CIPV_Printf( "button = %3d\n", button );
			
			if ( wk->code[ button ].size == TRUE ){
				///< ���͕����̈ړ�
				
				wk->state.param		= eSTATE_CUR_MOVE;
				wk->state.target	= button;
			}
			else {
				///< �g�k�J�����g�ړ�����
				
				wk->state.param		= eSTATE_FOCUS_MOVE;
				wk->state.target	= wk->code[ button ].group;
			}
      PMSND_PlaySE( BR_SND_SE_OK );

		}
		///< ���̑��̃{�^��
		else {
			
			///< �L�[�ʒu�ɂ��ϊ�
			if ( button == eHRT_BACK ){
				wk->cur[ 1 ].move_wk.pos.x = 0;
				wk->cur[ 1 ].move_wk.pos.y = 2;
        PMSND_PlaySE( BR_SND_SE_OK );

        { 
          u32 x, y;
          GFL_POINT pos;
          GFL_UI_TP_GetPointTrg( &x, &y );
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( wk->param.p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );
        }
			}
			else if ( button == eHRT_END ){
				wk->cur[ 1 ].move_wk.pos.x = 3;
        wk->cur[ 1 ].move_wk.pos.y = 2;
        PMSND_PlaySE( BR_SND_SE_OK );

        { 
          u32 x, y;
          GFL_POINT pos;
          GFL_UI_TP_GetPointTrg( &x, &y );
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( wk->param.p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );
        }
			}
			else {
				wk->cur[ 1 ].move_wk.pos.x = ( button - eHRT_NUM_0 ) % 5;
				wk->cur[ 1 ].move_wk.pos.y = ( button - eHRT_NUM_0 ) / 5;
        PMSND_PlaySE( BR_SND_SE_OK );
			}
			
			///< �����p�l��
			if ( button >= eHRT_NUM_0 && button <= eHRT_NUM_9 ){
				
				int cur_p;
				int now_g;
				int next_g;
				
				if ( wk->focus_now == 0 ){ 
          return;
        }  
        
        { 
          u32 x, y;
          GFL_POINT pos;
          GFL_UI_TP_GetPointTrg( &x, &y );
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( wk->param.p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );
        }
				
				cur_p = wk->cur[ 0 ].state;				
				wk->code[ cur_p ].state = button - eHRT_NUM_0 + 1;
				
				GFL_CLACT_WK_SetAnmSeq( wk->code[ cur_p ].clwk, CI_pv_disp_CodeAnimeGet( wk->code[ cur_p ].state, wk->code[ cur_p ].size ) );
				
				///< �J�[�\��
      #if 0
        CI_pv_disp_CurOAM_Visible( wk, 1, TRUE );
				CI_pv_disp_CurSQ_PosSet( wk, button - eHRT_NUM_0 );
				CI_pv_disp_CurOAM_Visible( wk, 1, FALSE );
				CI_pv_disp_CurOAM_Visible( wk, 2, TRUE );
				CI_pv_disp_CurSQ_PosSetEx( wk, button - eHRT_NUM_0, 2 );
				GFL_CLACT_WK_SetAnmSeq( wk->cur[ 2 ].clwk, eANM_CUR_TOUCH );
			#endif		
				now_g = wk->code[ cur_p ].group;
				cur_p++;
				if ( cur_p == wk->code_max ){
										
					wk->state.param	 = eSTATE_FOCUS_MOVE;
					wk->state.target = 0;
					wk->state.work	 = 0;
				}
				else {					
					next_g = wk->code[ cur_p ].group;
					
					if ( now_g != next_g ){
						
						wk->state.param	 = eSTATE_FOCUS_MOVE;
						wk->state.target = next_g;
						wk->state.work	 = 0;
					}
					else {
						
						wk->state.param	 = eSTATE_CUR_MOVE;
						wk->state.target = cur_p;
					}
				}
			}
			else {
				///< back End
				
				if ( button == eHRT_BACK ){
					///< �߂�
					CI_pv_Input_back( wk );
				}
				else {
					///< ����
					CI_pv_Input_End( wk );					
				}
			}
		}
	}
}


//--------------------------------------------------------------
/**
 * @brief	��Ԃ̍X�V����
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_StateUpdate( BR_CODEIN_WORK* wk )
{
	switch ( wk->state.param ){
	
	case eSTATE_INPUT:
		break;
	
	///< �t�H�[�J�X���ړ�����
	case eSTATE_FOCUS_MOVE:
		CI_pv_FocusSet( wk, wk->state.target );				///< ���̃t�H�[�J�X�ݒ�
		CI_pv_disp_SizeFlagSet( wk );						///< �T�C�Y�ݒ�
		CI_pv_disp_MovePosSet( wk, eMPS_MOVE_SET );			///< �ړ��n�ݒ�
		
		CI_pv_SeqChange( wk, eSEQ_FOCUS_MOVE );		
		wk->state.param	= eSTATE_BLANK;
		break;
	
	///< ���͈ʒu���ړ�����
	case eSTATE_CUR_MOVE:
		
		CI_pv_disp_CurBar_PosSet( wk, wk->state.target );	///< �J�[�\���ʒu��ύX
		CI_pv_StateInit( wk );
		break;
	
	case eSTATE_BLANK:
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	��Ԃ̏�����
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CI_pv_StateInit( BR_CODEIN_WORK* wk )
{	
	wk->state.param		= eSTATE_INPUT;
	wk->state.target	= 0;	
	wk->state.work	 	= 0;
}


//--------------------------------------------------------------
/**
 * @brief	�Ώۃt�H�[�J�X�̐擪�u���b�N���擾
 *
 * @param	wk	
 * @param	next	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int CI_pv_FocusTopSerach( BR_CODEIN_WORK* wk, int next )
{
	int i;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].group == next ){
			
			return i;
		}
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�Ώۃt�H�[�J�X�̍Ō���u���b�N���擾
 *
 * @param	wk	
 * @param	next	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int CI_pv_FocusBottomSerach( BR_CODEIN_WORK* wk, int next )
{
	int i;
	int block = 0;
	int search = 0;
	
	for ( i = 0; i < wk->code_max; i++ ){
		
		if ( wk->code[ i ].group == next ){
			
			search = 1;
		}
		else {
			
			if ( search == 1 ){
				
				return i - 1;
			}
		}
	}
	
	return wk->code_max - 1;
}
