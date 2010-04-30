#################################################################
# @brief  �Q�[�����|�P���������f�[�^ �R���o�[�^
# @file   fld_trade_poke.h
# @author obata
# @date   2009.12.11
#################################################################
require "gmm_generator.rb"


#===================================================================
# ���s�C���f�b�N�X
#===================================================================
ROW_MONSNO        = 1   # �����X�^�[�i���o�[ 
ROW_NICKNAME      = 2   # �j�b�N�l�[��
ROW_FORM          = 3   # �t�H�[���i���o�[
ROW_LEVEL         = 4   # ���x��
ROW_HP_RND        = 5   # HP����
ROW_AT_RND        = 6   # �U������
ROW_DF_RND        = 7   # �h�䗐��
ROW_AG_RND        = 8   # ��������
ROW_SA_RND        = 9   # ���U����
ROW_SD_RND        = 10  # ���h����
ROW_TOKUSEI       = 11  # ����\��
ROW_SEIKAKU       = 12  # ���i
ROW_SEX           = 13  # ����
ROW_ID            = 14  # ID
ROW_STYPE         = 15  # �������悳
ROW_BEAUTIFUL     = 16  # ��������
ROW_CUTE          = 17  # ���킢��
ROW_CLEVER        = 18  # ��������
ROW_STRONG        = 19  # �����܂���
ROW_ITEM          = 20  # �A�C�e��
ROW_OYA_NAME      = 21  # �e��
ROW_OYA_SEX       = 22  # �e����
ROW_FUR           = 23  # �щ�
ROW_COUNTRY       = 24  # �e�̍��R�[�h
ROW_CHANGE_MONSNO = 25  # �������郂���X�^�[�i���o�[
ROW_CHANGE_SEX    = 26  # ��������|�P�����̐���
ROW_STRID_NICKNAME= 27  # �j�b�N�l�[�����x��
ROW_STRID_OYA_NAME= 28  # �e�����x��

#===================================================================
# ���o�̓f�[�^
#===================================================================
class OutputData
  #--------
  # ������
  #--------
	def initialize
    # ���x��
    @label_nickname      = nil  # �j�b�N�l�[��
    @label_monsno        = nil  # �����X�^�[�i���o�[
    @label_formno        = nil  # �t�H�[���i���o�[
    @label_tokusei       = nil  # ����\��
    @label_seikaku       = nil  # ���i
    @label_sex           = nil  # ����
    @label_item          = nil  # �A�C�e��
    @label_oya_name      = nil  # �e��
    @label_oya_sex       = nil  # �e����
    @label_country       = nil  # �e�̍��R�[�h
    @label_chnage_monsno = nil  # �������郂���X�^�[�i���o�[
    @label_change_sex    = nil  # ��������|�P�����̐���
    @label_str_id_nickname = nil # �j�b�N�l�[���̕�����ID
    @label_str_id_oya_name = nil # �e���̕�����ID
    # �f�[�^
    @trade_no      = 0  # ����No.
		@monsno        = 0  # �����X�^�[�i���o�[  
		@formno        = 0  # �t�H�[���i���o�[
		@level         = 0  # ���x��
		@hp_rnd        = 0  # HP����
		@at_rnd        = 0  # �U������
		@df_rnd        = 0  # �h�䗐��
		@ag_rnd        = 0  # ��������
		@sa_rnd        = 0  # ���U����
		@sd_rnd        = 0  # ���h����
		@tokusei_idx   = 0  # ���� ( ���Ԗڂ̓����� )
		@seikaku       = 0  # ���i
		@sex           = 0  # ����
		@id            = 0  # ID
		@style         = 0  # �������悳
		@beautiful     = 0  # ��������
		@cute          = 0  # ���킢��
		@clever        = 0  # ��������
		@strong        = 0  # �����܂���
		@item          = 0  # �A�C�e��
		@oya_sex       = 0  # �e����
		@fur           = 0  # �щ�
		@country       = 0  # �e�̍��R�[�h
		@change_monsno = 0  # �������郂���X�^�[�i���o�[
		@change_sex    = 0  # ��������|�P�����̐���
    @str_id_nickname= 0  # �j�b�N�l�[���̕�����ID
    @str_id_oya_name= 0  # �e���̕�����ID
	end 
  #------------
	# �A�N�Z�b�T
  #------------
	attr_accessor :label_nickname, :label_monsno, :label_formno, 
                :label_tokusei, :label_seikaku, :label_sex, :label_item, 
                :label_oya_name, :label_oya_sex, :label_country,
                :label_change_monsno, :label_change_sex,
                :label_str_id_nickname, :label_str_id_oya_name,
                :trade_no, :monsno, :formno, :level,
		            :hp_rnd, :at_rnd, :df_rnd, :ag_rnd, :sa_rnd, :sd_rnd,
                :tokusei_idx, :seikaku, :sex, :id,
                :style, :beautiful, :cute, :clever, :strong,
                :item, :oya_sex, :fur, :country, 
                :change_monsno, :change_sex,
                :str_id_nickname, :str_id_oya_name
  # �f�o�b�O�o��
  def debug_out(path)
    # �f�[�^���ݒ肳��Ă��Ȃ�
    if @label_monsno == nil then return end
    # �t�@�C�����𐶐�
    monsname = @label_monsno.sub("MONSNO_", "").downcase
    filename = path + "/trade_poke_#{@trade_no}_#{monsname}.txt"
    # �t�@�C���ɏo��
    file = File::open(filename, "w")
    file.puts("TRADE_NO      = #{trade_no}")
    file.puts("NICKNAME      = #{@label_nickname}")
    file.puts("MONSNO        = #{@monsno}(#{@label_monsno})")
    file.puts("FORMNO        = #{@formno}(#{@label_formno})")
    file.puts("LEVEL         = #{@level}")
    file.puts("HP_RND        = #{@hp_rnd}")
    file.puts("DF_RND        = #{@df_rnd}")
    file.puts("AG_RND        = #{@ag_rnd}")
    file.puts("SA_RND        = #{@sa_rnd}")
    file.puts("SD_RND        = #{@sd_rnd}")
    file.puts("TOKUSEI       = #{@tokusei_idx}(#{@label_tokusei})")
    file.puts("SEIKAKU       = #{@seikaku}(#{@label_seikaku})")
    file.puts("SEX           = #{@sex}(#{@label_sex})")
    file.puts("ID            = #{@id}")
    file.puts("STYLE         = #{@style}")
    file.puts("BEAUTIFUL     = #{@beautiful}")
    file.puts("CUTE          = #{@cute}")
    file.puts("CLEVER        = #{@clever}")
    file.puts("STRONG        = #{@strong}")
    file.puts("ITEM          = #{@item}(#{@label_item})")
    file.puts("OYA_NAME      = #{@label_oya_name}")
    file.puts("OYA_SEX       = #{@oya_sex}(#{@label_oya_sex})")
    file.puts("FUR           = #{@fur}")
    file.puts("COUNTRY       = #{@country}(#{@label_country})")
    file.puts("CHANGE_MONSNO = #{@change_monsno}(#{@label_change_monsno})")
    file.puts("CHANGE_SEX    = #{@change_sex}(#{@label_change_sex})")
    file.puts("STRID_NICKNAME = #{@str_id_nickname}(#{label_str_id_nickname})")
    file.puts("STRID_OYA_NAME = #{@str_id_oya_name}(#{label_str_id_oya_name})")
    file.close
  end
  #------------------------
  # �o�C�i���t�@�C�����擾
  #------------------------
  def binary_filename
    monsname = @label_monsno.sub("MONSNO_", "").downcase
    filename = "trade_poke_#{@trade_no}_#{monsname}.bin"
    return filename
  end
  #--------------
  # �o�C�i���o��
  #--------------
  def binary_out(path)
    file = File::open( path + "/" + self.binary_filename, "wb")
    data = Array::new
    data << @trade_no << @monsno << @formno << @level <<
            @hp_rnd << @at_rnd << @df_rnd << @ag_rnd << @sa_rnd << @sd_rnd <<
            @tokusei_idx << @seikaku << @sex << @id <<
            @style << @beautiful << @cute << @clever << @strong <<
            @item << @oya_sex << @fur << @country <<
            @change_monsno << @change_sex <<
            @str_id_nickname << @str_id_oya_name
    file.write(data.pack("I*"))
    file.close
  end
  #-------------------------------
  # �����f�[�^No.��`���x�����擾
  #-------------------------------
  def def_label
    monsname = @label_monsno.sub("MONSNO_", "")
    label    = "FLD_TRADE_POKE_#{@trade_no + 1}_#{monsname}"
    return label
  end
end

#===================================================================
# ���T�u���[�`��
#===================================================================

#-----------------------------------------------------------
# @brief �����f�[�^����`����Ă���S�Ă̗�ԍ�����������
# @return �����f�[�^�̗�ԍ��z��
#-----------------------------------------------------------
def SearchPokeDataColumn()
	columns   = Array::new
	# �t�@�C���ǂݍ���
	file      = File::open(ARGV[0], "r")
	line      = file.readlines[ROW_MONSNO]
	file.close
	# ����
	line_data = line.split(/\s/)
	0.upto(line_data.size-1) do |idx|
		if line_data[idx].index("MONSNO_")!=nil then
			columns << idx
		end
	end
	return columns
end

#-----------------------------------------------------------
# @brief �����X�^�[No.�̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetMonsnoVal( label )
  #----------------------------------------
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("monsno_def.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end 
  #----------------------------------------
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("monsnum_def.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: �����X�^�[No.(#{label})�͒�`����Ă��܂���B")
end

#-----------------------------------------------------------
# @brief �t�H�[��No.�̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetFormnoVal( label )
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("monsno_def.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
  return 0
end

#-----------------------------------------------------------
# @brief �A�C�e���̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetItemVal( label )
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("itemsym.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: �A�C�e��(#{label})�͒�`����Ă��܂���B")
end

#-----------------------------------------------------------
# @brief ���ʂ̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetSexVal( label )
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("pm_version.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}\s*(\d*)\s*/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: ����(#{label})�͒�`����Ă��܂���B")
end

#-----------------------------------------------------------
# @brief ����R�[�h�̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetLangCodeVal( label )
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("pm_version.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\s*(\d*)\s/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: ����R�[�h(#{label})�͒�`����Ă��܂���B")
end

#-----------------------------------------------------------
# @brief �����̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetTokuseiVal( label )
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("tokusyu_def.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: ����(#{label})�͒�`����Ă��܂���B")
end

#-----------------------------------------------------------
# @brief �����̃C���f�b�N�X�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetTokuseiIndex( label )
  hash = Hash.new
  hash[ "TOKUSEI_1" ] = 0
  hash[ "TOKUSEI_2" ] = 1
  hash[ "TOKUSEI_3" ] = 2

  if hash.has_key?(label) == false then
    abort( "����(#{label})�̎w�肪�Ԉ���Ă��܂�\n" )
  end

  return hash[ label ]
end

#-----------------------------------------------------------
# @brief ���i�̒�`�ԍ����擾����
# @param label ���x����
# @return �w�胉�x�����̒�`�l
#-----------------------------------------------------------
def GetSeikakuVal( label )
	# �Q�ƃt�@�C���ǂݍ���
	file = File::open("poke_tool.h", "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{label}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: ���i(#{label})�͒�`����Ă��܂���B")
end


#===================================================================
# �����C��
#===================================================================

#-------------------------------------------------------------------
# @brief �Q�[���������f�[�^���R���o�[�g����
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C��
# @param ARGV[1] �o�C�i���o�͐�
#-------------------------------------------------------------------
trade_data = Array::new
# �t�@�C���f�[�^�ǂݍ���
file = File::open(ARGV[0], "r")
file_data = file.readlines
file.close
row_data = Array.new
file_data.each do |row|
  row_data << row.split(/\s/)
end
# �L���f�[�^�̗�ԍ����擾
column_idx = SearchPokeDataColumn()
trade_no = 0
# �S�f�[�^���擾
column_idx.each do |idx|
  data = OutputData::new
  # ������Ƃ��Ď擾
  data.trade_no              = trade_no
  data.label_monsno          = row_data[ROW_MONSNO       ][idx]
  data.label_nickname        = row_data[ROW_NICKNAME     ][idx]
  data.label_formno          = row_data[ROW_FORM         ][idx]
  data.level                 = row_data[ROW_LEVEL        ][idx]
  data.hp_rnd                = row_data[ROW_HP_RND       ][idx]
  data.at_rnd                = row_data[ROW_AT_RND       ][idx]
  data.df_rnd                = row_data[ROW_DF_RND       ][idx]
  data.ag_rnd                = row_data[ROW_AG_RND       ][idx]
  data.sa_rnd                = row_data[ROW_SA_RND       ][idx]
  data.sd_rnd                = row_data[ROW_SD_RND       ][idx]
  data.label_tokusei         = row_data[ROW_TOKUSEI      ][idx]
  data.label_seikaku         = row_data[ROW_SEIKAKU      ][idx]
  data.label_sex             = row_data[ROW_SEX          ][idx]
  data.id                    = row_data[ROW_ID           ][idx]
  data.style                 = row_data[ROW_STYPE        ][idx]
  data.beautiful             = row_data[ROW_BEAUTIFUL    ][idx]
  data.cute                  = row_data[ROW_CUTE         ][idx]
  data.clever                = row_data[ROW_CLEVER       ][idx]
  data.strong                = row_data[ROW_STRONG       ][idx]
  data.label_item            = row_data[ROW_ITEM         ][idx]
  data.label_oya_name        = row_data[ROW_OYA_NAME     ][idx]
  data.label_oya_sex         = row_data[ROW_OYA_SEX      ][idx]
  data.fur                   = row_data[ROW_FUR          ][idx]
  data.label_country         = row_data[ROW_COUNTRY      ][idx]
  data.label_change_monsno   = row_data[ROW_CHANGE_MONSNO][idx]
  data.label_change_sex      = row_data[ROW_CHANGE_SEX   ][idx]
  data.label_str_id_nickname = row_data[ROW_STRID_NICKNAME][idx]
  data.label_str_id_oya_name = row_data[ROW_STRID_OYA_NAME][idx]
  # �l�ɕϊ�
  data.monsno        = GetMonsnoVal(data.label_monsno)
  data.formno        = GetFormnoVal(data.label_formno)
  data.level         = data.level.to_i
  data.hp_rnd        = data.hp_rnd.to_i
  data.at_rnd        = data.at_rnd.to_i
  data.df_rnd        = data.df_rnd.to_i
  data.ag_rnd        = data.ag_rnd.to_i
  data.sa_rnd        = data.sa_rnd.to_i
  data.sd_rnd        = data.sd_rnd.to_i
  data.tokusei_idx   = GetTokuseiIndex(data.label_tokusei)
  data.seikaku       = GetSeikakuVal(data.label_seikaku)
  data.sex           = GetSexVal(data.label_sex)
  data.id            = data.id.to_i
  data.style         = data.style.to_i
  data.beautiful     = data.beautiful.to_i
  data.cute          = data.cute.to_i
  data.clever        = data.clever.to_i
  data.strong        = data.strong.to_i
  data.item          = GetItemVal(data.label_item)
  data.oya_sex       = GetSexVal(data.label_oya_sex)
  data.fur           = data.fur.to_i
  data.country       = GetLangCodeVal(data.label_country)
  data.change_monsno = GetMonsnoVal(data.label_change_monsno)
  data.change_sex    = GetSexVal(data.label_change_sex)
  data.str_id_nickname = trade_no * 2
  data.str_id_oya_name = trade_no * 2 + 1
  # �z��ɓo�^
  trade_data << data
  # ����No.���C���N�������g
  trade_no = trade_no + 1
end 
# �f�o�b�O�o��
trade_data.each do |data|
  data.debug_out(ARGV[1])
end
# �o�C�i���o��
trade_data.each do |data|
  data.binary_out(ARGV[1])
end
# �o�C�i�� ���X�g�o��
file = File::open("fld_trade_list.lst", "w")
trade_data.each do |data|
  file.puts("\"#{ARGV[1]}/#{data.binary_filename}\"")
end
file.close
# �w�b�_�[�o��
file = File::open("fld_trade_list.h", "w")
comment = "
//-------------------------------------------------
/**
 * @brief �����f�[�^�w��ID
 * @file fld_trade_list.h
 * @author obata
 *
 * fld_trade_poke.rb �ɂ�莩������
 */ 
//-------------------------------------------------"
file.puts(comment)
val = 0
trade_data.each do |data|
  label = data.def_label
  file.puts("#define #{label} (#{val})")
  val = val + 1
end
file.puts("#define FLD_TRADE_POKE_NUM (#{val})")
file.close


# gmm ���o��
directory = "./"
filename = "fld_trade.gmm"
stringLabel = Array.new
string = Array.new
trade_data.each do |data|
  stringLabel << data.label_str_id_nickname
  stringLabel << data.label_str_id_oya_name
  string << data.label_nickname
  string << data.label_oya_name
end
GenerateGMM( directory, filename, stringLabel, string, string )
