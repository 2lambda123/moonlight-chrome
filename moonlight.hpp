#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/mouse_lock.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/video_decoder.h"

#include "ppapi/c/ppb_gamepad.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/graphics_3d_client.h"

#include "ppapi/utility/completion_callback_factory.h"

#include "nacl_io/nacl_io.h"

#include <Limelight.h>

struct Shader {
  Shader() : program(0), texcoord_scale_location(0) {}
  ~Shader() {}

  GLuint program;
  GLint texcoord_scale_location;
};

class MoonlightInstance : public pp::Instance, public pp::MouseLock, public pp::Graphics3DClient {
    public:
        MoonlightInstance(PP_Instance instance) :
            pp::Instance(instance),
            pp::MouseLock(this),
            pp::Graphics3DClient(this),
            m_CallbackFactory(this),
            m_MouseLocked(false) {            
            // This function MUST be used otherwise sockets don't work (nacl_io_init() doesn't work!)            
            nacl_io_init_ppapi(pp_instance(), pp::Module::Get()->get_browser_interface());
            
            m_GamepadApi = static_cast<const PPB_Gamepad*>(pp::Module::Get()->GetBrowserInterface(PPB_GAMEPAD_INTERFACE));
            m_GlesApi = static_cast<const PPB_OpenGLES2*>(pp::Module::Get()->GetBrowserInterface(PPB_OPENGLES2_INTERFACE));
        }
        
        virtual ~MoonlightInstance();
        
        bool Init(uint32_t argc, const char* argn[], const char* argv[]);
        
        void HandleMessage(const pp::Var& var_message);
        
        bool HandleInputEvent(const pp::InputEvent& event);
        
        void PollGamepads();
        
        void DidLockMouse(int32_t result);
        void MouseLockLost();
        
        void OnConnectionStopped(uint32_t unused);
        void OnConnectionStarted(uint32_t error);
        
        void DidChangeView(const pp::Rect& position,
                           const pp::Rect& clip_ignored);
        
        static void* ConnectionThreadFunc(void* context);
        
        static void ClStageStarting(int stage);
        static void ClStageFailed(int stage, long errorCode);
        static void ClConnectionStarted(void);
        static void ClConnectionTerminated(long errorCode);
        static void ClDisplayMessage(char* message);
        static void ClDisplayTransientMessage(char* message);
        
        
        virtual void Graphics3DContextLost() {}
        static Shader CreateProgram(const char* vertexShader, const char* fragmentShader);
        static void CreateShader(GLuint program, GLenum type, const char* source, int size);
        static void PaintPicture(PP_VideoPicture picture);
        
        void DispatchGetPicture(uint32_t unused);
        void PictureReady(int32_t result, PP_VideoPicture picture);
        
        static void VidDecSetup(int width, int height, int redrawRate, void* context, int drFlags);
        static void VidDecCleanup(void);
        static int VidDecSubmitDecodeUnit(PDECODE_UNIT decodeUnit);

    private:
        static CONNECTION_LISTENER_CALLBACKS s_ClCallbacks;
        static DECODER_RENDERER_CALLBACKS s_DrCallbacks;
    
        pp::Graphics3D* m_Graphics3D;
        pp::VideoDecoder* m_VideoDecoder;
        pp::Size m_ViewSize;
        const PPB_OpenGLES2* m_GlesApi;
        Shader m_Texture2DShader;
        Shader m_RectangleArbShader;
        Shader m_ExternalOesShader;
        
        double m_LastPadTimestamps[4];
        const PPB_Gamepad* m_GamepadApi;
        pp::CompletionCallbackFactory<MoonlightInstance> m_CallbackFactory;
        bool m_MouseLocked;
};

extern MoonlightInstance* g_Instance;