String_ID vim_map_id_shared;
String_ID vim_map_id_normal;
String_ID vim_map_id_insert;

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
    active_color_table.arrays[defcolor_margin_active].vals[0] = color_margin_active;
}

CUSTOM_COMMAND_SIG(ncoder_go_to_normal_mode){
    ncoder_switch_vim_mapping(app, vim_map_id_normal, 0xff80ff80, 0xff293134, 0xff80ff80);
}

CUSTOM_COMMAND_SIG(ncoder_go_to_insert_mode){
    ncoder_switch_vim_mapping(app, vim_map_id_insert, 0xffff5533, 0xff00aacc, 0xffff5533);
}

function void
ncoder_setup_custom_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
    vim_map_id_shared = vars_save_string_lit("vim_map_id_shared");
    vim_map_id_normal = vars_save_string_lit("vim_map_id_normal");
    vim_map_id_insert = vars_save_string_lit("vim_map_id_insert");
    
    MappingScope();
    SelectMapping(mapping);
    
    SelectMap(vim_map_id_shared);
    ParentMap(global_id);
    BindCore(ncoder_startup, CoreCode_Startup);
    
    Bind(ncoder_go_to_normal_mode, KeyCode_Escape);
    Bind(interactive_switch_buffer, KeyCode_I, KeyCode_Control);
    
    SelectMap(vim_map_id_normal);
    ParentMap(vim_map_id_shared);
    Bind(ncoder_go_to_insert_mode, KeyCode_I);
    
    SelectMap(vim_map_id_insert);
    ParentMap(vim_map_id_shared);
    BindTextInput(write_text_and_auto_indent);
    
    SelectMap(file_id);
    ParentMap(vim_map_id_normal);
    
    SelectMap(code_id);
    ParentMap(vim_map_id_normal);
}