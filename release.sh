#!/usr/bin/env bash

# set working directory to the directory that this file is in
cd "${0%/*}"

scons target=template_release
