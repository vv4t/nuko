#include "r_local.h"

// Rectangle vertices template for HUD creation.
static const vertex_t particle_triangle[] = {
  { .pos = { -0.5f, -0.5f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = {  0.0f, +0.5f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.5f, 0.0f } },
  { .pos = { +0.5f, -0.5f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
};

static vertex_t particle_vertices[3 * MAX_PARTICLES];

static void particle_init_mesh(vertex_t *vertices, vec3_t pos, quat_t rot)
{
  for (int i = 0; i < 3; i++) { 
    vertices[i].pos = vec3_add(
      vec3_rotate(
        vec3_mulf(
          particle_triangle[i].pos,
          0.3f
        ),
        rot
      ),
      pos
    );
    
    vertices[i].uv = particle_triangle[i].uv;
  }
}

bool r_particle_init()
{
  if (!r_new_mesh(&r.particle_mesh, MAX_PARTICLES * 3 * sizeof(vertex_t))) {
    log_printf(LOG_ERROR, "r_particle_init(): failed to allocate particle mesh");
    return false;
  }
  
  r.num_particles = 0;
  
  return true;
}

void r_particle_frame()
{
  r.num_particles = 0;
}

static int random_particle_value(int *seed)
{
  *seed = (*seed * 64 + *seed * ~*seed) & 255;
  return *seed;
}

static vec3_t random_particle_dir(int *seed)
{
  int deg_theta = random_particle_value(seed);
  float rad_theta = (float) (deg_theta / 256.0f) * 360.0 * M_PI / 180.0f;
  
  float y_vel = 1.0f + ((float) random_particle_value(seed) / 256.0f) * 3.0f;
  
  return vec3_init(
    cos(rad_theta) * 2.0,
    y_vel,
    sin(rad_theta) * 2.0
  );
}

static vec3_t random_particle_rot_axis(int *seed)
{
  int x_dir = random_particle_value(seed);
  int y_dir = random_particle_value(seed);
  int z_dir = random_particle_value(seed);
  
  return vec3_normalize(vec3_init(x_dir, y_dir, z_dir));
}

void r_particle_emit(vec3_t pos, float interp, int seed)
{
  for (int i = 0; i < 10; i++) {
    int particle_seed = i * (1 + seed);
    
    float random_offset = (float) random_particle_value(&particle_seed) / 256.0f;
    float particle_interp = random_offset * 0.1f + interp;
    
    vec3_t dir = random_particle_dir(&particle_seed);
    
    vec3_t rot_axis = random_particle_rot_axis(&particle_seed);
    quat_t rot = quat_init_rotation(rot_axis, 4.0f * particle_interp);
    
    vec3_t projectile_pos = vec3_mulf(dir, particle_interp);
    vec3_t gravity_pos = vec3_init(0.0f, -4.0f * particle_interp * particle_interp, 0.0f);
    
    vec3_t local_pos = vec3_add(projectile_pos, gravity_pos);
    vec3_t world_pos = vec3_add(pos, local_pos);
    
    particle_init_mesh(&particle_vertices[3 * r.num_particles], world_pos, rot);
    r.num_particles++;
  }
}

void r_particle_draw()
{
  glDisable(GL_CULL_FACE); // Particles are flat faces
  
  if (r.num_particles > 0) {
    glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, r.view_projection_matrix.m); // Vertices are manually offset, therefore no need for model matrix
    
    glUniform1i(r.light_shader.ul_glow, 1); // Enable glow for bullets
    r_sub_mesh(&r.particle_mesh, particle_vertices, 0, r.num_particles * 3);
    r_draw_sub_mesh(&r.particle_mesh, 0, 3 * r.num_particles);
    glUniform1i(r.light_shader.ul_glow, 0); // Disable glow
  }
  
  glEnable(GL_CULL_FACE);
}
