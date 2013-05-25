#include "draw.h"

#include "gls.h"
#include "gl_utils.h"

///@file igl/draw.cpp Interactive Drawing. @ingroup igl

void draw_camera(Camera* camera) {
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_MODELVIEW);
    glsLoadMatrix(camera_viewmatrix(camera));
    glPopAttrib();
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glsLoadMatrix(camera_projectionmatrix(camera));
    glPopAttrib();
}

void draw_light(Light* light, int idx) {
    glPushMatrix();
    glsMultMatrix(frame_to_matrix(light->frame));
    if(is<PointLight>(light)) {
        auto point = cast<PointLight>(light);
        vec4f _pos = vec4f(0,0,0,1);
        vec4f _intensity = vec4f(point->intensity.x,point->intensity.y,point->intensity.z,1);
        glEnable(GL_LIGHT0 + idx);
        glLightfv(GL_LIGHT0 + idx, GL_AMBIENT, &zero4f.x);
        glLightfv(GL_LIGHT0 + idx, GL_DIFFUSE, &_intensity.x);
        glLightfv(GL_LIGHT0 + idx, GL_SPECULAR, &_intensity.x);
        glLightfv(GL_LIGHT0 + idx, GL_POSITION, &_pos.x);
        glLightf(GL_LIGHT0 + idx, GL_CONSTANT_ATTENUATION, 0);
        glLightf(GL_LIGHT0 + idx, GL_LINEAR_ATTENUATION, 0);
        glLightf(GL_LIGHT0 + idx, GL_QUADRATIC_ATTENUATION, 1);
    }
    else if(is<DirectionalLight>(light)) {
        auto directional = cast<DirectionalLight>(light);
        vec4f _dir = vec4f(0,0,-1,0);
        vec4f _intensity = vec4f(directional->intensity.x,directional->intensity.y,directional->intensity.z,1);
        glEnable(GL_LIGHT0 + idx);
        glLightfv(GL_LIGHT0 + idx, GL_AMBIENT, &zero4f.x);
        glLightfv(GL_LIGHT0 + idx, GL_DIFFUSE, &_intensity.x);
        glLightfv(GL_LIGHT0 + idx, GL_SPECULAR, &_intensity.x);
        glLightfv(GL_LIGHT0 + idx, GL_POSITION, &_dir.x);
        glLightf(GL_LIGHT0 + idx, GL_CONSTANT_ATTENUATION, 1);
        glLightf(GL_LIGHT0 + idx, GL_LINEAR_ATTENUATION, 0);
        glLightf(GL_LIGHT0 + idx, GL_QUADRATIC_ATTENUATION, 0);
    }
    else if(is<AreaLight>(light)) {
        auto area = cast<AreaLight>(light);
        vec4f _pos = vec4f(0,0,0,1);
        vec4f _intensity = vec4f(area->intensity.x,area->intensity.y,area->intensity.z,1);
        glEnable(GL_LIGHT0 + idx);
        glLightfv(GL_LIGHT0 + idx, GL_AMBIENT, &zero4f.x);
        glLightfv(GL_LIGHT0 + idx, GL_DIFFUSE, &_intensity.x);
        glLightfv(GL_LIGHT0 + idx, GL_SPECULAR, &_intensity.x);
        glLightfv(GL_LIGHT0 + idx, GL_POSITION, &_pos.x);
        glLightf(GL_LIGHT0 + idx, GL_CONSTANT_ATTENUATION, 0);
        glLightf(GL_LIGHT0 + idx, GL_LINEAR_ATTENUATION, 0);
        glLightf(GL_LIGHT0 + idx, GL_QUADRATIC_ATTENUATION, 1);
    }
    else if(is<EnvLight>(light)) {
        // skip
    }
    else NOT_IMPLEMENTED_ERROR();
    glPopMatrix();
}

void draw_lights(LightGroup* lights, const vec3f& ambient, bool doublesided) {
    glPushAttrib(GL_LIGHTING_BIT);
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
    //glLightModelf(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, (doublesided) ? 1 : 0);
    glutils_set_ambient_light(vec4f(ambient.x,ambient.y,ambient.z,1));
    for(auto i : range(lights->lights.size())) draw_light(lights->lights[i],i);
}

void draw_light_decorations(Light* light) {
    glPushMatrix();
    glsMultMatrix(frame_to_matrix(light->frame));
    if(is<PointLight>(light)) {
        glBegin(GL_POINTS);
        glsVertex(zero3f);
        glEnd();
    }
    else if(is<DirectionalLight>(light)) {
        glBegin(GL_LINES);
        glsVertex(zero3f);
        glsVertex(z3f);
        glEnd();
    }
    else if(is<AreaLight>(light)) {
        draw_shape_decorations(cast<AreaLight>(light)->shape, true, false, false);
    }
    else if(is<EnvLight>(light)) {
    }
    else NOT_IMPLEMENTED_ERROR();
    glPopMatrix();
}

void draw_lights_decorations(LightGroup* lights) {
    for(auto l : lights->lights) draw_light_decorations(l);
}

// TODO: fixme
void draw_material(Material* mat) {
    if(is<Lambert>(mat)) {
        auto lambert = cast<Lambert>(mat);
        glsColor(lambert->diffuse);
        glutils_set_material(zero3f, lambert->diffuse, lambert->diffuse / pif, zero3f, 0);
    }
    else if(is<Phong>(mat)) {
        auto phong = cast<Phong>(mat);
        glsColor(phong->diffuse);
        glutils_set_material(zero3f, phong->diffuse, phong->diffuse / pif, phong->specular * (phong->exponent+8) / (8*pif), phong->exponent);
    }
    else if(is<LambertEmission>(mat)) {
        auto lambertemission = cast<LambertEmission>(mat);
        glsColor(lambertemission->diffuse + lambertemission->emission);
        glutils_set_material(lambertemission->emission, lambertemission->diffuse, lambertemission->diffuse / pif, zero3f, 0 );
    }
    else NOT_IMPLEMENTED_ERROR();
}

void draw_shape(Shape* shape) {
    if(shape->_tesselation) return draw_shape(shape->_tesselation);
        
    if(is<PointSet>(shape)) {
        auto points = cast<PointSet>(shape);
        if(points->approximate) {
            glPointSize(points->approximate_radius);
            //glEnable(GL_POINT_SPRITE);
            glBegin(GL_POINTS);
            for(auto i : range(points->pos.size())) {
                if(not points->texcoord.empty()) glsTexCoord(points->texcoord[i]); else glsTexCoord(zero2f);
                glsNormal(z3f);
                glsVertex(points->pos[i]);
            }
            glEnd();
            //glDisable(GL_POINT_SPRITE);
        } else {
            for(int i = 0; i < points->pos.size(); i ++) {
                if(not points->texcoord.empty()) glutils_draw_sphere(points->pos[i],points->radius[i],points->texcoord[i],4,4);
                else glutils_draw_sphere(points->pos[i],points->radius[i],zero2f,4,4);
            }
        }
    }
    else if(is<LineSet>(shape)) {
        auto lines = cast<LineSet>(shape);
        if(lines->approximate) {
            glLineWidth(lines->approximate_radius);
            vec2f lineuv[2] = { {0,0}, {1,0} };
            glBegin(GL_LINES);
            for(auto l : lines->line) {
                int uvcount = 0;
                for(auto vid : l) {
                    if(not lines->texcoord.empty()) glsTexCoord(lines->texcoord[vid]); else glsTexCoord(lineuv[uvcount++]);
                    glsNormal(x3f);
                    glsVertex(lines->pos[vid]);
                }
            }
            glEnd();
        } else {
            for(auto lid : range(lines->line.size())) {
                auto l = lines->line[lid];
                auto f = lineset_cylinder_frame(lines, lid);
                if(not lines->texcoord.empty()) glutils_draw_cylinder(f,(lines->radius[l.x]+lines->radius[l.y])/2,length(lines->pos[l.y]-lines->pos[l.x]),lines->texcoord[l.x],lines->texcoord[l.y],4,4);
                else glutils_draw_cylinder(f,(lines->radius[l.x]+lines->radius[l.y])/2,length(lines->pos[l.y]-lines->pos[l.x]),zero2f,x2f,4,4);
            }            
        }
    }
    else if(is<TriangleMesh>(shape)) {
        auto mesh = cast<TriangleMesh>(shape);
        if(mesh->norm.empty() or mesh->texcoord.empty()) {
            vec2f triangleuv[3] = { {0,0}, {1,0}, {0,1} };
            glBegin(GL_TRIANGLES);
            for(auto f : mesh->triangle) {
                if(mesh->norm.empty()) glsNormal(triangle_normal(mesh->pos[f.x],mesh->pos[f.y],mesh->pos[f.z]));
                int uvcount = 0;
                for(auto vid : f) {
                    if(not mesh->texcoord.empty()) glsTexCoord(mesh->texcoord[vid]); else glsTexCoord(triangleuv[uvcount++]);
                    if(not mesh->norm.empty()) glsNormal(mesh->norm[vid]);
                    glsVertex(mesh->pos[vid]);
                }
            }
            glEnd();
        } else {
            glutils_draw_faces(mesh->triangle, mesh->pos, mesh->norm, mesh->texcoord);
        }
    }
    else if(is<Mesh>(shape)) {
        auto mesh = cast<Mesh>(shape);
        if(mesh->norm.empty() or mesh->texcoord.empty()) {
            vec2f triangleuv[3] = { {0,0}, {1,0}, {0,1} };
            vec2f quaduv[4] = { {0,0}, {1,0}, {1,1}, {0,1} };
            glBegin(GL_TRIANGLES);
            for(auto f : mesh->triangle) {
                if(mesh->norm.empty()) glsNormal(triangle_normal(mesh->pos[f.x],mesh->pos[f.y],mesh->pos[f.z]));
                int uvcount = 0;
                for(auto vid : f) {
                    if(not mesh->texcoord.empty()) glsTexCoord(mesh->texcoord[vid]); else glsTexCoord(triangleuv[uvcount++]);
                    if(not mesh->norm.empty()) glsNormal(mesh->norm[vid]);
                    glsVertex(mesh->pos[vid]);
                }
            }
            glEnd();
            glBegin(GL_QUADS);
            for(auto f : mesh->quad) {
                if(mesh->norm.empty()) glsNormal(quad_normal(mesh->pos[f.x],mesh->pos[f.y],mesh->pos[f.z],mesh->pos[f.w]));
                int uvcount = 0;
                for(auto vid : f) {
                    if(not mesh->texcoord.empty()) glsTexCoord(mesh->texcoord[vid]); else glsTexCoord(quaduv[uvcount++]);
                    if(not mesh->norm.empty()) glsNormal(mesh->norm[vid]);
                    glsVertex(mesh->pos[vid]);
                }
            }
            glEnd();
        } else {
            if(not mesh->triangle.empty()) glutils_draw_faces(mesh->triangle, mesh->pos, mesh->norm, mesh->texcoord);
            if(not mesh->quad.empty()) glutils_draw_faces(mesh->quad, mesh->pos, mesh->norm, mesh->texcoord);
        }
    }
    else if(is<FaceMesh>(shape)) {
        auto mesh = cast<FaceMesh>(shape);
        vec2f triangleuv[3] = { {0,0}, {1,0}, {0,1} };
        vec2f quaduv[4] = { {0,0}, {1,0}, {1,1}, {0,1} };
        glBegin(GL_TRIANGLES);
        for(auto f : mesh->triangle) {
            if(mesh->norm.empty()) glsNormal(triangle_normal(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x]));
            int uvcount = 0;
            for(auto vid : f) {
                auto v = mesh->vertex[vid];
                if(not mesh->texcoord.empty()) glsTexCoord(mesh->texcoord[v.z]); else glsTexCoord(triangleuv[uvcount++]);
                if(not mesh->norm.empty()) glsNormal(mesh->norm[v.y]);
                glsVertex(mesh->pos[v.x]);
            }
        }
        glEnd();
        glBegin(GL_QUADS);
        for(auto f : mesh->quad) {
            if(mesh->norm.empty()) glsNormal(quad_normal(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x],mesh->pos[mesh->vertex[f.w].x]));
            int uvcount = 0;
            for(auto vid : f) {
                auto v = mesh->vertex[vid];
                if(not mesh->texcoord.empty()) glsTexCoord(mesh->texcoord[v.z]); else glsTexCoord(quaduv[uvcount++]);
                if(not mesh->norm.empty()) glsNormal(mesh->norm[v.y]);
                glsVertex(mesh->pos[v.x]);
            }
        }
        glEnd();
    }
    else if(is<Sphere>(shape)) glutils_draw_sphere(cast<Sphere>(shape)->center,cast<Sphere>(shape)->radius);
    else if(is<Cylinder>(shape)) glutils_draw_cylinder(cast<Cylinder>(shape)->radius,cast<Cylinder>(shape)->height);
    else if(is<Quad>(shape)) glutils_draw_quad(zero3f,x3f,y3f,cast<Quad>(shape)->width,cast<Quad>(shape)->height);
    else if(is<Triangle>(shape)) glutils_draw_triangle(cast<Triangle>(shape)->v0,cast<Triangle>(shape)->v1,cast<Triangle>(shape)->v2);
    else NOT_IMPLEMENTED_ERROR();
}

void draw_shape_decorations(Shape* shape, bool edges, bool lines, bool control) {
    if(shape->_tesselation) {
        draw_shape_decorations(shape->_tesselation, edges, lines, false);
        edges = false; lines = false;
    }
    
    if(is<TriangleMesh>(shape)) {
        auto mesh = cast<TriangleMesh>(shape);
        if(edges) {
            for(auto f : mesh->triangle) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(mesh->pos[vid]);
                glEnd();
            }
        }
        if(lines) {
            glBegin(GL_LINES);
            for(auto l : mesh->_tesselation_lines) {
                glsVertex(mesh->pos[l.x]);
                glsVertex(mesh->pos[l.y]);
            }
            glEnd();
        }
    }
    else if(is<Mesh>(shape)) {
        auto mesh = cast<Mesh>(shape);
        if(edges) {
            for(auto f : mesh->triangle) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(mesh->pos[vid]);
                glEnd();
            }
            for(auto f : mesh->quad) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(mesh->pos[vid]);
                glEnd();
            }
        }
        if(lines) {
            glBegin(GL_LINES);
            for(auto l : mesh->_tesselation_lines) {
                glsVertex(mesh->pos[l.x]);
                glsVertex(mesh->pos[l.y]);
            }
            glEnd();
        }
    }
    else if(is<FaceMesh>(shape)) {
        auto mesh = cast<FaceMesh>(shape);
        if(edges) {
            for(auto f : mesh->triangle) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(mesh->pos[mesh->vertex[vid].x]);
                glEnd();
            }
            for(auto f : mesh->quad) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(mesh->pos[mesh->vertex[vid].x]);
                glEnd();
            }
        }
        if(lines) {
            glBegin(GL_LINES);
            for(auto l : mesh->_tesselation_lines) {
                glsVertex(mesh->pos[mesh->vertex[l.x].x]);
                glsVertex(mesh->pos[mesh->vertex[l.y].x]);
            }
            glEnd();
        }
    }
    else if(is<CatmullClarkSubdiv>(shape)) {
        auto subdiv = cast<CatmullClarkSubdiv>(shape);
        if(control) {
            for(auto f : subdiv->quad) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(subdiv->pos[vid]);
                glEnd();
            }
            glBegin(GL_POINTS);
            for(auto p : subdiv->pos) glsVertex(p);
            glEnd();
        }
    }
    else if(is<Subdiv>(shape)) {
        auto subdiv = cast<Subdiv>(shape);
        if(control) {
            for(auto f : subdiv->quad) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(subdiv->pos[vid]);
                glEnd();
            }
            for(auto f : subdiv->triangle) {
                glBegin(GL_LINE_LOOP);
                for(auto vid : f) glsVertex(subdiv->pos[vid]);
                glEnd();
            }
            glBegin(GL_POINTS);
            for(auto p : subdiv->pos) glsVertex(p);
            glEnd();
        }
    }
    else if(is<Spline>(shape)) {
        auto spline = cast<Spline>(shape);
        if(control) {
            for(auto s : spline->cubic) {
                glBegin(GL_LINE_STRIP);
                for(auto vid : s) {
                    glsVertex(spline->pos[vid]);
                }
                glEnd();
            }
            glBegin(GL_POINTS);
            for(auto p : spline->pos) glsVertex(p);
            glEnd();
        }
    }
    else if(is<Patch>(shape)) {
        auto patch = cast<Patch>(shape);
        if(control) {
            for(auto p : patch->cubic) {
                for(auto i : range(4)) {
                    glBegin(GL_LINE_STRIP);
                    for(auto j : range(4)) glsVertex(patch->pos[p[i][j]]);
                    glEnd();
                    glBegin(GL_LINE_STRIP);
                    for(auto j : range(4)) glsVertex(patch->pos[p[j][i]]);
                    glEnd();
                }
            }
            glBegin(GL_POINTS);
            for(auto p : patch->pos) glsVertex(p);
            glEnd();
        }
    }
    else if(is<Sphere>(shape)) { if(lines) glutils_draw_sphere_lines(cast<Sphere>(shape)->center,cast<Sphere>(shape)->radius); }
    else if(is<Cylinder>(shape)) { if(lines) glutils_draw_cylinder_lines(cast<Cylinder>(shape)->radius,cast<Cylinder>(shape)->height); }
    else if(is<Quad>(shape)) { if(lines) glutils_draw_quad_lines(zero3f,x3f,y3f,cast<Quad>(shape)->width,cast<Quad>(shape)->height); }
    else if(is<Triangle>(shape)) { if(lines) glutils_draw_triangle_lines(cast<Triangle>(shape)->v0,cast<Triangle>(shape)->v1,cast<Triangle>(shape)->v2); }
    else { }
}

void draw_primitive(Primitive* prim, float time) {
    glPushMatrix();
    glPushAttrib(GL_TEXTURE_BIT);
    glsMultMatrix(frame_to_matrix(prim->frame));
    draw_material(prim->material);
    if(is<Surface>(prim)) draw_shape(cast<Surface>(prim)->shape);
    else if(is<TransformedSurface>(prim)) {
        auto transformed = cast<TransformedSurface>(prim);
        glsMultMatrix(transformed_matrix(transformed, time));
        draw_shape(transformed->shape);
    }
    else NOT_IMPLEMENTED_ERROR();
    glPopAttrib();
    glPopMatrix();
}

void draw_primitives(PrimitiveGroup* group, float time) {
    for(auto p : group->prims) draw_primitive(p,time);
}

void draw_primitive_decorations(Primitive* prim, float time,
                                bool edges, bool lines, bool control,
                                float colorscale) {
    glPushMatrix();
    glsMultMatrix(frame_to_matrix(prim->frame));
    if(colorscale >= 0) glsColor(material_display_color(prim->material)*colorscale);
    if(is<Surface>(prim)) draw_shape_decorations(cast<Surface>(prim)->shape,edges,lines,control);
    else if(is<TransformedSurface>(prim)) {
        glsMultMatrix(transformed_matrix(cast<TransformedSurface>(prim), time));
        draw_shape_decorations(cast<TransformedSurface>(prim)->shape,edges,lines,control);
    }
    else NOT_IMPLEMENTED_ERROR();
    glPopMatrix();
}

void draw_primitives_decorations(PrimitiveGroup* group, float time,
                                 bool edges, bool lines, bool control,
                                 float colorscale) {
    for(auto p : group->prims) draw_primitive_decorations(p,time,edges,lines,control,colorscale);
}

void draw_gizmo(Gizmo* gizmo) {
    if(not gizmo) return;
    else if(is<Grid>(gizmo)) {
        auto grid = cast<Grid>(gizmo);
        glsColor(grid->color);
        glLineWidth(grid->thickness);
        for(int i = -grid->steps/2; i <= grid->steps/2; i ++) {
            float t = i / float(grid->steps/2);
            glutils_draw_line(grid->frame.o+(grid->frame.x*t-grid->frame.y)*(grid->size/2),
                               grid->frame.o+(grid->frame.x*t+grid->frame.y)*(grid->size/2));
            glutils_draw_line(grid->frame.o+(grid->frame.y*t-grid->frame.x)*(grid->size/2),
                               grid->frame.o+(grid->frame.y*t+grid->frame.x)*(grid->size/2));
        }
    }
    else if(is<Axes>(gizmo)) {
        auto axes = cast<Axes>(gizmo);
        glLineWidth(axes->thickness);
        glsColor(axes->color_x);
        glutils_draw_line(axes->frame.o,axes->frame.o+axes->frame.x*axes->size);
        glsColor(axes->color_y);
        glutils_draw_line(axes->frame.o,axes->frame.o+axes->frame.y*axes->size);
        glsColor(axes->color_z);
        glutils_draw_line(axes->frame.o,axes->frame.o+axes->frame.z*axes->size);
    }
    else if(is<Line>(gizmo)) {
        auto line = cast<Line>(gizmo);
        glsColor(line->color);
        glLineWidth(line->thickness);
        glutils_draw_line(line->pos0,line->pos1);
    }
    else if(is<Dot>(gizmo)) {
        auto axes = cast<Dot>(gizmo);
        glsColor(axes->color);
        glPointSize(axes->thickness);
        glutils_draw_point(axes->pos);
    }
    else NOT_IMPLEMENTED_ERROR();
}

void draw_gizmos(GizmoGroup* group) {
    for(auto g : group->gizmos) draw_gizmo(g);
}

void draw_scene(Scene* scene,
                const DrawOptions& opts, bool clear) {
    int w = camera_image_width(scene->camera,opts.res);
    int h = camera_image_height(scene->camera,opts.res);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    if(opts.doublesided) glDisable(GL_CULL_FACE);
    else glEnable(GL_CULL_FACE);
    
    glViewport(0, 0, w, h);
    
    if(clear) {
        glsClearColor(opts.background);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    draw_camera(scene->camera);
    
    draw_lights((opts.cameralights) ? scene->_cameralights : scene->lights,opts.ambient,opts.doublesided);
    
    if(opts.faces) draw_primitives(scene->prims,opts.time);

    // pop lighting attribs
    glPopAttrib();
    glPopAttrib();
}

void draw_scene_decorations(Scene* scene,
                            const DrawOptions& opts, bool clear) {
    int w = camera_image_width(scene->camera,opts.res);
    int h = camera_image_height(scene->camera,opts.res);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    if(opts.doublesided) glDisable(GL_CULL_FACE);
    else glEnable(GL_CULL_FACE);
    
    glViewport(0, 0, w, h);
    
    if(clear) {
        glsClearColor(opts.background);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    draw_camera(scene->camera);
    
    if(opts.gizmos) {
        if(scene->_defaultgizmos) draw_gizmos(scene->_defaultgizmos);
        if(scene->gizmos) draw_gizmos(scene->gizmos);
        
        glLineWidth(4);
        glPointSize(4);
        glsColor(vec3f(1,1,0));
        draw_lights_decorations(scene->lights);
    }

    if(opts.edges) {
        glLineWidth(1);
        glDepthRange(0, 0.9999);
        glsColor(one3f * 0.25f);
        draw_primitives_decorations(scene->prims,opts.time,true,false,false,0.25f);
        glDepthRange(0, 1);
    }
    if(opts.lines) {
        glLineWidth(1);
        glDepthRange(0, 0.9999);
        glsColor(one3f * 0.05f);
        draw_primitives_decorations(scene->prims,opts.time,false,true,false,-1);
        glDepthRange(0, 1);
    }
    if(opts.control) {
        if(opts.control_no_depth) {
            glPushAttrib(GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
        }
        glLineWidth(1); glPointSize(2);
        glsColor(one3f * 0.05f);
        draw_primitives_decorations(scene->prims,opts.time,false,false,true,-1);
        if(opts.control_no_depth) glPopAttrib();
    }
}

