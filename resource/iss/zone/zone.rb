#---------------------------------------------------------------------------------- 
# @brief �]�[��������, �]�[��ID���擾����
# @param str_zone_id �]�[��ID��\��������
# @return �w�肵���]�[��ID�̒l
#---------------------------------------------------------------------------------- 
def GetZoneID( str_zone_id )

	# �t�@�C�����J��
	file_def = File.open( "../../../prog/arc/fieldmap/zone_id.h" )

	# 1�s�����Ă���
	file_def.each do |line|
		if line.index(str_zone_id)!=nil then
			i0 = line.index("(") + 1
			i1 = line.index(")") - 1
			def_val = line[i0..i1].to_i
			return def_val
		end
	end
	
	# �t�@�C�������
	file_def.close

  puts( "===============================================================" );
  puts( "error: �w�肳�ꂽ�]�[��ID[# str_zone_id}]�͒�`����Ă��܂���B" );
  puts( "===============================================================" );
end

#----------------------------------------------------------------------------------
# @brief �J���g���b�N�r�b�g���쐬����
# @param �e�g���b�N�̊J����Ԃ�ێ�����z��
# @return �w�肵���z�񂩂狁�߂��J���g���b�N�r�b�g
#----------------------------------------------------------------------------------
def CalcOpenTrackBit( track_state )

  track_bit = 0

  # �S�g���b�N�̏�Ԃ�����
  0.upto(track_state.size-1) do |i|
    if track_state[i]=="��" then 
      track_bit = track_bit | (1 << i)
    end
  end
  return track_bit
end

#----------------------------------------------------------------------------------
# @brief ���g���b�N�r�b�g���쐬����
# @param �e�g���b�N�̊J����Ԃ�ێ�����z��
# @return �w�肵���z�񂩂狁�߂����g���b�N�r�b�g
#----------------------------------------------------------------------------------
def CalcCloseTrackBit( track_state )

  # �J���g���b�N�r�b�g�𔽓]
  track_bit = CalcOpenTrackBit( track_state )
  return ~track_bit 
end

#----------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

# �f�[�^�E�C���f�b�N�X
ROW_ZONE       = 0   # �]�[��ID
ROW_TRACK_1    = 1   # �f�t�H���g����(�g���b�N1)
ROW_TRACK_2    = 2   # �f�t�H���g����(�g���b�N2)
ROW_TRACK_3    = 3   # �f�t�H���g����(�g���b�N3)
ROW_TRACK_4    = 4   # �f�t�H���g����(�g���b�N4)
ROW_TRACK_5    = 5   # �f�t�H���g����(�g���b�N5)
ROW_TRACK_6    = 6   # �f�t�H���g����(�g���b�N6)
ROW_TRACK_7    = 7   # �f�t�H���g����(�g���b�N7)
ROW_TRACK_8    = 8   # �f�t�H���g����(�g���b�N8)
ROW_TRACK_9    = 9   # �f�t�H���g����(�g���b�N9)
ROW_TRACK_10   = 10  # �f�t�H���g����(�g���b�N10)
ROW_TRACK_11   = 11  # �f�t�H���g����(�g���b�N11)
ROW_TRACK_12   = 12  # �f�t�H���g����(�g���b�N12)
ROW_TRACK_13   = 13  # �f�t�H���g����(�g���b�N13)
ROW_TRACK_14   = 14  # �f�t�H���g����(�g���b�N14)
ROW_TRACK_15   = 15  # �f�t�H���g����(�g���b�N15)
ROW_TRACK_16   = 16  # �f�t�H���g����(�g���b�N16)
ROW_FADE_FRAME = 17  # �t�F�[�h����

# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# �t�@�C���f�[�^��ǂݍ���
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 3�s�ڈȍ~�̑S���C�����R���o�[�g
2.upto( file_lines.size - 1 ) do |i|
  # �R���o�[�g�Ώۂ̃f�[�^���擾
  line = file_lines[i]
  in_data = line.split(/\s/)
  if in_data[0]==nil then break end # ��f�[�^�𔭌�==>�I��
  # �g���b�N��Ԕz����쐬
  track_state = Array.new
  track_state << in_data[ROW_TRACK_1]
  track_state << in_data[ROW_TRACK_2]
  track_state << in_data[ROW_TRACK_3]
  track_state << in_data[ROW_TRACK_4]
  track_state << in_data[ROW_TRACK_5]
  track_state << in_data[ROW_TRACK_6]
  track_state << in_data[ROW_TRACK_7]
  track_state << in_data[ROW_TRACK_8]
  track_state << in_data[ROW_TRACK_9]
  track_state << in_data[ROW_TRACK_10]
  track_state << in_data[ROW_TRACK_11]
  track_state << in_data[ROW_TRACK_12]
  track_state << in_data[ROW_TRACK_13]
  track_state << in_data[ROW_TRACK_14]
  track_state << in_data[ROW_TRACK_15]
  track_state << in_data[ROW_TRACK_16]
  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_ZONE])
  out_data << CalcOpenTrackBit( track_state )
  out_data << CalcCloseTrackBit( track_state )
  out_data << in_data[ROW_FADE_FRAME].to_i
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_zone_data_" + in_data[ROW_ZONE].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("SSSS") )
  file.close
  # �o�̓t�@�C�������L��
  bin_file_list << filename
end

# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end

# �o�͂����o�C�i���t�@�C�����X�g���쐬
str = "ISS_Z_DATA = "
bin_file_list.each do |filename|
  str += "\\"
  str += "\n"
  str += "#{filename} "
end
file = File.open( ARGV[1] + "/iss_zone_data.list", "w" )
file.write( str )
file.close
