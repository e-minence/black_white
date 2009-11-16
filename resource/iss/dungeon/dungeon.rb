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
# @brief �L�[�̒l���v���O�����Ŏg�p����l�ɕϊ�����
# @param key �ϊ�����l
# @return �ϊ���̒l
#----------------------------------------------------------------------------------
def ConvertKey( key )

  # �L�[�̒l��64�{����(64=1����)
  return key * 64
end

#----------------------------------------------------------------------------------
# @brief �e���|�̒l���v���O�����Ŏg�p����l�ɕϊ�����
# @param tempo �ϊ�����l
# @return �ϊ���̒l
#----------------------------------------------------------------------------------
def ConvertTempo( tempo )

  # �e���|�̒l��� [-512, 512] �ɕ␳
  if tempo < -512 then tempo = -512 end
  if 512 < tempo then tempo = 512 end

  # �e���|�̒l�� [-512, 512] �� [0, 512] �ɕϊ�����
  tempo = ( tempo + 512 ) / 2 

  return tempo
end

#----------------------------------------------------------------------------------
# @brief ���o�[�u�̒l���v���O�����Ŏg�p����l�ɕϊ�����
# @param reverb �ϊ�����l
# @return �ϊ���̒l
#----------------------------------------------------------------------------------
def ConvertReverb( reverb )

  # ���o�[�u�̒l��64�{����
  return reverb * 64
end


#----------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

# �f�[�^�E�C���f�b�N�X
ROW_ZONE   = 0  # �]�[��ID
ROW_KEY    = 1  # �L�[
ROW_TEMPO  = 2  # �e���|
ROW_REVERB = 3  # ���o�[�u

# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# ���j�b�g�o�^�f�[�^
entry_data = Array.new

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
  out_data << ConvertKey(in_data[ROW_KEY].to_i)
  out_data << ConvertTempo(in_data[ROW_TEMPO].to_i)
  out_data << ConvertReverb(in_data[ROW_REVERB].to_i)
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/iss_dungeon_data_" + in_data[ROW_ZONE].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("SsSS") )
  file.close
  # �o�̓t�@�C�������L��
  bin_file_list << filename
  # ���j�b�g�o�^�f�[�^���L��
  entry_data << "ZONE_ID_"+in_data[ROW_ZONE]
end

# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end

# �o�͂����o�C�i���t�@�C�����X�g���쐬
str = "ISS_D_DATA = "
bin_file_list.each do |filename|
  str += "\\"
  str += "\n"
  str += "#{filename} "
end
file = File.open( ARGV[1] + "/iss_dungeon_data.list", "w" )
file.write( str )
file.close

# ���j�b�g�o�^�e�[�u�����o��
filename = "entry_table.cdat"
file = File.open( filename, "w")
file.puts("typedef struct")
file.puts("{")
file.puts("  u16 zoneID;")
file.puts("  ARCDATID datID;")
file.puts("} ENTRY_DATA;")
file.puts
file.puts("static const ENTRY_DATA entry_table[] =") 
file.puts("{")
0.upto(entry_data.size-1) do |i|
  zone_id = entry_data[i]
  dat_id = i
  file.puts("  { #{zone_id}, #{dat_id} },")
end
file.puts("};")
file.close
