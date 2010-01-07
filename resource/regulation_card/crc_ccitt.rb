



  ##CRC-CCITT = x16 + x12 + x5 + 1
  
  
#  (1) �����l�Ƃ��āAcrcReg��0xFFFF������
#(2) inData�ɓ��͒l�i1�o�C�g�j���i�[���A�ȉ�(3)�`(6)��8�񃋁[�v������
#(3) crcReg��inData�̔r���I�_���a�����A�ŉ����ޯĂ̂�w�ɒ��o�B
#�@�@�iw = crcReg ^ inData & 0x0001�j
#(4) crcReg���E��1�r�b�g�V�t�g��crcReg�Ɋi�[ (crcReg = crcReg >> 1)
#(5) w��1�ł���΁AcrcReg��0x8408�̔r���I�_���a����������ʂ�crcReg�Ɋi�[����B
#�@�@w��0�Ȃ牽�����Ȃ��B
#(6) inData���E��1�r�b�g�V�t�g���� (inData = inData >> 1)
  
  

  
 
begin
  crcReg = 0xffff
  
  fpr = File.new(ARGV[0],"rb")
  fpw = File.new(ARGV[1],"wb")
    
  fpr.each_byte{| data |
    inData = data
    fpw.write([data].pack("c"))
    
    
    for i in 1..8 
      w = (crcReg ^ inData) & 0x0001
      crcReg = crcReg >> 1
      if w == 1
        crcReg = crcReg ^ 0x8408
      end
      inData = inData >> 1
    end
  }
  fpr.close
  
  fpw.write([crcReg].pack("s"))
  fpw.close
  
end


