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

$KCODE="SJIS"

# $zone_spec�̒�`
require "../../batt_bg_tbl/zone_spec.rb"

class ZoneIDError < Exception; end
#------------------------------------------------
#------------------------------------------------
class ColumnID

	attr :cZONE_ID, true
	attr :cMAPRSC_ID, true
	attr :cAREA_ID, true
	attr :cMOVEMODEL, true
	attr :cMATRIXID, true
	attr :cSX, true
	attr :cSY, true
	attr :cSZ, true
	#attr :cSCRIPT, true
	#attr :cMSG, true
	attr :cENCOUNT, true
	attr :cMAPCHGTYPE, true
	attr :cBICYCLE, true
	attr :cDASH, true
	attr :cESCAPE, true
	attr :cFLY, true
  attr :cBGM_CHG, true
  attr :cPALACE_OK, true
	attr :cBGM_SPRING, true
	attr :cBGM_SUMMER, true
	attr :cBGM_AUTUMN, true
	attr :cBGM_WINTER, true
	attr :cWEATHER, true
	attr :cCAMERA, true
	attr :cCAMERA_AREA, true
	attr :cBATTLEBG, true
	attr :cNAME, true
	attr :cPLACENAMEFLAG, true
	attr :cCOMMENT, true

	def initialize( column )
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
			when "MATRIXID"
				@cMATRIXID = c_count
			when "SX"
				@cSX = c_count
			when "SY"
				@cSY = c_count
			when "SZ"
				@cSZ = c_count
			when "encount"
				@cENCOUNT = c_count
			when "map_chg_type"
				@cMAPCHGTYPE = c_count
			when "bicycle"
				@cBICYCLE = c_count
			when "dash"
				@cDASH = c_count
			when "escape"
				@cESCAPE = c_count
			when "fly"
				@cFLY = c_count
      when "bgm_chg"
        @cBGM_CHG = c_count
      when "palace_ok"
        @cPALACE_OK = c_count
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
			when "CAMERA_AREA"
				@cCAMERA_AREA = c_count
			when "BATTLE_BG"
				@cBATTLEBG = c_count
			when "PLACE_NAME"
				@cNAME = c_count
			when "PLACE_NAME_FLAG"
				@cPLACENAMEFLAG = c_count
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
	def initialize ( fname, check_flag )
		@check_before_write = check_flag
		@filename = fname
	end

  def open( column_data )
    @cl = column_data
		if @check_before_write == true then
			@fp = File.open("temp_"+@filename, "w")
		else
			@fp = File.open(@filename, "w")
		end
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

	def ox2bool( str, id )
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
    matrixId = "MATRIX_ID_#{column[@cl.cMATRIXID]}".upcase

    event_id = "NARC_eventdata_#{id.downcase}_bin"
		script =  "NARC_script_seq_#{id.downcase}_bin"
		sp_script =  "NARC_script_seq_sp_#{id.downcase}_bin"
		msg = "NARC_script_message_#{id.downcase}_dat"

		bgm_spring = column[@cl.cBGM_SPRING]
		bgm_summer = column[@cl.cBGM_SUMMER]
		bgm_autumn = column[@cl.cBGM_AUTUMN]
		bgm_winter = column[@cl.cBGM_WINTER]
		weather = "WEATHER_NO_#{column[@cl.cWEATHER].upcase}"
		camera = "#{column[@cl.cCAMERA]}"
		camera_area = "#{column[@cl.cCAMERA_AREA]}"
    if camera_area == "��"
      camera_area = matrixId  #�}�b�v�}�g���N�XID�Ɠ���ID
    elsif camera_area == "�~"
      camera_area = "camera_area_dummy"
    else
      camera_area = "NARC_camera_scroll_original_#{column[@cl.cCAMERA_AREA]}_bin"
    end
		battle_bg_str = "#{column[@cl.cBATTLEBG]}"
    if $zone_spec.has_key?( battle_bg_str ) then
      battle_bg = $zone_spec[battle_bg_str]
    else
			STDERR.puts "�퓬�w�i�̎w�肪���������I:#{battle_bg_str}:#{id.upcase}\n"
			exit 1
    end
		name = column[@cl.cNAME].upcase
		encount_id = case column[@cl.cENCOUNT]
			when "��"
				"NARC_encount_data_w_#{id.downcase}_bin"
			else
				"enc_dummy"
			end
    placename_flag = if column[@cl.cPLACENAMEFLAG] == "��" then 1 else 0 end

		map_chg_type = column[@cl.cMAPCHGTYPE]
		if !(map_chg_type =~ /^MC_TYPE_/) then
			STDERR.puts "�}�b�v�؂�ւ��^�C�v�̎w�肪��������!:#{map_chg_type}:\n"
		end
		dash_flag = ox2bool( column[@cl.cDASH], id )
		bicycle_flag = ox2bool( column[@cl.cBICYCLE], id )
		escape_flag = ox2bool( column[@cl.cESCAPE], id )
		fly_flag = ox2bool( column[@cl.cFLY], id )
    bgm_chg_flag = ox2bool( column[@cl.cBGM_CHG], id )
    palace_ok_flag = ox2bool( column[@cl.cPALACE_OK], id )


	@fp.puts <<DOCUMENT
	{//ZONE_ID_#{id.upcase} = #{linecount}
		#{maprsc.upcase},
		#{movemodel.upcase},
		#{area.upcase},
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
		#{placename_flag},
		#{weather},
		#{camera},
		#{map_chg_type},
		#{battle_bg},
		#{bicycle_flag},
		#{dash_flag},
		#{escape_flag},
		#{fly_flag},
    #{bgm_chg_flag},
    #{palace_ok_flag},
		#{camera_area}, // camera_area
		0, // padding
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
###############################################################
class ZoneNameBinaryFile < OutputFile

	def putHeader
	end

	def putFooter
	end

	def close
		@fp.close
	end

	def put	zone_id
    if zone_id =~ /[^a-zA-Z0-9]/ then
      raise ZoneIDError, "[#{zone_id}]�ɖ��Ή��̕������܂܂�Ă��܂�"
    end
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
def convert( datafilename, outputfiles )

  File.open(datafilename){|datafile|

    #��s�ڂ����̃C���f�b�N�X�����
    firstline = datafile.gets
    column_data = ColumnID.new( firstline.split )

    outputfiles.each{|file| file.open(column_data) }

    id_store = Hash.new

    datafile.each_with_index{|line, index|
      column = line.split
      id = column[column_data.cZONE_ID].upcase
      if id_store.has_key? id then
        raise Exception, "#{id}:�d������ID�ł��I\n"
      end
      id_store[id] = column
      if id == "END"
        #�I�[��`
        #STDERR.puts "�I�["
        break
      end
      outputfiles.each{|file| file.putLine index, column}
    }

    outputfiles.each{|file| file.close}

    #STDERR.puts "zonetable.xls���R���o�[�g���܂���\n"
  
  }
end

begin
  OPTION = ARGV.shift
  inputfilename = ARGV.shift

  outputfiles = if OPTION == "header" then
                  [
                    ZoneIDFile.new("zone_id.h", true),
                  ]
                elsif OPTION == "contents" then
                  [
                    ZoneDataFile.new("result/zonetable.c", false),
                    ZoneNameBinaryFile.new("result/zonename.bin", false),
                  ]
                end

  convert( inputfilename, outputfiles )
end

