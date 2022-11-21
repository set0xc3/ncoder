/*
ncoder_project_commands.cpp - commands for loading and using a project.
*/

// TOP

#include <string.h>

CUSTOM_COMMAND_SIG(ncoder_grep_in_project)
CUSTOM_DOC("Test")
{
    Scratch_Block scratch(app);
    
    // NOTE(allen): Load the project file from the hot directory
    String8 project_path = push_hot_directory(app, scratch);
    File_Name_Data dump = dump_file_search_up_path(app, scratch, project_path, string_u8_litexpr("projects.4coder"));
    String8 project_root = string_remove_last_folder(dump.file_name);
    String8 file_name = push_u8_stringf(scratch, "%.*s/projects.4coder", string_expand(project_path));
    
    if (dump.data.str == 0){
        print_message(app, string_u8_litexpr("Did not find projects.4coder.\n"));
    }
    
    // NOTE(allen): Parse config data out of project file
    Config *config_parse = 0;
    Variable_Handle projects_var = vars_get_nil();
    if (dump.data.str != 0){
        Token_Array array = token_array_from_text(app, scratch, dump.data);
        if (array.tokens != 0){
            config_parse = def_config_parse(app, scratch, dump.file_name, dump.data, array);
            if (config_parse != 0){
                projects_var = def_fill_var_from_config(app, vars_get_root(), vars_save_string_lit("prjects"), config_parse);
            }
        }
    }
    
    // NOTE(allen): Print Project
    if (!vars_is_nil(projects_var)){
        print_message(app, string_u8_litexpr("\n"));
    }
    
    Variable_Handle load_paths_var = vars_read_key(projects_var, vars_save_string_lit("projects"));
    vars_print(app, load_paths_var);
    
    for (Variable_Handle load_path_var = vars_first_child(load_paths_var);
         !vars_is_nil(load_path_var);
         load_path_var = vars_next_sibling(load_path_var)){
        String8 path = vars_string_from_var(scratch, load_path_var);
        print_message(app, path);
        print_message(app, string_u8_litexpr("\n"));
    }
}

CUSTOM_COMMAND_SIG(ncoder_save_project)
CUSTOM_DOC("Test")
{
    Scratch_Block scratch(app);
    
    // NOTE(allen): Load the project file from the hot directory
    String8 project_path = push_hot_directory(app, scratch);
    File_Name_Data dump = dump_file_search_up_path(app, scratch, project_path, string_u8_litexpr("projects.4coder"));
    String8 project_root = string_remove_last_folder(dump.file_name);
    String8 file_name = push_u8_stringf(scratch, "%.*s/projects.4coder", string_expand(project_path));
    
    if (dump.data.str == 0){
        FILE *out = fopen((char*)file_name.str, "wb");
        if (out != 0){
            fprintf(out, " ");
            fclose(out);
            dump = dump_file_search_up_path(app, scratch, project_path, string_u8_litexpr("projects.4coder"));
        }
        print_message(app, string_u8_litexpr("Did not find projects.4coder.\n"));
    }
    
    // NOTE(allen): Parse config data out of project file
    Config *config_parse = 0;
    Variable_Handle projects_var = vars_get_nil();
    if (dump.data.str != 0){
        Token_Array array = token_array_from_text(app, scratch, dump.data);
        if (array.tokens != 0){
            config_parse = def_config_parse(app, scratch, dump.file_name, dump.data, array);
            if (config_parse != 0){
                projects_var = def_fill_var_from_config(app, vars_get_root(), vars_save_string_lit("prjects"), config_parse);
            }
        }
    }
    
    // NOTE(set0xc3): Delete char
    if (dump.data.size == 1){
        FILE *out = fopen((char*)file_name.str, "wb");
        if (out != 0){
            fprintf(out, "");
            fclose(out);
        }
    }
    
    // NOTE(allen): Print Project
    if (!vars_is_nil(projects_var)){
        print_message(app, string_u8_litexpr("\n"));
    }
    
    // NOTE(allen): Open All Project Files
    
    // NOTE(set0xc3): Save project
    {
        String_ID project_id = vars_save_string_lit("project");
        String8 project = vars_read_string(scratch, project_id);
        
        project = push_stringf(scratch, "%s%i", project.str, config_parse->count + 1);
        project_id = vars_save_string(project);
        
        Variable_Handle project_var = vars_new_variable(projects_var, project_id, vars_save_string_lit("/mnt/duck/dev/ncoder"));
        String_Const_u8 var_key = vars_key_from_var(scratch, project_var);
        String_Const_u8 var_val = vars_string_from_var(scratch, project_var);
        
        project = push_stringf(scratch, "%.*s = \"%.*s\";\n",
                               string_expand(var_key),
                               string_expand(var_val));
        
        FILE *out = fopen((char*)file_name.str, "a");
        if (out != 0){
            fprintf(out, "%.*s", string_expand(project));
            fclose(out);
        }
        
        print_message(app, project);
    }
}

// BOTTOM