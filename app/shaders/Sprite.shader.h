#ifdef BRICABRAC_VERTEX_SHADER

BRICABRAC_UNIFORM(mat4, pmv)

BRICABRAC_ATTRIBUTE(vec2, position)
BRICABRAC_ATTRIBUTE(vec2, texcoord)

#ifndef BRICABRAC_HOSTED

varying vec2 v_texcoord;

void main() {
    v_texcoord = texcoord;
    gl_Position = pmv * vec4(position, 0, 1);
}

#endif // BRICABRAC_HOSTED
#endif // BRICABRAC_VERTEX_SHADER

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef BRICABRAC_FRAGMENT_SHADER

BRICABRAC_UNIFORM(sampler2D, texture)

#ifndef BRICABRAC_HOSTED

varying mediump vec2 v_texcoord;

void main() {
    gl_FragColor = texture2D(texture, v_texcoord);
}

#endif // BRICABRAC_HOSTED
#endif // BRICABRAC_FRAGMENT_SHADER
