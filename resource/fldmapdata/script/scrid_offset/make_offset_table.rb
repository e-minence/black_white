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

# enum_const_set���g����悤�ɂ���
require ENV["PROJECT_ROOT"] + 'tools/constant'

SCRIPT_GMM_DIR  = "script_message"
DEBUG_GMM_DIR = "debug"
DEFAULT_GMM_PATH = ENV["PROJECT_RSCDIR"] + "message/src/"
#SCRIPT_GMM_PATH = ENV["PROJECT_RSCDIR"] + "message/src/script_message/"
SCRIPT_PATH = ENV["PROJECT_RSCDIR"] + "fldmapdata/script/"

class ScrOfsConverterError < Exception; end

XLS2TAB = "ruby #{ENV["PROJECT_ROOT"]}tools/exceltool/xls2xml/tab_out.rb -c "

#----------------------------------------------------------------------------
# �X�N���v�g�I�t�Z�b�g�w��̃f�[�^�Z�b�g
#----------------------------------------------------------------------------
class ScrOfs
  attr_reader :name, :startno, :max, :scriptfile, :msgfile, :msgarc, :eventuse, :debuguse, :comment

  def initialize(name, startno, max, scriptfile, msgfile, msgarc, eventuse, debuguse, comment)
    @name = name
    @startno = startno
    @max = max
    @scriptfile = scriptfile
    @msgfile = msgfile
    @msgarc = msgarc
    @eventuse = eventuse
    @debuguse = debuguse
    @comment = comment

    STDERR.puts "MAKE SCRIPT OFFSET \"#{name}\", #{startno}-#{endno}"
  end

  def endno
    return @startno+@max-1
  end
end

#----------------------------------------------------------------------------
# ScrOfs���܂Ƃ߂�
#----------------------------------------------------------------------------
class ScrOfsData
  enum_const_set %w[
    COL_AREANAME
    COL_STARTNO
    COL_MAX
    COL_SCRFILE
    COL_MSGFILE
    COL_MSGARC
    COL_EVENTUSE
    COL_DEBUGMODE
    COL_COMMENT
  ]

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
    raiseError(errorMsg,lincount) if column[COL_MSGARC] != "�Ώۃ��b�Z�[�W�A�[�J�C�u"
    raiseError(errorMsg,lincount) if column[COL_EVENTUSE] != "�C�x���g�Q��"
    raiseError(errorMsg,lincount) if column[COL_DEBUGMODE] != "�f�o�b�O�p"
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
      msgarc = column[COL_MSGARC]
      eventuse = column[COL_EVENTUSE]
      debuguse = column[COL_DEBUGMODE]

      raiseError("�̈於���d�Ȃ��Ă��܂�.#{areaname}",linecount) if names.has_key?(areaname)
      names[areaname] = nil

      raiseError("�g�p�̈悪�O�̕����ɏd�Ȃ��Ă��܂�\n",linecount) if startno <= before_max
      before_max = startno + max - 1

      if msgarc == "�~"
        msgarc = ""
      end
      if msgfile != "�~"
        unless FileTest.exist?(DEFAULT_GMM_PATH + msgarc + "/" + msgfile) then
          raiseError("�w�肳�ꂽgmm�t�@�C��#{msgfile}�����݂��܂���", linecount)
        end
      else
        msgfile = nil
      end
      if scrfile != "�~"
        unless FileTest.exist?(SCRIPT_PATH + scrfile) then
          raiseError("�w�肳�ꂽscript�t�@�C��#{scrfile}�����݂��܂���", linecount)
        end
=begin
        if File.basename( scrfile, ".*" ).upcase != areaname then
          raiseError(
            "�Ǘ��̈於#{areaname}��script�t�@�C��#{scrfile}�̖��O�ɕs����������܂�", linecount )
        end
=end
      else
        scrfile = nil
      end

      case eventuse
      when "��" then
        eventuse = true
      when "�~" then
        eventuse = false
      else
        raiseError("�C�x���g�Q�Ƃɖ����Ȓl(#{eventuse})���ݒ肳��Ă��܂�",linecount)
      end
      case debuguse
      when "��" then
        debuguse = true
      when "�~" then
        debuguse = false
      else
        raiseError("�f�o�b�O�p�w��ɖ����Ȓl(#{debuguse})���ݒ肳��Ă��܂�",linecount)
      end

      comment = column[COL_COMMENT]
      @ofsdatas << ScrOfs.new(areaname, startno, max, scrfile, msgfile, msgarc, eventuse, debuguse, comment)
      @define_count += 1
    }
  end

  def make_ofs_header(file)
    file.puts "//���̃t�@�C����#{@manager_filename}���玩���������ꂽ���̂ł�"
  end
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
def set_date_string(string, keyword)
  date_string = Time.new.localtime.strftime("%Y %m/%d (%a) %H:%M")
  return string.sub(keyword, date_string)
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

output = set_date_string( head_string, /DATE_STRING/ )

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

output = set_date_string( head_string, /DATE_STRING/ )

scrData.ofsdatas.reverse_each{|data|
  start_id = "ID_#{data.name.upcase}_OFFSET,"
  end_id = "ID_#{data.name.upcase}_OFFSET_END,"
  data.scriptfile
  data.msgfile
  if data.scriptfile != nil && data.msgfile != nil
    scridx = "NARC_script_seq_#{data.scriptfile.sub(/\.ev$/,"_bin")}"
    if data.msgarc == "debug" then
      msgarc = "ARCID_DEBUG_MESSAGE"
      msgidx = "NARC_debug_message_#{data.msgfile.sub(/\.gmm/,"_dat")}"
    else
      msgarc = "ARCID_#{data.msgarc.upcase}"
      msgidx = "NARC_#{data.msgarc}_#{data.msgfile.sub(/\.gmm/,"_dat")}"
    end
    if data.debuguse == true then
      output += sprintf("#ifdef PM_DEBUG\n")
    end
    output += sprintf("    {//#{data.comment}\n")
    output += sprintf("      %-32s//(%5d),\n", start_id, data.startno)
    output += sprintf("      %-32s//(%5d),\n", end_id, data.endno)
    output += sprintf("      %s,\n", scridx)
    output += sprintf("      %s,\n", msgarc)
    output += sprintf("      %s\n", msgidx)
    output += sprintf("    },\n")
    if data.debuguse == true then
      output += sprintf("#endif //PM_DEBUG\n")
    end
  end
}
output += tail_string
return output
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
def make_uniq_script_list( scrData )
  head_string =<<END_OF_STRING
#======================================================================
#
# @file  uniq_script.list
# @bfief ��}�b�v�ˑ��̃X�N���v�g���X�g
# @author  tamada GAMEFREAK inc.
# @date  DATE_STRING
#
#  ���̃t�@�C���̓R���o�[�^�ɂ�莩����������Ă��܂�
#
#======================================================================

END_OF_STRING

  tail_string =<<END_OF_STRING

# ���������t�@�C��uniq_script.list�@����
END_OF_STRING

all_ev_list = "UNIQ_SCRIPTFILES = "
top_ev_list = "UNIQ_TOP_EVENT_SCRFILES = "
sub_ev_list = "UNIQ_SUB_EVENT_SCRFILES = "
debug_ev_list = "UNIQ_DEBUG_EVENT_SCRFILES = "

output = set_date_string( head_string, /DATE_STRING/ )

filenames = Hash.new

scrData.ofsdatas.each{|data|
  filename = data.scriptfile
  if filename != nil && filenames.has_key?(filename) == false then
    if data.debuguse == true then
      debug_ev_list += sprintf("\\\n\t%s ", filename)
    else
      all_ev_list += sprintf("\\\n\t%s ", filename)
      filenames[filename] = true

      if data.eventuse == true then
        top_ev_list += sprintf("\\\n\t%s ", filename)
      else
        sub_ev_list += sprintf("\\\n\t%s ", filename)
      end
    end
  end
}
output += "#��}�b�v�ˑ��̃X�N���v�g�t�@�C�����ׂ�\n"
output += all_ev_list + "\n\n"
output += "#�C�x���g�f�[�^�Ŏw�肳�꒼�ڌĂ΂��X�N���v�g�t�@�C��\n"
output += top_ev_list + "\n\n"
output += "#�v���O������X�N���v�g����Ă΂��X�N���v�g�t�@�C��\n"
output += sub_ev_list + "\n\n"
output += "#�f�o�b�O�p�̃X�N���v�g�t�@�C��\n"
output += debug_ev_list + "\n\n"
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

File.open("../uniq_script.list", "w"){|file|
  list = make_uniq_script_list( scrData )
  file.puts list
}

