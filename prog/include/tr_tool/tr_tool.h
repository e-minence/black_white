
//============================================================================================
/**
 * @file	tr_tool.h
 * @bfief	�g���[�i�[�f�[�^�c�[���S
 * @author	HisashiSogabe
 * @date	05.12.14
 */
//============================================================================================

#pragma once

//�g���[�i�[���b�Z�[�W�̕\��������`
#define	TRMSG_FIGHT_START				          ( 0 )		//�퓬�J�n�O
#define	TRMSG_FIGHT_LOSE				          ( 1 )		//�퓬����
#define	TRMSG_FIGHT_AFTER				          ( 2 )		//�퓬��
#define	TRMSG_FIGHT_START_1				        ( 3 )		//�퓬�J�n�O�i2vs2�g���[�i�[�̃f�[�^1�ځj
#define	TRMSG_FIGHT_LOSE_1				        ( 4 )		//�퓬�����i2vs2�g���[�i�[�̃f�[�^1�ځj
#define	TRMSG_FIGHT_AFTER_1				        ( 5 )		//�퓬��i2vs2�g���[�i�[�̃f�[�^1�ځj
#define	TRMSG_POKE_ONE_1				          ( 6 )		//�|�P����1�́i2vs2�g���[�i�[�̃f�[�^1�ځj
#define	TRMSG_FIGHT_START_2				        ( 7 )		//�퓬�J�n�O�i2vs2�g���[�i�[�̃f�[�^2�ځj
#define	TRMSG_FIGHT_LOSE_2				        ( 8 )		//�퓬�����i2vs2�g���[�i�[�̃f�[�^2�ځj
#define	TRMSG_FIGHT_AFTER_2				        ( 9 )		//�퓬��i2vs2�g���[�i�[�̃f�[�^2�ځj
#define	TRMSG_POKE_ONE_2				          ( 10 )	//�|�P����1�́i2vs2�g���[�i�[�̃f�[�^2�ځj
#define	TRMSG_FIGHT_NONE_DN				        ( 11 )	//����A�퓬���Ȃ���
#define	TRMSG_FIGHT_NONE_D				        ( 12 )	//���A�퓬���Ȃ���
#define TRMSG_FIGHT_AFTER_RECOVER_BEFORE  ( 13 )  //�t�B�[���h�퓬��F�񕜑O
#define TRMSG_FIGHT_AFTER_RECOVER_AFTER   ( 14 )  //�t�B�[���h�퓬��F�񕜌�
#define TRMSG_FIGHT_AFTER_GIFTITEM_BEFORE ( 15 )  //�t�B�[���h�퓬��F�A�C�e���������O
#define TRMSG_FIGHT_AFTER_GIFTITEM_AFTER  ( 16 )  //�t�B�[���h�퓬��F�A�C�e�������ꂽ��
#define	TRMSG_FIGHT_FIRST_DAMAGE		      ( 17 )	//�퓬���F����|�P�����ɍŏ��̃_���[�W��^�����Ƃ�
#define	TRMSG_FIGHT_POKE_HP_HALF		      ( 18 )	//�퓬���F����|�P�����̂g�o�����ȉ�
#define	TRMSG_FIGHT_POKE_LAST			        ( 19 )	//�퓬���F����|�P�������c��P�C
#define	TRMSG_FIGHT_POKE_LAST_HP_HALF	    ( 20 )	//�퓬���F����|�P�������c��P�C�łg�o�����ȉ�
#define	TRMSG_REVENGE_FIGHT_START		      ( 21 )	//�t�B�[���h�F�Đ�F�ΐ�O
#define	TRMSG_REVENGE_FIGHT_START_1		    ( 22 )	//�t�B�[���h�F�Đ�F�ΐ�O�F�E
#define	TRMSG_REVENGE_FIGHT_START_2		    ( 23 )	//�t�B�[���h�F�Đ�F�ΐ�O�F��

#define	TRMSG_FIGHT_WIN					(100)	//�퓬�����i�퓬���ł̂ݎg�p�����j

#define TRMSG_NONE              ( -1 )  //���b�Z�[�W��������Ȃ�����

//�g���[�i�[�f�[�^���擾����Ƃ���ID��`
#define	ID_TD_data_type		    ( 0 )		//�f�[�^�^�C�v
#define	ID_TD_tr_type		      ( 1 )		//�g���[�i�[����
#define	ID_TD_fight_type	    ( 2 )		//�퓬�^�C�v�i1vs1or2vs2�j
#define	ID_TD_poke_count	    ( 3 )		//�����|�P������
#define	ID_TD_use_item1		    ( 4 )		//�g�p����1
#define	ID_TD_use_item2		    ( 5 )		//�g�p����2
#define	ID_TD_use_item3		    ( 6 )		//�g�p����3
#define	ID_TD_use_item4		    ( 7 )		//�g�p����4
#define	ID_TD_aibit			      ( 8 )		//AI�p�^�[��
#define	ID_TD_hp_recover_flag ( 9 )		//�퓬��񕜂��邩�H
#define	ID_TD_gold            ( 10 )	//�����Â����W��
#define	ID_TD_gift_item       ( 11 )  //���悷��A�C�e��

//�g���[�i�[�����|�P�����p�����[�^
#define DATATYPE_NORMAL   ( 0 )
#define DATATYPE_WAZA     ( 1 )
#define DATATYPE_ITEM     ( 2 )
#define DATATYPE_MULTI    ( 3 )

//�A�Z���u����include����Ă���ꍇ�́A���̐錾�𖳎��ł���悤��ifndef�ň͂�ł���
#ifndef	__ASM_NO_DEF_

#include "battle/battle.h"
#include "buflen.h"

typedef enum
{ 
  TD_FIGHT_TYPE_1vs1 = 0,
  TD_FIGHT_TYPE_2vs2,
  TD_FIGHT_TYPE_3vs3,
  TD_FIGHT_TYPE_ROTATION,
}TD_FIGHT_TYPE;

//�g���[�i�[�f�[�^�p�����[�^
typedef struct{
	u8  data_type;            //�f�[�^�^�C�v
	u8	tr_type;					    //�g���[�i�[����
  u8  fight_type;           //1vs1 or 2vs2 or 3vs3 or rotate 
	u8	poke_count;           //�����|�P������

	u16	use_item[4];          //�g�p����

	u32	aibit;						    //AI�p�^�[��

  u8  hp_recover_flag :1;   //�퓬��񕜂��邩�H
  u8                  :7;  
  u8  gold;                 //�����Â����W��  

  u16 gift_item;            //�퓬����炦��A�C�e��
}TRAINER_DATA;

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�m�[�}���j
typedef	struct
{
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
}POKEDATA_TYPE_NORMAL;

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�Z�j
typedef	struct
{
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		waza[4];		//�����Ă�Z
}POKEDATA_TYPE_WAZA;

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�A�C�e���j
typedef	struct
{
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		itemno;			//�Z�b�g����A�C�e��
}POKEDATA_TYPE_ITEM;

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�}���`�j
typedef	struct
{
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		itemno;			//�Z�b�g����A�C�e��
	u16		waza[ PTL_WAZA_MAX ];		//�����Ă�Z
}POKEDATA_TYPE_MULTI;

extern	u32		TT_TrainerDataParaGet( int tr_id, int id );
extern	BOOL	TT_TrainerMessageCheck( int tr_id, int kindID, HEAPID heapID );
extern	void	TT_TrainerMessageGet( int tr_id, int msgID, STRBUF* msg, HEAPID heapID );
extern	void	TT_TrainerDataGet( int tr_id, TRAINER_DATA* td );
extern	void	TT_TrainerPokeDataGet( int tr_id,void* tpd );
extern	u8		TT_TrainerTypeSexGet( int trtype );

extern void	TT_EncountTrainerPersonalDataMake( TrainerID tr_id, BSP_TRAINER_DATA* data, HEAPID heapID );
extern void	TT_EncountTrainerPokeDataMake( TrainerID tr_id, POKEPARTY* pparty, HEAPID heapID );

#endif	__ASM_NO_DEF_

