//======================================================================
/**
 * @file  musical_script_command.h.h
 * @brief �~���[�W�J���X�N���v�g�p�̃}�N����`�t�@�C��
 * @author  Nobuhiko Ariizumi
 * @date  2009/04/08
 */
//======================================================================

#ifndef MUSICAL_SCRIPT_COMMAND_H__
#define MUSICAL_SCRIPT_COMMAND_H__


#endif MUSICAL_SCRIPT_COMMAND_H__

//���߃V���{���錾
//COMMAND_START
#define SCRIPT_ENUM_ScriptFinish    (0)
#define SCRIPT_ENUM_FrameWait       (1)
#define SCRIPT_ENUM_FrameWaitTime     (2)
#define SCRIPT_ENUM_SyncScript      (3)
#define SCRIPT_ENUM_CurtainUp       (4)
#define SCRIPT_ENUM_CurtainDown     (5)
#define SCRIPT_ENUM_CurtainMove     (6)
#define SCRIPT_ENUM_StageMove       (7)
#define SCRIPT_ENUM_StageChangeBg     (8)
#define SCRIPT_ENUM_PokeShow      (9)
#define SCRIPT_ENUM_PokeDir       (10)
#define SCRIPT_ENUM_PokeMove      (11)
#define SCRIPT_ENUM_PokeMoveOffset    (12)
#define SCRIPT_ENUM_PokeStopAnime     (13)
#define SCRIPT_ENUM_PokeStartAnime    (14)
#define SCRIPT_ENUM_PokeChangeAnime   (15)
#define SCRIPT_ENUM_PokeActionJump    (16)
#define SCRIPT_ENUM_PokeMngSetFlag    (17)
#define SCRIPT_ENUM_ObjectCreate    (18)
#define SCRIPT_ENUM_ObjectDelete    (19)
#define SCRIPT_ENUM_ObjectShow      (20)
#define SCRIPT_ENUM_ObjectHide      (21)
#define SCRIPT_ENUM_ObjectMove      (22)
#define SCRIPT_ENUM_EffectCreate    (23)
#define SCRIPT_ENUM_EffectDelete    (24)
#define SCRIPT_ENUM_EffectStart     (25)
#define SCRIPT_ENUM_EffectStop      (26)
#define SCRIPT_ENUM_EffectRepeatStart (27)
#define SCRIPT_ENUM_LightShowCircle   (28)
#define SCRIPT_ENUM_LightHide       (29)
#define SCRIPT_ENUM_LightMove       (30)
#define SCRIPT_ENUM_LightMoveTrace    (31)
#define SCRIPT_ENUM_LightColor      (32)
#define SCRIPT_ENUM_MessageShow     (33)
#define SCRIPT_ENUM_MessageHide     (34)
#define SCRIPT_ENUM_MessageColor    (35)
#define SCRIPT_ENUM_MessageColor    (35)
#define SCRIPT_ENUM_BgmStart        (36)
#define SCRIPT_ENUM_BgmStop         (37)
#define SCRIPT_ENUM_PokeActionRotate    (38)
#define SCRIPT_ENUM_PokeSetFrontBack    (39)
#define SCRIPT_ENUM_PokeDispItem    (40)
#define SCRIPT_ENUM_PokeTransEffect    (41)
#define SCRIPT_ENUM_PokeAttentionOn    (42)
#define SCRIPT_ENUM_PokeAttentionOff   (43)
#define SCRIPT_ENUM_StageStartMainPart   (44)
#define SCRIPT_ENUM_StageFinishMainPart   (45)
#define SCRIPT_ENUM_PokeActionComeNearToTop   (46)
#define SCRIPT_ENUM_Label   (47)
#define SCRIPT_ENUM_SeqBgmStart  (48)
#define SCRIPT_ENUM_SeqBgmStop   (49)
#define SCRIPT_ENUM_PokeAppealScript (50)
#define SCRIPT_ENUM_PokeSubScript (51)
#define SCRIPT_ENUM_BgmSetDataLink (52)
#define SEQ_END             (53)

#ifndef __C_NO_DEF_

//����Ƀp�f�B���O�������Ȃ��悤�ɂ���
  .option alignment off
  
#pragma mark [>System Command
//���߃}�N����`
//======================================================================
/**
 * @brief �V�X�e���F�~���[�W�J���I��
 *
 * #param_num 0
 */
//======================================================================
  .macro  ComScriptFinish
  .short  SCRIPT_ENUM_ScriptFinish
  .endm

//======================================================================
/**
 * @brief �V�X�e���F�w��t���[���E�F�C�g
 *
 * #param_num 1
 * @param frame �t���[����
 *
 * #param VALUE_INT
 */
//======================================================================
  .macro  ComFrameWait  frame
  .short  SCRIPT_ENUM_FrameWait
  .long \frame
  .endm

//======================================================================
/**
 * @brief �V�X�e���F�w��܂Ńt���[���E�F�C�g
 *
 * #param_num 1
 * @param frame �t���[����
 *
 * #param VALUE_INT
 */
//======================================================================
  .macro  ComFrameWaitTime  frame
  .short  SCRIPT_ENUM_FrameWaitTime
  .long \frame
  .endm

//======================================================================
/**
 * @brief �V�X�e���F�X�N���v�g�S�̂œ��������
 *
 * #param_num 1
 * @param comment �R�����g(��
 *
 * #param VALUE_INT
 *
 */
//======================================================================
  .macro  ComSyncScript comment
  .short  SCRIPT_ENUM_SyncScript
  .long \comment
  .endm



#pragma mark [>Curtain Command
//======================================================================
/**
 * @brief �J�[�e���F�����グ��(�Œ葬�x
 *
 * #param_num 1
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComCurtainUp autoWait
  .short  SCRIPT_ENUM_CurtainUp
  .long   \autoWait
  .endm

//======================================================================
/**
 * @brief �J�[�e���F����������(�Œ葬�x
 *
 * #param_num 1
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComCurtainDown autoWait
  .short  SCRIPT_ENUM_CurtainDown
  .long   \autoWait
  .endm

//======================================================================
/**
 * @brief �J�[�e���F���𓮂���(���x�ݒ��
 *
 * #param_num 3
 * @param frame �t���[����
 * @param heaight Y���W
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT frame
 * #param VALUE_INT posY
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComCurtainMove  frame height autoWait
  .short  SCRIPT_ENUM_CurtainMove
  .long \frame
  .long \height
  .long   \autoWait
  .endm

#pragma mark [>Stage Command
//======================================================================
/**
 * @brief �X�e�[�W�F����̃X�N���[��
 *
 * #param_num 3
 * @param frame �t���[����
 * @param pos   X���W
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT frame
 * #param VALUE_INT posX
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComStageMove  frame pos autoWait
  .short  SCRIPT_ENUM_StageMove
  .long \frame
  .long \pos
  .long   \autoWait
  .endm

//======================================================================
/**
 * @brief �X�e�[�W�F�w�i�̓ǂݑւ�
 *
 * #param_num 1
 * @param bgNo  �w�i�ԍ�
 *
 * #param VALUE_INT bgNo
 */
//======================================================================
  .macro  ComStageChangeBg  bgNo
  .short  SCRIPT_ENUM_StageChangeBg
  .long \bgNo
  .endm

//======================================================================
/**
 * @brief �X�e�[�W�F���C���p�[�g�̊J�n
 *
 * #param_num 0
 *
 */
//======================================================================
  .macro  ComStageStartMainPart
  .short  SCRIPT_ENUM_StageStartMainPart
  .endm

//======================================================================
/**
 * @brief �X�e�[�W�F���C���p�[�g�̏I��
 *
 * #param_num 0
 *
 */
//======================================================================
  .macro  ComStageFinishMainPart
  .short  SCRIPT_ENUM_StageFinishMainPart
  .endm

#pragma mark [>Pokemon Command
//======================================================================
/**
 * @brief �|�P�����F�\���ؑ�
 *
 * #param_num 2
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param flg   ON/OFF
 *
 * #param VALUE_INT pokeNo
 * #param COMBOBOX_TEXT OFF ON
 * #param COMBOBOX_VALUE  0 1
 */
//======================================================================
  .macro  ComPokeShow pokeNo  flg
  .short  SCRIPT_ENUM_PokeShow
  .long \pokeNo
  .long \flg
  .endm

//======================================================================
/**
 * @brief �|�P�����F�����ݒ�
 *
 * #param_num 2
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param dir   ����(���F�E)
 *
 * #param VALUE_INT pokeNo
 * #param COMBOBOX_TEXT ��  �E
 * #param COMBOBOX_VALUE  0 1
 */
//======================================================================
  .macro  ComPokeDir  pokeNo  dir
  .short  SCRIPT_ENUM_PokeDir
  .long \pokeNo
  .long \dir
  .endm

//======================================================================
/**
 * @brief �|�P�����F�ړ�
 *
 * #param_num 4
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param frame �t���[����
 * @param pos   ���W
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT frame
 * #param VALUE_VECFX32 posX posY posZ
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComPokeMove pokeNo  frame posX  posY  posZ autoWait
  .short  SCRIPT_ENUM_PokeMove
  .long \pokeNo
  .long \frame
  .long \posX
  .long \posY
  .long \posZ
  .long   \autoWait
 .endm

//======================================================================
/**
 * @brief �|�P�����F�ړ�(���΍��W)
 *
 * #param_num 4
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param frame �t���[����
 * @param offset  �I�t�Z�b�g
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT frame
 * #param VALUE_VECFX32 offsetX offsetY offsetZ
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComPokeMoveOffset pokeNo  frame offsetX offsetY offsetZ autoWait
  .short  SCRIPT_ENUM_PokeMoveOffset
  .long \pokeNo
  .long \frame
  .long \offsetX
  .long \offsetY
  .long \offsetZ
  .long   \autoWait
  .endm

//======================================================================
/**
 * @brief �|�P�����F�A�j����~
 *
 * #param_num 1
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 *
 * #param VALUE_INT pokeNo
 */
//======================================================================
  .macro  ComPokeStopAnime  pokeNo
  .short  SCRIPT_ENUM_PokeStopAnime
  .long \pokeNo
  .endm

//======================================================================
/**
 * @brief �|�P�����F�A�j���J�n
 *
 * #param_num 1
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 *
 * #param VALUE_INT pokeNo
 */
//======================================================================
  .macro  ComPokeStartAnime pokeNo
  .short  SCRIPT_ENUM_PokeStartAnime
  .long \pokeNo
  .endm

//======================================================================
/**
 * @brief �|�P�����F�A�j���ύX
 *
 * #param_num 2
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param animeNo �A�j���ԍ�
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT animeNo
 */
//======================================================================
  .macro  ComPokeChangeAnime  pokeNo  animeNo
  .short  SCRIPT_ENUM_PokeChangeAnime
  .long \pokeNo
  .long \animeNo
  .endm
  
//======================================================================
/**
 * @brief �|�P�����F���ʁE�w�ʐ؂�ւ�
 *
 * #param_num 2
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param dir   ����(���ʁF�w��)
 *
 * #param VALUE_INT pokeNo
 * #param COMBOBOX_TEXT ����  �w��
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComPokeSetFrontBack  pokeNo  dir
  .short  SCRIPT_ENUM_PokeSetFrontBack
  .long \pokeNo
  .long \dir
  .endm
  
//======================================================================
/**
 * @brief �|�P�����F�����i�\���؂�ւ�
 *
 * #param_num 2
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param flg   ON/OFF
 *
 * #param VALUE_INT pokeNo
 * #param COMBOBOX_TEXT OFF ON
 * #param COMBOBOX_VALUE  0 1
 */
//======================================================================
  .macro  ComPokeSetDispItem pokeNo  flg
  .short  SCRIPT_ENUM_PokeDispItem
  .long \pokeNo
  .long \flg
  .endm
  
//======================================================================
/**
 * @brief �|�P�����F�ϐg�G�t�F�N�g�Đ�
 *
 * #param_num 1
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 *
 * #param VALUE_INT pokeNo
 */
//======================================================================
  .macro  ComPokeTransEffect pokeNo
  .short  SCRIPT_ENUM_PokeTransEffect
  .long \pokeNo
  .endm

//======================================================================
/**
 * @brief �|�P�����F���ڃ|�P�����ݒ�(�����O�ɘA�����ČĂׂ܂�
 *
 * #param_num 1
 * @param pokeNo  �|�P�����ԍ�(-1�s��)
 *
 * #param VALUE_INT pokeNo
 */
//======================================================================
  .macro  ComPokeAttentionOn pokeNo
  .short  SCRIPT_ENUM_PokeAttentionOn
  .long \pokeNo
  .endm

//======================================================================
/**
 * @brief �|�P�����F���ڃ|�P��������
 *
 * #param_num 0
 *
 */
//======================================================================
  .macro  ComPokeAttentionOff
  .short  SCRIPT_ENUM_PokeAttentionOff
  .endm

//======================================================================
/**
 * @brief �|�P�����F�A�s�[���X�N���v�g���s
 *
 * #param_num 2
 * @param pokeNo �|�P�����ԍ�(-1�s��)
 * @param scriptNo �X�N���v�g�ԍ�
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT scriptNo
 */
//======================================================================
  .macro  ComPokeAppealScript pokeNo scriptNo
  .short  SCRIPT_ENUM_PokeAppealScript
  .long   \pokeNo
  .long   \scriptNo
  .endm


//======================================================================
/**
 * @brief �|�P�����F�T�u�X�N���v�g���s
 *
 * #param_num 2
 * @param pokeNo �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param scriptNo �X�N���v�g�ԍ�
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT scriptNo
 */
//======================================================================
  .macro  ComPokeSubScript pokeNo scriptNo 
  .short  SCRIPT_ENUM_PokeSubScript
  .long   \pokeNo
  .long   \scriptNo
  .endm


#pragma mark [>Pokemon Action Command
//======================================================================
/**
 * @brief �|�P�����A�N�V�����F���˂�
 *
 * #param_num 5
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param interval  �Ԋu
 * @param repeat  ��
 * @param height  ����
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT interval
 * #param VALUE_INT repeat
 * #param VALUE_FX32
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComPokeActionJump pokeNo  interval  repeat  height autoWait
  .short  SCRIPT_ENUM_PokeActionJump
  .long \pokeNo
  .long \interval
  .long \repeat
  .long \height
  .long   \autoWait
  .endm

//======================================================================
/**
 * @brief �|�P�����A�N�V�����F���
 *
 * #param_num 5
 * @param pokeNo  �|�P�����ԍ�(-1�Ŏ��O�o�^�Ώ�)
 * @param frame   �t���[����
 * @param angle   �J�n�p�x(360�x�v�Z
 * @param angle   ��]�p�x(�}�C�i�X�ŋt��]�E720��2��]
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT pokeNo
 * #param VALUE_INT frame
 * #param VALUE_INT startAngle
 * #param VALUE_INT rotateAngle
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComPokeActionRotate pokeNo  frame  startAngle rotateAngle autoWait
  .short  SCRIPT_ENUM_PokeActionRotate
  .long \pokeNo
  .long \frame
  .long \startAngle
  .long \rotateAngle
  .long   \autoWait
  .endm

//======================================================================
/**
 * @brief �|�P�����A�N�V�����F�g�b�v�Ɋ��
 *
 * #param_num 2
 * @param frame   �t���[����
 * @param autoWait �����t���[���E�F�C�g
 *
 * #param VALUE_INT frame
 * #param COMBOBOX_TEXT ON OFF
 * #param COMBOBOX_VALUE  1 0
 */
//======================================================================
  .macro  ComPokeActionComeNearToTop frame autoWait
  .short  SCRIPT_ENUM_PokeActionComeNearToTop
  .long \frame
  .long   \autoWait
  .endm
  

#pragma mark [>Pokemon Manage Command
//======================================================================
/**
 * @brief �|�P�����Ǘ��F����ԍ��ݒ�
 *
 * #param_num 4
 * @param flg1  �|�P�����P�̃t���O
 * @param flg2  �|�P�����Q�̃t���O
 * @param flg3  �|�P�����R�̃t���O
 * @param flg4  �|�P�����S�̃t���O
 *
 * #param COMBOBOX_TEXT OFF ON
 * #param COMBOBOX_VALUE  0 1
 * #param COMBOBOX_TEXT OFF ON
 * #param COMBOBOX_VALUE  0 1
 * #param COMBOBOX_TEXT OFF ON
 * #param COMBOBOX_VALUE  0 1
 * #param COMBOBOX_TEXT OFF ON
 * #param COMBOBOX_VALUE  0 1
 */
//======================================================================
  .macro  ComPokeMngSetFlg  flg1  flg2  flg3  flg4
  .short  SCRIPT_ENUM_PokeMngSetFlag
  .long \flg1
  .long \flg2
  .long \flg3
  .long \flg4
  .endm


#pragma mark [>Object Command
//======================================================================
/**
 * @brief �I�u�W�F�N�g�F�쐬
 *
 * #param_num 2
 * @param objNo �I�u�W�F�N�g�Ǘ��ԍ�
 * @param type  �I�u�W�F�N�g�ԍ�
 *
 * #param VALUE_INT objNo
 * #param VALUE_INT objType
 */
//======================================================================
  .macro  ComObjectCreate objNo type
  .short  SCRIPT_ENUM_ObjectCreate
  .long \objNo
  .long \type
  .endm

//======================================================================
/**
 * @brief �I�u�W�F�N�g�F�폜
 *
 * #param_num 1
 * @param objNo �I�u�W�F�N�g�Ǘ��ԍ�
 *
 * #param VALUE_INT objNo
 */
//======================================================================
  .macro  ComObjectDelete objNo
  .short  SCRIPT_ENUM_ObjectDelete
  .long \objNo
  .endm

//======================================================================
/**
 * @brief �I�u�W�F�N�g�F�\��
 *
 * #param_num 1
 * @param objNo �I�u�W�F�N�g�Ǘ��ԍ�
 *
 * #param VALUE_INT objNo
 */
//======================================================================
  .macro  ComObjectShow objNo
  .short  SCRIPT_ENUM_ObjectShow
  .long \objNo
  .endm

//======================================================================
/**
 * @brief �I�u�W�F�N�g�F��\��
 *
 * #param_num 1
 * @param objNo �I�u�W�F�N�g�Ǘ��ԍ�
 *
 * #param VALUE_INT objNo
 */
//======================================================================
  .macro  ComObjectHide objNo
  .short  SCRIPT_ENUM_ObjectHide
  .long \objNo
  .endm

//======================================================================
/**
 * @brief �I�u�W�F�N�g�F�ړ�
 *
 * #param_num 3
 * @param objNo �I�u�W�F�N�g�Ǘ��ԍ�
 * @param frame �t���[����
 * @param pos   ���W
 *
 * #param VALUE_INT objNo
 * #param VALUE_INT frame
 * #param VALUE_VECFX32 posX posY posZ
 */
//======================================================================
  .macro  ComObjectMove objNo frame posX  posY  posZ
  .short  SCRIPT_ENUM_ObjectMove
  .long \objNo
  .long \frame
  .long \posX
  .long \posY
  .long \posZ
  .endm


#pragma mark [>Effect Command
//======================================================================
/**
 * @brief �G�t�F�N�g�F�쐬
 *
 * #param_num 2
 * @param effNo �G�t�F�N�g�Ǘ��ԍ�
 * @param type  �G�t�F�N�g�ԍ�
 *
 * #param VALUE_INT effectNo
 * #param VALUE_INT effectType
 */
//======================================================================
  .macro  ComEffectCreate effNo type
  .short  SCRIPT_ENUM_EffectCreate
  .long \effNo
  .long \type
  .endm

//======================================================================
/**
 * @brief �G�t�F�N�g�F�j��
 *
 * #param_num 1
 * @param effNo �G�t�F�N�g�Ǘ��ԍ�
 *
 * #param VALUE_INT effectNo
 */
//======================================================================
  .macro  ComEffectDelete effNo
  .short  SCRIPT_ENUM_EffectDelete
  .long \effNo
  .endm

//======================================================================
/**
 * @brief �G�t�F�N�g�F�Đ�
 *
 * #param_num 3
 * @param effNo �G�t�F�N�g�Ǘ��ԍ�
 * @param emitNo  �G�~�b�^�ԍ�
 * @param pos   ���W
 *
 * #param VALUE_INT effectNo
 * #param VALUE_INT emmiterNo
 * #param VALUE_VECFX32 posX posY posZ
 */
//======================================================================
  .macro  ComEffectStart  effNo emitNo  posX  posY  posZ
  .short  SCRIPT_ENUM_EffectStart
  .long \effNo
  .long \emitNo
  .long \posX
  .long \posY
  .long \posZ
  .endm

//======================================================================
/**
 * @brief �G�t�F�N�g�F��~
 *
 * #param_num 2
 * @param effNo �G�t�F�N�g�Ǘ��ԍ�
 * @param emitNo  �G�~�b�^�ԍ�
 *
 * #param VALUE_INT effectNo
 * #param VALUE_INT emmiterNo
 */
//======================================================================
  .macro  ComEffectStop effNo emitNo
  .short  SCRIPT_ENUM_EffectStop
  .long \effNo
  .long \emitNo
  .endm

//======================================================================
/**
 * @brief �G�t�F�N�g�F�A���Đ�
 *
 * #param_num 6
 * @param effNo �G�t�F�N�g�Ǘ��ԍ�
 * @param emitNo  �G�~�b�^�ԍ�
 * @param interval  �Ԋu
 * @param repeat  ��
 * @param startPos  �J�n���W
 * @param endPos  �I�����W(�K���J�n���W���������Ȃ�悤�ɂ��Ă�������
 *
 * #param VALUE_INT effectNo
 * #param VALUE_INT emmiterNo
 * #param VALUE_INT interval
 * #param VALUE_INT repeat
 * #param VALUE_VECFX32 startPosX startPosY startPosZ
 * #param VALUE_VECFX32 endPosX endPosY endPosZ
 */
//======================================================================
  .macro  ComEffectRepeatStart  effNo emitNo  interval  repeat  posX1 posY1 posZ1 posX2 posY2 posZ2
  .short  SCRIPT_ENUM_EffectRepeatStart
  .long \effNo
  .long \emitNo
  .long \interval
  .long \repeat
  .long \posX1
  .long \posY1
  .long \posZ1
  .long \posX2
  .long \posY2
  .long \posZ2
  .endm

#pragma mark [>Light Command
//======================================================================
/**
 * @brief �X�|�b�g���C�g�F�쐬
 *
 * #param_num 2
 * @param lightNo �X�|�b�g���C�g�Ǘ��ԍ�
 * @param radian  ���a(������)
 *
 * #param VALUE_INT lightNo
 * #param VALUE_INT radian(invalid)
 */
//======================================================================
  .macro  ComLightShowCircle  lightNo radian
  .short  SCRIPT_ENUM_LightShowCircle
  .long \lightNo
  .long \radian
  .endm

//======================================================================
/**
 * @brief �X�|�b�g���C�g�F�j��
 *
 * #param_num 1
 * @param lightNo �X�|�b�g���C�g�Ǘ��ԍ�
 *
 * #param VALUE_INT lightNo
 */
//======================================================================
  .macro  ComLightHide  lightNo
  .short  SCRIPT_ENUM_LightHide
  .long \lightNo
  .endm

//======================================================================
/**
 * @brief �X�|�b�g���C�g�F�ړ�
 *
 * #param_num 3
 * @param lightNo �X�|�b�g���C�g�Ǘ��ԍ�
 * @param frame �t���[����
 * @param pos   ���W(Z����)
 *
 * #param VALUE_INT lightNo
 * #param VALUE_INT frame
 * #param VALUE_VECFX32 posX posY posZ(invalid!)
 */
//======================================================================
  .macro  ComLightMove  lightNo frame posX  posY  posZ
  .short  SCRIPT_ENUM_LightMove
  .long \lightNo
  .long \frame
  .long \posX
  .long \posY
  .long \posZ
  .endm

//======================================================================
/**
 * @brief �X�|�b�g���C�g�F�|�P�����Ǐ]�ړ�
 *
 * #param_num 4
 * @param lightNo �X�|�b�g���C�g�Ǘ��ԍ�
 * @param pokeNo  �|�P�����ԍ�(-1�s��)
 * @param frame �t���[����
 * @param ofs   �I�t�Z�b�g(Z����)
 *
 * #param VALUE_INT lightNo
 * #param VALUE_INT pokeNo
 * #param VALUE_INT frame
 * #param VALUE_VECFX32 offsetX offsetY offsetZ(invalid!)
 */
//======================================================================
  .macro  ComLightMoveTrace lightNo pokeNo  frame ofsX  ofsY  ofsZ
  .short  SCRIPT_ENUM_LightMoveTrace
  .long \lightNo
  .long \pokeNo
  .long \frame
  .long \ofsX
  .long \ofsY
  .long \ofsZ
  .endm

//======================================================================
/**
 * @brief �X�|�b�g���C�g�F�F�ݒ�
 *
 * #param_num 5
 * @param lightNo �X�|�b�g���C�g�Ǘ��ԍ�
 * @param colR  �F�F�q
 * @param colG  �F�F�f
 * @param colB  �F�F�a
 * @param alpha �����x
 *
 * #param VALUE_INT lightNo
 * #param VALUE_INT colR
 * #param VALUE_INT colG
 * #param VALUE_INT colB
 * #param VALUE_INT alpha
 */
//======================================================================
  .macro  ComLightColor lightNo colR  colG  colB  alpha
  .short  SCRIPT_ENUM_LightColor
  .long \lightNo
  .long \colR
  .long \colG
  .long \colB
  .long \alpha
  .endm

#pragma mark [>Message Command
//======================================================================
/**
 * @brief ���b�Z�[�W�F�\��
 *
 * #param_num 2
 * @param msgNo ���b�Z�[�W�ԍ�
 * @param speed ���b�Z�[�W���x(1�����ӂ�̃t���[����)
 *
 * #param VALUE_INT messageNo
 * #param VALUE_INT messageSpeed
 */
//======================================================================
  .macro  ComMessageShow  msgNo speed
  .short  SCRIPT_ENUM_MessageShow
  .long \msgNo
  .long \speed
  .endm

//======================================================================
/**
 * @brief ���b�Z�[�W�F��\��
 *
 * #param_num 0
 *
 */
//======================================================================
  .macro  ComMessageHide  
  .short  SCRIPT_ENUM_MessageHide
  .endm

//======================================================================
/**
 * @brief ���b�Z�[�W�F�F�ύX
 *
 * #param_num 3
 * @param colLetter �����F
 * @param colShadow �e�F
 * @param colBack �w�i�F
 *
 * #param VALUE_INT colLetter
 * #param VALUE_INT colShadow
 * #param VALUE_INT colBack
 */
//======================================================================
  .macro  ComMessageColor colLetter colShadow colBack
  .short  SCRIPT_ENUM_MessageColor
  .long \colLetter
  .long \colShadow
  .long \colBack
  .endm

#pragma mark [>Music Command
//======================================================================
/**
 * @brief BGM�F�Đ�
 *
 * #param_num 1
 * @param bgmNo BGM�ԍ�
 *
 * #param VALUE_INT bgmNo
 */
//======================================================================
  .macro  ComBgmStart bgmNo
  .short  SCRIPT_ENUM_BgmStart
  .long \bgmNo
  .endm

//======================================================================
/**
 * @brief BGM�F��~
 *
 * #param_num 0
 *
 */
//======================================================================
  .macro  ComBgmStop
  .short  SCRIPT_ENUM_BgmStop
  .endm
//======================================================================
/**
 * @brief BGM�F�f�[�^�֘A�t��
 *
 * #param_num 2
 * @param dstIdx Bank�̕��̔ԍ�
 * @param srcIdx Wave�̕��̔ԍ�
 *
 * #param VALUE_INT dstIdx
 * #param VALUE_INT srcIdx
 */
//======================================================================
  .macro  ComBgmSetDataLink dstIdx srcIdx
  .short  SCRIPT_ENUM_BgmSetDataLink
  .long \dstIdx
  .long \srcIdx
  .endm

//======================================================================
/**
 * @brief SEQ_BGM�F�Đ�
 *
 * #param_num 1
 * @param bgmNo BGM�ԍ�
 *
 * #param VALUE_INT bgmNo
 */
//======================================================================
  .macro  ComSeqBgmStart bgmNo
  .short  SCRIPT_ENUM_SeqBgmStart
  .long \bgmNo
  .endm

//======================================================================
/**
 * @brief SEQ_BGM�F��~
 *
 * #param_num 0
 *
 */
//======================================================================
  .macro  ComSeqBgmStop
  .short  SCRIPT_ENUM_SeqBgmStop
  .endm

#pragma mark [>Other Command
//======================================================================
/**
 * @brief ���������������x��������������
 *
 * #param_num 1
 * @param labelNo ���x���ԍ�
 *
 * #param VALUE_INT labelNo
 */
//======================================================================
  .macro  ComSEQ_Label labelNo
  .short  SCRIPT_ENUM_Label
  .long \labelNo
  .endm

//======================================================================
/**
 * @brief �~���[�W�J���I��(�_�~�[
 *
 * #param_num 0
 */
//======================================================================
  .macro  ComSEQ_END
  .short  SCRIPT_ENUM_ScriptFinish
  .endm

#endif //__C_NO_DEF_

