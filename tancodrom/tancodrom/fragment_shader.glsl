#version 330 core
out vec4 FragColor;

uniform bool isNight;

void main() {
    if (isNight) {
        FragColor = vec4(0.1, 0.1, 0.3, 1.0); // Night color
    } else {
        FragColor = vec4(0.5, 0.8, 0.3, 1.0); // Day color
    }
}
