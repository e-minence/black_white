#!/usr/bin/ruby -Ks
# �t�@�C����A������ �w�b�_�[�ɂ͘A�������t�@�C���̑��΃A�h���X������
#  > binlinker.rb �R�s�[���t�@�C�� ...    �o�͂���t�@�C�� + �o�͂���t�@�C���w�b�_�[����
#    �� �R�s�[���t�@�C���͕����w��ł���
#    �� �o�͂���t�@�C���w�b�_�[�����͂Q����
# ��  ruby binlinker.rb test1.bin test2.bin test3.bin output.bin MOJI
# k.ohno   2008.08.29


require 'ftools'

begin
  exit 1 if ARGV.size < 2

  outFileHeaderMsg = ARGV.pop
  outFileName = ARGV.pop
  
  ArrayFile = []
  ArraySize = []
  counter = 0
  
  ARGV.each do |sfile|
    if FileTest.exist?(sfile)      # ���݊m�F
      ArraySize[counter] = File.size(sfile)
      ArrayFile[counter] = sfile;
      counter = counter + 1
    else
      ArraySize[counter] = 0  # �t�@�C���������ꍇ�O�ɂȂ�
      ArrayFile[counter] = sfile;
      counter = counter + 1
    end
  end
  
  if counter == 0
    exit 1
  end

  writer = File::open(outFileName,"wb")
  
  writer.write(outFileHeaderMsg)
  writer.putc(0)
  writer.putc(0)
  
  totalsize = (counter + 2) * 4
  ArraySize.each do |size|
    writer.putc(totalsize)
    writer.putc(totalsize / 256)
    writer.putc(totalsize / 65536)
    writer.putc(totalsize / 16777216)
    totalsize = totalsize + size
  end

  writer.putc(totalsize)
  writer.putc(totalsize / 256)
  writer.putc(totalsize / 65536)
  writer.putc(totalsize / 16777216)


  ArrayFile.each do |sfile|
    if FileTest.exist?(sfile)      # ���݊m�F
      baseHandle = File::open(sfile)
      writer.write(baseHandle.read)
      baseHandle.close
    end
  end
  
  writer.close


end
