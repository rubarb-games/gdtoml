#!/usr/bin/env bash

# set working directory to the directory that this file is in
cd "${0%/*}"

folder_temp=".temp"
folder_godot_editor=".godot-editor"

# Functions

download_editor() {
	ver=$1
  os=$2

  printf "\nDownload Godot editor ${ver} for ${os}\n"

	case $os in
	  linux*)
	    exe_file="Godot_v${ver}_linux.x86_64"
	    ;;
	  osx*)
	  	die "Downloading macOS editor is not supported yet."
	    ;;
	  windows*)
	    exe_file="Godot_v${ver}_win64.exe"
	    ;;
	  *)
	  	die "Cannot download editor for unspecified OS."
	    ;;
	esac

  zip_file="${exe_file}.zip"
  url="https://github.com/godotengine/godot-builds/releases/download/${ver}/${zip_file}"
  downloaded_file="$(pwd)/${folder_temp}/${zip_file}"

  # Check if Godot editor has been installed, if not install it
  if [ -f "$(pwd)/${folder_godot_editor}/${exe_file}" ]; then
    echo "No need to download ${exe_file}, it is already installed"
  else

    # delete the downloaded file if it exists
    if [ -f "${downloaded_file}" ]; then
      rm "${downloaded_file}"
    fi

    # download and extract the godot editor to the godot-editor folder
    if curl -L# -o "${downloaded_file}" "${url}"; then
      echo "Downloaded ${url}"
      unpack "${downloaded_file}" "$(pwd)/${folder_godot_editor}"
      echo "Extracted ${zip_file}"
    else
      die "Cannot download ${url}"
    fi
  fi
}

make_dir() {
  mkdir -p $1
  echo "Made dir $1"
}

unpack() {
  zip_file=$1
  target_folder=$2
  unzip "${zip_file}" -d "${target_folder}"
}

die() {
  printf '%s\n' "$1" >&2
  exit 1
}

# The script

version=$(<"godot/godot_ver")

case $(uname | tr '[:upper:]' '[:lower:]') in
  linux*)
  	os="linux"
    exe_file="Godot_v${version}_linux.x86_64"
    ;;
  darwin*)
    die "macOS not supported yet. Cannot continue"
    ;;
  mingw*)
  	os="windows"
    exe_file="Godot_v${version}_win64.exe"
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

make_dir "$(pwd)/${folder_temp}"
make_dir "$(pwd)/${folder_godot_editor}"
download_editor ${version} ${os}

scons target=template_debug platform=${os} arch=x86_64 install
${godot_editor} -e --path "$(pwd)/godot/"
