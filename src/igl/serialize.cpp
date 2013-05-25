#include "serialize.h"          

#include "draw.h"
#include "raytrace.h"
#include "distraytrace.h"
#include "pathtrace.h"

///@file igl/serialize.cpp Serialization. @ingroup igl

Serializer::_Registry Serializer::_registry;

void Serializer::register_object_types() {
    static bool done = false;
    if(done) return;
    register_object_type<KeyframedValue>();
    register_object_type<PointSet>();
    register_object_type<LineSet>();
    register_object_type<TriangleMesh>();
    register_object_type<Mesh>();
    register_object_type<FaceMesh>();
    register_object_type<CatmullClarkSubdiv>();
    register_object_type<Subdiv>();
    register_object_type<TesselationOverride>();
    register_object_type<Spline>();
    register_object_type<Patch>();
    register_object_type<DisplacedShape>();
    register_object_type<Sphere>();
    register_object_type<Cylinder>();
    register_object_type<Quad>();
    register_object_type<Triangle>();
    register_object_type<Lambert>();
    register_object_type<Phong>();
    register_object_type<LambertEmission>();
    register_object_type<Material>();
    register_object_type<Camera>();
    register_object_type<LightGroup>();
    register_object_type<PointLight>();
    register_object_type<DirectionalLight>();
    register_object_type<AreaLight>();
    register_object_type<EnvLight>();
    register_object_type<Surface>();
    register_object_type<TransformedSurface>();
    register_object_type<PrimitiveGroup>();
    register_object_type<Texture>();
    register_object_type<GizmoGroup>();
    register_object_type<Grid>();
    register_object_type<Axes>();
    register_object_type<Line>();
    register_object_type<Dot>();
    register_object_type<Scene>();
    register_object_type<DrawOptions>();
    register_object_type<RaytraceOptions>();
    register_object_type<DistributionRaytraceOptions>();
    register_object_type<PathtraceOptions>();
    done = true;
}

const char* serialize_typename(Node* node) {
    if(not node) return nullptr;
    else if(is<KeyframedValue>(node)) return "KeyframedValue";
    else if(is<Shape>(node)) {
        if(not node) return nullptr;
        else if(is<PointSet>(node)) return "PointSet";
        else if(is<LineSet>(node)) return "LineSet";
        else if(is<TriangleMesh>(node)) return "TriangleMesh";
        else if(is<Mesh>(node)) return "Mesh";
        else if(is<FaceMesh>(node)) return "FaceMesh";
        else if(is<CatmullClarkSubdiv>(node)) return "CatmullClarkSubdiv";
        else if(is<Subdiv>(node)) return "Subdiv";
        else if(is<TesselationOverride>(node)) return "TesselationOverride";
        else if(is<Spline>(node)) return "Spline";
        else if(is<Patch>(node)) return "Patch";
        else if(is<DisplacedShape>(node)) return "DisplacedShape";
        else if(is<Sphere>(node)) return "Sphere";
        else if(is<Cylinder>(node)) return "Cylinder";
        else if(is<Quad>(node)) return "Quad";
        else if(is<Triangle>(node)) return "Triangle";
        else return "Shape";
    }
    else if(is<Material>(node)) {
        if(not node) return nullptr;
        else if(is<Lambert>(node)) return "Lambert";
        else if(is<Phong>(node)) return "Phong";
        else if(is<LambertEmission>(node)) return "LambertEmission";
        else return "Material";
    }
    else if(is<Camera>(node)) return "Camera";
    else if(is<LightGroup>(node)) return "LightGroup";
    else if(is<Light>(node)) {
        if(not node) return nullptr;
        else if(is<PointLight>(node)) return "PointLight";
        else if(is<DirectionalLight>(node)) return "DirectionalLight";
        else if(is<AreaLight>(node)) return "AreaLight";
        else if(is<EnvLight>(node)) return "EnvLight";
        else return "Light";
    }
    else if(is<Primitive>(node)) {
        if(not node) return nullptr;
        else if(is<Surface>(node)) return "Surface";
        else if(is<TransformedSurface>(node)) return "TransformedSurface";
        else return "Primitive";
    }
    else if(is<PrimitiveGroup>(node)) return "PrimitiveGroup";
    else if(is<Texture>(node)) return "Texture";
    else if(is<Gizmo>(node)) {
        if(not node) return nullptr;
        else if(is<Grid>(node)) return "Grid";
        else if(is<Axes>(node)) return "Axes";
        else if(is<Line>(node)) return "Line";
        else if(is<Dot>(node)) return "Dot";
        else return "Gizmo";
    }
    else if(is<GizmoGroup>(node)) return "GizmoGroup";
    else if(is<Scene>(node)) return "Scene";
    else if(is<DrawOptions>(node)) return "DrawOptions";
    else if(is<RaytraceOptions>(node)) return "RaytraceOptions";
    else if(is<DistributionRaytraceOptions>(node)) return "DistributionRaytraceOptions";
    else if(is<PathtraceOptions>(node)) return "PathtraceOptions";
    else return "Node";
}

void serialize_members(Node* node, Serializer& ser) {
    if(not node) WARNING("node is null");
    else if(is<KeyframedValue>(node)) {
        auto keyframed = cast<KeyframedValue>(node);
        ser.serialize_member("values",keyframed->values);
        ser.serialize_member("times",keyframed->times);
        ser.serialize_member("degree",keyframed->degree);
    }
    else if(is<Shape>(node)) {
        auto shape = cast<Shape>(node);
        if(not shape) ERROR("node is null");
        ser.serialize_member("intersect_accelerator_use",shape->intersect_accelerator_use);
        if(is<PointSet>(node)) {
            auto points = cast<PointSet>(node);
            ser.serialize_member("pos",points->pos);
            ser.serialize_member("radius",points->radius);
            ser.serialize_member("texcoord",points->texcoord);
            ser.serialize_member("approximate",points->approximate);
            ser.serialize_member("approximate_radius",points->approximate_radius);
        }
        else if(is<LineSet>(node)) {
            auto lines = cast<LineSet>(node);
            ser.serialize_member("pos",lines->pos);
            ser.serialize_member("radius",lines->radius);
            ser.serialize_member("texcoord",lines->texcoord);
            ser.serialize_member("line",lines->line);
            ser.serialize_member("approximate",lines->approximate);
            ser.serialize_member("approximate_radius",lines->approximate_radius);
        }
        else if(is<TriangleMesh>(node)) {
            auto mesh = cast<TriangleMesh>(node);
            ser.serialize_member("pos",mesh->pos);
            ser.serialize_member("norm",mesh->norm);
            ser.serialize_member("texcoord",mesh->texcoord);
            ser.serialize_member("triangle",mesh->triangle);
        }
        else if(is<Mesh>(node)) {
            auto mesh = cast<Mesh>(node);
            ser.serialize_member("pos",mesh->pos);
            ser.serialize_member("norm",mesh->norm);
            ser.serialize_member("texcoord",mesh->texcoord);
            ser.serialize_member("triangle",mesh->triangle);
            ser.serialize_member("quad",mesh->quad);
        }
        else if(is<FaceMesh>(node)) {
            auto mesh = cast<FaceMesh>(node);
            ser.serialize_member("pos",mesh->pos);
            ser.serialize_member("norm",mesh->norm);
            ser.serialize_member("texcoord",mesh->texcoord);
            ser.serialize_member("vertex",mesh->vertex);
            ser.serialize_member("triangle",mesh->triangle);
            ser.serialize_member("quad",mesh->quad);
        }
        else if(is<CatmullClarkSubdiv>(node)) {
            auto subdiv = cast<CatmullClarkSubdiv>(node);
            ser.serialize_member("pos",subdiv->pos);
            ser.serialize_member("norm",subdiv->norm);
            ser.serialize_member("texcoord",subdiv->texcoord);
            ser.serialize_member("quad",subdiv->quad);
            ser.serialize_member("level",subdiv->level);
            ser.serialize_member("smooth",subdiv->smooth);
        }
        else if(is<Subdiv>(node)) {
            auto subdiv = cast<Subdiv>(node);
            ser.serialize_member("pos",subdiv->pos);
            ser.serialize_member("norm",subdiv->norm);
            ser.serialize_member("texcoord",subdiv->texcoord);
            ser.serialize_member("triangle",subdiv->triangle);
            ser.serialize_member("quad",subdiv->quad);
            ser.serialize_member("crease_edge",subdiv->crease_edge);
            ser.serialize_member("crease_vertex",subdiv->crease_vertex);
            ser.serialize_member("level",subdiv->level);
            ser.serialize_member("smooth",subdiv->smooth);
        }
        else if(is<TesselationOverride>(node)) {
            auto override = cast<TesselationOverride>(node);
            ser.serialize_member("shape",override->shape);
            ser.serialize_member("level",override->level);
            ser.serialize_member("smooth",override->smooth);
        }
        else if(is<Spline>(node)) {
            auto spline = cast<Spline>(node);
            ser.serialize_member("pos",spline->pos);
            ser.serialize_member("radius",spline->radius);
            ser.serialize_member("cubic",spline->cubic);
            ser.serialize_member("continous",spline->continous);
            ser.serialize_member("level",spline->level);
            ser.serialize_member("smooth",spline->smooth);
        }
        else if(is<Patch>(node)) {
            auto patch = cast<Patch>(node);
            ser.serialize_member("pos",patch->pos);
            ser.serialize_member("texcoord",patch->texcoord);
            ser.serialize_member("cubic",patch->cubic);
            ser.serialize_member("continous_stride",patch->continous_stride);
            ser.serialize_member("level",patch->level);
            ser.serialize_member("smooth",patch->smooth);
        }
        else if(is<DisplacedShape>(node)) {
            auto displaced = cast<DisplacedShape>(node);
            ser.serialize_member("shape",displaced->shape);
            ser.serialize_member("displacement",displaced->displacement);
            ser.serialize_member("height",displaced->height);
            ser.serialize_member("level",displaced->level);
            ser.serialize_member("smooth",displaced->smooth);
        }
        else if(is<Sphere>(node)) {
            auto sphere = cast<Sphere>(node);
            ser.serialize_member("center",sphere->center);
            ser.serialize_member("radius",sphere->radius);
        }
        else if(is<Cylinder>(node)) {
            auto cylinder = cast<Cylinder>(node);
            ser.serialize_member("radius",cylinder->radius);
            ser.serialize_member("height",cylinder->height);
        }
        else if(is<Quad>(node)) {
            auto quad = cast<Quad>(node);
            ser.serialize_member("width",quad->width);
            ser.serialize_member("height",quad->height);
        }
        else if(is<Triangle>(node)) {
            auto triangle = cast<Triangle>(node);
            ser.serialize_member("v0",triangle->v0);
            ser.serialize_member("v1",triangle->v1);
            ser.serialize_member("v2",triangle->v2);
        }
        else NOT_IMPLEMENTED_ERROR();
    }
    else if(is<Material>(node)) {
        auto material = cast<Material>(node);
        ser.serialize_member("normal_texture",material->normal_texture);
        if(not material) ERROR("node is null");
        else if(is<Lambert>(node)) {
            auto lambert = cast<Lambert>(node);
            ser.serialize_member("diffuse",lambert->diffuse);
            ser.serialize_member("diffuse_texture",lambert->diffuse_texture);
        }
        else if(is<Phong>(node)) {
            auto phong = cast<Phong>(node);
            ser.serialize_member("diffuse",phong->diffuse);
            ser.serialize_member("specular",phong->specular);
            ser.serialize_member("reflection",phong->reflection);
            ser.serialize_member("blur_size",phong->blur_size);
            ser.serialize_member("exponent",phong->exponent);
            ser.serialize_member("diffuse_texture",phong->diffuse_texture);
            ser.serialize_member("specular_texture",phong->specular_texture);
            ser.serialize_member("reflection_texture",phong->reflection_texture);
            ser.serialize_member("exponent_texture",phong->exponent_texture);
            ser.serialize_member("use_reflected",phong->use_reflected);
        }
        else if(is<LambertEmission>(node)) {
            auto lambert = cast<LambertEmission>(node);
            ser.serialize_member("diffuse",lambert->diffuse);
            ser.serialize_member("diffuse_texture",lambert->diffuse_texture);
            ser.serialize_member("emission",lambert->emission);
            ser.serialize_member("emission_texture",lambert->emission_texture);
        }
        else NOT_IMPLEMENTED_ERROR();
    }
    else if(is<Camera>(node)) {
        auto camera = cast<Camera>(node);
        ser.serialize_member("frame",camera->frame);
        ser.serialize_member("view_dist",camera->view_dist);
        ser.serialize_member("image_width",camera->image_width);
        ser.serialize_member("image_height",camera->image_height);
        ser.serialize_member("image_dist",camera->image_dist);
        ser.serialize_member("focus_dist",camera->focus_dist);
        ser.serialize_member("focus_aperture",camera->focus_aperture);
        ser.serialize_member("orthographic",camera->orthographic);
        ser.serialize_member("shutter",camera->shutter);
    }
    else if(is<Light>(node)) {
        auto light = cast<Light>(node);
        ser.serialize_member("frame",light->frame);
        if(is<PointLight>(node)) {
            auto points = cast<PointLight>(node);
            ser.serialize_member("intensity",points->intensity);
        }
        else if(is<DirectionalLight>(node)) {
            auto directional = cast<DirectionalLight>(node);
            ser.serialize_member("intensity",directional->intensity);
        }
        else if(is<AreaLight>(node)) {
            auto area = cast<AreaLight>(node);
            ser.serialize_member("intensity",area->intensity);
            ser.serialize_member("shape",area->shape);
            ser.serialize_member("shadow_samples",area->shadow_samples);
        }
        else if(is<EnvLight>(node)) {
            auto env = cast<EnvLight>(node);
            ser.serialize_member("intensity",env->intensity);
            ser.serialize_member("envmap",env->envmap);
            ser.serialize_member("shadow_samples",env->shadow_samples);
            ser.serialize_member("hemisphere",env->hemisphere);
        }
        else NOT_IMPLEMENTED_ERROR();
    }
    else if(is<LightGroup>(node)) {
        auto lights = cast<LightGroup>(node);
        ser.serialize_member("lights",lights->lights);
    }
    else if(is<Primitive>(node)) {
        auto prim = cast<Primitive>(node);
        ser.serialize_member("frame",prim->frame);
        ser.serialize_member("material",prim->material);
        if(not prim) ERROR("node is null");
        else if(is<Surface>(node)) {
            auto surface = cast<Surface>(node);
            ser.serialize_member("shape",surface->shape);
        }
        else if(is<TransformedSurface>(node)) {
            auto transformed = cast<TransformedSurface>(node);
            ser.serialize_member("shape",transformed->shape);
            ser.serialize_member("pivot",transformed->pivot);
            ser.serialize_member("translation",transformed->translation);
            ser.serialize_member("scale",transformed->scale);
            ser.serialize_member("rotation_euler",transformed->rotation_euler);
            ser.serialize_member("anim_translation",transformed->anim_translation);
            ser.serialize_member("anim_scale",transformed->anim_scale);
            ser.serialize_member("anim_rotation_euler",transformed->anim_rotation_euler);
        }
        else NOT_IMPLEMENTED_ERROR();
    }
    else if(is<PrimitiveGroup>(node)) {
        auto group = cast<PrimitiveGroup>(node);
        ser.serialize_member("prims",group->prims);
        ser.serialize_member("intersect_accelerator_use",group->intersect_accelerator_use);
    }
    else if(is<Texture>(node)) {
        auto texture = cast<Texture>(node);
        ser.serialize_member("filename",texture->filename);
        ser.serialize_member("flipy",texture->flipy);
        if(ser.is_reading()) texture->image = imageio_read_auto3f(texture->filename,texture->flipy);
        else if(ser.is_writing_externals()) {
            if(texture->image.width() > 0 and texture->image.height() > 0) {
                imageio_write_auto(texture->filename,texture->image,texture->flipy);
            }
        }
    }
    else if(is<Gizmo>(node)) {
        auto gizmo = cast<Gizmo>(node);
        if(not gizmo) ERROR("node is null");
        else if(is<Grid>(node)) {
            auto grid = cast<Grid>(node);
            ser.serialize_member("frame",grid->frame);
            ser.serialize_member("color",grid->color);
            ser.serialize_member("steps",grid->steps);
            ser.serialize_member("size",grid->size);
            ser.serialize_member("thickness",grid->thickness);
        }
        else if(is<Axes>(node)) {
            auto axes = cast<Axes>(node);
            ser.serialize_member("frame",axes->frame);
            ser.serialize_member("color_x",axes->color_x);
            ser.serialize_member("color_y",axes->color_y);
            ser.serialize_member("color_z",axes->color_z);
            ser.serialize_member("size",axes->size);
            ser.serialize_member("thickness",axes->thickness);
        }
        else if(is<Line>(node)) {
            auto line = cast<Line>(node);
            ser.serialize_member("pos0",line->pos0);
            ser.serialize_member("pos1",line->pos1);
            ser.serialize_member("color",line->color);
            ser.serialize_member("thickness",line->thickness);
        }
        else if(is<Dot>(node)) {
            auto dot = cast<Dot>(node);
            ser.serialize_member("pos",dot->pos);
            ser.serialize_member("color",dot->color);
            ser.serialize_member("thickness",dot->thickness);
        }
        else NOT_IMPLEMENTED_ERROR();
    }
    else if(is<GizmoGroup>(node)) {
        auto gizmogroup = cast<GizmoGroup>(node);
        ser.serialize_member("gizmos",gizmogroup->gizmos);
    }
    else if(is<Scene>(node)) {
        auto scene = cast<Scene>(node);
        ser.serialize_member("camera",scene->camera);
        ser.serialize_member("lights",scene->lights);
        ser.serialize_member("prims",scene->prims);
        ser.serialize_member("draw_opts",scene->draw_opts);
        ser.serialize_member("raytrace_opts",scene->raytrace_opts);
        ser.serialize_member("distribution_opts",scene->distribution_opts);
        ser.serialize_member("pathtrace_opts",scene->pathtrace_opts);
    }
    else if(is<DrawOptions>(node)) {
        auto opts = cast<DrawOptions>(node);
        ser.serialize_member("res", opts->res);
        ser.serialize_member("samples", opts->samples);
        ser.serialize_member("doublesided", opts->doublesided);
        ser.serialize_member("time", opts->time);
        ser.serialize_member("background", opts->background);
        ser.serialize_member("ambient", opts->ambient);
        ser.serialize_member("cameralights", opts->cameralights);
        ser.serialize_member("cameralights_dir", opts->cameralights_dir);
        ser.serialize_member("cameralights_col", opts->cameralights_col);
        ser.serialize_member("faces", opts->faces);
        ser.serialize_member("edges", opts->edges);
        ser.serialize_member("lines", opts->lines);
        ser.serialize_member("control", opts->control);
        ser.serialize_member("control_no_depth", opts->control_no_depth);
        ser.serialize_member("gizmos", opts->gizmos);
    }
    else if(is<RaytraceOptions>(node)) {
        auto opts = cast<RaytraceOptions>(node);
        ser.serialize_member("res", opts->res);
        ser.serialize_member("samples", opts->samples);
        ser.serialize_member("doublesided", opts->doublesided);
        ser.serialize_member("time", opts->time);
        ser.serialize_member("background", opts->background);
        ser.serialize_member("ambient", opts->ambient);
        ser.serialize_member("cameralights", opts->cameralights);
        ser.serialize_member("cameralights_dir", opts->cameralights_dir);
        ser.serialize_member("cameralights_col", opts->cameralights_col);
        ser.serialize_member("max_depth", opts->max_depth);
        ser.serialize_member("shadows", opts->shadows);
        ser.serialize_member("reflections", opts->reflections);
    }
    else if(is<DistributionRaytraceOptions>(node)) {
        auto opts = cast<DistributionRaytraceOptions>(node);
        ser.serialize_member("res", opts->res);
        ser.serialize_member("samples", opts->samples);
        ser.serialize_member("doublesided", opts->doublesided);
        ser.serialize_member("time", opts->time);
        ser.serialize_member("background", opts->background);
        ser.serialize_member("ambient", opts->ambient);
        ser.serialize_member("cameralights", opts->cameralights);
        ser.serialize_member("cameralights_dir", opts->cameralights_dir);
        ser.serialize_member("cameralights_col", opts->cameralights_col);
        ser.serialize_member("max_depth", opts->max_depth);
        ser.serialize_member("shadows", opts->shadows);
        ser.serialize_member("reflections", opts->reflections);
        ser.serialize_member("samples_ambient", opts->samples_ambient);
        ser.serialize_member("samples_reflect", opts->samples_reflect);
    }
    else if(is<PathtraceOptions>(node)) {
        auto opts = cast<PathtraceOptions>(node);
        ser.serialize_member("res", opts->res);
        ser.serialize_member("samples", opts->samples);
        ser.serialize_member("doublesided", opts->doublesided);
        ser.serialize_member("time", opts->time);
        ser.serialize_member("background", opts->background);
        ser.serialize_member("ambient", opts->ambient);
        ser.serialize_member("cameralights", opts->cameralights);
        ser.serialize_member("cameralights_dir", opts->cameralights_dir);
        ser.serialize_member("cameralights_col", opts->cameralights_col);
        ser.serialize_member("max_depth", opts->max_depth);
        ser.serialize_member("shadows", opts->shadows);
        ser.serialize_member("indirect", opts->indirect);
        ser.serialize_member("reflections", opts->reflections);
        ser.serialize_member("shadow_samples", opts->shadow_samples);
        ser.serialize_member("indirect_samples", opts->indirect_samples);
        ser.serialize_member("image_scale", opts->image_scale);
        ser.serialize_member("image_gamma", opts->image_gamma);
    }
    else NOT_IMPLEMENTED_ERROR();
}
