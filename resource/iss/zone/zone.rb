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
ROW_DEFVOL_1  = 1   # �f�t�H���g����(�g���b�N1)
ROW_DEFVOL_2  = 2   # �f�t�H���g����(�g���b�N2)
ROW_DEFVOL_3  = 3   # �f�t�H���g����(�g���b�N3)
ROW_DEFVOL_4  = 4   # �f�t�H���g����(�g���b�N4)
ROW_DEFVOL_5  = 5   # �f�t�H���g����(�g���b�N5)
ROW_DEFVOL_6  = 6   # �f�t�H���g����(�g���b�N6)
ROW_DEFVOL_7  = 7   # �f�t�H���g����(�g���b�N7)
ROW_DEFVOL_8  = 8   # �f�t�H���g����(�g���b�N8)
ROW_DEFVOL_9  = 9   # �f�t�H���g����(�g���b�N9)
ROW_DEFVOL_10 = 10  # �f�t�H���g����(�g���b�N10)
ROW_DEFVOL_11 = 11  # �f�t�H���g����(�g���b�N11)
ROW_DEFVOL_12 = 12  # �f�t�H���g����(�g���b�N12)
ROW_DEFVOL_13 = 13  # �f�t�H���g����(�g���b�N13)
ROW_DEFVOL_14 = 14  # �f�t�H���g����(�g���b�N14)
ROW_DEFVOL_15 = 15  # �f�t�H���g����(�g���b�N15)
ROW_DEFVOL_16 = 16  # �f�t�H���g����(�g���b�N16)

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
  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_ZONE])
  out_data << 0  # padding
  out_data << in_data[ROW_DEFVOL_1].to_i
  out_data << in_data[ROW_DEFVOL_2].to_i
  out_data << in_data[ROW_DEFVOL_3].to_i
  out_data << in_data[ROW_DEFVOL_4].to_i
  out_data << in_data[ROW_DEFVOL_5].to_i
  out_data << in_data[ROW_DEFVOL_6].to_i
  out_data << in_data[ROW_DEFVOL_7].to_i
  out_data << in_data[ROW_DEFVOL_8].to_i
  out_data << in_data[ROW_DEFVOL_9].to_i
  out_data << in_data[ROW_DEFVOL_10].to_i
  out_data << in_data[ROW_DEFVOL_11].to_i
  out_data << in_data[ROW_DEFVOL_12].to_i
  out_data << in_data[ROW_DEFVOL_13].to_i
  out_data << in_data[ROW_DEFVOL_14].to_i
  out_data << in_data[ROW_DEFVOL_15].to_i
  out_data << in_data[ROW_DEFVOL_16].to_i
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_zone_data_" + in_data[ROW_ZONE].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("SSCCCCCCCCCCCCCCCC") )
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
