#ifndef NCODER_DRAW_H
#define NCODER_DRAW_H

function void ncoder_draw_file_bar(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Rect_f32 bar);
function Rect_f32 ncoder_draw_background_and_margin(Application_Links *app, View_ID view, b32 is_active_view);
function Rect_f32_Pair ncoder_layout_line_number_margin(Rect_f32 rect, f32 digit_advance, i64 digit_count);
function Rect_f32_Pair ncoder_layout_line_number_margin(Application_Links *app, Buffer_ID buffer, Rect_f32 rect, f32 digit_advance);
function void ncoder_draw_line_number_margin(Application_Links *app, View_ID view_id, Buffer_ID buffer, Face_ID face_id, Text_Layout_ID text_layout_id, Rect_f32 margin);


#endif //NCODER_DRAW_H
