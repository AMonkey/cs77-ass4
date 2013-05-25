#!/bin/bash

function check_error {
    if [ "$?" -ne "0" ]; then
        cd ..
        echo "An error occurred!"
        exit 1
    fi
}

# must cd into scenes folder to handle external data properly
cd scenes

echo "Texture Mapping"
../trace scene_textures.json test_textures.png
check_error

echo "Distribution: Soft Shadows"
../trace -d -P scene_cornellbox.json test_dist_shadows.png
check_error

echo "Distribution: Blurry Reflections"
../trace -d -P scene_blurry.json test_dist_reflect.png
check_error

echo "Distribution: Depth of Field"
../trace -d -P scene_focus.json test_dist_dof.png
check_error

echo "Distribution: Motion Blur"
../trace -d -P scene_transforms.json test_dist_motion.png
check_error

echo "Distribution: Ambient Occlusion"
../trace -d -P scene_robot.json test_dist_ao.png
check_error

echo "Path Tracer"
../trace -p -P scene_cornellbox.json test_path.png
check_error

echo "All completed successfully!"

cd ..
