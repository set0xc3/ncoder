#ifndef NCODER_MAP_H
#define NCODER_MAP_H

function void ncoder_setup_custom_mapping(Mapping *mapping, i64 global_id, i64 file_id, i64 code_id);
function void ncoder_switch_vim_mapping(Application_Links* app, Command_Map_ID mapid, u32 color_cursor, u32 color_at_cursor, u32 color_margin_active);

#endif //NCODER_MAP_H
