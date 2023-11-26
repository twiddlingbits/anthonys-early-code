/**
		LoadModule("Quit");
		WaitUntilListening(GetEventID("SE_CMD_SELECT_MODULE"), FindModule("quit"));
		SendMsg(FindModule("Quit"), GetEventID("SE_CMD_SELECT_MODULE"),FindModule("Quit"),0,0,0,0,0);
****/
