#!/usr/bin/perl
#
# Nintendo Wi-Fi Connection
#   �Ǝ��Q�[���T�[�o�p�F�؃g�[�N���������v���O�����T���v�� Ver0.1
#
# �����p���@
#   tokendec.pl token
#      token : �F�؃g�[�N��
#

use strict;
use warnings;
use Crypt::CBC;
use Digest::MD5 qw(md5_hex);
use MIME::Base64 qw(decode_base64);


# �F�؃g�[�N������������擾
if ($#ARGV < 0) {
  print STDERR "usage: tokendec.pl token\n";
  exit;
}
my $token = decode_base64($ARGV[0]);
chomp($token);


# AES������
#
# ���L�̃g�[�N�����͖{�T���v���p�̉��̂��̂ł��B
# ���ۂɎg�p����g�[�N�����́A�A�v���P�[�V�������̃T�[�r�X���P�[�^�J���p/���i
# �p�g�[�N���ɒu�������Ă��������B
# �T�[�r�X���P�[�^�ݒ���́ANWCWMS�ɂ��Ǝ��Q�[���T�[�o�̗��p�\�������F����
# �����_��NWCWMS�ɂĂ��m�点���܂��B
my $key = "1234123412341234";   # �g�[�N����
my $iv  = pack("H*", "0" x 32); # IV
my $cipher = Crypt::CBC->new({"key"            => $key,
                              "cipher"         => "Rijndael_PP_128",
                              "iv"             => $iv,
                              "regenerate_key" => 0,
                              "padding"        => "null",
                              "prepend_iv"     => 0
                             });
my $dectoken = $cipher->decrypt($token);


# �g�[�N�����
if ($dectoken !~ /^\\u\\(\d{13})\\m\\(\w{12})\\t\\(\d+)\\h\\(\w{8})$/ &&  # DS
    $dectoken !~ /^\\w\\(\d{16})\\m\\(\w{12})\\t\\(\d+)\\h\\(\w{8})$/) {  # Wii
  print STDERR "Invalid Token\n";
  exit;
}

my ($userid, $macadr, $timestamp, $hash) = ($1, $2, $3, $4);
print "UserID    : " . $userid . "\n";
print "MACADR    : " . $macadr . "\n";
print "TimeStamp : " . $timestamp . "\n";
print "Hash      : " . $hash . "\n";


# �n�b�V���l����H
$dectoken =~ /^(.*)\\h\\\w{8}$/;
my $calchash = substr(md5_hex($1), 0, 8);
if ($calchash ne $hash) {
  print STDERR "Hash Invalid (" . $calchash . ")\n";
}


# 24���Ԉȓ��H
if (time - $timestamp > 24 * 60 * 60) {
  print STDERR "Token Expired (" . (time - $timestamp) . ")\n";
}


exit;
