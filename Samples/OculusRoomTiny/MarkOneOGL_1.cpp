#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h> 
#pragma comment(lib, "Ws2_32.lib")

#include <GL/glew.h>
//#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_EXPOSE_NATIVE_WGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <OVR.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// Use to load .obj in the future
// #include <assimp/Importer.hpp>

// Use to load .DDS
// #include <SOIL.h>

GLFWwindow* window;

ovrHmd hmd;
ovrHmdDesc hmd_desc;
ovrFovPort eye_fov[2];
ovrGLConfig ovr_gl_config;
ovrEyeRenderDesc eye_render_desc[2];

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

//vrpn_Tracker_Remote* vrpn_tracker;
float oculus_pos[3];
float scale = 1.0f;

static void windowSizeCallback(GLFWwindow* p_Window, int p_Width, int p_Height)
{
    ovr_gl_config.OGL.Header.RTSize.w = p_Width; 
    ovr_gl_config.OGL.Header.RTSize.h = p_Height;

    int distortion_caps 
        = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

    ovrHmd_ConfigureRendering(hmd, 
            &ovr_gl_config.Config, 
            distortion_caps, 
            eye_fov, 
            eye_render_desc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}


static void setOpenGLState(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

}

void initOvr(void) 
{
    ovr_Initialize();
    hmd = ovrHmd_Create(0);
    if (!hmd) 
        hmd = ovrHmd_CreateDebug(ovrHmd_DK1);
}

glm::mat4 fromOVRMatrix4f(const OVR::Matrix4f &in) 
{
    glm::mat4 out;
    memcpy(glm::value_ptr(out), &in, sizeof(in));
    return out;
}

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj) {
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }
    return true;
}

bool check_program_link_status(GLuint obj) {
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;   
    }
    return true;
}

//Remember to shutdown((socket), SD_SEND) then closesocket((socket))
SOCKET connect_to_odroid() {
  //char* hostname = "165.123.192.149"; 
  //char* hostname = "50.191.183.184"; 
  char* hostname = "lakitu.crabdance.com";
  //char* hostname = "165.123.192.149";

  WSADATA wsaData;
  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
      printf("WSAStartup failed: %d\n", iResult);
      return 1;
  }

  struct addrinfo *result = NULL
	              , *ptr = NULL
				        , hints
						    ;

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  #define DEFAULT_PORT "50007"

  // Resolve the server address and port
  iResult = getaddrinfo(hostname, DEFAULT_PORT, &hints, &result);
  if (iResult != 0) {
      printf("getaddrinfo failed: %d\n", iResult);
      WSACleanup();
      return 1;
  }

  SOCKET ConnectSocket = INVALID_SOCKET;
  // Attempt to connect to the first address returned by
  // the call to getaddrinfo
  ptr=result;
  // Create a SOCKET for connecting to server
  ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
  if (ConnectSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup(); 
    return 1;
  }
 
  // Connect to server.
  iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
      closesocket(ConnectSocket);
      ConnectSocket = INVALID_SOCKET;
  } 
  freeaddrinfo(result);
  if (ConnectSocket == INVALID_SOCKET) {
      printf("Unable to connect to server!\n");
      WSACleanup();
      return 1;
  }

  return ConnectSocket;
}

void broadcastPose(ovrPosef pose, SOCKET s) {
  float tempHeadPitch, tempHeadRoll, HeadYaw, GamepadYaw;
  OVR::Quatf orientation = OVR::Quatf(pose.Orientation);
  orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&HeadYaw, &tempHeadPitch, &tempHeadRoll);

  float pi = 3.1419;

  char message[2];
  //SYSTEMTIME st;
  //GetSystemTime(&st);
  //int mil = st.wMilliseconds;
  //char pos = (mil <= 500 ? mil : (500 - mil))/10;
  char normalizedPitch  = 100.0*(tempHeadPitch + (pi/2.0))/(pi);
  char normalizedYaw    = 100.0*(HeadYaw + pi)/(2.0*pi); 
  message[0] = static_cast<char>(normalizedPitch);
  message[1] = static_cast<char>(normalizedYaw);

  std::printf("message[0] = %d, message[1] = %d \n", message[0], message[1]);
  //can confirm that at this point, message has the correct values in it

  //Send some data
  if( send(s , message , 2 , 0) < 0)
  {
      puts("Send failed");
      return;
  }
  //puts("Data Send\n");
}

//It doesn't matter which eye you feed into this,
//since they both have the same orientation, 
//I'm pretty sure.
void printPose(ovrPosef pose) {
  float tempHeadPitch, tempHeadRoll, HeadYaw, GamepadYaw;
  OVR::Quatf orientation = OVR::Quatf(pose.Orientation);
  orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&HeadYaw, &tempHeadPitch, &tempHeadRoll);
  std::cout << "pitch = " << tempHeadPitch << "; roll = " << "; yaw = " << HeadYaw << "\n";
}

GLuint loadShadersHardcoded() {
    // shader source code
    std::string vertex_source =
        "#version 330\n"
        "layout(location = 0) in vec4 vposition;\n"
        "layout(location = 1) in vec2 vtexcoord;\n"
        "out vec2 ftexcoord;\n"
        "void main() {\n"
        "   ftexcoord = vtexcoord;\n"
        "   gl_Position = vposition;\n"
        "}\n";
        
    std::string fragment_source =
        "#version 330\n"
        "uniform sampler2D tex;\n" // texture uniform
        "in vec2 ftexcoord;\n"
        "layout(location = 0) out vec4 FragColor;\n"
        "void main() {\n"
        //"FragColor = texture(tex, ftexcoord);\n"
		      "FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n"
        "}\n";
   
    // program and shader handles
    GLuint shader_program, vertex_shader, fragment_shader;
    
    // we need these to properly pass the strings
    const char *source;
    int length;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length); 
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
 
    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);   
    glCompileShader(fragment_shader);
    if(!check_shader_compile_status(fragment_shader)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
        
    // create program
    shader_program = glCreateProgram();
    
    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    
    // link the program and check for errors
    glLinkProgram(shader_program);
    check_program_link_status(shader_program);

    return shader_program;
}

void eye_texture_init() {

}

void sensor_init()
{
}

//TODO: actually use this function argument
//int glfw_main(void (*use_pose)(ovrPosef))
int glfw_main()
{
    initOvr();

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize glfw!\n");
        exit(EXIT_FAILURE);
    }

    ovrSizei client_size;
    client_size.w = 640;
    client_size.h = 480;

    window = glfwCreateWindow(client_size.w, 
            client_size.h, 
            "GLFW Oculus Rift Test", 
            NULL, 
            NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum l_Result = glewInit();
    if (l_Result!=GLEW_OK) {
        printf("glewInit() error.\n");
        exit(EXIT_FAILURE);
    }

    setOpenGLState();

    /* 
     * Rendering to a framebuffer requires a bound texture, so we need
     * to get some of the parameters of the texture from OVR.
     */
    ovrSizei texture_size_left = ovrHmd_GetFovTextureSize(hmd, 
            ovrEye_Left, 
            hmd_desc.DefaultEyeFov[0], 
            1.0f);
    ovrSizei texture_size_right = ovrHmd_GetFovTextureSize(hmd, 
            ovrEye_Right, 
            hmd_desc.DefaultEyeFov[1], 
            1.0f);
    ovrSizei texture_size;
    texture_size.w = texture_size_left.w + texture_size_right.w;
    texture_size.h = (texture_size_left.h > texture_size_right.h \
            ? texture_size_left.h : texture_size_right.h);


    /*
     * Run of the mill framebuffer initialization
     */

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint color_texture_id;
    glGenTextures(1, &color_texture_id);
    glBindTexture(GL_TEXTURE_2D, color_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 
            0, 
            GL_RGBA, 
            texture_size.w, 
            texture_size.h, 
            0, 
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLuint depth_buffer_id;
    glGenRenderbuffers(1, &depth_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, 
            GL_DEPTH_COMPONENT, 
            texture_size.w, 
            texture_size.h);

    glFramebufferTexture(GL_FRAMEBUFFER, 
            GL_COLOR_ATTACHMENT0, 
            color_texture_id, 
            0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
            GL_DEPTH_ATTACHMENT, 
            GL_RENDERBUFFER, 
            depth_buffer_id);

    static const GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    //This is for debugging purposes.
    GLenum fb_UNDEFINED = GL_FRAMEBUFFER_UNDEFINED;
    GLenum fb_INCOMPLETE_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
    GLenum fb_INCOMPLETE_MISSING_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
    GLenum fb_INCOMPLETE_DRAW_BUFFER = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER;
    GLenum fb_INCOMPLETE_READ_BUFFER = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER;

    //I earlier had this triplet _before_ the error checking.
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLenum fbo_check = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (fbo_check != GL_FRAMEBUFFER_COMPLETE) {
        MessageBoxA(NULL,"There is a problem with the FBO.","", MB_OK);
        //printf("There is a problem with the FBO.\n");
        exit(EXIT_FAILURE);
    }


    /*
     * Some oculus configurations
     */

    eye_fov[0] = hmd_desc.DefaultEyeFov[0];
    eye_fov[1] = hmd_desc.DefaultEyeFov[1];

    ovr_gl_config.OGL.Header.API = ovrRenderAPI_OpenGL;
    ovr_gl_config.OGL.Header.Multisample = 0;
    ovr_gl_config.OGL.Header.RTSize.w = client_size.w;
    ovr_gl_config.OGL.Header.RTSize.h = client_size.h;

    int distortion_caps = ovrDistortionCap_Chromatic | 
        ovrDistortionCap_TimeWarp;

    ovrHmd_ConfigureRendering(hmd, 
            &ovr_gl_config.Config, 
            distortion_caps, 
            eye_fov, 
            eye_render_desc);

    ovrGLTexture eye_texture[2];
    eye_texture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
    eye_texture[0].OGL.Header.TextureSize.w = texture_size.w;
    eye_texture[0].OGL.Header.TextureSize.h = texture_size.h;
    eye_texture[0].OGL.Header.RenderViewport.Pos.x = 0;
    eye_texture[0].OGL.Header.RenderViewport.Pos.y = 0;
    eye_texture[0].OGL.Header.RenderViewport.Size.w = texture_size.w/2;
    eye_texture[0].OGL.Header.RenderViewport.Size.h = texture_size.h;
    eye_texture[0].OGL.TexId = color_texture_id;
    // Right eye the same, except for the x-position in the texture...
    eye_texture[1] = eye_texture[0];
    eye_texture[1].OGL.Header.RenderViewport.Pos.x = (texture_size.w+1)/2;


    /*
     * Compilation of shaders and initialization of buffers
     */

    GLuint program_id = loadShadersHardcoded();

    GLint texture_location = glGetUniformLocation(program_id, "tex");

    //I don't know what I should put here :( so . . . uhhhh . . . TODO
    //bool res = loadOBJ("../assets/suzanne.obj", vertices, uvs, normals);

    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
            vertices.size() * sizeof(glm::vec3),
            &vertices[0],
            GL_STATIC_DRAW);

    GLuint mvp_id = glGetUniformLocation(program_id, "MVP");

    glfwSetWindowSizeCallback(window, windowSizeCallback);

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && \
            !glfwWindowShouldClose(window))
    {

        ovrFrameTiming m_HmdFrameTiming = ovrHmd_BeginFrame(hmd, 0);

        glUseProgram(program_id);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //They changed the eye initialization and removal in July or something, apparently.
        static ovrPosef eye_pose[2];

        for (int eye_idx = 0; eye_idx < ovrEye_Count; eye_idx++)
        {
            ovrEyeType eye = hmd_desc.EyeRenderOrder[eye_idx];
            //ovrPosef eye_pose = ovrHmd_BeginEyeRender(hmd, eye);
            eye_pose[eye] = ovrHmd_GetEyePose(hmd, eye);
            if (eye == 0) {
                printPose(eye_pose[0]);
	              SOCKET socket = connect_to_odroid();
				        broadcastPose(eye_pose[0], socket);
                shutdown(socket, SD_SEND);
                closesocket(socket);
			      }

            glViewport(eye_texture[eye].OGL.Header.RenderViewport.Pos.x,
                    eye_texture[eye].OGL.Header.RenderViewport.Pos.y,
                    eye_texture[eye].OGL.Header.RenderViewport.Size.w,
                    eye_texture[eye].OGL.Header.RenderViewport.Size.h
                    );

            // Projection
            OVR::Matrix4f projection_matrix = 
                OVR::Matrix4f(
                        ovrMatrix4f_Projection(
                            eye_render_desc[eye].Fov, 
                            0.3f, 
                            100.0f, 
                            true)).Transposed();
            
            // View
            glm::vec3 view = glm::vec3(
                    eye_render_desc[eye].ViewAdjust.x,
                    eye_render_desc[eye].ViewAdjust.y,
                    eye_render_desc[eye].ViewAdjust.z - 2.0f);
            glm::mat4 view_matrix = glm::translate(glm::mat4(1.f), 
                    view);
            
            /*
             * Integrate the orientation and position of the oculus 
             * provided by the optitrack system, instead of orientation
             * results provided by the oculus.
             */


            // Model
            OVR::Quatf orientation = OVR::Quatf(eye_pose[eye].Orientation);
            OVR::Matrix4f model_matrix = 
                OVR::Matrix4f(orientation.Inverted()).Transposed();

            // MVP 
            glm::mat4 mvp = 
                fromOVRMatrix4f(projection_matrix) *
                view_matrix *
                fromOVRMatrix4f(model_matrix);


            glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);


            // Render...
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glDrawArrays(GL_LINES, 0, vertices.size());
            glDisableVertexAttribArray(0);

            //ovrHmd_EndEyeRender(hmd, eye, eye_pose, &eye_texture[eye].Texture);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ovrHmd_EndFrame(hmd, eye_pose, &eye_texture[0].Texture);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glfwPollEvents();
    }

    glDeleteBuffers(1, &vertex_buffer);
    glDeleteProgram(program_id);

    glfwDestroyWindow(window);
    glfwTerminate();

    ovrHmd_Destroy(hmd);
    ovr_Shutdown();

    exit(EXIT_SUCCESS);
}


void glfw_main_broadcasting() {
	glfw_main();
}
