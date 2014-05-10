#ifndef SHADER_INC
#   define SHADER_INC

#define STRINGIFY(A) #A

const char* toonVS = STRINGIFY(
varying vec3 normal;            \n
void main()                     \n
{                               \n
    normal = gl_Normal;         \n
    gl_Position = ftransform(); \n
}                               \n
);

const char* toonFS = STRINGIFY(
uniform vec3 lightDir;          \n
varying vec3 normal;            \n
void main()                     \n
{                               \n
    float intensity = dot(normalize(lightDir), normalize(normal)); \n
    if ( intensity > 0.95 )     \n
        gl_FragColor = gl_FrontMaterial.diffuse; \n
    else if ( intensity > 0.5 ) \n
        gl_FragColor = gl_FrontMaterial.diffuse*0.9; \n
    else if ( intensity > 0.25 ) \n
        gl_FragColor = gl_FrontMaterial.diffuse*0.7; \n
    else                        \n
        gl_FragColor = gl_FrontMaterial.diffuse*0.4; \n
}                               \n
);

const char* gouraudVS = STRINGIFY(
void main()								\n
{									\n
    vec3 normal;							\n
    vec3 lightDir;							\n
    vec4 diffuse;							\n
    vec4 ambient;							\n
    vec4 globalAmbient;							\n
    vec4 specular;							\n
    vec3 refl;								\n
    vec3 eye; 								\n
    vec3 vertex;							\n
    float RdotE;							\n
    float NdotL;							\n
    int i;								\n
    gl_FrontColor = vec4(0.0,0.0,0.0,0.0);				\n
    for (i = 0; i<gl_MaxLights; i++) {					\n				
	//Aggregate diffuse, ambient, and specular light for each light source    
        vertex = vec3(normalize(gl_ModelViewMatrix * gl_Vertex)); 	\n
        eye = normalize(-vertex); 					\n
        specular = vec4(0.0,0.0,0.0,0.0);				\n
        normal = normalize(gl_NormalMatrix * gl_Normal);		\n
        lightDir = normalize(vec3(gl_LightSource[i].position));		\n
        NdotL = max(dot(normal, lightDir), 0.0);			\n
    
        diffuse = gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse;	\n
        diffuse = clamp(diffuse,0.0,1.0);					\n
        ambient = gl_FrontMaterial.diffuse * gl_LightSource[i].ambient;	\n
        ambient = clamp(ambient,0.0,1.0);					\n
        //Add specular highlights
        if (NdotL > 0.0) {							\n
            refl = normalize(reflect(-lightDir,normal));			\n
            RdotE = max(dot(refl, eye), 0.0);				\n
            specular += gl_FrontMaterial.specular * gl_LightSource[i].specular 
				* pow(RdotE,gl_FrontMaterial.shininess); \n
        }									\n
        gl_FrontColor += NdotL * diffuse + ambient + specular; \n      
   }									\n
    //Add global ambient light
    globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;     \n
    globalAmbient = clamp(globalAmbient,0.0,1.0);                 \n
    gl_FrontColor += globalAmbient;					\n

    gl_Position = ftransform();						\n
}									\n
);

const char* gouraudFS = STRINGIFY(
void main()			\n
{				\n
   //for each pixel, interpolate the color from the vertex colors in the VS
    gl_FragColor = gl_Color;	\n
}				\n
);				

const char* blinnPhongVS = STRINGIFY(
varying vec3 normal;							\n
varying vec3 lightDir[gl_MaxLights];							\n
varying vec3 halfVector[gl_MaxLights];						\n
void main()								\n
{
    int i;   \n
    normal = normalize(gl_NormalMatrix * gl_Normal);                      \n
    //Calculate the lightdirection and halfvector for each light source
    for (i = 0; i<gl_MaxLights; i++) { \n
       lightDir[i] = normalize(vec3(gl_LightSource[i].position));		\n
       halfVector[i] = normalize(gl_LightSource[i].halfVector.xyz);		\n
   }										\n
       gl_Position = ftransform(); 						\n
}									\n
);							

const char* blinnPhongFS = STRINGIFY(
varying vec3 normal;                                                    \n
varying vec3 lightDir[gl_MaxLights];                                                  \n
varying vec3 halfVector[gl_MaxLights];                                                \n
void main()							                                  	\n
{									                                    \n
    vec3 n;								                                \n
    vec3 halfV;								                            \n
    vec4 diffuse;                                                       \n
    vec4 ambient;                                                       \n
    float NdotL;							                            \n
    float NdotHV;							                            \n
    vec4 color = vec4(0.0,0.0,0.0,0.0);                                      \n
    int i;                                                              \n
    n = normalize(normal);                                               \n
    //calculate diffuse, ambient, and specular light per light source, and aggregate them
    for (i = 0; i < gl_MaxLights; i++) {                                \n
        diffuse = clamp((gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse),0.0,1.0);   \n
        ambient = clamp((gl_FrontMaterial.diffuse * gl_LightSource[i].ambient),0.0,1.0);   \n
        color += ambient; \n
        //normal and light direction are interpolated per pixel
        NdotL = max(dot(n,lightDir[i]),0.0);					\n
        if (NdotL > 0.0) {							\n
    	    color += diffuse * NdotL;					\n
    	    halfV = normalize(halfVector[i]);					\n
    	    NdotHV = max(dot(n,halfV),0.0);					\n
    	    color += gl_FrontMaterial.specular * gl_LightSource[i].specular * 
    	        pow(NdotHV, gl_FrontMaterial.shininess);			\n
       }                                                                \n
    }	
    //finally, add the global ambient light
    color += gl_LightModel.ambient * gl_FrontMaterial.ambient;								\n
    gl_FragColor = color;						\n
}									\n
);

const char* checkerBoardVS = STRINGIFY(
varying vec3 normal;                          \n
varying vec3 lightDir[gl_MaxLights];                            \n
varying vec3 halfVector[gl_MaxLights];                      \n
void main()                             \n
{
    int i;   \n
    normal = normalize(gl_NormalMatrix * gl_Normal);	\n
    //same as blinn-phong, except store opengl texture coordinates
    for (i = 0; i<gl_MaxLights; i++) { \n
        lightDir[i] = normalize(vec3(gl_LightSource[i].position));      \n
        halfVector[i] = normalize(gl_LightSource[i].halfVector.xyz);     \n
    } 									       \n
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();                        \n
 }                                   \n
);                        

const char* checkerBoardFS = STRINGIFY(
varying vec3 normal;                                                    \n
varying vec3 lightDir[gl_MaxLights];                                                  \n
varying vec3 halfVector[gl_MaxLights];                                                \n
void main()                                                             \n
{                                                                       \n
    vec3 n;                                                             \n
    vec3 halfV;                                                         \n
    vec4 diffuse;                                                       \n
    vec4 ambient;                                                       \n
    float NdotL;                                                        \n
    float NdotHV;                                                       \n
    vec4 color;                                     \n
    int i;                                                              \n

    n = normalize(normal);  \n
    //same as blinn-phong (aggregate light sources and interpolate normal/light directions
    for (i = 0; i < gl_MaxLights; i++) {                                \n
        diffuse = clamp((gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse),0.0,1.0);   \n
        ambient = clamp((gl_FrontMaterial.diffuse * gl_LightSource[i].ambient),0.0,1.0);   \n
        color += ambient;                                             \n
        NdotL = max(dot(n,lightDir[i]),0.0);                    \n
        if (NdotL > 0.0) {                          \n
            color += diffuse * NdotL;                   \n
            halfV = normalize(halfVector[i]);                   \n
            NdotHV = max(dot(n,halfV),0.0);                 \n
            color += gl_FrontMaterial.specular * gl_LightSource[i].specular * 
                pow(NdotHV, gl_FrontMaterial.shininess);            \n
       }                                                                \n
    }   								\n
    //add in global ambient light to each pixel's color
    color += gl_LightModel.ambient * gl_FrontMaterial.ambient;                              \n
    //augment the pixel color depending on position of texture coordinates
    if (gl_TexCoord[0].s <0.5 ^^ gl_TexCoord[0].t < 0.5)                 \n
         gl_FragColor = color * vec4(0.5,0.5,0.5,0.5);                                 \n
    else                                 \n
         gl_FragColor = color;                 \n

}                                   \n
);

const char* extraVS = STRINGIFY(
varying vec3 normal;            \n
void main()                     \n
{                               \n
    vec4 color;			\n
    //map xyz components of the normal to rgb values
    normal = normalize(gl_NormalMatrix * gl_Normal);                \n
    color=vec4(normal[0], normal[1], normal[2], 1.0);		    \n
    gl_FrontColor = color; \n

    gl_Position = ftransform(); \n
}                               \n
);

const char* extraFS = STRINGIFY(
uniform vec3 lightDir;          \n
varying vec3 normal;            \n
void main()                     \n
{                               \n
    //Reuses the toon shader code, except this time uses the color calculated
    //in the vertex shader
    float intensity = dot(normalize(lightDir), normalize(normal)); \n
    if ( intensity > 0.95 )     \n
        gl_FragColor = gl_Color; \n
    else if ( intensity > 0.5 ) \n
        gl_FragColor = gl_Color*0.9; \n
    else if ( intensity > 0.25 ) \n
        gl_FragColor = gl_Color*0.7; \n
    else                        \n
        gl_FragColor = gl_Color*0.4; \n
}                               \n
);



#endif
