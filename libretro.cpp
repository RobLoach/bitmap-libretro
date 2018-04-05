#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "libretro.h"

static uint32_t *frame_buf;


void retro_init(void)
{
   frame_buf = (uint32_t*) calloc(320 * 240, sizeof(uint32_t));
}

void retro_deinit(void)
{
   free(frame_buf);
   frame_buf = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   //log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "TestCore";
   info->library_version  = "v1";
   info->need_fullpath    = false;
   info->valid_extensions = NULL; // Anything is fine, we don't care.
}

static retro_video_refresh_t video_cb;
static retro_environment_t environ_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   float aspect = 4.0f / 3.0f;
   float sampling_rate = 30000.0f;

   info->timing = (struct retro_system_timing) {
      .fps = 60.0,
      .sample_rate = sampling_rate,
   };

   info->geometry = (struct retro_game_geometry) {
      .base_width   = 320,
      .base_height  = 240,
      .max_width    = 320,
      .max_height   = 240,
      .aspect_ratio = aspect,
   };
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   bool no_content = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);

}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   //audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   //audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   //input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   //input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}


void retro_reset(void)
{
}

static void update_input(void)
{
   //input_poll_cb();
   //if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
   //{
      /* stub */
   //}
}

static void render_checkered(void)
{
   unsigned y_coord, x_coord;
   uint32_t *buf    = frame_buf;
   unsigned stride  = 320;
   uint32_t color_r = 0xff << 16;
   uint32_t color_g = 0xff <<  8;
   uint32_t *line   = buf;

   for (unsigned y = 0; y < 240; y++, line += stride)
   {
      unsigned index_y = ((y - y_coord) >> 4) & 1;
      for (unsigned x = 0; x < 320; x++)
      {
         unsigned index_x = ((x - x_coord) >> 4) & 1;
         line[x] = (index_y ^ index_x) ? color_r : color_g;
      }
   }

   /*for (unsigned y = mouse_rel_y - 5; y <= mouse_rel_y + 5; y++)
      for (unsigned x = mouse_rel_x - 5; x <= mouse_rel_x + 5; x++)
         buf[y * stride + x] = 0xff;*/

   for (unsigned y = 0; y < 240; y++) {
      for (unsigned x = 0; x < 320; x++) {
         buf[y * 320 + x] = 0xff;
      }
   }

   video_cb(buf, 320, 240, stride << 2);
}

static void check_variables(void)
{
}

static void audio_callback(void)
{
   //audio_cb(0, 0);
}

void retro_run(void)
{
   render_checkered();

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();
}

bool retro_load_game(const struct retro_game_info *info)
{
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      //log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
      return false;
   }

   check_variables();

   (void)info;
   return true;
}

void retro_unload_game(void)
{
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   return retro_load_game(NULL);
}

size_t retro_serialize_size(void)
{
   return 2;
}

bool retro_serialize(void *data_, size_t size)
{
   return true;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}
