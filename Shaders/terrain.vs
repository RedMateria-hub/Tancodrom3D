#version 330

layout (location = 0) in vec3 Position;

uniform mat4 ViewProj;

out vec4 Color;

vec4 GetColor(float elevation)
{
   vec4 color;

   if (elevation < -10.0f)
   {
      // Sandy beach color.
      color = vec4(1.0f, 0.96f, 0.62f, 1.0f);
   }
   else if (elevation < 50.0f)
   {
      // Light yellow-green.
      color = vec4(0.48f, 0.77f, 0.46f, 1.0f);
   }
   else if (elevation < 120.0f)
   {
      // Dark yellow-green.
      color = vec4(0.1f, 0.48f, 0.19f, 1.0f);
   }
   else if (elevation < 200.0f)
   {
      // Dark brown.
      color = vec4(0.45f, 0.39f, 0.34f, 1.0f);
   }
   else
   {
      // White snow.
      color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
   }

   return color;
}

void main()
{
    gl_Position = ViewProj * vec4(Position, 1.0);

    Color = vec4(Position.y / 257.0);
    //Color = GetColor(Position.y/ 2.0);
}