CUSTOM_COMMAND_SIG(ncoder_startup)
CUSTOM_DOC("Default command for responding to a startup event")
{
    ProfileScope(app, "default startup");
    User_Input input = get_current_input(app);
    if (match_core_code(&input, CoreCode_Startup)){
        String_Const_u8_Array file_names = input.event.core.file_names;
        load_themes_default_folder(app);
        default_4coder_initialize(app, file_names);
        b32 auto_load = def_get_config_b32(vars_save_string_lit("automatically_load_project"));
        if (auto_load){
            load_project(app);
        }
    }
    
    {
        def_audio_init();
    }
    
    {
        def_enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
        clear_all_layouts(app);
    }
}

function void 
ncoder_reload_files_on_changes(Application_Links *app, Buffer_ID buffer){
    b32 auto_reload_file = def_get_config_b32(vars_save_string_lit("automatically_reload_file_on_changes"));
    if (auto_reload_file){
        
        Dirty_State dirty = buffer_get_dirty_state(app, buffer);
        if (HasFlag(dirty, DirtyState_UnloadedChanges)){
            
            // Save scroll state.
            View_ID view = get_active_view(app, Access_ReadVisible);
            Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
            
            // Reopen the file.
            Buffer_Reopen_Result result = buffer_reopen(app, buffer, 0);
            if (HasFlag(result, BufferReopenResult_Failed)){
                buffer_kill(app, buffer, 0);
            }
            
            // Restore scroll state.
            view_set_buffer_scroll(app, view, scroll, SetBufferScroll_SnapCursorIntoView);
            no_mark_snap_to_cursor(app, view);
        }
    }
}

function void
ncoder_tick(Application_Links *app, Frame_Info frame_info){
    View_ID view = get_active_view(app, Access_ReadVisible);
    for (Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_ReadWriteVisible)){
        
        {
            ncoder_reload_files_on_changes(app, buffer);
        }
        
        ////////////////////////////////
        // NOTE(allen): Clear layouts if virtual whitespace setting changed.
        
        {
            b32 enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
            if (enable_virtual_whitespace != def_enable_virtual_whitespace){
                def_enable_virtual_whitespace = enable_virtual_whitespace;
                ncoder_clear_all_layouts(app, buffer);
            }
        }
    }
    
    ////////////////////////////////
    // NOTE(allen): Update code index
    
    code_index_update_tick(app);
    
    ////////////////////////////////
    // NOTE(allen): Update fade ranges
    
    if (tick_all_fade_ranges(app, frame_info.animation_dt)){
        animate_in_n_milliseconds(app, 0);
    }
}

function void
ncoder_render_buffer(Application_Links *app, View_ID view_id, Face_ID face_id,
                     Buffer_ID buffer, Text_Layout_ID text_layout_id,
                     Rect_f32 rect){
    ProfileScope(app, "render buffer");
    
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);
    
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    
    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    u64 cursor_roundness_100 = def_get_config_u64(app, vars_save_string_lit("cursor_roundness"));
    f32 cursor_roundness = metrics.normal_advance*cursor_roundness_100*0.01f;
    f32 mark_thickness = (f32)def_get_config_u64(app, vars_save_string_lit("mark_thickness"));
    
    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if (token_array.tokens != 0){
        draw_cpp_token_colors(app, text_layout_id, &token_array);
        
        // NOTE(allen): Scan for TODOs and NOTEs
        b32 use_comment_keyword = def_get_config_b32(vars_save_string_lit("use_comment_keyword"));
        if (use_comment_keyword){
            Comment_Highlight_Pair pairs[] = {
                {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
                {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
            };
            draw_comment_highlights(app, buffer, text_layout_id, &token_array, pairs, ArrayCount(pairs));
        }
        
#if 0
        // TODO(allen): Put in 4coder_draw.cpp
        // NOTE(allen): Color functions
        
        Scratch_Block scratch(app);
        ARGB_Color argb = 0xFFFF00FF;
        
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, visible_range.first);
        for (;;){
            if (!token_it_inc_non_whitespace(&it)){
                break;
            }
            Token *token = token_it_read(&it);
            String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
            Code_Index_Note *note = code_index_note_from_string(lexeme);
            if (note != 0 && note->note_kind == CodeIndexNote_Function){
                paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);
            }
        }
#endif
    }
    else{
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }
    
    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);
    
    // NOTE(allen): Scope highlight
    b32 use_scope_highlight = def_get_config_b32(vars_save_string_lit("use_scope_highlight"));
    if (use_scope_highlight){
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    b32 use_error_highlight = def_get_config_b32(vars_save_string_lit("use_error_highlight"));
    b32 use_jump_highlight = def_get_config_b32(vars_save_string_lit("use_jump_highlight"));
    if (use_error_highlight || use_jump_highlight){
        // NOTE(allen): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if (use_error_highlight){
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                                 fcolor_id(defcolor_highlight_junk));
        }
        
        // NOTE(allen): Search highlight
        if (use_jump_highlight){
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if (jump_buffer != compilation_buffer){
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                                     fcolor_id(defcolor_highlight_white));
            }
        }
    }
    
    // NOTE(allen): Color parens
    b32 use_paren_helper = def_get_config_b32(vars_save_string_lit("use_paren_helper"));
    if (use_paren_helper){
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    // NOTE(allen): Line highlight
    b32 highlight_line_at_cursor = def_get_config_b32(vars_save_string_lit("highlight_line_at_cursor"));
    if (highlight_line_at_cursor && is_active_view){
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
        draw_line_highlight(app, text_layout_id, line_number, fcolor_id(defcolor_highlight_cursor_line));
    }
    
    // NOTE(allen): Whitespace highlight
    b64 show_whitespace = false;
    view_get_setting(app, view_id, ViewSetting_ShowWhitespace, &show_whitespace);
    if (show_whitespace){
        if (token_array.tokens == 0){
            draw_whitespace_highlight(app, buffer, text_layout_id, cursor_roundness);
        }
        else{
            draw_whitespace_highlight(app, text_layout_id, &token_array, cursor_roundness);
        }
    }
    
    // NOTE(allen): Cursor
    switch (fcoder_mode){
        case FCoderMode_Original:
        {
            draw_original_4coder_style_cursor_mark_highlight(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);
        }break;
        case FCoderMode_NotepadLike:
        {
            draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
        }break;
    }
    
    // NOTE(allen): Fade ranges
    paint_fade_ranges(app, text_layout_id, buffer);
    
    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);
    
    draw_set_clip(app, prev_clip);
}

function void
ncoder_render_caller(Application_Links *app, Frame_Info frame_info, View_ID view_id){
    ProfileScope(app, "default render caller");
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    
    Rect_f32 region = draw_background_and_margin(app, view_id, is_active_view);
    Rect_f32 prev_clip = draw_set_clip(app, region);
    
    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;
    
    
    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar){
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        ncoder_draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }
    
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);
    
    Buffer_Point_Delta_Result delta = delta_apply(app, view_id,
                                                  frame_info.animation_dt, scroll);
    if (!block_match_struct(&scroll.position, &delta.point)){
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }
    if (delta.still_animating){
        animate_in_n_milliseconds(app, 0);
    }
    
    // NOTE(allen): query bars
    region = default_draw_query_bars(app, region, view_id, face_id);
    
    // NOTE(allen): FPS hud
    if (show_fps_hud){
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
        animate_in_n_milliseconds(app, 1000);
    }
    
    // NOTE(allen): layout line numbers
    b32 show_line_number_margins = def_get_config_b32(vars_save_string_lit("show_line_number_margins"));
    Rect_f32 line_number_rect = {};
    if (show_line_number_margins){
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }
    
    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);
    
    // NOTE(allen): draw line numbers
    if (show_line_number_margins){
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }
    
    // NOTE(allen): draw the buffer
    ncoder_render_buffer(app, view_id, face_id, buffer, text_layout_id, region);
    
    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

function void
ncoder_whole_screen_render_caller(Application_Links *app, Frame_Info frame_info){
    Rect_f32 region = global_get_screen_rectangle(app);
    Vec2_f32 center = rect_center(region);
    
    Face_ID face_id = get_face_id(app, 0);
    Scratch_Block scratch(app);
    draw_string_oriented(app, face_id, finalize_color(defcolor_text_default, 0),
                         SCu8("Hello, World!"), center - V2f32(0.f, 0.f),
                         0, V2f32(1.f, 0.f));
}
