
if (sigbits&(1<<child_play_port->mp_SigBit))
	while (msg=(struct play_msg *)GetMsg(child_play_port))

	if(event->type == SE_SET_VAR0)
	/***************************************/
		{
		prnt("SetVar0",event);

		x=Event->arg1;
		SendW(VAR0lo+((x & 0x00FF)<<8));
		SendW(VAR0hi+(x & 0xFF00));
		}

SE_SET_VAR1 
	/*******************************************************/
		x=msg->arg1;
		ReplyMsg(msg);
		SendW(VAR1lo+((x & 0x00FF)<<8));
		SendW(VAR1hi+(x & 0xFF00));
		}

SE_SET_VAR2 
	/*******************************************************/
		x=msg->arg1;
		ReplyMsg(msg);
		SendW(VAR2lo+((x & 0x00FF)<<8));
		SendW(VAR2hi+(x & 0xFF00));
		}

SE_SET_VAR3 
	/*******************************************************/
		x=msg->arg1;
		ReplyMsg(msg);
		SendW(VAR3lo+((x & 0x00FF)<<8));
		SendW(VAR3hi+(x & 0xFF00));
		}

SE_SENDCMD
	/*******************************************************/
		if(msg->arg1) SendW(msg->arg1);
		if(msg->arg2) SendW(msg->arg2);
		if(msg->arg3) SendW(msg->arg3);
		if(msg->arg4) SendW(msg->arg4);
		if(msg->arg5) SendW(msg->arg5);
		ReplyMsg(msg);
		}

	default 
		ReplyMsg(msg);
		}
	}
