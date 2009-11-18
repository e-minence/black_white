#�t�B�[���h�J�����f�[�^����

$KCODE = "SJIS"

COL_NUMBER  = 0
COL_LENGTH  = 1
COL_PITCH   = 2
COL_YAW     = 3
COL_ROLL    = 4
COL_VIEWTYPE = 5
COL_DEPTH   = 6
COL_FOVY    = 7
COL_NEAR    = 8
COL_FAR     = 9
COL_PLLINK  = 10
COL_OFSX    = 11
COL_OFSY    = 12
COL_OFSZ    = 13
COL_COMMENT = 14

class VEC_DATA
	#�O���Q�Ƃł���悤�ɂ���
	attr_accessor :X, :Y, :Z
	def initialize()
		@X = ""
		@Y = ""
		@Z = ""
	end
end

class CLIP_DATA
	#�O���Q�Ƃł���悤�ɂ���
	attr_accessor :Near, :Far
	def initialize()
		@Near = ""
		@Far = ""
	end
end

class CAMERA_DATA
	#�O���Q�Ƃł���悤�ɂ���
	attr_accessor :Distance, :Angle, :ViewType, :DepthType, :PerspWay, :Clip, :Shift, :Comment, :PlayerLink
	def initialize()
		@Distance = ""
		@Angle = VEC_DATA.new
		@ViewType = ""
    @DepthType = ""
		@PerspWay = ""
		@Clip = CLIP_DATA.new
		@Shift = VEC_DATA.new
		@Comment = ""
    @PlayerLink = ""
	end
end

#�}�C�i�X�`�F�b�N
def CheckMinus(integer, bit)
	#15�r�b�g�ڂ��`�F�b�N�������ǂ�������
	return integer[bit]
end

#�V�t�g�l������쐬
def MakeShiftStr(hex_str)
	#16�i��������𐮐���
	integer = hex_str.hex
	#�}�C�i�X�`�F�b�N
	if CheckMinus(integer,31) == 1 then
		pre_str = "-0x"
		#���E�l���獷������
		val = 0x100000000 - integer
	else
		pre_str = "0x"
		val = integer
	end
	#������
	str = format("%x", val)
	return pre_str + str
end

#�A���O���l������쐬
def MakeAngleStr(hex_str)
	#16�i��������𐮐���
	integer = hex_str.hex
	#���E�l���獷������
	val = (0x10000 - integer) % 0x10000

	#�l��0��?
	if val == 0 then
		pre_str = "0x"
	else
		#�}�C�i�X�`�F�b�N
		if CheckMinus(val,15) == 1 then
			#�}�C�i�X�Ȃ�v���X��
			pre_str = "0x"
		else
			#�v���X�Ȃ�}�C�i�X��
			pre_str = "-0x"
		end
	end
	#������
	str = format("%x", val)
	return pre_str + str
end

csv_file = open(ARGV.shift,"r")

data_array = Array.new

#��s�ǂݔ�΂�
line = csv_file.gets

while line = csv_file.gets
	column = line.split ","

  if column.size < 2 then break end
	data = CAMERA_DATA.new

	#����
	data.Distance = "0x" + column[COL_LENGTH]
	#�A���O��
	data.Angle.X = MakeShiftStr(column[COL_PITCH])
	data.Angle.Y = MakeShiftStr(column[COL_YAW])
	data.Angle.Z = MakeShiftStr(column[COL_ROLL])
	#data.Angle.X = MakeAngleStr(column[2])
	#data.Angle.Y = MakeAngleStr(column[3])
	#data.Angle.Z = MakeAngleStr(column[4])

	#�r���[�^�C�v
	if column[COL_VIEWTYPE] == "PER" then
		data.ViewType = "GF_CAMERA_PERSPECTIV"
	elsif column[COL_VIEWTYPE] == "ORTHO" then
		data.ViewType = "GF_CAMERA_ORTHO"
	else
		data.ViewType = "GF_CAMERA_PERSPECTIV"
	end
  if column[COL_DEPTH] == "ZBUF" then
    data.DepthType = "DEPTH_TYPE_ZBUF"
  elsif column[COL_DEPTH] == "WBUF" then
    data.DepthType = "DEPTH_TYPE_WBUF"
  else
    raise Exception
  end
	#�p�[�X
	data.PerspWay = "0x" + column[COL_FOVY]
	#�N���b�v
	data.Clip.Near = "FX32_ONE * 0x" + column[COL_NEAR]
	data.Clip.Far = "FX32_ONE * 0x" + column[COL_FAR]
	#data.Clip.Near = "FX32_ONE * " + column[7]
	#data.Clip.Far = "FX32_ONE * " + column[8]
  #�v���C���[�̃����N
  if column[COL_PLLINK] == "��" then
    data.PlayerLink = "1"
  else
    data.PlayerLink = "0"
  end
	#�V�t�g
	data.Shift.X = MakeShiftStr(column[COL_OFSX])
	data.Shift.Y = MakeShiftStr(column[COL_OFSY])
	data.Shift.Z = MakeShiftStr(column[COL_OFSZ])
	#�R�����g
	data.Comment = column[COL_COMMENT].chomp
	#�z��Ɋi�[
	data_array << data
end

#�t�@�C���ɏ�������
dat_file_content = ""
data_array.each_with_index{|data, i|
	dat_file_content += sprintf("\t{\t\t\t\t\t\t\t// #{i} #{data.Comment}\n")
	dat_file_content += sprintf("\t\t#{data.Distance},\n")
	dat_file_content += sprintf("\t\t{\n")
	dat_file_content += sprintf("\t\t\t#{data.Angle.X},#{data.Angle.Y},#{data.Angle.Z}\n")
	dat_file_content += sprintf("\t\t},\n")
	dat_file_content += sprintf("\t\t#{data.ViewType},\n")
  dat_file_content += sprintf("\t\t#{data.DepthType},\n")
	dat_file_content += sprintf("\t\t#{data.PerspWay},\n")
	dat_file_content += sprintf("\t\t#{data.Clip.Near},\n")
	dat_file_content += sprintf("\t\t#{data.Clip.Far},\n")
	dat_file_content += sprintf("\t\t#{data.PlayerLink},\n")
	dat_file_content += sprintf("\t\t{#{data.Shift.X},#{data.Shift.Y},#{data.Shift.Z}}\n")
	dat_file_content += sprintf("\t},\n")
}

dat_file = open("field_camera.dat","w")
dat_file.printf("//���̃t�@�C���̓R���o�[�^�ɂ��o�͂���܂�\n")

dat_file.printf("static const FLD_CAMERA_PARAM FieldCameraData[] = {\n")
dat_file.puts "#{dat_file_content}"

dat_file.printf("};\n")



