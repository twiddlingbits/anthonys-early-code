;:ts=8

	xref	_GadLibBase
	xdef	_add_list_entry
_add_list_entry:
	movea.l	_GadLibBase,a6
	jmp	-30(a6)

	xdef	_askuser
_askuser:
	movea.l	_GadLibBase,a6
	jmp	-36(a6)

	xdef	_delete_list_entry_by_userdata
_delete_list_entry_by_userdata:
	movea.l	_GadLibBase,a6
	jmp	-42(a6)

	xdef	_calc_slider_internals
_calc_slider_internals:
	movea.l	_GadLibBase,a6
	jmp	-48(a6)

	xdef	_calc_knob_xy
_calc_knob_xy:
	movea.l	_GadLibBase,a6
	jmp	-54(a6)

	xdef	_calc_knob_size
_calc_knob_size:
	movea.l	_GadLibBase,a6
	jmp	-60(a6)

	xdef	_calc_top_element
_calc_top_element:
	movea.l	_GadLibBase,a6
	jmp	-66(a6)

	xdef	_delete_gadget
_delete_gadget:
	movea.l	_GadLibBase,a6
	jmp	-72(a6)

	xdef	_delete_gadget_chain
_delete_gadget_chain:
	movea.l	_GadLibBase,a6
	jmp	-78(a6)

	xdef	_delete_list_entry
_delete_list_entry:
	movea.l	_GadLibBase,a6
	jmp	-84(a6)

	xdef	_draw_button
_draw_button:
	movea.l	_GadLibBase,a6
	jmp	-90(a6)

	xdef	_draw_indent
_draw_indent:
	movea.l	_GadLibBase,a6
	jmp	-96(a6)

	xdef	_draw_knob
_draw_knob:
	movea.l	_GadLibBase,a6
	jmp	-102(a6)

	xdef	_handle_new_select
_handle_new_select:
	movea.l	_GadLibBase,a6
	jmp	-108(a6)

	xdef	_hide_gadget
_hide_gadget:
	movea.l	_GadLibBase,a6
	jmp	-114(a6)

	xdef	_move_knob
_move_knob:
	movea.l	_GadLibBase,a6
	jmp	-120(a6)

	xdef	_new_button_size
_new_button_size:
	movea.l	_GadLibBase,a6
	jmp	-126(a6)

	xdef	_new_gadget_size
_new_gadget_size:
	movea.l	_GadLibBase,a6
	jmp	-132(a6)

	xdef	_new_list_size
_new_list_size:
	movea.l	_GadLibBase,a6
	jmp	-138(a6)

	xdef	_new_slider_size
_new_slider_size:
	movea.l	_GadLibBase,a6
	jmp	-144(a6)

	xdef	_LockGadLibRender
_LockGadLibRender:
	movea.l	_GadLibBase,a6
	jmp	-150(a6)

	xdef	_refresh_button
_refresh_button:
	movea.l	_GadLibBase,a6
	jmp	-156(a6)

	xdef	_refresh_droplist
_refresh_droplist:
	movea.l	_GadLibBase,a6
	jmp	-162(a6)

	xdef	_refresh_gadget
_refresh_gadget:
	movea.l	_GadLibBase,a6
	jmp	-168(a6)

	xdef	_refresh_gadget_chain
_refresh_gadget_chain:
	movea.l	_GadLibBase,a6
	jmp	-174(a6)

	xdef	_refresh_knob
_refresh_knob:
	movea.l	_GadLibBase,a6
	jmp	-180(a6)

	xdef	_refresh_list
_refresh_list:
	movea.l	_GadLibBase,a6
	jmp	-186(a6)

	xdef	_refresh_list_text
_refresh_list_text:
	movea.l	_GadLibBase,a6
	jmp	-192(a6)

	xdef	_refresh_list_slider
_refresh_list_slider:
	movea.l	_GadLibBase,a6
	jmp	-198(a6)

	xdef	_refresh_slider
_refresh_slider:
	movea.l	_GadLibBase,a6
	jmp	-204(a6)

	xdef	_show_gadget
_show_gadget:
	movea.l	_GadLibBase,a6
	jmp	-210(a6)

	xdef	_xor_button
_xor_button:
	movea.l	_GadLibBase,a6
	jmp	-216(a6)

	xdef	_telluser
_telluser:
	movea.l	_GadLibBase,a6
	jmp	-222(a6)

	xdef	_UnLockGadLibRender
_UnLockGadLibRender:
	movea.l	_GadLibBase,a6
	jmp	-228(a6)

	xdef	_create_gadget
_create_gadget:
	movea.l	_GadLibBase,a6
	jmp	-234(a6)

	xdef	_create_gadget_chain
_create_gadget_chain:
	movea.l	_GadLibBase,a6
	jmp	-240(a6)

	xdef	_get_list_entry_ptr
_get_list_entry_ptr:
	movea.l	_GadLibBase,a6
	jmp	-246(a6)

	xdef	_QuickWindow
_QuickWindow:
	movea.l	_GadLibBase,a6
	jmp	-252(a6)

	END
