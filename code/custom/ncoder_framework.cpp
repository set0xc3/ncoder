function void
ncoder_initialize(Application_Links *app, String_Const_u8_Array file_names, i32 override_font_size, b32 override_hinting){
#define M \
"Welcome to " VERSION "\n" \
"\n"
    print_message(app, string_u8_litexpr(M));
#undef M
    
    Scratch_Block scratch(app);
    
    load_config_and_apply(app, &global_config_arena, override_font_size, override_hinting);
    
    // load project.4coder file
    {
        if(file_names.count != 0)
        {
            String_Const_u8 name_hot = string_front_of_path(file_names.vals[file_names.count - 1]);
            if (string_match(name_hot, string_u8_litexpr("project.4coder")))
            {
                set_hot_directory(app, file_names.vals[file_names.count - 1]);
            }
        }    
    }

    // open command line files
    String_Const_u8 hot_directory = push_hot_directory(app, scratch);
    for (i32 i = 0; i < file_names.count; i += 1){
        Temp_Memory_Block temp(scratch);
        String_Const_u8 input_name = file_names.vals[i];
        String_Const_u8 full_name = push_u8_stringf(scratch, "%.*s/%.*s",
                                                    string_expand(hot_directory),
                                                    string_expand(input_name));
        Buffer_ID new_buffer = create_buffer(app, full_name, BufferCreate_NeverNew|BufferCreate_MustAttachToFile);
        if (new_buffer == 0){
            create_buffer(app, input_name, 0);
        }
    }
    
    Color_Table *color_table = get_color_table_by_name(string_u8_litexpr("theme-ncoder"));
    if (color_table != 0)
        active_color_table = *color_table;
}