# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  Premake_config = debug
  GLFW_config = debug
  Glad_config = debug
  ImGui_config = debug
  TitaniumRenderer_config = debug
  TitaniumDecompiler_config = debug
  TitaniumApplication_config = debug

else ifeq ($(config),release)
  Premake_config = release
  GLFW_config = release
  Glad_config = release
  ImGui_config = release
  TitaniumRenderer_config = release
  TitaniumDecompiler_config = release
  TitaniumApplication_config = release

else ifeq ($(config),dist)
  Premake_config = dist
  GLFW_config = dist
  Glad_config = dist
  ImGui_config = dist
  TitaniumRenderer_config = dist
  TitaniumDecompiler_config = dist
  TitaniumApplication_config = dist

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := Premake GLFW Glad ImGui TitaniumRenderer TitaniumDecompiler TitaniumApplication

.PHONY: all clean help $(PROJECTS) Dependencies

all: $(PROJECTS)

Dependencies: GLFW Glad ImGui Premake

Premake:
ifneq (,$(Premake_config))
	@echo "==== Building Premake ($(Premake_config)) ===="
	@${MAKE} --no-print-directory -C vendor/premake -f Makefile config=$(Premake_config)
endif

GLFW:
ifneq (,$(GLFW_config))
	@echo "==== Building GLFW ($(GLFW_config)) ===="
	@${MAKE} --no-print-directory -C TitaniumRenderer/vendor/GLFW -f Makefile config=$(GLFW_config)
endif

Glad:
ifneq (,$(Glad_config))
	@echo "==== Building Glad ($(Glad_config)) ===="
	@${MAKE} --no-print-directory -C TitaniumRenderer/vendor/Glad -f Makefile config=$(Glad_config)
endif

ImGui:
ifneq (,$(ImGui_config))
	@echo "==== Building ImGui ($(ImGui_config)) ===="
	@${MAKE} --no-print-directory -C TitaniumRenderer/vendor/imgui -f Makefile config=$(ImGui_config)
endif

TitaniumRenderer: GLFW Glad ImGui
ifneq (,$(TitaniumRenderer_config))
	@echo "==== Building TitaniumRenderer ($(TitaniumRenderer_config)) ===="
	@${MAKE} --no-print-directory -C TitaniumRenderer -f Makefile config=$(TitaniumRenderer_config)
endif

TitaniumDecompiler:
ifneq (,$(TitaniumDecompiler_config))
	@echo "==== Building TitaniumDecompiler ($(TitaniumDecompiler_config)) ===="
	@${MAKE} --no-print-directory -C TitaniumDecompiler -f Makefile config=$(TitaniumDecompiler_config)
endif

TitaniumApplication: TitaniumRenderer GLFW Glad ImGui
ifneq (,$(TitaniumApplication_config))
	@echo "==== Building TitaniumApplication ($(TitaniumApplication_config)) ===="
	@${MAKE} --no-print-directory -C TitaniumApplication -f Makefile config=$(TitaniumApplication_config)
endif

clean:
	@${MAKE} --no-print-directory -C vendor/premake -f Makefile clean
	@${MAKE} --no-print-directory -C TitaniumRenderer/vendor/GLFW -f Makefile clean
	@${MAKE} --no-print-directory -C TitaniumRenderer/vendor/Glad -f Makefile clean
	@${MAKE} --no-print-directory -C TitaniumRenderer/vendor/imgui -f Makefile clean
	@${MAKE} --no-print-directory -C TitaniumRenderer -f Makefile clean
	@${MAKE} --no-print-directory -C TitaniumDecompiler -f Makefile clean
	@${MAKE} --no-print-directory -C TitaniumApplication -f Makefile clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo "  dist"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   Premake"
	@echo "   GLFW"
	@echo "   Glad"
	@echo "   ImGui"
	@echo "   TitaniumRenderer"
	@echo "   TitaniumDecompiler"
	@echo "   TitaniumApplication"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"