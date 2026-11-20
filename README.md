BRT API JUCE – Shared Classes

Overview

This repository contains a collection of shared C++ classes designed to be reused across different VST audio plugins developed with JUCE and the BRTLibrary.

The main goal is to centralize common functionality and avoid code duplication across projects, making plugin development easier to maintain, extend and manage.

The repository is intended to be integrated into other plugin projects as a Git submodule.

Repository Structure

The repository contains the following components:

File

Description

***ConfiguratorInterface.hpp***

Base class providing a common interface for plugin configuration, sound source management, listener positioning and loading of acoustic resources.

***BasicConfiguration.hpp***

Implements a basic configuration, including listener and acoustic model initialization, environment setup and sound source connections.

***AppUtils.hpp***

Provides utility functions for loading acoustic resources, such as SOFA files (HRTF, BRIR and near-field compensation).

***BRT_API_Juce.h***

Main header that includes the shared classes and provides a single entry point for using the library.

Requirements

The following dependencies are required:

JUCE – Audio plugin development framework.

BRTLibrary – Spatial audio and binaural rendering library.

C++ – Compatible with the C++ standard required by the dependencies.

Installation

The recommended way to integrate this repository into a VST plugin project is as a Git submodule.

1. Add the repository as a submodule

From the root directory of your plugin repository, run:

git submodule add <repository-url> Source/BRT_API_Juce

This will add the shared classes inside the Source directory.

2. Initialize the submodule

When cloning a project that already contains this submodule, use:

git clone --recurse-submodules <plugin-repository-url>

Alternatively, if the project has already been cloned:

git submodule update --init --recursive

3. Include the main header

Once the submodule has been added, simply include the main header in your source files:

#include "BRT_API_Juce/BRT_API_Juce.h"

This header provides access to the shared classes and their associated functionality.

Usage

The shared classes can be used as building blocks when implementing the configuration and initialization stages of a VST plugin.

For example, a plugin can inherit from CConfiguratorInterface or use CBasicConfiguration to initialize its acoustic environment and manage the required BRT resources.

The main idea is to keep common implementation details in this repository while allowing each plugin to implement its own specific functionality.

Integration in Other Projects

When using this repository in multiple plugins, each plugin maintains its own reference to the shared repository through Git submodules.

To retrieve the latest version of the shared classes:

git submodule update --remote

After updating, commit the new submodule reference in the plugin repository.

This ensures that each plugin can use a specific version of the shared classes while allowing improvements and bug fixes to be shared across projects.

License

See the repository license for terms of use.