#!ruby -Ks
#!/usr/bin/ruby
#
# ���M�����[�V�������o�C�i���ɂ���v���O����
# 2009.06.04 k.ohno
#
#

require "jcode"
require 'nkf'


COL_NO = 0    #No
COL_CUPNAME = 1    #�J�b�v��
COL_RULENAME = 2    #�ΐ탋�[��
COL_PROGRAM_LAVEL = 3    #�v���O�����Ŏg�����x��
COL_NUM_LO = 4    #�Q��������
COL_NUM_HI = 5    #�Q�������
COL_LEVEL = 6    #�Q�����x��
COL_LEVEL_RANGE = 7    #���x���͈�
COL_LEVEL_TOTAL = 8    #���x�����v
COL_BOTH_POKE = 9    #�����|�P����
COL_BOTH_ITEM = 10    #�����ǂ���
COL_VETO_POKE = 11    #�Q���֎~�|�P����
COL_VETO_ITEM = 12    #�������݋֎~����
COL_MUST_POKE = 13    #�K�{�|�P����
COL_MUST_POKE_FORM = 14    #�K�{�|�P�����t�H����
COL_SHOOTER = 15    #�V���[�^�[
COL_TIME_VS = 16    #�ΐ펞��
COL_TIME_COMMAND = 17    #���͎���
COL_NICKNAME = 18    #�j�b�N�l�[���\��
COL_AGE_LO = 19    #�N����ȏ�
COL_AGE_HI = 20    #�N����ȉ�
COL_SHOW_POKE = 21    #�|�P������������

POKENUM_MAX_BYTE = (656/8)  ##���̂��炢�ɑ����邩�� �W�Q�o�C�g
ITEMNUM_MAX_BYTE = (608/8)  ##���̂��炢�ɂӂ��邩��




class RegulationBin
  
  def initialize
    @HashLevelRange = {"�Ȃ�"=>0 ,"�ȏ�"=>1 ,"�ȉ�"=>2, "�ȏ�␳"=>3, "�S�␳"=>4, "�ȉ��␳"=>5}
    @HashOKNG = {"NG"=>0,"OK"=>1, "MANUAL"=>2}
    @HashONOFF = {"OFF"=>0,"ON"=>1, "MANUAL"=>2}
    @TempNum = "0"
    @HashPokeGmm = Hash.new
    @HashItemGmm = Hash.new
  end
  
  
  def bitingmm(names, outFH , max, gmmhash)
    bitarray = Array.new
    for i in 0..max
      bitarray.push(0)
    end
    
    byteorder = Array.new([1,2,4,8,16,32,64,128])
    
    
    arrayname = names.split(',')
    
    arrayname.each{|monsname|
      
      keyno = gmmhash.index(monsname).to_i
      if keyno == nil
        p "�s���ȃ|�P�������A�C�e���������̂Œ�~���܂� " + monsname
        exit(-1)
      end
      bitarray[keyno / 8]  = bitarray[keyno / 8] + byteorder[keyno % 8]
    }
    for i in 0..max
      num = bitarray[i]
      outFH.write([num].pack("c"))
    end
    
    
    
  end
  
  
  
  def valuefunc(index , value, outFH)
    case index
    when COL_NO    #No
    when COL_CUPNAME    #�J�b�v��
      msg = NKF.nkf('-m0 -S --utf16',value)
      i = 0
      mojibyte1 = 0
      msg.each_byte{ | mojibyte |
        if i % 2 == 0
          mojibyte1 = mojibyte
        else
          outFH.putc( mojibyte )
          outFH.putc( mojibyte1 )
        end
        i=i+1
      }
      outFH.putc(0xff)  ##�I�[
      outFH.putc(0xff)
      num = 24 - msg.length
      k=0
      while k < num
        outFH.putc(0)
        k=k+1
      end
    when COL_RULENAME    #�ΐ탋�[��
      msg = NKF.nkf("-m0 -S --utf16",value)
      i = 0
      mojibyte1 = 0
      msg.each_byte{ | mojibyte |
        if i % 2 == 0
          mojibyte1 = mojibyte
        else
          outFH.putc( mojibyte )
          outFH.putc( mojibyte1 )
        end
        i=i+1
      }
      outFH.putc(0xff)  ##�I�[
      outFH.putc(0xff)
      num = 24 - msg.length
      k=0
      while k < num
        outFH.putc(0)
        k=k+1
      end
    when COL_NUM_LO    #�Q��������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_NUM_HI    #�Q�������
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_LEVEL    #�Q�����x��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_LEVEL_RANGE    #���x���͈�
      num = @HashLevelRange[value]
      outFH.write([num].pack("c"))
    when COL_LEVEL_TOTAL    #���x�����v
      num = value.to_i
      outFH.write([num].pack("s"))
    when COL_BOTH_POKE    #�����|�P����
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_BOTH_ITEM    #�����ǂ���
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_VETO_POKE    #�Q���֎~�|�P����
      if value =~ /�Ȃ�/
        num = 0
        for i in 0..POKENUM_MAX_BYTE
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, POKENUM_MAX_BYTE ,@HashPokeGmm)
      end        
    when COL_VETO_ITEM    #�������݋֎~����
      if value =~ /�Ȃ�/
        num = 0
        for i in 0..ITEMNUM_MAX_BYTE 
          outFH.write([num].pack("c"))
        end
      else
        bitingmm(value,outFH, ITEMNUM_MAX_BYTE ,@HashItemGmm)
      end        
    when COL_MUST_POKE    #�K�{�|�P����
      if value =~ /�Ȃ�/
        num = 0
        outFH.write([num].pack("s"))
      else
        keyno = @HashPokeGmm.index(value).to_i
        p keyno
        outFH.write([keyno].pack("s"))
      end
    when COL_MUST_POKE    #�K�{�|�P�����t�H����
      num = value.to_i
      outFH.write([num].pack("c")) 
    when COL_SHOOTER    #�V���[�^�[
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    when COL_TIME_VS    #�ΐ펞��
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_TIME_COMMAND    #���͎���
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_NICKNAME    #�j�b�N�l�[���\��
      num = @HashONOFF[value]
      outFH.write([num].pack("c"))
    when COL_AGE_LO    #�N����ȏ�
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_AGE_HI    #�N����ȉ�
      num = value.to_i
      outFH.write([num].pack("c"))
    when COL_SHOW_POKE    #�|�P������������
      num = @HashOKNG[value]
      outFH.write([num].pack("c"))
    else
    end
    
    
  end

  
  
  def programfunc(index , value, outFH)
    case index
    when COL_NO               # No
      @TempNum = value
      
    when COL_PROGRAM_LAVEL    # Program
      num = value.to_i
      outFH.puts( "REG_" + value + " = " +  @TempNum)
    else
    end
  end

  def headerplus(outHeader)
    outHeader.puts("//���M�����[�V�����̃f�[�^�����o�����x���ł�")
    outHeader.puts("//reguration.rb reguration.xls ����̎��������ł�")
    outHeader.puts("#pragma once")
  end
  
  
  def convline( tabfile, outfile,outheader )
    
    outFH = File.new(outfile,"wb")
    outHeader = File.new(outheader,"w")
    
    headerplus(outHeader)
    
    fpr = File.new(tabfile,"r")
    fpr.each{ |line|
      index = 0
      if line =~ /^\d+/
      elsif line =~ /^No/
        next
      else
        break
      end
      tabarray = line.split(/\t+/)
      tabarray.each{ |value|
        valuefunc(index, value, outFH)
        programfunc(index, value, outHeader)
        index = index + 1
        
      }
    }
    outHeader.close
    outFH.close
    fpr.close
    
  end
  
  
  def gmmread( gmmfile , hashgmm)
    fpr = File.new(gmmfile,"r")
    pokeno = 0
    fpr.each{ |line|
      if line =~ /<row\s+id="MONSNAME_(\d+)">/
        pokeno = $1
      elsif line =~ /<row\s+id="ITEMNAME_(\d+)">/
        pokeno = $1
      elsif pokeno != 0
        line = NKF.nkf('-m0 -Ws',line)
        if line =~ /<language.name=.JPN.*>(.*)<\/language>/
          monsname = $1
          hashgmm.store(pokeno, monsname)
        end
        pokeno = 0
      end
    }
    fpr.close
  end
  
  
  def allgmmread(pokegmm,itemgmm)
  
    gmmread(pokegmm, @HashPokeGmm)
    gmmread(itemgmm, @HashItemGmm)
  end
  
end

begin
  
  
  
  
  dep = RegulationBin.new
  dep.allgmmread(ARGV[3],ARGV[4])
  dep.convline( ARGV[0],ARGV[1],ARGV[2] )
  
  
  exit(0)
end
