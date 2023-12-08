
struct VertexInput
{
    @builtin(vertex_index) vertex_id : u32
};

struct VertexOutput
{
    @builtin(position) clip_position : vec4<f32>,
    @location(0) uv : vec2<f32>
};

struct FragmentOutput
{
    @location(0) frag_color : vec4<f32>
};

struct GridData
{
    canvas_size : vec2<f32>,
    checker_size : vec2<f32>,
    color : vec2<f32>,
    padding : vec2<f32>
};

const positions = array(
    vec2<f32>(1.0, -1.0),
    vec2<f32>(1.0, 1.0),
    vec2<f32>(-1.0, 1.0),
    vec2<f32>(-1.0, -1.0)
);

const indices = array(
    0, 1, 2, 0, 2, 3
);

const tex_coords = array(
    vec2<f32>(1.0, 1.0),
    vec2<f32>(1.0, 0.0),
    vec2<f32>(0.0, 0.0),
    vec2<f32>(0.0, 1.0)
);

@group(0)
@binding(0)
var<uniform> grid_data : GridData;

fn fmod(a : f32, b : f32) -> f32
{
    return a - floor(a / b) * b;
}

fn checker(uv : vec2<f32>, repeats : vec2<f32>) -> f32 
{
    var cx = floor(repeats.x * uv.x);
    var cy = floor(repeats.y * uv.y); 
    var result = fmod(cx + cy, 2.0);

    return sign(result);
}

@vertex
fn vs_main(in : VertexInput) -> VertexOutput 
{
    var idx = indices[in.vertex_id];

    var out: VertexOutput;
    out.uv = tex_coords[idx];
    out.clip_position = vec4<f32>(positions[idx], 0.0, 1.0);

    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> FragmentOutput {
    var uv = in.uv;
    uv.x *= grid_data.canvas_size.x / grid_data.canvas_size.y;
    var c = mix(grid_data.color.x, grid_data.color.y, 
        checker(uv, grid_data.checker_size));

    var out: FragmentOutput;    
    out.frag_color = vec4<f32>(c, c, c, 1.0);

    return out;
}
