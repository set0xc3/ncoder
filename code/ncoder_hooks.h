#ifndef NCODER_HOOKS_H
#define NCODER_HOOKS_H

function void ncoder_reopen_files_on_unloaded_changes(Application_Links *app, Buffer_ID buffer);
function void ncoder_tick(Application_Links *app, Frame_Info frame_info);

function void ncoder_render_buffer(Application_Links *app, View_ID view_id, Face_ID face_id,
                                   Buffer_ID buffer, Text_Layout_ID text_layout_id,
                                   Rect_f32 rect);
function void ncoder_render_caller(Application_Links *app, Frame_Info frame_info, View_ID view_id);
function void ncoder_whole_screen_render_caller(Application_Links *app, Frame_Info frame_info);

#endif //NCODER_HOOKS_H
