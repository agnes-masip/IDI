#version 330 core

out vec4 FragColor;
in vec3 fcolor;

uniform int black;






void main() {
    if(black == 1){
        FragColor = vec4(0,0,0,1);
    }else{
       FragColor = vec4(fcolor,1);
       
        if(FragColor == glm::vec4(1,0,0,1) && int(gl_FragCoord.y) % 10 <= 4) {
                discard;
        }
          
    }
}


