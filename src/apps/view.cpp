#include "igl/scene.h"
#include "igl/serialize.h"
#include "igl/gls.h"
#include "igl/gl_utils.h"
#include "common/common.h"
#include "igl/intersect.h"
#include "tclap/CmdLine.h"

#include "igl/draw.h"
#include "igl/intersect.h"
#include "igl/tesselate.h"

#include "igl/raytrace.h"
#include "igl/distraytrace.h"
#include "igl/pathtrace.h"

///@file apps/view.cpp View: Interactice Viewer @ingroup apps
///@defgroup view View: Interactice Viewer
///@ingroup apps
///@{

const bool          fixed_dt = true; ///< fixed frame time

bool                screenshotAndExit = false; ///< take a screenshot and exit right away

Scene*              scene = nullptr; ///< scene

DrawOptions         draw_opts; ///< draw options

bool                animating = false; ///< whether it is currently playing back animation
bool                animating_steps = false; 
range1f             animate_interval = range1f(); ///< scene animation interval
bool                animate_loop = false; ///< whether to loop the animation
bool                simulate_has = false; ///< whether the scene has simulation

string              filename_scene = ""; ///< scene filename
string              filename_image = ""; ///< captured image filename

int                 tesselation_level = -1; ///< tesselation override level (-1 for default)
bool                tesselation_smooth = false; ///< tesselation override smooth

bool                hud = true; ///< whether to display the hud
timer_avg           hud_fps_update; ///< whether to display update frames-per-second in the hud
timer_avg           hud_fps_display; ///< whether to display draw frames-per-second in the hud

int                 selected_element = -1; ///< selected scene element (-1 for no selection)
int                 selected_subelement = -1; ///< selected scene subelement (-1 for no selection)
frame3f*            selected_frame = nullptr; ///< selected frame pointer (used to edit)
vec3f*              selected_point = nullptr; ///< selected point pointer (used to edit)

float               selecion_speed_keyboard_translate = 0.5f; ///< selection speed
float               selecion_speed_keyboard_rotate = pif / 8; ///< selection speed
float               selecion_speed_mouse_translate = 1.0f / 100; ///< selection speed
float               selecion_speed_mouse_rotate = 1.0 / (pif / 8); ///< selection speed


bool                trace = false; ///< whether to enable interactive tracing
RaytraceOptions     trace_opts; ///< tracing options
bool                trace_distributed = false; ///< tracing uses ditribution raytracing
DistributionRaytraceOptions         trace_distributed_opts; ///< distribution raytracing options
bool                trace_path = false; ///< tracing uses path tracing
PathtraceOptions    trace_path_opts; ///< pathtracing options
int                 trace_zoom = 8; ///< tracing image zoom (higher value render a smaller image)
bool                trace_updated = true; ///< scene was updated, restart the tracer
int                 trace_updated_counter = 10; ///< scene was updated, restart the tracer after counter idle cycles
int                 trace_progressive_cursample = 0; ///< progressive tracing current sample
int                 trace_progressive_maxsamples = 0; ///< progressive tracing number of samples
bool                trace_progressive_clear = false; ///< progressive tracing, whether to clear the background

image3f             trace_img; ///< traced image
ImageBuffer         trace_image_buffer; ///< progressive buffer

/// init trace progressive buffers
void trace_clear_buffers() {
    auto wc = camera_image_width(scene->camera, trace_opts.res);
    auto hc = camera_image_height(scene->camera, trace_opts.res);
    trace_image_buffer = ImageBuffer(wc,hc);
}

/// init trace image
void trace_init_res() {
    trace_opts.res = draw_opts.res / float(trace_zoom);
    trace_updated = true;
    
    if(not scene) return;
    auto wc = camera_image_width(scene->camera, trace_opts.res);
    auto hc = camera_image_height(scene->camera, trace_opts.res);
    trace_img = image3f(wc,hc);
    
    trace_clear_buffers();
}

/// sync trace options with draw options
void trace_sync_opts() {
    //trace_opts.cameralights = draw_opts.cameralights;
    trace_opts.time = draw_opts.time;
    //trace_opts.ambient = draw_opts.ambient;
    //trace_distributed_opts.cameralights = draw_opts.cameralights;
    trace_distributed_opts.time = draw_opts.time;
    trace_distributed_opts.res = trace_opts.res;
    //trace_distributed_opts.ambient = draw_opts.ambient;
    //trace_path_opts.cameralights = draw_opts.cameralights;
    trace_path_opts.time = draw_opts.time;
    trace_distributed_opts.res = trace_opts.res;
    //trace_path_opts.ambient = draw_opts.ambient;
}

/// performs one pass of progressive tracing
void trace_progressive_pass() {
    if(not trace) return;
    if(trace_progressive_cursample >= trace_progressive_maxsamples) return;
    trace_progressive_cursample++;
    trace_sync_opts();
    
    if(trace_distributed) PUT_YOUR_CODE_HERE("Distribution Raytracing");
    else if(trace_path) PUT_YOUR_CODE_HERE("Pathtracing");
    else raytrace_scene_progressive(trace_image_buffer, scene, trace_opts);
    
    trace_image_buffer.get_image(trace_img);
}

/// starts progressive tracing
void trace_progressive_start() {
    if(trace_progressive_clear) trace_img.set(trace_opts.background);
    trace_clear_buffers();
    trace_progressive_cursample = 0;
    trace_progressive_maxsamples = (trace_path) ? trace_path_opts.samples : (trace_distributed ? trace_distributed_opts.samples : trace_opts.samples );
}

/// trace the whole image
void trace_render() {
    trace_clear_buffers();
    trace_progressive_pass();
}

/// change the trace image zoom
void trace_zoom_update(int update) {
    if (update > 0) trace_zoom *= pow2(update);
    else trace_zoom /= pow2(-update);
    trace_zoom = clamp(trace_zoom, 1, 512);
    trace_init_res();
    trace_updated = true;
}

/// change the trace image samples
void trace_samples_update(int update) {
    int samples = log2(trace_opts.samples);// (int)(sqrt(trace_opts.samples)+0.5);
    trace_opts.samples = pow2(clamp(samples+update,1,100));
    int dsamples = log2(trace_distributed_opts.samples); // (int)(sqrt(trace_distributed_opts.samples)+0.5);
    trace_distributed_opts.samples = pow2(clamp(dsamples+update,1,100));
    int psamples = log2(trace_path_opts.samples); // (int)(sqrt(trace_path_opts.samples)+0.5);
    trace_path_opts.samples = pow2(clamp(psamples+update,1,100));
}

/// update timer
auto update_timer = timer();

// start animation playback
void animate_start() { update_timer.start(); animating = true; trace_updated = true; }

/// stop animation playback
void animate_stop() { animating = false; }

/// update the scene during animation playback
void animate_update() {
    float dt = update_timer.elapsed();
    if(fixed_dt) dt = clamp(1/30.0f,0.0f,animate_interval.max-draw_opts.time);
    else dt = clamp(dt,0.0f,animate_interval.max-draw_opts.time);
    if(dt > 0) {
        draw_opts.time += dt;
        update_timer.start();
    } else {
        if(draw_opts.time >= animate_interval.max) animate_stop();
    }
    trace_updated = true;
}

/// update the scene for one step in animation playback
void animate_step(bool forward) {
    float dt;
    if(forward) dt = clamp(1/30.0f,0.0f,animate_interval.max-draw_opts.time);
    else dt = clamp(-1/30.0f,animate_interval.min-draw_opts.time,0.0f);
    if(dt != 0) {
        draw_opts.time += dt;
    }
    trace_updated = true;
}

/// clear subelement selection
void selection_subelement_clear() {
    selected_subelement = -1;
    selected_point = nullptr;
}

/// clear element selection
void selection_element_clear() {
    selected_element = -1;
    selected_frame = nullptr;
    selection_subelement_clear();
}

/// select next element
void selection_element_next(bool forward) {
    auto nprims = scene->prims->prims.size();
    auto nlights = scene->lights->lights.size();
    auto nelements = nprims + nlights;
    if(selected_element < 0) selected_element = (forward) ? 0 : nelements-1;
    else selected_element = (selected_element + ( (forward) ? 1 : -1 ) + nelements) % nelements;
    selected_frame = (selected_element < nprims) ? &scene->prims->prims[selected_element]->frame : &scene->lights->lights[selected_element - nprims]->frame;
    selection_subelement_clear();
}

/// select next subelement
void selection_subelement_next(bool forward) {
    if(selected_element >= scene->prims->prims.size() or selected_element < 0) return;
    auto prim = scene->prims->prims[selected_element];
    if(not is<Surface>(prim)) return;
    auto shape = cast<Surface>(prim)->shape;
    auto pos = shape_get_pos(shape);
    if(not pos) return;
    auto nelements = pos->size();
    if(selected_subelement < 0) selected_subelement = (forward) ? 0 : nelements-1;
    else selected_subelement = (selected_subelement + ( (forward) ? 1 : -1 ) + nelements) % nelements;
    selected_point = &pos->at(selected_subelement);
}

/// move selection
void selection_move(const vec3f& t) {
    if(selected_point) {
        *selected_point += transform_vector(*selected_frame,t);
        shape_tesselation_init(cast<Surface>(scene->prims->prims[selected_element])->shape, tesselation_level >= 0, tesselation_level, tesselation_smooth);
        //if(accelerate_scene) {
            intersect_shape_accelerate(cast<Surface>(scene->prims->prims[selected_element])->shape);
        //}
    }
    else if(selected_frame) selected_frame->o += transform_vector(*selected_frame,t);
    trace_updated = true;
}

/// rotate selection
void selection_rotate(const vec3f& ea) {
    if(selected_point) return;
    if(selected_frame) {
        auto m = translation_matrix(selected_frame->o) *
                 rotation_matrix(ea.x, selected_frame->x) *
                 rotation_matrix(ea.y, selected_frame->y) *
                 rotation_matrix(ea.z, selected_frame->z) *
                 translation_matrix(- selected_frame->o);
        *selected_frame = transform_frame(m, *selected_frame);
    }
    trace_updated = true;
}

/// init scene
void init() {
    scene_tesselation_init(scene,tesselation_level>=0,tesselation_level,tesselation_smooth);
    scene_defaultgizmos_init(scene);
    animate_interval = scene_animation_interval(scene);
    draw_opts.time = 0;
    if(scene->draw_opts) draw_opts = *scene->draw_opts;
    if(scene->raytrace_opts) trace_opts = *scene->raytrace_opts;
    if(scene->distribution_opts) trace_distributed_opts = *scene->distribution_opts;
    if(scene->pathtrace_opts) trace_path_opts = *scene->pathtrace_opts;

    selection_element_clear();
    if(trace) {
        trace_init_res();
        //if(accelerate_scene) {
            scene->prims->intersect_accelerator_use = false;
            intersect_scene_accelerate(scene);
        //}
        sample_lights_init(scene->lights);
    }
    trace_updated = true;
}

/// grab pixels from the screen
image3f readpixels() {
    int w = camera_image_width(scene->camera,draw_opts.res);
    int h = camera_image_height(scene->camera,draw_opts.res);
    return glutils_read_pixels(w,h,true);
}

/// grab a screenshot and save it
void screenshot(const char *filename_png) {
    image3f img = readpixels();
    imageio_write_png(filename_png, img, true);
}

/// load scene
void load() {
    scene = nullptr;
    Serializer::read_json(scene, filename_scene);
    ERROR_IF_NOT(scene,"could not load scene");
    init();
}
/// reload the scene
void reload() {
    Scene* new_scene = nullptr;
    Serializer::read_json(new_scene, filename_scene);
    if(new_scene) {
        scene = new_scene;
        init();
    } else WARNING("could not reload scene");
}
/// save scene
void save() {
    Serializer::write_json(scene, filename_scene, true);
}

/// change the view to frame the scene
void frame() {
    auto bbox = intersect_scene_bounds(scene);
    vec3f c = center(bbox);
    vec3f s = size(bbox);
    camera_view_lookat(scene->camera,c+scene->camera->frame.z*length(s), c, scene->camera->frame.y);
    trace_updated = true;
}

/// print commands
void print_help() {
    printf("viewing ----------------------------\n"
           "mouse-left          rotate\n"
           "mouse-right         dolly\n"
           "mouse-middle        pan\n"
           "mouse-ctrl          pan\n"
           "1                   faces on/off\n"
           "2                   edges on/off\n"
           "3                   lines on/off\n"
           "4                   control on/off\n"
           "$                   depth check for control on/off\n"
           "5                   gizmos on/off\n"
           "7                   camera lights on/off\n"
           "8                   scene tesselation descrease\n"
           "9                   scene tesselation increase\n"
           "0                   scene tesselation smooth\n"
           "`                   doublesided on/off\n"
           "f                   frame\n"
           "\n"
           "application ------------------------\n"
           "esc                 quit\n"
           "h                   print help\n"
           "r                   reload\n"
           "*                   save\n"
           "i                   save screenshot\n"
           "j                   hud on/off\n"
           "/                   reset\n"
           "\n"
           "animation --------------------------\n"
           "space               animation on/off\n"
           ",/.                 animation step\n"
           "\n"
           "editing ----------------------------\n"
           "[/]                 select primitive/light\n"
           "{/}                 select shape point\n"
           "\\/|                clear select primitive/point\n"
           "alt-mouse-left      translate x\n"
           "alt-mouse-right     translate y\n"
           "alt-mouse-middle    translate z\n"
           "ctrl-alt-mouse      translate z\n"
           "w/s a/d q/e         translate xyz\n"
           "shift               rotate xyz\n"
           "\n"
           "tracing ----------------------------\n"
           ";/'                 trace resolution\n"
           ":/\"                trace samples\n"
           "P                   trace clear background\n"
           "\n");
}

/// keyboard event handler
void keyboard(unsigned char c, int x, int y) {
	switch(c) {
        case 27: exit(0); break;
        case 'h': print_help(); break;
        case '1': draw_opts.faces = not draw_opts.faces; break;
        case '2': draw_opts.edges = not draw_opts.edges; break;
        case '3': draw_opts.lines = not draw_opts.lines; break;
        case '4': draw_opts.control = not draw_opts.control; break;
        case '$': draw_opts.control_no_depth = not draw_opts.control_no_depth; break;
        case '5': draw_opts.gizmos = not draw_opts.gizmos; break;
        case '7': draw_opts.cameralights = not draw_opts.cameralights; break;
        case '8': tesselation_level = max(-1,tesselation_level-1); init(); break;
        case '9': tesselation_level = min(88,tesselation_level+1); init(); break;
        case '0': tesselation_smooth = !tesselation_smooth; init(); break;
        case '`': draw_opts.doublesided = not draw_opts.doublesided; break;
        case ' ': if(animating) animate_stop(); else animate_start(); break;
        case '/': init(); break;
        case '?': animate_loop = !animate_loop; break;
        case ',': animate_step(false); break;
        case '.': animate_step(true); break;
        case 'j': hud = not hud; break;
        case 'f': frame(); break;
        case 'r': reload(); break;
        case '*': save(); break;
        case 'i': screenshot(filename_image.c_str()); break;
        case '[': selection_element_next(false); break;
        case ']': selection_element_next(true); break;
        case '\\': selection_element_clear(); break;
        case '{': selection_subelement_next(false); break;
        case '}': selection_subelement_next(true); break;
        case '|': selection_subelement_clear(); break;
        case 'a': selection_move( - x3f * selecion_speed_keyboard_translate ); break;
        case 'd': selection_move(   x3f * selecion_speed_keyboard_translate ); break;
        case 'w': selection_move(   z3f * selecion_speed_keyboard_translate ); break;
        case 's': selection_move( - z3f * selecion_speed_keyboard_translate ); break;
        case 'q': selection_move( - y3f * selecion_speed_keyboard_translate ); break;
        case 'e': selection_move(   y3f * selecion_speed_keyboard_translate ); break;
        case 'A': selection_rotate( - x3f * selecion_speed_keyboard_rotate ); break;
        case 'D': selection_rotate(   x3f * selecion_speed_keyboard_rotate ); break;
        case 'W': selection_rotate(   z3f * selecion_speed_keyboard_rotate ); break;
        case 'S': selection_rotate( - z3f * selecion_speed_keyboard_rotate ); break;
        case 'Q': selection_rotate( - y3f * selecion_speed_keyboard_rotate ); break;
        case 'E': selection_rotate(   y3f * selecion_speed_keyboard_rotate ); break;
        case ';': trace_zoom_update(-1); break;
        case '\'': trace_zoom_update(+1); break;
        case ':': trace_samples_update(-1); break;
        case '"': trace_samples_update(+1); break;
        case 'P': trace_progressive_clear = not trace_progressive_clear; break;
		default: break;
	}
    
    trace_updated = true;
	glutPostRedisplay();
}

/// mouse action types
enum MouseAction {
    mouse_none,
    mouse_turntable_rotate,
    mouse_turntable_dolly,
    mouse_turntable_pan,
    mouse_edit_frame_move_x,
    mouse_edit_frame_move_y,
    mouse_edit_frame_move_z,
    mouse_edit_frame_rotate_x,
    mouse_edit_frame_rotate_y,
    mouse_edit_frame_rotate_z,
};

MouseAction mouse_action = mouse_none; ///< mouse current action
vec2i mouse_start; ///< mouse start/clicked location
vec2i mouse_last; ///< mouse last location (during dragging)

/// mouse motion event handler
void motion(int x, int y) {
    auto mouse = vec2i{x,y};
    auto delta = mouse - mouse_last;
    auto delta_f = vec2f{delta} * 0.01;
    switch(mouse_action) {
        case mouse_turntable_rotate: camera_view_turntable_rotate(scene->camera, -delta_f.x, -delta_f.y); break;
        case mouse_turntable_dolly: camera_view_turntable_dolly(scene->camera,-delta_f.y); break;
        case mouse_turntable_pan: camera_view_turntable_pan(scene->camera,-delta_f.x,delta_f.y); break;
        case mouse_edit_frame_move_x: selection_move(x3f*delta_f.y); break;
        case mouse_edit_frame_move_y: selection_move(y3f*delta_f.y); break;
        case mouse_edit_frame_move_z: selection_move(z3f*delta_f.y); break;
        case mouse_edit_frame_rotate_x: selection_rotate(x3f*delta_f.y); break;
        case mouse_edit_frame_rotate_y: selection_rotate(y3f*delta_f.y); break;
        case mouse_edit_frame_rotate_z: selection_rotate(z3f*delta_f.y); break;
        default: break;
    }
    mouse_last = vec2i(x,y);
    trace_updated = true;
    glutPostRedisplay();
}

/// mouse button event handler
void mouse(int button, int state, int x, int y) {
    mouse_start = vec2i(x,y);
    mouse_last = mouse_start;
    mouse_action = mouse_none;
    if(not state == GLUT_DOWN) return;
    
    if(glutGetModifiers() & GLUT_ACTIVE_ALT) {
        if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
            if(button == GLUT_LEFT_BUTTON) mouse_action = mouse_edit_frame_rotate_x;
            if(button == GLUT_RIGHT_BUTTON) mouse_action = mouse_edit_frame_rotate_y;
            if(button == GLUT_MIDDLE_BUTTON) mouse_action = mouse_edit_frame_rotate_z;
            if(glutGetModifiers() & GLUT_ACTIVE_CTRL) {
                if(button == GLUT_LEFT_BUTTON) mouse_action = mouse_edit_frame_rotate_z;
                if(button == GLUT_RIGHT_BUTTON) mouse_action = mouse_edit_frame_rotate_z;
            }
        } else {
            if(button == GLUT_LEFT_BUTTON) mouse_action = mouse_edit_frame_move_x;
            if(button == GLUT_RIGHT_BUTTON) mouse_action = mouse_edit_frame_move_y;
            if(button == GLUT_MIDDLE_BUTTON) mouse_action = mouse_edit_frame_move_z;
            if(glutGetModifiers() & GLUT_ACTIVE_CTRL) {
                if(button == GLUT_LEFT_BUTTON) mouse_action = mouse_edit_frame_move_z;
                if(button == GLUT_RIGHT_BUTTON) mouse_action = mouse_edit_frame_move_z;
            }
        }
    } else {
        if(button == GLUT_LEFT_BUTTON) mouse_action = mouse_turntable_rotate;
        if(button == GLUT_RIGHT_BUTTON) mouse_action = mouse_turntable_dolly;
        if(button == GLUT_MIDDLE_BUTTON) mouse_action = mouse_turntable_pan;
        if(glutGetModifiers() & GLUT_ACTIVE_CTRL) {
            if(button == GLUT_LEFT_BUTTON) mouse_action = mouse_turntable_pan;
            if(button == GLUT_RIGHT_BUTTON) mouse_action = mouse_turntable_pan;
        }
    }
    
    motion(x,y);
}

/// draw a string using glut
void draw_string(const string& name) {
    for(auto c : name) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
}

/// draw hud
void display_hud() {
    char buf[2048];
    sprintf(buf, "time: %6d / draw: %s%s%s%s%s / light: %s / tess: %2d%s",
            (int)round(draw_opts.time*1000),
            (draw_opts.faces)?"f":" ",(draw_opts.edges)?"e":" ",
            (draw_opts.lines)?"l":" ",(draw_opts.control)?"c":" ",
            (draw_opts.doublesided)?"d":" ",
            (draw_opts.cameralights)?"c":"s",
            tesselation_level,(tesselation_smooth)?"s":"f");
//    msg01 += "/"+to_string("%3d",(int)round(1/hud_fps_display.elapsed()));
//    msg01 += "/"+to_string("%3d",(animating)?(int)round(1/hud_fps_update.elapsed()):0);
    
    int w = glutGet(GLUT_WINDOW_WIDTH) / ((trace) ? 2 : 1);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    
    glColor3f(1,1,1);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPixelZoom(1.0,1.0);
    glRasterPos2f(5, 20);
    draw_string(buf);
    glPopMatrix();
}

/// draw trace image
void display_trace() {
    int w = glutGet(GLUT_WINDOW_WIDTH) / ((trace) ? 2 : 1);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    
    if(trace_updated) {
        trace_progressive_start();
        trace_updated = false;
    }
    
    glViewport(w, 0, w, h);
    
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glRasterPos2f(0, 0);
    auto zoom = min(float(w) / trace_img.width(), float(h) / trace_img.height());
    glPixelZoom(zoom,-zoom);
    glDrawPixels(trace_img.width(), trace_img.height(), GL_RGB, GL_FLOAT, trace_img.data()->data());
    glPopMatrix();
}

/// draw trace hud
void display_trace_hud() {
    char buf[2048];
    int samples = (trace_path) ? trace_path_opts.samples : (trace_distributed ? trace_distributed_opts.samples : trace_opts.samples );
    sprintf(buf, "res: %dx%d@%ds/%dx       prog: %d/%d",
            trace_img.width(), trace_img.height(), samples, trace_zoom,
            trace_progressive_cursample,trace_progressive_maxsamples);
    
    int w = glutGet(GLUT_WINDOW_WIDTH) / ((trace) ? 2 : 1);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    
    glColor3f(1,1,1);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPixelZoom(1.0,1.0);
    glRasterPos2f(5, 20);
    draw_string(buf);
    glPopMatrix();
}

/// main draw method
void display() {
    if(draw_opts.cameralights) scene_cameralights_update(scene,draw_opts.cameralights_dir,draw_opts.cameralights_col);
    draw_scene(scene,draw_opts,true);
    draw_scene_decorations(scene,draw_opts,false);
    
    if(selected_frame) {
        auto axes = Axes();
        axes.frame = *selected_frame;
        draw_gizmo(&axes);
        if(selected_point) {
            auto dot = Dot();
            dot.pos = transform_point(*selected_frame, *selected_point);
            draw_gizmo(&dot);
        }
    }
    
    if(hud) display_hud();
    
    if(trace) display_trace();
    if(trace and hud) display_trace_hud();
    
    glutSwapBuffers();
}

/// reshape event handler
void reshape(int w, int h) {
    if(not trace) {
        draw_opts.res = h;
        camera_image_set_aspectratio(scene->camera, w, h);
    } else {
        draw_opts.res = h;
        camera_image_set_aspectratio(scene->camera, w/2, h);
        trace_init_res();
    }
}

/// idle event handler
void idle() {
    if(animating) {
        hud_fps_update.start();
        if(animating_steps) animate_step(true);
        else animate_update();
        hud_fps_update.stop();
        if(not animating) hud_fps_update.reset();
        glutPostRedisplay();
    }
    if(trace and trace_progressive_cursample < trace_progressive_maxsamples) {
        trace_progressive_pass();
        glutPostRedisplay();
    }
}

/// glut initialization
void init(int* argc, char** argv) {
	glutInit(argc, argv);
    char buf[2048];
    sprintf(buf,"rgba depth double samples<%d",draw_opts.samples);
    glutInitDisplayString(buf);
	glutInitWindowPosition(0,0);
    int w = camera_image_width(scene->camera,draw_opts.res);
    int h = camera_image_height(scene->camera,draw_opts.res);
	glutInitWindowSize((not trace) ? w : w*2,h);
    auto title = "view | " + filename_scene;
    glutCreateWindow(title.c_str());
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
}

/// glut start main loop
void run() { glutMainLoop(); }

/// parses command line arguments
void parse_args(int argc, char** argv) {
	try {  
        TCLAP::CmdLine cmd("view", ' ', "0.0");
        
        TCLAP::SwitchArg traceArg("t","trace","Trace enabled",cmd);
        
        TCLAP::SwitchArg distributionArg("d","distribution_raytrace","Distribution Raytracing",cmd);
        TCLAP::SwitchArg pathArg("p","pathtrace","Pathtracing",cmd);
        
        TCLAP::UnlabeledValueArg<string> filenameScene("scene","Scene filename",true,"","scene",cmd);
        TCLAP::UnlabeledValueArg<string> filenameImage("image","Image filename",false,"","image",cmd);
        
        cmd.parse( argc, argv );
        
        if(traceArg.isSet()) trace = traceArg.getValue();
        if(distributionArg.isSet()) trace_distributed = distributionArg.getValue();
        if(pathArg.isSet()) trace_path = pathArg.getValue();

        trace = trace or trace_distributed or trace_path;
        
        filename_scene = filenameScene.getValue();
        if(filenameImage.isSet()) filename_image = filenameImage.getValue();
        else { filename_image = filename_scene.substr(0,filename_scene.length()-4)+"png"; }
	} catch (TCLAP::ArgException &e) { 
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    }
}

/// main: parses args, loads scene, start gui
int main(int argc, char** argv) {
    parse_args(argc, argv);
    load();
	init(&argc, argv);
	run();
}

///@}

