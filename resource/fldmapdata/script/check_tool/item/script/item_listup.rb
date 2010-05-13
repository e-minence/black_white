###################################################################################
# @file   item_listup.rb
# @brief  .ev �t�@�C���ɒ�`���ꂽ�A�C�e����񋓂���
# @author obata
# @date   2010.05.10
###################################################################################
require "item_info.rb"
require "place_name_hash.rb"

PATH_TO_CHECK_DIR      = "../../../" # �`�F�b�N�Ώۂ̃f�B���N�g��
FILE_NAME_CHECK_TARGET = "*.ev"   # �`�F�b�N�Ώۂ̃t�@�C����
PATH_TO_CHECK_TARGET_FILE = PATH_TO_CHECK_DIR + FILE_NAME_CHECK_TARGET # �`�F�b�N�Ώۃt�@�C���ւ̃p�X

PATH_TO_RESULT_OUTPUT = "../result/" # ���ʂ̏o�͐�f�B���N�g���ւ̑��΃p�X
FILE_NAME_RESULT      = "result.txt" # ��͌��ʂ̏o�̓t�@�C����
FILE_NAME_FAILED      = "failed.txt" # ��͎��s���X�g�̏o�̓t�@�C����


# �`�F�b�N�ΏۊO�̃t�@�C���ꗗ
skip_file_list = Array.new
skip_file_list << PATH_TO_CHECK_DIR + "item_get_scr.ev"
skip_file_list << PATH_TO_CHECK_DIR + "debug_scr.ev"
skip_file_list << PATH_TO_CHECK_DIR + "hide_item.ev"
skip_file_list << PATH_TO_CHECK_DIR + "hyouka_scr.ev"
skip_file_list << PATH_TO_CHECK_DIR + "poke_waza.ev"
skip_file_list << PATH_TO_CHECK_DIR + "fld_item.ev"
skip_file_list << PATH_TO_CHECK_DIR + "c03r0101.ev" # �������X�N���v�g

# �`�F�b�N�Ɏ��s�����t�@�C���ꗗ
failed_file_list = Array.new

# �`�F�b�N����R�}���h�ꗗ
item_get_command_list = Array.new
item_get_command_list << "_ADD_ITEM"
item_get_command_list << "_EASY_TALK_ITEM_EVENT"
item_get_command_list << "_ITEM_EVENT_KEYWAIT"
item_get_command_list << "_ITEM_EVENT_NOWAIT"
item_get_command_list << "_ITEM_EVENT_FIELD"
item_get_command_list << "_ITEM_EVENT_HIDEITEM"
item_get_command_list << "_ITEM_EVENT_SUCCESS_KEYWAIT"
item_get_command_list << "_ITEM_EVENT_SUCCESS_NOWAIT"

# �`�F�b�N�ΏۃR�}���h ==> �A�C�e����`���x�����擾���邽�߂̐��K�\��
item_label_pattern = Hash.new
item_label_pattern[ "_ADD_ITEM" ]                   = /_ADD_ITEM.*\(\s*(ITEM_[^,]*),/
item_label_pattern[ "_EASY_TALK_ITEM_EVENT" ]       = /_EASY_TALK_ITEM_EVENT*\(\s*(ITEM_[^,]*),/
item_label_pattern[ "_ITEM_EVENT_KEYWAIT" ]         = /_ITEM_EVENT_KEYWAIT.*\(\s*(ITEM_[^,]*),/
item_label_pattern[ "_ITEM_EVENT_NOWAIT" ]          = /_ITEM_EVENT_NOWAIT.*\(\s*(ITEM_[^,]*),/
item_label_pattern[ "_ITEM_EVENT_FIELD" ]           = /_ITEM_EVENT_FIELD.*\(\s*(ITEM_[^,]*),/
item_label_pattern[ "_ITEM_EVENT_SUCCESS_KEYWAIT" ] = /_ITEM_EVENT_SUCCESS_KEYWAIT.*\(\s*(ITEM_[^,]*),/
item_label_pattern[ "_ITEM_EVENT_SUCCESS_NOWAIT" ]  = /_ITEM_EVENT_SUCCESS_NOWAIT.*\(\s*(ITEM_[^,]*),/

# �擾�A�C�e�����
item_info_list = Array.new


#----------------------------------------------------------------------------------
# @brief main
#----------------------------------------------------------------------------------
create_place_name_hash

check_file_list = Dir[ PATH_TO_CHECK_TARGET_FILE ]
check_file_list -= skip_file_list


check_file_list.each do |file_path|
  # �t�@�C���̓��e���擾
  filename = File.basename( file_path )
  file = File::open( file_path, "r" )
  file_lines = file.readlines
  file.close
  
  file_lines.each do |line|
    # �e�R�}���h���`�F�b�N
    item_get_command_list.each do |command|
      # �R�}���h�𔭌�
      if line.include?( command ) then
        # �A�C�e�����x�����𔭌�
        if item_label_pattern[ command ] =~ line then
          item_label = $1.strip
          info = ItemInfo.new
          info.file_name  = filename
          info.item_label = item_label
          info.zone_id    = GetZoneID_byEvFileName( filename )
          info.item_name  = GetItemName( item_label )
          info.zone_name  = @@place_name_hash[ info.zone_id ]
          item_info_list << info
        # �A�C�e�����x�������Ȃ�
        else
          failed_file_list << file_path
        end
      end
    end
  end

  puts filename
end


# ��͌��ʂ��o��
file = File::open( PATH_TO_RESULT_OUTPUT + FILE_NAME_RESULT, "w" )
item_info_list.each do |info|
  data = info.zone_id + "\t" + info.zone_name + "\t" + info.item_name
  file.puts( data )
end
file.close

# ��͎��s���X�g���o��
file = File::open( PATH_TO_RESULT_OUTPUT + FILE_NAME_FAILED, "w" )
file.puts( skip_file_list )
file.puts( failed_file_list )
file.close
