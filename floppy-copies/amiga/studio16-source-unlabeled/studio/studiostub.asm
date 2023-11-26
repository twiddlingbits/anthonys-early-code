;:ts=8

	xref	_studioBase
	xdef	_BroadcastEventParms
_BroadcastEventParms:
	movea.l	_studioBase,a6
	jmp	-30(a6)

	xdef	_SendMsg
_SendMsg:
	movea.l	_studioBase,a6
	jmp	-36(a6)

	xdef	_NotifyMeOnEvent
_NotifyMeOnEvent:
	movea.l	_studioBase,a6
	jmp	-42(a6)

	xdef	_NotifyMeOnCount
_NotifyMeOnCount:
	movea.l	_studioBase,a6
	jmp	-48(a6)

	xdef	_IgnoreEvent
_IgnoreEvent:
	movea.l	_studioBase,a6
	jmp	-54(a6)

	xdef	_IgnoreCount
_IgnoreCount:
	movea.l	_studioBase,a6
	jmp	-60(a6)

	xdef	_ClearNotifyList
_ClearNotifyList:
	movea.l	_studioBase,a6
	jmp	-66(a6)

	xdef	_DeleteSample
_DeleteSample:
	movea.l	_studioBase,a6
	jmp	-72(a6)

	xdef	_DeleteModule
_DeleteModule:
	movea.l	_studioBase,a6
	jmp	-78(a6)

	xdef	_DeleteAudioHandler
_DeleteAudioHandler:
	movea.l	_studioBase,a6
	jmp	-84(a6)

	xdef	_WriteUnLockSamp
_WriteUnLockSamp:
	movea.l	_studioBase,a6
	jmp	-90(a6)

	xdef	_ReadUnLockSamp
_ReadUnLockSamp:
	movea.l	_studioBase,a6
	jmp	-96(a6)

	xdef	_SetSampLength
_SetSampLength:
	movea.l	_studioBase,a6
	jmp	-102(a6)

	xdef	_FreeChan
_FreeChan:
	movea.l	_studioBase,a6
	jmp	-108(a6)

	xdef	_UpdateStandardModState
_UpdateStandardModState:
	movea.l	_studioBase,a6
	jmp	-114(a6)

	xdef	_RestoreWindowParms
_RestoreWindowParms:
	movea.l	_studioBase,a6
	jmp	-120(a6)

	xdef	_AddModule
_AddModule:
	movea.l	_studioBase,a6
	jmp	-126(a6)

	xdef	_GetSamplePtr
_GetSamplePtr:
	movea.l	_studioBase,a6
	jmp	-132(a6)

	xdef	_AllocChan
_AllocChan:
	movea.l	_studioBase,a6
	jmp	-138(a6)

	xdef	_AddAudioHandler
_AddAudioHandler:
	movea.l	_studioBase,a6
	jmp	-144(a6)

	xdef	_GetEventID
_GetEventID:
	movea.l	_studioBase,a6
	jmp	-150(a6)

	xdef	_GetSampleNum
_GetSampleNum:
	movea.l	_studioBase,a6
	jmp	-156(a6)

	xdef	_QuickWrite
_QuickWrite:
	movea.l	_studioBase,a6
	jmp	-162(a6)

	xdef	_QuickRead
_QuickRead:
	movea.l	_studioBase,a6
	jmp	-168(a6)

	xdef	_WriteLockSamp
_WriteLockSamp:
	movea.l	_studioBase,a6
	jmp	-174(a6)

	xdef	_ReadLockSamp
_ReadLockSamp:
	movea.l	_studioBase,a6
	jmp	-180(a6)

	xdef	_QuickCopy
_QuickCopy:
	movea.l	_studioBase,a6
	jmp	-186(a6)

	xdef	_CreateSample
_CreateSample:
	movea.l	_studioBase,a6
	jmp	-192(a6)

	xdef	_CloseSample
_CloseSample:
	movea.l	_studioBase,a6
	jmp	-198(a6)

	xdef	_StandardEnviroment
_StandardEnviroment:
	movea.l	_studioBase,a6
	jmp	-204(a6)

	xdef	_GetOneSample
_GetOneSample:
	movea.l	_studioBase,a6
	jmp	-210(a6)

	END
