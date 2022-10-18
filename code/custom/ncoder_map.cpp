String_ID vim_map_id_shared;
String_ID vim_map_id_normal;
String_ID vim_map_id_visual;
String_ID vim_map_id_insert;
String_ID vim_map_id_replace;

#define ncoder_cpp_hotkeys() \
Bind(open_long_braces,            KeyCode_LeftBracket, KeyCode_Control); \
Bind(open_long_braces_semicolon,  KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift); \
Bind(open_long_braces_break,      KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift); \
Bind(place_in_scope,              KeyCode_ForwardSlash, KeyCode_Control);

#define ncoder_default_hotkeys() \
Bind(move_left,                         KeyCode_H); \
Bind(move_left_alpha_numeric_boundary,  KeyCode_W); \
Bind(seek_beginning_of_line,            KeyCode_H, KeyCode_Shift); \
Bind(move_down,                         KeyCode_J); \
Bind(move_up,                           KeyCode_K); \
Bind(move_up_10,                        KeyCode_U); \
Bind(move_down_10,                      KeyCode_D); \
Bind(move_right,                        KeyCode_L); \
Bind(move_right_alpha_numeric_boundary, KeyCode_E); \
Bind(seek_end_of_line,                  KeyCode_L, KeyCode_Shift); \
Bind(goto_end_of_file,                  KeyCode_LeftBracket); \
Bind(goto_beginning_of_file,            KeyCode_RightBracket);

function void 
ncoder_set_current_mapid(Application_Links* app, Command_Map_ID mapid){
    View_ID view = get_active_view(app, 0);
    Buffer_ID buffer = view_get_buffer(app, view, 0);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    Command_Map_ID* map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = mapid;
}

function void 
ncoder_switch_vim_mapping(Application_Links* app, Command_Map_ID map_id, 
                          u32 color_cursor, u32 color_at_cursor, u32 color_margin_active){
    ncoder_set_current_mapid(app, map_id);
    active_color_table.arrays[defcolor_cursor].vals[0] = color_cursor;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = color_at_cursor;
}

function void 
_copy_line(Application_Links *app)
{
    View_ID view     = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    Range_i64 range  = get_view_range(app, view);
    
    i64 cursor_pos   = view_get_cursor_pos(app, view);
    i64 line_num     = get_line_number_from_pos(app, buffer, cursor_pos);
    i64 line_end_pos = get_line_end_pos(app, buffer, line_num);
    
    seek_pos_of_visual_line(app, Side_Min);
    view_set_mark(app, view, seek_pos(line_end_pos));
    
    range = get_view_range(app, view);
    clipboard_post_buffer_range(app, 0, buffer, range);
    
    view_set_cursor_and_preferred_x(app, view, seek_pos(cursor_pos));
    view_set_mark(app, view, seek_pos(cursor_pos));
}

CUSTOM_COMMAND_SIG(ncoder_go_to_normal_mode){
    ncoder_switch_vim_mapping(app, vim_map_id_normal, 0xff80ff80, 0xff293134, 0xff80ff80);
}

CUSTOM_COMMAND_SIG(ncoder_go_to_visual_mode){
    set_mark(app);
    ncoder_switch_vim_mapping(app, vim_map_id_visual, 0xfff0ff80, 0xff293134, 0xfff0ff80);
}

CUSTOM_COMMAND_SIG(ncoder_go_to_insert_mode){
    ncoder_switch_vim_mapping(app, vim_map_id_insert, 0xffff5533, 0xff00aacc, 0xffff5533);
}

CUSTOM_COMMAND_SIG(ncoder_go_to_replace_mode){
    ncoder_switch_vim_mapping(app, vim_map_id_replace, 0xffff5533, 0xff00aacc, 0xffff5533);
}

CUSTOM_COMMAND_SIG(ncoder_write_text_null){
    
}

CUSTOM_COMMAND_SIG(ncoder_paste_after_cursor){
    set_mark(app);
    paste(app);
    cursor_mark_swap(app);
    ncoder_go_to_normal_mode(app);
}

CUSTOM_COMMAND_SIG(ncoder_paste_range){
    delete_range(app);
    paste(app);
    set_mark(app);
    ncoder_go_to_normal_mode(app);
}

CUSTOM_COMMAND_SIG(ncoder_copy_line){
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    Range_i64 range = get_view_range(app, view);
    
    _copy_line(app);
    set_mark(app);
    ncoder_go_to_normal_mode(app);
}

CUSTOM_COMMAND_SIG(ncoder_copy_range){
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    Range_i64 range = get_view_range(app, view);
    
    b32 result = clipboard_post_buffer_range(app, 0, buffer, range);
    if (result){
        copy(app);
    }else{
        set_mark(app);
        move_right(app);
        copy(app);
        cursor_mark_swap(app);
    }
    set_mark(app);
    ncoder_go_to_normal_mode(app);
}


CUSTOM_COMMAND_SIG(ncoder_cut){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    Range_i64 range = get_view_range(app, view);
    if (clipboard_post_buffer_range(app, 0, buffer, range)){
        buffer_replace_range(app, buffer, range, string_u8_empty);
    }
    set_mark(app);
    ncoder_go_to_normal_mode(app);
}

CUSTOM_COMMAND_SIG(ncoder_replace_text){
    set_mark(app);
    delete_char(app);
    write_text_and_auto_indent(app);
    cursor_mark_swap(app);
    ncoder_go_to_normal_mode(app);
}

CUSTOM_COMMAND_SIG(ncoder_insert_new_line_up){
    seek_pos_of_visual_line(app, Side_Min);
    write_text(app, string_u8_litexpr("\n"));
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ncoder_insert_new_line_down){
    seek_pos_of_visual_line(app, Side_Max);
    write_text(app, string_u8_litexpr("\n"));
    set_mark(app);
}

CUSTOM_COMMAND_SIG(ncoder_click_set_cursor_if_lbutton){
    View_ID view = get_active_view(app, Access_ReadVisible);
    Mouse_State mouse = get_mouse_state(app);
    if (mouse.l){
        i64 pos = view_pos_from_xy(app, view, V2f32(mouse.p));
        view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    }
    no_mark_snap_to_cursor(app, view);
    set_next_rewrite(app, view, Rewrite_NoChange);
}

function void
ncoder_setup_custom_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
    vim_map_id_shared = vars_save_string_lit("vim_map_id_shared");
    vim_map_id_normal = vars_save_string_lit("vim_map_id_normal");
    vim_map_id_visual = vars_save_string_lit("vim_map_id_visual");
    vim_map_id_insert = vars_save_string_lit("vim_map_id_insert");
    vim_map_id_replace = vars_save_string_lit("vim_map_id_replace");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(vim_map_id_shared);
    ParentMap(global_id);
    BindCore(ncoder_startup,               CoreCode_Startup);
    BindCore(click_set_cursor_and_mark,    CoreCode_ClickActivateView);
    Bind(save,                             KeyCode_S, KeyCode_Control);
    Bind(save_all_dirty_buffers,           KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(ncoder_go_to_normal_mode,         KeyCode_Escape);
    Bind(interactive_switch_buffer,        KeyCode_I, KeyCode_Control);
    Bind(list_all_functions_all_buffers,   KeyCode_I, KeyCode_Control, KeyCode_Shift);
    BindMouseMove(ncoder_click_set_cursor_if_lbutton);
    BindMouse(click_set_cursor_and_mark,   MouseCode_Left);
    BindMouseRelease(click_set_cursor,     MouseCode_Left);
	Bind(comment_line_toggle,              KeyCode_Semicolon, KeyCode_Control);
    Bind(word_complete,                    KeyCode_Tab);
    Bind(open_file_in_quotes,              KeyCode_1, KeyCode_Alt);
    Bind(open_matching_file_cpp,           KeyCode_2, KeyCode_Alt);
    Bind(goto_line,                        KeyCode_G, KeyCode_Control);
    Bind(search,                           KeyCode_F, KeyCode_Control);
    Bind(search_identifier,                KeyCode_F, KeyCode_Control, KeyCode_Shift);
    Bind(query_replace,                    KeyCode_R, KeyCode_Control);
    Bind(query_replace_identifier,         KeyCode_R, KeyCode_Control, KeyCode_Shift);
	Bind(command_lister,                   KeyCode_X, KeyCode_Alt);
	Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Control);
	Bind(backspace_char,                   KeyCode_Backspace);
	Bind(write_space,                      KeyCode_Space);
    
    SelectMap(vim_map_id_normal);
    ParentMap(vim_map_id_shared);
    ncoder_default_hotkeys();
	ncoder_cpp_hotkeys();
    Bind(ncoder_go_to_visual_mode,     KeyCode_V);
    Bind(ncoder_go_to_insert_mode,     KeyCode_I);
    Bind(ncoder_go_to_replace_mode,    KeyCode_R);
    Bind(ncoder_paste_after_cursor,    KeyCode_P);
    Bind(delete_char,                  KeyCode_X);
    Bind(undo,                         KeyCode_Z, KeyCode_Control);
    Bind(redo,                         KeyCode_Z, KeyCode_Control, KeyCode_Shift);
    Bind(delete_line,                  KeyCode_D, KeyCode_Control);
    Bind(ncoder_copy_line,             KeyCode_Y);
    Bind(move_line_up,                 KeyCode_K, KeyCode_Alt);
    Bind(move_line_down,               KeyCode_J, KeyCode_Alt);
    Bind(jump_to_definition,           KeyCode_W, KeyCode_Control, KeyCode_Shift);
    Bind(jump_to_definition_at_cursor, KeyCode_W, KeyCode_Control);
    //Bind(jump_to_next_point,           KeyCode_P, KeyCode_Control);
    Bind(jump_to_last_point,           KeyCode_P, KeyCode_Control, KeyCode_Shift);
	Bind(ncoder_insert_new_line_up,    KeyCode_K, KeyCode_Shift);
	Bind(ncoder_insert_new_line_down,  KeyCode_J, KeyCode_Shift);
	
    SelectMap(vim_map_id_visual);
    ParentMap(vim_map_id_shared);
    ncoder_default_hotkeys();
	ncoder_cpp_hotkeys();
    Bind(ncoder_paste_range, KeyCode_P);
    Bind(ncoder_copy_range,  KeyCode_Y);
    Bind(ncoder_cut,         KeyCode_X);
    
    SelectMap(vim_map_id_insert);
    ParentMap(vim_map_id_shared);
	ncoder_cpp_hotkeys();
    BindTextInput(write_text_and_auto_indent);
    
    SelectMap(vim_map_id_replace);
    ParentMap(vim_map_id_shared);
    BindTextInput(ncoder_replace_text);
    
    SelectMap(file_id);
    BindTextInput(ncoder_write_text_null);
    ParentMap(vim_map_id_normal);
    
    SelectMap(code_id);
    BindTextInput(ncoder_write_text_null);
    ParentMap(vim_map_id_normal);
}