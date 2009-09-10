#----------------------------------------------------------------------------
#
#
#   �X�N���v�g�t���O/���[�N�Ǘ��V�X�e��
#
#
#   2009.07.14  tamada GAMEFREAK inc.
#
#----------------------------------------------------------------------------
$KCODE= "SJIS"

lists = [
  ["sys_flag.xls", "SYS_FLAG_",  "SYSFLAG_START", 320],
  ["time_flag.xls", "TM_FLAG_", "TMFLAG_START", 192]
]

class FlagReadError < Exception; end

XLS2TAB = "ruby #{ENV["PROJECT_ROOT"]}tools/exceltool/xls2xml/tab_out.rb -c "

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
class PMFlag
  attr_reader :name, :comment
   def initialize(name, comment, access)
     @name = name
     @comment = comment
     @access = access
#     puts "new flag #{name} = #{comment}"
   end
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
class PMFlagData

  COL_NUMBER = 0
  COL_NAME = 1
  COL_ACTIVE = 2
  COL_COMMENT = 3

  attr_reader  :filename, :prefix, :start_no, :max, :count

  def initialize(filename, prefix, start_no, max)
    @filename = filename
    @prefix = prefix
    @start_no = start_no
    @max = max

    STDERR.puts "MAKE FLAGS #{filename}, #{prefix}, #{start_no} - #{max}"
    @flags = Array.new
    @count = 0

    if filename != nil then
      command = "#{XLS2TAB} #{@filename}"
      @tmpfile = `#{command}`.split("\n")
      read(@tmpfile)
    else
      @max.times{|n|
        name = @prefix + sprintf("%03d", n)
        @flags << PMFlag.new(name, "�̈�m�ۂɂ�鎩������", 0)
      }
    end
  end

  def raiseError(string)
    raise FlagReadError, "#{@filename}:#{@count}:\n #{string}"
  end

  #���\�b�h�F�ǂݍ���
  def read(lines)
    column = lines.shift.split(",")
    raiseError "�t�H�[�}�b�g�ُ�ł�.#{column[COL_NUMBER]}" if column[COL_NUMBER] != "number"
    raiseError "�t�H�[�}�b�g�ُ�ł�.#{column[COL_NAME]}" if column[COL_NAME] != "name"
    raiseError "�t�H�[�}�b�g�ُ�ł�.#{column[COL_ACTIVE]}" if column[COL_ACTIVE] != "active"
    raiseError "�t�H�[�}�b�g�ُ�ł�.#{column[COL_COMMENT]}" if column[COL_COMMENT] != "comment"

    lines.each_with_index{|line, index|
      column = line.split(",")
      if line =~ /^#END/ then
        break
      end
      begin
        number = Integer(column[COL_NUMBER])
      rescue
        puts "#{line}"
        raiseError("number�ɐ��l�łȂ��l#{column[COL_NUMBER]}���w�肳��Ă��܂�")
      end
      name = column[COL_NAME]
      active = column[COL_ACTIVE]
      comment = column[COL_COMMENT]
      if number != @count then
        raiseError "No(#{number})���Ԉ���Ă��܂��B��������#{@count}�ł��B\n#{line}"
      end
      if @count >= @max then
        raiseError "�v�f�����������܂� MAX=#{@max}\n"
      end
      @count += 1
      if active == "�~" then
        @flags << PMFlag.new(nil, comment, 0)
      elsif active != "��" then
        raiseError "active�Ɂ��~�ȊO�̗v�f\"#{active}\"�������Ă��܂�.\n#{line}"
      elsif name =~/^#{@prefix}/ then
        @flags << PMFlag.new(name, comment, 0)
      else
        raiseError "���̂��Ԉ���Ă��܂��B#{@prefix}���܂܂�Ă��܂���B\n#{line}"
      end
    }
  end

  #���\�b�h�F�����o��
  def write_define()
    output = ""
    output += "/* flags \"#{@filename}\" #{@start_no} �` #{@start_no + @max - 1} */\n"
    output += "\n"
    output += sprintf("\#define %-32s %5d /* 0x%04x %s */\n", 
                      @prefix + "AREA_START", @start_no, @start_no, nil)
    output += "\n"
    @flags.each_with_index{ |flag, index|
      number = @start_no + index
      name = flag.name
      if name == nil then
        output += sprintf("\t\t/* not used %5d(0x%04x) */\n", number, number)
      else
        output += sprintf("\#define %-32s %5d /* 0x%04x %s */\n", flag.name, number, number, flag.comment)
      end
    }
    output += "\n"
    output += sprintf("\#define %-32s %5d /* 0x%04x %s */\n", 
                      @prefix + "AREA_END", @start_no + @max - 1, @start_no + @max - 1, nil)
    output += "\n\n"
    return output
  end

end
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
class FLAGMANAGER

  COL_FILENAME = 0
  COL_STARTNO = 1
  COL_MAX = 2
  COL_PREFIX = 3

  attr_reader :manager_filename, :tmpfile, :flagdatas

  def initialize(filename)
    @manager_filename = filename
    command = "#{XLS2TAB} #{@manager_filename}"
    @tmpfile = `#{command}`.split("\n")
    @flagdatas = Array.new
    read_list(@tmpfile)
    check()
  end

  #�G���[����
  def raiseError(string, count)
    raise FlagReadError, "#{@manager_filename}:#{count}:\n #{string}"
  end

  #�ǂݍ��ݏ���
  def read_list(lines)
    linecount = 1
    column = lines.shift.split(",")
    raiseError("�t�@�C���t�H�[�}�b�g�ُ�F",linecount) if column[COL_FILENAME] != "�Ǘ��t�@�C����"
    raiseError("�t�@�C���t�H�[�}�b�g�ُ�F",linecount) if column[COL_STARTNO] != "�J�n�i���o�["
    raiseError("�t�@�C���t�H�[�}�b�g�ُ�F",linecount) if column[COL_MAX] != "�ő吔"
    raiseError("�t�@�C���t�H�[�}�b�g�ُ�F",linecount) if column[COL_PREFIX] != "�v���t�B�b�N�X�w��"

    next_pos = 0
    lines.each{ |line|
      linecount += 1
      column = line.split(",")
      filename = column[COL_FILENAME]
      if column.size < 4 || filename == "\#END" then break end

      prefix = column[COL_PREFIX]
      if column[COL_STARTNO] == "auto" then
        start_no = next_pos
      else
        start_no = Integer(column[COL_STARTNO])
      end
      begin 
        max = Integer(column[COL_MAX])
      rescue
        p column
        puts "#{line}"
        raiseError("�ő�l�ݒ�ُ�", linecount)
      end
      next_pos = start_no + max

      if filename != "-" then
        @flagdatas << PMFlagData.new(filename, prefix, start_no, max)
      else
        @flagdatas << PMFlagData.new(nil, prefix, start_no, max)
      end
    }
  end

  #�p�����[�^�`�F�b�N
  def check()
    prefixes = Hash.new

    before_tail = -1
    @flagdatas.each_with_index{|flagdata, index|
      #�v���t�B�b�N�X�̏d���`�F�b�N�i�G���[�j
      prefix = flagdata.prefix
      if prefixes.has_key?(prefix) then
        raiseError("�v���t�B�b�N�X���d�Ȃ��Ă��܂��B#{prefix}", index)
      else
        prefixes[prefix] = nil
      end
      #�g�p�̈�̏d���`�F�b�N�i�G���[�j
      n_min = flagdata.start_no
      if before_tail < n_min then
        before_tail = n_min + flagdata.max - 1
      else
        raiseError("#{flagdata.filename}:�g�p����i���o�[���O�̗̈�ɏd�Ȃ��Ă��܂�. \
        before tail = #{before_tail}, \
                   now start = #{n_min}", index)
      end
      #�g�p�̈�̒�`�`�F�b�N�i�x���j
      if flagdata.count < flagdata.max && flagdata.filename != nil then
        STDERR.puts "�x���F#{flagdata.filename}"
        STDERR.puts "�x���F�v�f�ő�܂Œ�`����Ă��܂���Bmax=#{flagdata.max}, count=#{flagdata.count}"
      end
    }

  end

  def write_defines()
    sym = File.basename(@manager_filename,".*").upcase
    flag_min = -1
    flag_max = 0
    output_content = ""
    @flagdatas.each{|flagdata|
      output_content += flagdata.write_define()
      if flag_min < 0 then flag_min = flagdata.start_no end
      flag_max = flagdata.start_no + flagdata.max
    }
    output = ""
    output += "/* ���������t���O��`�t�@�C�� �J�n*/"
    output += "\#pragma once"
    output += "\n\n"
    output += sprintf("\#define %s_TOTAL_MIN  %5d /* 0x%04x */\n",sym, flag_min, flag_min)
    
    output += output_content

    output += "\n\n"
    output += sprintf("\#define %s_TOTAL_MAX  %5d /* 0x%04x */\n",sym, flag_max, flag_max)
    output += "\n\n"
    output += "/* ���������t���O��`�t�@�C�� �I��*/"
    output += "\n\n"

    return output
  end
end

#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
man = FLAGMANAGER.new(ARGV[0])

temp = man.write_defines()

File.open(ARGV[1], "w"){|file|
  file.puts temp
}



