#include "Window.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <iostream>

#include <epoxy/gl.h>

#include <SDL2/SDL.h>

#include <Gwen/Controls/WindowControl.h>
#include <Gwen/Controls/Button.h>
#include <Gwen/Controls/CheckBox.h>

#include <engine/GWENInput.hpp>
#include <engine/renderer/GWENRenderer.hpp>
#include <engine/env/Environment.hpp>
#include <engine/env/System.hpp>

namespace glPortal {

const int Window::DEFAULT_WIDTH = 800;
const int Window::DEFAULT_HEIGHT = 600;
const char* Window::DEFAULT_TITLE = "GlPortal";

Window::Window() :
  width(0),
  height(0),
  window(nullptr) {
}

Window::~Window() = default;

void Window::initEpoxy() {
  System::Log() << "GL " << epoxy_gl_version();
}

void Window::create(const char *title) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  Config &config = Environment::getConfig();

  if (config.getAntialiasLevel() > 0) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.getAntialiasLevel());
  }
  
  int flags = SDL_WINDOW_OPENGL;
  if (config.isFullscreen()) {
    flags |= SDL_WINDOW_BORDERLESS;
  }

  SDL_DisplayMode dispMode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
  SDL_GetDesktopDisplayMode(0, &dispMode);

  int width = config.getWidth();
  int height = config.getHeight();

  if (width == 0) {
    width = dispMode.w;
  }
  if (height == 0) {
    height = dispMode.h;
  }

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      width, height, flags);

  context = SDL_GL_CreateContext(window);

  initEpoxy();

  this->width = width;
  this->height = height;

  glViewport(0, 0, width, height);

  // Allows unbound framerate if vsync is disabled
  SDL_GL_SetSwapInterval(config.hasVsync() ? 1 : 0);

  // Lock cursor in the middle of the screen
  lockMouse();

  gwenRenderer = std::make_unique<GWENRenderer>();
  gwenSkin = std::make_unique<Gwen::Skin::TexturedBase>(gwenRenderer.get());
  gwenSkin->Init((Environment::getDataDir() + "/gui/DefaultSkin.png").c_str());
  gwenCanvas = std::make_unique<Gwen::Controls::Canvas>(gwenSkin.get());
  gwenInput = std::make_unique<GWENInput>();
  gwenRenderer->Init();
  gwenInput->init(gwenCanvas.get());

#if 1 // Testing code
  Gwen::Controls::WindowControl *win = new Gwen::Controls::WindowControl(gwenCanvas.get());
  win->SetBounds( 30, 30, 300, 200 );
  Gwen::Controls::Button* pButtonA = new Gwen::Controls::Button(win);
  pButtonA->SetText("ABCDEFGHIJKL");
    Gwen::Controls::CheckBox* pCA = new Gwen::Controls::CheckBox(win);
  pCA->SetText("abc");
  pCA->SetPos(40, 40);
  pCA->SetChecked(true);
  //pCA->SetBounds( 30, 30, 100, 30);
#endif
}

void Window::setFullscreen() {
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Window::swapBuffers() {
  SDL_GL_SwapWindow(window);
}

void Window::getSize(int *width, int *height) const {
  SDL_GetWindowSize(window, width, height);
}

void Window::close() {
  SDL_HideWindow(window);

  gwenInput.release();
  gwenCanvas.release();
  gwenSkin.release();
  gwenRenderer.release();

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  window = nullptr;

  SDL_Quit();
}

void Window::lockMouse() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Window::unlockMouse() {
  SDL_SetRelativeMouseMode(SDL_FALSE);
}

} /* namespace glPortal */
