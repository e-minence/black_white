#!/usr/bin/ruby
#
# �ץ���ʤ�gmm��WB�˶����Ѵ�����ġ���Ǥ�  �����ˤ� GmmRead���饹��������ޤ���
#
#
#

require "nkf"
require "rexml/document"
include REXML



### �ᥤ��������饹

class GmmRead
  
  ##�֤���������ʪ�򤳤��˽񤱤Ф褤
  @@TagTbl=[
  ['��������','[2:00:����'],
  ['��������','[2:01:����'],
  ['��������','[2:02:����'],
  ['��������','[2:03:����'],
  ['��������','[2:04:����'],
  ['��������','[2:05:����'],
  ['��������','[2:06:����'],
  ['��������','[2:07:����'],
  ['��������',''],                      ##WB�ʤ��ʤ�ޤ���
  ['����10��',''],                      ##WB�ʤ��ʤ�ޤ���
  ['�ݥ����̾','[1:01:�ݥ�����²̾'],
  ['�˥å��͡���','[1:02:�ݥ����˥å��͡���'],
  ['�ݥ����ʬ��','[1:04:�ݥ����ʬ��'],
  ['�ȥ졼�ʡ�̾','[1:00:�ȥ졼�ʡ�̾'],
  ['�ޥå�̾','[1:05:�ޥå�̾'],
  ['�Ȥ�����̾','[1:06:�Ȥ�����̾'],
  ['�虜̾','[1:07:�虜̾'],
  ['��������','[1:08:��������'],
  ['�ɤ���̾','[1:09:�ɤ���̾'],
  ['�ݥ�å�̾',''],                      ##WB�ʤ��ʤ�ޤ���
  ['���å�̾','[1:10:���å�̾'],
  ['�ܥå���̾','[1:11:�ܥå���̾'],
  ['attackmsg',''],                      ##WB�ʤ��ʤ�ޤ���
  ['���ơ�����̾','[1:13:���ơ�����̾'],
  ['�ȥ졼�ʡ�����̾','[1:14:�ȥ졼�ʡ�����̾'],
  ['�ݥ����Υ�����','[1:03:�ݥ���󥿥���'],
  ['�ڤμ¤�̣����Ʈ�ѡ�',''],            ##WB�ʤ��ʤ�ޤ���
  ['�ü����̾','[1:17:�ü����̾'],
  ['�Хå��Υݥ��å�̾','[1:18:�Хå��Υݥ��å�̾'],
  ['���֡ʤɤ���������',''],            ##WB�ʤ��ʤ�ޤ���
  ['���֡ʤ虜������',''],            ##WB�ʤ��ʤ�ޤ���
  ['���֡ʤȤ�����������',''],            ##WB�ʤ��ʤ�ޤ���
  ['���֡ʤ�����ƥ����ȡ�',''],            ##WB�ʤ��ʤ�ޤ���
  ['���֡ʤ��Τ�������',''],            ##WB�ʤ��ʤ�ޤ���
  ['�ݥ��å����ե�̾',''],            ##WB�ʤ��ʤ�ޤ���
  ['�ϲ��Τɤ�����̾��',''],            ##WB�ʤ��ʤ�ޤ���
  ['���å�̾','[1:26:���å�̾'],   ##����Υ쥮��졼�����̾
  ['�ݥ��å�̾�ʥ�������Ĥ���',''],            ##WB�ʤ��ʤ�ޤ���
  ['�ʰײ���ñ��','[1:28:�ʰײ���ñ��'],
  ['��̩���ü������',''] ,           ##WB�ʤ��ʤ�ޤ���
  ['��̩���ü��������',''],            ##WB�ʤ��ʤ�ޤ���
  ['���������꡼̾','[1:31:���������꡼̾'],
  ['����̾','[1:32:����̾'],
  ['������','[1:33:������'],
  ['����ƥ���̾��','[1:34:����ƥ���̾��'],
  ['����ƥ��ȥ��','[1:35:����ƥ��ȥ��'],
  ['��̾','[1:36:��̾'],
  ['�ϰ�̾','[1:37:�ϰ�̾'],
  ['������̾��',''],            ##WB�ʤ��ʤ�ޤ���
  ['��ܥ��̾��','[1:39:��ܥ��̾��'],
  ['���᡼������å��ط�',''],            ##WB�ʤ��ʤ�ޤ���
  ['GTC:�ݥ����ʬ�����',''],            ##WB�ʤ��ʤ�ޤ���
  ['GTC:�ݥ�������̻������',''] ,           ##WB�ʤ��ʤ�ޤ���
  ['GTC:��٥�������',''],            ##WB�ʤ��ʤ�ޤ���
  ['���ܳ�',''],            ##WB�ʤ��ʤ�ޤ���
  ['�����̥����ɥ�������',''],           ##WB�ʤ��ʤ�ޤ���
  ['ʸ��ɽ����������','[190:02:���������ѹ���ñȯ��'],
  ['������Хå�����','[190:05:������Хå������ѹ���ñȯ��'],
  ['ʸ��ɽ������X',''],           ##WB�ʤ��ʤ�ޤ���
  ['ʸ��ɽ������Y',''],            ##WB�ʤ��ʤ�ޤ���
  ['����ߥ˥�����','[1:40:����ߥ˥�����'],
  ['���쥤�٥��̾','[1:41:���쥤�٥��̾'],
  ['���쥢���ƥ�̾','[1:42:���쥢���ƥ�̾'],
  ['���ܤΰ���','[1:43:���ܤΰ���'],
  ['�Ѹ�ΰ���','[1:44:�Ѹ�ΰ���'],
  ['�ե�󥹸�ΰ���','[1:45:�ե�󥹸�ΰ���'],
  ['�ɥ��ĸ�ΰ���','[1:46:�ɥ��ĸ�ΰ���'],
  ['�����ꥢ��ΰ���','[1:47:�����ꥢ��ΰ���'],
  ['���ڥ����ΰ���','[1:48:���ڥ����ΰ���'],
  ['���󥿥��','[BD:02:�إ��󥿥��'],
  ['����','[BD:03:�ر���'],
  ]

  
  def TagConv( )
    
    sjisline=""
    @ConvFileLine.each{ |line|
      sjisline = NKF.nkf("-e",line)
      @@TagTbl.each{ |tagbuf|
        if sjisline =~ /\[\d+:\d+:#{tagbuf[0]}:*\d*\]/
          if tagbuf[1] != ''
            p NKF.nkf("-e", sjisline)
            p NKF.nkf("-e", tagbuf[1])
            sjisline = sjisline.gsub(/\[\d+:\d+:#{tagbuf[0]}/,tagbuf[1])
          else
            p "Warring:Find not WBTag : " + @ConvFileLineTmp.length.to_s + " : " + sjisline
          end
        end
      }
      utfline = NKF.nkf("-w",sjisline)
      
      #utfline = line
      @ConvFileLineTmp.push(utfline)
    }
    @ConvFileLine = @ConvFileLineTmp
    @ConvFileLineTmp = Array.new
  end
  
  
  
  
  def initialize
    @TmplateFileLine = Array.new
    @ConvFileLine = Array.new
    @ConvFileLineTmp =  Array.new
  end
  
  
  
  def FileRead( gmmrenew )
    @ConvFileLine = Array.new
    p "conv:"+gmmrenew
    fpr = File.new(gmmrenew)
    fpr.each{ |line|
      @ConvFileLine.push(line)
    }
    fpr.close
  end
  
  
  def FileWrite( gmmrenew )
    fpr = File.new(gmmrenew,"w")
    @ConvFileLine.each{ |line|
      fpr.puts(line)
    }
    fpr.close
  end
  
  def TmplateRead( gmmrenew )
    @TmplateFileLine = Array.new
    fpr = File.new(gmmrenew)
    fpr.each{ |line|
      @TmplateFileLine.push(line)
    }
    fpr.close
  end
  
  
  def TmplateMake( )
    @TmplateFileLine.each{ |line|
      if line =~ /(<row)/
        break
      end
      @ConvFileLineTmp.push(line)
    }
    @TmplateFileLine = @ConvFileLineTmp
    @ConvFileLineTmp = Array.new
  end
  
  
  def TmplateFootMake( )
    writeflg = 0
    @TmplateFileLine.each{ |line|
      if line =~ /(<flowchart-group-list)/
        writeflg = 1
      end
      if writeflg == 1
        @ConvFileLine.push(line)
      end
    }
  end
  
  
  def BodyAdd( )
    @ConvFileLineTmp = @TmplateFileLine
    writeflg = 0
    @ConvFileLine.each{ |line|
      if line =~ /(<row)/
        writeflg = 1
      end
      if writeflg==1
        @ConvFileLineTmp.push(line)
      end
    }
    @ConvFileLine = @ConvFileLineTmp
    @ConvFileLineTmp = Array.new
  end
  
  
  
  
  def JpnConv( )
    
    @ConvFileLine.each{ |line|
      sjisline = NKF.nkf("-e",line)
      if sjisline =~ /(<language name="���ܸ�")/
        sjisline = sjisline.sub('<language name="���ܸ�"','<language name="JPN"')
      end
      utfline = NKF.nkf("-w",sjisline)
      #utfline = line
      @ConvFileLineTmp.push(utfline)
    }
    @ConvFileLine = @ConvFileLineTmp
    @ConvFileLineTmp = Array.new
  end
  
  
  def JpnDupe()
    dupearray = Array.new
    dupeflg = 0
    
    @ConvFileLine.each{ |line|
      sjisline = NKF.nkf("-e",line)
      if sjisline =~ /<language name/
        dupeflg = 1
      end
      if dupeflg == 1
        dupearray.push(sjisline)
      end
      if sjisline =~ /language>/
        dupeflg = 2
      end
      utfline = NKF.nkf("-w",sjisline)
      @ConvFileLineTmp.push(utfline)
      
      if dupeflg == 2
        dupearray.each{ |sjisline|
          if sjisline =~ /(<language name="JPN")/
            sjisline = sjisline.sub('<language name="JPN"','<language name="JPN_KANJI"')
          end
          utfline = NKF.nkf("-w",sjisline)
          @ConvFileLineTmp.push(utfline)
        }
        dupeflg = 0
        dupearray = Array.new
      end
    }
    @ConvFileLine = @ConvFileLineTmp
    @ConvFileLineTmp = Array.new
  end
  
  
  
end



