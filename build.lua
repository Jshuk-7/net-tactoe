workspace "net-tactoe"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "app/build-app.lua"