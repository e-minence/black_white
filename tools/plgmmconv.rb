#!/usr/bin/ruby
#
# �ץ���ʤ�gmm��WB�˶����Ѵ�����ġ���Ǥ�
# ���Υġ����  �ץ���ޤ����ݡ��Ȥ��ƻȤ����������ꤷ�Ƥ��ޤ���
#
#  1.�ƥ�ץ졼���֤�����
#     �������Υƥ�ץ졼�Ȥ򥳥�С��ȥե�����Υƥ�ץ졼�Ȥ˺����ؤ�
#  2.�����֤�����
#     �����ơ��֥��񤱤о�����ִ����Ƥ����
#  3.����������
#     ���ޤǤΰ������ä��Τ�,���ܸ�����ܸ��������Ԥ����䤹
#  4.���ܸ� -> JPN�Ѵ�
#     language="���ܸ�" �Ȥ�������� "JPN" ���Ѥ�ä��Τ��ִ����Ƥ����
#  
#  ����餬���̼������Ƥ��ޤ��Τ� �������Ӥˤ������äơ������Υץ����򥳥��Ȥˤ��Ĥ�
#  �Ѵ���ԤäƤ�������
#
#  ������ˡ�㡧 ruby ./tools/plgmmconv.rb ./resource/message/template.gmm ./resource/message/src/pl_wifi_lobby.gmm
#
#

require "nkf"
require "rexml/document"
include REXML
require "tools/gmmconv.rb"

### �ᥤ��
begin
  
  GRead = GmmRead.new
  GRead.FileRead( ARGV[1] )  ##����С����оݥե�������ɤ߹���
  GRead.TmplateRead( ARGV[0] ) ## �ƥ�ץ���ɤ߹���
  GRead.TmplateMake()  ##  �ƥ�ץ�򺹤��ؤ���
  GRead.BodyAdd()
  GRead.JpnConv()    ##   ���ܸ��JPN���ѹ�
  GRead.JpnDupe()    ##   JPN��JPN_KANJI�����䤹
  GRead.TagConv()   ## �������֤�������
  GRead.FileWrite( ARGV[1])  ##����С��ȷ�̤���Ϥ���
  
  
  exit(0)
end


