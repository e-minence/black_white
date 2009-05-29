#------------------------------------------------------------------------------
#	//�G���A�Ǘ��\���J��
#	//�w�b�_�s�ǂݔ�΂�
#	//�G���A�R�[�h�A�z�u���f���f�[�^�A�e�N�X�`���Z�b�g�A�l���f�[�^�A���C�g�t���O�����[�h
#	//���ꂼ����x�N�^�o�^
#	//�o�^�̍ہA�o�^�C���f�b�N�X��Ԃ��悤�ɂ���
#	//�Ԃ��Ă����C���f�b�N�X���G���A�f�[�^�Ƃ��ĕۑ�
#	//���C�g�t���O�ۑ�
#	//�G���AID���J��Ԃ��A�I��������A�e�x�N�^���A�[�J�C�u�p�X�N���v�g�t�@�C���Ƃ��ďo��
#
#
#	2008.12.18	tamada	wb���ɈڐA�J�n
#	2009.05.18  tamada  light/itp/ita�̊O���f�[�^(*.naix)�Q�ƂɑΉ�
#	2009.05.19  tamada  �G���[�ɂȂ��Ă��o�̓t�@�C������������Ă��邽�߂�
#	                    �ēxmake����Ɩ��Ȃ��ʂ��Ă��܂��Ώ�
#
#------------------------------------------------------------------------------

load 'area_common.def'	#COL_�`�̒�`�Ȃ�

#NAIXReader�̓ǂݍ���
lib_path_naixread = ENV["PROJECT_ROOT"] + "tools/naixread"
require lib_path_naixread

NO_ANIME_ID = 0xff

#�z��ւ̃G���g��
def EntryVec(vec, item)
	#���łɂ��邩���ׂ�
	index = vec.index(item)
	if index == nil then
		#�o�^
		vec << item
		return (vec.length-1)
	else
		return index
	end
end

#�����O�ݒ�擾
def GetInnerOuter(str)
	fix_str = str.chomp("\n").chomp("\r")
	if "OUT" == fix_str then
		return 1;
	elsif "IN" == fix_str then
		return 0;
	end
	#�w��s���̏ꍇ��0���Z�b�g
	return 0;
end



##	�G���[���`
class DataTypeError < Exception; end

##	�G���AID�����p
class AreaIDMaker
	def initialize(filename)
		@area_count = 0
		@area_names = {}
		@before_name = ""
		@season_count = 0

		@wf = File.open(filename, "w")
		@wf.printf("//���̃t�@�C���̓R���o�[�^�ɂ�萶������܂�\n")
		@wf.printf("enum {\n")
	end

	def close
		@wf.printf("\tAREA_ID_MAX\n");
		@wf.printf("};")
		@wf.close
	end

	def putLine(name)
		raise DataTypeError, "#{name}�����d��`����Ă��܂�" if @area_names.has_key?(name)
		@area_names[name] = @area_count
		@wf.printf("\t%-24s = %3d,\n",name.upcase, @area_count)
	end

	def putAreaName(column)
		area_id = column[COL_AREANAME]
		season_id = column[COL_SEASONID]
		case season_id
		when "SPRING"
			raise DataTypeError, "#{area_id}�F#{season_id}�̒��O����������" if @season_count != 0
			putLine(area_id)
			putLine(area_id + "_" + season_id)
			@season_count += 1
			@before_name = area_id
		when "SUMMER"
			raise DataTypeError, "#{area_id}�F#{season_id}�̒��O����������" if @season_count != 1
			raise DataTypeError, "#{area_id}�w���#{season_id}�ɕs�v�ł�" if area_id != ""
			putLine(@before_name + "_" + season_id)
			@season_count += 1
		when "AUTUMN"
			raise DataTypeError, "#{area_id}�F#{season_id}�̒��O����������" if @season_count != 2
			raise DataTypeError, "#{area_id}�w���#{season_id}�ɕs�v�ł�" if area_id != ""
			putLine(@before_name + "_" + season_id)
			@season_count += 1
		when "WINTER"
			raise DataTypeError, "#{area_id}�F#{season_id}�̒��O����������" if @season_count != 3
			raise DataTypeError, "#{area_id}�w���#{season_id}�ɕs�v�ł�" if area_id != ""
			putLine(@before_name + "_" + season_id)
			@season_count = 0
			@before_name = ""
		when "NO"
			raise DataTypeError, "#{area_id}�̒��O����������" if @season_count != 0
			putLine(area_id)
			@season_count = 0
			@before_name = ""
		else
			raise DataTypeError, "�G�ߎw��Ɏg���Ȃ�'#{season_id}'��#{area_id}�ɂ���܂���"
		end
		@area_count += 1
	end

end


def enum_search naix, name
  if name == "none" then return NO_ANIME_ID end
  index = naix.getIndex(name.sub(/\./,"_"))
  if index == nil then
    puts "#{naix.getArchiveName}��#{name}���݂���Ȃ��I"
    return NO_ANIME_ID
  end
  return index
end


#------------------------------------------------------------------------------
#	�R���o�[�g�{��
#------------------------------------------------------------------------------

begin
  ita_enum = NAIXReader.read("area_map_ita/area_map_ita.naix")
  itp_enum = NAIXReader.read("area_map_itp/area_map_itp_tbl.naix")
  light_enum = NAIXReader.read("../../field_light/field_light.naix")

  area_tbl_file = File.open(ARGV[0],"r")
  id_file = AreaIDMaker.new(TARGET_HEADER_FILENAME)

  build_vec = []
  tex_vec = []

  total_txt_file = File.open("area_data.txt", "w")

  #�s�ǂݔ�΂�
	READTHROUGH_LINES.times{|| area_tbl_file.gets }

  area_count = 0
  bin_out = ""
  area_tbl_file.each{|line|
    column = line.split "\t"

    #�G���A�h�c��
    id_file.putAreaName(column)

    #bin�t�@�C���쐬
    #�f�[�^��������
    bm_id = EntryVec(build_vec, column[COL_BMNAME])	#���f��
    bin_out += [bm_id].pack("S") 

    tex_id = EntryVec(tex_vec,column[COL_TEXNAME])		#�e�N�X�`���Z�b�g
    bin_out += [tex_id].pack("S") 

    #�n�`ITA�A�j���t�@�C���w��
    anm_ita_id = enum_search(ita_enum, column[COL_ITA_NAME].sub(/ita$/,"nsbta"))
    #�n�`ITP�A�j���t�@�C���w��
    anm_itp_id = enum_search(itp_enum, column[COL_ITP_NAME].sub(/xls$/,"itptbl"))
    bin_out +=[anm_ita_id,anm_itp_id].pack("CC")

    inout = GetInnerOuter(column[COL_INOUT])			#INNER/OUTER
    bin_out += [inout].pack("C") 

    light_idx = enum_search( light_enum, column[COL_LIGHTTYPE] ) #���C�g
    bin_out += [light_idx].pack("C") 

    total_txt_file.printf("AREA:%3d BM:%2d TEX:%2d ANM:%2d IO:%d LIGHT:%d\n",
                area_count, bm_id, tex_id, anm_ita_id, inout, light_idx);

    area_count += 1
  }

  File.open(TARGET_BIN_FILENAME, "wb"){|file|
    file.write(bin_out)
  }

  total_txt_file.close
  #�e�C���쐬
  id_file.close

end



