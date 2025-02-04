#----------------------------------------------------------------------------
#
#
#   スクリプトIDオフセット自動生成
#
#
#     2009.08.07  tamada GAMEFREAK inc.
#
#----------------------------------------------------------------------------

$KCODE= "SJIS"

# enum_const_setを使えるようにする
require ENV["PROJECT_ROOT"] + 'tools/constant'

SCRIPT_GMM_DIR  = "script_message"
DEBUG_GMM_DIR = "debug"
DEFAULT_GMM_PATH = ENV["PROJECT_RSCDIR"] + "message/src/"
#SCRIPT_GMM_PATH = ENV["PROJECT_RSCDIR"] + "message/src/script_message/"
SCRIPT_PATH = ENV["PROJECT_RSCDIR"] + "fldmapdata/script/"

class ScrOfsConverterError < Exception; end

XLS2TAB = "ruby #{ENV["PROJECT_ROOT"]}tools/exceltool/xls2xml/tab_out.rb -c "

#----------------------------------------------------------------------------
# スクリプトオフセット指定のデータセット
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
# ScrOfsをまとめる
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
    #ファイルフォーマットのチェック（カラムの説明が想定しているものと一致しているか？）
    errorMsg = "ファイルフォーマット異常："
    raiseError(errorMsg,lincount) if column[COL_AREANAME] != "管理領域名"
    raiseError(errorMsg,lincount) if column[COL_STARTNO] != "開始ナンバー"
    raiseError(errorMsg,lincount) if column[COL_MAX] != "最大値"
    raiseError(errorMsg,lincount) if column[COL_SCRFILE] != "対象スクリプトファイル"
    raiseError(errorMsg,lincount) if column[COL_MSGFILE] != "対象メッセージファイル"
    raiseError(errorMsg,lincount) if column[COL_MSGARC] != "対象メッセージアーカイブ"
    raiseError(errorMsg,lincount) if column[COL_EVENTUSE] != "イベント参照"
    raiseError(errorMsg,lincount) if column[COL_DEBUGMODE] != "デバッグ用"
    raiseError(errorMsg,lincount) if column[COL_COMMENT] != "コメント"

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

      raiseError("領域名が重なっています.#{areaname}",linecount) if names.has_key?(areaname)
      names[areaname] = nil

      raiseError("使用領域が前の部分に重なっています\n",linecount) if startno <= before_max
      before_max = startno + max - 1

      if msgarc == "×"
        msgarc = ""
      end
      if msgfile != "×"
        unless FileTest.exist?(DEFAULT_GMM_PATH + msgarc + "/" + msgfile) then
          raiseError("指定されたgmmファイル#{msgfile}が存在しません", linecount)
        end
      else
        msgfile = nil
      end
      if scrfile != "×"
        unless FileTest.exist?(SCRIPT_PATH + scrfile) then
          raiseError("指定されたscriptファイル#{scrfile}が存在しません", linecount)
        end
=begin
        if File.basename( scrfile, ".*" ).upcase != areaname then
          raiseError(
            "管理領域名#{areaname}とscriptファイル#{scrfile}の名前に不整合があります", linecount )
        end
=end
      else
        scrfile = nil
      end

      case eventuse
      when "○" then
        eventuse = true
      when "×" then
        eventuse = false
      else
        raiseError("イベント参照に無効な値(#{eventuse})が設定されています",linecount)
      end
      case debuguse
      when "○" then
        debuguse = true
      when "×" then
        debuguse = false
      else
        raiseError("デバッグ用指定に無効な値(#{debuguse})が設定されています",linecount)
      end

      comment = column[COL_COMMENT]
      @ofsdatas << ScrOfs.new(areaname, startno, max, scrfile, msgfile, msgarc, eventuse, debuguse, comment)
      @define_count += 1
    }
  end

  def make_ofs_header(file)
    file.puts "//このファイルは#{@manager_filename}から自動生成されたものです"
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
 * @bfief  スクリプトIDのオフセット定義
 * @author  tamada GAMEFREAK inc.
 * @date  DATE_STRING
 *
 * このファイルはコンバータにより自動生成されています
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
 * @bfief  スクリプトIDのオフセットとそれに対応するアーカイブの定義テーブル
 * @author  tamada GAMEFREAK inc.
 * @date  DATE_STRING
 *
 * このファイルはコンバータにより自動生成されています
 */
//======================================================================

//※スクリプトIDが大きい順に定義されているため、元データと並びが逆です
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
# @bfief 非マップ依存のスクリプトリスト
# @author  tamada GAMEFREAK inc.
# @date  DATE_STRING
#
#  このファイルはコンバータにより自動生成されています
#
#======================================================================

END_OF_STRING

  tail_string =<<END_OF_STRING

# 自動生成ファイルuniq_script.list　末尾
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
output += "#非マップ依存のスクリプトファイルすべて\n"
output += all_ev_list + "\n\n"
output += "#イベントデータで指定され直接呼ばれるスクリプトファイル\n"
output += top_ev_list + "\n\n"
output += "#プログラムやスクリプトから呼ばれるスクリプトファイル\n"
output += sub_ev_list + "\n\n"
output += "#デバッグ用のスクリプトファイル\n"
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

