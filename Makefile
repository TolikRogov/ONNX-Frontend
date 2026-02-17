# Makefile for managing CMake assemblies

# Colors for output
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[1;33m
BLUE := \033[0;34m
MAGENTA := \033[0;35m
CYAN := \033[0;36m
WHITE := \033[1;37m
BOLD := \033[1m
NC := \033[0m # No Color

# Loading .env file if it exist
ifneq (,$(wildcard ./.env))
	include .env
	export
endif

# Project configurations
PROJECT_NAME := MyProject
BUILD_DIR := build
SOURCE_DIR := src
DEBUG_BUILD_DIR := $(BUILD_DIR)/Debug
RELEASE_BUILD_DIR := $(BUILD_DIR)/Release
INSTALL_DIR := /usr/local/bin
PROJECT_ROOT := $(shell pwd)

# Cmake flags
CMAKE := cmake
CMAKE_FLAGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Default targets
.DEFAULT_GOAL := default

# Default target
.PHONY: default
default: env help

# Show env variables from .env file
.PHONY: env
env:
	@echo "$(CYAN)=== Environment Variables ===$(NC)"
	@if [ -f .env ]; then \
		echo "$(GREEN)Loading .env file...$(NC)"; \
		echo "$(YELLOW)Available variables:$(NC)"; \
		grep -v '^#' .env | grep -v '^$$' | while read line; do \
			var=$$(echo "$$line" | cut -d'=' -f1); \
			value=$$(echo "$$line" | cut -d'=' -f2-); \
			echo "  $(BOLD)$$var$(NC)=$(CYAN)$$value$(NC)"; \
		done; \
	else \
		echo "$(RED)No .env file found$(NC)"; \
	fi

# Show help
.PHONY: help
help:
	@echo "$(CYAN)=== Available targets ===$(NC)"
	@echo "  $(GREEN)debug$(NC)     		- Build debug version"
	@echo "  $(GREEN)release$(NC)   		- Build release version"
	@echo "  $(GREEN)all$(NC)       		- Build both versions"
	@echo "  $(YELLOW)configure-debug$(NC) 	- Config debug version"
	@echo "  $(YELLOW)configure-release$(NC) 	- Config release version"
	@echo "  $(YELLOW)configure$(NC)    	- Configure both options"
	@echo "  $(MAGENTA)run-debug$(NC)    	- Run debug version"
	@echo "  $(MAGENTA)run-release$(NC)  	- Run release version"
	@echo "  $(BLUE)list-src$(NC)		- Show source files"
	@echo "  $(BLUE)build-src$(NC)     	- Build single file (usage: make build-src FILE=filename)"
	@echo "  $(BLUE)run-src$(NC)     		- Build and run single file (usage: make run-src FILE=filename [ARGS=\"arguments\"])"
	@echo "  $(BLUE)run-src-debug$(NC)     	- Build and run single file in debug mode (usage: make run-src-debug FILE=filename [ARGS=\"arguments\"])"
	@echo "  $(BLUE)run-src-release$(NC)     	- Build and run single file in release mode (usage: make run-src-release FILE=filename [ARGS=\"arguments\"])"
	@echo "  $(RED)clean-debug$(NC)  	- Clear debug assemble"
	@echo "  $(RED)clean-release$(NC) 	- Clear release assemble"
	@echo "  $(RED)clean$(NC)     		- Clear all assemblies"
	@echo "  $(YELLOW)rebuild-debug$(NC)  	- Rebuild debug version"
	@echo "  $(YELLOW)rebuild-release$(NC) 	- Rebuild release version"
	@echo "  $(YELLOW)rebuild$(NC)     		- Rebuild both versions"
	@echo "  $(CYAN)env$(NC)         		- Show environment variables"
	@echo "  $(WHITE)help$(NC)         	- Show help"

# Debug assemble
.PHONY: debug
debug: configure-debug
	@echo "$(YELLOW)Build debug version...$(NC)"
	@$(CMAKE) --build $(DEBUG_BUILD_DIR) --config Debug
	@echo "$(GREEN)✓ Debug build completed successfully$(NC)"

# Release assemble
.PHONY: release
release: configure-release
	@echo "$(YELLOW)Build release version...$(NC)"
	@$(CMAKE) --build $(RELEASE_BUILD_DIR) --config Release
	@echo "$(GREEN)✓ Release build completed successfully$(NC)"

# Both assemblies
.PHONY: all
all: debug release
	@echo "$(GREEN)✓ All builds completed successfully$(NC)"

# Config debug version
.PHONY: configure-debug
configure-debug:
	@echo "$(BLUE)Config debug version...$(NC)"
	@mkdir -p $(DEBUG_BUILD_DIR)
	@cd $(DEBUG_BUILD_DIR) && $(CMAKE) $(PROJECT_ROOT) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_FLAGS)
	@echo "$(GREEN)✓ Debug configuration completed$(NC)"

# Config release version
.PHONY: configure-release
configure-release:
	@echo "$(BLUE)Config release version...$(NC)"
	@mkdir -p $(RELEASE_BUILD_DIR)
	@cd $(RELEASE_BUILD_DIR) && $(CMAKE) $(PROJECT_ROOT) -DCMAKE_BUILD_TYPE=Release $(CMAKE_FLAGS)
	@echo "$(GREEN)✓ Release configuration completed$(NC)"

# Config both versions
.PHONY: configure
configure: configure-debug configure-release
	@echo "$(GREEN)✓ All configurations completed$(NC)"

# Run debug version
.PHONY: run-debug
run-debug: debug
	@echo "$(MAGENTA)Run debug version...$(NC)"
	@./$(DEBUG_BUILD_DIR)/$(PROJECT_NAME) $(ARGS)

# Run release version
.PHONY: run-release
run-release: release
	@echo "$(MAGENTA)Run release version...$(NC)"
	@./$(RELEASE_BUILD_DIR)/$(PROJECT_NAME) $(ARGS)

# Show available source files in src
.PHONY: list-src
list-src:
	@echo "$(CYAN)=== Available source files in $(SOURCE_DIR)/ ===$(NC)"
	@if [ -d "$(SOURCE_DIR)" ]; then \
		for file in $(SOURCE_DIR)/*.cpp $(SOURCE_DIR)/*.c; do \
			if [ -f "$$file" ]; then \
				filename=$$(basename "$$file"); \
				echo "  $(GREEN)$$filename$(NC)"; \
			fi; \
		done; \
	else \
		echo "$(RED)Source directory $(SOURCE_DIR) not found$(NC)"; \
	fi

# Build single file using CMake
.PHONY: build-src
build-src: configure-debug
	@if [ -z "$(FILE)" ]; then \
		echo "$(RED)Error: Please specify FILE parameter (filename without extension)$(NC)"; \
		echo "$(YELLOW)Usage: make build-src FILE=filename$(NC)"; \
		echo ""; \
		echo "$(CYAN)Available source files:$(NC)"; \
		$(MAKE) list-src; \
		exit 1; \
	fi
	@echo "$(YELLOW)Building $(FILE) with CMake...$(NC)"
	@if cd $(DEBUG_BUILD_DIR) && $(CMAKE) --build . --target $(FILE); then \
		echo "$(GREEN)✓ Successfully built $(FILE)$(NC)"; \
	else \
		echo "$(RED)✗ Failed to build $(FILE) with CMake$(NC)"; \
		exit 1; \
	fi

# Build single file in release mode
.PHONY: build-src-release
build-src-release: configure-release
	@if [ -z "$(FILE)" ]; then \
		echo "$(RED)Error: Please specify FILE parameter (filename without extension)$(NC)"; \
		echo "$(YELLOW)Usage: make build-src-release FILE=filename$(NC)"; \
		echo ""; \
		echo "$(CYAN)Available source files:$(NC)"; \
		$(MAKE) list-src; \
		exit 1; \
	fi
	@echo "$(YELLOW)Building $(FILE) in release mode with CMake...$(NC)"
	@if cd $(RELEASE_BUILD_DIR) && $(CMAKE) --build . --target $(FILE); then \
		echo "$(GREEN)✓ Successfully built $(FILE) in release mode$(NC)"; \
	else \
		echo "$(RED)✗ Failed to build $(FILE) with CMake$(NC)"; \
		exit 1; \
	fi

# Run single file using CMake (debug mode by default)
.PHONY: run-src
run-src: build-src
	@echo "$(MAGENTA)Running $(FILE) with arguments: $(ARGS)$(NC)"
	@./$(DEBUG_BUILD_DIR)/$(FILE) $(ARGS)

# Run single file in debug mode with arguments
.PHONY: run-src-debug
run-src-debug: build-src
	@echo "$(MAGENTA)Running $(FILE) in debug mode with arguments: $(ARGS)$(NC)"
	@./$(DEBUG_BUILD_DIR)/$(FILE) $(ARGS)

# Run single file in release mode with arguments
.PHONY: run-src-release
run-src-release: build-src-release
	@echo "$(MAGENTA)Running $(FILE) in release mode with arguments: $(ARGS)$(NC)"
	@./$(RELEASE_BUILD_DIR)/$(FILE) $(ARGS)

# Clear debug assemble
.PHONY: clean-debug
clean-debug:
	@echo "$(RED)Clear debug assemble...$(NC)"
	@rm -rf $(DEBUG_BUILD_DIR)
	@echo "$(GREEN)✓ Debug assemble cleared$(NC)"

# Clear release assemble
.PHONY: clean-release
clean-release:
	@echo "$(RED)Clear release assemble...$(NC)"
	@rm -rf $(RELEASE_BUILD_DIR)
	@echo "$(GREEN)✓ Release assemble cleared$(NC)"

# Complete clean
.PHONY: clean
clean: clean-debug clean-release
	@echo "$(RED)Complete clean...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✓ All assemblies cleared$(NC)"

# Debug reassembly
.PHONY: rebuild-debug
rebuild-debug: clean-debug debug
	@echo "$(GREEN)✓ Debug rebuild completed$(NC)"

# Release reassembly
.PHONY: rebuild-release
rebuild-release: clean-release release
	@echo "$(GREEN)✓ Release rebuild completed$(NC)"

# Complete reassembly
.PHONY: rebuild
rebuild: clean all
	@echo "$(GREEN)✓ Complete rebuild completed$(NC)"

# Show information about assemblies
.PHONY: info
info:
	@echo "$(CYAN)=== Project information ===$(NC)"
	@echo "$(BOLD)Project name:$(NC) $(GREEN)$(PROJECT_NAME)$(NC)"
	@echo "$(BOLD)Debug directory:$(NC) $(BLUE)$(DEBUG_BUILD_DIR)$(NC)"
	@echo "$(BOLD)Release directory:$(NC) $(BLUE)$(RELEASE_BUILD_DIR)$(NC)"
	@echo "$(BOLD)CMake flags:$(NC) $(YELLOW)$(CMAKE_FLAGS)$(NC)"
	@if [ -d "$(DEBUG_BUILD_DIR)" ]; then \
		echo "$(BOLD)Debug assemble:$(NC) $(GREEN)exists$(NC)"; \
	else \
		echo "$(BOLD)Debug assemble:$(NC) $(RED)not configured$(NC)"; \
	fi
	@if [ -d "$(RELEASE_BUILD_DIR)" ]; then \
		echo "$(BOLD)Release assemble:$(NC) $(GREEN)exists$(NC)"; \
	else \
		echo "$(BOLD)Release assemble:$(NC) $(RED)not configured$(NC)"; \
	fi

# Create symlinks
.PHONY: links
links:
	@echo "$(YELLOW)Creating symlinks...$(NC)"
	@ln -sf $(DEBUG_BUILD_DIR)/$(PROJECT_NAME) ./debug-$(PROJECT_NAME) 2>/dev/null || true
	@ln -sf $(RELEASE_BUILD_DIR)/$(PROJECT_NAME) ./release-$(PROJECT_NAME) 2>/dev/null || true
	@echo "$(GREEN)Created symlinks:$(NC)"
	@echo "  $(BOLD)debug-$(PROJECT_NAME)$(NC) -> $(BLUE)$(DEBUG_BUILD_DIR)/$(PROJECT_NAME)$(NC)"
	@echo "  $(BOLD)release-$(PROJECT_NAME)$(NC) -> $(BLUE)$(RELEASE_BUILD_DIR)/$(PROJECT_NAME)$(NC)"

# Delete links
.PHONY: clean-links
clean-links:
	@echo "$(RED)Cleaning symbolic links...$(NC)"
	@-rm -f ./debug-$(PROJECT_NAME)* ./release-$(PROJECT_NAME)* ./$(PROJECT_NAME)* 2>/dev/null || true
	@echo "$(GREEN)✓ Symbolic links removed$(NC)"
