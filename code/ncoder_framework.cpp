function void
ncoder_initialize(Application_Links *app, String_Const_u8_Array file_names, i32 override_font_size, b32 override_hinting){
#define M \
"Welcome to " VERSION "\n" \
"If you're new to 4coder there is a built in tutorial\n" \
"Use the key combination [ X Alt ] (on mac [ X Control ])\n" \
"Type in 'hms_demo_tutorial' and press enter\n" \
"\n" \
"Direct bug reports and feature requests to https://github.com/4coder-editor/4coder/issues\n" \
"\n" \
"Other questions and discussion can be directed to editor@4coder.net or 4coder.handmade.network\n" \
"\n" \
"The change log can be found in CHANGES.txt\n" \
"\n"
    print_message(app, string_u8_litexpr(M));
#undef M
    
    Scratch_Block scratch(app);
    
    load_config_and_apply(app, &global_config_arena, override_font_size, override_hinting);
    
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
}