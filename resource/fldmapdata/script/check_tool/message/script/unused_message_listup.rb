#################################################################################
# @brief  �g�p���Ă��Ȃ����b�Z�[�W�̃��X�g�A�b�v
# @file   unused_message_listup.rb
# @author obata
# @date   2010.05.13
################################################################################# 
require "message.rb"

RESULT_FILE      = "../result/result.txt" # ��͌��ʂ̏o�̓t�@�C��
EV_FILES         = "../../../*.ev" # �`�F�b�N�Ώۂ� .ev �t�@�C��
MSG_ID_DEF_FILES = "../../../../../../prog/include/msg/script/*.h" # ���b�Z�[�WID�̒�`�t�@�C��


# ���b�Z�[�W���X�g
all_message_list = Array.new
used_message_list = Array.new
unused_message_list = Array.new


#--------------------------------------------------------------------------------
# �����ׂẴ��b�Z�[�WID���擾
#--------------------------------------------------------------------------------
header_list = Dir[ MSG_ID_DEF_FILES ]
header_list.each do |filename|
  file = File.open( filename, "r" )
  invalid_id = File::basename( filename, ".*" ) + "_max"
  file.each do |line|
    if line =~ /#define\s+(msg_\w*)\s/ then
      str_id = $1.strip
      if str_id != invalid_id then
        all_message_list << Message.new( filename, str_id )
      end
    end
  end
  file.close
end

#--------------------------------------------------------------------------------
# �����ׂĂ̎g�p����Ă��郁�b�Z�[�WID���擾
#--------------------------------------------------------------------------------
ev_list = Dir[ EV_FILES ]

ev_list.each do |filename| 
  file = File.open( filename, "r" )
  in_comment = false

  file.each do |line|
    if in_comment then
      if line.include?( "*/" ) then 
        in_comment = false # �R�����g�I��
      end 
    else
      # C �R�����g ���O
      line.gsub!( /\/\*.*?\*\//, "" )
      # C �R�����g���������O
      if line.include?( "/*" ) then 
        comment_head = line.index( "/*" )
        line         = line[0...comment_head] 
        in_comment = true
      end
      # C++ �R�����g���������O
      if line.include?( "//" ) then 
        comment_head = line.index( "//" )
        line         = line[0...comment_head] 
      end
      # ���b�Z�[�WID�̎Q�ƃ`�F�b�N
      while line =~ /(msg_\w*)/
        str_id = $1.strip
        used_message_list << Message.new( filename, str_id )
        line.gsub!( str_id, "" ) # �����������b�Z�[�WID�͏��O����
      end
    end
  end

  file.close
end

#--------------------------------------------------------------------------------
# �����
#--------------------------------------------------------------------------------
all_message_list.each do |check_message| 
  used = false
  used_message_list.each do |used_message|
    if check_message.str_id == used_message.str_id then
      used = true
      break
    end
  end

  # �g�p����Ă��Ȃ�
  if used == false then
    puts check_message.str_id
    unused_message_list << check_message
  end
end

puts all_message_list.size
puts used_message_list.size
puts unused_message_list.size

#--------------------------------------------------------------------------------
# ����͌��ʂ��o��
#--------------------------------------------------------------------------------
file = File::open( RESULT_FILE, "w" )
unused_message_list.each do |message|
  file.puts( message.filename + ": " + message.str_id )
end
file.close
