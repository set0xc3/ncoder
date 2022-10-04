function void
ncoder_draw_file_bar(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Rect_f32 bar){
    Scratch_Block scratch(app);
    
    draw_rectangle_fcolor(app, bar, 0.f, fcolor_id(defcolor_bar));
    
    FColor base_color = fcolor_id(defcolor_base);
    FColor pop2_color = fcolor_id(defcolor_pop2);
    
    i64 cursor_position = view_get_cursor_pos(app, view_id);
    Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(cursor_position));
    
    Fancy_Line list = {};
    String_Const_u8 unique_name = push_buffer_unique_name(app, scratch, buffer);
    push_fancy_string(scratch, &list, base_color, unique_name);
    push_fancy_stringf(scratch, &list, base_color, " - Row: %3.lld Col: %3.lld -", cursor.line, cursor.col);
    
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting,
                                                     Line_Ending_Kind);
    switch (*eol_setting){
        case LineEndingKind_Binary:
        {
            push_fancy_string(scratch, &list, base_color, string_u8_litexpr(" bin"));
        }break;
        
        case LineEndingKind_LF:
        {
            push_fancy_string(scratch, &list, base_color, string_u8_litexpr(" lf"));
        }break;
        
        case LineEndingKind_CRLF:
        {
            push_fancy_string(scratch, &list, base_color, string_u8_litexpr(" crlf"));
        }break;
    }
    
    u8 space[3];
    {
        Dirty_State dirty = buffer_get_dirty_state(app, buffer);
        String_u8 str = Su8(space, 0, 3);
        if (dirty != 0){
            string_append(&str, string_u8_litexpr(" "));
        }
        if (HasFlag(dirty, DirtyState_UnsavedChanges)){
            string_append(&str, string_u8_litexpr("*"));
        }
        if (HasFlag(dirty, DirtyState_UnloadedChanges)){
            string_append(&str, string_u8_litexpr("!"));
        }
        push_fancy_string(scratch, &list, pop2_color, str.string);
    }
    
    Vec2_f32 p = bar.p0 + V2f32(2.f, 2.f);
    draw_fancy_line(app, face_id, fcolor_zero(), &list, p);
}

function Rect_f32
ncoder_draw_background_and_margin(Application_Links *app, View_ID view, b32 is_active_view){
    FColor margin_color = get_panel_margin_color(is_active_view?UIHighlight_Active:UIHighlight_None);
    return(draw_background_and_margin(app, view, margin_color, fcolor_id(defcolor_back), 1.f));
}

function Rect_f32_Pair
ncoder_layout_line_number_margin(Rect_f32 rect, f32 digit_advance, i64 digit_count){
    f32 margin_width = (f32)digit_count*digit_advance + 8.f;
    return(rect_split_left_right(rect, margin_width));
}

function Rect_f32_Pair
ncoder_layout_line_number_margin(Application_Links *app, Buffer_ID buffer, Rect_f32 rect, f32 digit_advance){
    i64 line_count = buffer_get_line_count(app, buffer);
    i64 line_count_digit_count = digit_count_from_integer(line_count, 10);
    return(ncoder_layout_line_number_margin(rect, digit_advance, line_count_digit_count));
}

function void
ncoder_draw_line_number_margin(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Text_Layout_ID text_layout_id, Rect_f32 margin){
    ProfileScope(app, "draw line number margin");
    
    Scratch_Block scratch(app);
    FColor line_color = fcolor_id(defcolor_line_numbers_text);
    
    Rect_f32 prev_clip = draw_set_clip(app, margin);
    draw_rectangle_fcolor(app, margin, 0.f, fcolor_id(defcolor_line_numbers_back));
    
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    i64 line_count = buffer_get_line_count(app, buffer) + 1;
    i64 line_count_digit_count = digit_count_from_integer(line_count - 1, 10);
    
    Fancy_String fstring = {};
    u8 *digit_buffer = push_array(scratch, u8, line_count_digit_count);
    String_Const_u8 digit_string = SCu8(digit_buffer, line_count_digit_count);
    for (i32 i = 0; i < line_count_digit_count; i += 1){
        digit_buffer[i] = '0';
    }
    
    Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(visible_range.first));
    i64 line_number = cursor.line;
    
    Buffer_Cursor cursor_opl = view_compute_cursor(app, view_id, seek_pos(visible_range.one_past_last));
    i64 one_past_last_line_number = cursor_opl.line + 1;
    
    u8 *small_digit = digit_buffer + line_count_digit_count - 1;
    {
        u8 *ptr = small_digit;
        if (line_number == 0){
            *ptr = '0';
        }
        else{
            for (u64 X = line_number; X > 0; X /= 10){
                *ptr = '0' + (X%10);
                ptr -= 1;
            }
        }
    }
    
    for (;line_number < one_past_last_line_number &&
         line_number < line_count;){
        Range_f32 line_y = text_layout_line_on_screen(app, text_layout_id, line_number);
        Vec2_f32 p = V2f32(margin.x0, line_y.min);
        
        fill_fancy_string(&fstring, 0, line_color, 0, 0, digit_string);
        draw_fancy_string(app, face_id, fcolor_zero(), &fstring, p);
        
        line_number += 1;
        {
            u8 *ptr = small_digit;
            for (;;){
                if (ptr < digit_buffer){
                    break;
                }
                if (*ptr == ' '){
                    *ptr = '0';
                }
                if (*ptr == '9'){
                    *ptr = '0';
                    ptr -= 1;
                }
                else{
                    *ptr += 1;
                    break;
                }
            }
        }
    }
    
    draw_set_clip(app, prev_clip);
}