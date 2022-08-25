String_ID mapid_shared;
String_ID mapid_normal;
String_ID mapid_insert;
String_ID mapid_delete;

function void 
set_current_mapid(Application_Links* app, Command_Map_ID mapid){
    View_ID view = get_active_view(app, 0);
    Buffer_ID buffer = view_get_buffer(app, view, 0);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    Command_Map_ID* map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = mapid;
}

CUSTOM_COMMAND_SIG(go_to_normal_mode){
    set_current_mapid(app, mapid_normal);
    
    active_color_table.arrays[defcolor_cursor].vals[0] = 0xffff5533;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = 0xff00aacc;
    active_color_table.arrays[defcolor_margin_active].vals[0] = 0xffff5533;
}

CUSTOM_COMMAND_SIG(go_to_insert_mode){
    set_current_mapid(app, mapid_insert);
    
    active_color_table.arrays[defcolor_cursor].vals[0] = 0xff80ff80;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = 0xff293134;
    active_color_table.arrays[defcolor_margin_active].vals[0] = 0xff80ff80;
}

CUSTOM_COMMAND_SIG(go_to_delete_mode){
    set_current_mapid(app, mapid_delete);
    
    active_color_table.arrays[defcolor_cursor].vals[0] = 0xffffff00;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = 0xff0000ff;
}

CUSTOM_COMMAND_SIG(modal_delete_word_right){
    delete_alpha_numeric_boundary(app);
    go_to_normal_mode(app);
}

CUSTOM_COMMAND_SIG(modal_delete_word_left){
    backspace_alpha_numeric_boundary(app);
    go_to_normal_mode(app);
}

function void
ncoder_setup_custom_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id){
    mapid_shared = vars_save_string_lit("mapid_shared");
    mapid_normal = vars_save_string_lit("mapid_normal");
    mapid_insert = vars_save_string_lit("mapid_insert");
    mapid_delete = vars_save_string_lit("mapid_delete");
    
    MappingScope();
    SelectMapping(&framework_mapping);
    
    SelectMap(global_id);
    
    SelectMap(mapid_shared);
    BindCore(ncoder_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    Bind(go_to_normal_mode, KeyCode_Escape);
    Bind(move_left, KeyCode_Left);
    Bind(move_right, KeyCode_Right);
    Bind(move_up, KeyCode_Up);
    Bind(move_down, KeyCode_Down);
    
    SelectMap(mapid_normal);
    ParentMap(mapid_shared);
    Bind(go_to_insert_mode, KeyCode_Tab);
    Bind(go_to_delete_mode, KeyCode_D);
    
    SelectMap(mapid_insert);
    ParentMap(mapid_shared);
    BindTextInput(write_text_and_auto_indent);
    
    SelectMap(mapid_delete);
    Bind(go_to_normal_mode, KeyCode_Escape);
    Bind(modal_delete_word_left, KeyCode_Left);
    Bind(modal_delete_word_right, KeyCode_Right);
    
    /* This is to make sure that the default bindings on the buffers will be mapid_normal. */
    SelectMap(file_id);
    ParentMap(mapid_normal);
    
    SelectMap(code_id);
    ParentMap(mapid_normal);
}