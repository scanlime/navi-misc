if HAVE_SDL
if LINUX
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	SDLJoystick.cpp				\
	SDLJoystick.h				\
	SDLPlatformFactory.cpp			\
	SDLPlatformFactory.h			\
	SDLMedia.cpp				\
	SDLMedia.h				\
	SDLDisplay.h				\
	SDLDisplay.cpp
endif
else
if LINUX
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	wave.cpp				\
	wave.h					\
	LinuxPlatformFactory.cpp		\
	LinuxPlatformFactory.h			\
	LinuxDisplay.cpp			\
	LinuxDisplay.h				\
	LinuxMedia.cpp				\
	LinuxMedia.h				\
	USBJoystick.cpp				\
	USBJoystick.h				\
	XIJoystick.cpp				\
	XIJoystick.h				\
	XDisplay.cpp				\
	XDisplay.h				\
	XVisual.cpp				\
	XVisual.h				\
	XWindow.cpp				\
	XWindow.h
endif
endif

if SOLARIS
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	wave.cpp				\
	wave.h					\
	SolarisPlatformFactory.cpp		\
	SolarisPlatformFactory.h		\
	SolarisMedia.cpp			\
	SolarisMedia.h				\
	XDisplay.cpp				\
	XDisplay.h				\
	XVisual.cpp				\
	XVisual.h				\
	XWindow.cpp				\
	XWindow.h
endif

if IRIX
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	wave.cpp				\
	wave.h					\
	SGIPlatformFactory.cpp			\
	SGIPlatformFactory.h			\
	SGIDisplay.cpp				\
	SGIDisplay.h				\
	SGIMedia.cpp				\
	SGIMedia.h				\
	XDisplay.cpp				\
	XDisplay.h				\
	XVisual.cpp				\
	XVisual.h				\
	XWindow.cpp				\
	XWindow.h
endif

if MACOS
SUBDIRS = MacOSX

if HAVE_SDL
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	SDLJoystick.cpp				\
	SDLJoystick.h				\
	SDLMedia.cpp				\
	SDLMedia.h				\
	SDLDisplay.h				\
	SDLDisplay.cpp				\
	wave.cpp				\
	wave.h
else
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	wave.cpp				\
	wave.h
endif
endif
DIST_SUBDIRS = MacOSX

if BEOS
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	wave.cpp				\
	wave.h					\
	BeOSPlatformFactory.cpp			\
	BeOSPlatformFactory.h			\
	BeOSDisplay.cpp				\
	BeOSDisplay.h				\
	BeOSVisual.cpp				\
	BeOSVisual.h				\
	BeOSWindow.cpp				\
	BeOSWindow.h				\
	BeOSMedia.cpp				\
	BeOSMedia.h
endif

if WIN32
libPlatform_a_SOURCES =				\
	PlatformFactory.cpp			\
	BzfDisplay.cpp				\
	BzfJoystick.cpp				\
	BzfVisual.cpp				\
	BzfWindow.cpp				\
	BzfMedia.cpp				\
	SDLDisplay.h				\
	SDLDisplay.cpp				\
	SDLJoystick.h				\
	SDLJoystick.cpp				\
	SDLMedia.cpp				\
	SDLMedia.h				\
	wave.cpp				\
	wave.h					\
	WinPlatformFactory.cpp			\
	WinPlatformFactory.h			\
	WinDisplay.cpp				\
	WinDisplay.h				\
	WinJoystick.cpp				\
	WinJoystick.h				\
	WinVisual.cpp				\
	WinVisual.h				\
	WinWindow.cpp				\
	WinWindow.h				\
	WinMedia.cpp				\
	WinMedia.h
endif

if CLIENT_INCLUDED
noinst_LIBRARIES = libPlatform.a
endif

EXTRA_libPlatform_a_SOURCES =		\
	BeOSPlatformFactory.cpp		\
	BeOSPlatformFactory.h		\
	BeOSDisplay.cpp			\
	BeOSDisplay.h			\
	BeOSVisual.cpp			\
	BeOSVisual.h			\
	BeOSWindow.cpp			\
	BeOSWindow.h			\
	BeOSWindow2.cpp			\
	BeOSMedia.cpp			\
	BeOSMedia.h			\
	LinuxPlatformFactory.cpp	\
	LinuxPlatformFactory.h		\
	LinuxDisplay.cpp		\
	LinuxDisplay.h			\
	LinuxMedia.cpp			\
	LinuxMedia.h			\
	SDLJoystick.cpp			\
	SDLJoystick.h			\
	SDLPlatformFactory.cpp		\
	SDLPlatformFactory.h		\
	SDLMedia.cpp			\
	SDLMedia.h			\
	SDLDisplay.h			\
	SDLDisplay.cpp			\
	SGIPlatformFactory.cpp		\
	SGIPlatformFactory.h		\
	SGIDisplay.cpp			\
	SGIDisplay.h			\
	SGIMedia.cpp			\
	SGIMedia.h			\
	SolarisPlatformFactory.cpp	\
	SolarisPlatformFactory.h	\
	SolarisMedia.cpp		\
	SolarisMedia.h			\
	USBJoystick.cpp			\
	USBJoystick.h			\
	WinPlatformFactory.cpp		\
	WinPlatformFactory.h		\
	WinDisplay.cpp			\
	WinDisplay.h			\
	WinJoystick.cpp			\
	WinJoystick.h			\
	WinVisual.cpp			\
	WinVisual.h			\
	WinWindow.cpp			\
	WinWindow.h			\
	WinMedia.cpp			\
	WinMedia.h			\
	XDisplay.cpp			\
	XDisplay.h			\
	XIJoystick.cpp			\
	XIJoystick.h			\
	XVisual.cpp			\
	XVisual.h			\
	XWindow.cpp			\
	XWindow.h			\
	wave.cpp			\
	wave.h

AM_CPPFLAGS = $(SDL_CFLAGS)

EXTRA_DIST = \
	NewMedia.cpp \
	NewMedia.h \
	README

MAINTAINERCLEANFILES = \
	Makefile.in
