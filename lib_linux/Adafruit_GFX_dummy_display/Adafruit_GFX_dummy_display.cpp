/*
 * Copyright (c) 2019, Vincent Hervieux vincent.hervieux@gmail.com
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the name of the author Vincent Hervieux, nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <Adafruit_GFX_dummy_display.h>

Adafruit_GFX_dummy_display::Adafruit_GFX_dummy_display(uint16_t w, uint16_t h, uint16_t zoom) :
Adafruit_GFX(w, h),
window_(NULL),
renderer_(NULL),
zoom_ratio_(zoom)
{
   initWindowAndRendered(w, h);
}

Adafruit_GFX_dummy_display::~Adafruit_GFX_dummy_display()
{
   if(NULL != renderer_)
      SDL_DestroyRenderer(renderer_);
   if(NULL != window_)
      SDL_DestroyWindow(window_);
   SDL_Quit();
}

void Adafruit_GFX_dummy_display::display(void)
{
   SDL_RenderPresent(renderer_);
}

void Adafruit_GFX_dummy_display::clearDisplay(void)
{
   SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
   SDL_RenderClear(renderer_);
}

void Adafruit_GFX_dummy_display::drawPixel(int16_t x, int16_t y, uint16_t color)
{
   setColor(color);
   if(zoom_ratio_ == 1)
   {
      if(0 != SDL_RenderDrawPoint(renderer_, x, y))
      {
         fprintf(stderr, "Erreur SDL_SetRenderDrawPoint : %s", SDL_GetError());
      }
   }
   else
   {
      SDL_Rect r;
      r.x = x*zoom_ratio_ - (zoom_ratio_/2);
      r.y = y*zoom_ratio_ - (zoom_ratio_/2);
      r.w = zoom_ratio_;
      r.h = zoom_ratio_;
      if(0 != SDL_RenderFillRect(renderer_, &r))
      {
         fprintf(stderr, "Erreur SDL_SetRenderDrawRect : %s", SDL_GetError());
      }
   }
}

void Adafruit_GFX_dummy_display::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
   setColor(color);
   SDL_Rect r;
   r.x = x*zoom_ratio_;
   r.y = y*zoom_ratio_;
   r.w = zoom_ratio_;
   r.h = h*zoom_ratio_;
   if(0 != SDL_RenderFillRect(renderer_, &r))
   {
      fprintf(stderr, "Erreur SDL_SetRenderDrawRect : %s", SDL_GetError());
   }
}

void Adafruit_GFX_dummy_display::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
   setColor(color);
   SDL_Rect r;
   r.x = x*zoom_ratio_;
   r.y = y*zoom_ratio_;
   r.w = w*zoom_ratio_;
   r.h = zoom_ratio_;
   if(0 != SDL_RenderFillRect(renderer_, &r))
   {
      fprintf(stderr, "Erreur SDL_SetRenderDrawRect : %s", SDL_GetError());
   }
}

void Adafruit_GFX_dummy_display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
   setColor(color);
   SDL_Rect r;
   r.x = x*zoom_ratio_;
   r.y = y*zoom_ratio_;
   r.w = w*zoom_ratio_;
   r.h = h*zoom_ratio_;
   if(0 != SDL_RenderFillRect(renderer_, &r))
   {
      fprintf(stderr, "Erreur SDL_SetRenderDrawRect : %s", SDL_GetError());
   }
}

void Adafruit_GFX_dummy_display::fillScreen(uint16_t color)
{
   setColor(color);
}

void Adafruit_GFX_dummy_display::initWindowAndRendered(uint16_t w, uint16_t h)
{
   /* Init window and, création de la fenêtre et du renderer. */
   if(0 != SDL_Init(SDL_INIT_VIDEO))
   {
      fprintf(stderr, "Error SDL_Init : %s", SDL_GetError());
      goto Quit;
   }
   window_ = SDL_CreateWindow("Adafruit_GFX dummy display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           w * zoom_ratio_, h * zoom_ratio_, SDL_WINDOW_SHOWN);
   if(NULL == window_)
   {
      fprintf(stderr, "Error SDL_CreateWindow : %s", SDL_GetError());
      goto Quit;
   }
   renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
   if(NULL == renderer_)
   {
      fprintf(stderr, "Error SDL_CreateRenderer : %s", SDL_GetError());
      goto Quit;
   }
   return;
Quit:
    if(NULL != renderer_)
        SDL_DestroyRenderer(renderer_);
    if(NULL != window_)
        SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Adafruit_GFX_dummy_display::setColor(uint16_t color)
{
   static const SDL_Color white  = {255, 255, 255, 255};
   static const SDL_Color black  = {  0,   0,   0, 255};
   static const SDL_Color red    = {255,   0,   0, 255};
   static const SDL_Color orange = {255, 127,  40, 255};
   static const SDL_Color yellow = {255, 255,   0, 255};
   static const SDL_Color green  = {  0, 128,   0, 255};
   static const SDL_Color blue   = {  0,   0, 255, 255};
   const SDL_Color *c;
   switch (color)
   {
   case WHITE:
      c = &white;
      break;
   case BLACK:
      c = &black;
      break;
   case RED:
      c = &red;
      break;
   case ORANGE:
      c = &orange;
      break;
   case YELLOW:
      c = &yellow;
      break;
   case GREEN:
      c = &green;
      break;
   case BLUE:
      c = &blue;
      break;
   default:
      fprintf(stderr, "Unhandled color %hu\n", color);
      c = &white;
      break;
   }
   SDL_SetRenderDrawColor(renderer_, c->r, c->g, c->b, c->a);
}

