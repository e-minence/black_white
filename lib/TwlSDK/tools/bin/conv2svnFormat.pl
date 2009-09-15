#!/usr/bin/perl -s
#----------------------------------------------------------------------------
# Project:  TwlSDK - tools
# File:     conv2svnFormat.pl
#
# Copyright 2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Date:: 2007-09-19#$
# $Rev: 997 $
# $Author: ooshimay $
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# ���ݔN�̐���\�����擾
#----------------------------------------------------------------------------
sub GetThisYear
{
	my	@array	=	localtime(time);

	return (@array[5] + 1900);
}

#----------------------------------------------------------------------------
# ���C�����[�`��
#----------------------------------------------------------------------------
{
	my	$input_file;
	my	$output_file;
	my	$line;

	# ���̓t�@�C����ʖ��t�@�C���ɃR�s�[
	$output_file	=	@ARGV[0];
	$input_file		=	$output_file . ".convbackup";
	system `cp -rf $output_file $input_file`;

	open INPUTFILE, "$input_file" or die "ERROR: Cannot open file \'$input_file\'\n";
	open OUTPUTFILE, ">$output_file" or die "ERROR: Cannot create file \'$output_file\'\n";

	while ($line = <INPUTFILE>)
	{
		# Project �������u��
		if ($line =~ m/.*Project: *NitroSDK.*$/)
		{
			$line =~ s/NitroSDK/TwlSDK/;
		}
	
		# Copyright �N�������u��
		if ($line =~ m/.*Copyright [0-9]+.*$/)
		{
			my	$start	=	$line;
			my	$now	=	GetThisYear();
		
			$start	=~	s/.*Copyright ([0-9]+).*\n/$1/;
			if ($start >= $now)
			{
				$line	=~	s/Copyright [0-9\-,]+/Copyright $now/;
			}
			else
			{
				$line	=~	s/Copyright [0-9\-,]+/Copyright $start/;
			}
		}
	
		# Log �s���ւ̎����ϊ�������ɒu��
		if ($line =~ m/\${1}Log.*\$/)	# �������g����ϊ����Ȃ��ׂ̏璷�ȕ\��
		{
			my	$prefix	=	$line;
			my	$suffix	=	$line;
		
			$prefix	=~	s/^(.*)\${1}Log.*\$.*\n/$1/;	# �������g����ϊ����Ȃ��ׂ̏璷�ȕ\��
			#$suffix	=~	s/.*(\r?\n)$/$1/;
			if ($line =~ m/.*\r\n$/)
			{
				$suffix	=	"\r\n";
			}
			else
			{
				$suffix	=	"\n";
			}
		
			print OUTPUTFILE $prefix . "\$" . "Date::            \$" . $suffix;
			print OUTPUTFILE $prefix . "\$" . "Rev:\$" . $suffix;
			print OUTPUTFILE $prefix . "\$" . "Author:\$" . $suffix;
		
			while ($line = <INPUTFILE>)
			{
				if ((index($line, $prefix, 0) == 0) && ($line =~ m/.*Revision [0-9\.]+.*/))
				{
					while ($line = <INPUTFILE>)
					{
						$line	=~	s/\r?\n$//;
						if (length($line) <= length($prefix))
						{
							last;
						}

						# NoKeywords �܂ōs���Ă��܂����ꍇ��������
						if ($line =~ m/\${1}NoKeywords.*\$/)
						{
							last;
						}
					}
				}
				else
				{
					last;
				}
			}
		}
	
		# NoKeywords �s���폜
		if ($line =~ m/\${1}NoKeywords.*\$/)	# �������g����ϊ����Ȃ��ׂ̏璷�ȕ\��
		{
			next;
		}
	
		# �p�^�[���Ƀ}�b�`���Ȃ��s�͂��̂܂܏o��
		print OUTPUTFILE $line;
	}

	close OUTPUTFILE;
	close INPUTFILE;

	# �o�̓t�@�C���� sub-version ������t��
	system `svn propset -q --force "svn:keywords" "Id URL Author Date Rev" $output_file`;
	if ($output_file =~ m/.*\.sh$/)
	{
		system `svn propset -q --force "svn:eol-style" "LF" $output_file`;
		system `svn propset -q --force "svn:executable" "1" $output_file`;
	}
	elsif ($output_file =~ m/.*\.pl$/)
	{
		system `svn propset -q --force "svn:eol-style" "CRLF" $output_file`;
		system `svn propset -q --force "svn:executable" "1" $output_file`;
	}
	else
	{
		system `svn propset -q --force "svn:eol-style" "CRLF" $output_file`;
	}

	# �ꎞ�I�ɃR�s�[�����t�@�C�����폜
	system `rm -rf $input_file`
}
