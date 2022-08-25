////////////////////////////////
// NOTE(allen): Layout Invalidate

function void
ncoder_clear_all_layouts(Application_Links *app, Buffer_ID buffer){
    buffer_clear_layout_cache(app, buffer);
}