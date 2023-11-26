;:ts=8

	xref	_StudioBase
	xdef	_ASStop
_ASStop:
	movea.l	_StudioBase,a6
	jmp	-30(a6)

	xdef	_ASTrigger
_ASTrigger:
	movea.l	_StudioBase,a6
	jmp	-36(a6)

	xdef	_AttachWorkingFiles
_AttachWorkingFiles:
	movea.l	_StudioBase,a6
	jmp	-42(a6)

	xdef	_BroadcastEventParms
_BroadcastEventParms:
	movea.l	_StudioBase,a6
	jmp	-48(a6)

	xdef	_BroadcastEventParmsLessOne
_BroadcastEventParmsLessOne:
	movea.l	_StudioBase,a6
	jmp	-54(a6)

	xdef	_ClearNotifyList
_ClearNotifyList:
	movea.l	_StudioBase,a6
	jmp	-60(a6)

	xdef	_DeleteSample
_DeleteSample:
	movea.l	_StudioBase,a6
	jmp	-66(a6)

	xdef	_DeleteModule
_DeleteModule:
	movea.l	_StudioBase,a6
	jmp	-72(a6)

	xdef	_DeleteModState
_DeleteModState:
	movea.l	_StudioBase,a6
	jmp	-78(a6)

	xdef	_DeleteAudioHandler
_DeleteAudioHandler:
	movea.l	_StudioBase,a6
	jmp	-84(a6)

	xdef	_DeleteFileFilter
_DeleteFileFilter:
	movea.l	_StudioBase,a6
	jmp	-90(a6)

	xdef	_DeleteWorkingFile
_DeleteWorkingFile:
	movea.l	_StudioBase,a6
	jmp	-96(a6)

	xdef	_FreeChan
_FreeChan:
	movea.l	_StudioBase,a6
	jmp	-102(a6)

	xdef	_IgnoreEvent
_IgnoreEvent:
	movea.l	_StudioBase,a6
	jmp	-108(a6)

	xdef	_IgnoreCount
_IgnoreCount:
	movea.l	_StudioBase,a6
	jmp	-114(a6)

	xdef	_InitSharedSemaphore
_InitSharedSemaphore:
	movea.l	_StudioBase,a6
	jmp	-120(a6)

	xdef	_LoadDefaults
_LoadDefaults:
	movea.l	_StudioBase,a6
	jmp	-126(a6)

	xdef	_NotifyMeOnEvent
_NotifyMeOnEvent:
	movea.l	_StudioBase,a6
	jmp	-132(a6)

	xdef	_NotifyMeOnCount
_NotifyMeOnCount:
	movea.l	_StudioBase,a6
	jmp	-138(a6)

	xdef	_ObtainExclusiveSemaphore
_ObtainExclusiveSemaphore:
	movea.l	_StudioBase,a6
	jmp	-144(a6)

	xdef	_ObtainSharedSemaphore
_ObtainSharedSemaphore:
	movea.l	_StudioBase,a6
	jmp	-150(a6)

	xdef	_ReleaseSharedSemaphore
_ReleaseSharedSemaphore:
	movea.l	_StudioBase,a6
	jmp	-156(a6)

	xdef	_ReleaseExclusiveSemaphore
_ReleaseExclusiveSemaphore:
	movea.l	_StudioBase,a6
	jmp	-162(a6)

	xdef	_RestoreWindowParms
_RestoreWindowParms:
	movea.l	_StudioBase,a6
	jmp	-168(a6)

	xdef	_SampleSetLength
_SampleSetLength:
	movea.l	_StudioBase,a6
	jmp	-174(a6)

	xdef	_SaveDefaults
_SaveDefaults:
	movea.l	_StudioBase,a6
	jmp	-180(a6)

	xdef	_SendMsg
_SendMsg:
	movea.l	_StudioBase,a6
	jmp	-186(a6)

	xdef	_SetOriginalDefaults
_SetOriginalDefaults:
	movea.l	_StudioBase,a6
	jmp	-192(a6)

	xdef	_UpdateStandardModState
_UpdateStandardModState:
	movea.l	_StudioBase,a6
	jmp	-198(a6)

	xdef	_WaitUntilListening
_WaitUntilListening:
	movea.l	_StudioBase,a6
	jmp	-204(a6)

	xdef	_AddModule
_AddModule:
	movea.l	_StudioBase,a6
	jmp	-210(a6)

	xdef	_FindModule
_FindModule:
	movea.l	_StudioBase,a6
	jmp	-216(a6)

	xdef	_AddModState
_AddModState:
	movea.l	_StudioBase,a6
	jmp	-222(a6)

	xdef	_FindModState
_FindModState:
	movea.l	_StudioBase,a6
	jmp	-228(a6)

	xdef	_FindSample
_FindSample:
	movea.l	_StudioBase,a6
	jmp	-234(a6)

	xdef	_GetSamplePtr
_GetSamplePtr:
	movea.l	_StudioBase,a6
	jmp	-240(a6)

	xdef	_AllocChan
_AllocChan:
	movea.l	_StudioBase,a6
	jmp	-246(a6)

	xdef	_AddAudioHandler
_AddAudioHandler:
	movea.l	_StudioBase,a6
	jmp	-252(a6)

	xdef	_FindAudioHandler
_FindAudioHandler:
	movea.l	_StudioBase,a6
	jmp	-258(a6)

	xdef	_ASClose
_ASClose:
	movea.l	_StudioBase,a6
	jmp	-264(a6)

	xdef	_ASRead
_ASRead:
	movea.l	_StudioBase,a6
	jmp	-270(a6)

	xdef	_ASWrite
_ASWrite:
	movea.l	_StudioBase,a6
	jmp	-276(a6)

	xdef	_CreateSample
_CreateSample:
	movea.l	_StudioBase,a6
	jmp	-282(a6)

	xdef	_CloseSample
_CloseSample:
	movea.l	_StudioBase,a6
	jmp	-288(a6)

	xdef	_GetEventID
_GetEventID:
	movea.l	_StudioBase,a6
	jmp	-294(a6)

	xdef	_GetSampleNum
_GetSampleNum:
	movea.l	_StudioBase,a6
	jmp	-300(a6)

	xdef	_IsRenamePossible
_IsRenamePossible:
	movea.l	_StudioBase,a6
	jmp	-306(a6)

	xdef	_LoadModule
_LoadModule:
	movea.l	_StudioBase,a6
	jmp	-312(a6)

	xdef	_LoadModState
_LoadModState:
	movea.l	_StudioBase,a6
	jmp	-318(a6)

	xdef	_LoadSamp
_LoadSamp:
	movea.l	_StudioBase,a6
	jmp	-324(a6)

	xdef	_QuickSaveSamp
_QuickSaveSamp:
	movea.l	_StudioBase,a6
	jmp	-330(a6)

	xdef	_QuickLoadSamp
_QuickLoadSamp:
	movea.l	_StudioBase,a6
	jmp	-336(a6)

	xdef	_RenameTempFile
_RenameTempFile:
	movea.l	_StudioBase,a6
	jmp	-342(a6)

	xdef	_SClose
_SClose:
	movea.l	_StudioBase,a6
	jmp	-348(a6)

	xdef	_SOpen
_SOpen:
	movea.l	_StudioBase,a6
	jmp	-354(a6)

	xdef	_SRead
_SRead:
	movea.l	_StudioBase,a6
	jmp	-360(a6)

	xdef	_SReadTag
_SReadTag:
	movea.l	_StudioBase,a6
	jmp	-366(a6)

	xdef	_SSeek
_SSeek:
	movea.l	_StudioBase,a6
	jmp	-372(a6)

	xdef	_SWrite
_SWrite:
	movea.l	_StudioBase,a6
	jmp	-378(a6)

	xdef	_SWriteTag
_SWriteTag:
	movea.l	_StudioBase,a6
	jmp	-384(a6)

	xdef	_SampleRead
_SampleRead:
	movea.l	_StudioBase,a6
	jmp	-390(a6)

	xdef	_SampleReadTag
_SampleReadTag:
	movea.l	_StudioBase,a6
	jmp	-396(a6)

	xdef	_SampleSeek
_SampleSeek:
	movea.l	_StudioBase,a6
	jmp	-402(a6)

	xdef	_SampleWrite
_SampleWrite:
	movea.l	_StudioBase,a6
	jmp	-408(a6)

	xdef	_SampleWriteTag
_SampleWriteTag:
	movea.l	_StudioBase,a6
	jmp	-414(a6)

	xdef	_SampleClose
_SampleClose:
	movea.l	_StudioBase,a6
	jmp	-420(a6)

	xdef	_SampleCopy
_SampleCopy:
	movea.l	_StudioBase,a6
	jmp	-426(a6)

	xdef	_StandardEnviroment
_StandardEnviroment:
	movea.l	_StudioBase,a6
	jmp	-432(a6)

	xdef	_SaveModState
_SaveModState:
	movea.l	_StudioBase,a6
	jmp	-438(a6)

	xdef	_SaveSamp
_SaveSamp:
	movea.l	_StudioBase,a6
	jmp	-444(a6)

	xdef	_WorkingFilePath
_WorkingFilePath:
	movea.l	_StudioBase,a6
	jmp	-450(a6)

	xdef	_SampleGetOne
_SampleGetOne:
	movea.l	_StudioBase,a6
	jmp	-456(a6)

	xdef	_AddFileFilter
_AddFileFilter:
	movea.l	_StudioBase,a6
	jmp	-462(a6)

	xdef	_SampleCreate
_SampleCreate:
	movea.l	_StudioBase,a6
	jmp	-468(a6)

	xdef	_SampleOpen
_SampleOpen:
	movea.l	_StudioBase,a6
	jmp	-474(a6)

	xdef	_ASOpen
_ASOpen:
	movea.l	_StudioBase,a6
	jmp	-480(a6)

	xdef	_SampleCreateRevision
_SampleCreateRevision:
	movea.l	_StudioBase,a6
	jmp	-486(a6)

	xdef	_AddTimeCode
_AddTimeCode:
	movea.l	_StudioBase,a6
	jmp	-492(a6)

	xdef	_SubTimeCode
_SubTimeCode:
	movea.l	_StudioBase,a6
	jmp	-498(a6)

	xdef	_IncTimeCode
_IncTimeCode:
	movea.l	_StudioBase,a6
	jmp	-504(a6)

	xdef	_QuerySmpteTime
_QuerySmpteTime:
	movea.l	_StudioBase,a6
	jmp	-510(a6)

	xdef	_TestTimeCode
_TestTimeCode:
	movea.l	_StudioBase,a6
	jmp	-516(a6)

	xdef	_AddSmpteSink
_AddSmpteSink:
	movea.l	_StudioBase,a6
	jmp	-522(a6)

	xdef	_SetSmpteSource
_SetSmpteSource:
	movea.l	_StudioBase,a6
	jmp	-528(a6)

	xdef	_AddSmpteSource
_AddSmpteSource:
	movea.l	_StudioBase,a6
	jmp	-534(a6)

	xdef	_DeleteSmpteSource
_DeleteSmpteSource:
	movea.l	_StudioBase,a6
	jmp	-540(a6)

	xdef	_DeleteSmpteSink
_DeleteSmpteSink:
	movea.l	_StudioBase,a6
	jmp	-546(a6)

	xdef	_InjectTimeCode
_InjectTimeCode:
	movea.l	_StudioBase,a6
	jmp	-552(a6)

	xdef	_InjectTimeCodeError
_InjectTimeCodeError:
	movea.l	_StudioBase,a6
	jmp	-558(a6)

	xdef	_ASHandlerQueryRecordData
_ASHandlerQueryRecordData:
	movea.l	_StudioBase,a6
	jmp	-564(a6)

	xdef	_ASHandlerQueryPlayData
_ASHandlerQueryPlayData:
	movea.l	_StudioBase,a6
	jmp	-570(a6)

	xdef	_ASWaitStop
_ASWaitStop:
	movea.l	_StudioBase,a6
	jmp	-576(a6)

	xdef	_ASQueueReads
_ASQueueReads:
	movea.l	_StudioBase,a6
	jmp	-582(a6)

	xdef	_ASFlush
_ASFlush:
	movea.l	_StudioBase,a6
	jmp	-588(a6)

	xdef	_ASPos
_ASPos:
	movea.l	_StudioBase,a6
	jmp	-594(a6)

	xdef	_ASSeek
_ASSeek:
	movea.l	_StudioBase,a6
	jmp	-600(a6)

	xdef	_InitStudio16UserEnv
_InitStudio16UserEnv:
	movea.l	_StudioBase,a6
	jmp	-606(a6)

	xdef	_ASCountBlocks
_ASCountBlocks:
	movea.l	_StudioBase,a6
	jmp	-612(a6)
