#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <math.h>

#include "libretro.h"
#include "vendor/bitmap/bitmap_image.hpp"

static uint32_t *frame_buf;
static bitmap_image* image;
static bitmap_image* screen;
float x_coord = 0.0f;
float y_coord = 0.0f;

void retro_init(void)
{
   int width = 640;
   int height = 480;
   frame_buf = (uint32_t*) calloc(width * height, sizeof(uint32_t));

   image = new bitmap_image("image.bmp");
   screen = new bitmap_image(width, height);

   screen->clear(0);
}

void retro_deinit(void)
{
   delete image;
   free(frame_buf);
   delete screen;
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
      .base_width   = 640,
      .base_height  = 480,
      .max_width    = 640,
      .max_height   = 480,
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

static void draw(void)
{
   uint32_t *buf    = frame_buf;
   bitmap_image::rgb_t colour;

   // Clear the screen.
   screen->clear(0);

   // Display a Fractal.
   double    cr,    ci;
   double nextr, nexti;
   double prevr, previ;
   const unsigned int max_iterations = 50;
   for (unsigned int y = 0; y < screen->height(); ++y) {
      for (unsigned int x = 0; x < screen->width(); ++x) {
         cr = 1.5 * (2.0 * x / screen->width () - 1.0) - 0.5;
         ci =       (2.0 * y / screen->height() - 1.0);
         nextr = nexti = 0;
         prevr = previ = 0;
         for (unsigned int i = 0; i < max_iterations; ++i) {
            prevr = nextr;
            previ = nexti;
            nextr =     prevr * prevr - previ * previ + cr;
            nexti = 2 * prevr * previ + ci;
            if (((nextr * nextr) + (nexti * nexti)) > 4) {
               using namespace std;
               const double z = sqrt(nextr * nextr + nexti * nexti);
               //https://en.wikipedia.org/wiki/Mandelbrot_set#Continuous_.28smooth.29_coloring
               const unsigned int index = static_cast<unsigned int>
                  (1000.0 * log2(1.75 + i - log2(log2(z))) / log2(max_iterations));
               screen->set_pixel(x, y, jet_colormap[index]);
               break;
            }
         }
      }
   }

   // Blit the image.
   for (unsigned y = 0; y < image->height(); y++) {
      for (unsigned x = 0; x < image->width(); x++) {
         colour = image->get_pixel(x, y);
         screen->set_pixel(x_coord + x, y_coord + y, colour);
      }
   }

   // Flip the sreen.
   for (unsigned y = 0; y < screen->height(); y++) {
      for (unsigned x = 0; x < screen->width(); x++) {
         colour = screen->get_pixel(x, y);
         buf[y * screen->width() + x] = (uint32_t)((255 << 24) |
            (colour.red << 16) |
            (colour.green << 8) |
            (colour.blue << 0));
      }
   }

   // Display the screen.
   video_cb(buf, screen->width(), screen->height(), screen->width() << 2);
}

static void check_variables(void)
{
}

void retro_run(void)
{
   x_coord += 0.5f;
   y_coord += 0.2f;
   draw();

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
