#!/bin/bash

ADDONS="../../../../addons"
LIBS="../../../../libs"

# Define addons to generate bindings for
declare -A ADDONS_CONFIG=(
    ["waveforms"]="ofxWaveforms"
    ["particles"]="ofxEosParticles"
)

generate_bindings() {
    local project=$1
    local addon_name=$2

    echo -e "\n🌟️ Generating lua bindings for $project\n"

    # Generate main bindings
    swig -v -Wall -c++ -lua -fcompact -fvirtual \
        -I$LIBS/openFrameworks \
        -I$ADDONS/"$addon_name"/src \
        "${project^}Bindings.i"
    # Generate symbols file
    #
    swig -c++ -lua -fcompact -fvirtual \
        -I$LIBS/openFrameworks \
        -I$ADDONS/"$addon_name"/src \
        -debug-lsymbols "${project^}Bindings.i" >"${project}_symbols.txt"

    # Generate Lua syntax
    $ADDONS/ofxLua/scripts/lua_syntax.py "$project" "${project}_symbols.txt"

    # Cleanup
    rm -v "${project}_symbols.txt"
}

# Generate bindings each addon
for project in "${!ADDONS_CONFIG[@]}"; do
    generate_bindings "$project" "${ADDONS_CONFIG[$project]}"
done
