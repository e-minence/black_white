#!/usr/bin/ruby -Ks
#---------------------------------------------------------
#
# *.naix��ǂ�Ŕz��Ɋi�[���邽�߂̃N���X
#
# 2009.05.12  tamada
#
#---------------------------------------------------------

class NAIXReader

  class NAIXReaderInputError < Exception; end

  def initialize( inputHandle )
    @index = nil
    @arc_name = nil
    readNaix( inputHandle )
  end

  #�t�@�C�������w�肵��NAIXReader�𐶐�����
  def NAIXReader.read( filename )
    File.open(filename){|file|
      return new(file)
    }
  end

  #��`�z����擾����
  def getArray
    @index
  end

  #�A�[�J�C�u�����擾����
  def getArchiveName
    @arc_name
  end


  private

  #*.naix�t�@�C�������߂��Ĕz��Ɋi�[
  def readNaix( inputHandle )
    inside_flag = false
    count = 0
    @index = Array.new

    inputHandle.each{|line|
      column = line.split
      case line
      when /#define /
        @arc_name = column[1].sub(/\ANARC_/,"").sub(/_NAIX_/,"").downcase
        
      when /^enum \{/
        inside_flag = true

      when /^\tNARC_/
        if inside_flag == false || column[1] != "=" then
          raise NAIXReaderInputError, "#{line}"
        end
        if count != Integer(column[2].sub(/,/,"")) then
          raise NAIXReaderInputError, "#{line}"
        end
        @index << column[0]
        count += 1

      when /\};$/
        inside_flag = false
      else
        #puts "ELSE LINE:#{line}"
      end
    }
    @index
  end

end

#---------------------------------------------------------
#---------------------------------------------------------
begin
  if __FILE__ == $0 then
    puts "�g�����F"
    puts "#require���߂ŃX�N���v�g�Ɏ�荞��Ŏg�p���Ă�������"
    puts "reader = NAIXRead.read(filename)"
    puts "array = reader.get  #�z��Ŏ擾"
    puts "array.each{....     #�K���Ɏg�p����"
  end
end

