#pragma once

#include <iostream>

using namespace std;



namespace PokeIRC {

	using namespace System;
	using namespace System::Diagnostics;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Text::RegularExpressions;




	//==============================================================================
	/**
	 * IRCライブラリラッパークラス
	 */
	//==============================================================================
	public enum TR_URL_ENUM : unsigned char {
		TR_URL_UPLOAD=10,
		TR_URL_UPLOADFINISH,
		TR_URL_DOWNLOAD,
		TR_URL_GETUPLOADRESULT,
		TR_URL_DELETE,
		TR_URL_RETURN,
		TR_URL_DOWNLOADMATCHDATA,
		TR_URL_TRADE,
		TR_URL_TRADEFINISH,
		TR_URL_GETSERVERSTATE,
		TR_URL_INIT,
		TR_URL_UPDATE,
		TR_URL_SHUFFLE,
		TR_URL_MAX
	};
	typedef enum {
	    IRC_COMMAND_BOXLIST = 50,
	    IRC_COMMAND_BOXLISTEND,
	    IRC_COMMAND_BOXPOKE,
	    IRC_COMMAND_BOXPOKEEND,
		IRC_COMMAND_BOXPOKEDEL,
		IRC_COMMAND_BOXPOKEDELEND,
	    IRC_COMMAND_SNDV,
	    IRC_COMMAND_SNDEND,
	} IRC_COMMAND_e;



	public ref class NetIRC
	{
	private:

		static bool isEmulate;
		static const unsigned long long TEST_KEY = 0x100000000UL;


		static bool isSend;
		static bool isDataSend;

		static int URLNo;
		static String^ s_currentProxy;
		static u8 SendValue;   //通信コマンド


	public:
		static void Init(void);
		//static void callback(unsigned char *data, unsigned char size, unsigned char command, unsigned char value);
		static void draw(Form^ fm);
		static bool connect(void);
		static void shutdown(void);
		static bool sendData(u8 value);
		static bool SendLoop(void);
		static array<unsigned char>^ dataArray;

		static array<unsigned char>^ recvdataArray;
		static int recvDataSize;
	private:
		static bool isRecv;   //コマンドを受け取り完了
	public:
		static bool isRecvFlg(void){ return isRecv; }


		static void RecvURLCOMMAND(unsigned char * data,int size,unsigned char value);
		static int WaitForAsync(void);

	private:
		static void RequestUpload(void);
		static void RequestServerState(void);
		static void RequestPickupTraded(void);
		static void RequestDownload(void);
		static void RequestDownloadMatchData(void);




	private:
		static void _resultSend(int value);
	public:
		static void SetProxy(String^ proxy);



	};

}


