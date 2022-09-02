// TOP

#if !defined(FNCODER_LAYER_CPP)
#define FNCODER_LAYER_CPP

#include "4coder_default_include.cpp"

#include "ncoder_helper.h"
#include "ncoder_lister.h"
#include "ncoder_hooks.h"
#include "ncoder_framework.h"
#include "ncoder_draw.h"
#include "ncoder_map.h"

#include "ncoder_helper.cpp"
#include "ncoder_lister.cpp"
#include "ncoder_hooks.cpp"
#include "ncoder_framework.cpp"
#include "ncoder_draw.cpp"
#include "ncoder_map.cpp"

// NOTE(allen): Users can declare their own managed IDs here.

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

void
custom_layer_init(Application_Links *app){
    Thread_Context *tctx = get_thread_context(app);
    
    // NOTE(allen): setup for default framework
    default_framework_init(app);
    
    // NOTE(allen): default hooks and command maps
    set_all_default_hooks(app);
    
    // NOTE(set0xc3): custom hooks
    {
        set_custom_hook(app, HookID_Tick, ncoder_tick);
        set_custom_hook(app, HookID_BeginBuffer, ncoder_begin_buffer);
        set_custom_hook(app, HookID_RenderCaller, ncoder_render_caller);
        //set_custom_hook(app, HookID_WholeScreenRenderCaller, ncoder_whole_screen_render_caller);
    }
    
    mapping_init(tctx, &framework_mapping);
    String_ID global_map_id = vars_save_string_lit("keys_global");
    String_ID file_map_id = vars_save_string_lit("keys_file");
    String_ID code_map_id = vars_save_string_lit("keys_code");
    
    setup_default_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
	setup_essential_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
    
    ncoder_setup_custom_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
}

#endif //FNCODER_LAYER

// BOTTOM