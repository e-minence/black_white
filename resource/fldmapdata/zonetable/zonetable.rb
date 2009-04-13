###############################################################
#
#
#		�]�[���Ǘ��\�R���o�[�^
#
#		sousuke tamada	GAME FREAK Inc.
#
#		2005.10.12	������
#		2008.11.10	tamada	WB���ɈڐA�J�n
#
#
###############################################################

#------------------------------------------------
#------------------------------------------------
class ColumnID

	attr :cZONE_ID, true
	attr :cMAPRSC_ID, true
	attr :cAREA_ID, true
	attr :cMOVEMODEL, true
	attr :cMATRIXARC, true
	attr :cMATRIXID, true
	attr :cSX, true
	attr :cSY, true
	attr :cSZ, true
	attr :cEVENT, true
	attr :cSCRIPT, true
	attr :cMSG, true
	attr :cENCOUNT, true
	attr :cMAPTYPE, true
	attr :cBICYCLE, true
	attr :cDASH, true
	attr :cESCAPE, true
	attr :cFLY, true
	attr :cBGM_SPRING, true
	attr :cBGM_SUMMER, true
	attr :cBGM_AUTUMN, true
	attr :cBGM_WINTER, true
	attr :cWEATHER, true
	attr :cCAMERA, true
	attr :cBATTLEBG, true
	attr :cNAME, true
	attr :cWINDOW, true
	attr :cCOMMENT, true

	def setup column
		c_count = 0;
		column.each { |item|
			case item
			when "ID"
				@cZONE_ID = c_count
			when "MAPRESOURCE"
				@cMAPRSC_ID = c_count
			when "AREA_ID"
				@cAREA_ID = c_count
			when "MOVE_MODEL"
				@cMOVEMODEL = c_count
			when "MATRIXARC"
				@cMATRIXARC = c_count
			when "MATRIXID"
				@cMATRIXID = c_count
			when "SX"
				@cSX = c_count
			when "SY"
				@cSY = c_count
			when "SZ"
				@cSZ = c_count
			when "event"
				@cEVENT = c_count
			when "script"
				@cSCRIPT = c_count
			when "msg"
				@cMSG = c_count
			when "encount"
				@cENCOUNT = c_count
			when "maptype"
				@cMAPTYPE = c_count
			when "bicycle"
				@cBICYCLE = c_count
			when "dash"
				@cDASH = c_count
			when "escape"
				@cESCAPE = c_count
			when "fly"
				@cFLY = c_count
			when "BGM_SPRING"
				@cBGM_SPRING = c_count
			when "BGM_SUMMER"
				@cBGM_SUMMER = c_count
			when "BGM_AUTUMN"
				@cBGM_AUTUMN = c_count
			when "BGM_WINTER"
				@cBGM_WINTER = c_count
			when "WEATHER"
				@cWEATHER = c_count
			when "CAMERA"
				@cCAMERA = c_count
			when "BATTLE_BG"
				@cBATTLEBG = c_count
			when "PLACE_NAME"
				@cNAME = c_count
			when "WINDOW_TYPE"
				@cWINDOW = c_count
			when "���l"
				@cCOMMENT = c_count
			end
			c_count += 1
		}
	end

end




###############################################################
###############################################################
def filediff file1,file2
	f1 = File.open(file1,"r")
	l1 = f1.read
	f1.close
	f2 = File.open(file2,"r")
	l2 = f2.read
	f2.close
	return (l1 != l2)
end

def diff_overwrite new_file, old_file
	if FileTest.exist? old_file then
		if filediff(old_file,new_file) == true then
			#�������������ꍇ�͍X�V
			File.delete old_file
			File.rename new_file, old_file
		else
			#�������Ȃ������ꍇ��new_file���폜
			File.delete new_file
		end
	else
		#���݂��Ȃ��ꍇ��new_file��old_file�Ƀ��l�[��
		File.rename new_file, old_file
	end
end

###############################################################
#
#
#
###############################################################

class OutputFile
	def initialize cl, fname, check_flag
		@cl = cl
		@check_before_write = check_flag
		if check_flag == true then
			@fp = File.open("temp_"+fname, "w")
		else
			@fp = File.open(fname, "w")
		end
		@filename = fname
		putHeader
	end

	def getID column
		column[@cl.cZONE_ID].upcase
	end

	def putLine linecount, column
	end

	def putHeader
		@fp.puts "//#{Time.now.ctime}"
		@fp.puts "//#{@filename}"
	end

	def putFooter
		@fp.puts "//#{@filename}"
	end

	def close
		putFooter
		@fp.close
		if @check_before_write == true then
			temp_file = File::basename(@fp.path)
			diff_overwrite temp_file, @filename
		end
	end

end
###############################################################
#
#		ZoneData�z�񐶐�
#
###############################################################

class ZoneDataFile < OutputFile

	#------------------------------------------------
	#------------------------------------------------
	def putHeader
		@fp.puts "static const ZONEDATA ZoneData[] = {\n"
	end

	def putFooter
		@fp.puts "};\n"
	end

	def ox2bool str, id
		case str
		when "��"
			"TRUE"
		when "�~"
			"FALSE"
		else
			STDERR.puts "#{id}:#{str}:�z��O�̓��͂�����܂�\n"
			"FALSE"
		end
	end

	#------------------------------------------------
	#
	#	�]�[�����Ƃ̃f�[�^�o��
	#
	#------------------------------------------------
	def putLine linecount, column
		id = column[@cl.cZONE_ID]
		if id == "END"
			return "END"
		end

		maprsc = column[@cl.cMAPRSC_ID]
		area = column[@cl.cAREA_ID]
		if !(area =~ /^AREA_ID_/) then
			STDERR.puts "�G���A�̎w�肪��������!:#{area}:\n"
			exit 1
		end
		movemodel = column[@cl.cMOVEMODEL]
		matrixArc = "MATRIX_ARC_#{column[@cl.cMATRIXARC]}".upcase
		if matrixArc == "MATRIX_ARC_NGMATRIX" then
			matrixId = "MATRIX_ID_#{column[@cl.cMATRIXID]}".upcase
		else
			matrixId = "NARC_map_matrix_#{column[@cl.cMATRIXID].downcase}_mat_bin"
		end

		event_id = column[@cl.cEVENT] == "��" ? "NARC_zone_event_zone_#{id.downcase}_total_bin" : "event_dummy"
		script = column[@cl.cSCRIPT] == "��" ? "NARC_scr_seq_#{id.downcase}_bin" : "scr_dummy"
		sp_script = column[@cl.cSCRIPT] == "��" ? "NARC_scr_seq_sp_#{id.downcase}_bin" : "sp_scr_dummy"
		msg = column[@cl.cMSG] == "��" ? "NARC_msg_#{id.downcase}_dat" : "msg_dummy"
		bgm_spring = column[@cl.cBGM_SPRING]
		bgm_summer = column[@cl.cBGM_SUMMER]
		bgm_autumn = column[@cl.cBGM_AUTUMN]
		bgm_winter = column[@cl.cBGM_WINTER]
		weather = "WEATHER_SYS_#{column[@cl.cWEATHER].upcase}"
		camera = "#{column[@cl.cCAMERA]}"
		battle_bg = "#{column[@cl.cBATTLEBG]}"
		if !(battle_bg =~ /^BG_ID_/) then
			STDERR.puts "�퓬�w�i�̎w�肪���������I:#{battle_bg}:#{id.upcase}\n"
			exit 1
		end
		name = column[@cl.cNAME].upcase
		encount_id = case column[@cl.cENCOUNT]
			when "��"
				"NARC_enc_data_#{id.downcase}enc_bin"
			else
				"enc_dummy"
			end
		window = column[@cl.cWINDOW].upcase
		maptype = column[@cl.cMAPTYPE]
		if !(maptype =~ /^MAPTYPE_/) then
			STDERR.puts "�}�b�v�^�C�v�̎w�肪��������!:#{maptype}:\n"
		end
		dash_flag = ox2bool column[@cl.cDASH], id
		bicycle_flag = ox2bool column[@cl.cBICYCLE], id
		escape_flag = ox2bool column[@cl.cESCAPE], id
		fly_flag = ox2bool column[@cl.cFLY], id


	@fp.puts <<DOCUMENT
	{//ZONE_ID_#{id.upcase} = #{linecount}
		#{maprsc.upcase},
		#{area.upcase},
		#{movemodel.upcase},
		#{matrixArc},
		#{matrixId},
		#{script},
		#{sp_script},	//#{sp_script},
		#{msg},
		#{bgm_spring},
		#{bgm_summer},
		#{bgm_autumn},
		#{bgm_winter},
		#{encount_id},
		#{event_id},
		#{name},
		#{window},
		#{weather},
		#{camera},
		#{maptype},
		#{battle_bg},
		#{bicycle_flag},
		#{dash_flag},
		#{escape_flag},
		#{fly_flag},
		#{column[@cl.cSX]},
		#{column[@cl.cSY]},
		#{column[@cl.cSZ]},
	},
DOCUMENT
		return id.upcase
	end

end

###############################################################
#
#	�]�[�����e�[�u���f�[�^����
#
###############################################################
class ZoneNameFile < OutputFile
	def putHeader
		@fp.puts "static const char * const ZoneName[] = {\n"
	end
	def putFooter
		@fp.puts "};\n"
	end
	def putLine linecount, column
		zone_id = getID(column)
		@fp.puts "\t\"#{zone_id}\",\n"
	end
end

###############################################################
#
#	�]�[��ID��`�w�b�_����
#
###############################################################

class ZoneIDFile < OutputFile

	def putHeader
		@fp.puts "//zone id define table\n"
		#@fp.puts "//#{Time.now.ctime}"
		@fp.puts "\#ifndef __ZONE_ID_H__\n"
		@fp.puts "\#define __ZONE_ID_H__\n"
		#@fp.puts "enum {\n"
	end

	def putFooter
		@fp.puts "#define ZONE_ID_MAX	(#{@linecount_last + 1})\n"
		@fp.puts "\#endif // __ZONE_ID_H__\n"
		@fp.puts "\n"
	end

	#----------------------------------------
	#	�]�[�����Ƃ�ID
	#----------------------------------------
	def putLine linecount, column
		@linecount_last = linecount
		zone_id = getID(column)
		idstr = "ZONE_ID_#{zone_id}"
		@fp.printf("#define %-24s (%3d)\n", idstr, linecount)
	end

end

###############################################################
#
#	�]�[�����C�x���g��`����
#
###############################################################
class ZoneEventFile < OutputFile
	def putHeader
		@fp.puts "# zone event list start\n"
		@fp.puts "CONVSRCS	=	\\\n"
		@fp.puts "	zone_dummy_total.c \\\n"
	end
	def put	id
		@fp.puts "	zone_#{id.downcase}_total.c \\\n"
	end
	def putLine linecount, column
		#�C�x���g�t�@�C��������Ƃ�������������
		if column[cl.cEVENT] == "��" then
			put getID(column)
		end
	end
	def putFooter
		@fp.puts "\n"
		@fp.puts "# zone event list end\n"
	end
end

class ZoneEventArcFile < OutputFile
	def putHeader
		@fp.puts "\"zone_dummy_total.bin\"\n"
	end
	def put id
		@fp.puts "\"zone_#{id.downcase}_total.bin\"\n"
	end
	def putLine linecount, column
		#�C�x���g�t�@�C��������Ƃ�������������
		if column[cl.cEVENT] == "��" then
			put getID(column)
		end
	end
	def putFooter
	end
end

###############################################################
#	�}�b�v�ڑ���`�w�b�_�̃C���N���[�h����
###############################################################
class ZoneEventDoorHeader < OutputFile
	def putHeader
		@fp.puts "//#{@name}"
	end
	def put id
		@fp.puts "#include \"zone_#{id.downcase}evd.h\"\n"
	end
	def putLine linecount, column
		#�C�x���g�t�@�C��������Ƃ�������������
		if column[cl.cEVENT] == "��" then
			put getID(column)
		end
	end
end

###############################################################
#
###############################################################
class ZoneMsgHeader < OutputFile
	def putHeader
		@fp.puts "/* convert header file */"
	end
	def put id
		@fp.puts "#include \"../../../include/msgdata/msg_#{id.downcase}.h\"\n"
	end
	def putLine linecount, column
		#�C�x���g�t�@�C��������Ƃ�������������
		if column[cl.cMSG] == "��" then
			put getID(column)
		end
	end
end

###############################################################
###############################################################
class ZoneNameBinaryFile < OutputFile
=begin
	def initialize fname
		@fp = File.open(fname, "w")
		@name = fname
	end
=end

	def putHeader
	end

	def putFooter
	end

	def close
		@fp.close
	end

	def put	zone_id
		if zone_id.length <= 16 then
			@fp.syswrite zone_id
			blank = ""
			blank << 0
			blank *= (16 - zone_id.length)
			@fp.syswrite blank
		end
	end
	def putLine linecount, column
		put getID(column)
	end
end


###############################################################
#
#		�������烁�C��
#
###############################################################
def convert

	firstline = gets
	cl = ColumnID.new
	cl.setup firstline.split

	files = [
		#�J�������e�w��ID��`�A�t�@�C�����A�㏑���O�Ɋm�F���邩�ǂ���
		ZoneIDFile.new(cl, "zone_id.h", true),
		ZoneDataFile.new(cl, "zonetable.c", false),
	#	ZoneNameFile.new(cl, "mapname.dat", false),
		ZoneNameBinaryFile.new(cl, "zonename.bin", false),
	#	ZoneEventFile.new(cl, "eventlist.txt", false),
	#	ZoneEventArcFile.new(cl, "eventarc.txt", false),
	#	ZoneEventDoorHeader.new(cl, "doorevent.h", true),
	#	ZoneMsgHeader.new(cl, "msg_header.h", true),
	]

	linecount = 0
	id_store = Hash.new
	while line = gets
		column = line.split
		id = column[cl.cZONE_ID].upcase
		if id_store.has_key? id then
			STDERR.puts "#{id}:�d������ID�ł��I\n"
			exit 1
		else
			id_store[id] = column
		end
		if id == "END"
			#�I�[��`
			#STDERR.puts "�I�["
			break
		end
		files.each{|file| file.putLine linecount, column}
		linecount += 1
	end

	files.each{|file| file.close}

	#STDERR.puts "zonetable.xls���R���o�[�g���܂���\n"
end

convert

