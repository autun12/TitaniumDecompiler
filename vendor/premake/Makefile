# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
	@echo Regenerating project files with Premake5!
	.bin/premake5 gmake2 --file="../..premake5.lua"
endef

# File sets
# #############################################


# Rules
# #############################################

all: $(TARGETDIR) $(TARGET)
	@:

$(TARGET): 
	$(PREBUILDCMDS)
	$(PRELINKCMDS)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

clean:
	@echo Cleaning Premake


# File Rules
# #############################################
