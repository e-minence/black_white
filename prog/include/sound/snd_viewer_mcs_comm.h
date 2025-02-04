#pragma once

#ifdef DS_MCS_SRC
	#define DWORD u32
#endif

#define GFL_MCS_SNDVIEWER_ID	(0x13846590)

// commID
enum {
	// PC -> DS
	COMM_CONNECT = 1,
	COMM_DISCONNECT,
	COMM_PLAYERVOL_CHANGE,
	COMM_PLAYERPITCH_CHANGE,
  COMM_PLAYERTEMPO_CHANGE,
	COMM_PLAYERPAN_CHANGE,
	COMM_PLAYERREVERB_CHANGE,
	COMM_TRACKVOL_CHANGE,
	COMM_TRACKPITCH_CHANGE,
	COMM_TRACKMODD_CHANGE,
	COMM_TRACKMODS_CHANGE,
	COMM_TRACKPAN_CHANGE,

	// DS -> PC
	COMM_PANEL_RESET,
	COMM_SET_TRACKST,
};

// commID paramNum
enum {
	PNUM_COMM_CONNECT = 0,				        // 
	PNUM_COMM_DISCONNECT = 0,			        // 
	PNUM_COMM_PLAYERVOL_CHANGE = 1,				// val
	PNUM_COMM_PLAYERPITCH_CHANGE = 1,		  // val
  PNUM_COMM_PLAYERTEMPO_CHANGE = 1,			// val
	PNUM_COMM_PLAYERPAN_CHANGE = 1,				// val
	PNUM_COMM_PLAYERREVERB_CHANGE = 1,		// val
	PNUM_COMM_TRACKVOL_CHANGE = 2,				// val, trackID
	PNUM_COMM_TRACKPITCH_CHANGE = 2,			// val, trackID
	PNUM_COMM_TRACKMODD_CHANGE = 2,				// val, trackID
	PNUM_COMM_TRACKMODS_CHANGE = 2,				// val, trackID
	PNUM_COMM_TRACKPAN_CHANGE = 2,				// val, trackID

	PNUM_COMM_PANEL_RESET = 1,						// trackBit
	PNUM_COMM_SET_TRACKST = 1,						// trackBit
};

typedef struct {
	DWORD ID;
	DWORD command;
	DWORD	param;

}GFL_MCS_SNDVIEWER_HEADER;


