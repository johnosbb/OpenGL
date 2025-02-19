#!/bin/bash
mkdir -p include/glad src
curl -o src/glad.c https://raw.githubusercontent.com/Dav1dde/glad/master/src/glad.c
curl -o include/glad/glad.h https://raw.githubusercontent.com/Dav1dde/glad/master/include/glad/glad.h
curl -o include/KHR/khrplatform.h https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/main/api/KHR/khrplatform.h
