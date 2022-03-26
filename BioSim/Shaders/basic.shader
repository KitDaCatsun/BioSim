#version 330 core

#shader vertex
layout(location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#shader fragment
out vec4 FragColor;

void main() {
    FragColor = vec4(.87f, .38f, .19f, 1.0f);
}