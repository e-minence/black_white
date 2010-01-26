//=============================================================================================
/**
 * @file  btl_sick.h
 * @brief �|�P����WB �o�g���V�X�e�� ��ɃT�[�o�v�Z�p�r�̒萔�Q
 * @author  taya
 *
 * @date  2009.07.29  �쐬
 */
//=============================================================================================
#pragma once

enum {
  BTL_CALC_BASERANK_DEFAULT = 0,

  BTL_CALC_HITRATIO_MID = 6,
  BTL_CALC_HITRATIO_MIN = 0,
  BTL_CALC_HITRATIO_MAX = 12,

  BTL_CALC_CRITICAL_MIN = 0,
  BTL_CALC_CRITICAL_MAX = 4,

// �_���[�W�v�Z�֘A
  BTL_CALC_DMG_TARGET_RATIO_PLURAL = FX32_CONST(0.75f),     ///< �����̂��Ώۃ��U�̃_���[�W�␳��
  BTL_CALC_DMG_TARGET_RATIO_NONE   = FX32_CONST(1),         ///< �Ώۂɂ��_���[�W�␳�Ȃ�
  BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE = FX32_CONST(1.5f),    ///< ���U�^�C�v���V��̑g�ݍ��킹�ŗL���ȏꍇ�̕␳��
  BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE = FX32_CONST(0.5f), ///< ���U�^�C�v���V��̑g�ݍ��킹�ŕs���ȏꍇ�̕␳��
  BTL_CALC_DMG_WEATHER_RATIO_NONE = FX32_CONST(1),            ///< ���U�^�C�v���V��̑g�ݍ��킹�ɂ��␳�Ȃ�


// ��Ԉُ폈���֘A
  BTL_NEMURI_TURN_MIN = 2,  ///< �u�˂ނ�v�ŏ��^�[����
  BTL_NEMURI_TURN_MAX = 4,  ///< �u�˂ނ�v�ő�^�[����
  BTL_NEMURI_TURN_RANGE = (BTL_NEMURI_TURN_MAX - BTL_NEMURI_TURN_MIN + 1),

  BTL_MAHI_EXE_PER = 25,        ///< �u�܂Ёv�ł��т�ē����Ȃ��m��
  BTL_MAHI_AGILITY_RATIO = 25,  ///< �u�܂Ёv���̂��΂₳������
  BTL_KORI_MELT_PER = 20,       ///< �u������v���n����m��
  BTL_MEROMERO_EXE_PER = 50,    ///< �u���������v�œ����Ȃ��m��

  BTL_DOKU_SPLIT_DENOM = 8,         ///< �u�ǂ��v�ōő�HP�̉����̂P���邩
  BTL_MOUDOKU_INC_MAX = 15,         ///< �u�ǂ��ǂ��v�Ń_���[�W�ʂ���������^�[�����ő�
  BTL_YAKEDO_SPLIT_DENOM = 8,       ///< �u�₯�ǁv�ōő�HP�̉����̂P���邩
  BTL_YAKEDO_DAMAGE_RATIO = 50,     ///< �u�₯�ǁv�ŕ����_���[�W�������闦
  BTL_MOUDOKU_SPLIT_DENOM = 16,     ///< �����ǂ����A�ő�HP�̉����̂P���邩�i��{�l=�ŏ��l�j
  BTL_MOUDOKU_COUNT_MAX = BTL_MOUDOKU_SPLIT_DENOM-1,  ///< �����ǂ����A�_���[�W�{���������܂ŃJ�E���g�A�b�v���邩

  BTL_BIND_TURN_MAX = 6,        ///< �܂����n�̌p���^�[�����ő�l


  BTL_CONF_EXE_RATIO = 30,    ///< �������̎����m��
  BTL_CONF_TURN_MIN = 2,
  BTL_CONF_TURN_MAX = 5,
  BTL_CONF_TURN_RANGE = (BTL_CONF_TURN_MAX - BTL_CONF_TURN_MIN)+1,

  BTL_KANASIBARI_EFFECTIVE_TURN = 4,  ///< �u���Ȃ��΂�v�̎����^�[����

  BTL_CALC_SICK_TURN_PERMANENT = 0xff,  ///< �p���^�[������ݒ肵�Ȃ��i�i������j�ꍇ�̎w��l

  // �V��֘A
  BTL_CALC_WEATHER_MIST_HITRATIO = FX32_CONST(75),    ///< �u����v�̎��̖������␳��
  BTL_WEATHER_TURN_DEFAULT = 5,         ///< ���U�ɂ��V��ω��̌p���^�[�����f�t�H���g�l
  BTL_WEATHER_TURN_PERMANENT = 0xff,    ///< �V��p���^�[��������

  // �Ƃ������֘A
  BTL_CALC_TOK_CHIDORI_HITRATIO = FX32_CONST(0.8f),     /// �u���ǂ肠���v�̌���������
  BTL_CALC_TOK_HARIKIRI_HITRATIO = FX32_CONST(0.8f),    /// �u�͂肫��v�̖������ω���
  BTL_CALC_TOK_FUKUGAN_HITRATIO = FX32_CONST(1.3f),     /// �u�ӂ�����v�̖����ω���
  BTL_CALC_TOK_SUNAGAKURE_HITRATIO = FX32_CONST(0.8f),  /// �u���Ȃ�����v�̖������ω���
  BTL_CALC_TOK_YUKIGAKURE_HITRATIO = FX32_CONST(0.8f),  /// �u�䂫������v�̖����ω���
  BTL_CALC_TOK_HARIKIRI_POWRATIO = FX32_CONST(1.5f),    /// �u�͂肫��v�̍U���͕ω���
  BTL_CALC_TOK_TETUNOKOBUSI_POWRATIO = FX32_CONST(1.2f),/// �u�Ă̂��Ԃ��v�̍U���͕ω���
  BTL_CALC_TOK_SUTEMI_POWRATIO = FX32_CONST(1.2f),      /// �u���Ă݁v�̍U���͕ω���
  BTL_CALC_TOK_PLUS_POWRATIO = FX32_CONST(1.5f),        /// �u�v���X�v�̍U���͕ω���
  BTL_CALC_TOK_MINUS_POWRATIO = FX32_CONST(1.5f),       /// �u�}�C�i�X�v�̍U���͕ω���
  BTL_CALC_TOK_FLOWERGIFT_POWRATIO = FX32_CONST(1.5f),  /// �u�t�����[�M�t�g�v�̍U���͕ω���
  BTL_CALC_TOK_FLOWERGIFT_GUARDRATIO = FX32_CONST(1.5f),/// �u�t�����[�M�t�g�v�̓��h�ω���
  BTL_CALC_TOK_MORAIBI_POWRATIO = FX32_CONST(1.5f),     /// �u���炢�сv�̍U���͕ω���
  BTL_CALC_TOK_TECKNICIAN_POWRATIO = FX32_CONST(1.5f),  /// �u�e�N�j�V�����v�̍U���͕ω���
  BTL_CALC_TOK_HAYAASI_AGIRATIO = FX32_CONST(1.5f),     /// �u�͂₠���v�̂��΂₳�ω���
  BTL_CALC_TOK_SLOWSTART_ENABLE_TURN = 5,               /// �u�X���[�X�^�[�g�v�̓K�p�^�[��
  BTL_CALC_TOK_SLOWSTART_AGIRATIO = FX32_CONST(0.5f),   /// �u�X���[�X�^�[�g�v�̂��΂₳�ω���
  BTL_CALC_TOK_SLOWSTART_ATKRATIO = FX32_CONST(0.5f),   /// �u�X���[�X�^�[�g�v�̂��������ω���
  BTL_CALC_TOK_FUSIGINAUROKO_GDRATIO = FX32_CONST(1.5f),  /// �u�ӂ����Ȃ��낱�v�̂Ƃ��ڂ��ω���

  BTL_CALC_TOK_DOKUNOTOGE_PER   = 30,   ///�u�ǂ��̃g�Q�v�����m��
  BTL_CALC_TOK_HONONOKARADA_PER = 30,   ///�u�ق̂��̂��炾�v�����m��
  BTL_CALC_TOK_SEIDENKI_PER     = 30,   ///�u�����ł񂫁v�����m��
  BTL_CALC_TOK_HOUSI_PER        = 30,   ///�u�ق����v�����m��
  BTL_CALC_TOK_MEROMEROBODY_PER = 30,   ///�u���������{�f�B�v�����m��

  // �f�����␳�֘A
  BTL_CALC_AGILITY_MAX = 10000,

  // �̏d�����֘A
  BTL_POKE_WEIGHT_MIN = 1,

  // �t�B�[���h�E�i����ʓ��ւ̘A���֘A
  BTL_STANDALONE_PLAYER_CLIENT_ID = 0,
  BTL_BONUS_MONEY_MAX = 99999,
};
