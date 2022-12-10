function void
ncoder_initialize(Application_Links *app, String_Const_u8_Array file_names, i32 override_font_size, b32 override_hinting){
#define M \
"Welcome to " VERSION "\n" \
"\n"
    print_message(app, string_u8_litexpr(M));
#undef M
    
    Scratch_Block scratch(app);
    
    load_config_and_apply(app, &global_config_arena, override_font_size, override_hinting);
    
    // load project from open command line
    {
        if(file_names.count != 0)
        {
			String_Const_u8 hot_directory = push_hot_directory(app, scratch);
            String_Const_u8 file_path = file_names.vals[file_names.count - 1];
            String_Const_u8 file_name = string_front_of_path(file_path);
			
            if (string_match(file_name, string_u8_litexpr("project.4coder")))
            {
                if (file_path.str[0] == '.' && file_path.str[1] == '/')
                {
                    String_u8 string = Su8(hot_directory, file_path.size);
                    string_append_character(&string, '/');
                    string_append(&string, file_name);
					
                    File_Name_Data dump = dump_file_search_up_path(app, scratch, string.string, file_name);
                    if (dump.data.str != 0)
                    {
                        set_hot_directory(app, string.string);
                    }
                }
                else
                {
                    File_Name_Data dump = dump_file_search_up_path(app, scratch, file_path, file_name);
                    if (dump.data.str != 0)
                    {
                        set_hot_directory(app, file_path);
                    }
                }
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