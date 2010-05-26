#! ruby -Ks
# �t�@�C�����[�J�[����G�N�X�|�[�g���ꂽcsv���̘A���t�B�[���h�����𕪊����ăG�N�Z���ŉ{���ł���`�ɃR���o�[�g

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'
  require File.dirname(__FILE__) + '/../hash/wazano_hash.rb'
  require File.dirname(__FILE__) + '/../hash/item_hash.rb'
  require File.dirname(__FILE__) + '/../hash/tokusei_hash.rb'
  require 'date'

class PARA
  enum_const_set %w[
    POKENAME
    SHINKA_POKE1
    SHINKA_POKE2
    SHINKA_POKE3
    SHINKA_POKE4
    SHINKA_POKE5
    SHINKA_POKE6
    SHINKA_POKE7
    WAZA1
    WAZA2
    WAZA3
    WAZA4
    WAZA5
    WAZA6
    WAZA7
    WAZA8
    WAZA9
    WAZA10
    WAZA11
    WAZA12
    WAZA13
    WAZA14
    WAZA15
    WAZA16
    WAZA17
    WAZA18
    WAZA19
    WAZA20
    WAZA21
    WAZA22
    WAZA23
    WAZA24
    WAZA25
    ITEM1
    ITEM2
    ITEM3
    SPEABI1
    SPEABI2
    SPEABI3
    ZENKOKU_NO
    CHIHOU_NO
    SHINKA_LEVEL
    TYPE1
    TYPE2
    GROW
    EXP
    GET
    GENDER
    EGG_GROUP1
    EGG_GROUP2
    BIRTH_STEP
    FRIEND
    ESCAPE
    HP
    POW
    DEF
    AGI
    SPEPOW
    SPEDEF
    HP_EXP
    POW_EXP
    DEF_EXP
    AGI_EXP
    SPEPOW_EXP
    SPEDEF_EXP
    WAZA_LV1
    WAZA_LV2
    WAZA_LV3
    WAZA_LV4
    WAZA_LV5
    WAZA_LV6
    WAZA_LV7
    WAZA_LV8
    WAZA_LV9
    WAZA_LV10
    WAZA_LV11
    WAZA_LV12
    WAZA_LV13
    WAZA_LV14
    WAZA_LV15
    WAZA_LV16
    WAZA_LV17
    WAZA_LV18
    WAZA_LV19
    WAZA_LV20
    WAZA_LV21
    WAZA_LV22
    WAZA_LV23
    WAZA_LV24
    WAZA_LV25
    SHINKA_COND1
    SHINKA_COND2
    SHINKA_COND3
    SHINKA_COND4
    SHINKA_COND5
    SHINKA_COND6
    SHINKA_COND7
    WAZA_MACHINE
    GRA_NO
    FORM_NAME
    COLOR
    REVERSE
    OTHERFORM
    RANK
    PLTT_ONLY
    HEIGHT
    WEIGHT
    WAZA_OSHIE
    NO_JUMP
    MAX
  ]
end

  waza_machine_waza = [
    "�߂Ƃ�",
    "�h���S���N���[",
    "�T�C�R�V���b�N",
    "�߂�����",
    "�ق���",
    "�ǂ��ǂ�",
    "�����",
    "�r���h�A�b�v",
    "�x�m���V���b�N",
    "�߂��߂�p���[",
    "�ɂق�΂�",
    "���傤�͂�",
    "�ꂢ�Ƃ��r�[��",
    "�ӂԂ�",
    "�͂�����������",
    "�Ђ���̂���",
    "�܂���",
    "���܂���",
    "�e���L�l�V�X",
    "����҂̂܂���",
    "�������",
    "�\�[���[�r�[��",
    "�������Ƃ�",
    "�P�O�܂�{���g",
    "���݂Ȃ�",
    "������",
    "���񂪂���",
    "���Ȃ��ق�",
    "�T�C�R�L�l�V�X",
    "�V���h�[�{�[��",
    "�������",
    "�����Ԃ񂵂�",
    "���t���N�^�[",
    "�w�h���E�F�[�u",
    "������ق�����",
    "�w�h���΂�����",
    "���Ȃ��炵",
    "��������",
    "���񂹂��ӂ���",
    "�΂߂�����",
    "���������",
    "���炰��",
    "�j�g���`���[�W",
    "�˂ނ�",
    "��������",
    "�ǂ�ڂ�",
    "���[�L�b�N",
    "��񂵂傤",
    "�G�R�[�{�C�X",
    "�I�[�o�[�q�[�g",
    "�T�C�h�`�F���W",
    "����������",
    "�G�i�W�[�{�[��",
    "�݂˂���",
    "�˂��Ƃ�",
    "�Ȃ�����",
    "�`���[�W�r�[��",
    "�t���[�t�H�[��",
    "�₫����",
    "����������",
    "���ɂ�",
    "�A�N���o�b�g",
    "����������",
    "�����΂��͂�",
    "�V���h�[�N���[",
    "�����؂�����",
    "����������",
    "�M�K�C���p�N�g",
    "���b�N�J�b�g",
    "�t���b�V��",
    "�X�g�[���G�b�W",
    "�{���g�`�F���W",
    "�ł񂶂�",
    "�W���C���{�[��",
    "�邬�̂܂�",
    "�ނ��̂Ă�����",
    "��������",
    "���Ȃ炵",
    "������̂��Ԃ�",
    "����Ȃ���",
    "�V�U�[�N���X",
    "�h���S���e�[��",
    "�ӂ邢���Ă�",
    "�ǂ��Â�",
    "��߂���",
    "�����ނ���",
    "���΂�",
    "���΂�",
    "�Ƃ�ڂ�����",
    "�݂����",
    "���X�^�[�J�m��",
    "�g���b�N���[��",
    "���C���h�{���g",
    "����������",
    "�����肫",
    "�Ȃ݂̂�",
    "������Ƃ�",
    "�_�C�r���O",
    "�����̂ڂ�",
    "���킭����",
    "�o�[�N�A�E�g",
  ]

  waza_oshie_waza = [
    "�����̂�����",
    "�ق̂��̂�����",
    "�݂��̂�����",
    "�n�[�h�v�����g",
    "�u���X�g�o�[��",
    "�n�C�h���J�m��",
    "��イ��������",
  ]

	if ARGV.size < 2
		print "error: ruby wazamachine_split.rb read_file write_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "write_file:�������ރt�@�C��\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0
  ARGV_WRITE_FILE = 1

  read_data = []
  cnt = 0
  open( ARGV[ ARGV_READ_FILE ] ) {|fp_r|
    while str = fp_r.gets
      str = str.tr( "\"\r\n", "" )
      split_data = str.split(/,/)
      next if split_data.size <= 1       #�T�[�o����̃G�N�X�|�[�g�ŃS�~���R�[�h�����邱�Ƃ�����̂ŁA�r������
      read_data[ cnt ] = str
      cnt += 1
    end
  }

  fp_w = open( ARGV[ ARGV_WRITE_FILE ], "w" )

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)
    
    #i��0�Ȃ猩�o��
    if i == 0
      for j in PARA::POKENAME..PARA::SHINKA_COND7
        fp_w.printf( "%s,", split_data[ j ] )
      end
      waza_machine_waza.size.times {|j|
        fp_w.printf( "%s,", waza_machine_waza[ j ] )
      }
      for j in PARA::GRA_NO..PARA::WEIGHT
        fp_w.printf( "%s,", split_data[ j ] )
      end
      waza_oshie_waza.size.times {|j|
        fp_w.printf( "%s,", waza_oshie_waza[ j ] )
      }
      fp_w.printf( "%s,", split_data[ PARA::NO_JUMP ] )

      fp_w.print("\n")
    else
      for j in PARA::POKENAME..PARA::SHINKA_COND7
        fp_w.printf( "%s,", split_data[ j ] )
      end

      waza_machine = split_data[ PARA::WAZA_MACHINE ].split(//)

      waza_machine.size.times {|k|
        if waza_machine[ k ] == "��"
          fp_w.print("��,")
        else
          fp_w.print(",")
        end
      }

      padding = 101 - waza_machine.size

      padding.times {|k|
        fp_w.print(",")
      }

      for j in PARA::GRA_NO..PARA::WEIGHT
        fp_w.printf( "%s,", split_data[ j ] )
      end

      waza_machine = split_data[ PARA::WAZA_OSHIE ].split(//)

      waza_machine.size.times {|k|
        if waza_machine[ k ] == "��"
          fp_w.print("��,")
        else
          fp_w.print(",")
        end
      }

      padding = 7 - waza_machine.size

      padding.times {|k|
        fp_w.print(",")
      }

      fp_w.printf( "%s,", split_data[ PARA::NO_JUMP ] )

      fp_w.print("\n")
    end
  }

  fp_w.close
