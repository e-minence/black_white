#-------------------------------------------------------------------------------
# �f�o�b�O�T�E���h�pgmm�t�@�C�������c�[��
# 
# ���s����Ɓusound_data.sadl�v�̒�����uSEQ_�v���}�b�`���镶��������o����
# �T�E���h�f�o�b�O��ʂŎg��gmm�t�@�C�����쐬���܂��B
# �����t�H���_�Ɂusound_data.sadl�v�uname_gmm.head�v�uname_gmm.fott�v���K�v�ł��B
# 
# 2008.01.10 by AkitoMori
#-------------------------------------------------------------------------------



#�����R�[�h��UTF-8�ɕϊ�����̂ɕK�v
#require "iconv"
require "nkf"   ## iconv��cygwin��ruby���ƃo�O���Ă�炵���̂� nkf�ɕύX k.ohno 2009.1.9


#�ǂݍ��݃t�@�C����
input_soundfile  = "wb_sound_data.sadl"
input_headerfile = "name_gmm.head"
input_footerfile = "name_gmm.foot"

#�o�̓t�@�C����
output_gmm_file  = "snd_test_name.gmm"

#sprintf�p��`
output_msg_line  = "\t<row id=\"%s\">\n\t\t<language name=\"���{��\">%s</language>\n\t</row>\n"



#�o�͂Ɏg���t�@�C���̓ǂݍ���
headfile = File.open("name_gmm.head","r")
headstr  = headfile.read

footfile = File.open("name_gmm.foot","r")
footstr  = footfile.read

#�o�̓t�@�C���̃I�[�v��
outputfile = File.open(output_gmm_file,"w")

#�w�b�_�����o��
outputfile.puts(headstr)

#�T�E���h�V�[�P���X�̂ݎ��o�����`����gmm�ɏo��
File.open( input_soundfile ){|file|
	file.each{|line|
		divline = line.split(nil)
		seqname = divline[1]

		#�uSEQ_�v�����Ă����`���̂ݎ��o��
		if seqname=~/^SEQ_/ then

			#�o�͗p�ɖ��O�����H
			msgid   = "msg_"+seqname.downcase
			msgstr  = seqname.sub(/SEQ_SE_/,"")
			msgstr  = msgstr.sub(/SEQ_/,"")
			msgstr  = msgstr.gsub(/_/,"-")
			
			#GMM�p�ɕ�����𐮌`
			tmpbuf = sprintf( output_msg_line, msgid, msgstr)

      #UTF-8�ɕϊ����ďo��
#            tbuf = Iconv.iconv("UTF-8","SJIS",tmpbuf)
      
            tbuf = NKF.nkf("-w",tmpbuf)
            outputfile.puts(tbuf)
			
		end
	}
}


#�t�b�^�[�o��
outputfile.puts(footstr)


