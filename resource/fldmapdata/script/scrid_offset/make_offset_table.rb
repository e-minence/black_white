#----------------------------------------------------------------------------
#
#
#   �X�N���v�gID�I�t�Z�b�g��������
#
#
#     2009.08.07  tamada GAMEFREAK inc.
#
#----------------------------------------------------------------------------

$KCODE= "SJIS"


SCRIPT_GMM_PATH = ENV["PROJECT_RSCDIR"] + "message/src/script_message/"
SCRIPT_PATH = ENV["PROJECT_RSCDIR"] + "fldmapdata/script/"

class ScrOfsConverterError < Exception; end

XLS2TAB = "ruby #{ENV["PROJECT_ROOT"]}tools/exceltool/xls2xml/tab_out.rb -c "

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
class ScrOfs
  attr_reader :name, :startno, :max, :scriptfile, :msgfile, :comment

  def initialize(name, startno, max, scriptfile, msgfile, comment)
    @name = name
    @startno = startno
    @max = max
    @scriptfile = scriptfile
    @msgfile = msgfile
    @comment = comment

    STDERR.puts "MAKE SCRIPT OFFSET \"#{name}\", #{startno}-#{endno}"
  end

  def endno
    return @startno+@max-1
  end
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
class ScrOfsData

  COL_AREANAME  = 0
  COL_STARTNO   = 1
  COL_MAX       = 2
  COL_SCRFILE   = 3
  COL_MSGFILE   = 4
  COL_COMMENT   = 5

  attr_reader :manager_filename, :ofsdatas, :define_count

  def initialize(filename)
    @manager_filename = filename

    command = "#{XLS2TAB} #{@manager_filename}"
    @tmpfile = `#{command}`.split("\n")

    @define_count = 0
    @ofsdatas = Array.new

    read_list(@tmpfile)
  end

  def raiseError(string, count)
    raise ScrOfsConverterError, "#{@manager_filename}:#{count}:\n #{string}"
  end

  def read_list(lines)
    linecount = 1
    column = lines.shift.split(",")
    #�t�@�C���t�H�[�}�b�g�̃`�F�b�N�i�J�����̐������z�肵�Ă�����̂ƈ�v���Ă��邩�H�j
    errorMsg = "�t�@�C���t�H�[�}�b�g�ُ�F"
    raiseError(errorMsg,lincount) if column[COL_AREANAME] != "�Ǘ��̈於"
    raiseError(errorMsg,lincount) if column[COL_STARTNO] != "�J�n�i���o�["
    raiseError(errorMsg,lincount) if column[COL_MAX] != "�ő�l"
    raiseError(errorMsg,lincount) if column[COL_SCRFILE] != "�ΏۃX�N���v�g�t�@�C��"
    raiseError(errorMsg,lincount) if column[COL_MSGFILE] != "�Ώۃ��b�Z�[�W�t�@�C��"
    raiseError(errorMsg,lincount) if column[COL_COMMENT] != "�R�����g"

    names = Hash.new

    before_max = -1
    lines.each{|line|
      linecount += 1
      column = line.split(",")
      if column[COL_AREANAME] == "#END" || column.size < 5 then
          break
      end
      areaname = column[COL_AREANAME]
      startno = column[COL_STARTNO]
      if startno == "auto" then
        startno = before_max + 1
      else
        startno = Integer(startno)
      end
      max = Integer(column[COL_MAX])
      scrfile = column[COL_SCRFILE]
      msgfile = column[COL_MSGFILE]

      raiseError("�̈於���d�Ȃ��Ă��܂�.#{areaname}",linecount) if names.has_key?(areaname)
      names[areaname] = nil

      raiseError("�g�p�̈悪�O�̕����ɏd�Ȃ��Ă��܂�\n",linecount) if startno <= before_max
      before_max = startno + max - 1

      if msgfile != "�~"
        unless FileTest.exist?(SCRIPT_GMM_PATH + msgfile) then
          raiseError("�w�肳�ꂽgmm�t�@�C��#{msgfile}�����݂��܂���", linecount)
        end
      else
        msgfile = nil
      end
      if scrfile != "�~"
        unless FileTest.exist?(SCRIPT_PATH + scrfile) then
          raiseError("�w�肳�ꂽscript�t�@�C��#{scrfile}�����݂��܂���", linecount)
        end
      else
        scrfile = nil
      end

      comment = column[COL_COMMENT]
      @ofsdatas << ScrOfs.new(areaname, startno, max, scrfile, msgfile, comment)
      @define_count += 1
    }
  end

  def make_ofs_header(file)
    file.puts "//���̃t�@�C����#{@manager_filename}���玩���������ꂽ���̂ł�"
  end
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
def make_offset_header(scrData)
  head_string = <<END_OF_STRING
//======================================================================
/**
 * @file  scr_offset_id.h
 * @bfief  �X�N���v�gID�̃I�t�Z�b�g��`
 * @author  tamada GAMEFREAK inc.
 * @date  DATE_STRING
 *
 * ���̃t�@�C���̓R���o�[�^�ɂ�莩����������Ă��܂�
 */
//======================================================================
#ifndef SCR_OFFSET_ID_H
#define SCR_OFFSET_ID_H

//======================================================================
//======================================================================
END_OF_STRING

  tail_string = <<END_OF_STRING

#endif  /* SCR_OFFSET_ID_H */
END_OF_STRING

date_string = Time.new.localtime.strftime("%Y %m/%d (%a) %H:%M")
output = String.new
output = head_string.sub(/DATE_STRING/,date_string)

scrData.ofsdatas.each{|data|
  start_id = "ID_#{data.name.upcase}_OFFSET"
  end_id = "ID_#{data.name.upcase}_OFFSET_END"
  startno = "(#{data.startno})"
  endno = "(#{data.endno})"

  output += sprintf("#define %-32s %5s //%s\n",start_id, startno, data.comment)
  output += sprintf("#define %-32s %5s\n",  end_id, endno)
  output += "\n"
}

output += tail_string
return output
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
def make_cdat_table(scrData)
  head_string =<<END_OF_STRING
//======================================================================
/**
 * @file  scr_offset.cdat
 * @bfief  �X�N���v�gID�̃I�t�Z�b�g�Ƃ���ɑΉ�����A�[�J�C�u�̒�`�e�[�u��
 * @author  tamada GAMEFREAK inc.
 * @date  DATE_STRING
 *
 * ���̃t�@�C���̓R���o�[�^�ɂ�莩����������Ă��܂�
 */
//======================================================================

//���X�N���v�gID���傫�����ɒ�`����Ă��邽�߁A���f�[�^�ƕ��т��t�ł�
static const SCRIPT_ARC_TABLE ScriptArcTable[] = {

END_OF_STRING
  tail_string =<<END_OF_STRING
};
END_OF_STRING

date_string = Time.new.localtime.strftime("%Y %m/%d (%a) %H:%M")
output = String.new
output = head_string.sub(/DATE_STRING/,date_string)

scrData.ofsdatas.reverse_each{|data|
  start_id = "ID_#{data.name.upcase}_OFFSET,"
  end_id = "ID_#{data.name.upcase}_OFFSET_END,"
  data.scriptfile
  data.msgfile
  if data.scriptfile != nil && data.msgfile != nil
    scridx = "NARC_script_seq_#{data.scriptfile.sub(/\.ev$/,"_bin")}"
    msgidx = "NARC_script_message_#{data.msgfile.sub(/\.gmm/,"_dat")}"
    output += sprintf("    {//#{data.comment}\n")
    output += sprintf("      %-32s//(%5d),\n", start_id, data.startno)
    output += sprintf("      %-32s//(%5d),\n", end_id, data.endno)
    output += sprintf("      %s,\n", scridx)
    output += sprintf("      %s\n", msgidx)
    output += sprintf("    },\n")
  end
}
output += tail_string
return output
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

scrData =ScrOfsData.new(ARGV[0])



File.open("scr_offset_id.h","w"){|file|
  header = make_offset_header(scrData)
  file.puts header
}

File.open("scr_offset.cdat","w"){|file|
  table = make_cdat_table(scrData)
  file.puts table
}


