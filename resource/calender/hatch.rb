###################################################################
#
# @brief  �z���E�J�����_�[�̃R���o�[�g
# @author obata
# @date   2009.08.04
#
# === �o�C�i���E�f�[�^�E�t�H�[�}�b�g ===
# 
# [0] �f�[�^��(u16)
# [1]
# [2] ��(u8)              |
# [3] ��(u8)              |----- �f�[�^���̕������J��Ԃ�
# [4] �z���ω��ԍ�(s8)    |
# 
###################################################################

#==================================================================
#
# @brief ���t�����񂩂�, ���̐��l�����o��
#
# @param str_date ���t��\��������(*��*��)
#
# @return ���̒l
#
#==================================================================
def GetMonthValue( str_date )

  # "��"�̈ʒu�����߂�
  month_index = str_date.index( "��" )

  # ���E���̒l�𐔒l�ɕϊ�
  val = str_date[0..month_index-1].to_i

  # ���̒l��Ԃ�
  return val

end
  

#==================================================================
#
# @brief ���t�����񂩂�, ���̐��l�����o��
#
# @param str_date ���t��\��������(*��*��)
#
# @return ���̒l
#
#==================================================================
def GetDayValue( str_date )

  # ���E���̈ʒu�����߂�
  month_index = str_date.index( "��" )
  date_index  = str_date.index( "��" )

  # ���̒l�𐔒l�ɕϊ�
  val = str_date[month_index+2..date_index-1].to_i

  # ���̒l��Ԃ�
  return val

end


#==================================================================
#
# @brief ���C��
#
#==================================================================

LINE_INDEX_START = 1 # ���f�[�^�̐擪�s�ԍ�
ROW_INDEX_DATE   = 2 # ���t�f�[�^�̗�ԍ�
ROW_INDEX_HATCH  = 3 # �z���ω��ԍ��̗�ԍ�

SRC_FILENAME = "hatch.txt"  # ���̓t�@�C����
DST_FILENAME = "hatch.bin"  # �o�̓t�@�C����


line_index = 0;    # �s�C���f�b�N�X
month     = Array.new  # ���z��
date      = Array.new  # ���z��
hatch_no  = Array.new  # �z���ω��ԍ��z��


#-----------------------------------
# �ǂݍ���
#-----------------------------------

# �t�@�C�����J��
file  = File.open( SRC_FILENAME )

# ���ׂĂ̍s�ɂ��ď�������
file.each do |line|

	# ��f�[�^��������, �I��
	empty_test = line.gsub( /\s/, "" )
	if( empty_test == "" ) then
		break;
	end

  # ���f�[�^�݂̂���������
	if( LINE_INDEX_START <= line_index ) then

		# �^�u�ŋ�؂�
		line_split = line.split( /\t/ )

    # �e�f�[�^�����o��
    month     << GetMonthValue( line_split[ ROW_INDEX_DATE ] )
    date      << GetDayValue( line_split[ ROW_INDEX_DATE ] )
    hatch_no  << line_split[ ROW_INDEX_HATCH ].to_i

  end

	# �s�����J�E���g
	line_index += 1
end

# �t�@�C�������
file.close


#-----------------------------------
# ��������
#-----------------------------------

# �t�@�C�����J��
file = File.open( DST_FILENAME, "wb" )

# OUT: �f�[�^��(u16)
file.write( [ date.length ].pack( "S" ) )

# ���ׂẴf�[�^���o�͂���
0.upto( date.length - 1 ) do |i| 

  # OUT: ��(u8), ��(u8), �ω��ԍ�(u8)
  write_data = [ month[i], date[i], hatch_no[i] ]
  file.write( write_data.pack( "CCC" ) )
  
end

# �t�@�C�������
file.close


#-----------------------------------
# DEBUG:
#-----------------------------------
=begin
puts "data num = " + date.length.to_s
0.upto( date.length - 1 ) do |i|
  puts month[i].to_s + "/" + date[i].to_s + " = " + hatch_no[i].to_s
end
=end
