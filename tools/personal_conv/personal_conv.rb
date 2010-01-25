#! ruby -Ks

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'
  require File.dirname(__FILE__) + '/../hash/wazano_hash.rb'
  require File.dirname(__FILE__) + '/../hash/item_hash.rb'
  require File.dirname(__FILE__) + '/../hash/tokusei_hash.rb'
  require 'date'

=begin
  u8    basic_hp;       //��{�g�o
  u8    basic_pow;      //��{�U����
  u8    basic_def;      //��{�h���
  u8    basic_agi;      //��{�f����

  u8    basic_spepow;   //��{����U����
  u8    basic_spedef;   //��{����h���
  u8    type1;          //�����P
  u8    type2;          //�����Q

  u8    get_rate;       //�ߊl��
  u8    rank;           //�|�P���������N
  u16   pains_hp    :2; //���^�w�͒l�g�o
  u16   pains_pow   :2; //���^�w�͒l�U����
  u16   pains_def   :2; //���^�w�͒l�h���
  u16   pains_agi   :2; //���^�w�͒l�f����
  u16   pains_spepow:2; //���^�w�͒l����U����
  u16   pains_spedef:2; //���^�w�͒l����h���
  u16               :4; //���^�w�͒l�\��

  u16   item1;          //�A�C�e���P
  u16   item2;          //�A�C�e���Q

  u16   item3;          //�A�C�e���R
  u8    sex;            //���ʃx�N�g��
  u8    egg_birth;      //�^�}�S�̛z������

  u8    friend;         //�Ȃ��x�����l
  u8    grow;           //�����Ȑ�����
  u8    egg_group1;     //���Â���O���[�v1
  u8    egg_group2;     //���Â���O���[�v2

  u8    speabi1;        //����\�͂P
  u8    speabi2;        //����\�͂Q
  u8    speabi3;        //����\�͂R
  u8    escape;         //�����闦

  u16   form_index;     //�ʃt�H�����p�[�\�i���f�[�^�J�n�ʒu
  u16   form_gra_index; //�ʃt�H�����O���t�B�b�N�f�[�^�J�n�ʒu

  u8    form_max;       //�ʃt�H����MAX
  u8    color     :6;   //�F�i�}�ӂŎg�p�j
  u8    reverse   :1;   //���]�t���O
  u8    pltt_only :1;   //�ʃt�H�������p���b�g�̂ݕω�  
  u16   give_exp;       //���^�o���l

  u16   height;         //����
  u16   weight;         //�d��

  u32   machine1;       //�Z�}�V���t���O�P
  u32   machine2;       //�Z�}�V���t���O�Q
  u32   machine3;       //�Z�}�V���t���O�Q
  u32   machine4;       //�Z�}�V���t���O�Q
=end

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
    MAX
  ]
end

class SHINKA
  enum_const_set %w[
	  NONE
	  FRIEND_HIGH
	  FRIEND_HIGH_NOON
	  FRIEND_HIGH_NIGHT
	  LEVELUP
	  TUUSHIN
	  TUUSHIN_ITEM
	  ITEM
	  SPECIAL_POW
	  SPECIAL_EVEN
	  SPECIAL_DEF
	  SPECIAL_RND_EVEN
	  SPECIAL_RND_ODD
	  SPECIAL_LEVELUP
	  SPECIAL_NUKENIN
	  SPECIAL_BEAUTIFUL
	  ITEM_MALE
	  ITEM_FEMALE
	  SOUBI_NOON
	  SOUBI_NIGHT
	  WAZA
	  POKEMON
	  MALE
	  FEMALE
	  PLACE_TENGANZAN
	  PLACE_KOKE
	  PLACE_ICE
  ]
end

class SHINKA_PARAM
  enum_const_set %w[
    NONE
    LEVEL
    ITEM
    WAZA
    POKEMON
  ]
end

class FORM
  def initialize
    @form_name = []
    @form_index = 0
  end

  def add_form_name( form_name )
    @form_name << form_name
  end

  def get_form_name( index )
    return @form_name[ index ]
  end

  def get_form_max
    return @form_name.size
  end

  def set_form_index( index )
    @form_index = index
  end

  def get_form_index
    return @form_index
  end
end

def write_lst_file( fp_lst, gra_no, form_name )
  case gra_no
  when 1..151
   fp_lst.printf( "kanto/%03d/pfwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pfwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pfwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pfwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pfwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pfwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pbwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pbwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pbwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pbwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pbwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pbwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pmwb_%03d%s_n.ncl\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "kanto/%03d/pmwb_%03d%s_r.ncl\n",   gra_no, gra_no, form_name )
  when 152..251
   fp_lst.printf( "jyoto/%03d/pfwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pfwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pfwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pfwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pfwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pfwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pbwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pbwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pbwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pbwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pbwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pbwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pmwb_%03d%s_n.ncl\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "jyoto/%03d/pmwb_%03d%s_r.ncl\n",   gra_no, gra_no, form_name )
  when 252..386
   fp_lst.printf( "houen/%03d/pfwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pfwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pfwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pfwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pfwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pfwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pbwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pbwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pbwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pbwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pbwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pbwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pmwb_%03d%s_n.ncl\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "houen/%03d/pmwb_%03d%s_r.ncl\n",   gra_no, gra_no, form_name )
  when 387..493
   fp_lst.printf( "shinou/%03d/pfwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pfwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pfwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pfwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pfwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pfwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pbwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pbwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pbwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pbwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pbwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pbwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pmwb_%03d%s_n.ncl\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "shinou/%03d/pmwb_%03d%s_r.ncl\n",   gra_no, gra_no, form_name )
  else
   fp_lst.printf( "issyu/%03d/pfwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pfwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pfwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pfwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pfwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pfwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pbwb_%03d%s_m.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pbwb_%03d%s_f.ncg\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pbwb_%03dc%s_m.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pbwb_%03dc%s_f.ncg\n",  gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pbwb_%03d%s.nce\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pbwb_%03d%s.nmc\n",     gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pmwb_%03d%s_n.ncl\n",   gra_no, gra_no, form_name )
   fp_lst.printf( "issyu/%03d/pmwb_%03d%s_r.ncl\n",   gra_no, gra_no, form_name )
  end
end

#�f�[�^�e�[�u��
  type_table = {
    "�m�[�}��"=>0,
    "�m�|�}��"=>0,
    "�i��"=>1,
    "��s"=>2,
    "��"=>3,
    "�n��"=>4,
    "��"=>5,
    "��"=>6,
    "�S�[�X�g"=>7,
    "�S�|�X�g"=>7,
    "�S"=>8,
    "�|"=>8,
    "���^��"=>8,
    "��"=>9,
    "��"=>10,
    "��"=>11,
    "�d�C"=>12,
    "�G�X�p�["=>13,
    "�G�X�p�|"=>13,
    "�X"=>14,
    "�h���S��"=>15,
    "��"=>16,
  }

  color_table = {
    "��"=>0,
    "��"=>1,
    "��"=>2,
    "��"=>3,
    "��"=>4,
    "��"=>5,
    "��"=>6,
    "�D"=>7,
    "��"=>8,
    "��"=>9,
  }

  shinka_table = {
    "�Ȃ�"                      => [  0, "NONE",              SHINKA_PARAM::NONE ],
    "�Ȃ��x�@����"            => [  1, "NONE",              SHINKA_PARAM::NONE ],
    "�Ȃ��x�@�����@����"      => [  2, "NONE",              SHINKA_PARAM::NONE ],
    "�Ȃ��x�@�����@��"        => [  3, "NONE",              SHINKA_PARAM::NONE ],
    "���x���A�b�v"              => [  4, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "�ʐM"                      => [  5, "NONE",              SHINKA_PARAM::NONE ],
    "�ʐM�@�A�b�v�O���[�h"      => [  6, "�A�b�v�O���[�h",    SHINKA_PARAM::ITEM ],
    "�ʐM�@��������̂��邵"    => [  6, "��������̂��邵",  SHINKA_PARAM::ITEM ],
    "�ʐM�@���^���R�[�g"        => [  6, "���^���R�[�g",      SHINKA_PARAM::ITEM ],
    "�ʐM�@��イ�̃E���R"      => [  6, "��イ�̃E���R",    SHINKA_PARAM::ITEM ],
    "�ʐM�@�������̃L�o"        => [  6, "�������̃L�o",      SHINKA_PARAM::ITEM ],
    "�ʐM�@�������̃E���R"      => [  6, "�������̃E���R",    SHINKA_PARAM::ITEM ],
    "�ʐM�@���񂩂��̃L�o"      => [  6, "���񂩂��̃L�o",    SHINKA_PARAM::ITEM ],
    "�ʐM�@���񂩂��̃E���R"    => [  6, "���񂩂��̃E���R",  SHINKA_PARAM::ITEM ],
    "�ʐM�@�v���e�N�^�["        => [  6, "�v���e�N�^�[",      SHINKA_PARAM::ITEM ],
    "�ʐM�@�G���L�u�[�X�^�["    => [  6, "�G���L�u�[�X�^�[",  SHINKA_PARAM::ITEM ],
    "�ʐM�@�}�O�}�u�[�X�^�["    => [  6, "�}�O�}�u�[�X�^�[",  SHINKA_PARAM::ITEM ],
    "�ʐM�@���₵���p�b�`"      => [  6, "���₵���p�b�`",    SHINKA_PARAM::ITEM ],
    "�ʐM�@�ꂢ�����̂ʂ�"      => [  6, "�ꂢ�����̂ʂ�",    SHINKA_PARAM::ITEM ],
    "����@���݂Ȃ�̂���"      => [  7, "���݂Ȃ�̂���",    SHINKA_PARAM::ITEM ],
    "����@�����悤�̂���"      => [  7, "�����悤�̂���",    SHINKA_PARAM::ITEM ],
    "����@���̂���"          => [  7, "���̂���",        SHINKA_PARAM::ITEM ],
    "����@�ق̂��̂���"        => [  7, "�ق̂��̂���",      SHINKA_PARAM::ITEM ],
    "����@�݂��̂���"          => [  7, "�݂��̂���",        SHINKA_PARAM::ITEM ],
    "����@���[�t�̂���"        => [  7, "���[�t�̂���",      SHINKA_PARAM::ITEM ],
    "����@�Ђ���̂���"        => [  7, "�Ђ���̂���",      SHINKA_PARAM::ITEM ],
    "����@��݂̂���"          => [  7, "��݂̂���",        SHINKA_PARAM::ITEM ],
    "����@�U"                  => [  8, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@��"                  => [  9, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@�h"                  => [ 10, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@������������"      => [ 11, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@���������"      => [ 12, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@���x���A�b�v"        => [ 13, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@�莝���ɋ󂫂�����"  => [ 14, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@������"              => [ 15, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "����@�߂��߂���"        => [ 16, "�߂��߂���",        SHINKA_PARAM::ITEM ],
    "����@�߂��߂���"        => [ 17, "�߂��߂���",        SHINKA_PARAM::ITEM ],
    "�����@�����@�܂�܂邢��"  => [ 18, "�܂�܂邢��",      SHINKA_PARAM::ITEM ],
    "�����@��@����ǂ��L�o"    => [ 19, "����ǂ��L�o",      SHINKA_PARAM::ITEM ],
    "�����@��@����ǂ��c��"    => [ 19, "����ǂ��c��",      SHINKA_PARAM::ITEM ],
    "�Z�K���@���̂܂�"          => [ 20, "���̂܂�",          SHINKA_PARAM::WAZA ],
    "�Z�K���@�_�u���A�^�b�N"    => [ 20, "�_�u���A�^�b�N",    SHINKA_PARAM::WAZA ],
    "�Z�K���@���낪��"          => [ 20, "���낪��",          SHINKA_PARAM::WAZA ],
    "�Z�K���@���񂵂̂�����"    => [ 20, "���񂵂̂�����",    SHINKA_PARAM::WAZA ],
    "�|�P�����@�e�b�|�E�I"      => [ 21, "�e�b�|�E�I",        SHINKA_PARAM::POKEMON ],
    "���ʁ�"                    => [ 22, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "���ʁ�"                    => [ 23, "LEVEL",             SHINKA_PARAM::LEVEL ],
    "�ꏊ�@�Ă񂪂񂴂�"        => [ 24, "NONE",              SHINKA_PARAM::NONE ],
    "�ꏊ�@�ې�������"          => [ 25, "NONE",              SHINKA_PARAM::NONE ],
    "�ꏊ�@����������"          => [ 26, "NONE",              SHINKA_PARAM::NONE ],
  }

  shinka_cond = [ 
  	"SHINKA_NONE",
  	"SHINKA_FRIEND_HIGH",
  	"SHINKA_FRIEND_HIGH_NOON",
  	"SHINKA_FRIEND_HIGH_NIGHT",
  	"SHINKA_LEVELUP",
  	"SHINKA_TUUSHIN",
  	"SHINKA_TUUSHIN_ITEM",
  	"SHINKA_ITEM",
  	"SHINKA_SPECIAL_POW",
  	"SHINKA_SPECIAL_EVEN",
  	"SHINKA_SPECIAL_DEF",
  	"SHINKA_SPECIAL_RND_EVEN",
  	"SHINKA_SPECIAL_RND_ODD",
  	"SHINKA_SPECIAL_LEVELUP",
  	"SHINKA_SPECIAL_NUKENIN",
  	"SHINKA_SPECIAL_BEAUTIFUL",
  	"SHINKA_ITEM_MALE",
  	"SHINKA_ITEM_FEMALE",
  	"SHINKA_SOUBI_NOON",
  	"SHINKA_SOUBI_NIGHT",
  	"SHINKA_WAZA",	
  	"SHINKA_POKEMON",
  	"SHINKA_MALE",
  	"SHINKA_FEMALE",
  	"SHINKA_PLACE_TENGANZAN",
  	"SHINKA_PLACE_KOKE",
  	"SHINKA_PLACE_ICE",
  ]

	if ARGV.size < 2
		print "error: ruby personal_conv.rb read_file gmm_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "gmm_file:�|�P��������gmm�t�@�C���������o�����߂̌��ɂȂ�t�@�C��\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0
  ARGV_READ_GMM_FILE = 1
  EVO_MAX = 7 #�i����MAX��

  label = LabelMake.new
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

  monsno = Hash::new
  gra_hash = Hash::new
  monsname = []
  form = []
  machine = []
  pokelist = []
  
  form[ 0 ] = FORM::new
  read_data.size.times {|i|
    form[ i + 1 ] = FORM::new
  }

  #�|�P�������x�����O���t�B�b�N�f�[�^���k���X�g��gmm�t�@�C������
  print "�|�P�������x�����O���t�B�b�N�f�[�^���k���X�g��gmm�t�@�C���@������\n"
  fp_monsno = open( "monsno_def.h", "w" )
  fp_monsnum = open( "monsnum_def.h", "w" )
  fp_gra = open( "pokegra_wb.scr", "w" )
  fp_lst = open( "pokegra_wb.lst", "w" )
  fp_wave = open( "wb_sound_pv_wav.lst", "w" )
  fp_bank = open( "wb_sound_pv_bnk.lst", "w" )
  fp_num = open( "zukan2grano.txt", "w" )
  gmm = GMM::new
  gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "monsname.gmm" )

  fp_monsno.print( "//============================================================================================\n" )
  fp_monsno.print( "/**\n" )
  fp_monsno.print( " * @file	monsno_def.h\n" )
  fp_monsno.print( " * @bfief	�|�P����No��Define��`�t�@�C��\n" )
  fp_monsno.print( " * @author	PersonalConverter\n" )
  fp_monsno.print( " * �p�[�\�i���R���o�[�^���琶������܂���\n" )
  fp_monsno.print( "*/\n")
  fp_monsno.print( "//============================================================================================\n" )
  fp_monsno.print( "\n#pragma once\n" )
  fp_monsno.print( "#include  \"monsnum_def.h\"\n\n" )

  fp_monsnum.print( "//============================================================================================\n" )
  fp_monsnum.print( "/**\n" )
  fp_monsnum.print( " * @file	monsnum_def.h\n" )
  fp_monsnum.print( " * @bfief	�|�P����No��Define��`�t�@�C���i���x�����̓O���t�B�b�N�i���o�[�j\n" )
  fp_monsnum.print( " * @author	PersonalConverter\n" )
  fp_monsnum.print( " * �p�[�\�i���R���o�[�^���琶������܂���\n" )
  fp_monsnum.print( "*/\n")
  fp_monsnum.print( "//============================================================================================\n" )
  fp_monsnum.print( "\n#pragma once\n\n" )

	fp_gra.printf( "\"pfwb_000_m.NCGR\"\n" )
	fp_gra.printf( "\"pfwb_000_f.NCGR\"\n" )
	fp_gra.printf( "\"pfwb_000c_m.NCBR\"\n" )
	fp_gra.printf( "\"pfwb_000c_f.NCBR\"\n" )
	fp_gra.printf( "\"pfwb_000.NCER\"\n" )
	fp_gra.printf( "\"pfwb_000.NANR\"\n" )
	fp_gra.printf( "\"pfwb_000.NMCR\"\n" )
	fp_gra.printf( "\"pfwb_000.NMAR\"\n" )
	fp_gra.printf( "\"pfwb_000.NCEC\"\n" )
	fp_gra.printf( "\"pbwb_000_m.NCGR\"\n" )
	fp_gra.printf( "\"pbwb_000_f.NCGR\"\n" )
	fp_gra.printf( "\"pbwb_000c_m.NCBR\"\n" )
	fp_gra.printf( "\"pbwb_000c_f.NCBR\"\n" )
	fp_gra.printf( "\"pbwb_000.NCER\"\n" )
	fp_gra.printf( "\"pbwb_000.NANR\"\n" )
	fp_gra.printf( "\"pbwb_000.NMCR\"\n" )
	fp_gra.printf( "\"pbwb_000.NMAR\"\n" )
	fp_gra.printf( "\"pbwb_000.NCEC\"\n" )
	fp_gra.printf( "\"pmwb_000_n.NCLR\"\n" )
	fp_gra.printf( "\"pmwb_000_r.NCLR\"\n" )

  fp_lst.printf( "kanto/000/pfwb_000_m.ncg\n" )
  fp_lst.printf( "kanto/000/pfwb_000_f.ncg\n" )
  fp_lst.printf( "kanto/000/pfwb_000c_m.ncg\n" )
  fp_lst.printf( "kanto/000/pfwb_000c_f.ncg\n" )
  fp_lst.printf( "kanto/000/pfwb_000.nce\n" )
  fp_lst.printf( "kanto/000/pfwb_000.nmc\n" )
  fp_lst.printf( "kanto/000/pbwb_000_m.ncg\n" )
  fp_lst.printf( "kanto/000/pbwb_000_f.ncg\n" )
  fp_lst.printf( "kanto/000/pbwb_000c_m.ncg\n" )
  fp_lst.printf( "kanto/000/pbwb_000c_f.ncg\n" )
  fp_lst.printf( "kanto/000/pbwb_000.nce\n" )
  fp_lst.printf( "kanto/000/pbwb_000.nmc\n" )
  fp_lst.printf( "kanto/000/pmwb_000_n.ncl\n" )
  fp_lst.printf( "kanto/000/pmwb_000_r.ncl\n" )

  gmm.make_row_index( "MONSNAME_", 0, "�[�[�[�[�[" )

  pokelist << "�|�|�|�|�|\t999\t�m�[�}��\t�m�[�}��\t�|\t�|\n"

  cnt = 1
  other_form = 0
  monsno_max = 0
  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)
    if split_data[ PARA::POKENAME ] == ""
      other_form = 1
      if monsno_max == 0
        monsno_max = cnt
      end
    end
    if other_form == 1
      if split_data[ PARA::POKENAME ] == ""
      else
        if form[ monsno[ split_data[ PARA::POKENAME ] ] ].get_form_index == 0 && split_data[ PARA::OTHERFORM ] == "��"
          form[ monsno[ split_data[ PARA::POKENAME ] ] ].set_form_index( cnt )
        end
      end
    else
      monsno[ split_data[ PARA::POKENAME ] ] = cnt
      gra_hash[ split_data[ PARA::GRA_NO ] ] = cnt
      monsname[ cnt - 1 ] = split_data[ PARA::POKENAME ]
      fp_monsno.print( "#define\t\t" )
      label_str = label.make_label( "MONSNO_", split_data[ PARA::POKENAME ] )
      fp_monsno.print( label_str )
      tab_cnt = ( 19 - label_str.length ) / 2 + 1
      for j in 1..tab_cnt
        fp_monsno.print( "\t" )
      end
      fp_monsno.printf( "( %d )\n", cnt )

      fp_monsnum.printf( "#define\t\tMONSNO_%03d\t\t( %d )\t\t//%s\n", split_data[ PARA::GRA_NO ].to_i, cnt, split_data[ PARA::POKENAME ] )

      if split_data[ PARA::FORM_NAME ] == ""
        form_name = ""
      else
        form_name = "_"
        form_name << split_data[ PARA::FORM_NAME ]
      end

      gra_no = split_data[ PARA::GRA_NO ].to_i

	    fp_gra.printf( "\"pfwb_%03d%s_m.NCGR\"\n",   gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03d%s_f.NCGR\"\n",   gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03dc%s_m.NCBR\"\n",  gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03dc%s_f.NCBR\"\n",  gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03d%s.NCER\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03d%s.NANR\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03d%s.NMCR\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03d%s.NMAR\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pfwb_%03d%s.NCEC\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s_m.NCGR\"\n",   gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s_f.NCGR\"\n",   gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03dc%s_m.NCBR\"\n",  gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03dc%s_f.NCBR\"\n",  gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s.NCER\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s.NANR\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s.NMCR\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s.NMAR\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pbwb_%03d%s.NCEC\"\n",     gra_no, form_name )
	    fp_gra.printf( "\"pmwb_%03d%s_n.NCLR\"\n",   gra_no, form_name )
	    fp_gra.printf( "\"pmwb_%03d%s_r.NCLR\"\n",   gra_no, form_name )

      #SARC�o��
      #WAVE&BANK
      if gra_no >= 501
	      fp_wave.printf( "WAVE_ARC_PMWB_%03d		: AUTO, \"PMWB_%03d.swls\"\n",   gra_no, gra_no )
	      fp_bank.printf( "BANK_PMWB_%03d		: TEXT, \"PMWB_%03d.bnk\",	WAVE_ARC_PMWB_%03d\n",   gra_no, gra_no, gra_no )
      end

      fp_num.printf( "%s\n", split_data[ PARA::GRA_NO ] )

      write_lst_file( fp_lst, gra_no, form_name )

      gmm.make_row_index( "MONSNAME_", cnt, split_data[ PARA::POKENAME ] )

      speabi1 = split_data[ PARA::SPEABI1 ]
      if speabi1 == ""
        speabi1 = "�|"
      end
      speabi2 = split_data[ PARA::SPEABI2 ]
      if speabi2 == ""
        speabi2 = "�|"
      end
      pokelist << "%s\t%s\t%s\t%s\t%s\t%s\n" % [ split_data[ PARA::POKENAME ], split_data[ PARA::GRA_NO ], split_data[ PARA::TYPE1 ], split_data[ PARA::TYPE2 ], speabi1, speabi2 ]
    end
    if split_data[ PARA::FORM_NAME ] != "" && split_data[ PARA::POKENAME ] != ""
      form[ monsno[ split_data[ PARA::POKENAME ] ] ].add_form_name( split_data[ PARA::FORM_NAME ] )
    end
    if other_form == 0 || ( other_form == 1 && split_data[ PARA::OTHERFORM ] == "��" )
      cnt += 1
    end
  }

  #���̃X�N���v�g�Ŏg�p���邽�߂̃|�P����NoHash�𐶐�
  fp_hash = open( "monsno_hash.rb", "w" )
  fp_hash.printf("#! ruby -Ks\n\n" )

  #�����X�^�[�i���o�[�n�b�V���e�[�u��
  fp_hash.printf("\t$monsno_hash = {\n" )
  monsno.size.times {|no|
    split_data = read_data[ no ].split(/,/)
    fp_hash.printf("\t\t\"%s\"=>%d,\n", split_data[ PARA::POKENAME ], monsno[ split_data[ PARA::POKENAME ] ] )
  }
  fp_hash.printf("\t}\n" )

  #�t�H�����i���o�[�ő�l�n�b�V���e�[�u��
  fp_hash.printf("\t$formmax_hash = {\n" )
  monsno.size.times {|no|
    split_data = read_data[ no ].split(/,/)
    fp_hash.printf("\t\t\"%s\"=>%d,\n", split_data[ PARA::POKENAME ], form[ no ].get_form_max )
  }
  fp_hash.printf("\t}\n" )

  #GraNo2ZukanNo�n�b�V���e�[�u��
  fp_hash.printf("\t$gra2zukan_hash = {\n" )
  gra_hash.size.times {|no|
    split_data = read_data[ no ].split(/,/)
    fp_hash.printf("\t\t\"%s\"=>%d,\n", split_data[ PARA::GRA_NO ], gra_hash[ split_data[ PARA::GRA_NO ] ] )
  }
  fp_hash.printf("\t}\n" )
  fp_hash.close

  #gmm�t�@�C���̌�n��
  gmm.make_row_index( "MONSNAME_", monsno_max, "�^�}�S" )
  gmm.make_row_index( "MONSNAME_", monsno_max + 1, "�^�}�S" )
  gmm.close_gmm

  #�^�}�S�͓Ǝ��̎�����������
  fp_gra.print( "\"pfwb_egg_normal_m.NCGR\"\n" )
  fp_gra.print( "\"pfwb_egg_normal_f.NCGR\"\n" )
	fp_gra.print( "\"pfwb_eggc_normal_m.NCBR\"\n" )
	fp_gra.print( "\"pfwb_eggc_normal_f.NCBR\"\n" )
	fp_gra.print( "\"pfwb_egg_normal.NCER\"\n" )
	fp_gra.print( "\"pfwb_egg_normal.NANR\"\n" )
	fp_gra.print( "\"pfwb_egg_normal.NMCR\"\n" )
	fp_gra.print( "\"pfwb_egg_normal.NMAR\"\n" )
	fp_gra.print( "\"pfwb_egg_normal.NCEC\"\n" )
  fp_gra.print( "\"pbwb_egg_normal_m.NCGR\"\n" )
  fp_gra.print( "\"pbwb_egg_normal_f.NCGR\"\n" )
	fp_gra.print( "\"pbwb_eggc_normal_m.NCBR\"\n" )
	fp_gra.print( "\"pbwb_eggc_normal_f.NCBR\"\n" )
	fp_gra.print( "\"pbwb_egg_normal.NCER\"\n" )
	fp_gra.print( "\"pbwb_egg_normal.NANR\"\n" )
	fp_gra.print( "\"pbwb_egg_normal.NMCR\"\n" )
	fp_gra.print( "\"pbwb_egg_normal.NMAR\"\n" )
	fp_gra.print( "\"pbwb_egg_normal.NCEC\"\n" )
	fp_gra.print( "\"pmwb_egg_normal_n.NCLR\"\n" )
	fp_gra.print( "\"pmwb_egg_normal_r.NCLR\"\n" )

  fp_gra.print( "\"pfwb_egg_manafi_m.NCGR\"\n" )
  fp_gra.print( "\"pfwb_egg_manafi_f.NCGR\"\n" )
	fp_gra.print( "\"pfwb_eggc_manafi_m.NCBR\"\n" )
	fp_gra.print( "\"pfwb_eggc_manafi_f.NCBR\"\n" )
	fp_gra.print( "\"pfwb_egg_manafi.NCER\"\n" )
	fp_gra.print( "\"pfwb_egg_manafi.NANR\"\n" )
	fp_gra.print( "\"pfwb_egg_manafi.NMCR\"\n" )
	fp_gra.print( "\"pfwb_egg_manafi.NMAR\"\n" )
	fp_gra.print( "\"pfwb_egg_manafi.NCEC\"\n" )
  fp_gra.print( "\"pbwb_egg_manafi_m.NCGR\"\n" )
  fp_gra.print( "\"pbwb_egg_manafi_f.NCGR\"\n" )
	fp_gra.print( "\"pbwb_eggc_manafi_m.NCBR\"\n" )
	fp_gra.print( "\"pbwb_eggc_manafi_f.NCBR\"\n" )
	fp_gra.print( "\"pbwb_egg_manafi.NCER\"\n" )
	fp_gra.print( "\"pbwb_egg_manafi.NANR\"\n" )
	fp_gra.print( "\"pbwb_egg_manafi.NMCR\"\n" )
	fp_gra.print( "\"pbwb_egg_manafi.NMAR\"\n" )
	fp_gra.print( "\"pbwb_egg_manafi.NCEC\"\n" )
	fp_gra.print( "\"pmwb_egg_manafi_n.NCLR\"\n" )
	fp_gra.print( "\"pmwb_egg_manafi_r.NCLR\"\n" )

  fp_lst.print( "egg/pfwb_egg_normal_m.ncg\n" )
	fp_lst.print( "egg/pfwb_eggc_normal_m.ncg\n" )
	fp_lst.print( "egg/pfwb_egg_normal.nce\n" )
	fp_lst.print( "egg/pfwb_egg_normal.nmc\n" )
  fp_lst.print( "egg/pbwb_egg_normal_m.ncg\n" )
	fp_lst.print( "egg/pbwb_eggc_normal_m.ncg\n" )
	fp_lst.print( "egg/pbwb_egg_normal.nce\n" )
	fp_lst.print( "egg/pbwb_egg_normal.nmc\n" )
	fp_lst.print( "egg/pmwb_egg_normal_n.ncl\n" )
	fp_lst.print( "egg/pmwb_egg_normal_r.ncl\n" )
  fp_lst.print( "egg/pfwb_egg_manafi_m.ncg\n" )
	fp_lst.print( "egg/pfwb_eggc_manafi_m.ncg\n" )
	fp_lst.print( "egg/pfwb_egg_manafi.nce\n" )
	fp_lst.print( "egg/pfwb_egg_manafi.nmc\n" )
  fp_lst.print( "egg/pbwb_egg_manafi_m.ncg\n" )
	fp_lst.print( "egg/pbwb_eggc_manafi_m.ncg\n" )
	fp_lst.print( "egg/pbwb_egg_manafi.nce\n" )
	fp_lst.print( "egg/pbwb_egg_manafi.nmc\n" )
	fp_lst.print( "egg/pmwb_egg_manafi_n.ncl\n" )
	fp_lst.print( "egg/pmwb_egg_manafi_r.ncl\n" )

  fp_lst.print( "migawari/pfwb_migawari.ncg\n" )
	fp_lst.print( "migawari/pfwb_migawari.nce\n" )
	fp_lst.print( "migawari/pfwb_migawari.nmc\n" )
  fp_lst.print( "migawari/pbwb_migawari.ncg\n" )
	fp_lst.print( "migawari/pbwb_migawari.nce\n" )
	fp_lst.print( "migawari/pbwb_migawari.nmc\n" )
	fp_lst.print( "migawari/pmwb_migawari.ncl\n" )

  fp_gra.close
  fp_lst.close
  fp_num.close
	fp_wave.close
	fp_bank.close

  fp_pokelist = open( "pokelist.txt", "w" )
  pokelist.sort!
  pokelist.size.times {|i|
    fp_pokelist.printf( "%s", pokelist[ i ] )
  }
	fp_pokelist.close

  fp_monsno.printf( "#define\t\tMONSNO_TAMAGO\t\t\t\t( %d )\n", monsno_max )
  fp_monsno.printf( "#define\t\tMONSNO_DAMETAMAGO\t\t( %d )\n", monsno_max + 1 )
  fp_monsno.printf( "#define\t\tMONSNO_END\t\t\t\t\t( %d )\n",  monsno_max - 1 )
  fp_monsno.printf( "#define\t\tMONSNO_MAX\t\t\t\t\t( %d )\n",   monsno_max + 1 )
  fp_monsno.print( "\n" )

  fp_form = open( "otherform_wb.scr", "w" )
  fp_lst = open( "otherform_wb.lst", "w" )
  fp_pltt = open( "otherpltt_wb.scr", "w" )

  fp_monsno.print( "//�ʃt�H������`\n" )

  form.size.times {|i|
    if form[ i ].get_form_max != 0
      split_data = read_data[ i - 1 ].split(/,/)
      fp_monsno.printf( "//%s\n", split_data[ PARA::POKENAME ] )
      max = form[ i ].get_form_max
      max.times {|j|
        str = label.make_label( "FORMNO_", split_data[ PARA::POKENAME ] )
        str << "_"
        str << form[ i ].get_form_name( j ).upcase
        fp_monsno.printf( "#define\t\t%s\t\t\t\t( %d )\n", str, j )
        if j != 0
          if split_data[ PARA::PLTT_ONLY ] == "��"
  	        fp_form.printf( "\"pmwb_%s_%s_n.NCLR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pmwb_%s_%s_r.NCLR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
          else
  	        fp_form.printf( "\"pfwb_%s_%s_m.NCGR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%s_%s_f.NCGR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%sc_%s_m.NCBR\"\n",  split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%sc_%s_f.NCBR\"\n",  split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%s_%s.NCER\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%s_%s.NANR\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%s_%s.NMCR\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%s_%s.NMAR\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pfwb_%s_%s.NCEC\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s_m.NCGR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s_f.NCGR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%sc_%s_m.NCBR\"\n",  split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%sc_%s_f.NCBR\"\n",  split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s.NCER\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s.NANR\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s.NMCR\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s.NMAR\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pbwb_%s_%s.NCEC\"\n",     split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pmwb_%s_%s_n.NCLR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
  	        fp_form.printf( "\"pmwb_%s_%s_r.NCLR\"\n",   split_data[ PARA::GRA_NO ], form[ i ].get_form_name( j ) )
          end
          form_name = "_"
          form_name << form[ i ].get_form_name( j )

          write_lst_file( fp_lst, split_data[ PARA::GRA_NO ].to_i, form_name )
        end
      }
      fp_monsno.print( "\n" )
    end
  }

  #�݂����l�`�͍Ō�ɘA������
  fp_form.print( "\"pfwb_migawari.NCBR\"\n" )
	fp_form.print( "\"pfwb_migawari.NCER\"\n" )
	fp_form.print( "\"pfwb_migawari.NANR\"\n" )
	fp_form.print( "\"pfwb_migawari.NMCR\"\n" )
	fp_form.print( "\"pfwb_migawari.NMAR\"\n" )
	fp_form.print( "\"pfwb_migawari.NCEC\"\n" )
  fp_form.print( "\"pbwb_migawari.NCBR\"\n" )
	fp_form.print( "\"pbwb_migawari.NCER\"\n" )
	fp_form.print( "\"pbwb_migawari.NANR\"\n" )
	fp_form.print( "\"pbwb_migawari.NMCR\"\n" )
	fp_form.print( "\"pbwb_migawari.NMAR\"\n" )
	fp_form.print( "\"pbwb_migawari.NCEC\"\n" )
	fp_form.print( "\"pmwb_migawari.NCLR\"\n" )

  fp_monsno.close
  fp_monsnum.close
  fp_form.close
  fp_pltt.close

  print "�|�P�������x�����O���t�B�b�N�f�[�^���k���X�g��gmm�t�@�C���@�����I��\n"

  #�Z�o���e�[�u���n�b�V���̐���
  wothash = Hash::new

  print "�p�[�\�i���f�[�^������\n"
  #per_???.bin����
  print "per_000.bin ������\n"
  fp_per = open( "per_000.bin", "wb" )

  data = [ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ].pack( "C10 S4 C10 S2 C2 S3 L4" )

	data.size.times{ |c|
		fp_per.printf("%c",data[ c ])
	}

  fp_per.close
  print "per_000.bin �����I��\n"

  #wot_???.bin����
  print "wot_000.bin ������\n"
  fp_wot = open( "wot_000.bin", "wb" )

  data = [ 0xffff, 0xffff ].pack( "S2" )

	data.size.times{ |c|
		fp_wot.printf("%c",data[ c ])
	}

  fp_wot.close
  print "wot_000.bin �����I��\n"

  #evo_???.bin����
  print "evo_000.bin ������\n"
  fp_evo = open( "evo_000.bin", "wb" )

  for evo_cnt in 1..EVO_MAX
    data = [ 0,0,0 ].pack( "S3" )
	  data.size.times{ |c|
		  fp_evo.printf("%c",data[ c ])
	  }
  end

  fp_evo.close
  print "evo_000.bin �����I��\n"

  cnt = 1
  other_form = 0
  form_gra_index = 0
  seed = []
  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)
    if split_data[ PARA::POKENAME ] == ""
      other_form = 1
    end

    if wothash[ split_data[ PARA::POKENAME ] ] == nil
      wothash[ split_data[ PARA::POKENAME ] ] = []
    end

    if other_form == 0 || ( other_form == 1 && split_data[ PARA::OTHERFORM ] == "��" )
      printf( "per_%03d.bin ������\n", cnt )
      str = sprintf( "per_%03d.bin", cnt )
      fp_per = open( str, "wb" )

      hp      = split_data[ PARA::HP ].to_i
      pow     = split_data[ PARA::POW ].to_i
      defe    = split_data[ PARA::DEF ].to_i
      agi     = split_data[ PARA::AGI ].to_i
      spepow  = split_data[ PARA::SPEPOW ].to_i
      spedef  = split_data[ PARA::SPEDEF ].to_i
      if type_table[ split_data[ PARA::TYPE1 ] ] == nil
        printf( "��`����Ă��Ȃ��^�C�v�ł�:%s\n", split_data[ PARA::TYPE1 ] )
        exit( 1 )
      end
      type1   = type_table[ split_data[ PARA::TYPE1 ] ]
      if type_table[ split_data[ PARA::TYPE2 ] ] == nil
        printf( "��`����Ă��Ȃ��^�C�v�ł�:%s\n", split_data[ PARA::TYPE2 ] )
        exit( 1 )
      end
      type2   = type_table[ split_data[ PARA::TYPE2 ] ]

      get     = split_data[ PARA::GET ].to_i
      #@todo ����̓T�E���h�p�̒l�ɂȂ�͂�
      rank    = split_data[ PARA::RANK ].to_i
      exp_value = ( split_data[ PARA::HP_EXP ].to_i     <<  0 ) |
                  ( split_data[ PARA::POW_EXP ].to_i    <<  2 ) |
                  ( split_data[ PARA::DEF_EXP ].to_i    <<  4 ) |
                  ( split_data[ PARA::AGI_EXP ].to_i    <<  6 ) |
                  ( split_data[ PARA::SPEPOW_EXP ].to_i <<  8 ) |
                  ( split_data[ PARA::SPEDEF_EXP ].to_i << 10 )

      if split_data[ PARA::ITEM1 ] == ""
        item1 = 0
      else
        item1 = $item_hash[ split_data[ PARA::ITEM1 ] ]
      end
      if split_data[ PARA::ITEM2 ] == ""
        item2 = 0
      else
        item2 = $item_hash[ split_data[ PARA::ITEM2 ] ]
      end
      if split_data[ PARA::ITEM3 ] == ""
        item3 = 0
      else
        item3 = $item_hash[ split_data[ PARA::ITEM3 ] ]
      end

      gender  = split_data[ PARA::GENDER ].to_i
      birth   = split_data[ PARA::BIRTH_STEP ].to_i
      friend  = split_data[ PARA::FRIEND ].to_i
      grow    = split_data[ PARA::GROW ].to_i
      egg1    = split_data[ PARA::EGG_GROUP1 ].to_i
      egg2    = split_data[ PARA::EGG_GROUP2 ].to_i

      if split_data[ PARA::SPEABI1 ] == ""
        speabi1 = 0
      else
        speabi1 = $tokusei_hash[ split_data[ PARA::SPEABI1 ] ]
      end
      if split_data[ PARA::SPEABI2 ] == ""
        speabi2 = 0
      else
        speabi2 = $tokusei_hash[ split_data[ PARA::SPEABI2 ] ]
      end
      if split_data[ PARA::SPEABI3 ] == ""
        speabi3 = 0
      else
        speabi3 = $tokusei_hash[ split_data[ PARA::SPEABI3 ] ]
      end

      escape  = split_data[ PARA::ESCAPE ].to_i
      
      form_per_start  = form[ cnt ].get_form_index

      if form[ cnt ].get_form_max == 0
        form_gra_start = 0
      else
        form_gra_start =  form_gra_index
        form_gra_index += ( form[ cnt ].get_form_max - 1 )
      end

      form_max  = form[ cnt ].get_form_max

      if color_table[ split_data[ PARA::COLOR ] ] ==nil
        printf( "��`����Ă��Ȃ��J���[�ł�:%s\n", split_data[ PARA::COLOR ] )
        exit( 1 )
      end
      col_rev = color_table[ split_data[ PARA::COLOR ] ]
      if split_data[ PARA::REVERSE ] == "��"
        col_rev |= 0x40
      end
      if split_data[ PARA::PLTT_ONLY ] == "��"
        col_rev |= 0x80
      end

      exp = split_data[ PARA::EXP ].to_i

      if split_data[ PARA::HEIGHT ] == "" || split_data[ PARA::HEIGHT ] == nil
        height = 10
      else
        height = split_data[ PARA::HEIGHT ].to_i
      end

      if split_data[ PARA::WEIGHT ] == "" || split_data[ PARA::WEIGHT ] == nil
        weight = 10
      else
        weight = split_data[ PARA::WEIGHT ].to_i
      end

      waza_machine = split_data[ PARA::WAZA_MACHINE ].split(//)
      machine[ 0 ] = 0
      machine[ 1 ] = 0
      machine[ 2 ] = 0
      machine[ 3 ] = 0

      flag = 1
      flag_cnt = 32
      machine_index = 0

      waza_machine.size.times {|i|
        if waza_machine[ i ] == "��"
          machine[ machine_index ] |= flag
        end
        flag = flag << 1
        flag_cnt -= 1
        if flag_cnt == 0
          flag = 1
          flag_cnt = 32
          machine_index += 1
        end
      }

      data = [ hp,pow,defe,agi,spepow,spedef,type1,type2,get,rank,exp_value,item1,item2,item3,gender,birth,friend,grow,egg1,egg2,speabi1,speabi2,speabi3,escape,form_per_start,form_gra_start,form_max,col_rev,exp,height,weight,machine[0],machine[1],machine[2],machine[3] ].pack( "C10 S4 C10 S2 C2 S3 L4" )

	    data.size.times{ |c|
		    fp_per.printf("%c",data[ c ])
	    }

      fp_per.close
      printf( "per_%03d.bin �����I��\n", cnt )

      printf( "wot_%03d.bin ������\n", cnt )
      str = sprintf( "wot_%03d.bin", cnt )
      fp_wot = open( str, "wb" )
      waza_cnt = 0
      while split_data[ PARA::WAZA1 + waza_cnt ] != ""
        waza_no = $wazano_hash[ split_data[ PARA::WAZA1 + waza_cnt ] ]
        waza_lv = split_data[ PARA::WAZA_LV1 + waza_cnt ].to_i

        data = [ waza_no, waza_lv ].pack("S2")
	      data.size.times{ |c|
		      fp_wot.printf("%c",data[ c ])
	      }
        #�Z�o���n�b�V���e�[�u������
        wothash[ split_data[ PARA::POKENAME ] ] << split_data[ PARA::WAZA1 + waza_cnt ]
        waza_cnt += 1
        if waza_cnt == 25
          break
        end
      end

      data = [ 0xffff, 0xffff ].pack( "S2" )

	    data.size.times{ |c|
		    fp_wot.printf("%c",data[ c ])
	    }
      fp_wot.close
      printf( "wot_%03d.bin �����I��\n", cnt )

      printf( "evo_%03d.bin ������\n", cnt )
      str = sprintf( "evo_%03d.bin", cnt )
      fp_evo = open( str, "wb" )
      for evo_cnt in 0..EVO_MAX-1
        if split_data[ PARA::SHINKA_COND1 + evo_cnt ] == ""
          data = [ 0,0,0 ].pack( "S3" )
	        data.size.times{ |c|
		        fp_evo.printf("%c",data[ c ])
	        }
          next
        end
        if shinka_table[ split_data[ PARA::SHINKA_COND1 + evo_cnt ] ][ 0 ] == nil
          printf( "��`����Ă��Ȃ��i�������ł�:%s\n", split_data[ PARA::SHINKA_COND1 + evo_cnt ] )
          exit( 1 )
        end
        evo_cond = shinka_table[ split_data[ PARA::SHINKA_COND1 + evo_cnt ] ][ 0 ]
        case shinka_table[ split_data[ PARA::SHINKA_COND1 + evo_cnt ] ][ 2 ]
        when SHINKA_PARAM::NONE
          evo_param = 0
        when SHINKA_PARAM::LEVEL
          evo_param = split_data[ PARA::SHINKA_LEVEL ].to_i
        when SHINKA_PARAM::ITEM
          evo_param = $item_hash[ shinka_table[ split_data[ PARA::SHINKA_COND1 + evo_cnt ] ][ 1 ] ]
        when SHINKA_PARAM::WAZA
          evo_param = $wazano_hash[ shinka_table[ split_data[ PARA::SHINKA_COND1 + evo_cnt ] ][ 1 ] ]
        when SHINKA_PARAM::POKEMON
          evo_param = monsno[ shinka_table[ split_data[ PARA::SHINKA_COND1 + evo_cnt ] ][ 1 ] ]
        end
        if split_data[ PARA::SHINKA_POKE1 + evo_cnt ] == ""
          evo_mons = 0
        else
          evo_mons = monsno[ split_data[ PARA::SHINKA_POKE1 + evo_cnt ] ]
        end
        if split_data[ PARA::SHINKA_POKE1 + evo_cnt ] != ""
          seed[ monsno[ split_data[ PARA::SHINKA_POKE1 + evo_cnt ] ] ] = cnt
        end
        data = [ evo_cond, evo_param, evo_mons ].pack( "S3" )
        data.size.times{ |c|
          fp_evo.printf("%c",data[ c ])
        }
      end
      fp_evo.close
      printf( "evo_%03d.bin �����I��\n", cnt )
    end

    if other_form == 0 || ( other_form == 1 && split_data[ PARA::OTHERFORM ] == "��" )
      cnt += 1
    end
  }

  #�Z�o���n�b�V���e�[�u������
  fp_wothash = open( "wazaoboe_hash.rb", "w" )
  fp_wothash.printf("#! ruby -Ks\n\n" )
  fp_wothash.printf("\t$wazaoboe_hash = {\n" )

  monsname.size.times {|no|
    fp_wothash.printf( "\t\t\"%s\"=>[\n", monsname[ no ] )
    wothash[ monsname[ no ] ].size.times {|wot|
      fp_wothash.printf( "\t\t\t\t\"%s\",\n", wothash[ monsname[ no ] ][ wot ] )
    }
    fp_wothash.printf( "\t\t],\n", monsname[ no ] )
  }
  fp_wothash.printf("\t}\n" )
  fp_wothash.close

  print "�p�[�\�i���f�[�^�����I��\n"

  for i in 0..monsno_max - 1
    printf( "pms_%03d.bin ������\n", i )
    str = sprintf( "pms_%03d.bin", i )
    fp_pms = open( str, "wb" )
    pms = i
    pms_cnt = 0
    while seed[ pms ] != nil
      pms = seed[ pms ]
      pms_cnt += 1
      if pms_cnt > 7
        print "�i���f�[�^�Ɉُ킪����܂�\n"
		    exit( 1 )
      end
    end
    fp_pms.write( [ pms ].pack("s") )
    fp_pms.close
    printf( "pms_%03d.bin �����I��\n", i )
  end

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close


