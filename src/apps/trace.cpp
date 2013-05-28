#include "igl/serialize.h"
#include "igl/scene.h"
#include "igl/intersect.h"
#include "tclap/CmdLine.h"

#include "igl/intersect.h"
#include "igl/raytrace.h"
#include "igl/distraytrace.h"
#include "igl/pathtrace.h"
#include "igl/tesselate.h"

#include <thread>

///@file apps/trace.cpp Trace: Raytraces a scene @ingroup apps
///@defgroup trace Trace: Raytraces a scene
///@ingroup apps
///@{

Scene* scene; ///< scene
RaytraceOptions opts; ///< raytracing options

bool distribution = false; ///< whether to use distribution raytracing
DistributionRaytraceOptions disttrace_opts; ///< distribution raytracing options

bool pathtrace = false; ///< pathtracing
PathtraceOptions pathtrace_opts; ///< pathtracing options

bool progressive = false; ///< whether to use progressive image savings

ImageBuffer trace_image_buffer; ///< image buffer for progressive rendering

string filename_scene; ///< scene filename
string filename_image; ///< rendered image filename

int resolution = -1;
int samples = -1;

/// parse command line arguments
void parse_args(int argc, char** argv) {
	try {  
        TCLAP::CmdLine cmd("trace", ' ', "0.0");

        TCLAP::ValueArg<int> resolutionArg("r","resolution","Image resolution",false,0,"int",cmd);
        TCLAP::ValueArg<int> samplesArg("s","samples","Pixel samples",false,0,"int",cmd);
        
        TCLAP::SwitchArg progressiveArg("P","progressive","Progressive Rendering",cmd);
        
        TCLAP::SwitchArg distributionArg("d","distribution_raytrace","Distribution Raytracing",cmd);
        TCLAP::SwitchArg pathtraceArg("p","pathtrace","Pathtracing",cmd);
        
        TCLAP::UnlabeledValueArg<string> filenameScene("scene","Scene filename",true,"","filename",cmd);
        TCLAP::UnlabeledValueArg<string> filenameImage("image","Image filename",false,"","filename",cmd);
        
        cmd.parse( argc, argv );

        if(pathtraceArg.isSet()) pathtrace = pathtraceArg.getValue();
        if(distributionArg.isSet()) distribution = distributionArg.getValue();
        
        if(resolutionArg.isSet()) resolution = resolutionArg.getValue();
        if(samplesArg.isSet()) samples = samplesArg.getValue();
        if(progressiveArg.isSet()) progressive = progressiveArg.getValue();
        
        filename_scene = filenameScene.getValue();
        if(filenameImage.isSet()) filename_image = filenameImage.getValue();
        else { filename_image = filename_scene.substr(0,filename_scene.length()-4)+"png"; }
	} catch (TCLAP::ArgException &e) { 
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    }
}

void init_buffers(int w, int h) {
    trace_image_buffer = ImageBuffer(w, h);
}

void render_pass(image3f& img) {
    if(distribution) {
        //PUT_YOUR_CODE_HERE("Distribution Raytracing");
        dist_raytrace_scene_progressive(trace_image_buffer, scene, disttrace_opts);

    }
    else if(pathtrace) PUT_YOUR_CODE_HERE("Pathtracing");
    else raytrace_scene_progressive(trace_image_buffer, scene, opts);
}

void transfer(image3f& img) {
    auto w = img.width();
    auto h = img.height();
    for(int j = 0; j < h; j ++) {
        for(int i = 0; i < w; i ++) {
            img.at(i,h-1-j) = trace_image_buffer.accum.at(i,h-1-j) / trace_image_buffer.samples.at(i,h-1-j);
        }
    }
}

/// main: load scene, initialize acceleration, raytraces scene, saves image
int main(int argc, char** argv) {
    parse_args(argc,argv);
    Serializer::read_json(scene, filename_scene);
    if(scene->raytrace_opts) opts = *scene->raytrace_opts;
    if(scene->distribution_opts) disttrace_opts = *scene->distribution_opts;
    if(scene->pathtrace_opts) pathtrace_opts = *scene->pathtrace_opts;

    // command-line options override settings in json
    if(resolution > 0) {
        opts.res = resolution;
        disttrace_opts.res = resolution;
        pathtrace_opts.res = resolution;
    }
    if(samples > 0) {
        opts.samples = samples;
        disttrace_opts.samples = samples;
        pathtrace_opts.samples = samples;
    }

    scene_tesselation_init(scene,false,0,false);
    //scene_animation_snapshot(scene,opts.time);
    sample_lights_init(scene->lights);
    if(opts.cameralights) scene_cameralights_update(scene,opts.cameralights_dir, opts.cameralights_col);
    intersect_scene_accelerate(scene);
    
    auto w = camera_image_width(scene->camera, opts.res);
    auto h = camera_image_height(scene->camera, opts.res);
    image<vec3f> img;
    init_buffers(w, h);
    auto samples = (pathtrace ? pathtrace_opts.samples : (distribution ? disttrace_opts.samples : opts.samples ) );

    // for debug:
    for(auto s = 0; s < samples; s ++) {
        printf("Pass: %02d/%02d\n", s, samples);
        render_pass(img);
        if(progressive && s < samples-1) {
            trace_image_buffer.get_image(img);
            imageio_write_png(filename_image, img, false);
        }
    }
    trace_image_buffer.get_image(img);
    imageio_write_png(filename_image, img, false);
}

///@}
