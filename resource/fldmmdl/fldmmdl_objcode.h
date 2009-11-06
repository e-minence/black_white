//動作モデル OBJコード定義
#ifndef _FLDMMDL_OBJCODE_
#define _FLDMMDL_OBJCODE_

#define NONDRAW (0x0) //0 描画無し
#define HERO (0x1) //1 主人公男
#define CYCLEHERO (0x2) //2 主人公男自転車
#define SWIMHERO (0x3) //3 主人公男波乗り
#define HEROINE (0x4) //4 主人公女
#define CYCLEHEROINE (0x5) //5 主人公女自転車
#define SWIMHEROINE (0x6) //6 主人公女波乗り
#define RIVEL (0x7) //7 ライバル
#define BABYBOY1 (0x8) //8 男の子
#define BABYGIRL1 (0x9) //9 女の子
#define BOY1 (0xa) //10 少年１
#define BOY2 (0xb) //11 少年２
#define BOY3 (0xc) //12 少年３
#define BOY4 (0xd) //13 少年４
#define GIRL1 (0xe) //14 少女１
#define GIRL2 (0xf) //15 少女２
#define GIRL3 (0x10) //16 少女３
#define GIRL4 (0x11) //17 少女４
#define MAN1 (0x12) //18 青年１
#define MAN2 (0x13) //19 青年２
#define MAN3 (0x14) //20 青年３
#define WOMAN1 (0x15) //21 女性
#define WOMAN2 (0x16) //22 女性２
#define WOMAN3 (0x17) //23 女性３
#define MIDDLEMAN1 (0x18) //24 中年男性１
#define MIDDLEMAN2 (0x19) //25 中年男性２
#define MIDDLEWOMAN1 (0x1a) //26 中年女性１
#define MIDDLEWOMAN2 (0x1b) //27 中年女性２
#define OLDMAN1 (0x1c) //28 おじいさん
#define OLDWOMAN1 (0x1d) //29 おばあさん
#define TRAINERM (0x1e) //30 トレーナー♂
#define TRAINERW (0x1f) //31 トレーナー♀
#define VETERANM (0x20) //32 ベテラン♂
#define VETERANW (0x21) //33 ベテラン♀
#define BREEDERM (0x22) //34 ブリーダー♂
#define BREEDERW (0x23) //35 ブリーダー♀
#define RANGERM (0x24) //36 レンジャー♂
#define RANGERW (0x25) //37 レンジャー♀
#define SPORTSM (0x26) //38 ジョギング♂
#define SPORTSW (0x27) //39 ジョギング♀
#define CYCLEM (0x28) //40 サイクリング♂
#define CYCLEW (0x29) //41 サイクリング♀
#define SWIMMERM (0x2a) //42 スイム♂
#define SWIMMERW (0x2b) //43 スイム♀
#define WAITER (0x2c) //44 ウエーター
#define WAITRESS (0x2d) //45 ウエートレス
#define GENTLEMAN (0x2e) //46 紳士
#define LADY (0x2f) //47 淑女
#define FIGHTERM (0x30) //48 格闘家♂
#define FIGHTERW (0x31) //49 格闘家♀
#define BACKPACKERM (0x32) //50 バックパッカー♂
#define BACKPACKERW (0x33) //51 バックパッカー♀
#define DOCTOR (0x34) //52 ドクター
#define NURSE (0x35) //53 ナース
#define CAMERAMAN (0x36) //54 カメラマン
#define REPORTER (0x37) //55 レポーター
#define BUSINESSMAN (0x38) //56 サラリーマン
#define OL (0x39) //57 ＯＬ
#define CLOWN (0x3a) //58 ピエロ
#define DANCER (0x3b) //59 ダンサー
#define JUGGLING (0x3c) //60 ピエロ
#define MUSICIAN (0x3d) //61 ミュージシャン
#define BASEBALLM (0x3e) //62 野球選手
#define SOCCERM (0x3f) //63 サッカー選手
#define FOOTBALLM (0x40) //64 アメフト選手
#define TENNISW (0x41) //65 テニス選手
#define BADMAN (0x42) //66 不良
#define BADRIDER (0x43) //67 暴走族
#define FISHING (0x44) //68 釣り人
#define MOUNTMAN (0x45) //69 山男
#define MAID (0x46) //70 メイド
#define BAKER (0x47) //71 パン屋
#define WORKMAN (0x48) //72 作業員
#define RAILMAN (0x49) //73 鉄道員
#define CLEANINGM (0x4a) //74 清掃員
#define DELIVERY (0x4b) //75 配達員
#define PILOT (0x4c) //76 パイロット
#define POLICEMAN (0x4d) //77 ポリスマン
#define DRIVER (0x4e) //78 ドライバー
#define ASSISTANTM (0x4f) //79 助手♂
#define ASSISTANTW (0x50) //80 助手♀
#define INFORMATIONM (0x51) //81 受付♂
#define INFORMATIONW (0x52) //82 受付♀
#define SHOPM1 (0x53) //83 ショップ店員
#define PCWOMAN1 (0x54) //84 ポケセン受付１
#define PCWOMAN2 (0x55) //85 ポケセン受付２
#define PCWOMAN3 (0x56) //86 ポケセン受付３
#define WIFISM (0x57) //87 Wifi係員♂
#define WIFISW (0x58) //88 Wifi係員♀
#define SUNGLASSES (0x59) //89 サングラス
#define AMBRELLA (0x5a) //90 パラソル
#define LEADER1 (0x5b) //91 ジムリーダー１
#define LEADER2 (0x5c) //92 ジムリーダー２
#define LEADER3 (0x5d) //93 ジムリーダー３
#define LEADER4 (0x5e) //94 ジムリーダー４
#define LEADER5 (0x5f) //95 ジムリーダー５
#define LEADER6 (0x60) //96 ジムリーダー６
#define LEADER7 (0x61) //97 ジムリーダー７
#define LEADER8 (0x62) //98 ジムリーダー８
#define BIGFOUR1 (0x63) //99 四天王１
#define BIGFOUR2 (0x64) //100 四天王２
#define BIGFOUR3 (0x65) //101 四天王３
#define BIGFOUR4 (0x66) //102 四天王４
#define CHAMPION (0x67) //103 チャンピオン
#define HAKAIM (0x68) //104 ハカイ団♂
#define HAKAIW (0x69) //105 ハカイ団♀
#define SAGE1 (0x6a) //106 7賢者１
#define SAGE2 (0x6b) //107 7賢者２
#define SAGE3 (0x6c) //108 7賢者３
#define SAGE4 (0x6d) //109 7賢者４
#define SAGE5 (0x6e) //110 7賢者５
#define SAGE6 (0x6f) //111 7賢者６
#define SAGE7 (0x70) //112 7賢者７
#define GODDESS1 (0x71) //113 ２女神１
#define GODDESS2 (0x72) //114 ２女神２
#define BIGFOUR5 (0x73) //115 悪の四天王１
#define BIGFOUR6 (0x74) //116 悪の四天王２
#define BIGFOUR7 (0x75) //117 悪の四天王３
#define BIGFOUR8 (0x76) //118 悪の四天王４
#define BOSS (0x77) //119 Ｎ
#define HAKASE1 (0x78) //120 博士１
#define HAKASE2 (0x79) //121 博士２
#define ICPO (0x7a) //122 ハンサム
#define TBOSS (0x7b) //123 タワーボス
#define TREE (0x7c) //124 切れる木
#define ROCK (0x7d) //125 怪力の岩
#define MONSTERBALL (0x7e) //126 アイテムカプセル
#define PIKACHU (0x7f) //127 ピカチュウ
#define KABI32 (0x80) //128 テスト三頭身
#define POKE2 (0x81) //129 ポケモン２
#define POKE3 (0x82) //130 ポケモン３
#define POKE4 (0x83) //131 ポケモン４
#define POKE5 (0x84) //132 ポケモン５
#define POKE6 (0x85) //133 ポケモン６
#define POKE7 (0x86) //134 ポケモン７
#define POKE8 (0x87) //135 ポケモン８
#define POKE9 (0x88) //136 ポケモン９
#define POKE10 (0x89) //137 ポケモン１０
#define POKE11 (0x8a) //138 ポケモン１１
#define POKE12 (0x8b) //139 ポケモン１２
#define POKE13 (0x8c) //140 ポケモン１３
#define POKE14 (0x8d) //141 ポケモン１４
#define POKE15 (0x8e) //142 ポケモン１５
#define POKE16 (0x8f) //143 ポケモン１６
#define POKE17 (0x90) //144 ポケモン１７
#define POKE18 (0x91) //145 ポケモン１８
#define POKE19 (0x92) //146 ポケモン１９
#define POKE20 (0x93) //147 ポケモン２０
#define ACCORDION (0x94) //148 アコーディオン奏者
#define BASKETM (0x95) //149 バスケ選手
#define SUPPORT (0x96) //150 サポート
#define NONE (0x97) //151 透明なもの
#define GETHERO (0x98) //152 主人公男ゲット
#define GETHEROINE (0x99) //153 主人公女ゲット
#define REPORTHERO (0x9a) //154 主人公男レポート
#define REPORTHEROINE (0x9b) //155 主人公女レポート
#define PCHERO (0x9c) //156 主人公男差し出し
#define PCHEROINE (0x9d) //157 主人公女差し出し
#define BONE (0x9e) //158 骨の標本
#define BLACKSTONE (0x9f) //159 石（黒）
#define WHITESTONE (0xa0) //160 石（白）
#define BLACKMONOLITH (0xa1) //161 モノリス（黒）
#define WHITEMONOLITH (0xa2) //162 モノリス（白）
#define MAMA (0xa3) //163 ママ
#define OBJCODEMAX (0xa4) //164 最大

#endif