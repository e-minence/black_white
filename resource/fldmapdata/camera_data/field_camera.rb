#�t�B�[���h�J�����f�[�^����

$KCODE = "SJIS"

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
	attr_accessor :Distance, :Angle, :ViewType, :PerspWay, :Clip, :Shift, :Comment
	def initialize()
		@Distance = ""
		@Angle = VEC_DATA.new
		@ViewType = ""
		@PerspWay = ""
		@Clip = CLIP_DATA.new
		@Shift = VEC_DATA.new
		@Comment = ""
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
dat_file = open("field_camera.dat","w")

data_array = Array.new

#��s�ǂݔ�΂�
line = csv_file.gets

while line = csv_file.gets
	column = line.split ","

	data = CAMERA_DATA.new

	#����
	data.Distance = "0x" + column[1]
	#�A���O��
	data.Angle.X = MakeAngleStr(column[2])
	data.Angle.Y = MakeAngleStr(column[3])
	data.Angle.Z = MakeAngleStr(column[4])

	#�r���[�^�C�v
	if column[5] == "PER" then
		data.ViewType = "GF_CAMERA_PERSPECTIV"
	elsif column[5] == "ORTHO" then
		data.ViewType = "GF_CAMERA_ORTHO"
	else
		data.ViewType = "GF_CAMERA_PERSPECTIV"
	end
	#�p�[�X
	data.PerspWay = "0x" + column[6]
	#�N���b�v
	data.Clip.Near = "FX32_ONE * " + column[7]
	data.Clip.Far = "FX32_ONE * " + column[8]
	#�V�t�g
	data.Shift.X = MakeShiftStr(column[9])
	data.Shift.Y = MakeShiftStr(column[10])
	data.Shift.Z = MakeShiftStr(column[11])
	#�R�����g
	data.Comment = column[12].chomp
	#�z��Ɋi�[
	data_array << data
end

#�t�@�C���ɏ�������
dat_file.printf("//���̃t�@�C���̓R���o�[�^�ɂ��o�͂���܂�\n")

dat_file.printf("static const FLD_CAMERA_PARAM FieldCameraData[] = {\n")

data_array.each_with_index{|data, i|
	dat_file.printf("\t{\t\t\t\t\t\t\t// #{i} #{data.Comment}\n")
	dat_file.printf("\t\t#{data.Distance},\n")
	dat_file.printf("\t\t{\n")
	dat_file.printf("\t\t\t#{data.Angle.X},#{data.Angle.Y},#{data.Angle.Z}\n")
	dat_file.printf("\t\t},\n")
	dat_file.printf("\t\t#{data.ViewType},\n")
	dat_file.printf("\t\t#{data.PerspWay},\n")
	dat_file.printf("\t\t#{data.Clip.Near},\n")
	dat_file.printf("\t\t#{data.Clip.Far},\n")
	dat_file.printf("\t\t{#{data.Shift.X},#{data.Shift.Y},#{data.Shift.Z}}\n")
	dat_file.printf("\t},\n")
}

dat_file.printf("};\n")



