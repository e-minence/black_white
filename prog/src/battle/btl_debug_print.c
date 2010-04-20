//=============================================================================================
/**
 * @file  btl_debug_print.c
 * @brief ポケモンWB バトルシステム デバッグPrint機能
 * @author  taya
 *
 * @date  2010.01.07  作成
 */
//=============================================================================================

#include "btl_util.h"
#include "btl_server_cmd.h"
#include "btl_debug_print.h"


/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BtlPrintType GPrintType;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void print_type( void );
static void print_file_info( const char* filename, int line );



//=============================================================================================
/**
 * プリント時のヘッダタイプ通知を受け取り
 *
 * @param   type
 */
//=============================================================================================
void BTL_DEBUGPRINT_SetType( BtlPrintType type )
{
  GPrintType = type;
}

void BTL_DEBUGPRINT_PrintDump( const char* caption, const void* data, u32 size )
{
  OS_TPrintf( "[DUMP] %s (%dbytes)\n", caption, size );

  if( size > 8 ){ size = 8; }

  {
    const u8* p = data;
    u32 i;
    for(i=0; i<size; i++)
    {
      OS_TPrintf("%02x ", p[i]);
    }
    OS_TPrintf("\n");
  }
}

const char* BTL_DEBUGPRINT_GetFormatStr( BtlDebugStrID strID )
{
  switch( strID ){
  case DBGSTR_SETUP_DONE:                    return "セットアップ完了\n";
  case DBGSTR_DEBUGFLAG_BIT:                 return "デバッグフラグbit=%04x\n";
  case DBGSTR_ADAPTER_SEND_START:            return " Adapter-%d, コマンド %d を送信開始します\n";
  case DBGSTR_ADAPTER_RECV_DONE:             return " [Adapter] Received from All Clients\n";
  case DBGSTR_ADAPTER_RECVED_A_CLIENT:       return " [Adapter] Received from AdapterID=%d\n";
  case DBGSTR_ADAPTER_Create:                return "アダプタ(%d）作成：通信対応フラグ=%d\n";

  case DBGSTR_MAIN_PokeConGetByPos:          return "存在しない位置(pos=%d, clientID=%d, idx=%d)のポケモンデータを参照した\n";
  case DBGSTR_MAIN_PartyDataNotifyComplete:  return "パーティデータ相互受信できました。\n";
  case DBGSTR_MAIN_AIPartyDataSendComplete:  return "AIパーティデータ受信完了\n";
  case DBGSTR_MAIN_SwapPartyMember:          return "パーティメンバー入れ替え %d <-> %d\n";
  case DBGSTR_MAIN_RecvedMultiAITrainer:     return "ClientID=%d, trID=%d set\n";
  case DBGSTR_MAIN_SendAIPartyStart:         return "AIクライアント数=%d, パーティデータ送受信開始\n";
  case DBGSTR_MAIN_SendAIPartyParam:         return "AI Client=%d パーティ送信完了（ポケ数=%d\n";
  case DBGSTR_MAIN_GetFrontPokeData:         return "戦闘位置[%d] のポケモン... ";
  case DBGSTR_MAIN_GetFrontPokeDataResult:   return "    クライアント[%d]の %d 番目のポケを返す\n";
  case DBGSTR_MAIN_CommError:                return "通信エラーによる終了\n";
  case DBGSTR_MAIN_CheckHPByLvup:            return "PPLVUP Main line:%d, HP=%d\n";
  case DBGSTR_MAIN_PerappVoiceAdded:         return "ClientID=%d のペラップデータ構築\n";
  case DBGSTR_MAIN_PerappVoiceComplete:      return "ペラップボイス相互受信完了\n";

  case DBGSTR_CLIENT_RETURN_CMD_START:       return "ID[%d], 返信開始へ\n";
  case DBGSTR_CLIENT_RETURN_CMD_DONE:        return "ID[%d], %d byte 返信しました\n";
  case DBGSTR_CLIENT_SelectActionSkip:       return "Act選択(%d体目）スキップ\n";
  case DBGSTR_CLIENT_SelActPokeDead:         return "死んでてスキップなのでCheckedCntそのまま\n";
  case DBGSTR_CLIENT_SelectActionStart:      return "Act選択(%d体目=ID:%d）開始します  checked %d poke\n";
  case DBGSTR_CLIENT_SelectActionDone:       return "カバー位置数(%d)終了、アクション送信へ\n";
  case DBGSTR_CLIENT_SelectAction_Pokemon:   return "「ポケモン」を選んだ->選択画面へ\n";
  case DBGSTR_CLIENT_SelectAction_Fight:     return "「たたかう」を選んだ->選択画面へ\n";
  case DBGSTR_CLIENT_SelectChangePoke:       return " ポケモン選んだ ... idx=%d\n";
  case DBGSTR_CLIENT_SelectChangePokeCancel: return " ポケモン選ばなかった\n";

  case DBGSTR_CLIENT_ReturnSeqDone:       return "返信シーケンス終了\n";
  case DBGSTR_CLIENT_WazaLockInfo:        return "ワザロック：Client[%d] 前回使ったワザは %d, targetPos=%d\n";
  case DBGSTR_CLIENT_NoMorePuttablePoke:  return "myID=%d もう戦えるポケモンいない\n";
  case DBGSTR_CLIENT_NotDeadMember:       return "myID=%d 誰も死んでないから選ぶ必要なし\n";
  case DBGSTR_CLIENT_ChangePokeCmdInfo:   return "myID=%d %d体選択する必要あり（先頭位置=%d） mode=%d\n";
  case DBGSTR_CLIENT_NumChangePokeBegin:  return " 全Client, 選択すべきポケモン数=%d\n　位置=";
  case DBGSTR_CLIENT_NumChangePokeResult: return " 自分[%d]が選択すべきポケモン数=%d\n";
  case DBGSTR_CLIENT_PokeSelCnt:          return "入れ替え%d体選んだ\n";
  case DBGSTR_CLIENT_PokeChangeIdx:       return "ポケモン入れ替え %d体目 <-> %d体目\n";
  case DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk:     return "逃げ交換禁止チェック「かげふみ」by pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable:  return "  「かげふみ」有効！\n";
  case DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk:    return "逃げ交換禁止チェック「ありじごく」by pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable: return "  「ありじごく」有効！\n";
  case DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk:      return "逃げ交換禁止チェック「じりょく」by pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable:   return "  「じりょく」有効！\n";
  case DBGSTR_CLIENT_UpdateWazaProcResult:          return "ワザプロセス情報更新 : PokeID=%d, OrgWaza=%d, ActWaza=%d, ActTargetPos=%d, ActEnable=%d\n";
  case DBGSTR_CLIENT_CmdLimitTimeEnable:   return "コマンド選択制限時間が有効 -> %d sec\n";
  case DBGSTR_CLIENT_CmdLimitTimeOver:     return "コマンド選択タイムアップ ... 強制終了フラグON\n";
  case DBGSTR_CLIENT_ForcePokeChange:      return "強制入れ替え %d/%d PosIdx=%d, MemberIdx=%d\n";
  case DBGSTR_CLIENT_UpdateEnemyBaseHP:    return "相手ポケ基準HP値を更新 ->%d  (Poke=%d)\n";
  case DBGSTR_CLIENT_ForceQuitByTimeLimit: return "時間制限による強制終了(ClientID=%d)\n";
  case DBGSTR_CLIENT_StartCmd:             return "Client(%d), AdaptCmd(%d) 実行開始\n";
  case DBGSTR_CLIENT_RecvedQuitCmd:        return "Client(%d), バトル終了コマンド受信\n";
  case DBGSTR_CLIENT_ReplyToQuitCmd:       return "Client(%d), バトル終了コマンドに対し返信完了\n";
  case DBGSTR_CLIENT_StartRotAct:          return "ローテーション動作開始 clientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_EndRotAct:            return "ローテーション動作終了 clientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_ReadRecAct:           return "録画データ読み込み clientID=%d, Action count=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Fight:     return "   action=Fight, waza=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Change:    return "   action=Change, nextPokeIdx=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Move:      return "   action=Move!\n";
  case DBGSTR_CLIENT_UnknownServerCmd:     return "用意されていないコマンドNo[%d]！\n";
  case DBGSTR_CLIENT_RecPlayerBlackOut:    return "録画再生コントローラ：ブラックアウト\n";
  case DBGSTR_CLIENT_RecPlay_ChapterSkipped: return "client(%d), 指定チャプタ[%d]に到達した..\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokStart:    return "受けとくせい所持による交換チェック ... wazaType=%d\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokFind:     return " %d 番目がtype=%dを受けられるポケモンだ\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokDecide:   return "受けるための交替確定\n";
  case DBGSTR_CLIENT_CHGAI_PreSortList:    return " 入れ替え対象ソート前 ";
  case DBGSTR_CLIENT_CHGAI_AfterSortList:  return " 入れ替え対象ソート後 ";
  case DBGSTR_CLIENT_CHGAI_SortListVal:    return "Idx(%d)=Pow(%d), ";
  case DBGSTR_CLIENT_CHGAI_HOROBI:         return "入れ替えたい：ほろびのうた\n";
  case DBGSTR_CLIENT_CHGAI_FusigiNaMamori: return "入れ替えたい：ふしぎなまもり\n";
  case DBGSTR_CLIENT_CHGAI_NoEffWaza:      return "入れ替えたい：相性無効\n";
  case DBGSTR_CLIENT_CHGAI_Kodawari:       return "入れ替えたい：こだわり系\n";
  case DBGSTR_CLIENT_CHGAI_UkeTok:         return "入れ替えたい：受けとくせい\n";
  case DBGSTR_CLIENT_CHGAI_SizenKaifuku:   return "入れ替えたい：しぜんかいふく\n";
  case DBGSTR_CLIENT_CHGAI_WazaEff:        return "入れ替えたい：ワザ相性による\n";
  case DBGSTR_CLIENT_ROT_R:                return "ローテ右回転  対象ポケ=%d\n";
  case DBGSTR_CLIENT_ROT_L:                return "ローテ左回転  対象ポケ=%d\n";
  case DBGSTR_CLIENT_ROT_Determine:        return "ローテ対象ポケ、ワザ有効 dir=%d, Idx=%d, pokeID=%d\n";
  case DBGSTR_CLIENT_AI_PutPokeStart:      return "myID=%d 戦闘ポケが死んで %d体選択\n";
  case DBGSTR_CLIENT_AI_PutPokeDecide:     return "  -> %d番目を新たに出す\n";
  case DBGSTR_CLIENT_CoverRotateDecide:    return "空きを埋めるローテーション確定 ClientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_HPCheckByLvup:        return "PPLVUP Client line:%d, HP=%d\n";

  case DBGSTR_BPP_NemuriWakeCheck:        return "ポケ[%d]のねむりターン最大値=%d, 経過ターン=%d\n";

  case DBGSTR_VCORE_PokeListStart:      return "交換リスト画面  モード=%d   すでに%d体選択されている\n";
  case DBGSTR_VCORE_SelPokeEnd:         return "ポケ選択おわった\n";
  case DBGSTR_VCORE_SelPokeEnd_Sel:     return "  [%d]番目 選択された -> %d体 格納\n";
  case DBGSTR_VCORE_SelPokeEnd_Unsel:   return "  [%d]番目 選択されていない\n";

  case DBGSTR_NET_Startup:              return "自分のネットID=%d, 接続メンバー数=%d\n";
  case DBGSTR_NET_DecideClientID:       return "NetID:%d -> clientID=%d\n";
  case DBGSTR_NET_SendSucceed:          return "  送信成功\n";
  case DBGSTR_NET_RecvedServerParam:    return "netID=%d, サーバパラメータ受信しました。\n";
  case DBGSTR_NET_RecvedPartyData:      return "netID=%d, clientID=%d のパーティデータ受信完了, pData=%p, buf=%p\n";
  case DBGSTR_NET_PartyDataComplete:    return "パーティデータ相互受信完了  member=%d\n";
  case DBGSTR_NET_PerappVoiceComplete:  return "ペラップボイス相互受信完了  member=%d\n";
  case DBGSTR_NET_SendCmdDone:          return "マシン(%d) に %d bytes 送信完了\n";
  case DBGSTR_NET_RecvedAllClientsData: return "全クライアントからのデータ返ってきた\n";
  case DBGSTR_NET_ReturnToServerTrying: return "Try Return to Server %d byte ...";
  case DBGSTR_NET_RecvedClientData:     return "[BTLNET] recv from netID[%d], size=%d\n";
  case DBGSTR_NET_RecvedServerVersion:  return "サーババージョン受信 ... netID=%d, version=%d\n";
  case DBGSTR_NET_ServerDetermine:      return "サーバは netID=%d のマシンに決定\n";
  case DBGSTR_NET_SendAITrainerData:    return "AIトレーナーデータ送信完了, trID=%d\n";
  case DBGSTR_NET_RecvAITrainerData:    return "AIトレーナーデータ受信完了, trID=%d\n";
  case DBGSTR_NET_AIPartyInfo:          return "Client(%d)=AIパーティと判断する (pokeCnt=%d)\n  monsno=";
  case DBGSTR_NET_CreateAIPartyRecvBuffer: return "AIパーティデータ受信バッファを生成\n";
  case DBGSTR_NET_RecvedAIPartyData:       return "AIパーティデータ受信 clientID=%d\n";
  case DBGSTR_NET_PrappVoiceGetBufDisable:  return "ペラップボイス受信バッファ構築（無効データ）netID=%d\n";
  case DBGSTR_NET_PrappVoiceGetBufEnable:   return "ペラップボイス受信バッファ構築（有効データ）netID=%d\n";
  case DBGSTR_NET_PerappVoiceRecvedEnable:  return "ペラップボイス受信完了（有効データ）netID=%d, size=%d\n";
  case DBGSTR_NET_PerappVoiceRecvedDisable: return "ペラップボイス受信完了（無効データ）netID=%d, empty=%d\n";
  case DBGSTR_NET_PerappVoiceCheckRaw:      return "ペラップボイス受信チェック: ClientID:%d = netID:%d\n";

  case DBGSTR_CALCDMG_WazaParam:        return "ワザ情報：ID=%d, Type=%d\n";
  case DBGSTR_CALCDMG_BaseDamage:       return "基礎ダメージ値 (%d)\n";
  case DBGSTR_CALCDMG_RangeHosei:       return "対象数によるダメージ補正:%d\n";
  case DBGSTR_CALCDMG_WeatherHosei:     return "天候による補正が発生, 補正率=%08x, dmg=%d->%d\n";
  case DBGSTR_CALCDMG_RAW_DAMAGE:       return "威力:%d, Lv:%d, こうげき:%d, ぼうぎょ:%d,  ... 素ダメ:%d\n";
  case DBGSTR_CALCDMG_Critical:         return "クリティカルだから素ダメ->%d\n";
  case DBGSTR_CALCDMG_RandomHosei:      return "ランダム補正:%d%%  -> 素ダメ=%d\n";
  case DBGSTR_CALCDMG_TypeMatchHosei:   return "タイプ一致補正:%d%%  -> 素ダメ=%d\n";
  case DBGSTR_CALCDMG_TypeAffInfo:      return "タイプ相性:%02d -> ダメージ値：%d\n";
  case DBGSTR_CALCDMG_DamageResult:     return "ダメ受けポケモン=%d, ratio=%08x, Damage=%d -> %d\n";
  case DBGSTR_CALCDMG_DamageMarume:     return "ダメ受けポケモンのＨＰ値にまるめ->%d\n";
  case DBGSTR_STR_StdStrID:             return " STD:strID=%d\n";
  case DBGSTR_STR_SetStrID:             return " SET:strID=%d\n";
  case DBGSTR_STR_StdStrInfo:           return "STD STR ID=%d, argCnt=%d\n";
  case DBGSTR_STR_Arg:                  return "  arg(%d)=%d\n";
  case DBGSTR_STR_SetPoke1:             return "ms set_poke1 strID=%d, args[0]=%d, args[1]=%d\n";
  case DBGSTR_STR_SetPoke2:             return "ms set_poke2 strID=%d, args[0]=%d, args[1]=%d\n";
  case DBGSTR_PRINT_BufIdx:             return "bufIdx=%d ....\n";
  case DBGSTR_STR_TagSetNumber:         return "[TAG] SetNumber : keta=%d, value=%d\n";
  case DBGSTR_STR_SetPokeNickname:      return "setPokeNickName ID=%d ....\n";
  case DBGSTR_STR_SetTagWazaName:       return "[TAG] Set WazaName ... waza=%d\n";
  case DBGSTR_STR_SetItemName:          return "set Item Name argIdx=%d, ID=%d ....\n";
  case DBGSTR_SVFL_ActOrderStart:       return "ポケ[%d =monsno:%d]のアクション実行...\n";
  case DBGSTR_SVFL_ActOrder_Fight:      return "【たたかう】ポケ[%d]のワザ[%d]を、位置[%d]の相手に\n";
  case DBGSTR_SVFL_ActOrder_Item:       return "【どうぐ】を処理。アイテム%dを、%d番の相手に。\n";
  case DBGSTR_SVFL_ActOrder_Change:     return "【ポケモン】を処理。位置%d <- ポケ%d \n";
  case DBGSTR_SVFL_ActOrder_Escape:     return "【にげる】を処理。\n";
  case DBGSTR_SVFL_ActOrder_Dead:       return "【ひんし】なので何もせず\n";
  case DBGSTR_SVFL_ActOrder_Rotation:   return "【ローテーション】を処理  ClientID=%d, 回転方向=%d\n";
  case DBGSTR_SVFL_CorrectTarget_Info:  return "攻撃ポケ[%d]（位置=%d） -> 狙ったポケ[%d]（位置=%d）死んでいるので補正する\n";
  case DBGSTR_SVFL_CorrectHitFarOn:     return "ワザ[%d] 遠隔ON の補正対象決定\n";
  case DBGSTR_SVFL_CorrectHitFarOff:    return "ワザ[%d] 遠隔OFF の補正対象決定\n";
  case DBGSTR_SVFL_CorrectTargetNum:    return "補正対象数  %d / %d \n";
  case DBGSTR_SVFL_CorrectTargetDiff:   return "対象候補の距離 Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectTargetHP:     return "対象候補の残HP Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectResult:       return "補正後の対象ポケモンは 位置=%d, ID=%d\n";
  case DBGSTR_SVFL_AtkPower:            return "攻撃力=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_DefGuard:            return "防御力=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_WazaPower:           return "ワザID=%d : 威力=%d  (ratio=%08x)\n";
  case DBGSTR_SVFL_UncategoryWazaInfo:  return "未分類ワザ 攻撃PokeID=%d, 対象ポケ数=%d/%d\n";
  case DBGSTR_SVFL_QueWritePtr:         return "[* SVF *]         Que WritePtr=%d\n";
  case DBGSTR_SVFL_ClientPokeStillAlive:return "クライアント_%d (SIDE:%d) のポケはまだ%d体生きている\n";
  case DBGSTR_SVFL_ClientPokeDeadAll:   return "クライアント_%d (SIDE:%d) のポケは全滅した\n";
  case DBGSTR_SVFL_PosEffAdd:           return "位置エフェクトハンドラ生成: pos=%d, effect=%d, pokeID=%d\n";
  case DBGSTR_SVFL_PosEffDupFail:       return "位置(%d)にPOSEffect[%d]ハンドラを追加失敗（使用ポケ=%d）\n";
  case DBGSTR_SVFL_HitCheckInfo1:       return "攻撃ポケ[%d]  命中Rank=%d  ワザ的中率=%d\n";
  case DBGSTR_SVFL_HitCheckInfo2:       return "防御ポケ[%d]  回避Rank=%d\n";
  case DBGSTR_SVFL_HitCheckInfo3:       return "命中ランク=%d, 命中率=%d, 命中率補正=%08x\n";
  case DBGSTR_SVFL_HitCheckInfo4:       return "最終命中率 = %d\n";
  case DBGSTR_SVFL_StartAfterPokeChange:return "ターン途中ポケモン入れ替え後のサーバーコマンド生成\n";
  case DBGSTR_SVFL_PokeChangeRootInfo:  return "クライアント(%d)のポケモン(位置%d) を、%d番目のポケといれかえる\n";
  case DBGSTR_SVFL_UseItemCall:         return "シューター「アイテムコール」使用  対象Poke=%d\n";
  case DBGSTR_SVFL_ChangePokeReqInfo:   return "クライアント[%d]   空いている位置の数=%d, 位置ID= ";
  case DBGSTR_SVFL_TurnStart_Result:    return "【ターン開始コマンド】処理アクション %d / %d\n";
  case DBGSTR_SVFL_ActOrderMainStart:   return "コマンド生成コア 処理アクション開始Index=%d\n";
  case DBGSTR_SVFL_ActOrderMainDropOut: return "Result=%dによる中途サーバ返信\n";
  case DBGSTR_SVFL_ExpCalc_Base:        return "基本となる経験値=%d\n";
  case DBGSTR_SVFL_ExpCalc_MetInfo:     return "死亡ポケ[%d]が対面した相手ポケの数=%d, その内、生きてる数=%d\n";
  case DBGSTR_SVFL_ExpCalc_DivideInfo:  return "メンバーIdx[%d]のポケに経験値%dを分配\n";
  case DBGSTR_SVFL_ExpCalc_Result:      return "メンバーIdx[%d]のポケに対し、最終経験値=%d\n";
  case DBGSTR_SVFL_ExpAdjustCalc:       return "自分Lv=%d, 敵Lv=%d, 倍率=%08x, 基本経験値=%d -> 補正後経験値=%d\n";
  case DBGSTR_SVFL_RecDataSendComped:   return "操作記録データの送信完了\n";
  case DBGSTR_SVFL_SendServerCmd:       return "サーバコマンド送信します ... result=%d\n";
  case DBGSTR_SVFL_AllClientCmdPlayComplete:  return "全クライアントのコマンド再生終了...result=%d\n";
  case DBGSTR_SVFL_GotoQuit:            return "バトル終了へ\n";
  case DBGSTR_SVFL_ReqWazaCallActOrder: return "他ワザ呼び出し [%d] --> [%d], target=%d\n";
  case DBGSTR_SVFL_WazaExeFail_1:       return "ワザ出し失敗（１）  ポケID=%d, 失敗コード=%d\n";
  case DBGSTR_SVFL_WazaExeFail_2:       return "ワザ出し失敗（２）  ポケID=%d, 失敗コード=%d\n";
  case DBGSTR_SVFL_HandEx_AddSick:      return "状態異常 poke=%d, code=%d, AlmostFlag=%d\n";
  case DBGSTR_SVFL_CombiWazaCheck:      return "ワザ[%d]は合体ワザのひとつです\n";
  case DBGSTR_SVFL_CombiWazaFound:      return "  自分より後に、同じ位置(%d）にワザ(%d)を打つ味方（%d）が見つかった\n";
  case DBGSTR_SVFL_CombiDecide:         return "  ポケ(%d）が先発。ポケ(%d)と合体ワザ予約！\n";
  case DBGSTR_SVFL_StartAfterPokeIn:    return "空き位置にポケモン投入後のサーバーコマンド生成を開始する\n";
  case DBGSTR_SVFL_AfterPokeIn_Alive:   return "生きているポケモンとの入れ替えが発生 Cleint=%d, Pos %d <-> %d\n";
  case DBGSTR_SVFL_AfterPokeIn_Dead:    return "ひんしポケモンの空きを埋める入れ替えが発生 Pos %d <-> %d\n";
  case DBGSTR_SVFL_PokeDead:            return "ポケ(%d) しにます\n";
  case DBGSTR_SVFL_AddSickFailCode:     return "状態異常失敗コード=%d\n";
  case DBGSTR_SVFL_AddSickFixed:        return "状態異常決定：コード=%d\n";
  case DBGSTR_SVFL_WazaExeStart:        return "ポケ[%d], waza=%dのワザ出し処理開始\n";
  case DBGSTR_SVFL_ActionSortInfo:      return "アクションソート:Client(%d), actionCnt=%d\n";
  case DBGSTR_SVFL_ActionSortRotation:  return ".. rotate action nextPokeIdx = %d\n";
  case DBGSTR_SVFL_TrainerItemTarget:   return "トレーナーアイテム使用：tgtIdx=%d, tgtPokeID=%d\n";
  case DBGSTR_SVFL_DoubleTargetIntr:    return "ダブル敵１体ターゲット…割り込みポケ=%d\n";
  case DBGSTR_SVFL_DoubleTargetRegister:return "ダブル敵１体ターゲット…確定ポケ=%d\n";
  case DBGSTR_SVFL_DeadAlready:         return "ポケ(%d)既に死んでるので死亡コマンドを打ちません\n";
  case DBGSTR_SVFL_DeadDiffLevelCheck:  return "ポケ死亡、自分lv=%d, 敵Lv=%d\n";

  case DBGSTR_POSPOKE_Out:              return " poke[%d] out from pos[%d]\n";
  case DBGSTR_POSPOKE_In:               return " poke[%d] in to pos[%d]\n";

  case DBGSTR_SERVER_FlowResult:              return "サーバー処理結果=%d\n";
  case DBGSTR_SERVER_SendShooterChargeCmd:    return "シューターチャージコマンド発行\n";
  case DBGSTR_SERVER_ShooterChargeCmdDoneAll: return "全クライアントでシューターチャージコマンド処理終了\n";
  case DBGSTR_SERVER_SendActionSelectCmd:     return "アクション選択コマンド発行\n";
  case DBGSTR_SERVER_ActionSelectDoneAll:     return "アクション受け付け完了\n";
  case DBGSTR_SV_ChangePokeOnTheTurn:         return "ターン途中のポケ入れ替え発生\n";
  case DBGSTR_SV_PokeInReqForEmptyPos:        return "空き位置への新ポケ投入リクエスト受け付け\n";
  case DBGSTR_SV_StartChangePokeInfo:         return "入れ替えポケモン選択へ  交替されるポケ数=%d\n";
  case DBGSTR_SV_SendQuitACmad:               return "全クライアントにバトル終了コマンド(%d)送信開始\n";
  case DBGSTR_SV_ReplyQuitACmad:              return "全クライアントからバトル終了コマンド受信応答あり\n";
  case DBGSTR_SV_RestoreAction:               return "整合性チェック用サーバ：ClientID=%d, numAction:%d 復元\n";
  case DBGSTR_SV_CmdCheckOK:                  return "整合性チェック用サーバ：チェックOK\n";
  case DBGSTR_SV_CmdCheckNG:                  return "整合性チェック用サーバ：チェックNG!! cmdSize= %d , %d\n";
  case DBGSTR_SV_SendActRecordFailed:         return "アクション記録送信に失敗した!!\n";
  case DBGSTR_SV_SendActRecord:               return "アクション記録データを送信する (%dbytes)\n";


  case DBGSTR_SC_PutCmd:                return "[QUE]PutCmd=%d, Format=%02x, argCnt=%d, args=";
  case DBGSTR_SC_ReservedPos:           return "[QUE]reserved pos=%d, wp=%d\n";
  case DBGSTR_SC_WriteReservedPos:      return "[QUE]Write Reserved Pos ... pos=%d, cmd=%d";
  case DBGSTR_SC_PutMsgParam:           return "[QUE] PUT  MSG SC=%d, StrID=%d";
  case DBGSTR_SC_ReadMsgParam:          return "[QUE] READ MSG SC=%d, StrID=%d";
  case DBGSTR_SC_PutMsg_SE:             return "  SE_ID=%d\n";
  case DBGSTR_SC_ArgsEqual:             return " args = ";
  case DBGSTR_SC_ReadCmd:               return "Read cmd=%d\n";
  case DBGSTR_SC_ReserveCmd:            return " -> reserved cmd=%d\n";


  case DBGSTR_EVENT_AddFactorInfo:      return "FactorType=%x, subPri=%x, TotalPriority=%x\n";
  case DBGSTR_EV_AddFactor:             return "[ADD] Factor=%p Depend=%d Type=%d, Pri=%06x [ADD]\n";
  case DBGSTR_EV_DelFactor:             return "[DEL] DEL Factor=%p Depend=%d, Type=%d [DEL]\n";
  case DBGSTR_EV_LinkInfoHeader:        return "***** [[EV Chain]] *****\n";
  case DBGSTR_EV_LinkInfoFooder:        return "************************\n\n";
  case DBGSTR_EV_LinkPtr:               return "%p->";
  case DBGSTR_EV_LinkEmpty:             return " empty...\n";
  case DBGSTR_EVAR_Push:                return "PUSH [%5d] SP=%d\n";
  case DBGSTR_EVAR_Pop:                 return "Pop! [%5d] SP=%d\n";
  case DBGSTR_DEADREC_Add:              return " 死亡記録 ターン%d件目 ... pokeID=%d\n";
  case DBGSTR_POKESET_RemoveDeadPoke:   return "ポケ(%d) 死んだのでセットから除外\n";

  case DBGSTR_SIDE_AddFirst:            return "サイドエフェクト初回登録 side=%d, eff=%d, pCnt=%p, cnt=%d\n";
  case DBGSTR_SIDE_NoMoreAdd:           return "サイドエフェクトこれ以上登録できない side=%d, eff=%d\n";

  case DBGSTR_SCU_RelivePokeAct:        return "Pos[%d]=Vpos[%d] ポケ(%d)が生き返り\n";
  case DBGSTR_Item_PinchReactItem:      return "PokeID=%d, 最大HP=%d, 現HP=%d, n=%d ... ";
  case DBGSTR_Item_PinchReactOn:        return "反応あり";
  case DBGSTR_HANDWAZA_CombiWazaExe:    return "ポケ(%d)がポケ(%d）のワザ(%d)に続けて合体ワザ発動->効果=%d\n";

  case DBGSTR_REC_ReadActStart:         return "rec seek start RP= %d\n";
  case DBGSTR_REC_ReadActSkip:          return "rec seek RotateData skip %d byte\n";
  case DBGSTR_REC_SeekClient:           return "rec seek numClient=%d\n";
  case DBGSTR_REC_ReadActParam:         return "rec ReadPtr=%d, act=%d, waza=%d\n";


  case DBGSTR_csv:  return "%d,";
  case DBGSTR_done: return "done!";
  case DBGSTR_LF: return "\n";

  }
  return NULL;
}

const char* BTL_DEBUGPRINT_GetServerCmdName( int cmd )
{
  switch( cmd ){
  case SC_OP_HP_MINUS:  return "SC_OP_HP_MINUS";           ///< HPマイナス  [ClientID, マイナス量]
  case SC_OP_HP_PLUS:  return "SC_OP_HP_PLUS";            ///< HPプラス    [ClientID, プラス量]
  case SC_OP_HP_ZERO:  return "SC_OP_HP_ZERO";            ///< HP0 にする [ pokeID ]
  case SC_OP_PP_MINUS:  return "SC_OP_PP_MINUS";           ///< PPマイナス  [ClientID, マイナス量]
  case SC_OP_PP_PLUS:  return "SC_OP_PP_PLUS";            ///< PPプラス    [ClientID, プラス量]
  case SC_OP_RANK_UP:  return "SC_OP_RANK_UP";            ///< ステータスランクアップ  [ClientID, StatusType, プラス量]
  case SC_OP_RANK_DOWN:  return "SC_OP_RANK_DOWN";          ///< ステータスランクダウン  [ClientID, StatusType, マイナス量]
  case SC_OP_RANK_SET7:  return "SC_OP_RANK_SET7";          ///< ステータスランク７種セット[ pokeID, atk, def, sp_atk, sp_def, agi ]
  case SC_OP_RANK_RECOVER:  return "SC_OP_RANK_RECOVER";       ///< ステータスランク（７種）下がっているもののみフラットに
  case SC_OP_RANK_RESET:  return "SC_OP_RANK_RESET";         ///< ステータスランク（７種）全てをフラットに
  case SC_OP_ADD_CRITICAL:  return "SC_OP_ADD_CRITICAL";       ///< クリティカルランク加算[ pokeID, (int)value ]
  case SC_OP_SICK_SET:  return "SC_OP_SICK_SET";           ///< 状態異常 [PokeID, Sick, contParam]
  case SC_OP_CURE_POKESICK:  return "SC_OP_CURE_POKESICK";      ///< ポケモン系状態異常を回復 [PokeID ]
  case SC_OP_CURE_WAZASICK:  return "SC_OP_CURE_WAZASICK";      ///< ワザ系状態異常を回復 [PokeID, SickID ]
  case SC_OP_MEMBER_IN:  return "SC_OP_MEMBER_IN";          ///< メンバー入場
  case SC_OP_SET_STATUS:  return "SC_OP_SET_STATUS";         ///< 能力値（攻撃、防御等）を書き換え
  case SC_OP_SET_WEIGHT:  return "SC_OP_SET_WEIGHT";         ///< 体重設定
  case SC_OP_WAZASICK_TURNCHECK:  return "SC_OP_WAZASICK_TURNCHECK"; ///<
  case SC_OP_CHANGE_POKETYPE:  return "SC_OP_CHANGE_POKETYPE";    ///< 【計算】ポケモンのタイプ変更（ pokeID, type ）
  case SC_OP_CONSUME_ITEM:  return "SC_OP_CONSUME_ITEM";       ///< 所有アイテム削除
  case SC_OP_UPDATE_USE_WAZA:  return "SC_OP_UPDATE_USE_WAZA";    ///< ワザプロセス終了情報
  case SC_OP_SET_CONTFLAG:  return "SC_OP_SET_CONTFLAG";       ///< 永続フラグセット
  case SC_OP_RESET_CONTFLAG:  return "SC_OP_RESET_CONTFLAG";     ///< 永続フラグリセット
  case SC_OP_SET_TURNFLAG:  return "SC_OP_SET_TURNFLAG";       ///< ターンフラグセット
  case SC_OP_RESET_TURNFLAG:  return "SC_OP_RESET_TURNFLAG";     ///< ターンフラグ強制リセット
  case SC_OP_CHANGE_TOKUSEI:  return "SC_OP_CHANGE_TOKUSEI";     ///< とくせい書き換え
  case SC_OP_SET_ITEM:  return "SC_OP_SET_ITEM";           ///< アイテム書き換え
  case SC_OP_UPDATE_WAZANUMBER:  return "SC_OP_UPDATE_WAZANUMBER";  ///< ワザ書き換え
  case SC_OP_HENSIN:  return "SC_OP_HENSIN";             ///< へんしん
  case SC_OP_OUTCLEAR:  return "SC_OP_OUTCLEAR";           ///< 退場時クリア
  case SC_OP_ADD_FLDEFF:  return "SC_OP_ADD_FLDEFF";         ///< フィールドエフェクト追加
  case SC_OP_REMOVE_FLDEFF:  return "SC_OP_REMOVE_FLDEFF";      ///< フィールドエフェクト削除
  case SC_OP_SET_POKE_COUNTER:  return "SC_OP_SET_POKE_COUNTER";   ///< ポケモンカウンタ値セット
  case SC_OP_BATONTOUCH:  return "SC_OP_BATONTOUCH";         ///< バトンタッチ
  case SC_OP_MIGAWARI_CREATE:  return "SC_OP_MIGAWARI_CREATE";    ///< みがわり作成
  case SC_OP_MIGAWARI_DELETE:  return "SC_OP_MIGAWARI_DELETE";    ///< みがわり削除
  case SC_OP_SHOOTER_CHARGE:  return "SC_OP_SHOOTER_CHARGE";     ///< シューターエネルギーチャージ
  case SC_OP_SET_FAKESRC:  return "SC_OP_SET_FAKESRC";        ///< イリュージョン用参照ポケモン変更
  case SC_OP_CLEAR_CONSUMED_ITEM:  return "SC_OP_CLEAR_CONSUMED_ITEM";///< アイテム消費情報のクリア
  case SC_ACT_WAZA_EFFECT:  return "SC_ACT_WAZA_EFFECT";
  case SC_ACT_WAZA_EFFECT_EX:  return "SC_ACT_WAZA_EFFECT_EX";    ///< 【アクション】ワザエフェクト拡張（溜めターンエフェクトなどに使用）
  case SC_ACT_TAMEWAZA_HIDE:  return "SC_ACT_TAMEWAZA_HIDE";     ///< 【アクション】そらをとぶなどで画面から消える・現れる設定
  case SC_ACT_WAZA_DMG:  return "SC_ACT_WAZA_DMG";          ///< 【アクション】[ AtClient, DefClient, wazaIdx, Affinity ]
  case SC_ACT_WAZA_DMG_PLURAL:  return "SC_ACT_WAZA_DMG_PLURAL";   ///< 【アクション】複数体同時ダメージ処理 [ pokeCnt, affAbout, ... ]
  case SC_ACT_WAZA_ICHIGEKI:  return "SC_ACT_WAZA_ICHIGEKI";     ///< 【アクション】一撃ワザ処理
  case SC_ACT_SICK_ICON:  return "SC_ACT_SICK_ICON";         ///< 【アクション】ゲージに状態異常アイコン表示
  case SC_ACT_CONF_DMG:  return "SC_ACT_CONF_DMG";          ///< 【アクション】こんらん自爆ダメージ [ pokeID ]
  case SC_ACT_RANKUP:  return "SC_ACT_RANKUP";            ///< 【ランクアップ効果】 ○○の×××があがった！[ ClientID, statusType, volume ]
  case SC_ACT_RANKDOWN:  return "SC_ACT_RANKDOWN";          ///< 【ランクダウン効果】 ○○の×××がさがった！[ ClientID, statusType, volume ]
  case SC_ACT_DEAD:  return "SC_ACT_DEAD";              ///< 【ポケモンひんし】
  case SC_ACT_RELIVE:  return "SC_ACT_RELIVE";            ///< 【ポケモン生き返り】
  case SC_ACT_MEMBER_OUT_MSG:  return "SC_ACT_MEMBER_OUT_MSG";    ///< 【ポケモン退場メッセージ】[ ClientID, pokeID ]
  case SC_ACT_MEMBER_OUT:  return "SC_ACT_MEMBER_OUT";        ///< 【ポケモン退場】[ ClientID, memberIdx ]
  case SC_ACT_MEMBER_IN:  return "SC_ACT_MEMBER_IN";         ///< 【ポケモンイン】[ ClientID, posIdx, memberIdx ]
  case SC_ACT_WEATHER_DMG:  return "SC_ACT_WEATHER_DMG";       ///< 天候による一斉ダメージ処理[ weather, pokeCnt ]
  case SC_ACT_WEATHER_START:  return "SC_ACT_WEATHER_START";     ///< 天候変化
  case SC_ACT_WEATHER_END:  return "SC_ACT_WEATHER_END";       ///< ターンチェックで天候終了
  case SC_ACT_SIMPLE_HP:  return "SC_ACT_SIMPLE_HP";         ///< シンプルなHPゲージ増減処理
  case SC_ACT_KINOMI:  return "SC_ACT_KINOMI";            ///< きのみを食べる
  case SC_ACT_KILL:  return "SC_ACT_KILL";              ///< 強制瀕死演出（みちづれ、一撃ワザなど）
  case SC_ACT_MOVE:  return "SC_ACT_MOVE";              ///< ムーブ
  case SC_ACT_EXP:  return "SC_ACT_EXP";               ///< 経験値取得
  case SC_ACT_BALL_THROW:  return "SC_ACT_BALL_THROW";        ///< ボール投げ
  case SC_ACT_ROTATION:  return "SC_ACT_ROTATION";          ///< ローテーション
  case SC_ACT_CHANGE_TOKUSEI:  return "SC_ACT_CHANGE_TOKUSEI";    ///< とくせい変更
  case SC_ACT_FAKE_DISABLE:  return "SC_ACT_FAKE_DISABLE";      ///< イリュージョン解除
  case SC_ACT_EFFECT_BYPOS:  return "SC_ACT_EFFECT_BYPOS";      ///< 指定位置にエフェクト発動
  case SC_ACT_EFFECT_BYVECTOR:  return "SC_ACT_EFFECT_BYVECTOR";   ///< 指定発動位置＆終点位置でエフェクト発動
  case SC_ACT_CHANGE_FORM:  return "SC_ACT_CHANGE_FORM";       ///< フォルムナンバーチェンジ
  case SC_ACT_RESET_MOVE:  return "SC_ACT_RESET_MOVE";        ///< リセットムーブカットイン
  case SC_TOKWIN_IN:  return "SC_TOKWIN_IN";             ///< とくせいウィンドウ表示イン [ClientID]
  case SC_TOKWIN_OUT:  return "SC_TOKWIN_OUT";            ///< とくせいウィンドウ表示アウト [ClientID]
  case SC_MSG_WAZA:  return "SC_MSG_WAZA";              ///< ワザメッセージ表示[ ClientID, wazaIdx ]
  case SC_MSG_STD:  return "SC_MSG_STD";               ///< メッセージ表示 [MsgID, numArgs, arg1, arg2, ... ]
  case SC_MSG_SET:  return "SC_MSG_SET";               ///< メッセージ表示 [MsgID, numArgs, arg1, arg2, ... ]
  case SC_MSG_STD_SE:  return "SC_MSG_STD_SE";            ///< メッセージ表示＆SE [MsgID, SENo, numArgs, arg1, arg2, ... ]
  }

  return NULL;
}


//=============================================================================================
/**
 * デバッグ用ヘッダPrint
 *
 * @param   fileName
 * @param   line
 */
//=============================================================================================
void BTL_DEBUGPRINT_PrintHeader( const char* fileName, u32 line )
{
  print_type();
  print_file_info( fileName, line );
}
static void print_type( void )
{
  switch( GPrintType ){
  case BTL_PRINTTYPE_SERVER:      OS_TPrintf("SV/"); break;
  case BTL_PRINTTYPE_CLIENT:      OS_TPrintf("CL/"); break;
  case BTL_PRINTTYPE_STANDALONE:  OS_TPrintf("SA/"); break;
  case BTL_PRINTTYPE_UNKNOWN:
  default:
    OS_TPrintf("UN/");
    break;
  }
}
static void print_file_info( const char* filename, int line )
{
  static const struct {
    char* longName;
    char* shortName;
  }names[] = {
    { "btl_main.c",           "MAI" },
    { "btl_server.c",         "SVR" },
    { "btl_client.c",         "CLI" },
    { "btl_adapter.c",        "ADP" },
    { "btl_string.c",         "STR" },
    { "btl_net.c",            "NET" },
    { "btl_calc.c",           "CAL" },
    { "btl_sick.c",           "SIC" },
    { "btl_event.c",          "EVE" },
    { "btl_pokeparam.c",      "BPP" },
    { "btl_pokeset.c",        "SET" },
    { "btl_pospoke_state.c",  "POS" },
    { "btl_server_cmd.c",     "CMD" },
    { "btl_server_flow.c",    "FLW" },
    { "btl_field.c",          "FLD" },
    { "btl_rec.c",            "REC" },
    { "btlv_core.c",          "VIW" },
    { "btlv_scu.c",           "scU" },
    { "btlv_scd.c",           "scD" },
    { NULL,                   "OTR" },
  };
  u32 i;

  for(i=0; names[i].longName!=NULL; ++i)
  {
    if( !GFL_STD_StrCmp(names[i].longName, filename) )
    {
      break;
    }
  }
  OS_TPrintf( "[%s-%4d]", names[i].shortName, line);
}
