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
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

# �f�[�^�E�C���f�b�N�X
ROW_ZONE      = 0   # �]�[��ID
ROW_POS_X     = 1   # x���W
ROW_POS_Y     = 2   # y���W
ROW_POS_Z     = 3   # z���W
ROW_VOLUME_1  = 4   # ����1
ROW_VOLUME_2  = 5   # ����2
ROW_VOLUME_3  = 6   # ����3
ROW_VOLUME_4  = 7   # ����4
ROW_RANGE_X_1 = 8   # x����1
ROW_RANGE_X_2 = 9   # x����2
ROW_RANGE_X_3 = 10  # x����3
ROW_RANGE_X_4 = 11  # x����4
ROW_RANGE_Y_1 = 12  # y����1
ROW_RANGE_Y_2 = 13  # y����2
ROW_RANGE_Y_3 = 14  # y����3
ROW_RANGE_Y_4 = 15  # y����4
ROW_RANGE_Z_1 = 16  # z����1
ROW_RANGE_Z_2 = 17  # z����2
ROW_RANGE_Z_3 = 18  # z����3
ROW_RANGE_Z_4 = 19  # z����4

# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# �t�@�C���f�[�^��ǂݍ���
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 2�s�ڈȍ~�̑S���C�����R���o�[�g
1.upto( file_lines.size - 1 ) do |i|
  # �R���o�[�g�Ώۂ̃f�[�^���擾
  line = file_lines[i]
  in_data = line.split(/\s/)
  if in_data[0]==nil then break end # ��f�[�^�𔭌�==>�I��
  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_ZONE])
  out_data << 0  # padding
  out_data << in_data[ROW_POS_X].to_i
  out_data << in_data[ROW_POS_Y].to_i
  out_data << in_data[ROW_POS_Z].to_i
  out_data << in_data[ROW_VOLUME_1].to_i
  out_data << in_data[ROW_VOLUME_2].to_i
  out_data << in_data[ROW_VOLUME_3].to_i
  out_data << in_data[ROW_VOLUME_4].to_i
  out_data << in_data[ROW_RANGE_X_1].to_i
  out_data << in_data[ROW_RANGE_X_2].to_i
  out_data << in_data[ROW_RANGE_X_3].to_i
  out_data << in_data[ROW_RANGE_X_4].to_i
  out_data << in_data[ROW_RANGE_Y_1].to_i
  out_data << in_data[ROW_RANGE_Y_2].to_i
  out_data << in_data[ROW_RANGE_Y_3].to_i
  out_data << in_data[ROW_RANGE_Y_4].to_i
  out_data << in_data[ROW_RANGE_Z_1].to_i
  out_data << in_data[ROW_RANGE_Z_2].to_i
  out_data << in_data[ROW_RANGE_Z_3].to_i
  out_data << in_data[ROW_RANGE_Z_4].to_i
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_city_unit_" + in_data[ROW_ZONE].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("SSiiiCCCCCCCCCCCCCCCC") )
  file.close
  # �o�̓t�@�C�������L��
  bin_file_list << filename
end

# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end

# �o�͂����o�C�i���t�@�C�����X�g���쐬
str = "ISS_C_UNIT_DATA = "
bin_file_list.each do |filename|
  str += "\\"
  str += "\n"
  str += "#{filename} "
end
file = File.open( ARGV[1] + "/iss_city_unit.list", "w" )
file.write( str )
file.close 
