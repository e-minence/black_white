#-------------------------------------------------------------------------------
# �f�o�b�O�T�E���h�pgmm�t�@�C�������c�[��
# 
# ���s����Ɓusound_data.sadl�v�̒�����uSEQ_�v���}�b�`���镶��������o����
# �T�E���h�f�o�b�O��ʂŎg��gmm�t�@�C�����쐬���܂��B
# �����t�H���_�Ɂusound_data.sadl�v+ message�t�H���_�[��template.gmm���Œ�Q�Ƃ��܂�
# ruby gmm_make.rb template.gmm wb_sound_data.sadl soundtestname.gmm
# 2008.01.10 by AkitoMori
# 2009.01.28 k.ohno
#-------------------------------------------------------------------------------


require "nkf"
require "../../tools/gmmconv.rb"





class SEQNAMEConv < GmmRead
  
  def initialize
    super
  end
  
    ### ROW�^�O������
  
  
  

  ##SEQ����ROW�f�[�^�����
  def SeqRowAdd()
    gmmno = 0
    @ConvFileLineTmp = @TmplateFileLine
    #�T�E���h�V�[�P���X�̂ݎ��o�����`����gmm�ɏo��
    @ConvFileLine.each{ |line|
    
      divline = line.split(nil)
      seqname = divline[1]
      
      #�uSEQ_�v�����Ă����`���̂ݎ��o��
      if seqname=~/^SEQ_/ then
        gmmend = divline[2].to_i
        gmmend = gmmend - 1
        for xn in gmmno..gmmend
          
tbufdmy = <<"EOFZ"
          <row id="msg_seq_#{xn}">
          <language name="JPN" width="256">dmy</language>
          <language name="JPN_KANJI" width="256">dmy</language>
          </row>
EOFZ
          tbufdmy = NKF.nkf("-w",tbufdmy)
          @ConvFileLineTmp.push(tbufdmy)
          
        end
        gmmno = divline[2].to_i + 1
        #�o�͗p�ɖ��O�����H
        seqidname   = "msg_"+seqname.downcase
        msgstr  = seqname.sub(/SEQ_SE_/,"")
        msgstr  = msgstr.sub(/SEQ_/,"")
        seqmsg  = msgstr.gsub(/_/,"-")
        
 tbuf = <<"EOF"
	<row id="#{seqidname}">
		<language name="JPN" width="256">#{seqmsg}</language>
		<language name="JPN_KANJI" width="256">#{seqmsg}</language>
	</row>
EOF
        tbuf = NKF.nkf("-w",tbuf)
        @ConvFileLineTmp.push(tbuf)
			
      end
    }
    @ConvFileLine = @ConvFileLineTmp
    @ConvFileLineTmp = Array.new
    end
  
end



### ���C��
begin
  
  GRead = SEQNAMEConv.new
  GRead.FileRead( ARGV[1] )  ##�R���o�[�g�Ώۃt�@�C����ǂݍ���
  GRead.TmplateRead( ARGV[0] ) ## �e���v����ǂݍ���
  GRead.TmplateMake()  ##  �e���v�����������o��
  GRead.SeqRowAdd() ##SEQ��row�^�O�ɂ��Ēǉ�
  
  GRead.TmplateRead( ARGV[0] ) ## �e���v����ǂݍ���
  
  GRead.TmplateFootMake()  ##  �e���v���̍Ō��ǉ�
  
  GRead.FileWrite( ARGV[2] )  ##�R���o�[�g���ʂ��o�͂���
      
 ## exit(0)
end
