
struct VertexInput
{
    @location(0) position : vec2<f32>,
    @location(1) color : vec4<f32>
};

struct VertexOutput
{
    @builtin(position) clip_position : vec4<f32>,
    @location(1) color : vec4<f32>
};

struct FragmentOutput
{
    @location(0) frag_color : vec4<f32>
};

struct PerFrameData
{
    viewProj : mat4x4<f32>
};

@group(0)
@binding(0)
var<uniform> per_frame_data : PerFrameData;

@vertex
fn vs_main(in : VertexInput) -> VertexOutput 
{
    var out: VertexOutput;
    out.color = in.color;
    out.clip_position = per_frame_data.viewProj * vec4<f32>(in.position, 0.0, 1.0);

    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> FragmentOutput {
    var out: FragmentOutput;    
    out.frag_color = in.color;

    return out;
}

