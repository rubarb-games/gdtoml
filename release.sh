#!/usr/bin/env bash

# set working directory to the directory that this file is in
cd "${0%/*}"

folder_godot_editor=".godot-editor"
version=$(<"godot/godot_ver")

case $(uname | tr '[:upper:]' '[:lower:]') in
  linux*)
    os="linux"
    exe_file="Godot_v${version}_linux.x86_64"
    ;;
  darwin*)
    die "macOS not supported yet. Cannot continue"
    ;;
  msys*)
    os="windows"
    exe_file="Godot_v${version}_win64.exe"
    ;;
  *)
    die "OS not set. Cannot continue"
    ;;
esac

godot_editor="$(pwd)/${folder_godot_editor}/${exe_file}"

scons target=template_release

${godot_editor} --headless --quit --path "$(pwd)/godot/" --export-release "Linux" "../build/gdtoml.x86_64"
