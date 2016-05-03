/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2005 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Brecht Van Lommel.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file GPU_material.h
 *  \ingroup gpu
 */

#ifndef __GPU_MATERIAL_H__
#define __GPU_MATERIAL_H__

#include "DNA_customdata_types.h" /* for CustomDataType */
#include "DNA_listBase.h"

#include "BLI_sys_types.h" /* for bool */

#ifdef __cplusplus
extern "C" {
#endif

struct Image;
struct ImageUser;
struct Material;
struct Object;
struct Image;
struct Scene;
struct SceneRenderLayer;
struct GPUVertexAttribs;
struct GPUNode;
struct GPUNodeLink;
struct GPUNodeStack;
struct GPUMaterial;
struct GPUTexture;
struct GPULamp;
struct GPUProbe;
struct PreviewImage;
struct World;

typedef struct GPUNode GPUNode;
typedef struct GPUNodeLink GPUNodeLink;
typedef struct GPUMaterial GPUMaterial;
typedef struct GPULamp GPULamp;
typedef struct GPUParticleInfo GPUParticleInfo;
typedef struct GPUProbe GPUProbe;
typedef struct GPUSSR GPUSSR;

/* Functions to create GPU Materials nodes */

typedef enum GPUType {
	/* The value indicates the number of elements in each type */
	GPU_NONE = 0,
	GPU_FLOAT = 1,
	GPU_VEC2 = 2,
	GPU_VEC3 = 3,
	GPU_VEC4 = 4,
	GPU_MAT3 = 9,
	GPU_MAT4 = 16,

	GPU_TEX2D = 1002,
	GPU_SHADOW2D = 1003,
	GPU_TEXCUBE = 1004,
	GPU_ATTRIB = 3001
} GPUType;

typedef enum GPUBuiltin {
	GPU_VIEW_MATRIX =           (1 << 0),
	GPU_OBJECT_MATRIX =         (1 << 1),
	GPU_INVERSE_VIEW_MATRIX =   (1 << 2),
	GPU_INVERSE_OBJECT_MATRIX = (1 << 3),
	GPU_VIEW_POSITION =         (1 << 4),
	GPU_VIEW_NORMAL =           (1 << 5),
	GPU_OBCOLOR =               (1 << 6),
	GPU_AUTO_BUMPSCALE =        (1 << 7),
	GPU_CAMERA_TEXCO_FACTORS =  (1 << 8),
	GPU_PARTICLE_SCALAR_PROPS = (1 << 9),
	GPU_PARTICLE_LOCATION =	    (1 << 10),
	GPU_PARTICLE_VELOCITY =     (1 << 11),
	GPU_PARTICLE_ANG_VELOCITY = (1 << 12),
	GPU_LOC_TO_VIEW_MATRIX =    (1 << 13),
	GPU_INVERSE_LOC_TO_VIEW_MATRIX = (1 << 14),
} GPUBuiltin;

typedef enum GPUPBRBuiltin {
	GPU_LOD_FACTOR =        (1 << 0),
	GPU_PROBE =             (1 << 1),
	GPU_PLANAR_REFLECT =    (1 << 2),
	GPU_PLANAR_REFRACT =    (1 << 3),
	GPU_SH0 =               (1 << 4),
	GPU_SH1 =               (1 << 5),
	GPU_SH2 =               (1 << 6),
	GPU_SH3 =               (1 << 7),
	GPU_SH4 =               (1 << 8),
	GPU_SH5 =               (1 << 9),
	GPU_SH6 =               (1 << 10),
	GPU_SH7 =               (1 << 11),
	GPU_SH8 =               (1 << 12),
	GPU_CORRECTION_MATRIX = (1 << 13),
	GPU_PLANAR_RFL_MATRIX = (1 << 14),
	GPU_PROBE_POSITION =    (1 << 15),
	GPU_PLANAR_VECTOR =     (1 << 16),
	GPU_SSR =               (1 << 17),
	GPU_SSR_PARAMETERS =    (1 << 18),
	GPU_SSR_PARAMETERS2 =   (1 << 19),
	GPU_PIXEL_PROJ_MATRIX = (1 << 20),
} GPUPBRBuiltin;

typedef enum GPUOpenGLBuiltin {
	GPU_MATCAP_NORMAL = 1,
	GPU_COLOR = 2,
} GPUOpenGLBuiltin;

typedef enum GPUMatType {
	GPU_MATERIAL_TYPE_MESH = 1,
	GPU_MATERIAL_TYPE_MESH_REAL_SH = 2,
	GPU_MATERIAL_TYPE_WORLD = 3,
	GPU_MATERIAL_TYPE_LAMP = 5,
} GPUMatType;


typedef enum GPUBlendMode {
	GPU_BLEND_SOLID = 0,
	GPU_BLEND_ADD = 1,
	GPU_BLEND_ALPHA = 2,
	GPU_BLEND_CLIP = 4,
	GPU_BLEND_ALPHA_SORT = 8,
	GPU_BLEND_ALPHA_TO_COVERAGE = 16
} GPUBlendMode;

typedef struct GPUNodeStack {
	GPUType type;
	const char *name;
	float vec[4];
	struct GPUNodeLink *link;
	bool hasinput;
	bool hasoutput;
	short sockettype;
} GPUNodeStack;


#define GPU_DYNAMIC_GROUP_FROM_TYPE(f) ((f) & 0xFFFF0000)

#define GPU_DYNAMIC_GROUP_MISC     0x00010000
#define GPU_DYNAMIC_GROUP_LAMP     0x00020000
#define GPU_DYNAMIC_GROUP_OBJECT   0x00030000
#define GPU_DYNAMIC_GROUP_SAMPLER  0x00040000
#define GPU_DYNAMIC_GROUP_MIST     0x00050000
#define GPU_DYNAMIC_GROUP_WORLD    0x00060000
#define GPU_DYNAMIC_GROUP_MAT      0x00070000

typedef enum GPUDynamicType {

	GPU_DYNAMIC_NONE                 = 0,

	GPU_DYNAMIC_OBJECT_VIEWMAT       = 1  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_MAT           = 2  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_VIEWIMAT      = 3  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_IMAT          = 4  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_COLOR         = 5  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_AUTOBUMPSCALE = 6  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_LOCTOVIEWMAT  = 7  | GPU_DYNAMIC_GROUP_OBJECT,
	GPU_DYNAMIC_OBJECT_LOCTOVIEWIMAT = 8  | GPU_DYNAMIC_GROUP_OBJECT,

	GPU_DYNAMIC_LAMP_DYNVEC          = 1  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DYNCO           = 2  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DYNIMAT         = 3  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DYNPERSMAT      = 4  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DYNENERGY       = 5  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DYNCOL          = 6  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DISTANCE        = 7  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_ATT1            = 8  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_ATT2            = 9  | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_SPOTSIZE        = 10 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_SPOTBLEND       = 11 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_SPOTSCALE       = 12 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_COEFFCONST      = 13 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_COEFFLIN        = 14 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_COEFFQUAD       = 15 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_DYNMAT          = 16 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_AREASCALE       = 17 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_SIZEX           = 18 | GPU_DYNAMIC_GROUP_LAMP,
	GPU_DYNAMIC_LAMP_SIZEY           = 19 | GPU_DYNAMIC_GROUP_LAMP,

	GPU_DYNAMIC_SAMPLER_2DBUFFER     = 1  | GPU_DYNAMIC_GROUP_SAMPLER,
	GPU_DYNAMIC_SAMPLER_2DIMAGE      = 2  | GPU_DYNAMIC_GROUP_SAMPLER,
	GPU_DYNAMIC_SAMPLER_2DSHADOW     = 3  | GPU_DYNAMIC_GROUP_SAMPLER,

	GPU_DYNAMIC_MIST_ENABLE          = 1  | GPU_DYNAMIC_GROUP_MIST,
	GPU_DYNAMIC_MIST_START           = 2  | GPU_DYNAMIC_GROUP_MIST,
	GPU_DYNAMIC_MIST_DISTANCE        = 3  | GPU_DYNAMIC_GROUP_MIST,
	GPU_DYNAMIC_MIST_INTENSITY       = 4  | GPU_DYNAMIC_GROUP_MIST,
	GPU_DYNAMIC_MIST_TYPE            = 5  | GPU_DYNAMIC_GROUP_MIST,
	GPU_DYNAMIC_MIST_COLOR           = 6  | GPU_DYNAMIC_GROUP_MIST,

	GPU_DYNAMIC_HORIZON_COLOR        = 1  | GPU_DYNAMIC_GROUP_WORLD,
	GPU_DYNAMIC_AMBIENT_COLOR        = 2  | GPU_DYNAMIC_GROUP_WORLD,
	GPU_DYNAMIC_ZENITH_COLOR         = 3  | GPU_DYNAMIC_GROUP_WORLD,

	GPU_DYNAMIC_MAT_DIFFRGB          = 1  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_REF              = 2  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_SPECRGB          = 3  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_SPEC             = 4  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_HARD             = 5  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_EMIT             = 6  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_AMB              = 7  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_ALPHA            = 8  | GPU_DYNAMIC_GROUP_MAT,
	GPU_DYNAMIC_MAT_MIR              = 9  | GPU_DYNAMIC_GROUP_MAT
} GPUDynamicType;

GPUNodeLink *GPU_attribute(CustomDataType type, const char *name);
GPUNodeLink *GPU_uniform(float *num);
GPUNodeLink *GPU_dynamic_uniform(float *num, GPUDynamicType dynamictype, void *data);
GPUNodeLink *GPU_image(struct Image *ima, struct ImageUser *iuser, bool is_data, bool is_envmap);
GPUNodeLink *GPU_cube_map(struct Image *ima, struct ImageUser *iuser, bool is_data);
GPUNodeLink *GPU_image_preview(struct PreviewImage *prv);
GPUNodeLink *GPU_texture(int size, float *pixels);
GPUNodeLink *GPU_dynamic_texture(struct GPUTexture *tex, GPUDynamicType dynamictype, void *data);
GPUNodeLink *GPU_builtin(GPUBuiltin builtin);
GPUNodeLink *GPU_pbr_builtin(GPUPBRBuiltin builtin);
GPUNodeLink *GPU_opengl_builtin(GPUOpenGLBuiltin builtin);
void GPU_node_link_set_type(GPUNodeLink *link, GPUType type);

bool GPU_link(GPUMaterial *mat, const char *name, ...);
bool GPU_stack_link(GPUMaterial *mat, const char *name, GPUNodeStack *in, GPUNodeStack *out, ...);

void GPU_material_set_lamp_normal_link(GPUMaterial *material, GPUNodeLink *link);
GPUNodeLink *GPU_material_get_lamp_normal_link(GPUMaterial *material);
void GPU_material_set_lamp_position_link(GPUMaterial *material, GPUNodeLink *link);
GPUNodeLink *GPU_material_get_lamp_position_link(GPUMaterial *material);
void GPU_material_set_lamp_incoming_link(GPUMaterial *material, GPUNodeLink *link);
GPUNodeLink *GPU_material_get_lamp_incoming_link(GPUMaterial *material);
void GPU_material_output_link(GPUMaterial *material, GPUNodeLink *link);
void GPU_material_empty_output_link(GPUMaterial *material);
GPUNodeLink *GPU_material_get_output_link(GPUMaterial *material);
void GPU_material_enable_alpha(GPUMaterial *material);
GPUNodeLink *GPU_get_world_horicol(void);
GPUBlendMode GPU_material_alpha_blend(GPUMaterial *material, float obcol[4]);

/* High level functions to create and use GPU materials */
GPUMaterial *GPU_material_world(struct Scene *scene, struct World *wo);

GPUMaterial *GPU_material_from_blender(struct Scene *scene, struct Material *ma, bool use_opensubdiv,
		bool use_realistic_preview, bool use_planar_probe, bool use_alpha_as_depth, bool use_ssr, int samplecount, int parallax_correc);
GPUMaterial *GPU_material_matcap(struct Scene *scene, struct Material *ma, bool use_opensubdiv);
void GPU_material_free(struct ListBase *gpumaterial);

void GPU_materials_free(void);

bool GPU_lamp_override_visible(GPULamp *lamp, struct SceneRenderLayer *srl, struct Material *ma);
void GPU_material_bind(
        GPUMaterial *material, int oblay, int viewlay, double time, int mipmap,
        float viewmat[4][4], float viewinv[4][4], float cameraborder[4], bool scenelock);
void GPU_material_bind_uniforms(
        GPUMaterial *material, float obmat[4][4], float viewmat[4][4], float obcol[4],
        float autobumpscale, GPUParticleInfo *pi);
void GPU_material_bind_uniforms_probe(GPUMaterial *material, GPUProbe *probe, GPUSSR *ssr,struct GPUSSRSettings *ssr_settings,struct GPUBRDFSettings *brdf_settings);
void GPU_material_unbind(GPUMaterial *material);
bool GPU_material_bound(GPUMaterial *material);
struct Scene *GPU_material_scene(GPUMaterial *material);
GPUMatType GPU_material_get_type(GPUMaterial *material);
void GPU_material_set_type(GPUMaterial *material, GPUMatType type);

void GPU_material_vertex_attributes(GPUMaterial *material,
	struct GPUVertexAttribs *attrib);

bool GPU_material_do_color_management(GPUMaterial *mat);
bool GPU_material_use_new_shading_nodes(GPUMaterial *mat);

/* BRDF Shading */

typedef enum GPUBrdfType {
	GPU_BRDF_DIFFUSE                  = 0,
	
	GPU_BRDF_GLOSSY_SHARP             = 1,
	GPU_BRDF_GLOSSY_GGX               = 2,
	GPU_BRDF_GLOSSY_BECKMANN          = 3,
	GPU_BRDF_GLOSSY_ASHIKHMIN_SHIRLEY = 4,

	GPU_BRDF_ANISO_SHARP              = 5,
	GPU_BRDF_ANISO_GGX                = 6,
	GPU_BRDF_ANISO_BECKMANN           = 7,
	GPU_BRDF_ANISO_ASHIKHMIN_SHIRLEY  = 8,

	GPU_BRDF_REFRACT_SHARP            = 9,
	GPU_BRDF_REFRACT_GGX              = 10,
	GPU_BRDF_REFRACT_BECKMANN         = 11,

	GPU_BRDF_GLASS_SHARP              = 12,
	GPU_BRDF_GLASS_GGX                = 13,
	GPU_BRDF_GLASS_BECKMANN           = 14,

	GPU_BRDF_VELVET                   = 15,
	GPU_BRDF_TRANSLUCENT              = 16,
	GPU_BRDF_TRANSPARENT              = 17,
	GPU_BRDF_TOON                     = 18,
} GPUBrdfType;

typedef struct GPUBrdfInput {
	GPUMaterial *mat;
	GPUBrdfType type;

	GPUNodeLink *normal, *color, *roughness, *ior;
	GPUNodeLink *sigma, *toon_size, *toon_smooth, *anisotropy;
	GPUNodeLink *aniso_rotation, *aniso_tangent;

	GPUNodeLink *output;
} GPUBrdfInput;

void GPU_brdf_input_initialize(GPUBrdfInput *brdf);
void GPU_shade_BRDF(GPUBrdfInput *brdf);
void GPU_material_set_brdf_link(GPUMaterial *material, GPUBrdfInput *brdf);
GPUBrdfInput *GPU_material_get_brdf_link(GPUMaterial *material);

/* Exported shading */

typedef struct GPUShadeInput {
	GPUMaterial *gpumat;
	struct Material *mat;

	GPUNodeLink *rgb, *specrgb, *vn, *view, *vcol, *ref;
	GPUNodeLink *alpha, *refl, *spec, *emit, *har, *amb;
	GPUNodeLink *spectra, *mir, *refcol;
} GPUShadeInput;

typedef struct GPUShadeResult {
	GPUNodeLink *diff, *spec, *combined, *alpha;
} GPUShadeResult;

void GPU_shadeinput_set(GPUMaterial *mat, struct Material *ma, GPUShadeInput *shi);
void GPU_shaderesult_set(GPUShadeInput *shi, GPUShadeResult *shr);

/* Export GLSL shader */

typedef enum GPUDataType {
	GPU_DATA_NONE = 0,
	GPU_DATA_1I = 1,   /* 1 integer */
	GPU_DATA_1F = 2,
	GPU_DATA_2F = 3,
	GPU_DATA_3F = 4,
	GPU_DATA_4F = 5,
	GPU_DATA_9F = 6,
	GPU_DATA_16F = 7,
	GPU_DATA_4UB = 8,
} GPUDataType;

/* this structure gives information of each uniform found in the shader */
typedef struct GPUInputUniform {
	struct GPUInputUniform *next, *prev;
	char varname[32];         /* name of uniform in shader */
	GPUDynamicType type;      /* type of uniform, data format and calculation derive from it */
	GPUDataType datatype;     /* type of uniform data */
	struct Object *lamp;      /* when type=GPU_DYNAMIC_LAMP_... or GPU_DYNAMIC_SAMPLER_2DSHADOW */
	struct Image *image;      /* when type=GPU_DYNAMIC_SAMPLER_2DIMAGE */
	int texnumber;            /* when type=GPU_DYNAMIC_SAMPLER, texture number: 0.. */
	unsigned char *texpixels; /* for internally generated texture, pixel data in RGBA format */
	int texsize;              /* size in pixel of the texture in texpixels buffer:
	                           * for 2D textures, this is S and T size (square texture) */
} GPUInputUniform;

typedef struct GPUInputAttribute {
	struct GPUInputAttribute *next, *prev;
	char varname[32];     /* name of attribute in shader */
	int type;             /* from CustomData.type, data type derives from it */
	GPUDataType datatype; /* type of attribute data */
	const char *name;     /* layer name */
	int number;           /* generic attribute number */
} GPUInputAttribute;

typedef struct GPUShaderExport {
	ListBase uniforms;
	ListBase attributes;
	char *vertex;
	char *fragment;
} GPUShaderExport;

GPUShaderExport *GPU_shader_export(struct Scene *scene, struct Material *ma);
void GPU_free_shader_export(GPUShaderExport *shader);

/* Lamps */

GPULamp *GPU_lamp_from_blender(struct Scene *scene, struct Object *ob, struct Object *par, bool use_realistic_preview);
void GPU_lamp_free(struct Object *ob);

bool GPU_lamp_has_shadow_buffer(GPULamp *lamp);
void GPU_lamp_update_buffer_mats(GPULamp *lamp);
void GPU_lamp_shadow_buffer_bind(GPULamp *lamp, float viewmat[4][4], int *winsize, float winmat[4][4]);
void GPU_lamp_shadow_buffer_unbind(GPULamp *lamp);
int GPU_lamp_shadow_buffer_type(GPULamp *lamp);
int GPU_lamp_shadow_bind_code(GPULamp *lamp);
float *GPU_lamp_dynpersmat(GPULamp *lamp);

void GPU_lamp_update(GPULamp *lamp, int lay, int hide, float obmat[4][4]);
void GPU_lamp_update_size(GPULamp *lamp, float sizex, float sizey);
void GPU_lamp_update_colors(GPULamp *lamp, float r, float g, float b, float energy);
void GPU_lamp_update_distance(GPULamp *lamp, float distance, float att1, float att2,
                              float coeff_const, float coeff_lin, float coeff_quad);
void GPU_lamp_update_spot(GPULamp *lamp, float spotsize, float spotblend);
int GPU_lamp_shadow_layer(GPULamp *lamp);
GPUNodeLink *GPU_lamp_get_data(
        GPUMaterial *mat, GPULamp *lamp,
        GPUNodeLink **r_col, GPUNodeLink **r_lv, GPUNodeLink **r_dist, GPUNodeLink **r_shadow, GPUNodeLink **r_energy);

/* Probes */

/* Keep this enum in sync with DNA_object_types.h */
typedef enum GPUProbeType {
	GPU_PROBE_CUBE		= 2,
	GPU_PROBE_PLANAR	= 3,
} GPUProbeType;

GPUProbe *GPU_probe_world(struct Scene *scene, struct World *wo);
GPUProbe *GPU_probe_object(struct Scene *scene, struct Object *ob);
void GPU_probe_free(ListBase *gpuprobe);

void GPU_probe_buffer_bind(GPUProbe *probe);
void GPU_probe_switch_fb_cubeface(GPUProbe *probe, int cubeface, float viewmat[4][4], int *winsize, float winmat[4][4]);
void GPU_probe_attach_planar_fb(GPUProbe *probe, float camviewmat[4][4], float camwinmat[4][4], float viewmat[4][4], int *winsize, bool refraction);
void GPU_probe_buffer_unbind(GPUProbe *probe);
void GPU_probe_rebuild_mipmaps(GPUProbe *probe);
void GPU_probe_sh_compute(GPUProbe *probe);
void GPU_probe_sh_shader_bind(GPUProbe *probe);
void GPU_probe_sh_shader_unbind(void);
void GPU_probe_auto_update(GPUProbe *probe);
void GPU_probe_set_update(GPUProbe *probe, bool val);
bool GPU_probe_get_update(GPUProbe *probe);
struct Object *GPU_probe_get_object(GPUProbe *probe);
int GPU_probe_get_size(GPUProbe *probe);
int GPU_probe_get_type(GPUProbe *probe);
void GPU_probe_update_clip(GPUProbe *probe, float clipsta, float clipend);
void GPU_probe_update_layers(GPUProbe *probe, unsigned int lay);
unsigned int GPU_probe_get_layers(GPUProbe *probe);
void GPU_probe_update_sh_res(GPUProbe *probe, int res);
void GPU_probe_update_parallax(GPUProbe *probe, float correctionmat[4][4], float obmat[4][4]);
void GPU_probe_update_ref_plane(GPUProbe *probe, float obmat[4][4]);

/* World */

void GPU_mist_update_enable(short enable);
void GPU_mist_update_values(int type, float start, float dist, float inten, float color[3]);
void GPU_horizon_update_color(float color[3]);
void GPU_ambient_update_color(float color[3]);

struct GPUParticleInfo
{
	float scalprops[4];
	float location[3];
	float velocity[3];
	float angular_velocity[3];
};

#ifdef WITH_OPENSUBDIV
struct DerivedMesh;
void GPU_material_update_fvar_offset(GPUMaterial *gpu_material,
                                     struct DerivedMesh *dm);
#endif

void GPU_pbr_settings_validate(struct GPUPBRSettings *pbr_settings);

#ifdef __cplusplus
}
#endif

#endif /*__GPU_MATERIAL_H__*/
