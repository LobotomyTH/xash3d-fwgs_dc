
# List all of your C files here, but change the extension to ".o"
# Include "romdisk.o" if you want a rom disk.

XASH_CLIENT_OBJS = \
	engine/client/vgui/vgui_draw.o \
	engine/client/avi/avi_stub.o \
	engine/client/cl_cmds.o \
	engine/client/cl_custom.o \
	engine/client/cl_debug.o \
	engine/client/cl_demo.o \
	engine/client/cl_efrag.o \
	engine/client/cl_efx.o \
	engine/client/cl_events.o \
	engine/client/cl_font.o \
	engine/client/cl_frame.o \
	engine/client/cl_game.o \
	engine/client/cl_gameui.o \
	engine/client/cl_main.o \
	engine/client/cl_netgraph.o \
	engine/client/cl_parse.o \
	engine/client/cl_parse_48.o \
	engine/client/cl_qparse.o \
	engine/client/cl_pmove.o \
	engine/client/cl_remap.o \
	engine/client/cl_render.o \
	engine/client/cl_scrn.o \
	engine/client/cl_securedstub.o \
	engine/client/cl_tent.o \
	engine/client/cl_video.o \
	engine/client/cl_view.o \
	engine/client/console.o \
	engine/client/gamma.o \
	engine/client/in_joy.o \
	engine/client/input.o \
	engine/client/keys.o \
	engine/client/mod_dbghulls.o \
	engine/client/ref_common.o \
	engine/client/s_dsp.o \
	engine/client/s_load.o \
	engine/client/s_main.o \
	engine/client/s_mix.o \
	engine/client/s_mouth.o \
	engine/client/s_stream.o \
	engine/client/s_utils.o \
	engine/client/s_vox.o \
	engine/client/titles.o \
	engine/client/vid_common.o \
	engine/client/voice.o \

XASH_OBJS = \
	engine/common/base_cmd.o \
	engine/common/cfgscript.o \
	engine/common/cmd.o \
	engine/common/common.o \
	engine/common/con_utils.o\
	engine/common/crashhandler.o \
	engine/common/custom.o \
	engine/common/cvar.o \
	engine/common/dedicated.o \
	engine/common/filesystem_engine.o \
	engine/common/host.o \
	engine/common/host_state.o\
	engine/common/hpak.o\
	engine/common/identification.o \
	engine/common/imagelib/img_bmp.o \
	engine/common/imagelib/img_dds.o \
	engine/common/imagelib/img_ktx2.o \
	engine/common/imagelib/img_main.o \
	engine/common/imagelib/img_png.o \
	engine/common/imagelib/img_quant.o \
	engine/common/imagelib/img_tga.o \
	engine/common/imagelib/img_utils.o \
	engine/common/imagelib/img_wad.o \
	engine/common/imagelib/img_pvr.o \
	engine/common/infostring.o \
	engine/common/ipv6text.o \
	engine/common/launcher.o \
	engine/common/lib_common.o \
	engine/common/masterlist.o \
	engine/common/mod_alias.o\
	engine/common/mod_bmodel.o\
	engine/common/mod_sprite.o \
	engine/common/mod_studio.o \
	engine/common/model.o \
	engine/common/net_buffer.o \
	engine/common/net_chan.o \
	engine/common/net_encode.o \
	engine/common/net_ws.o \
	engine/common/pm_debug.o \
	engine/common/pm_surface.o \
	engine/common/pm_trace.o \
	engine/common/soundlib/libmpg/dct36.o \
	engine/common/soundlib/libmpg/dct64.o \
	engine/common/soundlib/libmpg/format.o \
	engine/common/soundlib/libmpg/frame.o \
	engine/common/soundlib/libmpg/index.o \
	engine/common/soundlib/libmpg/layer3.o \
	engine/common/soundlib/libmpg/libmpg.o \
	engine/common/soundlib/libmpg/mpg123.o \
	engine/common/soundlib/libmpg/parse.o \
	engine/common/soundlib/libmpg/reader.o \
	engine/common/soundlib/libmpg/synth.o \
	engine/common/soundlib/libmpg/tabinit.o \
	engine/common/soundlib/snd_main.o \
	engine/common/soundlib/snd_mp3.o \
	engine/common/soundlib/snd_utils.o \
	engine/common/soundlib/snd_wav.o \
	engine/common/sounds.o \
	engine/common/sys_con.o \
	engine/common/system.o \
	engine/common/world.o \
	engine/common/zone.o \
	public/build.o \
	public/crclib.o \
	public/crtlib.o \
	public/matrixlib.o \
	public/utflib.o \
	public/xash3d_mathlib.o 

XASH_SERVER_OBJS =	\
	engine/server/sv_client.o \
	engine/server/sv_cmds.o \
	engine/server/sv_custom.o \
	engine/server/sv_filter.o \
	engine/server/sv_frame.o \
	engine/server/sv_game.o \
	engine/server/sv_init.o \
	engine/server/sv_log.o \
	engine/server/sv_main.o \
	engine/server/sv_move.o \
	engine/server/sv_phys.o \
	engine/server/sv_pmove.o \
	engine/server/sv_query.o \
	engine/server/sv_save.o \
	engine/server/sv_world.o \
	
XASH_PLATFORM_OBJS = \
	engine/platform/misc/lib_static.o \
	engine/platform/dreamcast/s_dc.o \
	engine/platform/dreamcast/sys_dc.o \
	engine/platform/dreamcast/vid_dc.o \
	engine/platform/dreamcast/in_dc.o \



INCLUDE = -Icommon \
-Iengine/server \
-Iengine/client/vgui \
-Iengine/client/avi \
-Iengine/client \
-Iengine \
-Iengine/common \
-Iimagelib \
-Ifilesystem \
-Ipublic \
-Ipm_shared \
-Iengine/platform \
-Iengine/platform/dreamcast \
-I$(KOS_PORTS)/include/opus \
-I$(KOS_PORTS)/include/GL \


DEFINES = -DENGINE_DLL -D_KOS_ -D_SH4_ -DXASH_BUILD_COMMIT="64726f13-dirty" -DXASH_BUILD_BRANCH="master" -DFRAME_POINTERS=1 -DXASH_STATIC_LIBS=1 -DXASH_LOW_MEMORY=2 -DXASH_ENABLE_MAIN=1 -DXASH_REF_SOFT_ENABLED=0  -DXASH_REF_GL_ENABLED=1 -DHAVE_TGMATH_H=0 -DHAVE_STRNICMP=1 -DHAVE_STRICMP=1 -D_snprintf=snprintf
FLAGS = -Os -ffast-math -fnoomit-frame-pointer -fno-common -fno-builtin -fno-strict-aliasing -fno-stack-protector -mrelax -ffunction-sections -fdata-sections -fno-exceptions
 

CFLAGS +=  $(INCLUDE) $(DEFINES) $(FLAGS)  
